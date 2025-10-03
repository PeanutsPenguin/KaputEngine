#pragma once

#include "Resource/Resource.h"

#include "Audio/Sound.h"
#include "Utils/Policy.h"

namespace KaputEngine::Resource
{
	class SoundResource :
		public Resource,
		public std::enable_shared_from_this<SoundResource>
	{
		RESOURCE_DEFS(SoundResource, "Sound")

	public:
		SoundResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path):
			Resource(std::forward<decltype(path)>(path)),
			m_data(*this) { }

		~SoundResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) final;

		void unload() final;

		_NODISCARD Audio::Sound& data() noexcept;
		_NODISCARD const Audio::Sound& data() const noexcept;

		_NODISCARD std::shared_ptr<Audio::Sound> dataPtr() noexcept;
		_NODISCARD std::shared_ptr<const Audio::Sound> dataPtr() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

		_NODISCARD std::filesystem::path getSoundPath();

		_NODISCARD const std::filesystem::path& soundPath() const noexcept;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		Audio::Sound m_data;
		std::filesystem::path m_soundPath;
	};
}
