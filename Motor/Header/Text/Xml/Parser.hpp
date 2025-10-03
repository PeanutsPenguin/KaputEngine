#pragma once

#include "Parser.h"

#include "Node.hpp"

namespace KaputEngine::Text::Xml
{
	template <typename T>
	_Success_(return == eMapParseResult::SUCCESS)
	eMapParseResult IXmlMapParser::mapParse(const std::string& name, const XmlNode::Map& map, _Out_ T& value)
	{
		const auto it = map.find(name);

		if (it == map.end())
			return eMapParseResult::NOT_FOUND;

		std::optional<T> op = it->second->parse<T>();

		if (!op)
		{
			std::cerr << __FUNCTION__": Failed to parse " << name << ".\n";
			return eMapParseResult::FAILURE;
		}

		value = std::move(*op);
		return eMapParseResult::SUCCESS;
	}
}
