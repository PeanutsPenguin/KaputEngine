#pragma once

#include "IPhysicsUpdatable.h"

#include <LibMath/Vector/Vector3.h>

#include <unordered_map>

constexpr float TARGET_FPS = 60.f;

namespace KaputEngine
{
	enum ePhysicShape : uint8_t
	{
		E_NO_SHAPE,
		E_BOX_SHAPE,
		E_SPHERE_SHAPE,
		E_CAPSULE_SHAPE
	};

	class PhysicHandler : public IPhysicsUpdatable
	{
		friend class PhysicComponent;

	public:
		PhysicHandler();

		PhysicHandler(const PhysicHandler&) = delete;
		PhysicHandler(PhysicHandler&&) = delete;

		void setGravity(const LibMath::Vector3f& grav);

		void addToWorld(class RigidBody* toAdd);

		void removeFromWorld(class RigidBody* toRemove);

		void updateSimulation(double deltaTime);

		void updatePhysics(double deltaTime) override;

		void addCollisionsTag(const std::string& name);

		void removeTag(const std::string& name);

		std::unordered_map<std::string, unsigned int>& getTags();

		const std::unordered_map<std::string, unsigned int>& getTags() const noexcept;

		void destroy();

		~PhysicHandler() = default;

	private:
		std::unordered_map<std::string, unsigned int> m_collisionTag;
		unsigned int m_nextId = 1;
		RemoveVector<IPhysicsUpdatable> m_physicsQueue;
		struct PhysicHandlerImpl* m_handle;

		double m_targetFrameTime = 1 / TARGET_FPS;
		double m_accumulator = 0;

		void checkWorldCollisions();

		void updateCollision(PhysicComponent* comp, PhysicComponent* comp2);
	};
}
