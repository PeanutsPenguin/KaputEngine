#pragma once

#include "LinearMathArray.h"

namespace LibMath
{
    template <ArrayData TData = DefaultDataType>
    using LinearMathArray3 = LinearMathArray<3, TData>;

    MATHARRAY_NUMERICALIASES(LinearMathArray3)
}
