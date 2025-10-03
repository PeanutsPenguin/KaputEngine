#pragma once

#include "Utils/Function.h"

#include <optional>
#include <sal.h>
#include <string>

namespace KaputEngine::Inspector
{
	class Property
	{
	public:
		Property() = delete;
		Property(const Property&) = default;
		Property(Property&&) = default;

		template <typename Getter, typename T = typename function_traits<Getter>::return_type>
		Property(
			std::string name, Getter&& getter) noexcept;

		template <typename Getter, typename Setter, typename T = typename function_traits<Getter>::return_type>
		Property(
			std::string name, Getter&& getter, Setter&& setter) noexcept;

		_NODISCARD const std::type_info& type() const noexcept;

		_NODISCARD const std::string& name() const noexcept;

		template <typename T>
		_NODISCARD bool canRead() const noexcept;

		template <typename T>
		_NODISCARD bool canWrite() const noexcept;

		template <typename T>
		T get();

		template <typename T>
		void set(const T& value);

	private:
		std::string m_name;
		std::function<void(_Out_writes_bytes_(m_size) void* out)> m_getter;
		std::optional<std::function<void(const void* value)>> m_setter = std::nullopt;
		const std::type_info& m_typeInfo;
		size_t m_size;
	};
}
