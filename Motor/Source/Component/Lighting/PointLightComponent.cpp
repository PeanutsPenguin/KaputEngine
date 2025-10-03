#include "Component/Lighting/PointLightComponent.h"

#include "GameObject/GameObject.h"
#include "Inspector/Property.hpp"
#include "Scene/Scene.h"

using namespace KaputEngine;
using namespace KaputEngine::Rendering::Lighting;
using namespace KaputEngine::Text::Xml;

using namespace LibMath;

using KaputEngine::Inspector::Property;

COMPONENT_IMPL(PointLightComponent)

PointLightComponent::PointLightComponent(GameObject& parent)
	: LightComponent(parent) { }

PointLightComponent::PointLightComponent(GameObject& parent, const Id& id)
	: LightComponent(parent, id) { }

const PointLightData& PointLightComponent::getData() const noexcept
{
	return m_data;
}

LightData& PointLightComponent::getBaseData() noexcept
{
	return m_data;
}

bool PointLightComponent::deserializeMap(const XmlNode::Map& map)
{
	if (!TransformSource::deserializeMap(map))
		return false;

	if (!LightComponent::deserializeMap(map))
		return false;

	return true;
}

const LightData& PointLightComponent::getBaseData() const noexcept
{
	return m_data;
}

void PointLightComponent::setData(const PointLightData& data) noexcept
{
	m_data = data;
	updateThisBuffer();
}

_Ret_notnull_ TransformSource* PointLightComponent::getParentTransform() const
{
	return &m_parentObject;
}

void PointLightComponent::updateWorldTransform() const noexcept
{
	if (!m_dirtyTransform)
		return;

	TransformSource::updateWorldTransform();
	updateThisBuffer();
}

void PointLightComponent::setTransformDirty() const
{
	TransformSource::setTransformDirty();
	updateThisBuffer();
}

void PointLightComponent::setWorldTransform(const Transform& transform) noexcept
{
	TransformSource::setWorldTransform(transform);
	updateThisBuffer();
}

void PointLightComponent::update(double)
{
	// Update the light buffer with the current world position if the light is enabled
	if (m_index > -1 && m_data.enabled)
	{
		bool updateBuffer = m_dirtyTransform;
		updateWorldTransform();

		if (updateBuffer)
			updateThisBuffer();
	}
}

LightBuffer& PointLightComponent::getSceneBuffer(Scene& scene) const noexcept
{
	return scene.pointLightBuffer();
}

void PointLightComponent::getProperties(std::vector<Property>& out) noexcept
{
	Base::getProperties(out);

	out.emplace_back("World Position",
	[this]() -> Vector3f { return getWorldTransform().position.as<Vector>(); },
	[this](const Vector3f& value)
	{
		Transform trans = getWorldTransform();
		trans.position = value;

		setWorldTransform(trans);
	});
}
