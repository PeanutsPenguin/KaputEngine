#pragma once

#include "../../Angle/Radian.h"
#include "../../Arithmetic.h"
#include "../../LinearMathArray.h"
#include "../../MathArray/Internal/PointAliases.h"

namespace LibMath
{
	template <ArrIndex D, ArrayData TData>
	class Vector;

	namespace Internal
	{
		/// <summary>
		/// Base class for vectors and specializations
		/// </summary>
		/// <typeparam name="D">Size of the vector</typeparam>
		template <ArrIndex D, ArrayData TData>
			requires (D >= 1)
		class VectorBase : public PointAliases<D, TData>, public LinearMathArray<D, TData>
		{
			THIS_DEF(VectorBase)

		protected:
			MATHARRAY_CTORS(VectorBase, LinearMathArray<D, TData>)

		public:
			MATHARRAY_SELFDEFS_LINEAR(MathArray, Vector)
			MATHARRAY_SELFDELF_LINEAR_CONVERT

			/// <summary>
			/// Gets the radian angle between the vector another.
			/// </summary>
			_NODISCARD Radian angleFrom(const Vector<D, TData>& vect) const
			{
				return Radian(acos(dot(vect) / squareRoot(magnitudeSquared() * vect.magnitudeSquared())));
			}

			//float			distanceFrom(Vector<D> const&) const;				// return distance between 2 points
			//float			distanceSquaredFrom(Vector<D> const&) const;		// return square value of the distance between 2 points
			//float			distance2DFrom(Vector<D> const&) const;			// return the distance between 2 points on the X-Y axis only
			//float			distance2DSquaredFrom(Vector<D> const&) const;	// return the square value of the distance between 2 points points on the X-Y axis only

			/// <summary>
			/// Returns the dot product of the vector with another
			/// </summary>
			_NODISCARD TData dot(const Vector<D, TData>& vect) const
			{
				auto sum = TData(0);

				for (ArrIndex i = 0; i < D; ++i)
					sum += this->raw()[i] * vect.raw()[i];

				return sum;
			}

			/// <summary>
			/// Checks if the vector magnitude is greater than the magnitude of another vector.
			/// </summary>
			_NODISCARD bool isLongerThan(const Vector<D, TData>& vect) const
			{
				return magnitudeSquared() > vect.magnitudeSquared();
			}

			/// <summary>
			/// Checks if the vector magnitude is less than the magnitude of another vector.
			/// </summary>
			_NODISCARD bool isShorterThan(const Vector<D, TData>& vect) const
			{
				return magnitudeSquared() < vect.magnitudeSquared();
			}

			/// <summary>
			/// Checks if the vector is a unit vector.
			/// </summary>
			_NODISCARD bool isUnitVector() const
			{
				return almostEqual(magnitudeSquared(), TData(1));
			}

			/// <summary>
			/// Returns the magnitude of the vector.
			/// </summary>
			template <ArrayData PrecisionType = PrecisionType<TData>>
			_NODISCARD PrecisionType magnitude() const
			{
				return squareRoot(magnitudeSquared<PrecisionType>());
			}

			/// <summary>
			/// Returns the square magnitude of the vector.
			/// </summary>
			template <ArrayData PrecisionType = PrecisionType<TData>>
			_NODISCARD PrecisionType magnitudeSquared() const
			{
				auto sum = PrecisionType(0);

				for (const TData& data : this->raw())
					sum += data * data;

				return sum;
			}

			/// <summary>
			/// Creates an equivalent unit vector.
			/// </summary>
			template <ArrayData PrecisionType = PrecisionType<TData>>
			_NODISCARD Vector<D, TData> normalize() const &
			{
				// Copy the vector and normalize the copy in-place (see && function below)
				return Vector<D, TData>(*this).template normalize<PrecisionType>();
			}

			template <ArrayData PrecisionType = PrecisionType<TData>>
			_NODISCARD Vector<D, TData> normalize() &&
			{
				// Normalize in-place if this is r-value
				return this->template as<Vector>() /= magnitude<PrecisionType>();
			}

			/// <summary>
			/// Projects the vector onto another.
			/// </summary>
			template <ArrayData PrecisionType = PrecisionType<TData>>
			_NODISCARD Vector<D, TData> projectOnto(const Vector<D, TData>& b) const &
			{
				return dot(b) / b.template magnitudeSquared<PrecisionType>() * b;
			}

			/// <summary>
			/// Reflects the vector using another vector as an axis.
			/// </summary>
			/// <param name="axis">Axis to reflect from</param>
			/// <param name="unitAxis">Axis is known to be a unit vector</param>
			_NODISCARD Vector<D, TData> reflectOnto(const Vector<D, TData>& axis, bool unitAxis = false) const &
			{
				return Vector<D, TData>(*this).reflectOnto(axis, unitAxis);
			}

			_NODISCARD Vector<D, TData> reflectOnto(const Vector<D, TData>& axis, bool unitAxis = false) &&
			{
				auto& self = this->template as<Vector>();

				if (unitAxis)
					return self -= axis * TData(2) * dot(axis);

				return self -= projectOnto(axis) * TData(2);
			}

			/// <summary>
			/// Scales the vector by given factor.
			/// </summary>
			/// <param name="scale">Component-wise scale factor</param>
			_NODISCARD Vector<D, TData> scale(const Vector<D, TData>& scale) const &
			{
				return this->template as<Vector>() * scale;
			}

			_NODISCARD Vector<D, TData>& scale(const Vector<D, TData>& scale) &&
			{
				// Scale in-place if this is r-value
				return this->template as<Vector>() *= scale;
			}

			/// <summary>
			/// Translates the vector by a given delta.
			/// </summary>
			_NODISCARD Vector<D, TData> translate(const Vector<D, TData>& delta) const &
			{
				return this->template as<Vector>() + delta;
			}

			_NODISCARD Vector<D, TData>& translate(const Vector<D, TData>& delta) &&
			{
				// Translate in-place if this is r-value
				return this->template as<Vector>() += delta;
			}
		};
	}
}
