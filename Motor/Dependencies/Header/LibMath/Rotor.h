#pragma once

#include "Angle/Radian.h"
#include "Point/Cartesian.h"
#include "Quaternion.h"
#include "Trigonometry.h"

namespace LibMath
{
    template <ArrayData TData = DefaultDataType>
    class Rotor : public Quaternion<TData>
    {
    public:
        using AxisType  = Vector3<TData>;
        using PointType = Cartesian3<TData>;
        using QuaternionType = Quaternion<TData>;

        MATHARRAY_CTORS(Rotor, Quaternion<TData>)

        Rotor(const Vector3<TData>& axis, const Radian& angle, const bool axisNormalized = false)
        {
            const Radian halfAngle = angle / 2.f;

            this->w() = cos(halfAngle);
            this->vector() = Vector3<TData>
                { (axisNormalized ? axis : axis.normalize()) * sin(halfAngle) };
        }

        Rotor(const Radian& pitch, const Radian& yaw, const Radian& roll)
        {
            const Rotor pitchRot(AxisType::Dir::right(), pitch),
                        yawRot(AxisType::Dir::up(), yaw),
                        rollRot(AxisType::Dir::front(), roll);

            new (this) Rotor(pitchRot * yawRot * rollRot);
        }

        explicit Rotor(const QuaternionType& quat) : QuaternionType(quat.template as<Vector>().normalize()) { }

        /// <summary>
        /// Finds the axis of rotation.
        /// </summary>
        /// <returns>Axis of rotation. May not be a unit vector.</returns>
        _NODISCARD AxisType getAxis() const
        {
            return this->vector();
        }

        /// <summary>
        /// Finds the axis of rotation as a unit vector.
        /// </summary>
        _NODISCARD AxisType getNormalAxis() const
        {
            const AxisType axis = getAxis();
            return axis / axis.magnitude();
        }

        void setAxis(const AxisType& axis, bool axisNormalized = false)
        {
            const Radian halfAngle = getHalfAngle();

            this->vector() = axisNormalized
                // Not moving *sin outside the ternary to take advantage of in-place multiplication on r-value in the un-normalized branch
                ? axis * sin(halfAngle)
                : axis.normalize() * sin(halfAngle);
        }

        _NODISCARD Radian getAngle() const
        {
            return 2.f * getHalfAngle();
        }

        _NODISCARD Radian getHalfAngle() const
        {
            return Radian(atan2(1.f, this->w()));
        }

        void setAngle(const Radian& angle)
        {
            setHalfAngle(angle / 2.f);
        }

        void setHalfAngle(const Radian& angle)
        {
            this->scalar() = TData(cos(angle / 2.f));
        }

        _NODISCARD PointType rotate(const PointType& point) const
        {
            return (*this * point * this->conjugate()).vector().template as<PointType>();
        }


    };

    MATHARRAY_NUMERICALIASES(Rotor)
}
