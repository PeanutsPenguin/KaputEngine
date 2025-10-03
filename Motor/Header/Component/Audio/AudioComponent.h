#pragma once

#include "../Component.h"

namespace KaputEngine
{
	namespace Audio
	{
		class Sound;
	}

	class AudioComponent final : public Component
	{
		COMPONENT_SIGS(AudioComponent, Component)
		OBJECTBASE_DEFAULT_REGISTER(AudioComponent)

	public:
		AudioComponent(GameObject& parent);
		AudioComponent(GameObject& parent, const Id& id);

		_NODISCARD std::shared_ptr<Audio::Sound>& sound() noexcept;
		_NODISCARD const std::shared_ptr<Audio::Sound>& sound() const noexcept;

		_NODISCARD std::string getFilePath() const noexcept;

		void play(bool loop = false);

		void play2D(bool loop = false);

		_NODISCARD const float getVolume() const noexcept;

		void setVolume(float vol);

	protected:
		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;
		_NODISCARD _Success_(true) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

	private:
		std::shared_ptr<Audio::Sound> m_sound;
		float m_volume;

		static void defineLuaMembers(sol::usertype<AudioComponent>& type);
	};
}
