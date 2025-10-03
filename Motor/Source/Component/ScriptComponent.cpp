#include "Component/ScriptComponent.hpp"

#include "Application.h"
#include "Component/Component.hpp"
#include "GameObject/GameObject.h"
#include "Resource/Manager.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"

using namespace KaputEngine;
using namespace KaputEngine::Resource;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::string;

COMPONENT_IMPL(ScriptComponent)

ScriptComponent::ScriptComponent(GameObject& parent, std::shared_ptr<ScriptResource>&& script)
	: ScriptComponent(parent, s_nextId++) { }

ScriptComponent::ScriptComponent(GameObject& parent, const Id& id, std::shared_ptr<ScriptResource>&& script)
	: ScriptComponent(parent, id)
{
	m_script = std::move(script);
}

ScriptComponent::ScriptComponent(GameObject& parent) : ScriptComponent(parent, s_nextId++) { }

ScriptComponent::ScriptComponent(GameObject& parent, const Id& id)
	: Component(parent, id),
	m_environment(Application::luaState(), sol::create, Application::luaState().globals())
{
	if (m_script)
	{
		m_script->waitLoad();
		m_script->data().assignEnvironment(m_environment);
	}

	m_startFunc  = m_environment["start"];
	m_updateFunc = m_environment["update"];
	m_onCollisionFunc = m_environment["OnCollision"];
	m_onCollisionEnterFunc = m_environment["OnCollisionEnter"];
	m_onCollisionExitFunc = m_environment["OnCollisionExit"];

	m_environment["gameObject"] = parent.shared_from_this();
}

void ScriptComponent::start()
{
	if (!m_startFunc.valid())
		return;

	const sol::protected_function_result& res = m_startFunc();

	if (!res.valid())
		cerr << __FUNCTION__": " << static_cast<const sol::error&>(res).what() << "\n";
}

void ScriptComponent::update(double deltaTime)
{
	if (!m_updateFunc.valid())
		return;

	const sol::protected_function_result& res = m_updateFunc(deltaTime);

	if (!res.valid())
		cerr << __FUNCTION__": " << static_cast<const sol::error&>(res).what() << "\n";
}

void ScriptComponent::onCollision(_In_ PhysicComponent* other)
{
	if (!m_onCollisionFunc.valid())
		return;

	sol::protected_function_result res = m_onCollisionFunc(other);

	if (!res.valid())
	{
		sol::error err = res;
		cerr << __FUNCTION__": " << err.what() << "\n";
	}
}

void ScriptComponent::onCollisionEnter(_In_ PhysicComponent* other)
{
	if (!m_onCollisionEnterFunc.valid())
		return;

	sol::protected_function_result res = m_onCollisionEnterFunc(other);

	if (!res.valid())
	{
		sol::error err = res;
		cerr << __FUNCTION__": " << err.what() << "\n";
	}
}

void ScriptComponent::onCollisionExit()
{
	if (!m_onCollisionExitFunc.valid())
		return;

	sol::protected_function_result res = m_onCollisionExitFunc();

	if (!res.valid())
	{
		sol::error err = res;
		cerr << __FUNCTION__": " << err.what() << "\n";
	}
}

string ScriptComponent::getPath()
{
	if (this->m_script)
		return this->m_script->path()->string();
	else
		return "no script file";
}

void ScriptComponent::changeScript(const string& path)
{
	if (path == "")
		return;

	this->m_script = ResourceManager::get<ScriptResource>(path);

	m_script->waitLoad();
	m_script->data().assignEnvironment(m_environment);

	m_startFunc  = m_environment["start"];
	m_updateFunc = m_environment["update"];
	m_onCollisionFunc = m_environment["OnCollision"];
	m_onCollisionEnterFunc = m_environment["OnCollisionEnter"];
	m_onCollisionExitFunc = m_environment["OnCollisionExit"];

	m_environment["thisComponent"] = this;
	m_environment["gameObject"]    = parentObject()->shared_from_this();
}

void ScriptComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	if (m_script)
		context.value("Source", *m_script->path());
}

bool ScriptComponent::deserializeMap(const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	if (const auto it = map.find("Source"); it != map.end())
	{
		const std::optional<string> sourceOp = it->second->parse<string>();

		if (!sourceOp)
		{
			cerr << __FUNCTION__ ": Failed to parse Source.\n";
			return false;
		}

		this->changeScript(*sourceOp);
	}

	return true;
}

void ScriptComponent::defineLuaMembers(sol::usertype<ScriptComponent>& type)
{
	Component::defineLuaMembers(type);

	type["lua"] = sol::readonly_property(&ScriptComponent::m_environment);
}

void ScriptComponent::getProperties(_Out_ std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	if (!m_environment.valid())
		return;

	m_environment.for_each([&](const sol::object& key, const sol::object& value)
	{
		const std::string& name = key.as<std::string>();

		switch (value.get_type())
		{
		case sol::type::string:
			defineSolProperty(out, name, value.as<string>());
			break;
		case sol::type::number:
			defineSolProperty(out, name, value.as<float>());
			break;
		case sol::type::boolean:
			defineSolProperty(out, name, value.as<bool>());
			break;
		}
	});

	sol::object obj = m_environment[std::string()];
}
