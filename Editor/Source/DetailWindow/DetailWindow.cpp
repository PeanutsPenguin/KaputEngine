#include "Application.h"
#include "Component/Audio/AudioComponent.h"
#include "Component/PhysicComponent.h"
#include "Component/RenderComponent.h"
#include "Component/ScriptComponent.h"
#include "DetailWindow/DetailWindow.h"
#include "Editor/Editor.h"
#include "Inspector/Property.hpp"
#include "Rendering/Lighting/LightBuffer.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Material.h"
#include "Resource/Mesh.h"
#include "Resource/Sound.h"
#include "Scene/Scene.h"
#include "Scene/Transform/Transform.h"
#include "Text/Xml/Parser.h"
#include "Window/VirtualWindow.h"

#include <fstream>
#include <iostream>

#include <LibMath/Vector/Vector2.h>

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;
using namespace KaputEngine::Rendering;
using namespace KaputEngine::Rendering::Lighting;
using namespace KaputEngine::Resource;

using KaputEngine::Inspector::Property;

using LibMath::Vector2f;
using LibMath::Vector3f;

using KaputEditor::DetailWindow;

using std::cerr;
using std::format;
using std::ios;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::string_view;

DetailWindow::DetailWindow() : m_window(Application::addUIWindow("Details")), m_scene(nullptr) { }

void DetailWindow::setScene(_In_ Scene* scene)
{
	this->m_scene = scene;
}

void DetailWindow::render()
{
	Editor& editor = *Editor::getInstance();

	std::shared_ptr<ObjectBase> obj = editor.getSelectedObjectPtr();

	if (!obj)
		return;

	PhysicComponent* physicComponent = nullptr;

	this->m_window->beginWindow();

	this->m_window->renderText(obj->typeName(), true);

	renderProperties(*obj);

	this->m_window->callSeparator();

	if (std::shared_ptr<UIImage> img = std::dynamic_pointer_cast<UIImage>(editor.getSelectedObjectPtr()); img)
	{
		string texPath = img->getTexPath();
		this->m_window->renderText("Texture :");
		this->m_window->onSameLine(0);
		decltype(auto) _ = this->m_window->renderButton(texPath);

		if (Editor::getInstance()->getState() != E_PLAYING)
			this->changeUIImageTexture(img);

		this->m_window->endWindow();
		return;
	}

	std::shared_ptr<GameObject> gameObj = nullptr;
	if (gameObj = std::dynamic_pointer_cast<GameObject>(editor.getSelectedObjectPtr()); !gameObj)
	{
		this->m_window->endWindow();
		return;
	}

	for (auto& component : gameObj->components())
	{
		this->m_window->callSeparator();

		if (RenderComponent* castPtr = dynamic_cast<RenderComponent*>(&component); castPtr)
			this->renderRenderComponent(*castPtr);
		else if (PhysicComponent* castPtr = dynamic_cast<PhysicComponent*>(&component); castPtr)
		{
			this->renderPhysicComponent(*castPtr);
			physicComponent = castPtr;
		}
		else if (ScriptComponent* castPtr = dynamic_cast<ScriptComponent*>(&component); castPtr)
			this->renderScriptComponent(*castPtr);
		else if (AudioComponent* castPtr = dynamic_cast<AudioComponent*>(&component); castPtr)
			this->renderAudioComponent(*castPtr);
		else
		{
			renderObject(component);

			if (this->m_window->renderButton(format("Remove Component##{}", component.id())))
				if (editor.getState() != E_PLAYING)
					component.destroy();
		}
	}

	this->m_window->callSeparator();

	if (editor.getState() != E_PLAYING)
		this->renderAddComponentButton(*gameObj, physicComponent);

	this->m_window->endWindow();
}

void DetailWindow::removeItemFocus()
{
	this->m_window->beginWindow();
	this->m_window->removeItemFocus();
	this->m_window->endWindow();
}

bool DetailWindow::isFocus()
{
	return this->m_window->isWindowFocused();
}

bool DetailWindow::renderTransform(
	_In_opt_ ObjectBase* source, Transform& trans, const string& id, bool readOnly)
{
	bool modified = false;

	Editor& editor = *Editor::getInstance();

	if (this->m_window->renderVec3(string("Position ") + id, trans.position, readOnly))
		modified = true;

	Vector3f
		localEuler,
		&euler = source && source->id() == editor.getSelectedObjectId()
		? editor.getObjectAngles() : localEuler = trans.getRotationEuler();

	if (this->m_window->renderVec3(string("Rotation ") + id, euler, readOnly))
	{
		modified = true;
		trans.setRotationEuler(euler);
	}

	if (this->m_window->renderVec3(string("Scale ") + id, trans.scale, readOnly))
		modified = true;

	return modified;
}

void DetailWindow::renderPhysicComponent(PhysicComponent& physicComponent)
{
	this->m_window->renderText("Physic Component");

	renderProperties(physicComponent);

	this->renderSwitchShape(physicComponent);

	this->renderScale(physicComponent);

	this->renderSwitchTag(physicComponent);

	/*Freeze Translation*/
	std::bitset<3> axes = physicComponent.getFreezeTranslation();
	this->m_window->renderText("Freeze Translation :");

	string
		id = "##" + std::to_string(physicComponent.id()),
		partName = "Physic" + id;

	const char axeChars[] { 'X', 'Y', 'Z' };

	for (int i = 0; i < axes.size(); ++i)
	{
		partName = axeChars[i] + id + "Translation";

		bool freeze = axes[i];

		if (this->m_window->renderCheckBox(partName, freeze))
			physicComponent.setFreezeTranslation(i, freeze);

		if (i != 2)
			this->m_window->onSameLine(0);
	}
	/*Freeze Translation*/

	/*Freeze Rotation*/
	axes = physicComponent.getFreezeRotation();
	this->m_window->renderText("Freeze Rotation :");

	for (int i = 0; i < axes.size(); ++i)
	{
		partName = axeChars[i] + id + "Rotation";

		bool freeze = axes[i];

		if (this->m_window->renderCheckBox(partName, freeze))
			physicComponent.setFreezeRotation(i, freeze);

		if (i != 2)
			this->m_window->onSameLine(0);
	}
	/*Freeze Rotation*/

	/*Destroy*/
	partName = "Remove Component" + id;
	if (Editor::getInstance()->getState() != E_PLAYING)
		if (this->m_window->renderButton(partName))
			physicComponent.destroy();
}

void DetailWindow::renderAudioComponent(AudioComponent& audio)
{
	this->m_window->renderText("Audio Component");

	renderProperties(audio);

	string audioPath = audio.getFilePath();
	decltype(auto) _ = this->m_window->renderButton(audioPath);
	this->changeAudioComponentAsset(audio);
	this->m_window->onSameLine(0);
	this->m_window->renderText("Sound");

	string
		id = std::to_string(audio.id()),
		partName = "Volume##" + id;

	float vol = audio.getVolume() * 100;
	if (this->m_window->renderSlider(partName, vol, { 0, 100 }))
		audio.setVolume(vol / 100);

	partName = "Remove Component##" + std::to_string(audio.id());
	if (Editor::getInstance()->getState() != E_PLAYING)
		if (this->m_window->renderButton(partName))
		{
			audio.setCanUpdate(false);
			audio.destroy();
		}

}

void DetailWindow::renderSwitchShape(PhysicComponent& physicComponent)
{
	ePhysicShape shape = physicComponent.getShape();
	const char* changeShape = "Change Shape";
	bool button = false;

	switch (shape)
	{
	case E_NO_SHAPE:
		button = this->m_window->renderButton("No Shape");
		break;
	case E_BOX_SHAPE:
		button = this->m_window->renderButton("Box");
		break;
	case E_SPHERE_SHAPE:
		button = this->m_window->renderButton("Sphere");
		break;
	case E_CAPSULE_SHAPE:
		button = this->m_window->renderButton("Capsule");
		break;
	default:
		break;
	}

	if (button)
		this->m_window->InitPopUp(changeShape);

	if (this->m_window->beginPopUp(changeShape))
	{
		if (this->m_window->selectable("None"))
			physicComponent.setShape(E_NO_SHAPE);

		if (this->m_window->selectable("Box"))
			physicComponent.setShape(E_BOX_SHAPE);

		if (this->m_window->selectable("Sphere"))
			physicComponent.setShape(E_SPHERE_SHAPE);

		if (this->m_window->selectable("Capsule"))
			physicComponent.setShape(E_CAPSULE_SHAPE);

		this->m_window->endPopUp();
	}

	this->m_window->onSameLine(0);
	this->m_window->renderText("Collision Shape");
}

void DetailWindow::renderSwitchTag(PhysicComponent& physic)
{
	if (this->m_creatingTag)
		return this->renderCreatingTag(physic);

	const char* changeTag = "Change Tag";

	string tag = physic.getTag();

	if (tag == "")
	{
		physic.setTag("None");
		tag = "None";
	}

	if (this->m_window->renderButton(tag))
		this->m_window->InitPopUp(changeTag);

	if (this->m_window->beginPopUp(changeTag))
	{
		std::unordered_map<string, unsigned int>& um = Application::getWindow().currentScene()->getPhysicHandler().getTags();

		for (auto it = um.begin(); it != um.end(); it++)
		{
			if (this->m_window->beginMenu(it->first))
			{
				if (this->m_window->addMenuItem("Set Tag"))
					physic.setTag(it->first);

				if (this->m_window->addMenuItem("Delete Tag"))
				{
					if (tag == it->first)
						physic.setTag("None");

					Application::getWindow().currentScene()->getPhysicHandler().removeTag(it->first);
					this->m_window->endMenu();
					break;
				}

				this->m_window->endMenu();
			}
		}

		if (this->m_window->addMenuItem("Create New Tag"))
		{
			this->m_creatingTag = true;
			strcpy(this->m_newTagName, "newTag");
		}

		this->m_window->endPopUp();
	}

	this->m_window->onSameLine(0);
	this->m_window->renderText("Collision Tag");
}

void DetailWindow::renderCreatingTag(PhysicComponent& physic)
{
	if (this->m_window->renderInputText("##rename", this->m_newTagName, NAME_MAX_CHARACTERS))
	{
		string newTagname = this->m_newTagName;
		Application::getWindow().currentScene()->getPhysicHandler().addCollisionsTag(newTagname);
		physic.setTag(newTagname);
		this->m_creatingTag = false;
	}
}

void DetailWindow::renderScale(PhysicComponent& physic)
{
	ePhysicShape shape = physic.getShape();
	Vector3f scale = physic.getScale();

	string id = "##" + std::to_string(physic.id());

	decltype(auto) _ = false;
	switch (shape)
	{
	case E_BOX_SHAPE:
		_ = this->m_window->renderVec3("scale" + id, scale);
		break;
	case E_SPHERE_SHAPE:
		_ = this->m_window->renderInputFloat("Radius" + id, scale.x());
		break;
	case E_CAPSULE_SHAPE:
		_ = this->m_window->renderInputFloat("Radius" + id, scale.x());
		_ = this->m_window->renderInputFloat("Height" +  id, scale.y());
		break;
	default:
		break;
	}

	if (Editor::getInstance()->getState() != E_PLAYING)
		physic.setScale(scale);
}

void DetailWindow::renderRenderComponent(RenderComponent& render)
{
	this->m_window->renderText("RenderComponent");

	string
		id = "##" + std::to_string(render.id()),
		partName = "Render" + id;

	renderProperties(render);

	/*Mesh*/
	string meshPath = render.getMeshPath();
	decltype(auto) _ = this->m_window->renderButton(meshPath);
	if (Editor::getInstance()->getState() != E_PLAYING)
		this->changeRenderComponentMesh("Mesh", render);
	this->m_window->onSameLine(0);
	this->m_window->renderText("Mesh");
	/*Mesh*/

	/*Material*/
	string matPath  = render.getMatPath();
	_ = this->m_window->renderButton(matPath);
	if (Editor::getInstance()->getState() != E_PLAYING)
		this->changeRenderComponentMaterial("Material", render);
	this->m_window->onSameLine(0);
	this->m_window->renderText("Material");
	/*Material*/

	partName = "Remove Component" + id;
	/*Destroy*/
	if (this->m_window->renderButton(partName))
		if (Editor::getInstance()->getState() != E_PLAYING)
			render.destroy();
	/*Destroy*/
}

void DetailWindow::changeRenderComponentMesh(const char* checker, RenderComponent& render)
{
	if (!this->m_window->beginDragAndDropTarget())
		return;

	const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

	if (!asset)
	{
		this->m_window->endDragAndDropTarget();
		return;
	}

	ifstream file(asset, ios::in);

	bool isOpen = file.is_open();

	if (!isOpen)
		cerr << "Failed to open new kasset file source file " << asset << ".\n";
	else
	{
		string content { istreambuf_iterator(file), { } };
		string_view
			realContent = content,
			result;

		if (XmlParser::parseTagName(realContent, result))
			if (result == checker)
				render.mesh() = ResourceManager::get<MeshResource>(asset)->root();
	}

	file.close();
	this->m_window->endDragAndDropTarget();
}

void DetailWindow::changeRenderComponentMaterial(const char* checker, RenderComponent& render)
{
	if (!this->m_window->beginDragAndDropTarget())
		return;

	const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

	if (!asset)
	{
		this->m_window->endDragAndDropTarget();
		return;
	}

	ifstream file(asset, ios::in);

	bool isOpen = file.is_open();

	if (!isOpen)
		cerr << "Failed to open new kasset file source file " << asset << ".\n";
	else
	{
		string content { istreambuf_iterator(file), { } };
		string_view realContent = content;
		string_view result;

		if (XmlParser::parseTagName(realContent, result))
			if (result == checker)
				render.material() = ResourceManager::get<MaterialResource>(asset)->dataPtr();
	}

	file.close();
	this->m_window->endDragAndDropTarget();
}

void DetailWindow::changeAudioComponentAsset(AudioComponent& audio)
{
	if (!this->m_window->beginDragAndDropTarget() || Editor::getInstance()->getState() == E_PLAYING)
		return;

	const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

	if (!asset)
	{
		this->m_window->endDragAndDropTarget();
		return;
	}

	ifstream file(asset, ios::in);

	bool isOpen = file.is_open();

	if (!isOpen)
		cerr << "Failed to open new kasset file source file " << asset << ".\n";
	else
	{
		string content { istreambuf_iterator(file), { } };
		string_view realContent = content;
		string_view result;

		if (XmlParser::parseTagName(realContent, result))
			if (result == "Sound")
				audio.sound() = ResourceManager::get<SoundResource>(asset)->dataPtr();
	}

	file.close();
	this->m_window->endDragAndDropTarget();
}

void DetailWindow::changeUIImageTexture(std::shared_ptr<UIImage>& img)
{
	if (!this->m_window->beginDragAndDropTarget())
		return;

	const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

	if (!asset)
	{
		this->m_window->endDragAndDropTarget();
		return;
	}

	ifstream file(asset, ios::in);

	bool isOpen = file.is_open();

	if (!isOpen)
		cerr << "Failed to open new kasset file source file " << asset << ".\n";
	else
	{
		string content { istreambuf_iterator(file), { } };
		string_view
			realContent = content,
			result;

		if (XmlParser::parseTagName(realContent, result))
			if (result == "Texture")
				img->setID(ResourceManager::get<TextureResource>(asset));
	}

	file.close();
	this->m_window->endDragAndDropTarget();
}

void DetailWindow::renderScriptComponent(ScriptComponent& script)
{
	renderObject(script);

	string
		id = "##" + std::to_string(script.id()),
		partName = "Script" + id;

	/*Script Path*/
	string scriptPath = script.getPath();
	decltype(auto) _ = this->m_window->renderButton(scriptPath);
	if (Editor::getInstance()->getState() != E_PLAYING)
		this->changeScriptComponent(script);
	/*Script Path*/

	partName = "Remove Component" + id;
	/*Destroy*/
	if (Editor::getInstance()->getState() != E_PLAYING)
		if (this->m_window->renderButton(partName))
		{
			script.setCanUpdate(false);
			script.destroy();
		}
	/*Destroy*/
}

void DetailWindow::changeScriptComponent(ScriptComponent& script)
{
	if (!this->m_window->beginDragAndDropTarget())
		return;

	const char* asset = this->m_window->getDragNDropValue<const char>("Kasset");

	if (asset)
	{
		ifstream file(asset, ios::in);

		bool isOpen = file.is_open();

		if (!isOpen)
			cerr << "Failed to open new kasset file source file " << asset << ".\n";
		else
		{
			string content { istreambuf_iterator(file), { } };
			string_view
				realContent = content,
				result;

			if (XmlParser::parseTagName(realContent, result))
				if (result == "Script")
					script.changeScript(asset);
		}

		file.close();
	}

	this->m_window->endDragAndDropTarget();
}

void DetailWindow::renderAddComponentButton(GameObject& obj, const bool physic)
{
	const char* addComponent = "Add component";

	if (this->m_window->renderButton(addComponent))
		this->m_window->InitPopUp(addComponent);

	if (!this->m_window->beginPopUp(addComponent))
		return;

	Scene* parentScene = obj.parentScene();
	DirectionalLightBuffer* dirBuffer = parentScene ? &parentScene->directionalLightBuffer() : nullptr;
	PointLightBuffer* pointBuffer = parentScene ? &parentScene->pointLightBuffer() : nullptr;

	for (string& type : Component::getTypes())
	{
		if (physic && type == "PhysicComponent")
			continue;

		if (dirBuffer && dirBuffer->getCount() >= DirectionalLightBuffer::MaxLights && type == "DirectionalLightComponent")
			continue;

		if (pointBuffer && pointBuffer->getCount() >= PointLightBuffer::MaxLights && type == "PointLightComponent")
			continue;

		if (this->m_window->selectable(type))
			Component::create(type, obj);
	}

	this->m_window->endPopUp();
}

void DetailWindow::renderProperties(ObjectBase& obj)
{
	std::vector<Property> props;
	obj.getProperties(props);

	for (Property prop : props)
	{
		const string
			id    = format("##{}{}", obj.id(), prop.name()),
			label = prop.name() + id;

		if (prop.canRead<bool>())
		{
			const bool writable = prop.canWrite<bool>();
			bool value = prop.get<bool>();

			if (this->m_window->renderCheckBox(label, value, !writable))
				prop.set(value);
		}
		else if (prop.canRead<float>())
		{
			const bool writable = prop.canWrite<float>();
			float value = prop.get<float>();

			if (this->m_window->renderInputFloat(label, value, !writable))
				prop.set(value);
		}
		else if (prop.canRead<unsigned int>())
		{
			const bool writable = prop.canWrite<unsigned int>();
			const unsigned int original = prop.get<unsigned int>();
			int cpy = original;

			if (prop.canWrite<unsigned int>() && this->m_window->renderInt(label, cpy, !writable))
				if (cpy >= 0)
					prop.set(cpy);
		}
		else if (prop.canRead<unsigned long long>())
		{
			const bool writable = prop.canWrite< unsigned long long>();
			const unsigned int original = prop.get<unsigned long long>();
			int cpy = original;

			if (!writable)
				this->m_window->renderText(prop.name() + ": " + std::to_string(original));
			else if (this->m_window->renderInt(label, cpy, !writable))
				if (cpy >= 0)
					prop.set(cpy);
		}
		else if (prop.canRead<Vector3f>())
		{
			const bool writable = prop.canWrite<Vector3f>();
			Vector3f value = prop.get<Vector3f>();

			if (this->m_window->renderVec3(label, value, !writable))
				prop.set(value);
		}
		else if (prop.canRead<Vector2f>())
		{
			const bool writable = prop.canWrite<Vector2f>();
			Vector2f value = prop.get<Vector2f>();

			if (this->m_window->renderVec2(label, value))
				prop.set(value);
		}
		else if (prop.canRead<string>())
		{
			const bool writable = prop.canWrite<string>();
			const string original = prop.get<string>();

			if(!writable)
			{
				this->m_window->renderText(format("{}: {}", prop.name(), original));
				continue;
			}

			string value = original;

			if (this->m_window->renderInputText(label.c_str(), value.data(), NAME_MAX_CHARACTERS))
				prop.set(value);
		}
		else if (prop.canRead<Color>())
		{
			const bool writable = prop.canWrite<Color>();
			Color value = prop.get<Color>();

			if (this->m_window->renderColorPicker(label, value, !writable))
				prop.set(value);
		}
		else if (prop.canRead<Transform>())
		{
			const bool writable = prop.canWrite<Transform>();
			Transform original  = prop.get<Transform>();

			this->m_window->renderText(prop.name(), true);

			if (renderTransform(&obj, original, id, !writable))
				prop.set(original);
		}
		else
			this->m_window->renderText(format(
			"Property \"{}\" type is unsupported type {}.",
			prop.name(), prop.type().name()));
	}
}

void DetailWindow::renderObject(ObjectBase& obj)
{
	this->m_window->renderText(obj.typeName());
	renderProperties(obj);
}
