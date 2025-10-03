#pragma once

namespace KaputEngine
{
#ifdef KAPUT_GUID
#include <array>
#include <cstdint>
#include <iosfwd>

	class Id
	{
	public:
		using Data = std::array<uint64_t, 2>;

		Id() = default;
		Id(const Data& data);

		_NODISCARD static Id generate();

		const Data& data() const noexcept;

		_NODISCARD bool operator==(const Id& other) const noexcept;
		_NODISCARD bool operator!=(const Id& other) const noexcept;

	private:
		Data m_data { };
	};

	std::ostream& operator<<(std::ostream& os, const Id& id);
#else
	using Id = unsigned long long;
#endif
}
