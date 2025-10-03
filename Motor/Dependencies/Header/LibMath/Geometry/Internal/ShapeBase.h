#pragma once

#include "../../MathArray/Concepts.h"
#include "../../Point/Cartesian.h"

// Macro for defining an m_samples array of a specified size while overriding static definitions and overrides from ShapeBase.
// Requires Root to be already defined.
#define DEFINE_SAMPLES(count) \
	public: \
		static const ArrIndex SampleCount = count; \
		_NODISCARD typename This::PointType* samples() noexcept override \
		{ \
			return this->m_Samples; \
		} \
		_NODISCARD const typename This::PointType* samples() const noexcept override \
		{ \
			return this->m_Samples; \
		} \
	protected: \
		typename This::PointType m_Samples[count]; \
	private:

#define DEFINE_PT_AS_SAMPLE \
	public: \
		static const bool PointsAsSamples = true; \
		_NODISCARD  typename This::ConstPointPtr points(typename This::PointType[This::PointCount]) const noexcept override \
		{ \
			return this->points(); \
		} \
		_NODISCARD typename This::ConstPointPtr points() const noexcept \
		{ \
			return reinterpret_cast<typename This::ConstPointPtr>(*this->samples()); \
		} \
	private:

namespace LibMath
{
	template <ArrIndex D, ArrayData TData>
	class Line;

	template <ArrIndex D, ArrayData TData>
	class Ray;

	/// <summary>
	/// Base class for definitions in shape classes and specializations
	/// </summary>
	/// <typeparam name="D">Number of dimensions</typeparam>
	/// <typeparam name="N">Number of samples</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex D, ArrIndex N, ArrayData TData>
	class ShapeBase
	{
	public:
		static constexpr ArrIndex Dimension = D;
		static constexpr ArrIndex PointCount = N;

		using DataType   = TData;
		using PointType  = Cartesian<D, TData>;
		using LineType   = Line<D, TData>;
		using LinearType = LinearMathArray<D, TData>;
		using RayType    = Ray<D, TData>;
		using VectorType = Vector<D, TData>;
		
		using PointPtr = PointType(&)[N];
		using ConstPointPtr = const PointType(&)[N];

		static constexpr bool PointsAsSamples = false;

		static constexpr ArrIndex SampleCount = 0;

		virtual ~ShapeBase() = default;

		/// <summary>
		/// Gets the sample points used to define the shape.
		/// </summary>
		_NODISCARD virtual PointType* samples() noexcept = 0;

		/// <summary>
		/// Gets the sample points used to define the shape.
		/// </summary>
		_NODISCARD virtual const PointType* samples() const noexcept = 0;

		/// <summary>
		/// Gets the points from the samples.
		/// </summary>
		/// <param name="buffer">Pre-allocated space to use if the points require calculation</param>
		/// <returns>Array reference to the points, either the original buffer or an internal array</returns>
		_NODISCARD virtual ConstPointPtr points(PointType buffer[PointCount]) const = 0;
	};
}
