#pragma once

#include <iostream>
#include "Constants.hpp"
#include <optional>

namespace
{

template<typename T>
constexpr T 	gcd( T a, T b )
{
	if (b == 0) return (a);
	return gcd(b, a % b);
}

template<typename T>
constexpr T		customAbs( T value ) noexcept
{
	return value < 0 ? -value : value;
}

template<typename T>
constexpr T		customFloor( T value ) noexcept
{
	if (value >= 4503599627370496.0 || value <= -4503599627370496.0)
		return value;

	long long i = static_cast<long long>(value);
	if (value < 0 && value != static_cast<double>(i))
		return static_cast<double>(i - 1);
	return static_cast<double>(i);
}

}

struct Fraction
{
	long long	numerator = 0;
	long long	denominator = 1;

	constexpr Fraction() = default;
	constexpr Fraction( long long num, long long den = 1 )
		: numerator(num), denominator(den)
	{
		if (denominator == 0)
			throw std::invalid_argument("Error: Fraction denominator cannot be zero.");
		simplify();
	}

	constexpr void			simplify() noexcept
	{
		long long	g = gcd(abs(numerator), abs(denominator));

		if (g != 0)
		{
			numerator /= g;
			denominator /= g;
		}
		if (denominator < 0)
		{
			numerator = -numerator;
			denominator = -denominator;
		}
	}
	static constexpr std::optional<Fraction>	fromDouble( double value, double eps = maths::eps ) noexcept
	{
		double	num = value;
		double	h1 = 1., h2 = 0.;
		double	k1 = 0., k2 = 1.;

		for (std::size_t i = 0; i < 64; ++i)
		{
			double	a = customFloor(num);
			
			double	h = a * h1 + h2;
			double	k = a * k1 + k2;

			h2 = h1; h1 = h;
			k2 = k1; k1 = k;

			if (k != 0.)
			{
				double	approx = h / k;

				if (customAbs(value - approx) < eps)
					return Fraction(static_cast<long long>(h), static_cast<long long>(k));
			}
			double	fracPart = num - a;
			if (customAbs(fracPart) < eps)
				break;
			num = 1. / fracPart;
		}
		return std::nullopt;
	}

	friend std::ostream&	operator<<( std::ostream& os, const Fraction& f )
	{
		return os << f.numerator << "/" << f.denominator;
	}
};
