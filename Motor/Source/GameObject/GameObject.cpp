#include "GameObject/GameObject.h"

#include "Component/PhysicComponent.h"
#include "GameObject/ObjectBase.hpp"
#include "Inspector/Property.hpp"
#include "Scene/Scene.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"
#include "Utils/RemoveVector.hpp"

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::string;

OBJECTBASE_IMPL(GameObject)

decltype(GameObject::s_createFuncs) GameObject::s_createFuncs = {};
decltype(GameObject::s_deserializeFuncs) GameObject::s_deserializeFuncs = {};

GameObject::GameObject(ConstructorBlocker, const Id id) : ObjectBase(id),
	deletePolicy(eDeletePolicy::DELETE),
	m_components(RemoveVectorStatusSource<Component>::createRemoveVector<std::shared_ptr<Component>>()),
	m_parent(nullptr),
	m_scene(nullptr),
	m_children(RemoveVectorStatusSource<GameObject>::createRemoveVector<std::shared_ptr<GameObject>>()),
	m_physicBody(nullptr) { }

_Ret_maybenull_
std::shared_ptr<GameObject> GameObject::load(const XmlNode& node)
{
	const auto createIt = s_deserializeFuncs.find(node.name);

	if (createIt == s_deserializeFuncs.end())
	{
		cerr << __FUNCTION__": Unknown game object type \"" << node.name << "\".\n";
		return nullptr;
	}

	return createIt->second(node);
}

GameObject::GameObject(ConstructorBlocker) : GameObject(ConstructorBlocker(), s_nextId++) { }

_Success_(return) std::shared_ptr<GameObject> GameObject::create(const string& typeName)
{
	if (const auto it = s_createFuncs.find(typeName); it != s_createFuncs.end())
		return it->second();
	else
	{
		cerr << __FUNCTION__": Unknown game object type \"" << typeName << "\".\n";
		return nullptr;
	}
}

std::vector<string> GameObject::getTypes()
{
	std::vector<string> types;
	types.reserve(s_createFuncs.size());

	for (const std::pair<string, std::function<CreateFunc>>& pair : s_createFuncs)
		types.emplace_back(pair.first);

	return types;
}

GameObject::~GameObject()
{
	for (GameObject& child : m_children)
		switch (child.deletePolicy)
		{
			case eDeletePolicy::DELETE:
				child.~GameObject();
				break;
			case eDeletePolicy::DETACH:
				child.attachTo(m_parent, false, eDeletePolicy::DETACH);
				break;
		}
}

void GameObject::destroy()
{
	if (isRoot())
		throw std::logic_error("Cannot manually destroy scene root object.");

	detach();
}

void GameObject::start()
{
	if (!validateStart())
		return;

	m_started = true;

	for (Component& component : m_components)
		component.start();

	for (GameObject& child : m_children)
		child.start();
}

void KaputEngine::GameObject::onCollision(_In_ PhysicComponent* other)
{
	ObjectBase::onCollision(other);

	for (Component& component : m_components)
	{
		std::string name = component.typeName();
		if (name == "PhysicComponent")
			continue;

		component.onCollision(other);
	}
}

void KaputEngine::GameObject::onCollisionEnter(_In_ PhysicComponent* other)
{
	ObjectBase::onCollisionEnter(other);

	for (Component& component : m_components)
	{
		std::string name = component.typeName();
		if (name == "PhysicComponent")
			continue;

		component.onCollisionEnter(other);
	}
}

void KaputEngine::GameObject::onCollisionExit()
{
	ObjectBase::onCollisionExit();

	for (Component& component : m_components)
	{
		std::string name = component.typeName();
		if (name == "PhysicComponent")
			continue;

		component.onCollisionExit();
	}
}

_Ret_maybenull_ std::shared_ptr<GameObject> GameObject::findChild(const Id& id) const
{
	for (GameObject& child : m_children)
	{
		if (child.id() == id)
			return child.shared_from_this();
		else
			if (std::shared_ptr<GameObject> foundChild = child.findChild(id))
				return foundChild;
	}

	return nullptr;
}

const GameObject::ComponentList& GameObject::components() const noexcept
{
	return m_components;
}

void GameObject::removeComponent(Component& component)
{
	m_components.erase(component);
}

_Ret_maybenull_ GameObject* GameObject::parentObject() const noexcept
{
	return m_parent;
}

_Ret_maybenull_ Scene* GameObject::parentScene() const noexcept
{
	return m_scene;
}

const RemoveVector<GameObject, std::shared_ptr<GameObject>>& GameObject::children() const noexcept
{
	return m_children;
}

void GameObject::attachTo(
	_In_opt_ GameObject* const parent, const bool adjustWorldTransform, const eDeletePolicy policy)
{
	// Attaching to a null parent is a shorthand for detaching
	if (!parent)
	{
		detach(nullptr);
		return;
	}

	Transform oldWorld = getWorldTransform();

	detach(parent->m_scene);
	m_parent = parent;
	deletePolicy = policy;

	if (!adjustWorldTransform)
	{
		// Update local to keep same world transform with new parent
		setWorldTransform(oldWorld);
	}

	parent->m_children.push_back(this->shared_from_this());
}

void GameObject::detach()
{
	detach(nullptr);
}

void GameObject::detach(_In_opt_ Scene* const newScene)
{
	switchScene(newScene);

	if (m_parent)
	{
		m_parent->m_children.erase(*this);
		m_parent = nullptr;
	}
}

void GameObject::registerQueues(Scene& scene)
{
	ObjectBase::registerQueues(scene);

	for (Component& component : m_components)
		component.registerQueues(scene);
}

void GameObject::unregisterQueues()
{
	ObjectBase::unregisterQueues();

	for (Component& component : m_components)
		component.unregisterQueues();
}

void GameObject::setWorldTransform(const Transform& transform) noexcept
{
	TransformSource::setWorldTransform(transform);

	if (this->m_physicBody)
		this->m_physicBody->setTransform(transform);

	for (Component& component : m_components)
		if (TransformSource* transformSource = dynamic_cast<TransformSource*>(&component))
			transformSource->setTransformDirty();

	// Only mark the children as dirty and not this as the world transform is provided
	for (GameObject& child : m_children)
		switchScene(m_scene);
}

void GameObject::updateWorldTransform() const noexcept
{
	MatrixTransformSource::updateWorldTransform();
}

void GameObject::setWorldTransformWithoutPhysic(const Transform& transform) noexcept
{
	TransformSource::setWorldTransform(transform);

	// Only mark the children as dirty and not this as the world transform is provided
	for (GameObject& child : m_children)
		switchScene(m_scene);
}

_Ret_maybenull_ TransformSource* GameObject::getParentTransform() const
{
	if(this->m_parent)
		this->m_parent->updateWorldTransform();
	return m_parent;
}

RemoveVectorStatus& GameObject::getParentStatus() const noexcept
{
	return m_parentStatus;
}

void GameObject::setTransformDirty() const noexcept
{
	// Skip updating branches already marked dirty
	if (isTransformDirty())
		return;

	m_dirtyTransform = true;

	for (Component& component : m_components)
		if (TransformSource* transformSource = dynamic_cast<TransformSource*>(&component))
			transformSource->setTransformDirty();

	for (GameObject& child : m_children)
		child.setTransformDirty();
}

void GameObject::switchScene(_In_opt_ Scene* const newScene)
{
	bool recurse = false;

	// Skip updating branches already marked dirty
	if (!isTransformDirty())
	{
		m_dirtyTransform = true;
		recurse = true;
	}

	if (m_scene != newScene)
	{
		recurse = true;

		if (m_scene)
			unregisterQueues();

		m_scene = newScene;

		if (newScene)
		{
			if (newScene->started())
				start();

			registerQueues(*newScene);
		}
	}

	if (!recurse)
		return;

	for (Component& component : m_components)
		if (TransformSource* transformSource = dynamic_cast<TransformSource*>(&component))
			transformSource->setTransformDirty();

	for (GameObject& child : m_children)
		child.switchScene(newScene);
}

bool GameObject::isRoot() const noexcept
{
	return !m_parent;
}

void GameObject::serializeValues(XmlSerializeContext& context) const
{
	ObjectBase::serializeValues(context);
	TransformSource::serializeValues(context);

	context.value("DeletePolicy", static_cast<uint8_t>(deletePolicy));

	if (!m_components.empty())
	{
		context.startObject("Components");
		for (const Component& component : m_components)
			component.serializeObject(context);
		context.endObject();
	}

	if (!m_children.empty())
	{
		context.startObject("Children");
		for (const GameObject& child : m_children)
			child.serializeObject(context);
		context.endObject();
	}
}

void GameObject::setPhysicComponentPtr(_In_opt_ PhysicComponent* ptr)
{
	this->m_physicBody = ptr;
}

_Ret_maybenull_ PhysicComponent* GameObject::getPhysicComponent()
{
	return this->m_physicBody;
}

bool GameObject::deserializeMap(const XmlNode::Map& map)
{
	if (!ObjectBase::deserializeMap(map))
		return false;

	if (!TransformSource::deserializeMap(map))
		return false;

	if (const auto it = map.find("DeletePolicy"); it != map.end())
	{
		std::optional<uint8_t> op = it->second->parse<uint8_t>();

		if (!op)
		{
			deserializeError(__FUNCTION__, "DeletePolicy");
			return false;
		}

		deletePolicy = static_cast<eDeletePolicy>(*op);
	}

	if (const auto it = map.find("Components"); it != map.end())
	{
		const std::vector<XmlNode>* children = std::get_if<std::vector<XmlNode>>(&it->second->body);

		if (!children)
		{
			cerr << __FUNCTION__": Expected nodes in Components node.\n";
			return false;
		}

		for (const XmlNode& node : *children)
			Component::load(node, *this);
	}

	if (const auto it = map.find("Children"); it != map.end())
	{
		const std::vector<XmlNode>* children = std::get_if<std::vector<XmlNode>>(&it->second->body);

		if (!children)
		{
			cerr << __FUNCTION__": Expected nodes in Children node.\n";
			return false;
		}

		for (const XmlNode& node : *children)
			if (std::shared_ptr<GameObject> child = GameObject::load(node))
				child->attachTo(this, true, child->deletePolicy);
	}

	return true;
}

void GameObject::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("World Transform",
	[this]() { return getWorldTransform(); },
	[this](const Transform& value) { setWorldTransform(value); });
}
