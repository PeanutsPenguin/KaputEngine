#include "Hierarchy/Hierarchy.h"

#include "Application.h"
#include "Editor/Editor.h"
#include "PickingHandler/PickingHandler.h"
#include "Utils/RemoveVector.hpp"
#include "Window/VirtualWindow.h"

using namespace KaputEngine;

using KaputEditor::HierarchyWindow;
using Rendering::Color;

using std::string;

#define SPAWN_DISTANCE 5
HierarchyWindow::HierarchyWindow()
{
	this->m_window = Application::addUIWindow("Hierarchy");
	this->m_scene = nullptr;
	this->m_renamingId = 0;
}

void HierarchyWindow::setScene(std::shared_ptr<Scene> scene)
{
	this->m_scene = scene;
}

void HierarchyWindow::setRenamingObject(GameObject& obj)
{
	this->m_renamingId = obj.id();
	strcpy(this->m_newName, obj.name.c_str());
}

void HierarchyWindow::displayEntity(GameObject& obj)
{
	const Id& id = obj.id();
	string hider = "##";
	string objName = obj.name;
	string nodeName = objName;
	
	if (objName == "")
		nodeName = obj.typeName();

	nodeName += hider + std::to_string(id);

	TreeNodeFlags flags = E_BASIC;

	bool
		children = false,
		selected = false,
		open     = false;

	if (Editor::getInstance()->getSelectedObjectId() == id)
		selected = true;

	if (obj.children().size() > 0)
		children = true;

	if (children && selected)
		flags = E_SELECTED_AND_OPEN;
	else if (!children && selected)
		flags = E_LEAF_SELECTED_OPEN;
	else if (!children && !selected)
		flags = E_LEAF_AND_OPEN;

	open = this->m_window->TreeNode(nodeName, flags);

	if (this->m_window->isItemHovered() && Application::getInputManager().isMouseLeftDoubleClicked())
		Editor::getInstance()->getPickingHandler().setCamPosition(obj.getWorldTransform().position);

	if (this->m_window->beginDragAndDropSource())
	{
		this->m_window->setDragNDropValue("GameObject", id);
		this->m_window->renderText(objName);
		this->m_window->endDragAndDropSource();
	}

	if (this->m_window->beginDragAndDropTarget())
	{
		if (const Id* childID = this->m_window->getDragNDropValue<Id>("GameObject"); childID)
		{
			std::shared_ptr<GameObject> ptr = Editor::getInstance()->idToGameObject(*childID, *this->getScene());

			if (ptr)
			{
				if (ptr->parentObject()->id() == id)
					ptr->attachTo(&this->getScene()->sceneRoot());
				else
					ptr->attachTo(obj.shared_from_this().get());
			}
		}

		this->m_window->endDragAndDropTarget();
	}

	if (this->m_window->isItemClicked())
		if (Editor::getInstance()->setSelectedObject(id, *this->getScene()))
			Editor::getInstance()->getPickingHandler().activateGizmos();

	if (this->m_renamingId == id)
		if (this->m_window->renderInputText("##rename", this->m_newName, NAME_MAX_CHARACTERS))
		{
			objName = this->m_newName;
			obj.rename(objName.c_str());
			this->m_renamingId = 0;
		}

	if (this->m_window->startRightClickItemMenu())
	{
		for (string& type : GameObject::getTypes())
			if (this->m_window->addMenuItem(std::format("Create {}", type)))
			{
				Editor* instance = Editor::getInstance();
				std::shared_ptr<GameObject> object = GameObject::create(type);
				object->attachTo(obj.shared_from_this().get());
				bool _ = instance->setSelectedObject(object->id(), *this->getScene());
				LibMath::Vector3f newPosition = instance->getPickingHandler().getCamera()->getCam()->getWorldTransform().position 
												+ instance->getPickingHandler().getCamera()->getCam()->getFront() 
												* SPAWN_DISTANCE;
				Transform basic;
				basic.position = newPosition;
				object->setWorldTransform(basic);
			}

		if (this->m_window->addMenuItem("Detach"))
			obj.attachTo(&this->getScene()->sceneRoot());

		if (this->m_window->addMenuItem("Rename") && this->m_renamingId != id)
		{
			strcpy(this->m_newName, objName.c_str());
			this->m_renamingId = id;
		}

		if (this->m_window->addMenuItem("Delete"))
		{
			Editor::getInstance()->unselectObject();
			Editor::getInstance()->getPickingHandler().setState(E_NOT_RENDERING);

			if (std::weak_ptr<Camera> camPtr = Application::getWindow().currentScene()->getPrimaryCamera(); !camPtr.expired())
				if (obj.id() == camPtr.lock()->id())
					Application::getWindow().currentScene()->setPrimaryCamera(nullptr);

			obj.destroy();
		}

		this->m_window->endPopup();
	}

	if (open)
	{
		for (GameObject& object : obj.children())
			if (object.id() != GIZMO_X_ID && object.id() != GIZMO_Y_ID && object.id() != GIZMO_Z_ID)
				this->displayEntity(object);

		this->m_window->popTree();
	}
}

void HierarchyWindow::renderSceneValues()
{
	string fullText = "ScenePath :" + Editor::getInstance()->getScenePath();
	this->m_window->renderText(fullText);

	Color& sceneCol = Application::getWindow().currentScene()->clearColor();
	decltype(auto) _ = this->m_window->renderColorPicker("Background Color", sceneCol);

	this->m_window->renderText("Main Camera : ");

	std::weak_ptr<const Camera> camPtr = Application::getWindow().currentScene()->getPrimaryCamera();

	if (camPtr.expired())
	{
		this->m_window->onSameLine(0);
		decltype(auto) _ = this->m_window->renderButton("No Main Camera");
	}
	else
	{
		const Camera& cam = *camPtr.lock();
		string name = cam.name;

		if (name == "")
			name = cam.typeName() + std::to_string(cam.id());

		this->m_window->onSameLine(0);

		if (this->m_window->renderButton(name))
			decltype(auto) _ = Editor::getInstance()->setSelectedObject(cam.id(), *Application::getWindow().currentScene());
	}

	if (this->m_window->beginDragAndDropTarget())
	{
		if (const Id* childID = this->m_window->getDragNDropValue<Id>("GameObject"); childID)
		{
			std::shared_ptr<GameObject> ptr = Editor::getInstance()->idToGameObject(*childID, *this->getScene());

			if (ptr)
			{
				std::shared_ptr<Camera> cam = std::dynamic_pointer_cast<Camera>(ptr);

				if (cam)
					Application::getWindow().currentScene()->setPrimaryCamera(*cam);
			}
		}

		this->m_window->endDragAndDropTarget();
	}
}

void HierarchyWindow::renderRightClick()
{
	if (Application::getInputManager().isKeyPressed(KEY_MOUSE_RIGHT_BUTTON, this->m_window) && this->m_window->isMouseInWindow())
		this->m_window->InitPopUp("HierarchyRightClick");

	if (this->m_window->beginPopUp("HierarchyRightClick"))
	{
		for (string& type : GameObject::getTypes())
			if (this->m_window->selectable(std::format("Create {}", type)))
			{
				Editor* instance = Editor::getInstance();
				std::shared_ptr<GameObject> object = GameObject::create(type);
				object->attachTo(&Application::getWindow().currentScene()->sceneRoot());
				bool _ = instance->setSelectedObject(object->id(), *this->getScene());
				LibMath::Vector3f newPosition = instance->getPickingHandler().getCamera()->getCam()->getWorldTransform().position 
												+ instance->getPickingHandler().getCamera()->getCam()->getFront() 
												* SPAWN_DISTANCE;
				Transform basic;
				basic.position = newPosition;
				object->setWorldTransform(basic);
			}

		this->m_window->endPopUp();
	}
}

bool HierarchyWindow::displayCanvas()
{
	TreeNodeFlags flags = E_BASIC;
	const bool open = this->m_window->TreeNode("Canvas", flags);

	if (this->m_window->startRightClickItemMenu())
	{
		for (string& type : UIObject::getTypes())
			if (this->m_window->selectable(std::format("Create {}", type)))
			{
				std::shared_ptr<UIObject> object = UIObject::create(type);
				this->getScene()->addUiObject(object);
			}

		this->m_window->endPopUp();
	}

	return open;
}

bool HierarchyWindow::renderUIObject(std::shared_ptr<UIObject> object)
{
	const Id& id = object->id();
	string name = object->name;
	string hider = "##";

	if (name == "")
		name = object->typeName() + hider + std::to_string(id);

	TreeNodeFlags flags = E_LEAF_SELECTED_OPEN;

	bool selected = false;
	bool open = false;

	if (Editor::getInstance()->getSelectedObjectId() == id)
		selected = true;

	if (!selected)
		flags = E_LEAF_AND_OPEN;

	decltype(auto) _ = this->m_window->TreeNode(name, flags);

	if (this->m_renamingId == id)
		if (this->m_window->renderInputText("##rename", this->m_newName, NAME_MAX_CHARACTERS))
		{
			name = this->m_newName;
			object->rename(this->m_newName);
			this->m_renamingId = 0;
		}

	if (this->m_window->isItemClicked())
		decltype(auto) _ = Editor::getInstance()->setSelectedObject(object);

	if (this->m_window->startRightClickItemMenu())
	{
		if (this->m_window->addMenuItem("Rename") && this->m_renamingId != id)
		{
			strcpy(this->m_newName, name.c_str());
			this->m_renamingId = id;
		}

		if (this->m_window->addMenuItem("Delete"))
		{
			this->m_window->endPopUp();
			this->m_window->popTree();
			return true;
		}

		this->m_window->endPopUp();
	}

	this->m_window->popTree();
	return false;
}

bool HierarchyWindow::displayRoot()
{
	string name = "Root";
	GameObject& obj = this->getScene()->sceneRoot();

	TreeNodeFlags flags = E_BASIC;

	bool children = this->getScene()->sceneRoot().children().size() > 0;
	bool selected = false;

	if (Editor::getInstance()->getSelectedObjectId() == obj.id())
		selected = true;

	const bool open = this->m_window->TreeNode(name, flags);

	if (this->m_window->startRightClickItemMenu())
	{
		for (string& type : GameObject::getTypes())
			if (this->m_window->addMenuItem(std::format("Create {}", type)))
			{
				std::shared_ptr<GameObject> object = GameObject::create(type);
				object->attachTo(&obj);
			}

		this->m_window->endPopup();
	}

	if (this->m_window->beginDragAndDropTarget())
	{
		if (const Id* childID = this->m_window->getDragNDropValue<Id>("GameObject"); childID)
		{
			std::shared_ptr<GameObject> ptr = Editor::getInstance()->idToGameObject(*childID, *this->getScene());

			if (ptr)
			{
				if (ptr->parentObject()->id() ==  obj.id())
					ptr->attachTo(&this->getScene()->sceneRoot());
				else
					ptr->attachTo(&this->getScene()->sceneRoot());

			}
		}

		this->m_window->endDragAndDropTarget();
	}

	return open;
}

void HierarchyWindow::render()
{
	this->m_window->beginWindow();

	this->renderRightClick();

	this->renderSceneValues();

	this->m_window->callSeparator();

	if (this->displayRoot())
	{
		for (GameObject& obj : this->getScene()->sceneRoot().children())
			this->displayEntity(obj);

		this->m_window->popTree();
	}

	if (this->displayCanvas())
	{
		std::vector<std::shared_ptr<UIObject>>& uiObj = this->getScene()->getHUD();

		for (size_t i = 0; i < uiObj.size(); ++i)
		{
			if (std::shared_ptr<KaputEngine::SceneRenderer> castptr = std::dynamic_pointer_cast<KaputEngine::SceneRenderer>(uiObj[i]); castptr)
				continue;

			if (this->renderUIObject(uiObj[i]))
				uiObj.erase(uiObj.begin() + i);
		}

		this->m_window->popTree();
	}

	this->m_window->endWindow();
}

std::shared_ptr<Scene>& HierarchyWindow::getScene() noexcept
{
	if (m_scene)
		return this->m_scene;
	else
		return Application::getWindow().currentScene();
}

const std::shared_ptr<KaputEngine::Scene>& KaputEditor::HierarchyWindow::getScene() const noexcept
{
	if (m_scene)
		return this->m_scene;
	else
		return Application::getWindow().currentScene();
}

