#pragma once

#include "LibMath/Angle/Radian.h"
#include "LibMath/MathArray/Linear/LinearMathArray3.h"
#include "LibMath/Matrix.h"
#include "LibMath/Trigonometry.h"
#include "LibMath/Rotor.h"

namespace LibMath
{
	template <ArrayData TData = DefaultDataType>
	class Transform : public Matrix<4, 4, TData>
	{
	public:
		MATHARRAY_CTORS(Transform, Matrix<4, 4, TData>)
		
		_NODISCARD static Transform generateTranslate(const LinearMathArray3<TData>& delta)
		{
			Transform out = Transform::Identity().template as<Transform>();
			out[3].template splice<0, 3>() = delta;

			return out;
		}

		_NODISCARD static Transform generateScale(const LinearMathArray3<TData>& scale)
		{
			Transform out = Transform::Identity().template as<Transform>();

			for (ArrIndex i = 0; i < 3; i++)
				out.raw2D()[i][i] = scale.raw()[i];

			return out;
		}

		_NODISCARD static Transform generateXRotation(Radian angle)
		{
			Transform out = Transform::Identity().template as<Transform>();

			float s = sin(angle), c = cos(angle);

			out.raw2D()[1][1] = c;
			out.raw2D()[1][2] = -s;
			out.raw2D()[2][1] = s;
			out.raw2D()[2][2] = c;

			return out;
		}

		_NODISCARD static Transform generateYRotation(Radian angle)
		{
			Transform out = Transform::Identity().template as<Transform>();

			float s = sin(angle), c = cos(angle);

			out.raw2D()[0][0] = c;
			out.raw2D()[2][0] = s;
			out.raw2D()[2][0] = -s;
			out.raw2D()[2][2] = c;

			return out;
		}

		_NODISCARD static Transform generateZRotation(Radian angle)
		{
			Transform out = Transform::Identity().template as<Transform>();

			float s = sin(angle), c = cos(angle);

			out.raw2D()[0][0] = c;
			out.raw2D()[0][1] = -s;
			out.raw2D()[1][0] = s;
			out.raw2D()[1][1] = c;

			return out;
		}

		_NODISCARD static Transform generateRotation(const Rotor<TData>& rotor)
		{
			const TData
				&x = rotor.x(),
				&y = rotor.y(),
				&z = rotor.z(),
				&w = rotor.w();

			return Matrix4f
			{
				1 - 2*y*y - 2*z*z,	2*x *y + 2*w*z,			2*x*z - 2*w*y,		0,
				2*x*y - 2*w*z,		1 - 2*x*x - 2*z*z,		2*y*z + 2*w*x,		0,
				2*x*z + 2*w*y,		2*y*z - 2*w *x,			1 - 2*x*x - 2*y*y,	0,
				0,					0,						0,					1
			};
		}
	};

	MATHARRAY_NUMERICALIASES(Transform)
}
