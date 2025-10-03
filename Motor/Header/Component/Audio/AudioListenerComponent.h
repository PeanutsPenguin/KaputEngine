#pragma once

#include "../Component.h"

namespace KaputEngine
{
	namespace Audio
	{
		class Sound;
	}

	class AudioListenerComponent final : public Component
	{
		COMPONENT_SIGS(AudioListenerComponent, Component)
		OBJECTBASE_DEFAULT_REGISTER(AudioListenerComponent)

	public:
		AudioListenerComponent(GameObject& parent);
		AudioListenerComponent(GameObject& parent, const Id& id);

		_NODISCARD bool isPrimary() const noexcept;

		_NODISCARD static exclusive_weak_ptr<AudioListenerComponent> getPrimary() noexcept;
		static void setPrimary(AudioListenerComponent& listener) noexcept;
		static void setPrimary(nullptr_t) noexcept;

		void getProperties(std::vector<Inspector::Property>& out) noexcept final;

	protected:
		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;
		_NODISCARD _Success_(true) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

	private:
		static exclusive_weak_ptr<AudioListenerComponent> s_primary;
	};
}
