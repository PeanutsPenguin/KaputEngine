#pragma once

#include "Line.h"

namespace LibMath
{
	/// <summary>
	/// Generic 2D shape
	/// </summary>
	/// <typeparam name="N">Number of samples</typeparam>
	/// <typeparam name="TData">Data type</typeparam>
	template <ArrIndex N, ArrayData TData>
		requires (N >= 3)
	class Shape<2, N, TData> : public ShapeBase<2, N, TData>
	{
		THIS_DEF(Shape);

	public:
		/// <summary>
		/// Generates the set of lines along the perimeter.
		/// </summary>
		/// <param name="buffer">Buffer to hold the lines</param>
		/// <returns>Passthrough reference to the buffer</returns>
		_NODISCARD virtual typename This::LineType* lines(typename This::LineType buffer[This::PointCount]) const
		{
			typename This::PointType ptBuffer[N];
			const typename This::PointType* pts = this->points(ptBuffer);

			for (ArrIndex i = 0; i < This::PointCount - 1; i++)
			{
				typename This::LineType& line = buffer[i];

				line.samples()[0] = pts[i];
				line.samples()[1] = pts[i + 1];
			}

			typename This::LineType& loopbackLine = buffer[This::PointCount - 1];

			loopbackLine.samples()[0] = pts[This::PointCount - 1];
			loopbackLine.samples()[1] = pts[0];

			return buffer;
		}

		/// <summary>
		/// Generates the set of vectors along the perimeter.
		/// </summary>
		/// <param name="buffer">Buffer to hold the vectors</param>
		/// <returns>Passthrough reference to the buffer</returns>
		_NODISCARD virtual typename This::VectorType* vectors(typename This::VectorType buffer[This::PointCount]) const
		{
			typename This::PointType ptBuffer[This::PointCount];
			const typename This::PointType* pts = this->points(ptBuffer);

			for (ArrIndex i = 0; i < This::PointCount - 1; i++)
				buffer[i] = pts[i + 1] - pts[i];

			buffer[This::PointCount - 1] = pts[0] - pts[This::PointCount - 1];

			return buffer;
		}
	};

	template <ArrIndex N, ArrayData TData = DefaultDataType>
	using Shape2D = Shape<2, N, TData>;
}
