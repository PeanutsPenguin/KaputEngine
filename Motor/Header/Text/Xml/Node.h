#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace KaputEngine::Text::Xml
{
	struct XmlNode
	{
		using Body = std::variant<std::string, std::vector<XmlNode>>;
		using Map = std::map<std::string_view, _Notnull_ const XmlNode*>;

		size_t depth = 0;
		std::string name;
		Body body;

		XmlNode() = default;
		XmlNode(const XmlNode&) = default;
		XmlNode(XmlNode&&) noexcept = default;
		XmlNode(size_t depth);

		XmlNode& operator=(const XmlNode&) = default;
		XmlNode& operator=(XmlNode&&) noexcept = default;

		/// <summary>
		/// Converts the top-level to a referencing map.
		/// </summary>
		/// <remarks>Moving the node will invalidate existing maps</remarks>
		_NODISCARD std::optional<Map> toMap() const;

		template <typename T>
		_NODISCARD std::optional<T> parse() const noexcept;
	};
}
