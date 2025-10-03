#define STB_IMAGE_IMPLEMENTATION

#include "Resource/Texture.h"

#include "Queue/Context.h"
#include "Resource/Manager.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"
#include "Utils/Policy.h"

#include <assimp/texture.h>
#include <stb_image/stb_image.h>

using namespace KaputEngine::Text::Xml;

using KaputEngine::Resource::TextureResource;

using KaputEngine::Rendering::Buffer::TextureBuffer;
using KaputEngine::Queue::ContextQueue;

using LibMath::Vector3i;

using std::cerr;
using std::string;
using std::string_view;
using std::filesystem::path;

RESOURCE_IMPL(TextureResource)

TextureResource::~TextureResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& TextureResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	if (!isKasset())
	{
		cerr << __FUNCTION__ ": Attempt to load Texture resource from text but is not a kasset.\n";
		m_loadState = eLoadState::UNLOADED;

		return m_loadFuture;
	}

	static constexpr const char* Context = __FUNCTION__;

	return m_loadFuture = createFuture<void>(policy,
	[this, content = std::move(content)](eMultiThreadPolicy) -> void
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		XmlNode document;
		string_view view = content;

		if (!XmlParser::parse(view, document))
		{
			cerr << Context << ": Failed to load Texture resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (!deserializeNode(document))
		{
			cerr << Context << ": Failed to load Texture resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		stbi_set_flip_vertically_on_load(true);

		Vector3i size;
		uint8_t* imageData = stbi_load(m_imagePath.string().c_str(), &size.x(), &size.y(), &size.z(), 0);

		if (!imageData)
		{
			m_loadState = eLoadState::UNLOADED;
			std::cerr << Context << ": Failed to load texture image file " << m_imagePath << ".\n";
			return;
		}

		if (m_stopSource.stop_requested())
		{
			stbi_image_free(imageData);
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		ContextQueue::instance().push([this, &size, imageData]() -> void
		{
			m_data.create(size, imageData);
			stbi_image_free(imageData);

			if (m_stopSource.stop_requested())
				unload();
			else
				m_loadState = eLoadState::LOADED;
		}).wait();
	});
}

std::future<void>& TextureResource::load(aiTexture&& texture, eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	static const char* Context = __FUNCTION__;

	return m_loadFuture = createFuture<void>(policy,
	[this, texture = std::move(texture)](eMultiThreadPolicy)
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		bool compressed = !texture.mHeight;
		uint8_t* imageData = reinterpret_cast<uint8_t*>(texture.pcData);
		Vector3i size;

		if (compressed)
		{
			imageData = stbi_load_from_memory(
				imageData, texture.mWidth, &size.x(), &size.y(), &size.z(), 0);

			if (!imageData)
			{
				cerr << Context << ": Failed to load texture image file.\n";
				m_loadState = eLoadState::UNLOADED;
				return;
			}

			if (m_stopSource.stop_requested())
			{
				stbi_image_free(imageData);
				m_loadState = eLoadState::UNLOADED;
				return;
			}
		}
		else
			size = { (int)texture.mWidth, (int)texture.mHeight, 3 };

		ContextQueue::instance().push([this, size, imageData, compressed]() -> void
		{
			if (m_stopSource.stop_requested())
			{
				if (compressed)
					stbi_image_free(imageData);

				m_loadState = eLoadState::UNLOADED;
				return;
			}

			m_data.create(size, imageData);

			if (compressed)
				stbi_image_free(imageData);

			m_loadState = eLoadState::LOADED;
		}).wait();
	});
}

void TextureResource::unload()
{
	if (!startUnload())
		return;

	m_data.destroy();
	m_loadState = eLoadState::UNLOADED;
}

const path& TextureResource::imagePath() const noexcept
{
	return m_imagePath;
}

const TextureBuffer& TextureResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<const TextureBuffer> TextureResource::dataPtr() const noexcept
{
	return { shared_from_this(), &m_data };
}

void TextureResource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Source", m_imagePath);
}

_Success_(return) bool TextureResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (mapParse("Source", map, m_imagePath) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize Source.\n";
		return false;
	}

	return true;
}
