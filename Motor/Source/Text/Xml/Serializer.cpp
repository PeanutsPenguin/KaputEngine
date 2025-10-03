#include "Text/Xml/Serializer.h"

#include "Text/Xml/Context.h"
#include "Text/Xml/Node.h"

#include <ostream>

using namespace KaputEngine::Text::Xml;

void XmlSerializer::serializeIndent(std::ostream& stream, const size_t indent)
{
	for (size_t i = 0; i < indent; ++i)
		stream << "    ";
}

void XmlSerializer::serializeEmptyTag(
	std::ostream& stream, const std::string_view& name, const size_t indent)
{
	serializeIndent(stream, indent);
	stream << '<' << name << "/>\n";
}

void XmlSerializer::serializeOpenTag(
	std::ostream& stream, const std::string_view& name, const size_t indent, const bool isValue)
{
	serializeIndent(stream, indent);
	stream << '<' << name << '>';

	if (!isValue)
		stream << '\n';
}

void XmlSerializer::serializeCloseTag(
	std::ostream& stream, const std::string_view& name, const size_t indent, const bool isValue)
{
	if (!isValue)
		serializeIndent(stream, indent);

	stream << "</" << name << ">\n";
}

void XmlSerializer::serializeNode(
	std::ostream& stream, const XmlNode& node, const bool indent)
{
	size_t indentValue = indent ? node.depth : 0;

	serializeIndent(stream, indentValue);

	const std::string* value = std::get_if<std::string>(&node.body);

	if (value && value->empty())
		serializeEmptyTag(stream, node.name, indentValue);

	serializeOpenTag(stream, node.name, indentValue);

	if (value)
		stream << *value;
	else
		for (const XmlNode& child : std::get<std::vector<XmlNode>>(node.body))
			serializeNode(stream, child, indent);

	serializeCloseTag(stream, node.name, indentValue);
}

template <>
static void XmlSerializer::serializeSingleValue<int8_t>(std::ostream& stream, const int8_t& value)
{
	stream << static_cast<int>(value);
}

template <>
static void XmlSerializer::serializeSingleValue<uint8_t>(std::ostream& stream, const uint8_t& value)
{
	stream << static_cast<unsigned int>(value);
}

void IXmlPolymorphicSerializer::serializeObject(XmlSerializeContext& context) const
{
	context.startObject(xmlTypeName());
	serializeValues(context);
	context.endObject();
}
