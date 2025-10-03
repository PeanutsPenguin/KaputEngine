#pragma once

#define ARR_UNSIGNED_INDEX

#include <cstring>
#include <initializer_list>
#include <iosfwd>
#include <iostream>
#include <sal.h>
#include <stdexcept>

#include "../Arithmetics.h"
#include "../Concepts.h"
#include "Macros.h"

namespace LibMath::Internal
{
	/// <summary>
	/// Base type providing a template for specializations of <see cref="MathArray"/>.
	/// </summary>
	/// <typeparam name="TData">Data type of items</typeparam>
	/// <typeparam name="W">Width of the array</typeparam>
	/// <typeparam name="H">Height of the array</typeparam>
	template <ArrIndex W, ArrIndex H, ArrayData TData>
		requires (W >= 1, H >= 1)
	class MathArrayBase
	{
		THIS_DEF(MathArrayBase)

	public:
#pragma region Static definitions
		static constexpr ArrIndex
			Width = W,
			Height = H,
			Size = W * H,
			ByteSize = Size * sizeof(TData);

		using DataType = TData;

		/// <summary>
		/// Reference type of the array as linear
		/// </summary>
		using ArrayRefLinear = TData(&)[Size];

		/// <summary>
		/// Const reference type of the array as linear
		/// </summary>
		using ConstArrayRefLinear = const TData(&)[Size];

		/// <summary>
		/// Reference type of the array as 2D
		/// </summary>
		using ArrayRef2D = TData(&)[W][H];

		/// <summary>
		/// Const reference type of the array as 2D
		/// </summary>
		using ConstArrayRef2D = const TData(&)[W][H];

		/// <summary>
		/// Binds to a target template type with the same size and data type
		/// </summary>
		/// <remarks>Matches the return type of as<T>()</remarks>
		template <template <ArrIndex, ArrIndex, typename> class T>
			requires IsMathArray<T<W, H, TData>>
		using StaticAs = T<W, H, TData>;

		/// <summary>
		/// Binds to a target fixed-height template type with the same length and data type
		/// </summary>
		/// <remarks>Matches the return type of as<T>()</remarks>
		template <template <ArrIndex, typename> class T>
			requires IsLinearMathArray<T<H, TData>>
		using StaticAsHeight = T<H, TData>;

		/// <summary>
		/// Binds to a target type of matching size and template data type
		/// </summary>
		/// <remarks>Matches the return type of as<T>()</remarks>
		template <template <typename> class T>
			requires IsMathArray<T<TData>>
		using StaticAsData = T<TData>;

		MATHARRAY_SELFDEFS(MathArray)

		// These functions are defined to prevent arithmetic operations from falling back to default operators in Arithmetics.h
		// (e.g. Matrix * Matrix)
		// Types implementing custom arithmetic operators should shadow relevant functions
#pragma region Arithmetic compatibility
		static consteval bool CanDefaultAddScalar([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		static consteval bool CanDefaultSubtractScalar([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		static consteval bool CanDefaultMultiplyScalar([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		static consteval bool CanDefaultDivideScalar([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		template <IsMathArray<W, H, TData>>
		static consteval bool CanDefaultAddArray([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		template <IsMathArray<W, H, TData>>
		static consteval bool CanDefaultSubtractArray([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		template <IsMathArray<W, H, TData>>
		static consteval bool CanDefaultMultiplyArray([[ maybe_unused ]] bool rValue)
		{
			return true;
		}

		template <IsMathArray<W, H, TData>>
		static consteval bool CanDefaultDivideArray([[ maybe_unused ]] bool rValue)
		{
			return true;
		}
#pragma endregion
#pragma endregion

#pragma region Index
		_NODISCARD TData& operator()(ArrIndex x, ArrIndex y)
		{
			validateWidth(x);
			validateHeight(y);

			return raw2D()[x][y];
		}

		_NODISCARD const TData& operator()(ArrIndex x, ArrIndex y) const
		{
			return const_cast<This*>(this)->operator()(x, y);
		}

		/// <summary>
		/// Returns a single value reference from a linear array.
		/// </summary>
		_NODISCARD TData& operator[](ArrIndex y)
			requires (W == 1)
		{
			validateHeight(y);

			return raw2D()[0][y];
		}

		/// <summary>
		/// Returns a single value constant reference from a linear array.
		/// </summary>
		_NODISCARD const TData& operator[](ArrIndex y) const
			requires (W == 1)
		{
			return const_cast<This*>(this)->operator[](y);
		}

		/// <summary>
		/// Returns a column reference from a 2D array as a linear array.
		/// <summary>
		_NODISCARD MathArray<1, H, TData>& operator[](ArrIndex x)
			requires (W > 1)
		{
			validateWidth(x);

			return reinterpret_cast<MathArray<1, H, TData>&>(raw2D()[x]);
		}

		/// <summary>
		/// Returns a column constant reference from a 2D array as a linear array.
		/// <summary>
		_NODISCARD const MathArray<1, H, TData>& operator[](ArrIndex x) const
			requires (W > 1)
		{
			return const_cast<This*>(this)->operator[](x);
		}
#pragma endregion

		_NODISCARD TData sum() const noexcept
		{
			TData res = TData(0);

			for (const TData data : raw())
				res += data;

			return res;
		}

		_NODISCARD TData product() const noexcept
		{
			TData res = TData(1);

			for (const TData data : raw())
				res *= data;

			return res;
		}

#pragma region raw
		/// <summary>
		/// Returns the data as a linear array reference.
		/// </summary>
		/// <remarks>Can be used as a pointer</remarks>
		_NODISCARD ArrayRefLinear raw() noexcept
		{
			return reinterpret_cast<ArrayRefLinear>(raw2D());
		}

		/// <summary>
		/// Returns the data as a linear array constant reference.
		/// </summary>
		/// <remarks>Can be used as a pointer</remarks>
		_NODISCARD ConstArrayRefLinear raw() const noexcept
		{
			return reinterpret_cast<ConstArrayRefLinear>(raw2D());
		}

		/// <summary>
		/// Return the data as a 2D array reference.
		/// </summary>
		_NODISCARD ArrayRef2D raw2D() noexcept
		{
			return m_Values;
		}

		/// <summary>
		/// Return the data as a 2D array constant reference.
		/// </summary>
		_NODISCARD ConstArrayRef2D raw2D() const noexcept
		{
			return m_Values;
		}
#pragma endregion

#pragma region Cast
		/// <summary>
		///	Casts the array to a complete array type of matching size and data type.
		/// </summary>
		template <IsMathArray<W, H, TData> T>
		_NODISCARD T& as() noexcept
		{
			return static_cast<T&>(*this);
		}

		/// <summary>
		///	Casts the array to a complete array type of matching size and data type.
		/// </summary>
		template <IsMathArray<W, H, TData> T>
		_NODISCARD const T& as() const noexcept
		{
			return static_cast<const T&>(*this);
		}

		/// <summary>
		///	Casts the array to an array type of template data type of matching size and data type.
		/// </summary>
		template <template <ArrIndex, ArrIndex, typename> class T>
		_NODISCARD StaticAs<T>& as() noexcept
		{
			return static_cast<StaticAs<T>&>(*this);
		}

		/// <summary>
		///	Casts the array to an array type of template data type of matching size and data type.
		/// </summary>
		template <template <ArrIndex, ArrIndex, typename> class T>
		_NODISCARD const StaticAs<T>& as() const noexcept
		{
			return static_cast<const StaticAs<T>&>(*this);
		}

		/// <summary>
		///	Casts the array to a fixed-width array type of template size and data type of matching size and data type.
		/// </summary>
		template <template <ArrIndex, typename> class T>
		_NODISCARD StaticAsHeight<T>& as() noexcept
		{
			return static_cast<StaticAsHeight<T>&>(*this);
		}

		/// <summary>
		///	Casts the array to a fixed-width array type of template size and data type of matching size and data type.
		/// </summary>
		template <template <ArrIndex, typename> class T>
		_NODISCARD const StaticAsHeight<T>& as() const noexcept
		{
			return static_cast<const StaticAsHeight<T>&>(*this);
		}

		/// <summary>
		/// Cast the array to a full template array type of matching size and data type.
		/// </summary>
		template <template <typename> class T>
		_NODISCARD StaticAsData<T>& as() noexcept
		{
			return static_cast<StaticAsData<T>&>(*this);
		}

		/// <summary>
		/// Cast the array to a full template array type of matching size and data type.
		/// </summary>
		template <template <typename> class T>
		_NODISCARD const StaticAsData<T>& as() const noexcept
		{
			return static_cast<const StaticAsData<T>&>(*this);
		}
#pragma endregion

		static void validateWidth(ArrIndex x)
		{
			return validateIndex(x, W);
		}

		static void validateHeight(ArrIndex y)
		{
			return validateIndex(y, H);
		}

	protected:
#pragma region Constructors
		MathArrayBase() = default;

		template <ArrayData TRightData>
			requires std::convertible_to<TRightData, TData>
		MathArrayBase(const MathArray<W, H, TRightData>& arr)
		{
			if constexpr (std::is_same_v<TData, TRightData>)
				std::memcpy(m_Values, arr.raw(), ByteSize);
			else
				for (ArrIndex i = 0; i < Size; i++)
					raw()[i] = static_cast<TData>(arr.raw()[i]);
		}

		explicit MathArrayBase(
			_In_reads_bytes_(count * sizeof(TData)) const TData* values, const ArrIndex count
		) : m_Values()
		{
#if _DEBUG
			if (count > Size)
				throw new std::invalid_argument("too many initializers");
#endif

			std::memcpy(m_Values, values, count * sizeof(TData));
		}

		explicit MathArrayBase(TData data) noexcept
		{
			for (TData& val : this->raw())
				val = data;
		}

		explicit MathArrayBase(ConstArrayRefLinear values) noexcept
		{
			std::memcpy(m_Values, values, ByteSize);
		}

		explicit MathArrayBase(ConstArrayRef2D values) noexcept
		{
			std::memcpy(m_Values, values, ByteSize);
		}

		template <ArrIndex VSize>
			requires (VSize < Size)
		explicit MathArrayBase(const std::array<TData, VSize>& values) noexcept
			: m_Values()
		{
			std::memcpy(m_Values, values.data(), VSize * sizeof(TData));
		}

		explicit MathArrayBase(const std::array<TData, Size>& values) noexcept
			: m_Values()
		{
			// Constructor overload to skip zero-init if the size matches
			std::memcpy(m_Values, values.data(), Size * sizeof(TData));
		}

		MathArrayBase(std::initializer_list<TData> values) : m_Values()
		{
#if _DEBUG
			if (values.size() > Size)
				throw std::invalid_argument("too many initializers");
#endif

			std::memcpy(m_Values, data(values), values.size() * sizeof(TData));
		}
#pragma endregion

		/// <summary>
		/// Validates that the provided index is in range of a dimension the array, otherwise throws an exception.
		/// </summary>
		static void validateIndex(ArrIndex index, ArrIndex size)
		{
#if _DEBUG
			if (index >= size)
				throw std::out_of_range("Index out of range of the array.");
#else
			(void)index;
			(void)size;
#endif
		}

		TData m_Values[W][H];
	};

	template <ArrIndex W, ArrIndex H, ArrayData TData>
	std::ostream& operator<<(std::ostream& left, const MathArray<W, H, TData>& right)
	{
		if constexpr (W == 1)
		{
			left << "{ " << right.raw()[0];

			for (ArrIndex i = 1; i < H; i++)
				left << ", " << right.raw()[i];

			left << " }";
		}
		else
		{
			left << "\n[\n";

			for (ArrIndex y = 0; y < H; y++)
			{
				// \t creates too large spacing in the console
				left << "  " << right.raw2D()[0][y];

				for (ArrIndex x = 1; x < W; x++)
					left << ", " << right.raw2D()[x][y];

				left << '\n';
			}

			left << ']';
		}

		return left;
	}
}
