#include "Physics/PhysicHandler.h"

#include "Component/PhysicComponent.h"
#include "Physics/PrivateBulletWrapper.h"
#include "Utils/RemoveVector.hpp"

#include <btBulletDynamicsCommon.h>

using namespace KaputEngine;

using LibMath::Vector3f;

PhysicHandler::PhysicHandler() : m_physicsQueue(IPhysicsUpdatable::createRemoveVector())
{
	this->m_handle = new PhysicHandlerImpl;

	///btCollisionConfiguration allows to configure Bullet collision detection
	this->m_handle->m_config = new btDefaultCollisionConfiguration;

	///btCollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
	///Time of Impact, Closest Points and Penetration Depth.
	this->m_handle->m_dispatcher = new btCollisionDispatcher(this->m_handle->m_config);

	///The btBroadphaseInterface class provides an interface to detect aabb-overlapping object pairs.
	this->m_handle->m_interface = new btDbvtBroadphase;

	///The btSequentialImpulseConstraintSolver is a fast SIMD implementation of the Projected Gauss Seidel (iterative LCP) method.
	//Weird
	this->m_handle->m_solver = new btSequentialImpulseConstraintSolver;

	///btDiscreteDynamicsWorld provides discrete rigid body simulation
	//Keep that
	this->m_handle->m_world = new btDiscreteDynamicsWorld(this->m_handle->m_dispatcher, this->m_handle->m_interface, this->m_handle->m_solver, this->m_handle->m_config);
}

void PhysicHandler::setGravity(const Vector3f& grav)
{
	this->m_handle->m_world->setGravity({ grav.x(), grav.y(), grav.z() });
}

void PhysicHandler::addToWorld(RigidBody* toAdd)
{
	this->m_handle->m_world->addRigidBody(toAdd->m_btBody);
}

void PhysicHandler::removeFromWorld(RigidBody* toRemove)
{
	this->m_handle->m_world->removeRigidBody(toRemove->m_btBody);
}

void PhysicHandler::updateSimulation(const double deltaTime)
{
	this->m_handle->m_world->stepSimulation(static_cast<btScalar>(deltaTime), 1, m_targetFrameTime);
}

void PhysicHandler::updatePhysics(double deltaTime)
{
	m_accumulator += deltaTime;

	while (m_accumulator >= m_targetFrameTime) {
		this->updateSimulation(m_targetFrameTime);
		this->checkWorldCollisions();

		for (IPhysicsUpdatable& physicObj : m_physicsQueue)
			physicObj.updatePhysics(deltaTime);

		m_accumulator -= m_targetFrameTime;
	}
}

void PhysicHandler::addCollisionsTag(const std::string& name)
{
	if (name == "")
		return;

	this->m_collisionTag[name] =  this->m_nextId;
	this->m_nextId++;
}

void PhysicHandler::removeTag(const std::string& name)
{
	unsigned int KaputFlag = this->m_collisionTag.find(name)->second;

	for (int i = this->m_handle->m_world->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = this->m_handle->m_world->getCollisionObjectArray()[i];

		if (btRigidBody* body = btRigidBody::upcast(obj); body && body->getMotionState())
		{
			PhysicComponent* comp = static_cast<PhysicComponent*>(body->getUserPointer());
			unsigned int btFlag = this->m_collisionTag.find(comp->getTag())->second;

			if (btFlag == KaputFlag)
			{
				PhysicComponent* comp = static_cast<PhysicComponent*>(body->getUserPointer());
				comp->setTag("None");
			}
		}
	}

	this->m_collisionTag.erase(name);
}

std::unordered_map<std::string, unsigned int>& PhysicHandler::getTags()
{
	return this->m_collisionTag;
}

const std::unordered_map<std::string, unsigned int>& KaputEngine::PhysicHandler::getTags() const noexcept
{
	return this->m_collisionTag;
}

void PhysicHandler::destroy()
{
	//remove the rigidBodies from the dynamics world and delete them
	for (int i = this->m_handle->m_world->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = this->m_handle->m_world->getCollisionObjectArray()[i];

		if (btRigidBody* body = btRigidBody::upcast(obj); body && body->getMotionState())
			delete body->getMotionState();

		this->m_handle->m_world->removeCollisionObject(obj);
		delete obj;
	}

	delete this->m_handle->m_world;
	delete this->m_handle->m_solver;
	delete this->m_handle->m_interface;
	delete this->m_handle->m_dispatcher;
	delete this->m_handle->m_config;
	delete this->m_handle;
}

void PhysicHandler::checkWorldCollisions()
{
	for (IPhysicsUpdatable& physicObj : m_physicsQueue)
		physicObj.m_state.second = false;

	int numManifolds = this->m_handle->m_dispatcher->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = this->m_handle->m_world->getDispatcher()->getManifoldByIndexInternal(i);

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		PhysicComponent* component1 = static_cast<PhysicComponent*>(obA->getCollisionShape()->getUserPointer());
		PhysicComponent* component2 = static_cast<PhysicComponent*>(obB->getCollisionShape()->getUserPointer());
		bool isColliding = obA->checkCollideWith(obB);

		if (isColliding)
		{
			this->updateCollision(component1, component2);
			this->updateCollision(component2, component1);
		}
	}

	for (IPhysicsUpdatable& physicObj : m_physicsQueue)
	{
		if (physicObj.m_state.second)
			continue;

		if (physicObj.m_state.first == E_COLLIDING)
		{
			physicObj.m_state.first = E_NO_COLLISION;
			physicObj.onCollisionExit();
		}
	}
}

void PhysicHandler::updateCollision(PhysicComponent* comp, PhysicComponent* comp2)
{
	switch (comp->getState().first)
	{
	case E_NO_COLLISION:
		comp->setCollisionState(E_ENTER_COLLISION);
		comp->onCollisionEnter(comp2);
		break;
	case E_ENTER_COLLISION:
		comp->setCollisionState(E_COLLIDING);
		comp->onCollision(comp2);
		break;
	case E_COLLIDING:
		comp->setCollisionState(E_COLLIDING);
		comp->onCollision(comp2);
		break;
	}
}
