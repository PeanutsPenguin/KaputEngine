#pragma once

#include <sal.h>

// Macro for including the comma character in macro parameter values
// Prefer ARGS over COMMA
#define COMMA ,

// Packs a set of tokens into a single macro argument.
// The token set can include commas.
#define ARGS(...) __VA_ARGS__

/*
Macro for defining nearest template types for a math array type. Depends on Base being already defined.
selfFull - Full declaration of the nearest constructible type with a variable width, height, and data type
linearFull - Full declaration of the nearest constructible type with a width of 1, variable height, and variable data type
dataFull - Full declaration of the nearest constructible type matching the size and a variable data type
Output:
Self<ArrIndex, ArrIndex, ArrayData>
LinearSelf<D, TData>
DataSelf<ArrayData>
*/
#define MATHARRAY_SELFDEFS_FULL(selfFull, linearFull, dataFull) \
	template <ArrIndex W = This::Width, ArrIndex H = This::Height, ArrayData TData = typename This::DataType> \
	using Self = selfFull; \
	template <ArrIndex D = This::Height, ArrayData TData = typename This::DataType> \
	using LinearSelf = linearFull; \
	template <ArrayData TData = typename This::DataType> \
	using DataSelf = dataFull; \

/*
Macro for defining nearest template types for a 2D math array type. Depends on Base being already defined.
selfName - Name of the nearest constructible full template type
Output:
Self<ArrIndex, ArrIndex, ArrayData>
LinearSelf<D, TData>
DataSelf<ArrayData>
*/
#define MATHARRAY_SELFDEFS(selfName) \
MATHARRAY_SELFDEFS_FULL(\
	ARGS(selfName<W, H, TData>), \
	ARGS(selfName<1, D, TData>), \
	ARGS(selfName<W, H, TData>))

/*
Macro for defining nearest template types for a linear math array type. Depends on Base being already defined.
selfName - Name of the nearest constructible full template type
linearName - Name of the nearest constructible linear template type
Output:
Self<ArrIndex, ArrIndex, ArrayData>
LinearSelf<D, TData>
DataSelf<ArrayData>
*/
#define MATHARRAY_SELFDEFS_LINEAR(selfName, linearName) \
MATHARRAY_SELFDEFS_FULL(\
	ARGS(selfName<W, H, TData>), \
	ARGS(linearName<D, TData>), \
	ARGS(linearName<D, TData>))

/*
Macro for defining nearest template types for math array type with a variable data type. Depends on Base being already defined.
selfName - Name of the nearest constructible full template type
linearName - Name of the nearest constructible linear template type
dataName - Name of the nearest constructible data template type
Output:
Self<ArrIndex, ArrIndex, ArrayData>
LinearSelf<D, TData>
DataSelf<ArrayData>
*/
#define MATHARRAY_SELFDEFS_DATA(selfName, linearName, dataName) \
MATHARRAY_SELFDEFS_FULL(\
	ARGS(selfName<W, H, TData>), \
	ARGS(linearName<D, TData>), \
	ARGS(dataName<TData>))

/*
Macro for defining standard constructors for MathArray types.
typeName - Name of the current type
__VA_ARGS__ - Base type with template arguments
*/
#define MATHARRAY_CTORS(typeName, ...) \
	typeName() : __VA_ARGS__() { } \
	template <LibMath::ArrayData TRightData> requires std::convertible_to<TRightData, typename typeName::DataType> \
	typeName(const MathArray<typeName::Width, typeName::Height, TRightData>& array) : __VA_ARGS__(array) { } \
	explicit typeName( \
		_In_reads_bytes_(count * sizeof(typename typeName::DataType) \
	) const typename typeName::DataType* values, LibMath::ArrIndex count) : __VA_ARGS__(values, count) { } \
	explicit typeName(typename typeName::DataType value) noexcept : __VA_ARGS__(value) { } \
	explicit typeName(typename typeName::ConstArrayRefLinear values) noexcept : __VA_ARGS__(values) { } \
	explicit typeName(typename typeName::ConstArrayRef2D values) noexcept : __VA_ARGS__(values) { } \
	template <LibMath::ArrIndex VSize> \
	explicit typeName(const std::array<typename typeName::DataType, VSize>& values) noexcept : __VA_ARGS__(values) { } \
	typeName(std::initializer_list<typename typeName::DataType> values) : __VA_ARGS__(values) { } \
	template <LibMath::ArrayData TRightData> requires std::convertible_to<TRightData, typename typeName::DataType> \
	typeName& operator=(const MathArray<typeName::Width, typeName::Height, TRightData>& right) \
	{ \
		new (this) typeName(right); \
		return *this; \
	}

// Macro for locally exposing inherited static definitions through This::
// Dependency of other macros
// Should be used in a private context
#define THIS_DEF(...) using This = __VA_ARGS__;

#define MATHARRAY_PARTIAL_ALIAS(name, x, y, varConst) \
	_NODISCARD varConst typename This::DataType& name() varConst noexcept { return this->raw2D()[x][y]; }

#define MATHARRAY_ALIAS(name, x, y) MATHARRAY_PARTIAL_ALIAS(name, x, y,) MATHARRAY_PARTIAL_ALIAS(name, x, y, const)

#define MATHARRAY_NUMERICALIAS(suffix, type, ...) using __VA_ARGS__##suffix = __VA_ARGS__<type>;

#define MATHARRAY_NUMERICALIASES(...) \
MATHARRAY_NUMERICALIAS(c, char, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(uc, unsigned char, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(s, short, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(us, unsigned short, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(i, int, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(ui, unsigned int, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(l, long, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(ul, unsigned long, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(ll, long long, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(ull, unsigned long long, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(f, float, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(d, double, __VA_ARGS__) \
MATHARRAY_NUMERICALIAS(ld, long double, __VA_ARGS__)
