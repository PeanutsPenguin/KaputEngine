#include "Resource/ShaderProgram.h"

#include "Resource/Manager.hpp"
#include "Resource/Shader.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"
#include "Utils/Policy.h"

using namespace KaputEngine::Text::Xml;

using KaputEngine::Rendering::ShaderProgram;
using KaputEngine::Resource::ShaderProgramResource;

using std::cerr;
using std::string;
using std::string_view;

RESOURCE_IMPL(ShaderProgramResource)

ShaderProgramResource::~ShaderProgramResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& ShaderProgramResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	return m_loadFuture = createFuture<void>(policy,
	[this, content = std::move(content)](eMultiThreadPolicy) -> void
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (!deserializeForLoad(content))
		{
			cerr << __FUNCTION__": Failed to load ShaderProgram resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		int shaderIndex = 0;
		ShaderProgram::ShaderList shaders;

		shaders.reserve(m_shaderPaths.size());

		for (const std::filesystem::path& path : m_shaderPaths)
		{
			const std::shared_ptr<ShaderResource> shader = ResourceManager::get<ShaderResource>(path, false);
			shader->loadExisting(eMultiThreadPolicy::MONO_THREAD).wait();
			shaders.emplace_back(shader->dataPtr());
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		m_data.create(std::move(shaders));

		if (m_stopSource.stop_requested())
			m_data.destroy();

		m_loadState = eLoadState::LOADED;
	});
}

void ShaderProgramResource::unload()
{
	if (!startUnload())
		return;

	m_data.destroy();
	m_loadState = eLoadState::UNLOADED;
}

const std::vector<std::filesystem::path>& ShaderProgramResource::shaderPaths() const noexcept
{
	return m_shaderPaths;
}

const ShaderProgram& ShaderProgramResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<const ShaderProgram> ShaderProgramResource::dataPtr() const noexcept
{
	return { shared_from_this(), &m_data };
}

std::shared_ptr<const ShaderProgramResource> ShaderProgramResource::defaultProgram()
{
	return ResourceManager::get<ShaderProgramResource>("Kaput/Shader/pbr/pbr.kasset", true);
}

void ShaderProgramResource::serializeValues(XmlSerializeContext& context) const
{
	context.startObject("Shaders");

	for (const std::filesystem::path& path : m_shaderPaths)
		context.value("Shader", path);

	context.endObject();
}

_Success_(return) bool ShaderProgramResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	const auto shadersIt = map.find("Shaders");

	if (shadersIt == map.end())
	{
		cerr << __FUNCTION__": Failed to deserialize ShaderProgram.\n";
		return false;
	}

	const std::vector<XmlNode>* shaders = std::get_if<std::vector<XmlNode>>(&shadersIt->second->body);

	if (!shaders)
	{
		cerr << __FUNCTION__": Expected array in node Shaders.\n";
		return false;
	}

	for (const XmlNode& node : *shaders)
	{
		const std::optional<std::filesystem::path> pathOp = node.parse<std::filesystem::path>();

		if (!pathOp)
		{
			cerr << __FUNCTION__": Failed to deserialize shader path.\n";
			return false;
		}

		m_shaderPaths.emplace_back(*pathOp);
	}

	return true;
}
