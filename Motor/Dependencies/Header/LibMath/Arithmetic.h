#pragma once

#include <concepts>
#include <limits>

#include "MathArray/Concepts.h"

namespace LibMath
{
	template <typename T>
	concept numeric = std::integral<T> || std::floating_point<T>;

	/// <summary>
	/// Returns the absolute of a floating-point value.
	/// </summary>
	template <numeric T>
	_NODISCARD constexpr T abs(T val) noexcept
	{
		return val < T(0) ? -val : val;
	}

	template <numeric T>
	_NODISCARD T min(T a, T b) noexcept
	{
		return a < b ? a : b;
	}

	template <numeric T>
	_NODISCARD T max(T a, T b) noexcept
	{
		return a > b ? a : b;
	}

	template <numeric T, std::integral TPow = int>
	_NODISCARD constexpr T pow(T val, TPow power)
	{
		if (power == TPow(0))
			return T(1);

		T out(1);

		for (TPow p(1); p <= power; ++p)
			out *= val;

		return out;
	}

	/// <summary>
	/// Returns if two floating-point values are similar enough to be considered equal
	/// </summary>
	template <std::floating_point T>
	_NODISCARD constexpr bool almostEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) noexcept
	{
		return abs(a - b) <= epsilon;
	}

	/// <summary>
	/// Rounds up a floating-point value to the nearest integer.
	/// </summary>
	_NODISCARD double ceiling(double val) noexcept;

	/// <summary>
	/// Rounds down a floating-point value to the nearest integer.
	/// </summary>
	_NODISCARD double floor(double val) noexcept;

	/// <summary>
	/// Limits a value to be within a given range.
	/// </summary>
	/// <param name="value">Value to limit</param>
	/// <param name="min">Minimum value</param>
	/// <param name="max">Maximum value</param>
	template <typename T>
	_NODISCARD constexpr T clamp(const T& value, const T& min, const T& max)
	{
		if (value < min)
			return min;

		if (value > max)
			return max;

		return value;
	}

	/// <summary>
	/// Returns the square root of a floating-point value.
	/// </summary>
	_NODISCARD float squareRoot(float val) noexcept;

	/// <summary>
	/// Wraps a value to be within a given range.
	/// </summary>
	/// <param name="value">Value to wrap</param>
	/// <param name="min">Lower bounds of the range</param>
	/// <param name="max">Higher bounds of the range</param>
	template <numeric T, numeric PrecisionType = PrecisionType<T>>
	_NODISCARD constexpr T wrap(T value, T min, T max) noexcept
	{
		T range = max - min;

		if (value > range)
			value -= static_cast<T>(floor(value / PrecisionType(range))) * range;
		else if (value < -range)
			value += static_cast<T>(floor(abs(value) / PrecisionType(range))) * range;

		if (value > max)
			value = min + abs(value);
		else if (value < min)
			value = max - abs(value);

		return value;
	}

	template <typename T>
	_NODISCARD constexpr T getLerpRatio(const T& value, const T& min, const T& max) noexcept
	{
		return (value - min) / (max - min);
	}

	template <typename T, typename TRatio = T>
	_NODISCARD constexpr T lerp(const TRatio& ratio, const T& min, const T& max) noexcept
	{
		return min + (max - min) * ratio;
	}
}
