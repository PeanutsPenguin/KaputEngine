#include "Scene/Transform/MatrixSource.h"

using KaputEngine::MatrixTransformSource;

const LibMath::Matrix4f& MatrixTransformSource::getWorldTransformMatrix() const noexcept
{
    this->updateWorldTransform();

    return m_worldTransformMatrix;
}

void MatrixTransformSource::updateWorldTransform() const noexcept
{
    if (m_dirtyTransform)
        TransformSource::updateWorldTransform();

    m_worldTransformMatrix = m_worldTransform.toMatrix();
}
