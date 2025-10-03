#include "PickingHandler/PickingHandler.h"

#include "Application.h"
#include "Component/RenderComponent.h"
#include "Editor/Editor.h"
#include "GameObject/GameObject.hpp"
#include "Resource/Manager.hpp"
#include "Resource/ShaderProgram.h"
#include "Scene/Transform/MatrixSource.h"
#include "Utils/RemoveVector.hpp"

#include <LibMath/Matrix.h>
#include <glad/glad.h>

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Rendering::Lighting;
using namespace KaputEngine::Resource;

using KaputEditor::PicklingHandler;
using KaputEditor::SceneCamera;

using KaputEngine::Rendering::Color;
using KaputEngine::Rendering::Mesh;

using std::cout;

_NODISCARD static Color toPickingColor(const Id& id) noexcept
{
	Color picking = Color::Black;

	for (ArrIndex i = 0; i < 3; ++i)
	{
		const uint32_t mask  = 0xFF << (i * 8);
		const uint8_t  shift = i * 8;

		picking.raw()[i] = ((id & mask) >> shift) / 255.f;
	}

	return picking;
}

PicklingHandler::PicklingHandler() : m_defaultMesh(.1f, 20, 30)
{
	this->m_program = ResourceManager::get<ShaderProgramResource>("Kaput/Shader/Picking/PickingProgram.kasset")->dataPtr();
	this->m_camera = std::make_shared<SceneCamera>();

	this->m_state = E_NOT_RENDERING;

	this->m_translate = ResourceManager::get<MeshResource>("Kaput/Mesh/TranslateGizmo.kasset");
	this->m_rotate = ResourceManager::get<MeshResource>("Kaput/Mesh/rotateGizmo.kasset");

	std::shared_ptr<GameObject> xGizmo = GameObject::create<GameObject>((Id)GIZMO_X_ID);
	xGizmo->addComponent<RenderComponent>(this->m_translate->root(), nullptr);
	xGizmo->setLocalScale({ .2f, .2f, .2f });
	this->m_gizmos.push_back(xGizmo);

	std::shared_ptr<GameObject> yGizmo = GameObject::create<GameObject>((Id)GIZMO_Y_ID);
	yGizmo->addComponent<RenderComponent>(this->m_translate->root(), nullptr);
	Transform newTrans = xGizmo->getWorldTransform();
	newTrans.scale = { .2f, .2f, .2f };
	newTrans.setRotationEuler({ 0, 0, 90 });
	yGizmo->setWorldTransform(newTrans);
	this->m_gizmos.push_back(yGizmo);

	std::shared_ptr<GameObject> zGizmo = GameObject::create<GameObject>((Id)GIZMO_Z_ID);
	zGizmo->addComponent<RenderComponent>(this->m_translate->root(), nullptr);
	Transform newTrans2 = xGizmo->getWorldTransform();
	newTrans2.scale = { .2f, .2f, .2f };
	newTrans2.setRotationEuler({ 90, -90, 0 });
	zGizmo->setWorldTransform(newTrans2);
	this->m_gizmos.push_back(zGizmo);
}

Id PicklingHandler::getPickedID()
{
	// TODO Use ContextQueue for gl calls

	this->bindFramebuffer();

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!this->m_program->use())
		return 0;

	// Only the positions are needed (not the UVs and normals)
	// TODO Verify for potential undefined behavior - Vertex attributes are still needed to get the position and size betweem vertices
	glEnableVertexAttribArray(0);

	for (GameObject& object : this->getScene()->sceneRoot().children())
		this->sendPositions(object, toPickingColor(object.id()));

	glClear(GL_DEPTH_BUFFER_BIT);
	this->pickingRenderGizmos();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned char data[4];
	Vector2d mousPos = this->getRelativeMousePosition();
	glReadPixels(mousPos.x(), mousPos.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	this->unbindFramebuffer();

	// Convert the color back to an integer ID
	int pickedID =
		data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	return pickedID;
}

Vector2f PicklingHandler::worldToScreenPosition(const GameObject& obj)
{
	const Matrix4f
		&projView = this->m_camera->getCam()->getViewProjectionMatrix(),
		&model    = obj.getWorldTransformMatrix();

	Matrix4f MVP = projView * model;

	Vector2f
		result,
		viewPort = this->m_window->getContentSize();

	// TODO Use 2-loop
	result.x() = MVP[3][0] / MVP[3][3] * viewPort.x() / 2 + viewPort.x() / 2;
	result.y() = MVP[3][1] / MVP[3][3] * viewPort.y() / 2 + viewPort.y() / 2;

	return result;
}

GameObject& PicklingHandler::getSelectedGizmo()
{
	switch (this->m_state)
	{
	case E_MOVING_X:
		return *this->m_gizmos[GIZMO_X_INDEX];
	case E_MOVING_Y:
		return *this->m_gizmos[GIZMO_Y_INDEX];
	case E_MOVING_Z:
		return *this->m_gizmos[GIZMO_Z_INDEX];
	}

	return *this->m_gizmos[GIZMO_X_INDEX];
}

void PicklingHandler::onGizmoClick()
{
	std::shared_ptr<GameObject> obj = nullptr;

	if (obj = std::dynamic_pointer_cast<GameObject>(Editor::getInstance()->getSelectedObjectPtr()); !obj)
		return;

	this->m_objectScreenPosition = this->worldToScreenPosition(*obj);

	this->m_oldObjectTransform = obj->getWorldTransform();
	this->m_oldObjectRotation = Editor::getInstance()->getObjectAngles();

	switch (Editor::getInstance()->getState())
	{
	case E_MOVING:
		this->m_Direction = this->getSelectedGizmo().getWorldTransform().position - this->m_oldObjectTransform.position;
		break;
	case E_SCALING:
		this->setDirectionValue(1);
		break;
	case E_ROTATING:
		this->setDirectionValue(10);
		break;
	default:
		break;
	}

	this->m_gizmoScreenPosition = this->getRelativeMousePosition();
}

float PicklingHandler::getXRatio(const Vector2d& origin, const Vector2d& gizmo, const Vector2d& mousPos)
{
	const float Xdenum = (gizmo.x() - origin.x());

	if (!Xdenum || (abs(Xdenum) <= 1))
		return 0;

	return  (mousPos.x() - origin.x())  / Xdenum - 1;
}

float PicklingHandler::getYRatio(const Vector2d& origin, const Vector2d& gizmo, const Vector2d& mousPos)
{
	const float Ydenum = (gizmo.y() - origin.y());

	if (!Ydenum|| (abs(Ydenum) <= 1))
		return 0;

	return  (mousPos.y() - origin.y())  / Ydenum - 1;
}

void PicklingHandler::setDirectionValue(const float value)
{
	switch (this->m_state)
	{
	case E_MOVING_X:
		this->m_Direction = { value, 0, 0 };
		break;
	case E_MOVING_Y:
		this->m_Direction = { 0, value, 0 };
		break;
	case E_MOVING_Z:
		this->m_Direction = { 0, 0, value };
		break;
	default:
		break;
	}
}

Vector2d PicklingHandler::getRelativeMousePosition()
{
	Vector2d
		padding     = this->m_window->getPadding(),
		contentSize = this->m_window->getContentSize(),
		windowPos   = this->m_window->getPosition(), // Based on the entire screen (Top Left Corner)
		mousPos     = Application::getInputManager().mousePosition();

	mousPos -= padding;
	mousPos -= windowPos;
	mousPos.y() = contentSize.y() - mousPos.y();

	return mousPos;
}

void PicklingHandler::switchToRotateMesh()
{
	this->activateGizmos();

	for (std::shared_ptr<GameObject> gizmo : this->m_gizmos)
	{
		gizmo->getComponent<RenderComponent>()->mesh() = this->m_rotate->root();

		Transform newTrans = gizmo->getWorldTransform();

		switch (gizmo->id())
		{
		case GIZMO_X_ID:
			newTrans.setRotationEuler({ 0, 90, 0 });
			break;
		case GIZMO_Y_ID:
			newTrans.setRotationEuler({ 90, 0, 0 });
			break;
		case GIZMO_Z_ID:
			newTrans.setRotationEuler({ 0, 0, 0 });
			break;
		default:
		    break;
		}

		gizmo->setWorldTransform(newTrans);
	}
}

void PicklingHandler::switchToMoveMesh()
{
	this->activateGizmos();

	for (std::shared_ptr<GameObject> gizmo : this->m_gizmos)
	{
		gizmo->getComponent<RenderComponent>()->mesh() = this->m_translate->root();

		Transform newTrans = gizmo->getWorldTransform();

		switch (gizmo->id())
		{
		case GIZMO_X_ID:
			newTrans.setRotationEuler({ 0, 0, 0 });
			break;
		case GIZMO_Y_ID:
			newTrans.setRotationEuler({ 0, 0, 90 });
			break;
		case GIZMO_Z_ID:
			newTrans.setRotationEuler({ 90, -90, 0 });
			break;
		default:
			break;
		}

		gizmo->setWorldTransform(newTrans);
	}
}

void PicklingHandler::destroy()
{
	for (int i = 0; i < m_gizmos.size(); ++i)
	{
		this->m_gizmos[i]->detach();
		this->m_gizmos[i].reset();
	}

	this->m_camera.reset();
}

void PicklingHandler::setCamPosition(const LibMath::Vector3f& pos)
{
	this->m_camera->getCam()->setLocalPosition(pos);
}

void PicklingHandler::update()
{
	if (Editor::getInstance()->m_closing)
		return;

	const double deltaTime = Application::getDeltaTime();

	if (this->m_window->isWindowFocused())
		this->m_camera->updateVision(deltaTime);

	if (Application::getInputManager().isLeftClickPressed(this->m_window)
		&& this->m_state != E_MOVING_X
		&& this->m_state != E_MOVING_Y
		&& this->m_state != E_MOVING_Z)
	{
		const Id pickedID = this->getPickedID();

		if (pickedID == 0x00ffffff)
		{
			this->m_state = E_NOT_RENDERING;
			Editor::getInstance()->unselectObject();
		}
		else if (!this->checkGizmosId(pickedID))
			if (Editor::getInstance()->setSelectedObject(pickedID, *this->getScene()))
				this->activateGizmos();
	}
	// TODO Consider a single bit flag for any movement
	else if ((this->m_state == E_MOVING_X
				|| this->m_state == E_MOVING_Y
				|| this->m_state == E_MOVING_Z)
				&& Application::getInputManager().isLeftClickReleased())
				{
					this->m_state = E_RENDERING;
				}

	else if (Application::getInputManager().isLeftClickDown()
			&& (this->m_state == E_MOVING_X
			|| this->m_state == E_MOVING_Y
			|| this->m_state == E_MOVING_Z))
			{
				this->onGizmoDown();
			}
}

void PicklingHandler::sceneViewRenderGizmos()
{
	// TODO Use ContextQueue
	glClear(GL_DEPTH_BUFFER_BIT);
	switch (this->m_state)
	{
	case E_NOT_RENDERING:
		return;
	case E_RENDERING:
	case E_MOVING_X:
	case E_MOVING_Y:
	case E_MOVING_Z:
		if (!this->m_program->use())
			return;

		switch (Editor::getInstance()->getState())
		{
		case E_MOVING:
		case E_SCALING:
			this->placeGizmo(.5f);
			break;
		case E_ROTATING:
			this->placeGizmo(0);
			break;
		default:
			break;
		}

		this->sendPositions(*this->m_gizmos[GIZMO_X_INDEX], Color::Red);
		this->sendPositions(*this->m_gizmos[GIZMO_Y_INDEX], Color::Green);
		this->sendPositions(*this->m_gizmos[GIZMO_Z_INDEX], Color::Blue);
		return;
	default:
		return;
	}
}

void PicklingHandler::placeGizmo(const float offset)
{
	std::shared_ptr<GameObject> obj = nullptr;

	if (obj = std::dynamic_pointer_cast<GameObject>(Editor::getInstance()->getSelectedObjectPtr()); !obj)
		return;

	const Transform& worldtrans = obj->getWorldTransform();
	Transform gizmoTrans;
	Cartesian3f newPos;

	newPos = { worldtrans.position.x() + offset, worldtrans.position.y(), worldtrans.position.z() };
	gizmoTrans = this->m_gizmos[GIZMO_X_INDEX]->getWorldTransform();
	this->m_gizmos[GIZMO_X_INDEX]->setWorldTransform({ newPos, gizmoTrans.rotation, gizmoTrans.scale });

	newPos = { worldtrans.position.x(), worldtrans.position.y() + offset, worldtrans.position.z() };
	gizmoTrans = this->m_gizmos[GIZMO_Y_INDEX]->getWorldTransform();
	this->m_gizmos[GIZMO_Y_INDEX]->setWorldTransform({ newPos, gizmoTrans.rotation, gizmoTrans.scale });

	newPos = { worldtrans.position.x(), worldtrans.position.y(), worldtrans.position.z() + offset };
	gizmoTrans = this->m_gizmos[GIZMO_Z_INDEX]->getWorldTransform();
	this->m_gizmos[GIZMO_Z_INDEX]->setWorldTransform({ newPos, gizmoTrans.rotation, gizmoTrans.scale });
}

bool PicklingHandler::checkGizmosId(const Id id)
{
	for (int i = 0; i < 3; ++i)
		if (id == GIZMO_X_ID + i)
		{
			this->m_state = static_cast<eGizmosState>(E_MOVING_X + i);
			this->onGizmoClick();
			return true;
		}

	return false;
}

void PicklingHandler::onGizmoDown()
{
	const Vector2f
		distance = (Vector2f)this->getRelativeMousePosition() - this->m_objectScreenPosition,
		base     = this->m_gizmoScreenPosition - this->m_objectScreenPosition;

	const float ratio = distance.dot(base) / base.magnitudeSquared() - 1;

	switch (Editor::getInstance()->getState())
	{
	case E_MOVING:
		this->moveWithGizmo(ratio);
		this->placeGizmo(.5f);
		break;
	case E_SCALING:
		this->scaleWithGismo(ratio);
		this->placeGizmo(.5f);
		break;
	case E_ROTATING:
		this->rotateWithGizmo(ratio);
		this->placeGizmo(0);
		break;
	default:
		break;
	}
}

void PicklingHandler::moveWithGizmo(const float ratio)
{
	std::shared_ptr<GameObject> obj = nullptr;

	if (obj = std::dynamic_pointer_cast<GameObject>(Editor::getInstance()->getSelectedObjectPtr()); !obj)
		return;

	Transform newTrans = obj->getWorldTransform();
	newTrans.position = this->m_oldObjectTransform.position + this->m_Direction * ratio;

	if (Editor::getInstance()->getState() != E_PLAYING)
		obj->setWorldTransform(newTrans);
}

void PicklingHandler::rotateWithGizmo(const float ratio)
{
	std::shared_ptr<GameObject> obj = nullptr;

	if (obj = std::dynamic_pointer_cast<GameObject>(Editor::getInstance()->getSelectedObjectPtr()); !obj)
		return;

	Transform newTrans = obj->getWorldTransform();
	const Vector3f newEuler = this->m_oldObjectRotation + this->m_Direction * ratio;
	Editor::getInstance()->setObjectAngles(newEuler);
	newTrans.setRotationEuler(newEuler);

	if (Editor::getInstance()->getState() != E_PLAYING)
		obj->setWorldTransform(newTrans);
}

void PicklingHandler::scaleWithGismo(const float ratio)
{
	std::shared_ptr<GameObject> obj = nullptr;

	if (obj = std::dynamic_pointer_cast<GameObject>(Editor::getInstance()->getSelectedObjectPtr()); !obj)
		return;

	Transform newTrans = obj->getWorldTransform();
	newTrans.scale = this->m_oldObjectTransform.scale + this->m_Direction * ratio;

	if (Editor::getInstance()->getState() != E_PLAYING)
		obj->setWorldTransform(newTrans);
}

void PicklingHandler::resize()
{
	const Vector2f size = this->m_window->getContentSize();

	Application::resizeViewport(size);
	this->m_image->setImageSize(size);

	const float aspect = size.x() / size.y();

	if (this->getScene())
		this->m_camera->getCam()->setAspect(aspect);

	this->m_framebuffer->resize(size);
}

void PicklingHandler::render()
{
	this->bindFramebuffer();

	if (this->getScene())
	{
		this->getScene()->clearBackground();

		const auto& objects = this->getScene()->sceneRoot().children();

		for (GameObject& obj : objects)
			this->renderSceneView(obj);
		for (GameObject& obj : objects)
			this->renderPhysicsComponent(obj);

		this->sceneViewRenderGizmos();
	}

	this->unbindFramebuffer();
}

void PicklingHandler::setState(const eGizmosState state)
{
	this->m_state = state;
}

void PicklingHandler::renderPhysicsComponent(GameObject& obj)
{
	for (auto& component : obj.components())
		if (PhysicComponent* castPtr = dynamic_cast<PhysicComponent*>(&component); castPtr)
			castPtr->render(*this->m_camera->getCam().get());

	for (GameObject& object :obj.children())
		this->renderPhysicsComponent(object);
}

void PicklingHandler::pickingRenderGizmos()
{
	switch (this->m_state)
	{
	case E_NOT_RENDERING:
		break;
	case E_RENDERING:
	case E_MOVING_X:
	case E_MOVING_Y:
	case E_MOVING_Z:
		for (int i = GIZMO_X_INDEX; i < 3; ++i)
			this->sendPositions(*this->m_gizmos[i], toPickingColor(this->m_gizmos[i]->id()));
		break;
	default:
		break;
	}
}

std::shared_ptr<SceneCamera>& PicklingHandler::getCamera() noexcept
{
	return this->m_camera;
}

void PicklingHandler::activateGizmos()
{
	this->m_state = E_RENDERING;

	if (Editor::getInstance()->getState() == E_NONE)
		Editor::getInstance()->setState(E_MOVING);

	switch (Editor::getInstance()->getState())
	{
	case E_MOVING:
	case E_SCALING:
		this->placeGizmo(.5f);
		break;
	case E_ROTATING:
		this->placeGizmo(0);
		break;
	default:
	break;
	}
}

void PicklingHandler::renderSceneView(GameObject& object)
{
	bool hasRendered = false;

	for (Component& component : object.components())
		if (RenderComponent* castPtr = dynamic_cast<RenderComponent*>(&component))
		{
			if (!castPtr->mesh() || !castPtr->getCanRender())
				continue;

			castPtr->render(*this->m_camera->getCam());
			hasRendered = true;
		}

	if (!hasRendered)
		if (this->m_program->use())
		{
			const Matrix4f
				&projview = this->m_camera->getCam()->getViewProjectionMatrix(),
				&model = object.getWorldTransformMatrix();

			const Matrix4f MVP = projview * model;

			this->m_program->setUniform("MVP", MVP);
			this->m_program->setUniform("PickingColor", Color::Black);

			this->m_defaultMesh.draw();
		}

	for (GameObject& obj : object.children())
		this->renderSceneView(obj);
}

void PicklingHandler::sendPositions(const GameObject& object, const Color& col)
{
	const Matrix4f
		&projview = this->m_camera->getCam()->getViewProjectionMatrix(),
		&model    = object.getWorldTransformMatrix();

	Matrix4f MVP = projview * model;

	this->m_program->setUniform("MVP", MVP);
	this->m_program->setUniform("PickingColor", col);

	const RenderComponent* render = object.getComponent<RenderComponent>();

	if (render)
	{
		std::shared_ptr<const Mesh> mesh = render->mesh();

		if (mesh)
			mesh->draw();
		else
			m_defaultMesh.draw();
	}
	else
		m_defaultMesh.draw();

	for (const GameObject& obj : object.children())
		this->sendPositions(obj, toPickingColor(obj.id()));
}
