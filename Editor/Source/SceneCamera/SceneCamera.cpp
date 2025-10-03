#include "SceneCamera/SceneCamera.h"

#include "Application.h"

using namespace LibMath;
using namespace KaputEditor;
using namespace KaputEngine;

SceneCamera::SceneCamera()
{
	this->m_cam = GameObject::create<Camera>();
	this->m_cam->setLocalPosition({ 0, 1, 3 });
}

void SceneCamera::updateVision(const double deltaTime)
{
	InputManager& inputs = Application::getInputManager();

	if (inputs.isKeyDown(KEY_MOUSE_RIGHT_BUTTON))
	{
		const Transform& transform = this->m_cam->getLocalTransform();

		const float speedDelta = static_cast<float>(movementSpeed * deltaTime);

		const Vector2f mouseDelta = inputs.mouseDelta() * sensitivity;

		float pitch = this->m_cam->getPitch();

		///our angle is equal to our offset on the screen
		this->m_cam->setYaw(this->m_cam->getYaw() + mouseDelta.x());
		pitch += mouseDelta.y();

		///Fix the rotation between 0 and 90
		if (pitch > 89.0f)
		    pitch = 89.0f;
		if (pitch < -89.0f)
		    pitch = -89.0f;

		this->m_cam->setPitch(pitch);

		Cartesian3f newPos = transform.position;

		if (inputs.isKeyDown(KEY_W))
			newPos += this->m_cam->getFront() * speedDelta;
		if (inputs.isKeyDown(KEY_S))
			newPos -= this->m_cam->getFront() * speedDelta;
		if (inputs.isKeyDown(KEY_A))
			newPos -= this->m_cam->getRight() * speedDelta;
		if (inputs.isKeyDown(KEY_D))
			newPos += this->m_cam->getRight() * speedDelta;

		if (newPos != transform.position)
			this->m_cam->setLocalPosition(newPos);
	}
}

std::shared_ptr<Camera> SceneCamera::getCam()
{
	return this->m_cam;
}
