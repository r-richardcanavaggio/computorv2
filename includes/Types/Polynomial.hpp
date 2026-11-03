#pragma once

#include <vector>
#include <string>
#include <variant>
#include <array>

#include "Real.hpp"
#include "Complex.hpp"
#include "Matrix.hpp"

using Root = std::variant<Real, Complex>;

struct SolveResult
{
	std::vector<Root>	roots;
	short				degree = 0;
	bool				isInf = false;
	Real				delta = 0;
};

class Polynomial
{
  private:
	std::vector<Real>	m_coeffs;
	std::string			m_var = "x";

	Polynomial	euclideanDiv( const Polynomial& b, bool mod = false ) const;
	SolveResult	solveDegree0() const noexcept;
	SolveResult	solveDegree1() const noexcept;
	SolveResult	solveDegree2() const;
	void		trim() noexcept
	{
		while (!m_coeffs.empty() && abs(m_coeffs.back()) < maths::eps)
			m_coeffs.pop_back();
	}

  public:
	Polynomial() = default;
	Polynomial( std::initializer_list<Real> list ) : m_coeffs(list) { trim(); }
	Polynomial( Real x )
	{
		if (x != 0)
			m_coeffs.push_back(x);
	}

	Polynomial(const Polynomial&) = default;
	Polynomial&	operator=(const Polynomial&) = default;
	Polynomial(Polynomial&&) noexcept = default;
	Polynomial&	operator=(Polynomial&&) noexcept = default;

	std::size_t	degree() const noexcept { return m_coeffs.empty() ? 0 : m_coeffs.size() - 1; }

	void	setName( std::string_view name ) { m_var = std::string(name); }

	template < typename T >
	T	eval( const T& x ) const
	{
		if (m_coeffs.empty())
			return T(0);

		T	result(0);

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
			result = result * x + m_coeffs[i];
		return result;
	}

	template < real_complex K >
	Matrix<K>	eval( const Matrix<K>& m ) const
	{
		if (m.getRows() != m.getCols())
			throw std::runtime_error("Error: Evaluation only possible on square matrices");

		Matrix<K>	result(m.getRows(), m.getCols());
		Matrix<K>	id = Matrix<K>::identity(m.getRows());

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
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

	Root	eval( const Root& r ) const
	{
		return std::visit([this]( const auto& val ) -> Root { return this->eval(val); }, r);
	}

	SolveResult	solve() const
	{
		if (degree() == 0) return solveDegree0();
		else if (degree() == 1) return solveDegree1();
		else if (degree() == 2) return solveDegree2();
		else
			throw std::runtime_error("Math Error: Polynomial evaluation for degree > 2 not supported.");
	}

	Polynomial	pow( int exp ) const
	{
		if (exp < 0) throw std::runtime_error("Math Error: Negative exponent for polynomials not supported.");
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

	Polynomial&	operator+=( const Polynomial& other )
	{
		if (m_coeffs.size() < other.m_coeffs.size())
			m_coeffs.resize(other.m_coeffs.size(), 0);

		for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
			m_coeffs[i] += other.m_coeffs[i];
		trim();
		return *this;
	}
	Polynomial&	operator+=( Real x )
	{
		if (m_coeffs.empty())
		{
			if (x != 0) m_coeffs.push_back(x);
			return *this;
		}
		m_coeffs[0] += x;
		trim();
		return *this;
	}
	friend Polynomial operator+( Polynomial a, const Polynomial& b ) { a += b; return a; }
	friend Polynomial operator+( Polynomial p, Real x ) { p += x; return p; }
	friend Polynomial operator+( Real x, Polynomial p ) { p += x; return p; }

	Polynomial& operator-=( const Polynomial& other )
	{
		if (m_coeffs.size() < other.m_coeffs.size()) m_coeffs.resize(other.m_coeffs.size(), 0);

		for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
			m_coeffs[i] = m_coeffs[i] - other.m_coeffs[i];
		trim();
		return *this;
	}
	Polynomial& operator-=( Real x )
	{
		if (m_coeffs.empty())
		{
			if (x != 0) m_coeffs.push_back(-x);
			return *this;
		}
		m_coeffs[0] -= x;
		trim();
		return *this;
	}
	friend Polynomial operator-( Polynomial a, const Polynomial& b ) { a -= b; return a; }
	friend Polynomial operator-( Polynomial p, Real x ) { p -= x; return p; }
	friend Polynomial operator-( Real x, Polynomial p ) { return -p + x; }

	Polynomial	operator-() const
	{
		Polynomial	result;
		result.m_coeffs.reserve(m_coeffs.size());
		for (auto val : m_coeffs) result.m_coeffs.push_back(-val);
		return result;
	}

	Polynomial&	operator*=( const Polynomial& other )
	{
		if (m_coeffs.empty() || other.m_coeffs.empty())
		{
			m_coeffs.clear();
			return *this;
		}
		std::size_t			newSize = m_coeffs.size() + other.m_coeffs.size() - 1;
		std::vector<Real>	res(newSize, 0);
		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
			for (std::size_t j = 0; j < other.m_coeffs.size(); ++j)
				res[i + j] += m_coeffs[i] * other.m_coeffs[j];
		m_coeffs = std::move(res);
		trim();
		return *this;
	}
	Polynomial&	operator*=( Real x )
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
	friend Polynomial operator*( Polynomial a, const Polynomial& b ) { a *= b; return a; }
	friend Polynomial operator*( Polynomial p, Real x ) { p *= x; return p; }
	friend Polynomial operator*( Real x, Polynomial p ) { p *= x; return p; }

	Polynomial&	operator/=( const Polynomial& other ) { return *this = euclideanDiv(other); }
	friend Polynomial operator/(Polynomial a, const Polynomial& b) { a /= b; return a;}
	Polynomial&	operator/=( Real x )
	{
		if (x == 0) throw std::runtime_error("Error: Division by zero.");
		return *this *= inv(x);
	}
	friend Polynomial operator/( Polynomial p, Real x ) { p /= x; return p; }
	friend Polynomial operator/( Real x, const Polynomial& p )
	{
		if (p.degree() > 0) throw std::runtime_error("Math Error: Division by a polynomial (rational function) is not supported.");
		return Polynomial(x) / p;
	}

	Polynomial operator%( const Polynomial& other ) const { return euclideanDiv(other, true); }
	Polynomial operator%=( const Polynomial& other ) { return *this = euclideanDiv(other, true); }

	bool operator==( const Polynomial& other ) const { return m_coeffs == other.m_coeffs; }

	friend std::ostream& operator<<( std::ostream& os, const Polynomial& p )
	{
		if (p.m_coeffs.empty()) return os << "0";

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

			Real	mag = abs(coeff);

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
};

std::ostream& operator<<( std::ostream& os, const Polynomial& p );
std::ostream& operator<<( std::ostream& os, const SolveResult& roots );