#pragma once

#include "Concepts.h"
#include "../Arithmetic.h"

#include <type_traits>

// Unified definition of +-*/ operators for math arrays
// Types implementing custom operators should shadow CanDefault- functions to invalidate matching default operators
// (see MathArrayBase.h #region Arithmetic compatibility)
#define MATHARRAY_OPERATORS(op, name, commutative) \
/* Scalar, Array */ \
template <IsMathArray TRight> \
requires (TRight::CanDefault##name##Scalar(true)) \
_NODISCARD TRight operator op(typename TRight::DataType left, const TRight& right) \
{ \
	return TRight(right) op left; \
}\
/* Scalar, Array (in place on r-value) */ \
template <IsMathArray TRight> requires \
(TRight::CanDefault##name##Scalar(true) && \
!std::is_const_v<TRight>) \
_NODISCARD TRight operator op(typename TRight::DataType left, TRight&& right) \
{ \
	if constexpr (commutative) \
		return right op##= left; \
	else \
	{ \
		for (ArrIndex i; i < TRight::Size; ++i) \
			right.raw()[i] = left op right.raw()[i]; \
		return right; \
	} \
}\
/* Array, Scalar */ \
template <IsMathArray TLeft> \
requires (TLeft::CanDefault##name##Scalar(false)) \
_NODISCARD TLeft operator op(const TLeft& left, typename TLeft::DataType right) \
{ \
	return TLeft(left) op right; \
} \
/* Array (in-place on r-value), Scalar */ \
template <IsMathArray TLeft> requires \
(TLeft::CanDefault##name##Scalar(false) && \
!std::is_const_v<TLeft>) \
_NODISCARD TLeft operator op(TLeft&& left, typename TLeft::DataType right) \
{ \
for (ArrIndex i = 0; i < TLeft::Size; ++i) \
	left.raw()[i] op##= right; \
return left; \
} \
/* Array (in-place), Scalar */ \
template <IsMathArray TSelf> requires \
(TSelf::CanDefault##name##Scalar(false) && \
!std::is_const_v<TSelf>) \
TSelf& operator op##=(TSelf& left, typename TSelf::DataType right) \
{ \
	(void)(std::move(left) op right); \
	return left; \
} \
/* Array, Array */ \
template <IsMathArray TLeft, OperableMathArray<TLeft> TRight> requires \
(TLeft::template CanDefault##name##Array<TRight>(false) && \
TRight::template CanDefault##name##Array<TLeft>(true)) \
_NODISCARD TLeft operator op(const TLeft& left, const TRight& right) \
{ \
	return TLeft(left) op right; \
} \
/* Array (in place on r-value), Array */ \
template <IsMathArray TLeft, OperableMathArray<TLeft> TRight> requires \
(TLeft::template CanDefault##name##Array<TRight>(false) && \
TRight::template CanDefault##name##Array<TLeft>(true) && \
!std::is_const_v<TLeft>) \
_NODISCARD TLeft operator op(TLeft&& left, const TRight& right) \
{ \
for (ArrIndex i = 0; i < TLeft::Size; ++i) \
	left.raw()[i] op##= right.raw()[i]; \
return left; \
} \
/* Array (in place), Array */ \
template <IsMathArray TLeft, OperableMathArray<TLeft> TRight> requires \
(TLeft::template CanDefault##name##Array<TRight>(false) && \
TRight::template CanDefault##name##Array<TLeft>(true) && \
!std::is_const_v<TLeft>) \
TLeft& operator op##=(TLeft& left, const TRight& right) \
{ \
	(void)(std::move(left) op right); \
	return left; \
}

namespace LibMath
{
	MATHARRAY_OPERATORS(+, Add, true)
	MATHARRAY_OPERATORS(-, Subtract, false)
	MATHARRAY_OPERATORS(*, Multiply, true)
	MATHARRAY_OPERATORS(/, Divide, false)

#pragma region Negation
	template <IsMathArray TSelf>
	_NODISCARD TSelf operator-(const TSelf& self)
	{
		return -std::remove_const_t<TSelf>(self);
	}

	template <IsMathArray TSelf> requires (!std::is_const_v<TSelf>)
	_NODISCARD TSelf& operator-(TSelf&& self)
	{
		for (typename TSelf::DataType& data : std::forward<TSelf>(self).raw())
			data = -data;

		return self;
	}
#pragma endregion

#pragma region Equality
	template <ArrIndex W, ArrIndex H, ArrayData TLeftData, ArrayData TRightData>
	_NODISCARD bool operator==(
		const MathArray<W, H, TLeftData>& left, const MathArray<W, H, TRightData>& right)
	{
		for (ArrIndex i = 0; i < W * H; ++i)
			if constexpr (std::floating_point<TLeftData>)
			{
				if (!almostEqual<TLeftData>(left.raw()[i], right.raw()[i]))
					return false;
			}
			else if constexpr (std::floating_point<TRightData>)
			{
				if (!almostEqual<TRightData>(left.raw()[i], right.raw()[i]))
					return false;
			}
			else
				if (left.raw()[i] != right.raw()[i])
					return false;

		return true;
	}

	template <ArrIndex W, ArrIndex H, ArrayData TLeftData, ArrayData TRightData>
	_NODISCARD bool operator!=(
		const MathArray<W, H, TLeftData>& left, const MathArray<W, H, TRightData>& right)
	{
		return !(left == right);
	}
#pragma endregion
}

// Cleanup
#undef MATHARRAY_OPERATORS
