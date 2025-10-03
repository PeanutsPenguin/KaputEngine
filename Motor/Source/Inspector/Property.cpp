#include "Inspector/Property.h"

using KaputEngine::Inspector::Property;

const std::type_info& Property::type() const noexcept
{
	return m_typeInfo;
}

const std::string& Property::name() const noexcept
{
	return m_name;
}
