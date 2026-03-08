#pragma once

#include "Real.hpp"

namespace maths
{

template<typename T>
const T&	min( const T& a, const T& b ) noexcept
{
	return (b < a) ? b : a;
}

template<typename T>
const T&	max( const T& a, const T& b) noexcept
{
	return (a < b) ? b : a;
}

template<typename T>
T			abs( T x ) noexcept
{
	return (x < static_cast<T>(0) ? -x : x);
}

template<typename T>
T			pow( T base, int exp )
{
	if (exp < 0)
		return (T(1) / pow(base, -exp));

	T	res = 1.0;

	while (exp > 0)
	{
		if (exp % 2 == 1)
			res *= base;
		base *= base;
		exp /= 2;
	}
	return (res);
}

inline Real	sqrt( Real x )
{
	if (x <= 0.0)
		return (0.0);

	Real		guess = x / 2;
	const Real	tolerance = 1e-6;

	while (true)
	{
		Real	other_side = x / guess;
		Real	new_guess = (guess + other_side) / 2;

		Real	diff = abs(guess - new_guess);

		if (diff < tolerance)
			return (new_guess);
		guess = new_guess;
	}
}

inline double	cos( double x, size_t terms = 20 )
{
	while (x < 0)
		x += 2 * M_PI;
	while (x > 2 * M_PI)
		x -= 2 * M_PI;
	
	double	result = 1.0;
	double	term = 1.0;

	for (size_t k = 1; k < terms; k++)
	{
		term *= -x * x / ((2 * k) * (2 * k - 1));
		result += term;
	}
	return (result);
}

inline double	sin( double x, size_t terms = 20 )
{
	while (x < -M_PI)
		x += 2 * M_PI;
	while (x > M_PI)
		x -= 2 * M_PI;

	double	result = x;
	double	term = x;

	for (size_t k = 1; k < terms; k++)
	{
		term *= -x * x / ((2 * k) * (2 * k + 1));
		result += term;
	}
	return (result);
}

inline double	arctan( double x, size_t terms = 20 )
{
	double	x_abs = maths::abs(x);

	double	num;
	double	step;

	if (x_abs <= 1)
	{
		num = x_abs;
		step = x_abs * x_abs;
	}
	else
	{
		num = 1.0 / x_abs;
		step = 1.0 / (x_abs * x_abs);
	}
	
	double	sum = 0.0;
	int		denom = 1;
	int		sign = 1;

	for (size_t k = 0; k < terms; k++)
	{
		sum += sign * (num / denom); 
		num *= step;
		denom += 2;
		sign = -sign;
	}
	
	if (x_abs > 1)
		sum = M_PI / 2.0 - sum;
	if (x < 0)
		sum = -sum;

	return (sum);
}

inline double	arctan2( double a, double b )
{
	if (b == 0)
	{
		if (a >= 0) return (0.0);
		else  return (M_PI);
	}

	if (a == 0)
	{
		if (b > 0) return (M_PI / 2);
		else return (-M_PI / 2);
	}
	double	ratio = maths::abs(b / a);
	double	alpha = maths::arctan(ratio);
	double	theta = 0.0;

	if (a > 0 && b > 0)		 theta = alpha;
	else if (a < 0 && b > 0) theta = M_PI - alpha;
	else if (a < 0 && b < 0) theta = M_PI + alpha;
	else if (a > 0 && b < 0) theta = -alpha;

	return (theta);
}

}