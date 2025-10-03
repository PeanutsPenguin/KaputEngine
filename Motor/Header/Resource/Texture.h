#pragma once

#include "Resource/Resource.h"

#include "Rendering/Buffer/TextureBuffer.h"

class aiTexture;

namespace KaputEngine::Resource
{
	class TextureResource final :
		public Resource,
		public std::enable_shared_from_this<TextureResource>
	{
		RESOURCE_DEFS(TextureResource, "Texture")

	public:
		TextureResource(ConstructorBlocker, Bind<std::filesystem::path> auto&& path) :
			Resource(std::forward<decltype(path)>(path)),
			m_data(*this) { }

		~TextureResource() final;

		std::future<void>& load(std::string&& content, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD) final;

		std::future<void>& load(aiTexture&& texture, eMultiThreadPolicy policy = eMultiThreadPolicy::MONO_THREAD);

		void unload() final;

		const std::filesystem::path& imagePath() const noexcept;

		_NODISCARD const Rendering::Buffer::TextureBuffer& data() const noexcept;
		_NODISCARD std::shared_ptr<const Rendering::Buffer::TextureBuffer> dataPtr() const noexcept;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const final;

	private:
		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) final;

		Rendering::Buffer::TextureBuffer m_data;
		std::filesystem::path m_imagePath;
	};
}
