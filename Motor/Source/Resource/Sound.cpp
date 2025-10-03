#include "Resource/Sound.h"

#include "Resource/Manager.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"

using namespace KaputEngine::Text::Xml;

using KaputEngine::Audio::Sound;
using KaputEngine::Resource::SoundResource;

using std::cerr;
using std::filesystem::path;
using std::string;

RESOURCE_IMPL(SoundResource)

SoundResource::~SoundResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& SoundResource::load(string&& content, const eMultiThreadPolicy policy)
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
			cerr << Context << ": Failed to load Sound resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (!m_data.create(m_soundPath))
		{
			cerr << Context << ": Failed to load Sound resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
			unload();
		else
			m_loadState = eLoadState::LOADED;
	});
}

void SoundResource::unload()
{
	if (startUnload())
		m_loadState = eLoadState::UNLOADED;

	m_data.destroy();
}

const path& SoundResource::soundPath() const noexcept
{
	return m_soundPath;
}

Sound& SoundResource::data() noexcept
{
	return m_data;
}

const Sound& SoundResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<Sound> SoundResource::dataPtr() noexcept
{
	return { this->shared_from_this(), &m_data };
}

std::shared_ptr<const Sound> SoundResource::dataPtr() const noexcept
{
	return { this->shared_from_this(), &m_data };
}

_Success_(return) bool SoundResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (mapParse("Source", map, m_soundPath) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Source path.\n";
		return false;
	}

	return true;
}

void SoundResource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Source", m_soundPath);
}

std::filesystem::path KaputEngine::Resource::SoundResource::getSoundPath()
{
	return m_soundPath;
}
