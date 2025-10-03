#pragma once

#include "RegistryDefs.h"

#include <LibMath/MathArray/Linear/LinearMathArray4.h>

#include <cstdint>

namespace sol
{
	class state;
}

namespace KaputEngine
{
	class Registry;
}

namespace KaputEngine::Rendering
{
	class Color : public LibMath::LinearMathArray4f
	{
		REGISTER_FINAL_SIG(Color);
		THIS_DEF(Color)

	public:
		static Color Hex(uint32_t hex);

		MATHARRAY_CTORS(Color, LibMath::LinearMathArray4f)

		MATHARRAY_ALIAS(r, 0, 0)
		MATHARRAY_ALIAS(g, 0, 1)
		MATHARRAY_ALIAS(b, 0, 2)
		MATHARRAY_ALIAS(a, 0, 3)

		static const Color
			White, Black, Red, Green, Blue, Yellow, Transparent;
	};
}
