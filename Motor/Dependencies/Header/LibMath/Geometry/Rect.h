#pragma once

#include "Extents.h"
#include "Shape.h"

namespace LibMath
{
	/// <summary>
	/// Base type for axis-aligned bounding boxes
	/// </summary>
	/// <typeparam name="D">Numebr of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData = DefaultDataType>
		requires (D >= 2)
	class Rect : public Shape<D, D * 4, TData>
	{
		THIS_DEF(Rect);
		DEFINE_SAMPLES(2);

	public:
		explicit Rect(const typename This::PointType center, const typename This::LinearType sizes)
			: Shape<D, D * 4, TData>(), m_Samples { center, sizes.template as<typename This::PointType>() } { }

		_NODISCARD typename This::PointType& center() noexcept
		{
			return samples()[0];
		}
		_NODISCARD const typename This::PointType& center() const noexcept
		{
			return samples()[0];
		}

		_NODISCARD typename This::PointType& sizes() noexcept
		{
			return samples()[1];
		}
		_NODISCARD const typename This::PointType& sizes() const noexcept
		{
			return samples()[1];
		}

		_NODISCARD Extents<TData> extents(ArrIndex axis) const
		{
			const TData
				axisPos = center()[axis],
				offset = sizes()[axis] / TData(2);

			return Extents<TData> { axisPos - offset, axisPos + offset };
		}
	};

	template <ArrayData TData>
	using Rect2D = Rect<2, TData>;
	using Rect2Df = Rect2D<DefaultDataType>;

	template <ArrayData TData>
	using Rect3D = Rect<3, TData>;
	using Rect3Df = Rect3D<DefaultDataType>;
}
