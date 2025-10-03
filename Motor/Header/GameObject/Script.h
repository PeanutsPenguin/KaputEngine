#pragma once

#define SOL_LUAJIT 1
#define SOL_ALL_SAFETIES_ON 1

#include <filesystem>
#include <vcruntime.h>

#include <sol.hpp>

namespace KaputEngine
{
	namespace Resource
	{
		class ScriptResource;
	}

	class Script
	{
	public:
		Script() = default;
		Script(Resource::ScriptResource& parent);
		Script(const Script&) = delete;
		Script(Script&&) = delete;

		~Script() = default;

		Script& operator=(const Script&) = delete;
		Script& operator=(Script&&) = delete;

		void create(const std::filesystem::path& path);
		void create(const std::string_view& content);

		void destroy();

		void assignEnvironment(sol::environment& env) const;

		_NODISCARD _Ret_maybenull_ Resource::ScriptResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::ScriptResource* parentResource() const noexcept;

	private:
		sol::function m_load;
		Resource::ScriptResource* m_resource = nullptr;
	};
}
