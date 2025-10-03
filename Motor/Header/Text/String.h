#pragma once

#include <LibMath/Arithmetic.h>

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

constexpr unsigned int NAME_MAX_CHARACTERS = 25;

#ifdef KAPUT_GUID
namespace KaputEngine
{
	class Id;
}
#endif

namespace KaputEngine::Text
{
	struct StringUtilities final
	{
		/// <summary>
		/// Splits a string into multiple parts based on a separator character.
		/// </summary>
		/// <param name="str">String or view to split</param>
		/// <param name="separator">Separator character, excluded from all results</param>
		/// <param name="removeEmpty">Do not include empty strings in the output</param>
		/// <param name="maxSplits">Max amount of splits, after which the remainder is added as a single string</param>
		/// <returns>Vector of splits</returns>
		_NODISCARD static std::vector<std::string_view> split(
			const std::string_view& str, char separator, bool removeEmpty = false, size_t maxSplits = std::numeric_limits<int>::max());

		/// <summary>
		/// Splits a string into multiple parts based on a separator.
		/// </summary>
		/// <param name="str">String or view to split, modified to be moved forward during the split</param>
		/// <param name="separator">Separator character, excluded from all results</param>
		/// <param name="removeEmpty">Do not include empty strings in the output</param>
		/// <param name="maxSplits">Max amount of splits, after which the remainder is added as a single string</param>
		/// <returns>Vector of splits</returns>
		_NODISCARD static std::vector<std::string_view> split(
			std::string_view& str, char separator, bool removeEmpty = false, size_t maxSplits = std::numeric_limits<int>::max());

		/// <summary>
		/// Performs a single split on a string based on a separator character.
		/// </summary>
		/// <param name="str">String or view to split, modified to contain the resulting right-hand side of the split.</param>
		/// <param name="separator">Separator character, excluded from the result</param>
		/// <param name="ignoreEmpty">Keep splitting until a non empty string is found or the end of the string is reached</param>
		/// <param name="separatorFound">True if the separator was found, false if the string was empty or the separator was missing</param>
		/// <returns>Left-hand side of the split. Contains the entire original string if the separator character was missing.</returns>
		_NODISCARD static std::string_view splitSingle(
			std::string_view& str, char separator, bool ignoreEmpty = false, _Out_opt_ bool* separatorFound = nullptr);

		_NODISCARD static std::string_view warp(const std::string_view& str, char destination);

		_NODISCARD static std::string_view trimLeft(const std::string_view& str, _In_ const char* trimmed);

		template <typename T>
		static std::optional<T> parse(std::string_view&)
		{
			static_assert(false, "Type not supported for parsing");
			return std::nullopt;
		}

		template <LibMath::numeric T>
		_NODISCARD static std::optional<T> parse(std::string_view& view);

		template <LibMath::IsMathArray T>
		_NODISCARD static std::optional<T> parse(std::string_view& view);

		template <typename T> requires (std::is_enum_v<T>)
		_NODISCARD static std::optional<T> parse(std::string_view& view);
	};

	template <>
	_NODISCARD std::optional<bool> StringUtilities::parse<bool>(std::string_view& view);

	template <>
	_NODISCARD std::optional<std::string> StringUtilities::parse<std::string>(std::string_view& view);

	template <>
	_NODISCARD std::optional<std::filesystem::path> StringUtilities::parse<std::filesystem::path>(std::string_view& view);

#ifdef KAPUT_GUID
	template <>
	_NODISCARD std::optional<Id> StringUtilities::parse<Id>(std::string_view& view);
#endif
}
