#include "Component/Component.h"

#include "GameObject/GameObject.h"

using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

using std::cerr;
using std::string;

COMPONENT_IMPL(Component)

decltype(Component::s_createFuncs) Component::s_createFuncs = { };
decltype(Component::s_deserializeFuncs) Component::s_deserializeFuncs = { };

Component::Component(GameObject& parent) : ObjectBase(),
	m_parentObject(parent) { }

Component::Component(GameObject& parent, const Id& id) : ObjectBase(id),
	m_parentObject(parent) { }

void Component::destroy()
{
	m_parentObject.removeComponent(*this);
}

std::vector<string> Component::getTypes()
{
	std::vector<string> types;
	types.reserve(s_createFuncs.size());

	for (const std::pair<string, std::function<CreateFunc>>& pair : s_createFuncs)
		types.emplace_back(pair.first);

	return types;
}

_Success_(return) Component* Component::create(const string& typeName, GameObject& parent)
{
	if (const auto it = s_createFuncs.find(typeName); it != s_createFuncs.end())
		return &it->second(parent);
	else
	{
		cerr << __FUNCTION__": Unknown component type \"" << typeName << "\".\n";
		return nullptr;
	}
}

_Ret_maybenull_ Component* Component::load(const XmlNode& node, GameObject& parent)
{
	const auto createIt = s_deserializeFuncs.find(node.name);

	if (createIt == s_deserializeFuncs.end())
	{
		cerr << __FUNCTION__": Unknown component type \"" << node.name << "\".\n";
		return nullptr;
	}

	const std::optional<XmlNode::Map> mapOp = node.toMap();

	if (!mapOp)
	{
		cerr << __FUNCTION__": Failed to deserialize " << node.name << ".\n";
		return nullptr;
	}

	return createIt->second(*mapOp, parent);
}

_Ret_notnull_ GameObject* Component::parentObject() const noexcept
{
	return &m_parentObject;
}

_Ret_maybenull_ Scene* Component::parentScene() const noexcept
{
	return m_parentObject.parentScene();
}
