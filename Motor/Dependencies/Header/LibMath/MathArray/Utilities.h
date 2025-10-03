#pragma once

#include "Concepts.h"

namespace LibMath
{
	/// <summary>
	/// Converts a math array to a different data type.
	/// </summary>
	template <ArrayData TData, IsMathArray TSelf>
	_NODISCARD typename TSelf::template DataSelf<TData> array_convert(const TSelf& self) noexcept
	{
		typename TSelf::template DataSelf<TData> out;

		for (ArrIndex i = 0; i < TSelf::Size; i++)
			out.raw()[i] = TData(self.raw()[i]);

		return out;
	}
}
