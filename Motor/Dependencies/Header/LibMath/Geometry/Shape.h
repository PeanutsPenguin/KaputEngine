#pragma once

#include "Internal/ShapeBase.h"

namespace LibMath
{
	/// <summary>
	/// Generic shape
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="N">Number of samples</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrIndex N, ArrayData TData>
	class Shape : public ShapeBase<D, N, TData> { };
}
