#include "Types/Polynomial.hpp"
#include "Constants.hpp"

Polynomial::Polynomial( std::initializer_list<Real> list ) : m_coeffs(list)
{
	trim();
}

Polynomial::Polynomial( Real x )
{
	if (x != 0)
		m_coeffs.push_back(x);
}

std::size_t	Polynomial::degree() const noexcept
{
	return m_coeffs.empty() ? 0 : m_coeffs.size() - 1;
}

void	Polynomial::setName( const std::string& name )
{
	m_var = name;
}

Root	Polynomial::eval( const Root& r ) const
{
	return std::visit([this]( const auto& val ) -> Root { return this->eval(val); }, r);
}

Polynomial::Result	Polynomial::solve() const
{
	if (degree() == 0) return solveDegree0();
	else if (degree() == 1) return solveDegree1();
	else if (degree() == 2) return solveDegree2();
	else
		throw std::runtime_error("Math Error: Polynomial evaluation for degree > 2 not supported.");
}

Polynomial		Polynomial::pow( int exp ) const
{
	if (exp < 0)
		throw std::runtime_error("Math Error: Negative exponent for polynomials not supported.");
	Polynomial	res(1);
	Polynomial	base = *this;

	while (exp > 0)
	{
		if (exp % 2 == 1)
			res = res * base;
		base *= base;
		exp /= 2;
	}
	return res;
}

Polynomial&	Polynomial::operator+=( const Polynomial& other )
{
	if (m_coeffs.size() < other.m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size(), 0);

	for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] += other.m_coeffs[i];
	trim();
	return *this;
}

Polynomial&	Polynomial::operator+=( Real x )
{
	if (m_coeffs.empty())
	{
		if (x != 0)
			m_coeffs.push_back(x);
		return *this;
	}
	m_coeffs[0] += x;
	trim();
	return *this;
}

Polynomial&	Polynomial::operator-=( const Polynomial& other )
{
	if (m_coeffs.size() < other.m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size(), 0);

	for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] = m_coeffs[i] - other.m_coeffs[i];
	trim();
	return *this;
}

Polynomial&	Polynomial::operator-=( Real x )
{
	if (m_coeffs.empty())
	{
		if (x != 0)
			m_coeffs.push_back(-x);
		return *this;
	}
	m_coeffs[0] -= x;
	trim();
	return *this;
}

Polynomial	Polynomial::operator-() const
{
	Polynomial	result;

	result.m_coeffs.reserve(m_coeffs.size());
	for (auto val : m_coeffs)
		result.m_coeffs.push_back(-val);
	return result;
}

Polynomial&	Polynomial::operator*=( const Polynomial& other )
{
	if (m_coeffs.empty() || other.m_coeffs.empty())
	{
		m_coeffs.clear();
		return *this;
	}
	std::size_t			newSize = m_coeffs.size() + other.m_coeffs.size() - 1;
	std::vector<Real>	res(newSize, 0);

	for (std::size_t i = 0; i < m_coeffs.size(); ++i)
	{
		for (std::size_t j = 0; j < other.m_coeffs.size(); ++j)
			res[i + j] += m_coeffs[i] * other.m_coeffs[j];
	}
	m_coeffs = std::move(res);
	trim();
	return *this;
}

Polynomial&	Polynomial::operator*=( Real x )
{
	if (x == 0)
	{
		m_coeffs.clear();
		return *this;
	}
	for (std::size_t i = 0; i < m_coeffs.size(); ++i)
		m_coeffs[i] *= x;
	return *this;
}

Polynomial&	Polynomial::operator/=( const Polynomial& other )
{
	*this = euclideanDiv(other);
	return *this;
}

Polynomial&	Polynomial::operator/=( Real x )
{
	*this *= maths::inv(x);
	return *this;
}

Polynomial&	Polynomial::operator%=( const Polynomial& other )
{
	*this = euclideanDiv(other, true);
	return *this;
}

void		Polynomial::trim() noexcept
{
	while (!m_coeffs.empty() && maths::abs(m_coeffs.back()) < maths::eps)
		m_coeffs.pop_back();
}

std::ostream&   operator<<( std::ostream& os, const Root& r )
{
	std::visit([&os]( const auto & val ) { os << val; }, r);
	return os;
}

std::ostream&	operator<<( std::ostream& os, const Polynomial& p )
{
	if (p.m_coeffs.empty())
		return os << "0";

	bool	printedAny = false;

	for (std::size_t i = p.m_coeffs.size(); i-- > 0;)
	{
		Real	coeff = p.m_coeffs[i];

		if (coeff == 0)
			continue;

		if (printedAny)
			os << (coeff >= 0 ? " + " : " - ");
		else if (coeff < 0)
			os << "-";

		Real	mag = maths::abs(coeff);

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
		return (os << "0");
	return (os);
}

std::ostream&   operator<<( std::ostream& os, const Polynomial::Result& res )
{
	os << "Polynomial degree: " << res.degree << '\n';
	if (res.degree == 0)
	{
		if (res.isInf)
			os << "All real numbers are solution\n";
		else
			os << "No solution\n";
		return os;
	}
	if (res.degree == 1)
		return os << "The solution is:\n" << res.roots[0] << '\n';
	Real	delta = res.delta;

	os << "Discriminant is " << delta << '\n';
	if (delta > 0)
		os << "Strictly positive, the two solutions are:\n"
			<< res.roots[0] << '\n' << res.roots[1] << '\n';
	else if (delta == 0)
		os << "Discriminant is null, the solution is:\n"
			<< res.roots[0] << '\n';
	else
		os << "Strictly negative, the two solutions are:\n"
			<< res.roots[0] << '\n' << res.roots[1] << '\n';
	return os;
}

Polynomial  Polynomial::euclideanDiv( const Polynomial& b, bool mod ) const
{
	if (b.m_coeffs.empty())
		throw std::runtime_error("Error: division by polynomial zero.");

	Polynomial	remainder = *this, quotient, term;

	while (!remainder.m_coeffs.empty() && remainder.degree() >= b.degree())
	{
		auto	degDiff = remainder.degree() - b.degree();
		auto	coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();

		term.m_coeffs.resize(degDiff + 1, 0);
		term.m_coeffs.back() = coefRatio;

		quotient += term;
		remainder -= b * term;

		remainder.trim();
	}
	if (mod) return remainder;
	return quotient;
}

Polynomial::Result	Polynomial::solveDegree0() const noexcept
{
	if (m_coeffs.empty() || m_coeffs[0] == 0)
		return Result{{}, 0, true};
	return Result{{}, 0, false};
}

Polynomial::Result	Polynomial::solveDegree1() const noexcept
{
	return {{Real(-m_coeffs[0] / m_coeffs[1])}, 1};
}

Polynomial::Result Polynomial::solveDegree2() const
{
	Real				a = m_coeffs[2], b = m_coeffs[1], c = m_coeffs[0];
	Real				twoA = a * 2.;
	Real				delta = maths::square(b) - a * c * 4.;
	std::vector<Root>	res;

	if (delta > 0)
	{
		res.push_back((-b - maths::sqrt(delta)) / twoA);
		res.push_back((-b + maths::sqrt(delta)) / twoA);
	}
	else if (delta == 0)
		res.push_back(-b / twoA);
	else
	{
		Real	re = -b / twoA;
		Real	im = maths::sqrt(-delta) / twoA;
		res.push_back(Complex(re, -im));
		res.push_back(Complex(re, im));
	}
	return {res, 2, false, delta};
}
