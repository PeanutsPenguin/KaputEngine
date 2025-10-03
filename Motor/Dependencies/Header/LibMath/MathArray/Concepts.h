#pragma once

#include <concepts>
#include <sal.h>

namespace LibMath
{
#define ARR_UNSIGNED_INDEX

#ifdef  ARR_UNSIGNED_INDEX
	/// <summary>
	/// Index type used in math arrays
	/// </summary>
	using ArrIndex = size_t;
#else
	/// <summary>
	/// Index type used in math arrays
	/// </summary>
	using ArrIndex = long;
#endif //  MAT_UNSIGNED_INDEX

	/// <summary>
	/// Default typename value of TData in math arrays
	/// </summary>
	using DefaultDataType = float;

	/// <summary>
	/// Type usable as data in math arrays
	/// </summary>
	template <typename T, typename TData = T>
	concept ArrayData = (std::integral<T> || std::floating_point<T>) && std::convertible_to<T, TData>;

	namespace Internal
	{
		// Type aliases don't support template specialization
		// Wrap the alias in a specialized struct to expose through a proper alias

		template <ArrayData T> struct PrecisionType_s { using type = T; };
		template <> struct PrecisionType_s<char> { using type = float; };
		template <> struct PrecisionType_s<unsigned char> { using type = float; };
		template <> struct PrecisionType_s<int> { using type = float; };
		template <> struct PrecisionType_s<unsigned int> { using type = float; };
		template <> struct PrecisionType_s<long> { using type = double; };
		template <> struct PrecisionType_s<unsigned long> { using type = double; };
		template <> struct PrecisionType_s<long long> { using type = long double; };
		template <> struct PrecisionType_s<unsigned long long> { using type = long double; };
	}

	/// <summary>
	/// Promotes an integer type to a floating point type for operations requiring decimals
	/// Functions using this type should also expose an ArrayData template defaulted to this type to override the type used.
	/// <summary>
	template <ArrayData	T>
	using PrecisionType = typename Internal::PrecisionType_s<T>::type; // Redirect to the specialized alias wrapped in a struct

#pragma region Suffix
	template <ArrayData TData>
	_NODISCARD _Ret_null_ static consteval const char* typeSuffix()
	{
		return nullptr;
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<char>()
	{
		return "c";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<unsigned char>()
	{
		return "uc";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<short>()
	{
		return "s";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<unsigned short>()
	{
		return "us";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<int>()
	{
		return "i";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<unsigned int>()
	{
		return "ui";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<long>()
	{
		return "l";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<unsigned long>()
	{
		return "ul";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<long long>()
	{
		return "ll";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<unsigned long long>()
	{
		return "ull";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<float>()
	{
		return "f";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<double>()
	{
		return "d";
	}

	template <>
	_NODISCARD _Ret_notnull_ static consteval const char* typeSuffix<long double>()
	{
		return "ld";
	}
#pragma endregion

	template <ArrIndex W, ArrIndex H, ArrayData TData>
		requires (W >= 1, H >= 1)
	class MathArray;

	/// <summary>
	/// Concept verifying the type derived from MathArrayBase and can be constructed without parameters
	/// </summary>
	template <class T, ArrIndex W = 0, ArrIndex H = 0, typename TData = void>
	concept IsMathArray = std::derived_from<T, MathArray<
		W == 0 ? T::Width : W,
		H == 0 ? T::Height : H,
		std::conditional_t<std::is_void_v<TData>, typename T::DataType, TData>>>;

	/// <summary>
	/// Validates that two types are math arrays usable in arithmetic operations
	/// </summary>
	template <class A, class B>
	concept OperableMathArray = IsMathArray<A> && IsMathArray<B> && A::Width == B::Width && A::Height == B::Height;

	template <class T, ArrIndex D = 0, typename TData = void>
	concept IsLinearMathArray = IsMathArray<T, 1, D, TData>;
}
