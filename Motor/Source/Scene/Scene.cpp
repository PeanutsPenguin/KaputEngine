#include "Scene/Scene.h"

#include "Component/Audio/AudioListenerComponent.h"
#include "GameObject/Camera.h"
#include "Queue/Context.h"
#include "Rendering/Lighting/LightBuffer.hpp"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Parser.hpp"
#include "Utils/RemoveVector.hpp"
#include "Window/UIObject.h"


#include <fstream>
#include <glad.h>

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Rendering::Color;
using KaputEngine::Rendering::Lighting::DirectionalLightBuffer;
using KaputEngine::Rendering::Lighting::PointLightBuffer;
using KaputEngine::Queue::ContextQueue;

using std::cerr;
using std::ios;
using std::filesystem::path;
using std::string;
using std::string_view;

_Success_(return) bool Scene::load(const path& path)
{
	std::ifstream file(path, ios::in);

	if (!file.is_open())
	{
		cerr << __FUNCTION__": Failed to open file: " << path << ".\n";
		return false;
	}

	string content(std::istreambuf_iterator(file), { });
	file.close();

	string_view view = content;
	XmlNode document;

	if (!XmlParser::parse(view, document))
	{
		cerr << __FUNCTION__": Failed to parse scene document: " << path << ".\n";
		return false;
	}

	if (document.name != "Scene")
	{
		cerr << __FUNCTION__": XML document is not a scene: " << path << ".\n";
		return false;
	}

	std::optional<XmlNode::Map> mapOp = document.toMap();

	if (!mapOp)
	{
		cerr << __FUNCTION__": Failed to deserialize Scene.\n";
		return false;
	}

	if (!deserializeMap(*mapOp))
	{
		cerr << __FUNCTION__"Failed to load scene.\n";
		return false;
	}

	return true;
}

bool Scene::save(const path& path, const bool indent) const
{
	std::ofstream file(path, ios::out | ios::trunc);

	if (!file.is_open())
	{
		cerr << __FUNCTION__": Failed to open file: " << path << ".\n";
		return false;
	}

	XmlSerializeContext context(file, indent);
	serializeObject(context);

	file.close();

	return true;
}

Scene::Scene():
	m_sceneRoot(*this),
	m_updateQueue(IWorldUpdatable::createRemoveVector()),
	m_renderQueue(IWorldRenderable::createRemoveVector()),
	m_directionalLightBuffer(),
	m_pointLightBuffer()
{
	m_directionalLightBuffer.create(0);
	m_pointLightBuffer.create(1);
}

void Scene::start()
{
	if (m_started)
	{
		cerr << __FUNCTION__"Scene already started.\n";
		return;
	}

	m_started = true;
	m_sceneRoot.start();
}

bool Scene::started() const noexcept
{
	return m_started;
}

SceneRoot& Scene::sceneRoot() noexcept
{
	return m_sceneRoot;
}

const SceneRoot& Scene::sceneRoot() const noexcept
{
	return m_sceneRoot;
}

std::weak_ptr<Camera> Scene::getPrimaryCamera() noexcept
{
	return m_camera;
}

std::weak_ptr<const Camera> Scene::getPrimaryCamera() const noexcept
{
	return m_camera;
}

void Scene::setPrimaryCamera(Camera& camera) noexcept
{
	m_camera = camera.weak_from_this();
}

void Scene::setPrimaryCamera(nullptr_t) noexcept
{
	m_camera = { };
}

std::vector<std::shared_ptr<UIObject>>& Scene::getHUD()
{
	return this->m_HUDObject;
}

void Scene::update(const double deltaTime)
{
	if (!this->started())
		this->start();

	this->m_physics.updatePhysics(deltaTime);

	for (IWorldUpdatable& updatable : m_updateQueue)
		updatable.update(deltaTime);
}

void Scene::render()
{
	if (!m_camera.expired())
		render(*m_camera.lock());
}

void Scene::render(const Camera& camera)
{
	clearBackground();

	for (IWorldRenderable& renderable : m_renderQueue)
		renderable.render(camera);
}

Color& Scene::clearColor() noexcept
{
	return this->m_clearColor;
}

const Color& Scene::clearColor() const noexcept
{
	return this->m_clearColor;
}

void Scene::addUiObject(_In_ const std::shared_ptr<UIObject>& ptr)
{
	this->m_HUDObject.push_back(ptr);
}

_Ret_maybenull_ std::shared_ptr<UIObject> Scene::getUIObjectByName(const std::string& name)
{
	for (std::shared_ptr<UIObject>& ptr :  m_HUDObject)
	{
		if (ptr->name == name)
			return ptr;
	}

	return nullptr;
}

void Scene::clearBackground()
{
	ContextQueue::instance().push([this]()
	{
		glClearColor(m_clearColor.r(), m_clearColor.g(), m_clearColor.b(), m_clearColor.a());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}).wait();
}

PhysicHandler& Scene::getPhysicHandler() noexcept
{
	return this->m_physics;
}

DirectionalLightBuffer& Scene::directionalLightBuffer() noexcept
{
	return m_directionalLightBuffer;
}

PointLightBuffer& Scene::pointLightBuffer() noexcept
{
	return m_pointLightBuffer;
}

void Scene::destroy()
{
	this->m_physics.destroy();
}

_Ret_notnull_ const char* Scene::xmlTypeName() const noexcept
{
	return "Scene";
}

void Scene::serializeValues(XmlSerializeContext& context) const
{
	context.value("Background", m_clearColor);

	if (!m_camera.expired())
		context.value("Camera", m_camera.lock()->id());

	m_sceneRoot.serializeObject(context);

	if (!m_HUDObject.empty())
	{
		context.startObject("HUD");

		for (std::shared_ptr<UIObject> ptr : m_HUDObject)
			ptr->serializeObject(context);

		context.endObject();
	}
	const std::unordered_map<std::string, unsigned int>& map = this->m_physics.getTags();
	int size = map.size();
	context.value("TagSize",size);
	int itNum = 0;

	for (auto& it: map) 
	{
		context.value("Tag" + std::to_string(itNum), it.first);
		++itNum;
	}
}

_Success_(return) bool Scene::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (const auto it = map.find("SceneRoot"); it != map.end())
	{
		const std::optional<XmlNode::Map> mapOp = it->second->toMap();

		if (!mapOp)
		{
			cerr << "Failed to deserialize Scene.\n";
			return false;
		}

		const XmlNode::Map& map = *mapOp;

		if (!m_sceneRoot.deserializeMap(map))
		{
			cerr << __FUNCTION__": Failed to parse Root.\n";
			return false;
		}
	} else
	{
		cerr << __FUNCTION__": Scene does not define SceneRoot.\n";
		return false;
	}

	if (const auto it = map.find("Background"); it != map.end())
	{
		std::optional<Color> op = it->second->parse<Color>();

		if (!op)
		{
			cerr << __FUNCTION__": Failed to parse Background.\n";
			return false;
		}

		m_clearColor = *op;
	}

	if (const auto it = map.find("Camera"); it != map.end())
	{
		std::optional<Id> op = it->second->parse<Id>();

		if (!op)
		{
			cerr << __FUNCTION__": Failed to parse Camera.\n";
			return false;
		}

		std::shared_ptr<GameObject> camObj = m_sceneRoot.findChild(*op);

		if (!camObj)
		{
			std::cerr << __FUNCTION__": No game object matching camera id" << *op << ".\n";
			return false;
		}

		std::shared_ptr<Camera> camCast = std::dynamic_pointer_cast<Camera>(camObj);

		if (!camCast)
		{
			cerr << __FUNCTION__": Game object with id " << *op << " is not a camera.\n";
			return false;
		}

		m_camera = camCast;
	}

	if (const auto it = map.find("HUD"); it != map.end())
	{
		const std::vector<XmlNode>* children = std::get_if<std::vector<XmlNode>>(&it->second->body);

		if (!children)
		{
			cerr << __FUNCTION__": Expected nodes in HUD node.\n";
			return false;
		}

		for (const XmlNode& node : *children)
			if (std::shared_ptr<UIObject> child = UIObject::load(node))
			{
				this->m_HUDObject.push_back(child);
			}
	}

	if (!this->parseTags(map))
		return false;

	return true;
}

bool Scene::parseTags(_In_ const Text::Xml::XmlNode::Map& map)
{
	const auto tagSizeIt = map.find("TagSize");

	if (tagSizeIt == map.end())
	{
		cerr << __FUNCTION__": Required property TagSize missing.\n";
		return true; //Return true so the opening scene doesn't crash and the Tag Map will just be empty 
	}

	const std::optional<int> tagSizeOp = tagSizeIt->second->parse<int>();

	if (!tagSizeOp)
	{
		cerr << __FUNCTION__": Failed to deserialize TagSize.\n";
		return false;
	}

	if (*tagSizeOp == 0)
		return true;

	for (int i = 0; i < *tagSizeOp; ++i)
	{
		const auto tagValueIt = map.find("Tag" + std::to_string(i));

		if (tagValueIt == map.end())
		{
			cerr << __FUNCTION__": Required property Tag missing.\n";
			return false;
		}

		const std::optional<string> tagValueOp = tagValueIt->second->parse<string>();

		if (!tagValueOp)
		{
			cerr << __FUNCTION__": Failed to deserialize Tag.\n";
			return false;
		}

		this->m_physics.addCollisionsTag(*tagValueOp);
	}
	
	return true;
}
