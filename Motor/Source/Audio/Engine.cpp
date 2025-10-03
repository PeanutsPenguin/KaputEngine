#include "Audio/Engine.h"

#include "Component/Audio/AudioListenerComponent.h"
#include "GameObject/GameObject.h"

#include <iostream>

#include <irrKlang.h>

using namespace KaputEngine::Audio;

using LibMath::Cartesian3f;
using LibMath::Rotorf;
using LibMath::Vector3f;

AudioEngine::AudioEngine() :
	m_engine(irrklang::createIrrKlangDevice())
{
	if (!m_engine)
	{
		std::cerr << __FUNCTION__": Failed to create audio engine.\n";
		return;
	}
}

AudioEngine::~AudioEngine() noexcept
{
	if (m_engine)
	{
		m_engine->drop();
		m_engine = nullptr;
	}
}

void AudioEngine::stopAll()
{
	m_engine->stopAllSounds();
	m_engine->removeAllSoundSources();
}

void AudioEngine::updateTracking()
{
	const exclusive_weak_ptr<AudioListenerComponent> primary = AudioListenerComponent::getPrimary();

	if (primary.expired())
	{
		stopAll();
		return;
	}

	const Transform& trans = primary->parentObject()->getWorldTransform();
	const Cartesian3f& pos = trans.position;
	const Rotorf& rot      = trans.rotation;

	const Vector3f forward = rot.rotate(Cartesian3f::Dir::front());

	m_engine->setListenerPosition({ pos.x(), pos.y(), pos.z() }, { forward.x(), forward.y(), forward.z() });
}

irrklang::ISoundEngine& AudioEngine::engine() noexcept
{
	return *m_engine;
}

const irrklang::ISoundEngine& AudioEngine::engine() const noexcept
{
	return *m_engine;
}
