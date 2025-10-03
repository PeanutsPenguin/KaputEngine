#include "LibMath/Trigonometry.h"

#include <cmath>

namespace LibMath
{
	float sin(Radian rad)
	{
		return std::sin(rad.raw());
	}

	float cos(Radian rad)
	{
		return std::cos(rad.raw());
	}

	float tan(Radian rad)
	{
		return std::tan(rad.raw());
	}

	Radian asin(float value)
	{
		return Radian(std::asin(value));
	}

	Radian acos(float value)
	{
		return Radian(std::acos(value));
	}

	Radian	atan(float);		// Radian angle = atan(0.546302);		// Radian{0.500000}
	Radian	atan(float, float); // Radian angle = atan(1, -2);			// Radian{2.677945}
}