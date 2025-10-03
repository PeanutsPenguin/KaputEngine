#include "Resource/Material.hpp"

#include "Resource/Manager.hpp"
#include "Resource/Texture.h"

using namespace KaputEngine::Rendering;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Resource::MaterialResource;

using std::cerr;
using std::string;

RESOURCE_IMPL(MaterialResource)

MaterialResource::~MaterialResource()
{
	if (cancelForDestroy())
		unload();
}

std::future<void>& MaterialResource::load(string&& content, const eMultiThreadPolicy policy)
{
	if (!startLoad())
		return m_loadFuture;

	static constexpr const char* Context = __FUNCTION__;

	return m_loadFuture = createFuture<void>(policy,
	[this, content = std::move(content)](eMultiThreadPolicy) -> void
	{
		if (m_stopSource.stop_requested())
		{
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (!deserializeForLoad(content))
		{
			cerr << Context << ": Failed to load Material resource.\n";
			m_loadState = eLoadState::UNLOADED;
			return;
		}

		if (m_stopSource.stop_requested())
			unload();
		else
			m_loadState = eLoadState::LOADED;
	});
}

void MaterialResource::unload()
{
	if (startUnload())
		m_loadState = eLoadState::UNLOADED;
}

Material& MaterialResource::data() noexcept
{
	return m_data;
}

const Material& MaterialResource::data() const noexcept
{
	return m_data;
}

std::shared_ptr<Material> MaterialResource::dataPtr() noexcept
{
	return { this->shared_from_this(), &m_data };
}

std::shared_ptr<const Material> MaterialResource::dataPtr() const noexcept
{
	return { this->shared_from_this(), &m_data };
}

const std::shared_ptr<const MaterialResource> MaterialResource::defaultMaterial() noexcept
{
	return ResourceManager::get<MaterialResource>("Kaput/Material/noTextureMat.kasset");
}

_Success_(return) bool MaterialResource::deserializeSamplerTexture(TextureSample& sample, const XmlNode& node)
{
	std::optional<XmlNode::Map> mapOp = node.toMap();

	if (!mapOp)
	{
		cerr << __FUNCTION__": Failed to parse TextureSample.\n";
		return false;
	}

	const XmlNode::Map& map = *mapOp;

	if (mapParse("Index", map, sample.unitIndex) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize TextureSample.\n";
		return false;
	}

	string path;

	if (mapParse("Source", map, path) != eMapParseResult::SUCCESS)
	{
		cerr << __FUNCTION__": Failed to deserialize TextureSample.\n";
		return false;
	}

	sample.texture = ResourceManager::get<TextureResource>(path)->dataPtr();

	return true;
}

_Success_(return) bool MaterialResource::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (const auto it = map.find("Albedo"); it != map.end())
		if (!deserializeSampler(m_data.albedo(), *it->second))
		{
			cerr << __FUNCTION__": Failed to deserialize Albedo.\n";
			return false;
		}

	if (const auto it = map.find("Normal"); it != map.end())
		if (!deserializeSampler(m_data.normal(), *it->second))
		{
			std::cerr << __FUNCTION__": Failed to deserialize Normal.\n";
			return false;
		}

	if (const auto it = map.find("Metallic"); it != map.end())
		if (!deserializeSampler(m_data.metallic(), *it->second))
		{
			cerr << __FUNCTION__": Failed to deserialize Metallic.\n";
			return false;
		}

	if (const auto it = map.find("Roughness"); it != map.end())
		if (!deserializeSampler(m_data.roughness(), *it->second))
		{
			cerr << __FUNCTION__": Failed to deserialize Roughness.\n";
			return false;
		}

	if (const auto it = map.find("AmbientOcclusion"); it != map.end())
		if (!deserializeSampler(m_data.ambientOcclusion(), *it->second))
		{
			cerr << __FUNCTION__": Failed to deserialize AmbientOcclusion.\n";
			return false;
		}

	return true;
}

void MaterialResource::serializeValues(XmlSerializeContext& context) const
{
	serializeSampler(m_data.albedo(), context);
	serializeSampler(m_data.normal(), context);
	serializeSampler(m_data.metallic(), context);
	serializeSampler(m_data.roughness(), context);
	serializeSampler(m_data.ambientOcclusion(), context);
}
