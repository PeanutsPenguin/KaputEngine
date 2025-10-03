#include "Text/Xml/Node.h"

#include <iostream>

using namespace KaputEngine::Text::Xml;

XmlNode::XmlNode(size_t depth) : depth(depth) { }

std::optional<XmlNode::Map> XmlNode::toMap() const
{
	const std::vector<XmlNode>* children = std::get_if<std::vector<XmlNode>>(&body);

	if (!children)
	{
		std::cerr << __FUNCTION__ ": Cannot convert value node \"" << name << "\" to a map.";
		return std::nullopt;
	}

	Map map;

	for (const XmlNode& child : *children)
		map.emplace(child.name, &child);

	return map;
}
