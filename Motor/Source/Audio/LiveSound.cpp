#include "Audio/LiveSound.h"

#include "Audio/Sound.h"

#include <irrKlang.h>

using KaputEngine::Audio::LiveSound;

using irrklang::ISound;

template <typename T>
using exclusive_weak_ptr = KaputEngine::exclusive_weak_ptr<T>;

LiveSound::LiveSound(ConstructorBlocker, Sound& source, ISound& sound) :
	m_source(source),
	m_sound(sound) { }

LiveSound::~LiveSound()
{
	m_sound.drop();
}

_Ret_maybenull_ std::shared_ptr<LiveSound> LiveSound::create(
	Sound& source, _In_opt_ ISound* const sound, const float volume)
{
	if (!sound)
		return nullptr;

	sound->setVolume(volume);

	std::shared_ptr<LiveSound> ptr = std::make_shared<LiveSound>(ConstructorBlocker(), source, *sound);

	ptr->m_ptr = ptr;
	//sound->setSoundStopEventReceiver(&source, std::to_address(ptr));

	return ptr;
}

exclusive_weak_ptr<LiveSound> LiveSound::exclusive_weak_from_this() noexcept
{
	return m_ptr;
}
exclusive_weak_ptr<const LiveSound> LiveSound::exclusive_weak_from_this() const noexcept
{
	return m_ptr;
}

void LiveSound::stop()
{
	m_sound.stop();
}
