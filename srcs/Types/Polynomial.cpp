#include "Types/Polynomial.hpp"
#include "Constants.hpp"

Polynomial Polynomial::operator-() const
{
	Polynomial result;

	result.m_coeffs.reserve( m_coeffs.size() );
	for (auto val : m_coeffs)
		result.m_coeffs.push_back(-val);
	return result;
}

Polynomial& Polynomial::operator+=( const Polynomial& other )
{
	if (m_coeffs.size() < other.m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size(), 0);

	for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] += other.m_coeffs[i];
	trim();
	return *this;
}

Polynomial& Polynomial::operator-=( const Polynomial& other )
{
	if (m_coeffs.size() < other.m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size(), 0);

	for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] -= other.m_coeffs[i];
	trim();
	return *this;
}

Polynomial& Polynomial::operator*=( const Polynomial& other )
{
	if (m_coeffs.empty() || other.m_coeffs.empty())
	{
		m_coeffs.clear();
		return *this;
	}
	std::size_t		  newSize = m_coeffs.size() + other.m_coeffs.size() - 1;
	std::vector<Real> res( newSize, 0 );

	for (std::size_t i = 0; i < m_coeffs.size(); ++i)
	{
		for (std::size_t j = 0; j < other.m_coeffs.size(); ++j)
			res[i + j] += m_coeffs[i] * other.m_coeffs[j];
	}
	m_coeffs = std::move(res);
	trim();
	return *this;
}

namespace maths
{

Polynomial	pow( const Polynomial& p, int n )
{
	if (n < 0)
		throw std::runtime_error( "Math Error: Negative exponent for polynomials not supported." );

	if (n == 0)
		return Polynomial(1);

	Polynomial	res(p);

	for (int i = 1; i < n; ++i)
		res *= p;
	return res;
}

}

Real Polynomial::eval( Real x ) const
{
	Real result;

	for (std::size_t i = 0; i < m_coeffs.size(); ++i)
		result += m_coeffs[i] * maths::pow( x, i );
	return result;
}

Polynomial Polynomial::eval( const Polynomial& p ) const
{
	Polynomial result;

	for (std::size_t i = 0; i < m_coeffs.size(); ++i)
		result += m_coeffs[i] * maths::pow(p, i);
	return result;
}

Polynomial::Result Polynomial::solve() const
{
	switch (degree())
	{
		case 0: return Result::solveDegree0(m_coeffs);
		case 1: return Result::solveDegree1(m_coeffs);
		case 2: return Result::solveDegree2(m_coeffs);
		default: 
			throw std::runtime_error("Math Error: Polynomial evaluation for degree > 2 not supported.");
	}
}

std::ostream& operator<<( std::ostream& os, const Polynomial& p )
{
	if (p.m_coeffs.empty())
		return os << "0";

	bool printedAny = false;

	for (std::size_t i = p.m_coeffs.size(); i-- > 0;)
	{
		Real coeff = p.m_coeffs[i];

		if (coeff == 0)
			continue;

		if (printedAny)
			os << ( coeff >= 0 ? " + " : " - " );
		else if (coeff < 0)
			os << "-";

		Real mag = maths::abs( coeff );

		if (i == 0 || mag != 1)
			os << mag;
		if (i > 0)
		{
			os << p.m_var;
			if (i > 1)
				os << "^" << i;
		}
		printedAny = true;
	}
	if (!printedAny)
		return ( os << "0" );
	return ( os );
}
