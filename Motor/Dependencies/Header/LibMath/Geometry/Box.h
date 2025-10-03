#pragma once

#include "Rect.h"

namespace LibMath
{
	/// <summary>
	/// Axis-aligned 3D bounding box
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	class Box : public Rect<3, TData>
	{
		THIS_DEF(Box);

	public:
		explicit Box(const typename This::PointType center, const typename This::LinearType sizes)
			: Rect<3, TData>(center, sizes) { }
		
		_NODISCARD typename This::ConstPointPtr points(typename This::PointType buffer[This::PointCount]) const override
		{
			Extents<TData> ext[3];

			for (ArrIndex axis = 0; axis < 3; axis++)
				ext[axis] = this->extents(axis);

			for (int i = 0; i < 2; i++)
			{
				const int offset = i + 1;
				
				buffer[0 * offset] = typename This::PointType { ext[0].min, ext[2][i] };
				buffer[1 * offset] = typename This::PointType { ext[0].max, ext[2][i] };
				buffer[2 * offset] = typename This::PointType { ext[0].max, ext[2][i] };
				buffer[3 * offset] = typename This::PointType { ext[0].min, ext[2][i] };
			}

			return reinterpret_cast<typename This::ConstPointPtr>(*buffer);
		}
	};

	using Boxf = Box<DefaultDataType>;
}
