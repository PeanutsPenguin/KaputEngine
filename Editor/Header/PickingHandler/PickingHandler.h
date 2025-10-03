#pragma once

#include "GameObject/GameObject.h"
#include "Rendering/Primitive.h"
#include "Rendering/ShaderProgram.h"
#include "Resource/Mesh.h"
#include "SceneCamera/SceneCamera.h"
#include "SceneView/SceneView.h"

constexpr int GIZMO_X_INDEX = 0;
constexpr int GIZMO_X_ID	= 600;

constexpr int GIZMO_Y_INDEX	= 1;
constexpr int GIZMO_Y_ID	= 601;

constexpr int GIZMO_Z_INDEX	= 2;
constexpr int GIZMO_Z_ID	= 602;

namespace KaputEditor
{
	enum eGizmosState : int
	{
		E_NOT_RENDERING,
		E_RENDERING,
		E_MOVING_X,
		E_MOVING_Y,
		E_MOVING_Z
	};

	class PicklingHandler : public SceneView
	{
	public:
		PicklingHandler();

		void update();

		void render() override;

		void setState(eGizmosState state);

		_NODISCARD std::shared_ptr<SceneCamera>& getCamera() noexcept;

		void activateGizmos();

		void switchToRotateMesh();

		void switchToMoveMesh();

		void destroy();

		void setCamPosition(const LibMath::Vector3f& pos);

		~PicklingHandler() = default;

	private:
		std::vector<std::shared_ptr<KaputEngine::GameObject>> m_gizmos;
		std::shared_ptr<const KaputEngine::Rendering::ShaderProgram> m_program;
		std::shared_ptr<SceneCamera> m_camera;
		eGizmosState m_state;
		KaputEngine::Rendering::Sphere m_defaultMesh;

		LibMath::Vector2f m_objectScreenPosition;
		LibMath::Vector2f m_gizmoScreenPosition;

		KaputEngine::Transform m_oldObjectTransform;
		LibMath::Vector3f m_oldObjectRotation;

		LibMath::Vector3f m_Direction;

		std::shared_ptr<const KaputEngine::Resource::MeshResource> m_translate;
		std::shared_ptr<const KaputEngine::Resource::MeshResource> m_rotate;

		void renderSceneView(KaputEngine::GameObject& object);

		void sendPositions(const KaputEngine::GameObject& object, const KaputEngine::Rendering::Color& col);

		void pickingRenderGizmos();

		void renderPhysicsComponent(KaputEngine::GameObject& obj);

		void resize() override;

		void sceneViewRenderGizmos();

		void placeGizmo(float offset);

		_NODISCARD bool checkGizmosId(KaputEngine::Id id);

		void onGizmoDown();

		void moveWithGizmo(float ratio);

		void rotateWithGizmo(float ratio);

		void scaleWithGismo(float ratio);

		_NODISCARD KaputEngine::Id getPickedID();

		LibMath::Vector2f worldToScreenPosition(const KaputEngine::GameObject& position);

		KaputEngine::GameObject& getSelectedGizmo();

		void onGizmoClick();

		_NODISCARD float getXRatio(const LibMath::Vector2d& origin, const LibMath::Vector2d& gizmo, const LibMath::Vector2d& mousPos);

		_NODISCARD float getYRatio(const LibMath::Vector2d& origin, const LibMath::Vector2d& gizmo, const LibMath::Vector2d& mousPos);

		void setDirectionValue(float value);

		_NODISCARD LibMath::Vector2d getRelativeMousePosition();
	};
}
