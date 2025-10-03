#include "Resource/Resource.h"

#include "Resource/Manager.h"
#include "Text/String.h"
#include "Text/Xml/Node.h"
#include "Utils/Policy.h"

#include <fstream>
#include <iostream>

using namespace KaputEngine::Text;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Resource::Resource;

using std::cerr;
using std::string;
using std::string_view;

Resource::~Resource()
{
	if (m_path)
		ResourceManager::remove(*m_path);
}

bool Resource::isKasset() const noexcept
{
	return m_path && m_path->extension() == ".kasset";
}

const std::optional<std::filesystem::path>& Resource::path() const noexcept
{
	return m_path;
}

Resource::eLoadState Resource::loadState() const noexcept
{
	return m_loadState;
}

bool Resource::processing() const noexcept
{
	return static_cast<int8_t>(loadState()) & static_cast<int8_t>(eLoadState::PROCESSING);
}

std::future<void>& Resource::loadExisting(const eMultiThreadPolicy policy)
{
	if (!m_path)
		return m_loadFuture;

	std::ifstream file(*m_path, std::ios::in);

	if (!file.is_open())
	{
		cerr << __FUNCTION__": Error opening file: " << *m_path << '\n';
		return m_loadFuture;
	}

	string content = string(std::istreambuf_iterator { file }, {});
	file.close();

	string_view view = content;

	view = StringUtilities::warp(view, '\n').substr(1);
	return load(std::move(content), policy);
}

void Resource::cancelLoad()
{
	m_stopSource.request_stop();
}

bool Resource::startLoad() noexcept
{
	if (m_loadState != eLoadState::UNLOADED)
		return false;

	m_loadState = eLoadState::LOADING;

	// Reset the stop token
	m_stopSource = std::stop_source();
	return true;
}

bool Resource::startUnload() noexcept
{
	if (m_loadState != eLoadState::LOADED)
		return false;

	m_loadState = eLoadState::UNLOADING;
	return true;
}

bool Resource::cancelForDestroy() noexcept
{
	if (m_loadState != eLoadState::LOADED)
		return true;

	if (processing())
	{
		cancelLoad();
		m_loadFuture.wait();
	}

	return false;
}

std::future<void>& Resource::noFuture() noexcept
{
	return m_loadFuture = emptyFuture<void>();
}

_Success_(return) bool Resource::deserializeForLoad(const string& content)
{
	XmlNode document;
	string_view view = content;

	if (!XmlParser::parse(view, document))
	{
		cerr << __FUNCTION__": Failed to load Model resource.\n";
		m_loadState = eLoadState::UNLOADED;
		return false;
	}

	if (m_stopSource.stop_requested())
	{
		m_loadState = eLoadState::UNLOADED;
		return true;
	};

	if (!deserializeNode(document))
	{
		cerr << __FUNCTION__": Failed to load Model resource.\n";
		m_loadState = eLoadState::UNLOADED;
		return false;
	}

	return true;
}

void Resource::waitLoad() const
{
	if (processing())
		m_loadFuture.wait();
}
