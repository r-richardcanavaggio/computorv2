#include "Complex.hpp"

Complex		Complex::operator+( const Complex& other ) const noexcept
{
	Complex z(0, 0);

	z.real = this->real + other.real;
	z.imag = this->imag + other.imag;
	return (z);
}

Complex		Complex::operator-( const Complex& other ) const noexcept
{
	Complex z(0, 0);

	z.real = this->real - other.real;
	z.imag = this->imag - other.imag;
	return (z);
}

Complex&	Complex::operator+=( const Complex& other ) noexcept
{
	real += other.real;
	imag += other.imag;
	return (*this);
}

Complex&	Complex::operator-=( const Complex& other ) noexcept
{
	real -= other.real;
	imag -= other.imag;
	return (*this);
}

Complex		Complex::operator-() const
{
	return (Complex(-real, -imag));
}
