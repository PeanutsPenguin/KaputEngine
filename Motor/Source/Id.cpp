#ifdef KAPUT_GUID
#include "Id.h"

#include <combaseapi.h>
#include <iostream>

using KaputEngine::Id;
using std::ostream;

Id::Id(const Data& data) : m_data(data) { }

Id Id::generate()
{
	GUID guid;
	if (CoCreateGuid(&guid) != S_OK)
	{
		std::cerr << __FUNCTION__": Failed to generate Id.\n";
		return { };
	}

	Data data = reinterpret_cast<Data&>(guid);

	std::cout << std::hex;

	for (auto d : data)
		std::cout << d << '-';

	std::cout << std::dec << '\n';

	return { reinterpret_cast<Data&>(guid) };
}

const Id::Data& Id::data() const noexcept
{
	return m_data;
}

bool Id::operator==(const Id& other) const noexcept
{
	return m_data == other.m_data;
}

bool Id::operator!=(const Id& other) const noexcept
{
	return m_data != other.m_data;
}

ostream& KaputEngine::operator<<(ostream& os, const Id& id)
{
	os << std::hex;

	for (const auto& data : id.data())
		os << data;

	os << std::dec;

	return os;
}
#endif