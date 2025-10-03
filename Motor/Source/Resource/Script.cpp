#include "Resource/Script.h"

#include "Resource/Manager.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"

#include <filesystem>
#include <fstream>

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Resource::ScriptResource;

using std::cerr;
using std::string;
using std::string_view;

RESOURCE_IMPL(ScriptResource)

ScriptResource::~ScriptResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& ScriptResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	static constexpr const char* Context = __FUNCTION__;

	return m_loadFuture = createFuture<void>(policy,
	[this, content = std::move(content)](eMultiThreadPolicy)
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (!deserializeForLoad(content))
		{
			cerr << Context << ": Failed to load Script resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		std::ifstream file(std::filesystem::path(m_luaPath), std::ios::in);

		if (!file.is_open())
		{
			m_loadState = eLoadState::UNLOADED;
			cerr << "Failed to open source file " << m_luaPath << ".\n";
			return;
		}

		const string scriptContent(std::istreambuf_iterator { file }, { });

		file.close();

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		m_data.create(string_view(scriptContent));
		m_loadState = eLoadState::LOADED;
	});
}

void ScriptResource::unload()
{
	m_data.destroy();
}

const Script& ScriptResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<const Script> ScriptResource::dataPtr() const noexcept
{
	return { shared_from_this(), &m_data };
}

void ScriptResource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Source", m_luaPath);
}

_Success_(return) bool ScriptResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	// Without doing this, m_luaPath somehow becomes corrupted from the time of construction, but only in release builds.
	// mapParse attempting to operator= leads to a bad deallocation of the old path that should be empty
	// Trick is a possible source of memory leak if the same resource is deserialized multiple times over its lifetime
	new (&m_luaPath) std::filesystem::path();

	if (mapParse("Source", map, m_luaPath) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Source.\n";
		return false;
	}

	return true;
}
