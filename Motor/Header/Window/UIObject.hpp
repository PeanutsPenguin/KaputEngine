#pragma once

#include "Window/UIObject.h"

#include "GameObject/ObjectBase.hpp"
#include "Window/VirtualWindow.h"

namespace KaputEngine
{
	REGISTER_IMPL(UIObject,
	if constexpr (!std::is_abstract_v<T>)
		T::template registerCreate<T>();
	if constexpr (!std::is_abstract_v<T>)
		T::template registerDeserialize<T>();
	T::template registerLuaType<T>(state))


	template <std::derived_from<UIObject> T, typename... Args>
	std::shared_ptr<T> UIObject::create(Args&&... args)
	{
		return std::make_shared<T>(ConstructorBlocker(), std::forward<Args>(args)...);
	}

	template<std::derived_from<UIObject> T>
	void UIObject::registerCreate()
	{
		s_createFuncs[T::TypeName] = []() -> std::shared_ptr<UIObject>
		{
			return UIObject::create<T>();
		};
	}

	template <std::derived_from<UIObject> T>
	void UIObject::registerDeserialize()
	{
		using Text::Xml::XmlNode;
		using std::cerr;

		static constexpr const char* Context = __FUNCTION__;

		s_deserializeFuncs[T::TypeName] = [](const XmlNode& node) -> std::shared_ptr<UIObject>
		{
			std::shared_ptr<T> obj;

			const std::optional<XmlNode::Map> mapOp = node.toMap();

			if (!mapOp)
			{
				cerr << Context << ": Failed to deserialize " << T::TypeName << ".\n";
				return nullptr;
			}

			const XmlNode::Map& map = *mapOp;

			std::optional<Id> idOp = deserializeId(map);

			if (!idOp)
			{
				cerr << Context << ": Failed to deserialize Id.\n";
				return nullptr;
			}

			obj = UIObject::create<T>(*idOp);

			return obj->deserializeMap(map) ? obj : nullptr;
		};
	}

	template <std::derived_from<UIObject> T>
	sol::usertype<T> UIObject::registerLuaType(sol::state& lua)
	{
		sol::usertype<T> type;

		if constexpr (std::is_abstract_v<T>)
		{
			type = lua.globals().new_usertype<T>(
			T::TypeName,
			sol::base_classes, sol::bases<typename T::Base>());
		}
		else
		{
			type = lua.globals().new_usertype<T>(
			T::TypeName,
			sol::factories(&UIObject::create<T>),
			sol::base_classes, sol::bases<typename T::Base>());
		}

		T::defineLuaMembers(type);

		return type;
	}

	template <std::derived_from<UIObject> T>
	void UIObject::defineLuaMembers(sol::usertype<T>& type)
	{
		ObjectBase::defineLuaMembers(type);

		type["position"] = sol::property(
			[](UIObject& obj) -> LibMath::Vector2f
			{
				return obj.m_position.splice<2>();
			},
				[](UIObject& obj, const LibMath::Vector2f& pos)
			{
				LibMath::Vector2f ratio = pos / obj.getWindowSize();
				obj.m_position = { pos.x(), pos.y(), ratio.x(), ratio.y() };
			}
		);

		type["name"] = sol::readonly_property(&ObjectBase::name);
	}
}

