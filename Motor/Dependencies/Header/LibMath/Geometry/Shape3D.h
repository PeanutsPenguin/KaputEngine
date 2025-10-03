#pragma once

#include "Internal/ShapeBase.h"

namespace LibMath
{
	/// <summary>
	/// Generic 3D shape
	/// </summary>
	/// <typeparam name="N">Number of samples</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex N, ArrayData TData = DefaultDataType>
	using Shape3D = ShapeBase<3, N, TData>;
}
