#pragma once

#include "MathArray/MathArray.h"

namespace LibMath
{
	template <ArrayData TData = DefaultDataType>
	using MathArray1 = MathArray<1, 1, TData>;
	
	template <ArrayData TData = DefaultDataType>
	using MathArray2 = MathArray<2, 2, TData>;
	
	template <ArrayData TData = DefaultDataType>
	using MathArray3 = MathArray<3, 3, TData>;
	
	template <ArrayData TData = DefaultDataType>
	using MathArray4 = MathArray<4, 4, TData>;
	
	MATHARRAY_NUMERICALIASES(MathArray1)
	MATHARRAY_NUMERICALIASES(MathArray2)
	MATHARRAY_NUMERICALIASES(MathArray3)
	MATHARRAY_NUMERICALIASES(MathArray4)
}
