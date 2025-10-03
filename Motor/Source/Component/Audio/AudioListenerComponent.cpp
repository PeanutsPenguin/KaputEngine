#include "Component/Audio/AudioListenerComponent.h"

#include "Audio/Sound.h"
#include "GameObject/GameObject.h"
#include "GameObject/ObjectBase.hpp"
#include "Inspector/Property.hpp"
#include "Resource/Sound.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"
#include "Utils/Assign.h"

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::AudioListenerComponent;
using KaputEngine::Audio::Sound;
using KaputEngine::Inspector::Property;
using KaputEngine::Resource::SoundResource;

using std::cerr;
using std::filesystem::path;

STATIC_DEF(AudioListenerComponent::s_primary);

COMPONENT_IMPL(AudioListenerComponent)

AudioListenerComponent::AudioListenerComponent(GameObject& parent) :
	Component(parent) { }

AudioListenerComponent::AudioListenerComponent(GameObject& parent, const Id& id)
	: Component(parent, id) { }

bool AudioListenerComponent::isPrimary() const noexcept
{
	return exclusive_weak_from_this() == s_primary;
}

exclusive_weak_ptr<AudioListenerComponent> AudioListenerComponent::getPrimary() noexcept
{
	return s_primary;
}

void AudioListenerComponent::setPrimary(AudioListenerComponent& listener) noexcept
{
	s_primary = listener.exclusive_weak_from_this();
}

void AudioListenerComponent::setPrimary(nullptr_t) noexcept
{
	s_primary = { };
}

void AudioListenerComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	if (isPrimary())
		context.value("Primary", true);
}

_Success_(true) bool AudioListenerComponent::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	switch (bool primary; mapParse("Primary", map, primary))
	{
	case eMapParseResult::SUCCESS:
		setPrimary(*this);
		break;
	case eMapParseResult::FAILURE:
		cerr << __FUNCTION__": Failed to deserialize AudioListenerComponent.\n";
		return false;
		break;
	}

	return true;
}

void AudioListenerComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("Is Primary",
	[this]() -> bool { return isPrimary(); },
	[this](const bool& value)
	{
		if (value)
			setPrimary(*this);
		else
			setPrimary(nullptr);
	});
}
