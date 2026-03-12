
#include "Maths.hpp"

namespace maths
{

Real	min( const Real& a, const Real& b )
{
	return ((b < a) ? b : a);
}

Real	max( const Real& a, const Real& b )
{
	return ((a < b) ? b : a);
}

Real	abs( const Real& x )
{
	return ((x < 0) ? -x : x);
}

Real	abs( const Complex& z )
{
	Real	a = z.getReal();
	Real	b = z.getImag();

	return (sqrt(pow(a, 2) + pow(b, 2)));
}

Real	pow( const Real& base, int exp )
{
	if (exp < 0)
		return (Real(1) / pow(base, -exp));

	Real	res = 1.0;
	Real	x = base;

	while (exp > 0)
	{
		if (exp % 2 == 1)
			res *= x;
		x *= x;
		exp /= 2;
	}
	return (res);
}

Complex	pow( const Complex& base, int exp )
{
	Real	m = abs(base);
	Real	a = base.getReal();
	Real	b = base.getImag();
	Real	theta = maths::arctan2(a, b);
	Complex	result(0);

	result.setReal(maths::cos(Real(exp) * theta));
	result.setImag(maths::sin(Real(exp) * theta));
	result *= maths::pow(m, exp);
	return (result);
}

Real	sqrt( const Real& x )
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

Complex	sqrt( const Complex& z )
{
	Real	z_abs = maths::abs(z);

	Real	real_part = maths::sqrt((z_abs + z.getReal()) / Real(2));
	Real	imag_part = z.getImag() / maths::abs(z.getImag()) * maths::sqrt((z_abs - z.getReal()) / Real(2));

	return (Complex(real_part, imag_part));
}

static Complex	exp_complex( const Complex& z, size_t terms )
{
	Complex	sum(1, 0);
	Complex	current_term(1, 0);

	for (size_t i = 1; i < terms; i++)
	{
		current_term = (current_term * z) / Real(i);
		sum += current_term;
	}
	return (sum);
}

Complex cos( const Complex& z, size_t terms )
{
	Complex iz(-z.getImag(), z.getReal());
	Complex	numerator = maths::exp_complex(iz, terms) + maths::exp_complex(-iz, terms);

	return (numerator / Real(2));
}

Real	cos( const Real& r, size_t terms )
{
	Real	x = r;

	while (x < 0)
		x += 2 * pi;
	while (x > 2 * pi)
		x -= 2 * pi;
	
	Real	result = 1.0;
	Real	term = 1.0;

	for (size_t k = 1; k < terms; k++)
	{
		term *= -x * x / ((2 * k) * (2 * k - 1));
		result += term;
	}
	return (result);
}

Complex sin( const Complex& z, size_t terms )
{
	Complex iz(-z.getImag(), z.getReal());
	Complex	numerator = maths::exp_complex(iz, terms) - maths::exp_complex(-iz, terms);

	return (numerator / Complex(0, 2));
}

Real	sin( const Real& r, size_t terms )
{
	Real	x = r;

	while (x < -pi)
		x += 2 * pi;
	while (x > pi)
		x -= 2 * pi;

	Real	result = x;
	Real	term = x;

	for (size_t k = 1; k < terms; k++)
	{
		term *= -x * x / ((2 * k) * (2 * k + 1));
		result += term;
	}
	return (result);
}

Real	tan( const Real& x, size_t terms )
{
	return (Real(maths::sin(x, terms) / maths::cos(x, terms)));
}

Complex	tan( const Complex& z, size_t terms )
{
	return (sin(z, terms) / cos(z, terms));
}

static double	arctan( double x, size_t terms = 50 )
{
	double	x_abs = static_cast<double>(abs(x));
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
		sum = pi / 2.0 - sum;
	if (x < 0)
		sum = -sum;

	return (sum);
}

static double	arctan2( double a, double b )
{
	if (b == 0)
	{
		if (a >= 0) return (0.0);
		else  return (pi);
	}

	if (a == 0)
	{
		if (b > 0) return (pi / 2);
		else return (-pi / 2);
	}
	double	ratio = static_cast<double>(abs(b / a));
	double	alpha = arctan(ratio);
	double	theta = 0.0;

	if (a > 0 && b > 0)		 theta = alpha;
	else if (a < 0 && b > 0) theta = pi - alpha;
	else if (a < 0 && b < 0) theta = pi + alpha;
	else if (a > 0 && b < 0) theta = -alpha;

	return (theta);
}

Real arctan( const Real& x, size_t terms )
{
	return (Real(arctan(x.getReal(), terms)));
}

Real	arctan2( const Real& a, const Real& b )
{
	return (Real(arctan2(a.getReal(), b.getReal())));
}
}