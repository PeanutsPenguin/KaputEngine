#include "Scene/Transform/Transform.h"

#include <LibMath/Angle/Degree.h>
#include <LibMath/Angle/Radian.h>
#include <LibMath/Matrix.h>
#include <LibMath/Rotor.h>
#include <LibMath/Transform.h>

using KaputEngine::Transform;

using LibMath::Degree;
using LibMath::Radian;
using LibMath::Matrix4f;
using LibMath::Quaternionf;
using LibMath::Transformf;
using LibMath::Vector3f;

Matrix4f Transform::toMatrix() const noexcept
{
    return Transformf::generateTranslate(position) *
           Transformf::generateRotation(rotation) *
           Transformf::generateScale(scale);
}

Transform Transform::inverse() const
{
    Transform result;

    result.scale = LibMath::Vector3f::Dir::one() / this->scale;
    result.rotation = this->rotation.conjugate();
    result.position = result.rotation.rotate(-this->position);
    result.position /= this->scale;

    return result;
}

Transform Transform::combine(const Transform& rValue) const
{
    Transform result;

    result.position = rValue.rotation.rotate(this->position) * rValue.scale + rValue.position;
    result.rotation = this->rotation * rValue.rotation;
    result.scale = this->scale * rValue.scale;

    return result;
}

void Transform::setRotationEuler(const Vector3f& angles)
{
    Quaternionf qX = this->fromAxis(Vector3f::Dir::right(), angles.x());
    Quaternionf qY = this->fromAxis(Vector3f::Dir::up(), angles.y());
    Quaternionf qZ = this->fromAxis(Vector3f::Dir::front(), angles.z());

    this->rotation = qZ * qY * qX;
}

Vector3f Transform::getRotationEuler() const noexcept
{
    Vector3f result;
    Quaternionf quat = this->rotation.normalize();

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (quat.w() * quat.x() + quat.y() * quat.z());
    double cosr_cosp = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
    result.x() = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = LibMath::squareRoot(1 + 2 * (quat.w() * quat.y() - quat.x() * quat.z()));
    double cosp = LibMath::squareRoot(1 - 2 * (quat.w() * quat.y() - quat.x() * quat.z()));
    result.y() = 2 * std::atan2(sinp, cosp) - LibMath::g_pi / 2;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
    double cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
    result.z() = std::atan2(siny_cosp, cosy_cosp);

    // TODO Convert to loop or LibMath inline function
    result.x() = Radian(result.x()).degree(true);
    result.y() = Radian(result.y()).degree(true);
    result.z() = Radian(result.z()).degree(true);

    return result;
}

Quaternionf Transform::fromAxis(const Vector3f& axis, float angle)
{
    Degree halfAngle(angle * 0.5);

    float  sinHalf = LibMath::sin(halfAngle);
    float  cosHalf = LibMath::cos(halfAngle);

    return Quaternionf(axis * sinHalf, cosHalf);
}
