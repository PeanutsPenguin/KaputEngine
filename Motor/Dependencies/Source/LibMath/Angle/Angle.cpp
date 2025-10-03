#include "LibMath/Angle/Angle.h"
#include "LibMath/Arithmetic.h"

namespace LibMath
{
	const float Angle::Epsilon = 4.76837158e-07f;

	float Angle::wrap(float value, bool signedQuadrant, const WrapRanges& ranges) noexcept
	{
		// Bypass expensive unsigned wrap if already in signed range
		// Fixes an edge case where unsigned wrap can introduce a small loss of precision, leading to the output being different from the original value
		if (signedQuadrant && value >= ranges.signedRange[0] && value < ranges.signedRange[1])
			return value;

		float out = LibMath::wrap(value, ranges.unsignedRange[0], ranges.unsignedRange[1]);
		const float(&range)[2] = signedQuadrant ? ranges.signedRange : ranges.unsignedRange;

		if (signedQuadrant && out > ranges.signedRange[1])
			out -= ranges.unsignedRange[1];

		if (out == range[1])
			out = range[0];

		return out;
	}
}
