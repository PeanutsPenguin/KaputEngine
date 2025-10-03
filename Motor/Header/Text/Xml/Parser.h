#pragma once

#include "Node.h"

#include <string_view>

namespace KaputEngine::Text::Xml
{
	class XmlParser final
	{
	public:
		_NODISCARD _Success_(return) static bool parse(
			_Inout_ std::string_view& content, _Out_ XmlNode& node);

		_NODISCARD _Success_(return) static bool parseTagName(
			_Inout_ std::string_view& content, _Out_ std::string_view& tagName);

	private:
		_NODISCARD _Success_(return) static bool parseRecursive(
			_Inout_ std::string_view& content, _Inout_ XmlNode& node);

		_NODISCARD _Success_(return) static bool parseValueNode(
			_Inout_ std::string_view& content, _Inout_ XmlNode& node);

		_NODISCARD _Success_(return) static bool parseComplexNode(
			_Inout_ std::string_view& content, _Inout_ XmlNode& node);

		_NODISCARD _Success_(return) static bool parseClosingTag(
			_Inout_ std::string_view& content, const XmlNode& node);
	};

	struct IXmlParser
	{
		_NODISCARD _Success_(return) virtual bool deserializeNode(_In_ const XmlNode& node) = 0;
	};

	enum class eMapParseResult
	{
		SUCCESS,
		FAILURE,
		NOT_FOUND,
	};

	struct IXmlMapParser : IXmlParser
	{
		_NODISCARD _Success_(return) virtual bool deserializeNode(_In_ const XmlNode& node) override;
		_NODISCARD _Success_(return) virtual bool deserializeMap(_In_ const XmlNode::Map& map) = 0;

	protected:
		template <typename T>
		_NODISCARD _Success_(return == eMapParseResult::SUCCESS)
			eMapParseResult mapParse(const std::string& name, const XmlNode::Map& map, _Out_ T& value);
	};
}
