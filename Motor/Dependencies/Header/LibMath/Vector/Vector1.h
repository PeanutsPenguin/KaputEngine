#pragma once

#include "Internal/VectorBase.h"

#include "../MathArray/Internal/DirectionAliases.h"

namespace LibMath
{
	template <ArrayData TData>
	class Vector<1, TData> : public Internal::VectorBase<1, TData>
	{
	public:
		using Dir = Internal::DirectionAliases<Vector>;
		
		/// <summary>
		/// Creates a zero vector.
		/// </summary>
		Vector() : Internal::VectorBase<1, TData>() { }

		/// <summary>
		/// Creates a vector with the component set to a given value.
		/// </summary>
		/// <param name="value">Value of the component</param>
		Vector(TData value) : Internal::VectorBase<1, TData>(value) { }
	};

	template <ArrayData TData = DefaultDataType>
	using Vector1 = Vector<1, TData>;

	MATHARRAY_NUMERICALIASES(Vector1)
}
