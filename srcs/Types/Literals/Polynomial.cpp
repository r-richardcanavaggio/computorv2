#include "Polynomial.hpp"
#include "Maths/Maths.hpp"
#include <variant>

template<real_complex K>
Matrix<K>	Polynomial::eval( const Matrix<K>& m ) const
{
	if (m.getRows() != m.getCols())
		throw std::runtime_error("Evaluation only possible on square matrices");

	Matrix<K>	result(m.getRows(), m.getCols());
	Matrix<K>	id = m.identity();

	for (int i = 0; i <= degree(); i++)
	{
		if (m_coeffs[i] != 0)
		{
			if (i == 0)
				result += id * m_coeffs[i];
			else
				result += m.pow(i) * m_coeffs[i];
		}
	}
	return result;
}

std::ostream& operator<<( std::ostream& os, const SolveResult& res )
{
	os << "Polynomial degree: " << res.degree << '\n';
	if (res.degree == 0)
	{
		if (res.state == SolveResult::State::Infinite)
			os << "All real numbers are solution\n";
		else
			os << "No solution\n";
		return os;
	}
	if (res.degree == 1)
	{
		os << "The solution is:\n" << res.roots[0] << '\n';
		return os;
	}
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

std::ostream&	operator<<( std::ostream& os, const Root& r )
{
	std::visit([&os]( const auto & val ) { os << val; }, r);
	return os;
}

std::ostream&	operator<<( std::ostream& os, const Polynomial& p )
{
	if (p.empty()) return (os << "0");

	bool	printedAny = false;

	for (std::size_t i = p.size(); i-- > 0;)
	{
		Real	coeff = p.getCoeffs()[i];

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
			os << p.getName();
			if (i > 1)
				os << "^" << i;
		}
		printedAny = true;
	}
	if (!printedAny)
		return (os << "0");
	return (os);
}
