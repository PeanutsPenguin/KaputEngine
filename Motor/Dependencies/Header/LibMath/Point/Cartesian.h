#pragma once

#include "../MathArray/Internal/DirectionAliases.h"
#include "../MathArray/Internal/PointAliases.h"
#include "../MathArray/Linear/LinearMathArray.h"
#include "../Vector.h"

namespace LibMath
{
	template <ArrIndex D, ArrayData TData = DefaultDataType>
	class Cartesian :
		public Internal::PointAliases<D, TData>,
		public LinearMathArray<D, TData>
	{
		THIS_DEF(Cartesian)

	public:
		using Dir = Internal::DirectionAliases<Cartesian>;

		MATHARRAY_CTORS(Cartesian, LinearMathArray<D, TData>)
		MATHARRAY_SELFDEFS_LINEAR(MathArray, Cartesian)

		template <IsMathArray<1, D, TData> T>
		static consteval bool CanDefaultSubtractArray([[ maybe_unused ]] bool rValue)
		{
			return !std::derived_from<T, Cartesian>;
		}

		_NODISCARD static constexpr Cartesian Origin()
		{
			return Cartesian();
		}

		_NODISCARD Vector<D, TData> operator-(const Cartesian& right) const
		{
			return *this - right.template as<Vector>();
		}
	};

	template <ArrayData TData = DefaultDataType>
	using Cartesian1 = Cartesian<1, TData>;

	template <ArrayData TData = DefaultDataType>
	using Cartesian2 = Cartesian<2, TData>;

	template <ArrayData TData = DefaultDataType>
	using Cartesian3 = Cartesian<3, TData>;

	template <ArrayData TData = DefaultDataType>
	using Cartesian4 = Cartesian<4, TData>;

	MATHARRAY_NUMERICALIASES(Cartesian1)
	MATHARRAY_NUMERICALIASES(Cartesian2)
	MATHARRAY_NUMERICALIASES(Cartesian3)
	MATHARRAY_NUMERICALIASES(Cartesian4)
}
