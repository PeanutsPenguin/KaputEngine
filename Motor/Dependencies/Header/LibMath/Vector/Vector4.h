#pragma once

#include "Internal/VectorBase.h"

#include "../MathArray/Internal/DirectionAliases.h"

namespace LibMath
{
	template <ArrayData TData>
	class Vector<4, TData> : public Internal::VectorBase<4, TData>
	{
	public:
		using Dir = Internal::DirectionAliases<Vector>;
		
		MATHARRAY_CTORS(Vector, Internal::VectorBase<4, TData>)

		_NODISCARD Vector homogenize() const &
		{
			// Copy the vector and homogenize the copy in-place (see && function below)
			return Vector(*this).homogenize();
		}

		_NODISCARD Vector& homogenize() &&
		{
			// Homogenize in-place if this is r-value
			return *this /= this->w();
		}
	};

	template <ArrayData TData = DefaultDataType>
	using Vector4  = Vector<4, TData>;
	
	MATHARRAY_NUMERICALIASES(Vector4)
}
