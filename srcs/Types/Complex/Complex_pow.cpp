#include "Complex.hpp"

Complex	Complex::pow( int exp ) const
{
	Real	m = abs();
	double	a = getReal().getReal();
	double	b = getImag().getReal();
	double	theta = maths::arctan2(a, b);

	Complex	result(0);

	result.setReal(maths::cos(exp * theta));
	result.setImag(maths::sin(exp * theta));
	result *= maths::pow(m, exp);
	return (result);
}
