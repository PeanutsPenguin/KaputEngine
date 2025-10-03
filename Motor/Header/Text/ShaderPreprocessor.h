#pragma once

#include <filesystem>

namespace KaputEngine::Text
{
    class ShaderPreprocessor
    {
    public:
		ShaderPreprocessor() = default;
		ShaderPreprocessor(const ShaderPreprocessor&) = delete;
		ShaderPreprocessor(ShaderPreprocessor&&) = delete;

		_NODISCARD _Success_(return) std::optional<std::string> openFile(const std::filesystem::path& path);
		_NODISCARD _Success_(return) bool addFile(const std::filesystem::path& path);

        const std::string& result();

    private:
        _NODISCARD _Success_(return) bool addFile(const std::filesystem::path& path, bool root);

        std::string m_result;
        std::vector<std::filesystem::path> m_includedPaths;
    };
}
