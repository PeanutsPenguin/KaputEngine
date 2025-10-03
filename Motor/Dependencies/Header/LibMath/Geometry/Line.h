#pragma once

#include "../Vector.h"
#include "Shape.h"

namespace LibMath
{
	/// <summary>
	/// Line segment from one Cartesian point to another
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData = DefaultDataType>
	class Line : public Shape<D, 2, TData>
	{
		THIS_DEF(Line);
		DEFINE_SAMPLES(2);
		DEFINE_PT_AS_SAMPLE;

	public:
		explicit Line(typename This::PointType a, typename This::PointType b)
			: Shape<D, 2, TData>(), m_Samples { a, b } { }

		/// <summary>
		/// Generates the vector representing the movement of the line.
		/// </summary>
		_NODISCARD typename This::VectorType toVector() const noexcept
		{
			const typename This::PointType(&pts)[2] = points();
			return pts[1] - pts[0];
		}

		/// <summary>
		/// Finds the length of the line.
		/// </summary>
		/// <typeparam name="PrecisionType">Type to use for distance calculations</typeparam>
		template <ArrayData PrecisionType = PrecisionType<TData>>
		_NODISCARD PrecisionType length() const
		{
			return toVector().template magnitude<PrecisionType>();
		}

		/// <summary>
		/// Finds the square of the length of the line.
		/// </summary>
		/// <typeparam name="PrecisionType">Type to use for distance calculations</typeparam>
		_NODISCARD typename This::DataType squareLength() const
		{
			return toVector.squareLength();
		}

		/// <summary>
		/// Converts the line to a ray.
		/// </summary>
		typename This::RayType toRay() const
		{
			return typename This::RayType(m_Samples[0], m_Samples[1] - m_Samples[0]);
		}
	};

	/// <summary>
	/// 2D line segment
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	using Line2D = Line<2, TData>;

	/// <summary>
	/// 2D line segment using floating-points
	/// </summary>
	using Line2Df = Line2D<DefaultDataType>;

	/// <summary>
	/// 3D line segment
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	using Line3D = Line<3, TData>;

	/// <summary>
	/// 3D line segment using floating-points
	/// </summary>
	using Line3Df = Line3D<DefaultDataType>;

	/// <summary>
	/// 4D line segment
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	using Line4D = Line<4, TData>;

	/// <summary>
	/// 4D line segment using floating-points
	/// </summary>
	using Line4Df = Line4D<DefaultDataType>;
}
