#include "Types/Polynomial.hpp"

std::ostream&   operator<<( std::ostream& os, const Root& r )
{
	std::visit([&os]( const auto & val ) { os << val; }, r);
	return os;
}

std::ostream&   operator<<( std::ostream& os, const SolveResult& res )
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
		auto		degDiff = remainder.degree() - b.degree();
		auto		coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();

		term.m_coeffs.resize(degDiff + 1, 0);
		term.m_coeffs.back() = coefRatio;

		quotient += term;
		remainder -= b * term;

		remainder.trim();
	}
	if (mod) return remainder;
	return quotient;
}

SolveResult	Polynomial::solveDegree0() const noexcept
{
	if (m_coeffs.empty() || m_coeffs[0] == 0)
		return SolveResult{{}, 0, true};
	return SolveResult{{}, 0, false};
}

SolveResult	Polynomial::solveDegree1() const noexcept
{
	return {{Real(-m_coeffs[0] / m_coeffs[1])}, 1};
}

SolveResult Polynomial::solveDegree2() const
{
	Real				a = m_coeffs[2], b = m_coeffs[1], c = m_coeffs[0];
	Real				twoA = 2 * a;
	Real				delta = square(b) - 4 * a * c;
	std::vector<Root>	res;

	if (delta > 0)
	{
		res.push_back((-b - sqrt(delta)) / twoA);
		res.push_back((-b + sqrt(delta)) / twoA);
	}
	else if (delta == 0)
		res.push_back(-b / twoA);
	else
	{
		Real	re = -b / twoA;
		Real	im = sqrt(-delta) / twoA;
		res.push_back(Complex(re, -im));
		res.push_back(Complex(re, im));
	}
	return {res, 2, false, delta};
}
