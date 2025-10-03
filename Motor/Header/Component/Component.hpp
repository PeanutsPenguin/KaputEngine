#pragma once

#include "Application.h"
#include "Component.h"
#include "GameObject/GameObject.hpp"

namespace KaputEngine
{
	REGISTER_IMPL(Component,
	if constexpr (!std::is_abstract_v<T>)
		T::template registerCreate<T>();
	if constexpr (!std::is_abstract_v<T>)
		T::template registerDeserialize<T>();
	T::template registerLuaType<T>(state))

	template <std::derived_from<Component> T>
	void Component::registerCreate()
	{
		s_createFuncs[T::TypeName] = [](GameObject& parent) -> Component&
		{
			return parent.addComponent<T>();
		};
	}

	template <std::derived_from<Component> T>
	void Component::registerDeserialize()
	{
		s_deserializeFuncs[T::TypeName] = [](const Text::Xml::XmlNode::Map& map, GameObject& parent)
			-> Component*
		{
			std::optional<Id> idOp = deserializeId(map);

			if (!idOp)
			{
				std::cerr << __FUNCTION__": Failed deserialize " << T::TypeName << ".\n";
				return nullptr;
			}

			T& comp = parent.addComponent<T>(*idOp);

			if (comp.deserializeMap(map))
				return &comp;
			else
			{
				parent.removeComponent(comp);
				return nullptr;
			}
		};
	}

	template <std::derived_from<Component> T>
	sol::usertype<T> Component::registerLuaType(sol::state& lua)
	{
		sol::usertype<T> type = lua.globals().new_usertype<T>(
			T::TypeName,
			sol::no_constructor,
			sol::base_classes, sol::bases<typename T::Base>());

		T::defineLuaMembers(type);

		return type;
	}

	template <std::derived_from<Component> T>
	void Component::defineLuaMembers(sol::usertype<T>& type)
	{
		ObjectBase::defineLuaMembers(type);

		auto gameObjectType = Application::luaState().globals()["GameObject"];
		/*gameObjectType[std::format("add{}", T::TypeName).c_str()] = &GameObject::addComponent<T>;*/
		gameObjectType[std::format("get{}", T::TypeName).c_str()] = (T* (GameObject::*)())(&GameObject::getComponent<T>);
	}
}
