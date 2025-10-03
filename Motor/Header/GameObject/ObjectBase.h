#pragma once

#include "IWorldUpatable.h"
#include "Inspector/Property.h"
#include "RegistryDefs.h"
#include "Text/Xml/Node.h"
#include "Text/Xml/Parser.h"
#include "Text/Xml/Serializer.h"

#include "Id.h"

#include <sol/sol.hpp>
#include <vcruntime.h>

#define OBJECTBASE_SIGS(type, base) \
public: \
static constexpr const char* TypeName = #type; \
using Base = base; \
_NODISCARD constexpr const char* typeName() const noexcept override; \
private:

#define OBJECTBASE_IMPL(type) \
constexpr const char* type::typeName() const noexcept { return type::TypeName; } \

#define OBJECTBASE_REGISTER_FINAL_IMPL(type) \
REGISTER_FINAL_IMPL(type, registerLuaType<type>(state))

#define OBJECTBASE_DEFAULT_REGISTER(type) \
protected: \
REGISTER_SIG(type) \
{ \
	type::Base::registerType<T>(state); \
} \
private:

namespace KaputEngine
{
    class GameObject;
    class Scene;
    class Registry;
    class PhysicComponent;

	class ObjectBase :
		public IWorldUpdatable,
		protected Text::Xml::IXmlMapParser,
		protected Text::Xml::IXmlPolymorphicSerializer
	{
		friend Scene;

		REGISTER_SIG(ObjectBase);

	public:
		static constexpr const char* TypeName = "ObjectBase";

		_NODISCARD virtual constexpr const char* typeName() const noexcept;

		_NODISCARD bool getCanUpdate() const noexcept;
		void setCanUpdate(bool canUpdate) noexcept;

		_NODISCARD _Ret_maybenull_ virtual GameObject* parentObject() const noexcept = 0;
		_NODISCARD _Ret_maybenull_ virtual Scene* parentScene() const noexcept = 0;

		_NODISCARD const Id& id() const noexcept;

		void rename(_In_ const char* name);

		virtual void start();

        virtual void destroy();

        virtual void onCollision(_In_ PhysicComponent* other){ };

        virtual void onCollisionEnter(_In_ PhysicComponent* other){ };

        virtual void onCollisionExit(){ };

		_NODISCARD const char* xmlTypeName() const noexcept final;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const override;

		virtual void getProperties(std::vector<Inspector::Property>& out) noexcept;

		std::string name;

	protected:
		struct ConstructorBlocker
		{
			explicit ConstructorBlocker() = default;
		};

		ObjectBase();
		ObjectBase(const Id& id);

		virtual void registerQueues(Scene& scene);
		virtual void unregisterQueues();

		template <std::derived_from<ObjectBase> T = ObjectBase>
		static sol::usertype<T> registerLuaType(sol::state& lua);

		template <std::derived_from<ObjectBase> T>
		static void defineLuaMembers(sol::usertype<T>& type);

		_NODISCARD static sol::state& getAppLuaState() noexcept;

		_NODISCARD _Success_(return) virtual bool deserializeMap(const Text::Xml::XmlNode::Map& map);

		bool validateStart() const noexcept;

		_NODISCARD _Success_(return) static std::optional<Id> deserializeId(const Text::Xml::XmlNode::Map& map);

		void deserializeError(_In_ const char* context, _In_opt_ const char* value = nullptr) const noexcept;
		static void deserializeErrorStatic(_In_ const char* context, _In_ const char* value);

		bool
			m_started   = false,
			m_canUpdate = true;

		Id m_id;
		static Id s_nextId;

	private:
		void registerUpdate(Scene& scene);
		void unregisterUpdate();
	};
}
