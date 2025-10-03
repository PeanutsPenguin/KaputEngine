#include "Text/Xml/Context.h"

#include "Text/Xml/Serializer.h"

using namespace KaputEngine::Text::Xml;

XmlSerializeContext::XmlSerializeContext(std::ostream& stream, const bool indent) :
	m_nameStack(),
	m_stream(stream),
	m_indent(indent),
	m_depth(0) { }

void XmlSerializeContext::emptyObject(const std::string& name)
{
	XmlSerializer::serializeEmptyTag(m_stream, name, m_indent ? m_depth : 0);
}

void XmlSerializeContext::startObject(std::string&& name)
{
	XmlSerializer::serializeOpenTag(m_stream, name, m_indent ? m_depth : 0, false);
	m_nameStack.push(std::move(name));
	++m_depth;
}

void XmlSerializeContext::endObject()
{
	--m_depth;
	XmlSerializer::serializeCloseTag(m_stream, m_nameStack.top(), m_indent ? m_depth : 0, false);
	m_nameStack.pop();
}
