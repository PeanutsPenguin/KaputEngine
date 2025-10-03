#pragma once

#include "Utils/RemoveVector.h"

namespace KaputEngine
{
	enum CollisionState
	{
		E_NO_COLLISION,
		E_ENTER_COLLISION,
		E_COLLIDING,
	};

	struct IPhysicsUpdatable: RemoveVectorStatusSource<IPhysicsUpdatable>
	{
		virtual void updatePhysics(double deltaTime){ };

		virtual void onCollisionEnter(IPhysicsUpdatable* collider){ };

		virtual void onCollision(IPhysicsUpdatable* collider){ };

		virtual void onCollisionExit(){ };

		_NODISCARD std::pair<CollisionState, bool> getState() const noexcept {
			return this->m_state;
		}

		void setCollisionState(CollisionState state)
		{
			this->m_state.first = state;
			this->m_state.second = true;
		}

		std::pair<CollisionState, bool> m_state;
	};
}