#include "LibMath/Angle/Angle.h"
#include "LibMath/Angle/Degree.h"
#include "LibMath/Angle/Radian.h"
#include "LibMath/Arithmetic.h"
#include "LibMath/Trigonometry.h"

namespace LibMath
{
	Degree::Degree() : Degree(0.f) { }
	Degree::Degree(Degree const& deg) : Degree(deg.m_value) { }
	Degree::Degree(float value) : m_value(value) { }

	Degree::operator Radian() const
	{
		return Radian(m_value * (float)(g_pi / 180.f));
	}

	float Degree::degree(bool signedQuadrant) const
	{
		static const Angle::WrapRanges ranges =
		{
			{ 0.f, 360.f },
			{ -180.f, 180.f }
		};

		return Angle::wrap(m_value, signedQuadrant, ranges);
	}

	float Degree::radian(bool signedQuadrant) const
	{
		return ((Radian)*this).radian(signedQuadrant);
	}

	void Degree::wrap(bool signedQuadrant)
	{
		m_value = degree(signedQuadrant);
	}

	float Degree::raw() const noexcept
	{
		return m_value;
	}

#pragma region Operators
	Degree& Degree::operator+=(Degree right)
	{
		m_value += right.m_value;
		return *this;
	}

	Degree& Degree::operator-=(Degree right)
	{
		m_value -= right.m_value;
		return *this;
	}

	Degree& Degree::operator*=(float right)
	{
		m_value *= right;
		return *this;
	}

	Degree& Degree::operator/=(float right)
	{
		m_value /= right;
		return *this;
	}

	bool operator==(Degree left, Degree right)
	{
		return almostEqual(left.degree(), right.degree(), Angle::Epsilon);
	}

	bool operator==(Degree left, Radian const& right)
	{
		return almostEqual(left.degree(), right.degree(), Angle::Epsilon);
	}

	Degree operator-(Degree self)
	{
		return Degree(-self.raw());
	}

	Degree operator+(Degree left, Degree right)
	{
		return Degree(left.raw() + right.raw());
	}

	Degree operator-(Degree left, Degree right)
	{
		return Degree(left.raw() - right.raw());
	}

	Degree operator*(Degree left, float right)
	{
		return Degree(left.raw() * right);
	}

	Degree operator/(Degree left, float right)
	{
		return Degree(left.raw() / right);
	}

	Degree Literal::operator""_deg(long double val)
	{
		return Degree((float)val);
	}

	Degree Literal::operator""_deg(unsigned long long val)
	{
		return Degree((float)val);
	}
#pragma endregion
}
