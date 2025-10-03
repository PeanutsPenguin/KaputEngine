#pragma once

#include "../../../Arithmetic.h"
#include "../../../MathArray.h"

// Defines implicit reference conversion operators of linear types to smaller sizes
#define MATHARRAY_SELFDELF_LINEAR_CONVERT \
template <ArrIndex DD> requires (DD < This::Height) \
_NODISCARD operator typename This::template LinearSelf<DD, typename This::DataType>& () noexcept { return this->splice<0, DD>().template as<typename This::LinearSelf>(); } \
template <ArrIndex DD> requires (DD < This::Height) \
_NODISCARD operator const typename This::template LinearSelf<DD, typename This::DataType>& () const noexcept { return this->splice<0, DD>().template as<typename This::LinearSelf>(); }

namespace LibMath
{
	namespace Internal
	{
		/// <summary>
		/// Base type providing a template for linear specializations of <see cref="MathArray"/>
		/// </summary>
		/// <typeparam name="TData">Data type of items</typeparam>
		/// <typeparam name="D">Size of the array</typeparam>
		template <ArrIndex D, ArrayData TData>
			requires (D >= 1)
		class LinearMathArrayBase : public MathArrayBase<1, D, TData>
		{
			THIS_DEF(LinearMathArrayBase<D, TData>)

		protected:
			MATHARRAY_CTORS(LinearMathArrayBase, MathArrayBase<1, D, TData>)

		public:
			MATHARRAY_SELFDELF_LINEAR_CONVERT

			static constexpr ArrIndex EulerRotationAxies = pow(2, D) / 2 - 1;
			using EulerRotation = MathArray<1, EulerRotationAxies == 0 ? 1 : EulerRotationAxies, TData>;

			/// <summary>
			/// Casts a segment of the array to a smaller linear array type reference.
			/// </summary>
			/// <typeparam name="Start">Start index</typeparam>
			/// <typeparam name="Length">Number of items to take</typeparam>
			/// <remarks>Start and Length are validated to be in the range of the original array</remarks>
			template <ArrIndex Start, ArrIndex Length>
				requires (Start + Length <= D)
			_NODISCARD MathArray<1, Length, TData>& splice() noexcept
			{
				return reinterpret_cast<MathArray<1, Length, TData>&>(this->raw()[Start]);
			}

			/// <summary>
			/// Casts a segment of the array to a const smaller linear array type reference.
			/// </summary>
			/// <typeparam name="Start">Start index</typeparam>
			/// <typeparam name="Length">Number of items to take</typeparam>
			/// <remarks>Start and Length are validated to be in the range of the original array</remarks>
			template <ArrIndex Start, ArrIndex Length>
				requires (Start + Length <= D)
			_NODISCARD const MathArray<1, Length, TData>& splice() const noexcept
			{
				// Reuse non-const implementation
				return const_cast<This*>(this)->splice<Start, Length>();
			}

#ifdef STATICASSERT_FALLBACKS
			template <ArrIndex, ArrIndex Length>
			_NODISCARD MathArray<1, Length, TData>& splice() noexcept
			{
				static_assert(false, "Splice goes out of bounds of the source array.");
			}

			template <ArrIndex, ArrIndex Length>
			_NODISCARD const MathArray<1, Length, TData>& splice() const noexcept
			{
				static_assert(false, "Splice goes out of bounds of the source array.");
			}
#endif

			/// <summary>
			/// Casts a segment of the array to a smaller linear array type reference.
			/// </summary>
			/// <typeparam name="Length">Number of items to take from the start</typeparam>
			/// <remarks>Length is validated to be in the range of the original array</remarks>
			template <ArrIndex Length>
				requires (Length < D)
			_NODISCARD MathArray<1, Length, TData>& splice() noexcept
			{
				return this->splice<0, Length>();
			}

			/// <summary>
			/// Casts a segment of the array to a const smaller linear array type reference.
			/// </summary>
			/// <typeparam name="Length">Number of items to take from the start</typeparam>
			/// <remarks>Length is validated to be in the range of the original array</remarks>
			template <ArrIndex Length>
				requires (Length < D)
			_NODISCARD const MathArray<1, D, TData>& splice() const noexcept
			{
				// Reuse non-const implementation
				return const_cast<This*>(this)->splice<Length>();
			}

#ifdef STATICASSERT_FALLBACKS
			template <ArrIndex Length>
			_NODISCARD MathArray<1, Length, TData>& splice() noexcept
			{
				static_assert(false, "Splice goes out of bounds of the source array.");
			}

			template <ArrIndex Length>
			_NODISCARD const MathArray<1, Length, TData>& splice() const noexcept
			{
				static_assert(false, "Splice goes out of bounds of the source array.");
			}
#endif

#if 0 // Causes unrelated as overloads to fail
			/// <summary>
			/// Casts the linear array to a different linear type reference of same or smaller size.
			/// </summary>
			template <IsLinearMathArray T>
				requires (T::Size <= D)
			_NODISCARD T& as() noexcept
			{
				return splice<T::Length>().template as<T>();
			}

			/// <summary>
			/// Casts the linear array to a different linear type const reference of same or smaller size.
			/// </summary>
			template <IsLinearMathArray T>
				requires (T::Size <= D)
			_NODISCARD const T& as() const noexcept
			{
				return const_cast<LinearMathArrayBase*>(this)->as<T>();
			}

			/// <summary>
			/// Casts the linear array to a different linear type reference f same or smaller size.
			/// </summary>
			template <template <typename> class T>
				requires (IsLinearMathArray<T<TData>> && T<TData>::Size <= D)
			_NODISCARD T<TData>& as() noexcept
			{
				return as<T<TData>>();
			}

			/// <summary>
			/// Casts the linear array to a different linear type const reference f same or smaller size.
			/// </summary>
			template <template <typename> class T>
				requires (IsLinearMathArray<T<TData>> && T<TData>::Size <= D)
			_NODISCARD const T<TData>& as() const noexcept
			{
				return as<T<TData>>();
			}
#endif
		};
	}
}
