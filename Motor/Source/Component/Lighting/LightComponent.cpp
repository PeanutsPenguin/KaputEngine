#include "Component/Lighting/LightComponent.h"

#include "GameObject/GameObject.h"
#include "Inspector/Property.hpp"
#include "Rendering/Color.h"
#include "Rendering/Lighting/LightBuffer.h"
#include "Rendering/Lighting/LightData.h"
#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"

using namespace LibMath;
using namespace KaputEngine::Rendering::Lighting;
using namespace KaputEngine::Text::Xml;

using KaputEngine::LightComponent;
using KaputEngine::Inspector::Property;
using KaputEngine::Rendering::Color;

using std::cerr;

COMPONENT_IMPL(LightComponent)

LightComponent::LightComponent(GameObject& parent) : Component(parent) { }
LightComponent::LightComponent(GameObject& parent, const Id& id) : Component(parent, id) { }

void LightComponent::registerQueues(Scene& scene)
{
	Component::registerQueues(scene);

	LightBuffer& buffer = getSceneBuffer(scene);

	m_index = buffer.registerLight(*this);
	buffer.updateLight(*this);
}

void LightComponent::unregisterQueues()
{
	Component::unregisterQueues();

	LightBuffer& buffer = getSceneBuffer(*parentScene());

	// Disable light
	getBaseData().enabled = false;
	updateThisBuffer();

	buffer.unregisterLight(*this);
}

void LightComponent::updateThisBuffer() const noexcept
{
	if (m_index == -1)
		return;

	LightBuffer& buffer = getSceneBuffer(*parentScene());
	buffer.updateLight(*this);
}

const int& LightComponent::index() const noexcept
{
	return m_index;
}

void LightComponent::serializeValues(XmlSerializeContext& context) const
{
	Component::serializeValues(context);

	const LightData& data = getBaseData();

	context.value("Color", data.color);
	context.value("Enabled", data.enabled);
	context.value("Intensity", data.intensity);
}

bool LightComponent::deserializeMap(const XmlNode::Map& map)
{
	if (!Component::deserializeMap(map))
		return false;

	LightData& data = getBaseData();

	if (const auto it = map.find("Color"); it != map.end())
	{
		const std::optional<Color> op = it->second->parse<Color>();

		if (!op)
		{
			cerr << __FUNCTION__ ": Failed to parse Color.\n";
			return false;
		}

		data.color = *op;
	}

	if (const auto it = map.find("Enabled"); it != map.end())
	{
		const std::optional<bool> op = it->second->parse<bool>();

		if (!op)
		{
			cerr << __FUNCTION__ ": Failed to parse Enabled.\n";
			return false;
		}

		data.enabled = *op;
	}

	if (const auto it = map.find("Intensity"); it != map.end())
	{
		const std::optional<float> op = it->second->parse<float>();

		if (!op)
		{
			cerr << __FUNCTION__ ": Failed to parse Intensity.\n";
			return false;
		}

		data.intensity = *op;
	}

	return true;
}

void LightComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("Enabled",
	[this]() -> bool { return getBaseData().enabled; },
	[this](const bool& value)
	{
		getBaseData().enabled = value;
		updateThisBuffer();
	});

	out.emplace_back("Intensity",
	[this]() -> float { return getBaseData().intensity; },
	[this](const float& value)
	{
		getBaseData().intensity = value;
		updateThisBuffer();
	});

	out.emplace_back("Color",
	[this]() -> Color { return getBaseData().color; },
	[this](const Color& value)
	{
		getBaseData().color = value;
		updateThisBuffer();
	});
}
