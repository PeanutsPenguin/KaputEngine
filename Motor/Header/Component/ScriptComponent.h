#pragma once

#include "Component/Component.h"

#include "Resource/Script.h"

namespace KaputEngine
{
	class ScriptComponent : public Component
	{
		COMPONENT_SIGS(ScriptComponent, Component)
		OBJECTBASE_DEFAULT_REGISTER(ScriptComponent)

	public:
		using FunctionType = void();

		COMPONENT_CTORS(ScriptComponent)
		ScriptComponent(GameObject& parent, std::shared_ptr<Resource::ScriptResource>&& script);
		ScriptComponent(GameObject& parent, const Id& id, std::shared_ptr<Resource::ScriptResource>&& script);

		~ScriptComponent() override = default;

		void start() override;

		void update(double deltaTime) override;

		void onCollision(_In_ PhysicComponent* other) override;

		void onCollisionEnter(_In_ PhysicComponent* other) override;

		void onCollisionExit() override;

		_NODISCARD std::string getPath();

		void changeScript(const std::string& path);

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		void getProperties(_Out_ std::vector<Inspector::Property>& out) noexcept override;

	protected:
		bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

	private:
		template <typename T>
		void defineSolProperty(
			_Out_ std::vector<Inspector::Property>& out, const std::string& name, const T& value) noexcept;

		static void defineLuaMembers(sol::usertype<ScriptComponent>& type);

		std::shared_ptr<Resource::ScriptResource> m_script;
		sol::environment m_environment;
		sol::function m_startFunc;
		sol::function m_updateFunc;
		sol::function m_onCollisionFunc;
		sol::function m_onCollisionExitFunc;
		sol::function m_onCollisionEnterFunc;
	};
}
