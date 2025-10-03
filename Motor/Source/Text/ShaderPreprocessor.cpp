#include "Text/ShaderPreprocessor.h"

#include "Text/String.h"

#include <fstream>
#include <iostream>

using namespace KaputEngine::Text;

_Success_(return) std::optional<std::string> ShaderPreprocessor::openFile(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::in);

	if (!file.is_open())
	{
		std::cerr << "Failed to open shader source file " << path << ".\n";
		return {};
	}

	std::string content(std::istreambuf_iterator(file), {});
	file.close();

	return content;
}

_Success_(return) bool ShaderPreprocessor::addFile(const std::filesystem::path& path)
{
	return addFile(path, true);
}

const std::string& ShaderPreprocessor::result()
{
	return m_result;
}

_Success_(return) bool ShaderPreprocessor::addFile(const std::filesystem::path& inclusion, const bool root)
{
	// Check if the file has already been included
	for (const std::filesystem::path& path : m_includedPaths)
		if (std::filesystem::equivalent(path, inclusion))
			return true;

	m_includedPaths.emplace_back(inclusion);

	std::optional<std::string> shaderSource = openFile(inclusion);
	if (!shaderSource)
		return false;

	// Can be optimized by splitting lines progressively
	// Keep segments of the file whole until the preprocessor token
	std::vector<std::string_view> lines = StringUtilities::split(shaderSource.value(), '\n', true);

	for (std::string_view& line : lines)
	{
		if (line.starts_with("//? "))
		{
			if (root)
				line = line.substr(3);
			else
				continue;
		}

		if (line.starts_with("#include"))
		{
			size_t
				start = line.find('\"'),
				end = line.find('\"', start + 1);

			if (start == std::string::npos || end == std::string::npos)
			{
				std::cerr << "Invalid #include directive in shader source file " << inclusion << ".\n";
				return false;
			}

			std::string_view newInclusion = line.substr(start + 1, end - start - 1);
			std::filesystem::path newIncludePath = newInclusion;

			if (newInclusion.starts_with('.'))
				newIncludePath = inclusion.parent_path() / newIncludePath;

			if (!addFile(newIncludePath, false))
				return false;
		}
		else
			m_result += std::string(line) + '\n';
	}

	return true;
}
