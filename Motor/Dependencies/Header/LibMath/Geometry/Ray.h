#pragma once

#include "../Vector.h"
#include "Shape.h"

namespace LibMath
{
	template <ArrIndex D, ArrayData TData = DefaultDataType>
	class Ray : public Shape<D, 2, TData>
	{
		THIS_DEF(Ray);
		DEFINE_SAMPLES(2);

	public:
		explicit Ray(typename This::PointType origin, typename This::VectorType direction)
			: Shape<D, 2, TData>(), m_Samples { origin, direction.normalize().template as<Cartesian>() } { }

		_NODISCARD typename This::PointType* points() noexcept override
		{
			return &origin();
		}

		_NODISCARD const typename This::PointType* points() const noexcept override
		{
			return &origin();
		}

		_NODISCARD typename This::PointType& origin()
		{
			return m_Samples[0];
		}

		_NODISCARD const typename This::PointType& origin() const
		{
			return m_Samples[0];
		}

		_NODISCARD typename This::VectorType& direction() noexcept
		{
			return m_Samples[1].template as<Vector>();
		}

		_NODISCARD const typename This::VectorType& direction() const noexcept
		{
			return m_Samples[1].template as<Vector>();
		}

		typename This::LineType toLine(PrecisionType<TData> length) const
		{
			return typename This::LineType(origin(), origin + (direction() * length));
		}
	};

	template <ArrayData TData>
	using Ray2D = Ray<2, TData>;

	template <ArrayData TData>
	using Ray3D = Ray<3, TData>;
}
