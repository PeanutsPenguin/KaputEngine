#pragma once

#include "Resource/Material.h"

#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"

namespace KaputEngine::Resource
{
	template <typename TData>
	_Success_(return) bool MaterialResource::deserializeSampler(Rendering::Sampler<TData>& sampler, const Text::Xml::XmlNode& node)
	{
		using namespace KaputEngine::Rendering;
		using namespace KaputEngine::Text::Xml;

		const std::optional<XmlNode::Map> mapOp = node.toMap();

		if (!mapOp)
		{
			std::cerr << __FUNCTION__": Failed to parse Sampler.\n";
			return false;
		}

		const XmlNode::Map& map = *mapOp;

		const auto globalIt = map.find("Global");
		const auto attributeIt = map.find("Attribute");

		if (globalIt != map.end() and attributeIt != map.end())
		{
			std::cerr << __FUNCTION__": Sampler cannot have both Global and Attribute.\n";
			return false;
		}

		if (globalIt != map.end())
		{
			std::optional<TData> op = globalIt->second->parse<TData>();

			if (!op)
			{
				std::cerr << __FUNCTION__": Failed to deserialize Global.\n";
				return false;
			}

			sampler.fallbackData() = *op;
		}
		else if (attributeIt != map.end())
		{
			std::optional<unsigned int> op = attributeIt->second->parse<unsigned int>();

			if (!op)
			{
				std::cerr << __FUNCTION__": Failed to deserialize Attribute.\n";
				return false;
			}

			sampler.fallbackData() = VertexAttribute(*op);
		}
		else
		{
			// No return - Message is a warning and not an error
			std::cerr << __FUNCTION__": Sampler does not define fallback.\n";
		}

		if (const auto it = map.find("Texture"); it != map.end())
			if (!deserializeSamplerTexture(sampler.texture(), *it->second))
			{
				std::cerr << __FUNCTION__": Failed to deserialize Texture.\n";
				return false;
			}

		return true;
	}

	template<typename TData>
	void MaterialResource::serializeSampler(const Rendering::Sampler<TData>& sampler, Text::Xml::XmlSerializeContext& context) const
	{
		using namespace KaputEngine::Rendering;
		using namespace KaputEngine::Text::Xml;

		switch (sampler.fallbackMode())
		{
		case eSamplerFallback::GLOBAL:
			context.value("Global", sampler.global());
			break;

		case eSamplerFallback::ATTRIBUTE:
			context.value("Attribute", sampler.attribute().index);
			break;
		}

		if (sampler.texture().texture)
		{
			// TODO
		}
	}
};
