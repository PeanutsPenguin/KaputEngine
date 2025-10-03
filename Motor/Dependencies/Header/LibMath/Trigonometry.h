#pragma once

#include "Angle/Radian.h"
#include <vcruntime.h>

#include <numbers>

namespace LibMath
{
	constexpr float g_pi = std::numbers::pi_v<float>;

	_NODISCARD float	sin(Radian);		// float result = sin(Radian{0.5});		// 0.479426
	_NODISCARD float	cos(Radian);		// float result = sin(Degree{45});		// 0.707107			// this make use implicit conversion
	_NODISCARD float	tan(Radian);		// float result = sin(0.5_rad);			// 0.479426			// this make use user defined litteral
	_NODISCARD Radian	asin(float);		// Radian angle = asin(0.479426);		// Radian{0.500001}
	_NODISCARD Radian	acos(float);		// Degree angle = acos(0.707107);		// Degree{44.99998}	// this make use implicit conversion
	_NODISCARD Radian	atan(float);		// Radian angle = atan(0.546302);		// Radian{0.500000}
	_NODISCARD Radian	atan(float, float); // Radian angle = atan(1, -2);			// Radian{2.677945}
}
