#include "Text/Xml/Parser.h"

#include "Text/String.h"
#include "Text/Xml/Node.h"

#include <iostream>

using namespace KaputEngine::Text;
using namespace KaputEngine::Text::Xml;

static constexpr const char* const TrimmedChars = " \n\r\t";

_Success_(return) bool XmlParser::parse(
	_Inout_ std::string_view& content, _Out_ XmlNode& node)
{
	content = StringUtilities::trimLeft(content, TrimmedChars);

	node = {};
	return parseRecursive(content, node);
}

_Success_(return) bool XmlParser::parseRecursive(
	_Inout_ std::string_view& content, _Inout_ XmlNode& node)
{
	std::string_view tagName;

	if (!parseTagName(content, tagName))
	{
		std::cerr << __FUNCTION__": Depth: " << node.depth << "Failed to parse opening tag name.\n";
		return false;
	}

	if (tagName.ends_with('/'))
	{
		// Tag is self-closing
		node.name = tagName.substr(0, tagName.size() - 1);
		return true;
	}
	else
		node.name = tagName;

	content = StringUtilities::trimLeft(content, TrimmedChars);

	return content.starts_with('<')
		? parseComplexNode(content, node)
		: parseValueNode(content, node);

	return true;
}

_Success_(return) bool XmlParser::parseValueNode(
	_Inout_ std::string_view& content, _Inout_ XmlNode& node)
{
	node.body = std::string(StringUtilities::splitSingle(content, '<', false));

	// Move back to the next tag opening and skip line breaks
	content = StringUtilities::trimLeft({ content.data() - 1, content.size() + 1 }, TrimmedChars);

	return parseClosingTag(content, node);
}

_Success_(return) bool XmlParser::parseComplexNode(
	_Inout_ std::string_view& content, _Inout_ XmlNode& node)
{
	auto& children = std::get<std::vector<XmlNode>>(node.body = std::vector<XmlNode>());

	while (!content.starts_with("</"))
	{
		XmlNode& child = children.emplace_back(node.depth + 1);

		if (!parseRecursive(content, child))
			return false;

		content = StringUtilities::trimLeft(content, TrimmedChars);
	}

	return parseClosingTag(content, node);
}

_Success_(return) bool XmlParser::parseClosingTag(
	_Inout_ std::string_view& content, const XmlNode& node)
{
	std::string_view tagName;

	if (!parseTagName(content, tagName))
	{
		std::cerr << __FUNCTION__": Depth " << node.depth <<
			": Failed to parse closing tag for opened tag \"" << node.name << "\".\n";
		return false;
	}

	if (tagName.substr(1).compare(node.name) != 0)
	{
		std::cerr << __FUNCTION__": Depth " << node.depth <<
			": Mismatched closing tag, expected \"/" << node.name << "\" but found \"" << tagName << "\".\n";
		return false;
	}

	return true;
}

_Success_(return) bool XmlParser::parseTagName(_Inout_ std::string_view& content, _Out_ std::string_view& tagName)
{
	if (!content.starts_with('<'))
	{
		std::cerr << __FUNCTION__": Expected '<' at the start of the tag.\n";
		return false;
	}

	// Move past the tag opening
	content = content.substr(1);

	bool closingTagFound = false;
	tagName = StringUtilities::splitSingle(content, '>', false, &closingTagFound);

	if (!closingTagFound)
	{
		std::cerr << __FUNCTION__": Expected '>' at the end of the tag \"" << tagName << "\".\n";
		return false;
	}

	return true;
}

_Success_(return) bool IXmlMapParser::deserializeNode(_In_ const XmlNode& node)
{
	std::optional<XmlNode::Map> op = node.toMap();

	if (!op)
		return false;

	return deserializeMap(*op);
}
