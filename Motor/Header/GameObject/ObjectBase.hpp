#pragma once

#include "GameObject.h"
#include "GameObject/ObjectBase.h"
#include "Inspector/Property.hpp"

#include <format>
#include <sol.hpp>

namespace KaputEngine
{
	REGISTER_IMPL(ObjectBase,
		T::template registerLuaType<T>(state))

	template <std::derived_from<ObjectBase> T>
	sol::usertype<T> ObjectBase::registerLuaType(sol::state& lua)
	{
		sol::usertype<T> type;

		if constexpr (std::same_as<T, ObjectBase>)
			type = lua.globals().new_usertype<T>(T::TypeName, sol::no_constructor);
		else
			type = lua.globals().new_usertype<T>(T::TypeName, sol::no_constructor, sol::base_classes, sol::bases<typename T::Base>());

		T::defineLuaMembers(type);

		return type;
	}

	template <std::derived_from<ObjectBase> T>
	void ObjectBase::defineLuaMembers(sol::usertype<T>& type)
	{
		type["id"]               = sol::readonly_property(&id);
		type["name"]		     = &T::name;
		type["typeName"]         = sol::readonly_property(&T::typeName);
		type["declaredTypeName"] = sol::var(std::ref(T::TypeName));
		type["parentObject"]     = sol::readonly_property(&parentObject);
		type["canUpdate"]        = sol::property(&getCanUpdate, &setCanUpdate);
		type["destroy"]          = &destroy;

		auto objBaseType = getAppLuaState().globals()["ObjectBase"];

		objBaseType[std::format("castTo{}", T::TypeName)] = [](ObjectBase* obj) -> T*
		{
			return dynamic_cast<T*>(obj);
		};
	}
}
