#include "Rendering/Material.h"
#include "Resource/Manager.hpp"
#include "Resource/Mesh.h"
#include "Resource/Texture.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace KaputEngine::Text::Xml;

using KaputEngine::Rendering::Material;
using KaputEngine::Rendering::Mesh;
using KaputEngine::Resource::MeshResource;

using std::cerr;
using std::string;
using std::string_view;

RESOURCE_IMPL(MeshResource)

MeshResource::~MeshResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& MeshResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	static const char* Context = __FUNCTION__;

	return m_loadFuture = createFuture<void>(policy,
	[this, content = std::move(content)](eMultiThreadPolicy policy) -> void
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		};

		if (!deserializeForLoad(content))
		{
			cerr << Context << ": Failed to load Mesh resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(m_modelPath.string(),
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cerr << Context << ": " << importer.GetErrorString() << '\n';
			cerr << Context << ": Failed to load Model resource.\n";

			m_loadState = eLoadState::UNLOADED;
			return;
		}

		for (size_t i = 0; i < scene->mNumTextures; ++i)
		{
			aiTexture& tex = *scene->mTextures[i];
			const std::shared_ptr<TextureResource> res =
				Resource::create<TextureResource>(tex.mFilename.C_Str());

			res->load(std::move(tex), policy);
			ResourceManager::add(res);
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		m_materials.reserve(scene->mNumMaterials);

		for (size_t i = 0; i < scene->mNumMaterials; ++i)
		{
			Material& mat = m_materials.emplace_back();

			if (!mat.init(std::move(*scene->mMaterials[i])))
			{
				cerr << Context << ": Failed to load Model resource.\n";
				m_loadState = eLoadState::UNLOADED;
				return;
			}
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		m_meshes.reserve(scene->mNumMeshes);

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh& assimpMesh = *scene->mMeshes[i];
			Mesh& mesh = i == 0 ? m_root : m_meshes.emplace_back();

			const std::shared_ptr<Material> mat =
				assimpMesh.mMaterialIndex == -1 ? nullptr :
				std::shared_ptr<Material>{ shared_from_this(), &m_materials[assimpMesh.mMaterialIndex] };

			if (!mesh.init(assimpMesh, nullptr))
			{
				cerr << Context << ": Failed to load Model resource.\n";
				m_loadState = eLoadState::UNLOADED;
				return;
			}
		}

		m_loadState = eLoadState::LOADED;
	});
}

void MeshResource::unload()
{
	m_meshes.clear();
	m_root.destroy();
}

std::shared_ptr<const Mesh> MeshResource::root() const noexcept
{
	return { shared_from_this(), &m_root };
}

_Success_(return) bool MeshResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (mapParse("Source", map, m_modelPath) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Source.\n";
		return false;
	}

	return true;
}

void MeshResource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Source", m_modelPath);
}
