#pragma once

#include "Context.h"

#include "Serializer.hpp"

namespace KaputEngine::Text::Xml
{
	template <typename T>
	void XmlSerializeContext::value(const std::string& name, const T& value)
	{
		if (name.empty())
		{
			XmlSerializer::serializeSingleValue(m_stream, value);
			return;
		}

		XmlSerializer::serializeOpenTag(m_stream, name, m_indent ? m_depth : 0, true);
		XmlSerializer::serializeSingleValue(m_stream, value);
		XmlSerializer::serializeCloseTag(m_stream, name, m_indent ? m_depth : 0, true);
	}
}
