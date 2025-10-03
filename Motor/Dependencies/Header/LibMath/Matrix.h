#pragma once

#include "MathArray/MathArray.h"

#include <sal.h>

namespace LibMath
{
	template <ArrIndex W, ArrIndex H, ArrayData TData = DefaultDataType>
		requires (W >= 1, H >= 1)
	class Matrix : public MathArray<W, H, TData>
	{
		THIS_DEF(Matrix)
		
	public:
		MATHARRAY_CTORS(Matrix, MathArray<W, H, TData>)
		MATHARRAY_SELFDEFS(Matrix)

		using Minor = Matrix<W == 1 ? 1 : W - 1, H == 1 ? 1 : H - 1, TData>;
		using Transposed = Matrix<H, W, TData>;

		template <ArrIndex WR>
		using Multiplied = Matrix<WR, H, TData>;

		template <IsMathArray<W, H, TData> T>
		static consteval bool CanDefaultMultiplyArray([[maybe_unused]] bool rValue)
		{
			// Matrix multiplication has special requirements on size of left and right values
			// Prevents operator* calls falling back on the default MathArray operator if requirements aren't met
			return !std::derived_from<T, Matrix>;
		}
		
		_NODISCARD static constexpr Matrix Identity() noexcept
			requires (W == H)
		{
			// Use this function to generate an identity matrix
			// No constructor generates such a matrix for the sake of consistency between MathArray types
			
			Matrix out;

			for (ArrIndex i = 0; i < W; ++i)
				out.raw2D()[i][i] = TData(1);

			return out;
		}

#ifdef STATICASSERT_FALLBACKS
		_NODISCARD static constexpr Matrix Identity() noexcept
		{
			static_assert(false, "Identity matrices can only be generated for square matrices.");
		}
#endif

		_NODISCARD Transposed transpose() const noexcept
		{
			// Could be implemented in-place with a const& and && function pair
			// Would navigate the top-left half of the matrix in a staircase pattern, swapping the mirror value with std::swap
			// Possible optimization by skipping along the diagonal (x == y)
			// Consider keeping the current implementation for const& instead of calling && from a copy
			
			Transposed out;

			// Navigate the matrix with x and y swapped
			for (ArrIndex x = 0; x < H; ++x)
				for (ArrIndex y = 0; y < W; ++y)
					out.raw2D()[x][y] = this->raw2D()[y][x];

			return out;
		}

		_NODISCARD Minor minor(ArrIndex x, ArrIndex y) const noexcept
			requires (W == H && W > 1)
		{
			Minor minor;
			ArrIndex minorX = 0;

			for (ArrIndex thisX = 0; thisX < W; ++thisX)
			{
				if (thisX == x)
					continue;

				// Copy the row in chunks before and after the minor index
				// memcpy allows a size of 0 for a no-effect copy
				std::memcpy(&minor.raw2D()[minorX][0], &this->raw2D()[thisX][0], y * sizeof(TData));
				std::memcpy(&minor.raw2D()[minorX][y], &this->raw2D()[thisX][y + 1], (H - y) * sizeof(TData));

				++minorX;
			}

			return minor;
		}

#ifdef STATICASSERT_FALLBACKS
		_NODISCARD Minor minor(ArrIndex x, ArrIndex y) const noexcept
		{
			static_assert(false, "Matrix minors can only be calculated for square matrices.");
		}
#endif

#pragma region Reduce
		template <ArrayData PrecisionType = PrecisionType<TData>>
		_NODISCARD Matrix reduce(_Out_ _Maybenull_ TData* detFactor = nullptr) const &
			requires (W == H)
		{
			// Copy the matrix and apply reduction in-place on the copy (see && function below)
			return Matrix(*this).reduce<PrecisionType>(detFactor);
		}

		template <ArrayData PrecisionType = PrecisionType<TData>>
		Matrix& reduce(_Out_ _Maybenull_ TData* detFactor = nullptr) &&
			requires (W == H)
		{
			// Initialize the out parameter
			if (detFactor)
				*detFactor = TData(1);

			auto& array = this->raw2D();
			
			for (ArrIndex i = 0; i < H - 1; ++i)
			{
				// Skip iteration if 0 is already in place
				if (array[i][i + 1] == 0)
					continue;

				const TData& topLeft = array[i][i];

				// Having 0 as top-left would cause division by zero - Swap rows
				if (topLeft == TData(0))
				{
					// Swapping rows in a matrix changes the determinant sign
					if (detFactor)
						*detFactor = -*detFactor;

					// Swap each item of the rows
					for (ArrIndex x = 0; x < W; ++x)
						std::swap(array[x][i], array[x][i + 1]);
				}
				
				// For each lower row, multiply by a factor of the iteration row
				for (ArrIndex y = i + 1; y < H; ++y)
				{
					const PrecisionType factor = array[i][y] / topLeft;
					
					// First element is always canceled by the factor
					array[i][y] = TData(0);
					
					// Multiply remaining elements by the canceling factor
					for (ArrIndex x = i + 1; x < W; ++x)
						array[x][y] -= array[x][i] * factor;
				}
			}

			return *this;
		}

#ifdef STATICASSERT_FALLBACKS
		template <ArrayData PrecisionType = PrecisionType<TData>>
		Matrix& reduce(_Out_ _Maybenull_ TData* detFactor = nullptr) &&
		{
			static_assert(false, "Matrix reduction can only be applied to square matrices.");
		}
#endif
#pragma endregion

#pragma region Determinant
		_NODISCARD TData determinant([[ maybe_unused ]] bool superior = false) const noexcept
			requires (W == 1 && H == 1)
		{
			// Determinant is the single item in a 1x1 matrix
			return this->raw2D()[0][0];
		}

		_NODISCARD TData determinant([[ maybe_unused ]] bool superior = false) const noexcept
			requires (W == 2 && H == 2)
		{
			// Determinant is a crossing of items in a 2x2 matrix
			const auto& array = this->raw2D();
			return array[0][0] * array[1][1] - array[0][1] * array[1][0];
		}
		
		_NODISCARD TData determinant(bool superior = false) const
			requires (W == H && W > 2)
		{
			// For matrices larger than 2x2, find the determinant based on diagonal multiplication instead of recursive minors
			
			if (!superior)
			{
				// Matrix is not superior - Get a reduced (superior) matrix and get its determinant from the diagonal
				// Reducing can change the determinant - Get the modification factor and apply to the reduced determinant
				TData factor;
				return reduce(&factor).determinant(true) * factor;
			}

			// Matrix is superior - Find the determinant by multiplying the diagonal
			auto product = TData(1);

			for (ArrIndex i = 0; i < W; ++i)
				product *= this->raw2D()[i][i];

			return product;
		}

#ifdef STATICASSERT_FALLBACKS
		_NODISCARD TData determinant(bool superior = false) const
		{
			static_assert(false, "Determinant can only be calculated for square matrices.");
		}
#endif
#pragma endregion
		
		_NODISCARD Matrix cofactor() const
			requires (W > 1 && H > 1)
		{
			Matrix out;

			for (ArrIndex x = 0; x < W; ++x)
				for (ArrIndex y = 0; y < H; ++y)
					// pow: Change the sign based on x+y being even or odd, creating the checkerboard pattern of signs
					out.raw2D()[x][y] = minor(x, y).determinant() * pow(-1, x+y);

			return out;
		}

#ifdef STATICASSERT_FALLBACKS
		_NODISCARD Matrix cofactor() const
		{
			static_assert(false, "Matrices of width or height of 1 cannot have cofactors.");
		}
#endif

		_NODISCARD Transposed adjugate() const
		{
			return cofactor().transpose();
		}

		_NODISCARD Matrix invert(bool superior = false) const
			requires (W == H)
		{
			return adjugate() * (TData(1) / determinant(superior));
		}
		
#ifdef STATICASSERT_FALLBACKS
		_NODISCARD Matrix invert(bool superior = false) const
		{
			static_assert(false, "Matrices can only be inverted if they are square.");
		}
#endif

		template <ArrIndex WR>
		_NODISCARD Multiplied<WR> operator*(const Matrix<WR, W, TData>& right) const
		{
			Multiplied<WR> out;
	
			for (ArrIndex x = 0; x < Multiplied<WR>::Width; ++x)
				for (ArrIndex y = 0; y < Multiplied<WR>::Height; ++y)
				{
					auto sum = TData(0);
					
					for (ArrIndex i = 0; i < W; ++i)
						sum += this->raw2D()[i][y] * right.raw2D()[x][i];
	
					out.raw2D()[x][y] = sum;
				}
	
			return out;
		}

#ifdef STATICASSERT_FALLBACKS
		template <ArrIndex WR, ArrIndex HR>
		_NODISCARD Multiplied<WR> operator*(const Matrix<WR, HR, TData>&)
		{
			static_assert(false, "Incompatible dimensions for matrix multiplication.");
		}
#endif
		
		Matrix& operator*=(const Matrix& right)
		{
			// This operator exits solely as an alias on * in order to allow the *= syntax.
			// Unlike with other operators, results depend on other values in the matrix, thus directly putting each result back into the original matrix would falsify other results.
			// A proper implementation would require some caching that although could possibly reduce the stack footprint by reusing cache variables across the operation, the time complexity would most likely be the same if not worse.
			*this = *this * right;
			return *this;
		}
	};

	template <ArrayData TData = DefaultDataType>
	using Matrix1 = Matrix<1, 1, TData>;

	template <ArrayData TData = DefaultDataType>
	using Matrix2 = Matrix<2, 2, TData>;

	template <ArrayData TData = DefaultDataType>
	using Matrix3 = Matrix<3, 3, TData>;

	template <ArrayData TData = DefaultDataType>
	using Matrix4 = Matrix<4, 4, TData>;

	MATHARRAY_NUMERICALIASES(Matrix1)
	MATHARRAY_NUMERICALIASES(Matrix2)
	MATHARRAY_NUMERICALIASES(Matrix3)
	MATHARRAY_NUMERICALIASES(Matrix4)
}
