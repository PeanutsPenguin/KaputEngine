#pragma once

#include "GameObject/ObjectBase.h"

#include "Utils/Pointer.h"

#define COMPONENT_SIGS(type, base) \
OBJECTBASE_SIGS(type, base) \
friend GameObject; \
friend Component; \
friend Scene; \
public: \
_NODISCARD exclusive_weak_ptr<type> exclusive_weak_from_this() noexcept; \
_NODISCARD exclusive_weak_ptr<const type> exclusive_weak_from_this() const noexcept; \
private:

#define COMPONENT_CTORS(type) \
explicit type(GameObject& parent); \
type(GameObject& parent, const Id& id);

#define COMPONENT_IMPL(type) \
OBJECTBASE_IMPL(type) \
KaputEngine::exclusive_weak_ptr<type> type::exclusive_weak_from_this() noexcept \
{ \
	return static_cast<exclusive_weak_ptr<type>>(m_ptr); \
} \
KaputEngine::exclusive_weak_ptr<const type> type::exclusive_weak_from_this() const noexcept \
{ \
	return static_cast<exclusive_weak_ptr<type>>(m_ptr); \
}

namespace KaputEngine
{
	class Component:
		public ObjectBase,
		public RemoveVectorStatusSource<Component>
	{
		COMPONENT_SIGS(Component, ObjectBase)

	protected:
		REGISTER_SIG(Component);

	public:
		COMPONENT_CTORS(Component)

		_NODISCARD static std::vector<std::string> getTypes();

		_Success_(return) static Component* create(const std::string& typeName, GameObject& parent);
		_Ret_maybenull_ static Component* load(const Text::Xml::XmlNode& node, GameObject& parent);

		_NODISCARD _Ret_notnull_ GameObject* parentObject() const noexcept final;
		_NODISCARD _Ret_maybenull_ Scene* parentScene() const noexcept final;

		void destroy() override;

	protected:
		template <std::derived_from<Component> T>
		static sol::usertype<T> registerLuaType(sol::state& lua);

		template <std::derived_from<Component> T>
		static void defineLuaMembers(sol::usertype<T>& type);

		GameObject& m_parentObject;

		exclusive_weak_ptr<Component> m_ptr;

	private:
		using CreateFunc = Component&(GameObject& parent);
		using DeserializeFunc = _Ret_maybenull_ Component*(const Text::Xml::XmlNode::Map& map, GameObject& parent);

		template <std::derived_from<Component> T>
		static void registerCreate();

		template <std::derived_from<Component> T>
		static void registerDeserialize();

		static std::unordered_map<std::string, CreateFunc*> s_createFuncs;
		static std::unordered_map<std::string, DeserializeFunc*> s_deserializeFuncs;
	};
}
