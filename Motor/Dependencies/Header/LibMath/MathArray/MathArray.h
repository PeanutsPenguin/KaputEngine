#pragma once

#include "Internal/MathArrayBase.h"

namespace LibMath
{
	/// <summary>
	/// 2D numeric array with support for arithmetics
	/// </summary>
	/// <typeparam name="TData">Data type of items</typeparam>
	/// <typeparam name="W">Width of the array</typeparam>
	/// <typeparam name="H">Height of the array</typeparam>
	template <ArrIndex W, ArrIndex H, ArrayData TData = DefaultDataType>
		requires (W >= 1, H >= 1)
	class MathArray : public Internal::MathArrayBase<W, H, TData>
	{
	public:
		MATHARRAY_CTORS(MathArray, Internal::MathArrayBase<W, H, TData>)
	};
}
