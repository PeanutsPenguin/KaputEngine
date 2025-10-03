#pragma once

#include "../Trigonometry.h"
#include "Shape.h"

namespace LibMath
{
	/// <summary>
	/// Spherical shape
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrayData TData = DefaultDataType>
	class NSphere : public Shape<D, 1, TData>
	{
		THIS_DEF(NSphere)

	public:
		NSphere(typename This::PointType origin, TData radius)
			: Shape<D, 1, TData>(), m_Origin(origin), m_Radius(radius) { }

		_NODISCARD typename This::PointType* samples() noexcept override
		{
			return &m_Origin;
		}
		_NODISCARD const typename This::PointType* samples() const noexcept override
		{
			return &m_Origin;
		}

		_NODISCARD typename This::ConstPointPtr points(typename This::PointType _[This::PointCount]) const noexcept override
		{
			(void)_;
			return reinterpret_cast<typename This::ConstPointPtr>(m_Origin);
		}

		_NODISCARD typename This::PointType& origin()
		{
			return m_Origin;
		}
		_NODISCARD const typename This::PointType& origin() const
		{
			return m_Origin;
		}

		_NODISCARD TData& radius()
		{
			return m_Radius;
		}
		_NODISCARD const TData& radius() const
		{
			return m_Radius;
		}

		_NODISCARD TData radiusSquared() const
		{
			return radius() * radius();
		}

		_NODISCARD TData radiusCubed() const
		{
			return radius() * radius() * radius();
		}

		_NODISCARD TData diameter() const
		{
			return m_Radius * TData(2);
		}

		_NODISCARD TData circumference() const
		{
			return diameter() * g_pi;
		}

		/// <summary>
		/// Gets the 2D area of a circle matching the parameters of the n-sphere.
		/// </summary>
		/// <returns>Area given by 2πr</returns>
		_NODISCARD TData area() const
		{
			return m_Radius * g_pi * TData(2);
		}

		/// <summary>
		/// Gets the 3D volume of a sphere matching the parameters of the n-sphere.
		/// </summary>
		/// <typeparam name="PrecisionType">Type use in volume calculations</typeparam>
		/// <returns>Volume given by 4/3πr³</returns>
		template <ArrayData PrecisionType = PrecisionType<TData>>
		_NODISCARD PrecisionType volume() const
		{
			return PrecisionType(4) / PrecisionType(3) * g_pi * radiusCubed();
		}

	protected:
		typename This::PointType m_Origin;
		TData m_Radius;
	};

	/// <summary>
	/// 2D circle
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	using Circle = NSphere<2, TData>;

	/// <summary>
	/// 2D circle using floating-points
	/// </summary>
	using Circlef = Circle<DefaultDataType>;

	/// <summary>
	/// 3D sphere
	/// </summary>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrayData TData>
	using Sphere = NSphere<3, TData>;

	/// <summary>
	/// 3D sphere using floating-points
	/// </summary>
	using Spheref = Sphere<DefaultDataType>;
}
