#pragma once

#include "Rect.h"

namespace LibMath
{
	template <ArrayData TData>
	class OrientedRectangle : public Rect<2, TData>
	{
		THIS_DEF(OrientedRectangle);

	public:
		template <std::same_as<typename This::PointType>... T>
		explicit OrientedRectangle(Radian rotation, typename This::PointType center, T... sizes)
			: Rect<2, TData>(center, sizes...), rotation(rotation) { }

		Radian rotation;
	};
}
