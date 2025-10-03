#include "Scene/Transform/Source.h"

#include "Text/Xml/Context.hpp"
#include "Text/Xml/Node.hpp"

using namespace LibMath;
using namespace KaputEngine;
using namespace KaputEngine::Text::Xml;

bool TransformSource::isTransformDirty() const noexcept
{
    return m_dirtyTransform;
}

void TransformSource::setTransformDirty() const
{
	m_dirtyTransform = true;
}

void TransformSource::reverseCalculateLocal(const Transform& parent)
{
    Transform parentinv = parent.inverse();

    this->m_localTransform = this->m_worldTransform.combine(parentinv);
}

const Transform& TransformSource::getLocalTransform() const noexcept
{
    return m_localTransform;
}

void TransformSource::setLocalTransform(const Transform& transform) noexcept
{
    setTransformDirty();
    m_localTransform = transform;
}

void TransformSource::setLocalPosition(const Cartesian3f& position) noexcept
{
    setTransformDirty();
    m_localTransform.position = position;
}

void TransformSource::setLocalRotation(const Rotorf& rotation) noexcept
{
    setTransformDirty();
    m_localTransform.rotation = rotation;
}

void TransformSource::setLocalScale(const Vector3f& scale) noexcept
{
    setTransformDirty();
    m_localTransform.scale = scale;
}

const Transform& TransformSource::getWorldTransform() const noexcept
{
    updateWorldTransform();
    return m_worldTransform;
}

const Transform& TransformSource::getWorldTransformUnsafe() const noexcept
{
    return m_worldTransform;
}

void TransformSource::setWorldTransform(const Transform& transform) noexcept
{
    m_worldTransform = transform;

    if (const TransformSource* parent = getParentTransform())
        this->reverseCalculateLocal(parent->m_worldTransform);

	m_dirtyTransform = false;
}

void TransformSource::updateWorldTransform() const noexcept
{
    // No need to update
    if (!m_dirtyTransform)
        return;

    if (const TransformSource* parent = getParentTransform())
        m_worldTransform = m_localTransform.combine(parent->getWorldTransform());
    else
        m_worldTransform = m_localTransform;

    m_dirtyTransform = false;
}

void TransformSource::serializeValues(XmlSerializeContext& context) const
{
	context.value("Position", m_localTransform.position);
	context.value("Rotation", m_localTransform.rotation);
	context.value("Scale", m_localTransform.scale);
}

bool TransformSource::deserializeMap(const XmlNode::Map& map)
{
    using std::cerr;

    if (const auto it = map.find("Position"); it != map.end())
    {
		std::optional<Cartesian3f> op = it->second->parse<Cartesian3f>();

		if (!op)
		{
			cerr << __FUNCTION__": Failed to parse Position.\n";
			return false;
		}

		m_localTransform.position = *op;
    }

    if (const auto it = map.find("Rotation"); it != map.end())
    {
        std::optional<Rotorf> op = it->second->parse<Rotorf>();

        if (!op)
        {
            cerr << __FUNCTION__": Failed to parse Position.\n";
            return false;
        }

        m_localTransform.rotation = *op;
    }

	if (const auto it = map.find("Scale"); it != map.end())
	{
		std::optional<Vector3f> op = it->second->parse<Vector3f>();

		if (!op)
		{
			cerr << __FUNCTION__": Failed to parse Scale.\n";
			return false;
		}

		m_localTransform.scale = *op;
	}

    setTransformDirty();
    return true;
}
