#pragma once

#include "GameObject/GameObject.h"

#include "Component/PhysicComponent.h"
#include "GameObject/ObjectBase.hpp"

namespace KaputEngine
{
	REGISTER_IMPL(GameObject,
	if constexpr (!std::is_abstract_v<T>)
		T::template registerCreate<T>();
	if constexpr (!std::is_abstract_v<T>)
		T::template registerDeserialize<T>();
	T::template registerLuaType<T>(state))

	template <std::derived_from<GameObject> T, typename... Args>
	std::shared_ptr<T> GameObject::create(Args&&... args)
	{
		return std::make_shared<T>(ConstructorBlocker(), std::forward<Args>(args)...);
	}

	template<std::derived_from<GameObject> T>
	void GameObject::registerCreate()
	{
		s_createFuncs[T::TypeName] = []() -> std::shared_ptr<GameObject>
		{
			return GameObject::create<T>();
		};
	}

	template <std::derived_from<GameObject> T>
	void GameObject::registerDeserialize()
	{
		using Text::Xml::XmlNode;
		using std::cerr;

		static constexpr const char* Context = __FUNCTION__;

		s_deserializeFuncs[T::TypeName] = [](const XmlNode& node) -> std::shared_ptr<GameObject>
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

			obj = GameObject::create<T>(*idOp);

			return obj->deserializeMap(map) ? obj : nullptr;
		};
	}

	template <std::derived_from<GameObject> T>
	sol::usertype<T> GameObject::registerLuaType(sol::state& lua)
	{
		sol::usertype<T> type = lua.globals().new_usertype<T>(
			T::TypeName,
			sol::factories(&GameObject::create<T>),
			sol::base_classes, sol::bases<typename T::Base>());

		T::defineLuaMembers(type);

		return type;
	}

	template <std::derived_from<GameObject> T>
	void GameObject::defineLuaMembers(sol::usertype<T>& type)
	{
		ObjectBase::defineLuaMembers(type);

		type["removeComponent"] = &removeComponent;

		type["getWorldPosition"] = [](GameObject& obj) -> LibMath::Vector3f
		{
			return obj.getWorldTransform().position;
		};
		type["getWorldScale"] = [](GameObject& obj) -> LibMath::Vector3f
		{
			return obj.getWorldTransform().scale;
		};

		type["setLocalPosition"] = [](GameObject& obj, const LibMath::Vector3f& pos)
		{
			obj.setLocalPosition(pos);
		};
		type["setLocalScale"] = [](GameObject& obj, const LibMath::Vector3f& scale)
		{
			obj.setLocalScale(scale);
		};

		type["attachTo"] = [](GameObject& obj, GameObject& parent)
		{
			obj.attachTo(&parent, true);
		};
		type["detach"] = (void (GameObject::*)())&detach;
	}

	template <std::derived_from<Component> T, typename... Args>
	T& GameObject::addComponent(Args&&... args)
	{
		std::shared_ptr<T> component = std::static_pointer_cast<T>(
			m_components.push_back(std::make_shared<T>(*this, std::forward<Args>(args)...)));

		component->m_ptr = static_cast<std::weak_ptr<Component>>(component);

		if (m_scene)
			component->registerQueues(*m_scene);

		if (std::shared_ptr<PhysicComponent> castPtr = std::dynamic_pointer_cast<PhysicComponent>(component); castPtr)
		{
			castPtr->setTransform(this->m_worldTransform);
			this->m_physicBody = std::to_address(castPtr);
		}

		if (m_started)
			component->start();

		return *component;
	}

	template <std::derived_from<Component> T>
	_Success_(return) T* GameObject::getComponent()
	{
		for (auto& component : m_components)
			if (T* castPtr = dynamic_cast<T*>(&component); castPtr)
				return castPtr;

		return nullptr;
	}

	template <typename T>
	_Success_(return) const T* GameObject::getComponent() const
	{
		return const_cast<GameObject*>(this)->getComponent<T>();
	}
}
