#pragma once

namespace LibMath
{
	class Degree;

	class Radian
	{
	public:
		Radian();
		explicit Radian(float);
		Radian(Radian const&);

		operator Degree() const;

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
		/// <param name="signedQuadrant">Defines the bounds as [-π, π] or [0, 2π].</param>
		void wrap(bool signedQuadrant = false);

		/// <summary>
		/// Returns the raw angle value.
		/// </summary>
		float raw() const noexcept;

		Radian&	operator+=(Radian);
		Radian&	operator-=(Radian);
		Radian&	operator*=(float);
		Radian&	operator/=(float);

	private:
		float m_value;
	};

	bool	operator==(Radian left, Radian right);
	bool	operator==(Radian left, Degree const& right);

	Radian	operator-(Radian self);

	Radian	operator+(Radian left, Radian right);
	Radian	operator-(Radian left, Radian right);
	Radian	operator*(Radian left, float right);
	Radian	operator/(Radian left, float right);

	inline namespace Literal
	{
		Radian operator""_rad(long double val);			// Radian angle = 0.5_rad;
		Radian operator""_rad(unsigned long long val);	// Radian angle = 1_rad;
	}
}
