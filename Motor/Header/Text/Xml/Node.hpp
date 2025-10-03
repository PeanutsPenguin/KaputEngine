#pragma once

#include "Node.h"

#include "Text/String.hpp"

namespace KaputEngine::Text::Xml
{
	template <typename T>
	std::optional<T> XmlNode::parse() const noexcept
	{
		const std::string* valueText = std::get_if<std::string>(&body);

		if (!valueText)
		{
			std::cerr << __FUNCTION__": Expected string for value node.\n";
			return std::nullopt;
		}

		std::string_view view = *valueText;
		const auto valueOp = StringUtilities::parse<T>(view);

		if (!valueOp)
		{
			std::cerr << __FUNCTION__": Failed to parse node value " << view << " \n";
			return std::nullopt;
		}

		return *valueOp;
	}
}
