#include "Editor/Editor.h"

#include "Application.h"

using namespace KaputEditor;
using namespace KaputEngine;
using namespace LibMath;

Editor* Editor::s_instance = nullptr;

void Editor::init()
{
	this->m_currentScene.first = std::make_shared<Scene>();
	Application::getWindow().currentScene() = this->m_currentScene.first;
	Application::getWindow().setDockSpace(true);
	this->m_state = E_NONE;

	Application::onClose([this]{
		this->m_closing = true;
		Application::setWindowShouldClose(false);
	});
}

_Ret_notnull_ Editor* Editor::getInstance() noexcept
{
	if (!s_instance)
		s_instance = new Editor();

	return s_instance;
}

bool Editor::setSelectedObject(const Id& id, Scene& scene)
{
	if (id == scene.sceneRoot().id())
		return false;

	std::shared_ptr<GameObject> obj = idToGameObject(id, scene);

	if (obj)
	{
		m_selectedId = id;
		m_object = obj;
		m_objEulerAngles = obj->getLocalTransform().getRotationEuler();
		this->m_picker.activateGizmos();

		if (this->m_detailWindow.isFocus())
			this->m_detailWindow.removeItemFocus();

		return true;
	}

	return false;
}

bool Editor::setSelectedObject(std::shared_ptr<ObjectBase> obj)
{
	this->m_object = obj;
	this->m_selectedId = obj->id();
	return true;
}

void Editor::setObjectAngles(const Vector3f& angles)
{
	this->m_objEulerAngles = angles;
}

const Id& Editor::getSelectedObjectId() noexcept
{
	return m_selectedId;
}

std::shared_ptr<ObjectBase> Editor::getSelectedObjectPtr() noexcept
{
	return m_object;
}

EditorState Editor::getState() const noexcept
{
	return this->m_state;
}

void Editor::setState(EditorState state)
{
	this->m_state = state;
}

std::shared_ptr<GameObject> Editor::idToGameObject(const Id& id, Scene& scene)
{
	if (id == Id { } || id == scene.sceneRoot().id())
		return nullptr;

	std::pair<bool, std::shared_ptr<GameObject>> result = getGameObjectFromId(id, scene.sceneRoot());

	return result.first ? result.second : nullptr;
}

Vector3f& Editor::getObjectAngles() noexcept
{
	return m_objEulerAngles;
}

GameView& Editor::getGameView() noexcept
{
	return m_gameView;
}

PicklingHandler& Editor::getPickingHandler() noexcept
{
	return m_picker;
}

DetailWindow& Editor::getDetailWindow() noexcept
{
	return m_detailWindow;
}

HierarchyWindow& Editor::getHierarchyWindow() noexcept
{
	return m_hierarchy;
}

ToolsWindow& Editor::getToolWindow() noexcept
{
	return m_tool;
}

FileExplorer& Editor::getFileExplorerWindow() noexcept
{
	return m_fileExplorer;
}

SaveWindow& Editor::getSaveWindow() noexcept
{
	return m_saveWindow;
}

std::string& Editor::getScenePath() noexcept
{
	return this->m_currentScene.second;
}

bool Editor::loadScene(_In_ const char* path)
{
	if (!path)
		return false;

	std::shared_ptr newScene = std::make_shared<Scene>();

	if (!newScene->load(path))
		return false;

	this->m_currentScene.first->destroy();
	this->m_currentScene.first = newScene;
	Application::getWindow().currentScene() = this->m_currentScene.first;

	if (std::shared_ptr<Scene> scene = Application::getWindow().currentScene(); scene)
		scene->save("Kaput/Scene/Editor.kscene", true);

	this->unselectObject();
	this->m_picker.setState(E_NOT_RENDERING);
	this->m_state = E_NONE;

	Application::audio().stopAll();

	return true;
}

bool Editor::openScene(_In_ const char* path)
{
	if (!path)
		return false;

	if (path == this->m_currentScene.second)
		return false;
	else if (loadScene(path))
	{
		this->m_currentScene.second = path;
		return true;
	}

	return false;
}

void Editor::destroy()
{
	this->m_currentScene.first.reset();
	m_saveWindow.destroy();
	m_picker.destroy();
	delete s_instance;
}

void Editor::unselectObject()
{
	this->m_selectedId = NO_ID;
	this->m_object = nullptr;
}

// TODO Consider returning optional or nullable pointer
std::pair<bool, std::shared_ptr<GameObject>> Editor::getGameObjectFromId(const Id& id, GameObject& object)
{
	std::pair<bool, std::shared_ptr<GameObject>> result;

	result.first = false;
	result.second = nullptr;

	if (id == object.id())
	{
		result.second = object.shared_from_this();
		m_selectedId = id;
		result.first = true;
		return result;
	}

	for (GameObject& obj : object.children())
	{
		if (id == obj.id())
		{
			result.second = obj.shared_from_this();
			m_selectedId = id;
			result.first = true;
			return result;
		}

		result = getGameObjectFromId(id, obj);

		if (result.first)
			return result;
	}

	return result;
}
