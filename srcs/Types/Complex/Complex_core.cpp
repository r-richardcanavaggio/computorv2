#include "Complex.hpp"

Complex::Complex() : real(Real()), imag(Real()) {}

Complex::Complex( Real r, Real i ) : real(r), imag(i) {}

Complex::Complex( Real r ) : real(r), imag(0.) {}

Real	Complex::getReal() const
{
	return (this->real);
}

Real	Complex::getImag() const
{
	return (this->imag);
}

void	Complex::setReal( Real value )
{
	this->real = value;
}

void	Complex::setImag( Real value )
{
	this->imag = value;
}

Complex	Complex::conj() const
{
	Complex	z(0, 0);

	z.real = this->real;
	z.imag = -this->imag;
	return (z);
}

bool	Complex::operator==( const Complex& other ) const noexcept
{
	return (this->real == other.real && this->imag == other.imag);
}
