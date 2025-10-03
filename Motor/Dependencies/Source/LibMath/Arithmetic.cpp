#include <cmath>

#include "LibMath/Arithmetic.h"

namespace LibMath
{
	double ceiling(double val) noexcept
	{
		return std::ceil(val);
	}

	double floor(double val) noexcept
	{
		return std::floor(val);
	}

	float squareRoot(float val) noexcept
	{
		return sqrt(val);
	}
}
