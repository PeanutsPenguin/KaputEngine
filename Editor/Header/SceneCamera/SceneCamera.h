#pragma once

#include "GameObject/Camera.h"

namespace KaputEditor
{
	class SceneCamera
	{
	public:
		SceneCamera();

		void updateVision(double deltaTime);

		std::shared_ptr<KaputEngine::Camera> getCam();

		~SceneCamera() = default;

	private:

		float
			movementSpeed = 10.f,
			sensitivity = .05f;

		std::shared_ptr<KaputEngine::Camera> m_cam;
	};
}
