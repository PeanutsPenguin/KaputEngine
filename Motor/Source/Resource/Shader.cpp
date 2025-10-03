#include "Resource/Shader.h"

#include "Resource/Manager.hpp"
#include "Text/ShaderPreprocessor.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"

#include <filesystem>

using namespace KaputEngine::Text::Xml;

using KaputEngine::Rendering::Shader;
using KaputEngine::Resource::ShaderResource;
using KaputEngine::Text::ShaderPreprocessor;

using std::cerr;
using std::filesystem::path;
using std::string;

RESOURCE_IMPL(ShaderResource)

ShaderResource::~ShaderResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& ShaderResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	static constexpr const char* Context = __FUNCTION__;

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
			cerr << Context << ": Failed to load Shader resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_shaderType == GL_INVALID_ENUM)
		{
			m_loadState = eLoadState::UNLOADED;
			cerr << Context << ": Kasset " << *m_path << " defines invalid shader type \"" << m_shaderPath << "\".\n";
			return;
		}

		ShaderPreprocessor preprocessor;
		std::filesystem::path sourceTmp = m_shaderPath;

		if (!preprocessor.addFile(std::move(sourceTmp)))
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		const string& shaderSource = preprocessor.result();

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

		m_data.create(m_shaderType, shaderSource);

		if (m_stopSource.stop_requested())
			unload();

		if (m_stopSource.stop_requested())
			unload();
		else
			m_loadState = eLoadState::LOADED;
	});
}

void ShaderResource::unload()
{
	if (!startUnload())
		return;

	m_data.destroy();
	m_loadState = eLoadState::UNLOADED;
}

const path& ShaderResource::shaderPath() const noexcept
{
	return m_shaderPath;
}

const Shader& ShaderResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<const Shader> ShaderResource::dataPtr() const noexcept
{
	return { shared_from_this(), &m_data };
}

GLenum ShaderResource::mapShaderType(const string& name)
{
	if (!strcmp("Vertex", name.c_str()))
		return GL_VERTEX_SHADER;
	if (!strcmp("Fragment", name.c_str()))
		return GL_FRAGMENT_SHADER;
	if (!strcmp("Geometry", name.c_str()))
		return GL_GEOMETRY_SHADER;
	if (!strcmp("Compute", name.c_str()))
		return GL_COMPUTE_SHADER;

	return GL_INVALID_ENUM;
}

void ShaderResource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Source", m_shaderPath);

	switch (m_shaderType)
	{
	case GL_VERTEX_SHADER:
		context.value("Type", "Vertex");
		break;
	case GL_FRAGMENT_SHADER:
		context.value("Type", "Fragment");
		break;
	case GL_GEOMETRY_SHADER:
		context.value("Type", "Geometry");
		break;
	case GL_COMPUTE_SHADER:
		context.value("Type", "Compute");
	}
}

_Success_(return) bool ShaderResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (mapParse("Source", map, m_shaderPath) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Shader.\n";
		return false;
	}

	string type;
	if (mapParse("Type", map, type) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Shader.\n";
		return false;
	}

	m_shaderType = mapShaderType(type);

	return true;
}
