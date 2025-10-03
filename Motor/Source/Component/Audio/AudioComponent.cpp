#include "Component/Audio/AudioComponent.h"

#include "Audio/Sound.h"
#include "Component/Component.hpp"
#include "GameObject/GameObject.h"
#include "Resource/Manager.hpp"
#include "Resource/Sound.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"


using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::AudioComponent;
using KaputEngine::Audio::Sound;
using KaputEngine::Resource::ResourceManager;
using KaputEngine::Resource::SoundResource;

using namespace std;
using std::cerr;
using std::filesystem::path;

COMPONENT_IMPL(AudioComponent)

AudioComponent::AudioComponent(GameObject& parent) :
	Component(parent) { }

AudioComponent::AudioComponent(GameObject& parent, const Id& id) :
	Component(parent, id) { }

std::shared_ptr<Sound>& AudioComponent::sound() noexcept
{
	return m_sound;
}

const std::shared_ptr<Sound>& AudioComponent::sound() const noexcept
{
	return m_sound;
}

string AudioComponent::getFilePath() const noexcept
{
	if (this->m_sound)
		return this->m_sound->parentResource()->path()->string();
	else
		return "No Sound File";
}

void AudioComponent::play(bool loop)
{
	if (!m_sound)
	{
		cerr << __FUNCTION__": No sound to play.\n";
		return;
	}

	m_sound->play(m_parentObject.getWorldTransform().position, loop, this->m_volume);
}

void KaputEngine::AudioComponent::play2D(bool loop)
{
	if (!m_sound)
	{
		cerr << __FUNCTION__": No sound to play.\n";
		return;
	}

	m_sound->play(loop, this->m_volume);
}

const float AudioComponent::getVolume() const noexcept
{
	return this->m_volume;
}

void AudioComponent::setVolume(float vol)
{
	vol = LibMath::clamp(vol, 0.f, 1.f);
	this->m_volume = vol;
}

void AudioComponent::defineLuaMembers(sol::usertype<AudioComponent>& type)
{
	Component::defineLuaMembers(type);

	type["play"]     = [](AudioComponent& comp) { comp.play(false); };
	type["playLoop"] = [](AudioComponent& comp) { comp.play(true); };
	type["play2D"] = [](AudioComponent& comp) { comp.play2D(false); };
	type["play2DLoop"] = [](AudioComponent& comp) { comp.play2D(true); };
}

void AudioComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	if (m_sound)
		if (const SoundResource* res = m_sound->parentResource(); res)
		context.value("Sound", *res->path());

	context.value("Volume", m_volume);
}

_Success_(true) bool AudioComponent::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	switch (path sourcePath; mapParse("Sound", map, sourcePath))
	{
	case eMapParseResult::SUCCESS:
		m_sound = ResourceManager::get<SoundResource>(sourcePath)->dataPtr();
		break;
	case eMapParseResult::FAILURE:
		cerr << __FUNCTION__": Failed to deserialize audio source of AudioComponent.\n";
		break;
	}

	if (mapParse("Volume", map, m_volume) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Volume. Volume is set to 1.\n";
		m_volume = 1;
	}

	return true;
}
