#include "SceneView/SceneView.h"

#include "Application.h"
#include "GameObject/Camera.h"

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Rendering::Lighting;

using KaputEditor::SceneView;

using KaputEngine::Rendering::Color;

SceneView::SceneView(const char* const name)
{
	this->m_framebuffer = std::make_unique<CustomFrameBuffer>();
	this->m_framebuffer->create({ 800, 600 });
	this->m_scene = nullptr;

	UIWindowFlags flags = UIWindowFlags::E_NO_SCROLL_BAR;

	this->m_window = Application::addUIWindow(name, flags);

	this->m_image = this->m_window->addGlImage({800,600}, this->m_framebuffer->getTexId());
}

void SceneView::preRender()
{
	Application::clearScreen(Color::White);

	if (this->m_window->hasResized())
		this->resize();
	else
	{
		const Vector2f size = this->m_window->getContentSize();

		Application::resizeViewport(size);
	}
}

void SceneView::render()
{
	this->bindFramebuffer();

	if (std::shared_ptr<KaputEngine::Scene> scene = getScene(); scene)
	{
		std::weak_ptr<Camera> cam = scene->getPrimaryCamera();

		if (!cam.expired())
			scene->render(*cam.lock());
		else
			Application::clearScreen(Color::Black);
	}
	else
		Application::clearScreen(Color::Black);

	this->unbindFramebuffer();
}

void SceneView::resize()
{
	const Vector2f size = this->m_window->getContentSize();

	Application::resizeViewport(size);
	this->m_image->setImageSize(size);

	const float aspect = size.x() / size.y();

	std::shared_ptr<Scene> const scene = getScene();

	if (scene)
	{
		std::weak_ptr<Camera> cam = scene->getPrimaryCamera();

		if (!cam.expired())
			cam.lock()->setAspect(aspect);
	}

	this->m_framebuffer->resize(size);
}

void SceneView::bindFramebuffer()
{
	this->m_framebuffer->bind();
}

void SceneView::unbindFramebuffer()
{
	this->m_framebuffer->unbind();
}

_Ret_notnull_ VirtualWindow* SceneView::getWindow()
{
	return this->m_window;
}

std::shared_ptr<Scene> SceneView::scene() noexcept
{
	return this->m_scene;
}
const std::shared_ptr<Scene> SceneView::scene() const noexcept
{
	return this->m_scene;
}

void SceneView::setScene(_In_ std::shared_ptr<Scene>& scene)
{
	this->m_scene = scene;

	// TODO Remove (Needed for light view)
	//this->m_camera->getCam()->attachTo(&this->m_scene->sceneRoot());
}

std::shared_ptr<Scene> SceneView::getScene()
{
	if (m_scene)
		return this->m_scene;
	else
		return Application::getWindow().currentScene();
}
