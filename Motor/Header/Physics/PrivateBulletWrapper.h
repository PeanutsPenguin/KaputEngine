#pragma once

class btRigidBody;
class btCollisionShape;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;

namespace KaputEngine
{
	struct RigidBody { btRigidBody* m_btBody; };

	struct CollisionShape { btCollisionShape* m_btShape; };

    struct PhysicHandlerImpl
    {
        btDiscreteDynamicsWorld* m_world;
        btSequentialImpulseConstraintSolver* m_solver;
        btBroadphaseInterface* m_interface;
        btCollisionDispatcher* m_dispatcher;
        btDefaultCollisionConfiguration* m_config;
    };
}