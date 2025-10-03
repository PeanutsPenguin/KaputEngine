#include "LibMath/Angle/Angle.h"
#include "LibMath/Angle/Degree.h"
#include "LibMath/Angle/Radian.h"
#include "LibMath/Arithmetic.h"
#include "LibMath/Trigonometry.h"

namespace LibMath
{
	Radian::Radian() : Radian(0.f) { }
	Radian::Radian(Radian const& rad) : Radian(rad.m_value) { }
	Radian::Radian(float value) : m_value(value) { }

	Radian::operator Degree() const
	{
		return Degree(m_value * (180.f / g_pi));
	}

	float Radian::degree(bool signedQuadrant) const
	{
		return Degree(*this).degree(signedQuadrant);
	}

	float Radian::radian(bool signedQuadrant) const
	{
		static const Angle::WrapRanges ranges =
		{
			{ 0.f, 2 * g_pi },
			{ -g_pi, g_pi }
		};

		return Angle::wrap(m_value, signedQuadrant, ranges);
	}

	void Radian::wrap(bool signedQuadrant)
	{
		m_value = radian(signedQuadrant);
	}

	float Radian::raw() const noexcept
	{
		return m_value;
	}

#pragma region Operators
	Radian& Radian::operator+=(Radian right)
	{
		m_value += right.m_value;
		return *this;
	}

	Radian& Radian::operator-=(Radian right)
	{
		m_value -= right.m_value;
		return *this;
	}

	Radian& Radian::operator*=(float right)
	{
		m_value *= right;
		return *this;
	}

	Radian& Radian::operator/=(float right)
	{
		m_value /= right;
		return *this;
	}

	bool operator==(Radian left, Radian right)
	{
		float lRad = left.radian(), rRad = right.radian();
		return almostEqual(lRad, rRad, Angle::Epsilon);
	}

	bool operator==(Radian left, Degree const& right)
	{
		return almostEqual(left.radian(), right.radian(), Angle::Epsilon);
	}

	Radian	operator-(Radian self)
	{
		return Radian(-self.raw());
	}

	Radian operator+(Radian left, Radian right)
	{
		return Radian(left.raw() + right.raw());
	}

	Radian operator-(Radian left, Radian right)
	{
		return Radian(left.raw() - right.raw());
	}

	Radian operator*(Radian left, float right)
	{
		return Radian(left.raw() * right);
	}

	Radian operator/(Radian left, float right)
	{
		return Radian(left.raw() / right);
	}

	Radian Literal::operator""_rad(long double val)
	{
		return Radian(static_cast<float>(val));
	}

	Radian Literal::operator""_rad(unsigned long long val)
	{
		return Radian(static_cast<float>(val));
	}
#pragma endregion
}
