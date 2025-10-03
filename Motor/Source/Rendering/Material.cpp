#include "Rendering/Material.h"
#include "Resource/Manager.hpp"
#include "Resource/Material.h"
#include "Resource/Texture.h"

#include <assimp/material.h>

using namespace LibMath;

using namespace KaputEngine;
using namespace KaputEngine::Rendering;

using KaputEngine::Resource::MaterialResource;
using KaputEngine::Resource::ResourceManager;
using KaputEngine::Resource::TextureResource;

using std::cerr;
using std::nullopt;

Material::Material(MaterialResource& parent) : m_resource(&parent) { }

struct MaterialKey
{
	const char* key = nullptr;
	unsigned int type;
	unsigned int idx;
};

template <typename T>
_NODISCARD _Success_(return) static bool getProperty(
	const aiMaterial& material, const aiTextureType texType, const MaterialKey& key, Sampler<T>& sampler, unsigned int& index)
{
	if (key.key)
		if (T value; material.Get(key.key, key.type, key.idx, value) == aiReturn_SUCCESS)
			sampler = value;

	//if (!material.GetTextureCount(texType))
	//	return true;

	//aiString path;

	//if (material.GetTexture(texType, 0, &path) != aiReturn_SUCCESS)
	//{
	//	cerr << __FUNCTION__": Failed to load Material texture.\n";
	//	return false;
	//}

	//std::shared_ptr<TextureResource> res =
	//	ResourceManager::get<TextureResource>(path.C_Str(), false);

	//if (!res)
	//{
	//	cerr << __FUNCTION__": Failed to load Material texture \"" << path.C_Str() << "\".\n";
	//	return false;
	//}

	//sampler.texture() = TextureSample
	//{
	//	.texture = res->dataPtr(),
	//	.unitIndex = index++
	//};

	return true;
}

_Success_(return) bool Material::init(aiMaterial&& material)
{
	unsigned int unitIndex = 0;

	m_normal = VertexAttribute(3);

	if (!getProperty(material, aiTextureType_BASE_COLOR, { AI_MATKEY_BASE_COLOR }, m_albedo, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material albedo.\n";
		return false;
	}
	if (!getProperty(material, aiTextureType_DIFFUSE, { AI_MATKEY_COLOR_DIFFUSE }, m_albedo, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material albedo.\n";
		return false;
	}
	if (!getProperty(material, aiTextureType_NORMALS, { }, m_normal, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material normal.\n";
		return false;
	}
	if (!getProperty(material, aiTextureType_METALNESS, { AI_MATKEY_METALLIC_FACTOR }, m_metallic, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material metallic.\n";
		return false;
	}
	if (!getProperty(material, aiTextureType_DIFFUSE_ROUGHNESS, { AI_MATKEY_ROUGHNESS_FACTOR }, m_roughness, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material roughness.\n";
		return false;
	}
	if (!getProperty(material, aiTextureType_AMBIENT_OCCLUSION, { }, m_ambientOcclusion, unitIndex))
	{
		cerr << __FUNCTION__": Failed to load material ambient occlusion.\n";
		return false;
	}

	return true;
}

Sampler<Color>& Material::albedo() noexcept
{
	return m_albedo;
}

const Sampler<Color>& Material::albedo() const noexcept
{
	return m_albedo;
}

void Material::setAlbedo(const Color& col)
{
	this->m_albedo.fallbackData() = col;
}

Sampler<Vector3f>& Material::normal() noexcept
{
	return m_normal;
}

const Sampler<Vector3f>& Material::normal() const noexcept
{
	return m_normal;
}

Sampler<float>& Material::metallic() noexcept
{
	return m_metallic;
}

const Sampler<float>& Material::metallic() const noexcept
{
	return m_metallic;
}

void Material::setMetallic(const float metal)
{
	this->m_metallic.fallbackData() = metal;
}

Sampler<float>& Material::roughness() noexcept
{
	return m_roughness;
}

const Sampler<float>& Material::roughness() const noexcept
{
	return m_roughness;
}

void Material::setRoughness(const float rough)
{
	this->m_roughness.fallbackData() = rough;
}

Sampler<float>& Material::ambientOcclusion() noexcept
{
	return m_ambientOcclusion;
}

const Sampler<float>& Material::ambientOcclusion() const noexcept
{
	return m_ambientOcclusion;
}

void Material::setOcclusion(const float occl)
{
	this->m_ambientOcclusion.fallbackData() = occl;
}

_Ret_maybenull_ MaterialResource* Material::parentResource() noexcept
{
	return m_resource;
}

_Ret_maybenull_ const MaterialResource* Material::parentResource() const noexcept
{
	return m_resource;
}
