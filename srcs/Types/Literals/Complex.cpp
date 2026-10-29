#include "Complex.hpp"
#include "Maths/Maths.hpp"

Real	Complex::norm() const
{
	return maths::sqrt(real * real + imag * imag);
}

Complex	Complex::pow( int exp ) const
{
	Real	m = norm();
	auto	theta = maths::arctan2(real, imag);

	return Complex(maths::cos(theta * exp), maths::sin(theta * exp)) * maths::pow(m, exp);
}

std::ostream&	operator<<( std::ostream& os, Complex z )
{
	auto near = [&]( Real x, Real target )
	{
		return (Real(x - target).abs() < maths::eps);
	};

	Real	re = z.real;
	Real	im = z.imag;

	if (im.abs() < maths::eps)
	{
		os << re;
		return (os);
	}
	if (re.abs() < maths::eps)
	{
		if (near(im, 1))
			os << "i";
		else if (near(im, -1))
			os << "-i";
		else
			os << im << "i";
		return (os);
	}
	os << re << " ";
	if (near(im, 1))
		os << "+ i";
	else if (near(im, -1))
		os << " - i";
	else if (im > 0)
		os << "+ " << im << "i";
	else
		os << "- " << -im << "i";
	return (os);
}
