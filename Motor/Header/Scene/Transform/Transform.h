#pragma once

#include <LibMath/Matrix.h>
#include <LibMath/Rotor.h>
#include <LibMath/Vector/Vector3.h>

namespace KaputEngine
{
	struct Transform
    {
        LibMath::Cartesian3f position;
        LibMath::Rotorf rotation = LibMath::Vector4f::Dir::hyperUp().as<LibMath::Rotor>();
        LibMath::Vector3f scale  = LibMath::Vector3f::Dir::one();

        _NODISCARD LibMath::Matrix4f toMatrix() const noexcept;

        _NODISCARD Transform inverse() const;

        _NODISCARD Transform combine(const Transform& rValue) const;

        void setRotationEuler(const LibMath::Vector3f& angles);

        _NODISCARD LibMath::Vector3f getRotationEuler() const noexcept;

        _NODISCARD LibMath::Quaternionf fromAxis(const LibMath::Vector3f& axis, float angle);

        _NODISCARD bool operator==(const Transform& rvalue) const noexcept = default;
        _NODISCARD bool operator!=(const Transform& rvalue) const noexcept = default;
    };
}
