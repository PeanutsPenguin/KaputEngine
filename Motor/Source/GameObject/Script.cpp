#include "GameObject/Script.h"

#include "Application.h"

using KaputEngine::Application;
using KaputEngine::Script;
using KaputEngine::Resource::ScriptResource;

Script::Script(ScriptResource& parent) : m_resource(&parent) { }

void Script::create(const std::filesystem::path& path)
{
	m_load = Application::luaState().load_file(path.string());
}

void Script::create(const std::string_view& content)
{
	m_load = Application::luaState().load(content);
}

void Script::destroy()
{
	m_load = { };
}

void Script::assignEnvironment(sol::environment& env) const
{
	env.set_on(m_load);
	m_load();
}

_Ret_maybenull_ ScriptResource* Script::parentResource() noexcept
{
	return m_resource;
}

_Ret_maybenull_ const ScriptResource* Script::parentResource() const noexcept
{
	return m_resource;
}
