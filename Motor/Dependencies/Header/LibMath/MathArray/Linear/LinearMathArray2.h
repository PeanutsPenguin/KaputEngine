#pragma once

#include "LinearMathArray.h"

namespace LibMath
{
    template <ArrayData TData = DefaultDataType>
    using LinearMathArray2 = LinearMathArray<2, TData>;

    MATHARRAY_NUMERICALIASES(LinearMathArray2)
}
