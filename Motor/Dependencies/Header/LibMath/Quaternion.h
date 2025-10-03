#pragma once

#include "MathArray/Linear/LinearMathArray4.h"
#include "Vector/Vector3.h"
#include "MathArray/Internal/PointAliases.h"

namespace LibMath
{
    template <ArrayData TData = DefaultDataType>
    class Quaternion :
        public Internal::PointAliases<4, TData>, // xyzw aliases
        public LinearMathArray4<TData>
    {
        THIS_DEF(Quaternion)
        
    public:
        static constexpr Quaternion Identity()
        {
            return Quaternion { 0, 0, 0, TData(1) };
        }

        template <IsLinearMathArray<4, TData> T>
        static consteval bool CanDefaultMultiplyArray(const bool rValue)
        {
            return !std::derived_from<T, Quaternion> && (rValue || std::derived_from<Cartesian4<TData>, T>);
        }
        
        MATHARRAY_CTORS(Quaternion, LinearMathArray4<TData>)
        
        Quaternion(const Vector3<TData>& vector, const TData& scalar) noexcept
            : LinearMathArray4<TData> { vector.x(), vector.y(), vector.z(), scalar } { }

#pragma region Alias
        MATHARRAY_ALIAS(scalar, 0, 3)

        _NODISCARD Vector3<TData>& vector() noexcept
        {
            // Cast the xyz triplet to Vector3
            return this->template splice<0, 3>().template as<Vector>();
        }

        _NODISCARD const Vector3<TData>& vector() const noexcept
        {
            // Reuse the non-const version
            return const_cast<Quaternion*>(this)->vector();
        }
#pragma endregion

        _NODISCARD bool isUnitQuaternion() const
        {
            // Cast this to Vector4 and borrow its unit function
            return this->template as<Vector>().isUnitVector();
        }

        _NODISCARD Quaternion normalize() const & noexcept
        {
            // Cast this to Vector4 and borrow its normalize function, then cast back to Quaternion
            return this->
                template as<Vector>().normalize().
                template as<Quaternion>();
        }

        _NODISCARD Quaternion& normalize() && noexcept
        {
            // Borrow the r-value version of normalize if this is r-value
            return std::move(this->
                template as<Vector>()).normalize().
                template as<Quaternion>();
        }

        _NODISCARD Quaternion conjugate() const & noexcept
        {
            // Copy the quaternion and apply in-place conjugate on the copy (see && function below)
            return Quaternion(vector() * TData(-1), scalar());
        }

        _NODISCARD Quaternion& conjugate() && noexcept
        {
            // Apply conjugate in-place if this is r-value
            vector() *= TData(-1);
            return *this;
        }
        
#pragma region Operators
        _NODISCARD Quaternion operator*(const Quaternion& right) const &
        {
            return Quaternion(*this) * right;
        }
        
        _NODISCARD Quaternion operator*(const Quaternion& right) &&
        {
            return *this *= right;
        }
        
        Quaternion& operator*=(const Quaternion& right)
        {
            return *this = Quaternion(
                scalar() * right.vector() + right.scalar() * vector() + vector().cross(right.vector()),
                scalar() * right.scalar() - vector().dot(right.vector())
            );
        }

        _NODISCARD Quaternion operator*(const Cartesian3<TData>& right) const
        {
            return Quaternion(*this) *= right;
        }
        
        _NODISCARD Quaternion operator*=(const Cartesian3<TData>& right)
        {
            const auto& rightVect = right.template as<Vector>();
            
            return *this = Quaternion(
               scalar() * rightVect + vector().cross(rightVect),
               -vector().dot(rightVect)
           );
        }
#pragma endregion
    };
    
    MATHARRAY_NUMERICALIASES(Quaternion)
}
