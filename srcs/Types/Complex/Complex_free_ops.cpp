#include "Complex.hpp"

Complex	operator+( const Complex& z, const Real& x )
{
	Complex	z1(0);

	z1.setReal(z.getReal() + x.getValue());
	z1.setImag(z.getImag());
	return (z1);
}

Complex	operator+( const Real& x, const Complex& z )
{
	Complex	z1(0);

	z1.setReal(z.getReal() + x.getValue());
	z1.setImag(z.getImag());
	return (z1);
}

Complex	operator-( const Complex& z, const Real& x )
{
	Complex	z1(0);

	z1.setReal(z.getReal() - x.getValue());
	z1.setImag(z.getImag());
	return (z1);
}

Complex	operator-( const Real& x, const Complex& z )
{
	Complex	z1(0);

	z1.setReal(x - z.getReal());
	z1.setImag(-z.getImag());
	return (z1);
}

Complex	operator*( const Complex& z, const Real& x )
{
	Complex	z1(0);

	z1.setReal(x * z.getReal());
	z1.setImag(x * z.getImag());
	return (z1);
}

Complex	operator*( const Real& x, const Complex& z )
{
	Complex	z1(0);

	z1.setReal(x * z.getReal());
	z1.setImag(x * z.getImag());
	return (z1);
}

Complex	operator/( const Complex& z, const Real& x )
{
	if (x.getValue() == 0.)
		throw std::runtime_error("Error: cannot divide by zero");

	Complex z1(0);

	z1.setReal(z.getReal() / x.getValue());
	z1.setImag(z.getImag() / x.getValue());
	return (z1);
}

Complex	operator/( const Real& x, const Complex& z )
{
	if (z.getReal() == 0. && z.getImag() == 0.)
		throw std::runtime_error("Error: cannot divide by zero");

	Complex z1(0);

	z1.setReal(x / z.getReal());
	z1.setImag(x / z.getImag());
	return (z1);
}
