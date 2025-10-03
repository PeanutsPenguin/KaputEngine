#pragma once

#include "../MathArray/Concepts.h"

namespace LibMath
{
	/// <summary>
	/// Boundaries of a shape on a given axis
	/// </summary>
	template <ArrayData TData>
	struct Extents
	{
		TData min, max;

		TData& operator[](int index)
		{
			return reinterpret_cast<TData*>(this)[index];
		}

		const TData& operator[](int index) const
		{
			return reinterpret_cast<const TData*>(this)[index];
		}
	};
}
