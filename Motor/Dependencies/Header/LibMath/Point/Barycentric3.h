#pragma once

#include "../Vector/Vector3.h"
#include "Cartesian.h"

namespace LibMath
{
	template <ArrayData TData>
	class Barycentric3 : public LinearMathArray3<TData>
	{
		THIS_DEF(Barycentric3)

	public:
		MATHARRAY_CTORS(Barycentric3, LinearMathArray3<TData>)
		MATHARRAY_SELFDEFS_DATA(MathArray, LinearMathArray, Barycentric3)

		MATHARRAY_ALIAS(s, 0, 0)
		MATHARRAY_ALIAS(t, 0, 1)
		MATHARRAY_ALIAS(u, 0, 2)

		static Barycentric3<PrecisionType<TData>> Map(
			const Cartesian3<TData>& point,
			const Cartesian3<TData> triPoints[3])
		{
			using PData = PrecisionType<TData>;

			Vector3<TData> deltas[]
			{
				triPoints[2] - point,
				triPoints[1] - point,
				triPoints[0] - point
			};

			TData
				d00 = deltas[0].dot(deltas[0]),
				d01 = deltas[0].dot(deltas[1]),
				d11 = deltas[1].dot(deltas[1]),
				d20 = deltas[2].dot(deltas[0]),
				d21 = deltas[2].dot(deltas[1]);

			const PData den = static_cast<PData>(1) / (d00 * d11 - d01 * d01);

			PData
				s = (d11 * d20 - d01 * d21) * den,
				t = (d00 * d21 - d01 * d20) * den;

			return Barycentric3<PData> { s, t, PData(1) - s - t };
		}
	};

	MATHARRAY_NUMERICALIASES(Barycentric3)
}
