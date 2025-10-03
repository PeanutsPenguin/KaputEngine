#include "Audio/Sound.h"

#include "Application.h"
#include "Audio/LiveSound.h"
#include "Utils/RemoveVector.hpp"
#include "Resource/Sound.h"

#include <LibMath/Vector/Vector3.h>

#include <ik_EStreamModes.h>
#include <ik_ISoundEngine.h>

using KaputEngine::Audio::LiveSound;
using KaputEngine::Audio::Sound;
using KaputEngine::Resource::SoundResource;

template <typename T>
using exclusive_weak_ptr = KaputEngine::exclusive_weak_ptr<T>;

using LibMath::Vector3f;

using enum irrklang::E_STREAM_MODE;
using irrklang::E_STOP_EVENT_CAUSE;
using irrklang::ISound;
using irrklang::ISoundSource;

using std::cerr;
using std::filesystem::path;

Sound::Sound(SoundResource& parent) :
	m_parentResource(&parent),
	m_liveSounds(LiveSound::createRemoveVector<std::shared_ptr<LiveSound>>()) { }

Sound::~Sound() noexcept
{
	if (m_soundSource)
		destroy();
}

_Success_(true) bool Sound::create(const path& path)
{
	//m_soundSource = Application::audio().engine().addSoundSourceFromFile(
	//	path.string().c_str(), ESM_AUTO_DETECT, true);

	//if (!m_soundSource)
	//{
	//	cerr << __FUNCTION__": Failed to create sound source from file: " << path.string() << "\n";
	//	return false;
	//}

	return true;
}

void Sound::destroy()
{
	if (m_soundSource)
	{
		//m_soundSource->drop();
		m_soundSource = nullptr;
	}
}

bool Sound::valid() const noexcept
{
	return m_soundSource;
}

void Sound::play(const bool loop, const float volume)
{
	ISound* ptr = Application::audio().engine().play2D(this->m_parentResource->getSoundPath().string().c_str(), loop, false, true);

	if (ptr)
		ptr->setVolume(volume);
}

void Sound::play(const Vector3f& pos, const bool loop, const float volume)
{
	ISound* ptr = Application::audio().engine().play3D(this->m_parentResource->getSoundPath().string().c_str(), { pos.x(), pos.y(), pos.z() }, loop, false, true);

	if (ptr)
		ptr->setVolume(volume);
}

_Ret_maybenull_ SoundResource* Sound::parentResource() noexcept
{
	return m_parentResource;
}

_Ret_maybenull_ const SoundResource* Sound::parentResource() const noexcept
{
	return m_parentResource;
}

void Sound::OnSoundStopped(ISound*, E_STOP_EVENT_CAUSE, void* userData)
{
	LiveSound* sound = static_cast<LiveSound*>(userData);
	m_liveSounds.erase(*sound);
}
