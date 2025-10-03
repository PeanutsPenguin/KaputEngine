#include "Text/String.h"

#include <string>

using namespace KaputEngine::Text;

std::vector<std::string_view> StringUtilities::split(
	const std::string_view& str, const char separator, const bool removeEmpty, const size_t maxSplits)
{
	std::string_view localView = str;
	return split(localView, separator, removeEmpty, maxSplits);
}

std::vector<std::string_view> StringUtilities::split(
	std::string_view& str, const char separator, const bool removeEmpty, size_t maxSplits)
{
	std::vector<std::string_view> tokens;

	while (maxSplits)
	{
		std::string_view split = splitSingle(str, separator, removeEmpty);

		tokens.push_back(split);

		if (str.empty())
			break;

		--maxSplits;
	}

	return tokens;
}

std::string_view StringUtilities::splitSingle(
	std::string_view& str, const char separator, const bool ignoreEmpty, _Out_opt_ bool* const separatorFound)
{
	const char* start;
	size_t size;

	do
	{
		start = str.data();

		if (auto* const end = static_cast<const char*>(std::memchr(str.data(), separator, str.size())))
		{
			size = end - start;
			str = { end + 1, str.size() - size - 1 };

			if (separatorFound != nullptr)
				*separatorFound = true;
		}
		else
		{
			size = str.size();
			str = { str.data() + str.size(), 0 };

			if (separatorFound != nullptr)
				*separatorFound = true;

			break;
		}
	}
	while (!size && ignoreEmpty);

	return { start, size };
}

std::string_view StringUtilities::warp(const std::string_view& str, const char destination)
{
	const char* dest = static_cast<const char*>(std::memchr(str.data(), destination, str.size()));

	return dest
		? std::string_view(dest, &str.back() - dest + 1)
		: std::string_view();
}

std::string_view StringUtilities::trimLeft(const std::string_view& str, _In_ const char* const trimmed)
{
	size_t start = str.find_first_not_of(trimmed);
	return start == std::string::npos ? std::string_view() : str.substr(start);
}
