#pragma once

#include <string_view>

namespace KaputEngine::Text::Xml
{
    struct XmlSerializer final
    {
        static void serializeIndent(std::ostream& stream, size_t indent);
        static void serializeEmptyTag(std::ostream& stream, const std::string_view& name, size_t indent);
        static void serializeOpenTag(std::ostream& stream, const std::string_view& name, size_t indent, bool isValue = false);
        static void serializeCloseTag(std::ostream& stream, const std::string_view& name, size_t indent, bool isValue = false);

        static void serializeNode(std::ostream& stream, const struct XmlNode& node, bool indent);

        template <typename T>
        static void serializeSingleValue(std::ostream& stream, const T& value);
    };

    struct IXmlSerializer
    {
        virtual void serializeValues(class XmlSerializeContext& context) const = 0;
    };

    struct IXmlPolymorphicSerializer : IXmlSerializer
    {
        virtual void serializeObject(class XmlSerializeContext& context) const;

    protected:
        _NODISCARD _Ret_notnull_ virtual const char* xmlTypeName() const = 0;
    };

    template <>
    static void XmlSerializer::serializeSingleValue<int8_t>(std::ostream& stream, const int8_t& value);

    template <>
    static void XmlSerializer::serializeSingleValue<uint8_t>(std::ostream& stream, const uint8_t& value);
}
