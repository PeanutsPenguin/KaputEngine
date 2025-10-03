#include "Component/Lighting/DirectionalLightComponent.h"

#include "GameObject/ObjectBase.hpp"
#include "Scene/Scene.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Rendering::Lighting;
using namespace KaputEngine::Text::Xml;

using KaputEngine::Inspector::Property;

COMPONENT_IMPL(DirectionalLightComponent)

DirectionalLightComponent::DirectionalLightComponent(GameObject& parent)
	: LightComponent(parent) { }

DirectionalLightComponent::DirectionalLightComponent(GameObject& parent, const Id& id)
	: LightComponent(parent, id) { }

const DirectionalLightData& DirectionalLightComponent::getData() const noexcept
{
	return m_data;
}

const LightData& DirectionalLightComponent::getBaseData() const noexcept
{
	return m_data;
}

void DirectionalLightComponent::setData(const DirectionalLightData& data) noexcept
{
	m_data = data;
	updateThisBuffer();
}

void DirectionalLightComponent::serializeValues(XmlSerializeContext& context) const
{
	LightComponent::serializeValues(context);
	context.value("Direction", m_data.direction);
}

LightBuffer& DirectionalLightComponent::getSceneBuffer(Scene& scene) const noexcept
{
	return scene.directionalLightBuffer();
}

LightData& DirectionalLightComponent::getBaseData() noexcept
{
	return m_data;
}

bool DirectionalLightComponent::deserializeMap(const XmlNode::Map& map)
{
	if (!LightComponent::deserializeMap(map))
		return false;

	if (const auto it = map.find("Direction"); it != map.end())
	{
		const std::optional<Vector3f> op = it->second->parse<Vector3f>();

		if (!op)
		{
			std::cerr << __FUNCTION__ ": Failed to parse Direction.\n";
			return false;
		}

		m_data.direction = *op;
	}

	return true;
}

void DirectionalLightComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("Direction",
	[this]() -> Vector3f { return m_data.direction; },
	[this](const Vector3f& direction) { m_data.direction = direction; });
}
