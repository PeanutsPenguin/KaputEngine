#pragma once

#include "Internal/LinearMathArrayBase.h"

namespace LibMath
{
	/// <summary>
	/// Linear math array representing a single value with an X component
	/// </summary>
	/// <typeparam name="TData">Data type of the value</typeparam>
	template <ArrayData TData>
	class MathArray<1, 1, TData> : public Internal::LinearMathArrayBase<1, TData>
	{
		THIS_DEF(MathArray)

	public:
		MathArray() : Internal::LinearMathArrayBase<1, TData>() { }
		MathArray(TData value) : Internal::LinearMathArrayBase<1, TData>(value) { }
		
		MATHARRAY_ALIAS(value, 0, 0)

		/// <summary>
		/// Implicit reference conversion of the value
		/// </summary>
		_NODISCARD operator const TData& () const noexcept
		{
			return value();
		}

		/// <summary>
		/// Implicit reference conversion of the value
		/// </summary>
		_NODISCARD operator TData&() noexcept
		{
			return value();
		}
	};

	/// <summary>
	/// Linear math array representing a single value with an X component
	/// </summary>
	/// <typeparam name="TData">Data type of the value</typeparam>
	template <ArrayData TData = DefaultDataType>
	using LinearMathArray1  = LinearMathArray<1, TData>;
	
MATHARRAY_NUMERICALIASES(LinearMathArray1)
}
