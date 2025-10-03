#pragma once

#include "ObjectBase.h"
#include "Scene/Transform/MatrixSource.h"

#include "../Component/Component.h"

#include <sol/sol.hpp>

#define GAMEOBJECT_SIGS(type, base) \
OBJECTBASE_SIGS(type, base) \
friend GameObject; \
public: \
explicit type(ConstructorBlocker); \
type(ConstructorBlocker, const Id& id); \
_NODISCARD std::shared_ptr<type> shared_from_this(); \
_NODISCARD std::weak_ptr<type> weak_from_this(); \
private:

#define GAMEOBJECT_CTOR_IMPL(type, base) \
type::type(ConstructorBlocker) : base(ConstructorBlocker()) { } \
type::type(ConstructorBlocker, Id id) : base(ConstructorBlocker(), id) { } \

#define GAMEOBJECT_IMPL(type) \
OBJECTBASE_IMPL(type) \
std::shared_ptr<type> type::shared_from_this() { return std::static_pointer_cast<type>(GameObject::shared_from_this()); } \
std::weak_ptr<type> type::weak_from_this() { return std::static_pointer_cast<type>(GameObject::weak_from_this().lock()); }

namespace KaputEngine
{
	class Scene;
	class PhysicComponent;

	enum class eDeletePolicy : uint8_t
	{
		DETACH,
		DELETE
	};

	class GameObject :
		public std::enable_shared_from_this<GameObject>,
		public ObjectBase,
		public RemoveVectorStatusSource<GameObject>,
		public MatrixTransformSource
	{
		OBJECTBASE_SIGS(GameObject, ObjectBase)

	protected:
		REGISTER_SIG(GameObject);

	public:
		using ComponentList = RemoveVector<Component, std::shared_ptr<Component>>;

		explicit GameObject(ConstructorBlocker);
		GameObject(ConstructorBlocker, Id id);

		_NODISCARD _Ret_maybenull_ static std::shared_ptr<GameObject> load(const Text::Xml::XmlNode& node);

		template <std::derived_from<GameObject> T, typename... Args>
		_NODISCARD static std::shared_ptr<T> create(Args&&... args);

		_NODISCARD _Success_(return) static std::shared_ptr<GameObject> create(const std::string& typeName);

		_NODISCARD static std::vector<std::string> getTypes();

		~GameObject() override;

		void destroy() override;

		void start() override;

		void onCollision(_In_ PhysicComponent* other) override;

		void onCollisionEnter(_In_ PhysicComponent* other) override;

		void onCollisionExit() override;

		_NODISCARD _Ret_maybenull_ std::shared_ptr<GameObject> findChild(const Id& id) const;

		_NODISCARD const ComponentList& components() const noexcept;

		_NODISCARD bool isRoot() const noexcept;

		template <std::derived_from<Component> T, typename... Args>
		T& addComponent(Args&&... args);

		template <std::derived_from<Component> T>
		_NODISCARD _Success_(return) T* getComponent();

		template <typename T>
		_NODISCARD _Success_(return) const T* getComponent() const;

		void removeComponent(Component& component);

		_NODISCARD _Ret_maybenull_ GameObject* parentObject() const noexcept final;
		_NODISCARD _Ret_maybenull_ Scene* parentScene() const noexcept final;

		_NODISCARD const RemoveVector<GameObject, std::shared_ptr<GameObject>>& children() const noexcept;

		void attachTo(_In_opt_ GameObject* parent, bool adjustWorldTransform = false, eDeletePolicy policy = eDeletePolicy::DELETE);
		void detach();

		void setWorldTransform(const Transform& transform) noexcept override;

		void updateWorldTransform() const noexcept override;

		void setWorldTransformWithoutPhysic(const Transform& transform) noexcept;

		_NODISCARD _Ret_maybenull_ TransformSource* getParentTransform() const override;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		void setPhysicComponentPtr(_In_opt_ PhysicComponent* ptr);

		_NODISCARD _Ret_maybenull_ PhysicComponent* getPhysicComponent();

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

		eDeletePolicy deletePolicy;

	protected:
		ComponentList m_components;
		GameObject* m_parent;
		Scene* m_scene;

		mutable RemoveVectorStatus m_parentStatus;
		RemoveVector<GameObject, std::shared_ptr<GameObject>> m_children;

		bool deserializeMap(const Text::Xml::XmlNode::Map& map) override;

		void switchScene(_In_opt_ Scene* newScene);

		void detach(_In_opt_ Scene* newScene);

		void setTransformDirty() const noexcept override;

	protected:
		template <std::derived_from<GameObject> T>
		static sol::usertype<T> registerLuaType(sol::state& lua);

		template <std::derived_from<GameObject> T>
		static void defineLuaMembers(sol::usertype<T>& type);

	private:
		void registerQueues(Scene& scene) override;
		void unregisterQueues() override;

		RemoveVectorStatus& getParentStatus() const noexcept;

		template <std::derived_from<GameObject> T>
		static void registerCreate();

		template <std::derived_from<GameObject> T>
		static void registerDeserialize();

		using CreateFunc = std::shared_ptr<GameObject>();
		using DeserializeFunc = _Ret_maybenull_ std::shared_ptr<GameObject>(const Text::Xml::XmlNode& node);

		static std::unordered_map<std::string, CreateFunc*> s_createFuncs;
		static std::unordered_map<std::string, DeserializeFunc*> s_deserializeFuncs;

		PhysicComponent* m_physicBody;
	};
}
