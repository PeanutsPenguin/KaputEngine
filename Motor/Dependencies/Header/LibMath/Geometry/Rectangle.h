#pragma once

#include "Rect.h"

namespace LibMath
{
	/// <summary>
	/// Axis-aligned 2D bounding box
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	class Rectangle : public Rect<2, TData>
	{
		THIS_DEF(Rectangle);

	public:
		explicit Rectangle(const typename This::PointType center, const typename This::LinearType sizes)
			: Rect<2, TData>(center, sizes) { }

		_NODISCARD typename This::ConstPointPtr points(typename This::PointType buffer[This::PointCount]) const override
		{
			Extents<TData> ext[2];

			for (ArrIndex axis = 0; axis < 2; axis++)
				ext[axis] = this->extents(axis);

			buffer[0] = typename This::PointType { ext[0].min, ext[1].min };
			buffer[1] = typename This::PointType { ext[0].max, ext[1].min };
			buffer[2] = typename This::PointType { ext[0].max, ext[1].max };
			buffer[3] = typename This::PointType { ext[0].min, ext[1].max };
			
			return reinterpret_cast<typename This::ConstPointPtr>(*buffer);
		}
	};

	using Rectanglef = Rectangle<DefaultDataType>;
}
