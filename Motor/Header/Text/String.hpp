#pragma once

#include "String.h"

#include "Id.h"

#include <charconv>
#include <iostream>

namespace KaputEngine::Text
{
	template <LibMath::numeric T>
	std::optional<T> StringUtilities::parse(std::string_view& view)
	{
		std::string_view originalView = view;

		view = trimLeft(view, ", \n\t");

		T value = 0;
		const std::from_chars_result res = std::from_chars(view.data(), view.data() + view.size(), value);

		if (res.ec == std::errc::invalid_argument)
		{
			const std::string_view preview = originalView.size() > 10 ? originalView.substr(0, 10) : originalView;
			std::cerr << __FUNCTION__": Failed to parse value \"" << preview << "\".\n";

			return std::nullopt;
		}

		if (res.ec == std::errc::result_out_of_range)
		{
			std::cerr << __FUNCTION__": Value " << originalView << " out of range for type " << typeid(T).name() << ".\n";

			return std::nullopt;
		}

		view = view.substr(res.ptr - view.data());
		return value;
	}

	template <LibMath::IsMathArray T>
	std::optional<T> StringUtilities::parse(std::string_view& view)
	{
		using std::cerr;
		using MathArrayType = typename T::template StaticAs<LibMath::MathArray>;

		// Redirection of derived types to reduce the footprint of instantiations
		if constexpr (!std::same_as<T, MathArrayType>)
		{
			if (const std::optional<MathArrayType> resOp = parse<MathArrayType>(view))
				return resOp->template as<T>();
			else
				return std::nullopt;
		}
		else
		{
			view = trimLeft(view, " \t");

			if (!view.starts_with('{'))
			{
				cerr << __FUNCTION__": Expected '{' at the start of array.\n";
				return std::nullopt;
			}

			view = view.substr(1);

			LibMath::ArrIndex index = 0;
			MathArrayType arr;

			while (!view.starts_with('}'))
			{
				if (index == T::Size)
				{
					cerr << __FUNCTION__": Too many values for array of size " << MathArrayType::Size << ".\n";
					return std::nullopt;
				}

				if (view.empty())
				{
					cerr << __FUNCTION__": Expected '}' at the end of array.\n";
					return std::nullopt;
				}

				const auto valueOp = parse<typename MathArrayType::DataType>(view);

				if (!valueOp)
				{
					cerr << __FUNCTION__": Failed to parse value in array.\n";
					return std::nullopt;
				}

				arr[index++] = *valueOp;
				view = trimLeft(view, ", ");
			}

			return arr;
		}
	}

	template <>
	_NODISCARD inline std::optional<bool> StringUtilities::parse<bool>(std::string_view& view)
	{
		if (view == "true")
			return true;
		if (view == "false")
			return false;

		const auto parseOpt = parse<uint8_t>(view);

		if (!parseOpt)
			return std::nullopt;

		return *parseOpt != 0;
	}

	template <>
	_NODISCARD inline std::optional<std::string> StringUtilities::parse<std::string>(std::string_view& view)
	{
		if (view.starts_with('"'))
			view = view.substr(1);

		if (view.ends_with('"'))
			view = view.substr(0, view.size() - 1);

		return std::string(view);
	}

	template <>
	_NODISCARD inline std::optional<std::filesystem::path> StringUtilities::parse<std::filesystem::path>(std::string_view& view)
	{
		std::optional<std::string> path = parse<std::string>(view);

		if (!path)
			return std::nullopt;

		return std::filesystem::path(*path);
	}

#ifdef KAPUT_GUID
	template <>
	_NODISCARD inline std::optional<Id> StringUtilities::parse<Id>(std::string_view& view)
	{
		uint64_t parts[sizeof(Id) / sizeof(uint64_t)];

		for (uint64_t& part : parts)
		{
			std::optional<uint64_t> partOp = parse<uint64_t>(view);

			if (!partOp)
				return std::nullopt;

			view = trimLeft(view, "-");

			part = *partOp;
		}

		return { reinterpret_cast<Id::Data&>(parts) };
	}
#endif
}
