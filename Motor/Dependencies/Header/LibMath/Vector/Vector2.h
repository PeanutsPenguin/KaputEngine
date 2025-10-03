#pragma once

#include "Internal/VectorBase.h"

#include "../MathArray/Internal/DirectionAliases.h"

namespace LibMath
{
	template <ArrayData TData>
	class Vector<2, TData> : public Internal::VectorBase<2, TData>
	{
	public:
		using Dir = Internal::DirectionAliases<Vector>;
		
		MATHARRAY_CTORS(Vector, Internal::VectorBase<2, TData>)

		_NODISCARD TData cross(const Vector& vect) const noexcept
		{
			return this->x() * vect.y() - vect.x() * this->y();
		}
	};

	template <ArrayData TData = DefaultDataType>
	using Vector2  = Vector<2, TData>;
	
	MATHARRAY_NUMERICALIASES(Vector2)
}
