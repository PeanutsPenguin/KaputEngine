#pragma once

#include "Internal/VectorBase.h"

#include "../MathArray/Internal/DirectionAliases.h"

namespace LibMath
{
	template <ArrayData TData>
	class Vector<3, TData> : public Internal::VectorBase<3, TData>
	{
	public:
		using Dir = Internal::DirectionAliases<Vector>;

		MATHARRAY_CTORS(Vector, Internal::VectorBase<3, TData>)

		/// <summary>
		/// Gets the cross product of the vector with another.
		/// </summary>
		_NODISCARD Vector cross(const Vector& vect) const noexcept
		{
			return Vector
			{
				this->y() * vect.z() - this->z() * vect.y(),
				this->z() * vect.x() - this->x() * vect.z(),
				this->x() * vect.y() - this->y() * vect.x()
			};
		}
	};

	template <ArrayData TData = DefaultDataType>
	using Vector3 = Vector<3, TData>;

	MATHARRAY_NUMERICALIASES(Vector3)
}
