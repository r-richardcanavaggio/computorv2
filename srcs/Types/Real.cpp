
#include "Real.hpp"

Real::Real() : value(0.0) {}

Real::Real( double r ) : value(r) {}

Real	Real::operator+( const Real& other ) const noexcept
{
	Real	r(0.);

	r.value = this->value + other.value;
	return (r);
}

Real	Real::operator-( const Real& other ) const noexcept
{
	Real	r(0.);

	r.value = this->value - other.value;
	return (r);
}

Real	Real::operator*( const Real& other ) const noexcept
{
	Real	r(0.);
	
	r.value = this->value * other.value;
	return (r);
}

Real	Real::operator/( const Real& other ) const
{
	if (other.value == 0.)
		throw std::runtime_error("Error: cannot divide by zero");

	Real	r(0.);

	r.value = this->value / other.value;
	return (r);
}

Real	Real::operator%( const Real& other ) const
{
	if (other == 0)
		throw std::runtime_error("Error: cannot modulo by zero");

	long	quotient = static_cast<long>(*this / other);

	return (*this - static_cast<Real>(quotient) * other);
}

Real::operator long() const
{
	return (static_cast<long>(this->value));
}

Real::operator int() const
{
	return (static_cast<int>(this->value));
}

Real::operator double() const
{
	return (this->getValue());
}

Real	Real::operator-() const noexcept
{
	return (Real(-value));
}

Real&	Real::operator+=( const Real& other ) noexcept
{
	value += other.value;
	return (*this);
}

Real&	Real::operator-=( const Real& other ) noexcept
{
	value -= other.value;
	return (*this);
}

Real&	Real::operator*=( const Real& other ) noexcept
{
	value *= other.value;
	return (*this);
}

Real&	Real::operator/=( const Real& other )
{
	if (other.value == 0.)
		throw std::runtime_error("Error: cannot divide by zero.");

	value /= other.value;
	return (*this);
}

bool	Real::operator==( const Real& other ) const noexcept
{
	return (this->value == other.value);
}

double	Real::getValue() const
{
	return (this->value);
}

std::ostream&	operator<<( std::ostream& os, const Real& r )
{
	os << r.value;
	return (os);
}

Real	Real::square() const noexcept
{
	return (Real(value * value));
}

Real	Real::abs() const
{
	return ((value < 0) ? -value : value);
}
