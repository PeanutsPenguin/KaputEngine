#pragma once

#include "LinearMathArray.h"

namespace LibMath
{
    template <ArrayData TData = DefaultDataType>
    using LinearMathArray4 = LinearMathArray<4, TData>;

    MATHARRAY_NUMERICALIASES(LinearMathArray4)
}
