#pragma once

#include "Component/Component.h"

#include "IPhysicsUpdatable.h"
#include "Physics/PhysicHandler.h"
#include "Rendering/ShaderProgram.h"
#include "Scene/Scene.h"
#include "Scene/Transform/Transform.h"
#include "Text/Xml/Context.h"

#define SECTORS 20
#define STACKS 15

namespace KaputEngine
{
	class PhysicComponent :
		public Component,
		public IPhysicsUpdatable
	{
		COMPONENT_SIGS(PhysicComponent, Component)
		OBJECTBASE_DEFAULT_REGISTER(PhysicComponent)

		friend class PhysicHandler;
		friend class GameObject;

	public:
		COMPONENT_CTORS(PhysicComponent)
		PhysicComponent(GameObject& parent, ePhysicShape shape, float mass);
		PhysicComponent(GameObject& parent, const Id& id, ePhysicShape shape, float mass);

		~PhysicComponent() override;

		void destroy() override;

		void setTransform(const Transform& trans);

		void setMass(float mass);

		void setScale(const LibMath::Vector3f& size);

		_NODISCARD float getFriction() const noexcept;

		_NODISCARD std::bitset<3>& getFreezeTranslation() noexcept;
		_NODISCARD const std::bitset<3>& getFreezeTranslation() const noexcept;

		_NODISCARD std::bitset<3>& getFreezeRotation() noexcept;
		_NODISCARD const std::bitset<3>& getFreezeRotation() const noexcept;

		void setFreezeTranslation(int axe, bool value);

		void setFreezeRotation(int axe, bool value);

		void setBounciness(float value);

		_NODISCARD float getBounciness();

		void setRigidBodyPosition(const LibMath::Vector3f& vec);

		void resetVelocity();

		void setFriction(float fric);

		_NODISCARD ePhysicShape getShape();

		void setShape(ePhysicShape shape);

		void updatePhysics(double deltaTime) override;

		void render(const Camera& camera);

		void renderRightForm();

		_NODISCARD float getMass() const noexcept;

		_NODISCARD LibMath::Vector3f getScale() const noexcept;

		void setCanPhysicUpdate(bool update);

		void start() override;

		bool getCanPhysicUpdate();

		void addImpulse(const LibMath::Vector3f force);

		void addForce(const LibMath::Vector3f force);

		void setPushVelocity(const LibMath::Vector3f force);

		void setLinearVelocity(const LibMath::Vector3f force);

		_NODISCARD LibMath::Vector3f getLinearVelocity() const;

		_NODISCARD bool compareTag(const std::string& tag);

		_NODISCARD std::string getTag();

		void setTag(const std::string& tag);

		void getProperties(std::vector<Inspector::Property>& out) noexcept override;

	protected:
		void registerPhysics(Scene& scene);

		void unregisterPhysics(Scene& scene);

		void registerQueues(Scene& scene) override;

		void unregisterQueues() override;

		void serializeValues(Text::Xml::XmlSerializeContext& context) const;

	private:
		void drawBoxShapeMesh();

		void drawSphereShapeMesh();

		void drawCapsuleShapeMesh();

		void getRightShape(ePhysicShape shape, const LibMath::Vector3f& size);

		void onCollisionEnter(_In_ IPhysicsUpdatable* collider) override;

		void onCollision(_In_ IPhysicsUpdatable* collider) override;

		void onCollisionExit() override;

		static void defineLuaMembers(sol::usertype<PhysicComponent>& type);

		_NODISCARD _Success_(return) static std::optional<ePhysicShape> parseShape(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<float> parseMass(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<float> parseFriction(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<std::string> parseTag(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parsePhysicUpdate(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeTranslationX(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeTranslationY(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeTranslationZ(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeRotationX(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeRotationY(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<bool> parseFreezeRotationZ(const Text::Xml::XmlNode::Map& map);
		_NODISCARD _Success_(return) static std::optional<LibMath::Vector3f> parseScale(const Text::Xml::XmlNode::Map& map);

		_NODISCARD _Success_(return) bool deserializeMap(_In_ const Text::Xml::XmlNode::Map& map) override;

		std::unique_ptr<struct RigidBody> m_body;
		std::unique_ptr<struct CollisionShape> m_collisionShape;
		std::shared_ptr<const KaputEngine::Rendering::ShaderProgram> m_program;
		std::string m_collisionTag;
		std::bitset<3> m_fixRotation;
		std::bitset<3> m_fixTranslation;
		ePhysicShape m_shape;
		bool m_physicUpdate;
	};
}
