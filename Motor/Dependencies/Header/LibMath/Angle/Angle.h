#pragma once

#include <vcruntime.h>

namespace LibMath
{
	class Angle
	{
	public:
		static const float Epsilon;

		struct WrapRanges
		{
			float unsignedRange[2];
			float signedRange[2];
		};

		_NODISCARD static float wrap(float value, bool signedQuadrant, const WrapRanges& ranges) noexcept;
	};
}
