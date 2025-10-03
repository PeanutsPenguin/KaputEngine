#pragma once

#include "Source.h"

namespace KaputEngine
{
	class MatrixTransformSource: public TransformSource
	{
	public:
		_NODISCARD const LibMath::Matrix4f& getWorldTransformMatrix() const noexcept;

		void updateWorldTransform() const noexcept override;

	protected:
		mutable LibMath::Matrix4f m_worldTransformMatrix;
	};
}
