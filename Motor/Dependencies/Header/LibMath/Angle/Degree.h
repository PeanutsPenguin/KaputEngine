#pragma once

namespace LibMath
{
	class Radian;

	class Degree
	{
	public:
		Degree();
		explicit Degree(float value);
		Degree(Degree const& deg);

		operator Radian() const;

		/// <summary>
		/// Returns the value in degrees within bounds.
		/// </summary>
		/// <param name="signedQuadrant">Defines the bounds as [-180; 180] or [0; 360]</param>
		float degree(bool signedQuadrant = false) const;

		/// <summary>
		/// Returns the value in radians within bounds.
		/// </summary>
		/// <param name="signedQuadrant">Defines the bounds as [-π; π] or [0; 2π]</param>
		float radian(bool signedQuadrant = true) const;

		/// <summary>
		/// Wraps back the value back in bounds.
		/// </summary>
		/// <param name="signedQuadrant">Defines the bounds as [-180; 180] or [0; 360]</param>
		void wrap(bool signedQuadrant = false);

		/// <summary>
		/// Returns the raw angle value.
		/// </summary>
		float raw() const noexcept;

		Degree&	operator+=(Degree right);
		Degree&	operator-=(Degree right);
		Degree& operator*=(float right);
		Degree&	operator/=(float right);

	private:
		float m_value;
	};

	bool	operator==(Degree left, Degree right);
	bool	operator==(Degree left, Radian const& right);

	Degree	operator-(Degree self);

	Degree	operator+(Degree left, Degree right);
	Degree	operator-(Degree left, Degree right);
	Degree	operator*(Degree left, float right);
	Degree	operator/(Degree left, float right);

	inline namespace Literal
	{
		LibMath::Degree operator""_deg(long double val);
		LibMath::Degree operator""_deg(unsigned long long val);
	}
}
