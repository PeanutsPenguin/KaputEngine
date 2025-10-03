#include "Scene/Root.h"

using namespace KaputEngine;

using KaputEngine::Text::Xml::XmlNode;

using std::cerr;

SceneRoot::SceneRoot(Scene& scene): GameObject(ConstructorBlocker())
{
	m_scene = &scene;
}

constexpr const char* SceneRoot::typeName() const noexcept
{
	return TypeName;
}

_Success_(return) bool SceneRoot::deserializeMap(_In_ const XmlNode::Map& map)
{
	if (std::optional<Id> op = deserializeId(map); op)
		m_id = *op;
	else
	{
		deserializeError(__FUNCTION__);
		return false;
	}

	if (!GameObject::deserializeMap(map))
	{
		deserializeError(__FUNCTION__);
		return false;
	}

	return true;
}
