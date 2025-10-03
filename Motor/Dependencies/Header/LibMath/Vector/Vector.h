#pragma once

#include "../MathArray/Internal/DirectionAliases.h"
#include "Internal/VectorBase.h"

namespace LibMath
{
	template <ArrIndex D, ArrayData TData = DefaultDataType>
	class Vector : public Internal::VectorBase<D, TData>
	{
	public:
		using Dir = Internal::DirectionAliases<Vector>;

		MATHARRAY_CTORS(Vector, Internal::VectorBase<D, TData>)
	};
}
