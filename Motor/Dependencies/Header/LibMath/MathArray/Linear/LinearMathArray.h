#pragma once

#include "Internal/LinearMathArrayBase.h"

namespace LibMath
{
	template <ArrIndex D, ArrayData TData>
	class MathArray<1, D, TData> : public Internal::LinearMathArrayBase<D, TData>
	{
	public:
		MATHARRAY_CTORS(MathArray, Internal::LinearMathArrayBase<D, TData>)
	};

	template <ArrIndex D, ArrayData TData = DefaultDataType>
	using LinearMathArray = MathArray<1, D, TData>;

	template <ArrIndex D>
	using LinearMathArrayf = LinearMathArray<D, float>;

	template <ArrIndex D>
	using LinearMathArrayd = LinearMathArray<D, double>;
}
