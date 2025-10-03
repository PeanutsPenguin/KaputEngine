#pragma once

#include "Serializer.h"

namespace KaputEngine::Text::Xml
{
    template <typename T>
    void XmlSerializer::serializeSingleValue(std::ostream& stream, const T& value)
    {
        stream << value;
    }
}
