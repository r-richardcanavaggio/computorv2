#pragma once

#include "Real.hpp"
#include "Complex.hpp"
#include "Matrix.hpp"
#include "Maths/Maths.hpp"

#include <vector>
#include <string>
#include <variant>
#include <array>

#include "SolveResult.hpp"

class Polynomial
{
	private:
		std::vector<Real>	m_coeffs;
		std::string			m_variable = "x";

		constexpr std::pair<Polynomial, Polynomial>	euclideanDiv( const Polynomial& b ) const
		{
			if (b.empty()) throw std::runtime_error("Error: division by polynomial zero.");
			Polynomial	quotient;
			Polynomial	remainder = *this;

			while (!remainder.empty() && remainder.degree() >= b.degree())
			{
				auto		degDiff = remainder.degree() - b.degree();
				auto		coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();
				Polynomial	term;
				
				term.m_coeffs.resize(degDiff + 1, 0);
				term.m_coeffs.back() = coefRatio;

				quotient += term;
				remainder -= b * term;

				remainder.trim();
			}
			return {quotient, remainder};
		}

		constexpr SolveResult			solveDegree0() const noexcept
		{
			if (empty() || m_coeffs[0] == 0) return SolveResult{ .state = SolveResult::State::Infinite, .degree = 0 };
			return SolveResult{ .state = SolveResult::State::None, .degree = 0 };
		}
		constexpr SolveResult			solveDegree1() const noexcept
		{
			Real	a = m_coeffs[1];
			Real	b = (m_coeffs.size() > 0) ? m_coeffs[0] : 0;
			Real	res = -b / a;
			return SolveResult{ .state = SolveResult::State::Ok, .degree = 1, .roots = {res} };
		}
		constexpr SolveResult			solveDegree2() const
		{
			Real	a = m_coeffs[2];
			Real	b = size() > 1 ? m_coeffs[1] : 0;
			Real	c = size() > 0 ? m_coeffs[0] : 0;
			Real	delta = b.square() - a * 4 * c;

			SolveResult res{
				.state = SolveResult::State::Ok,
				.degree = 2,
				.delta = delta
			};

			Real	twoA = a * 2;
			if (delta > 0)
			{
				Real	sqrtDelta = maths::sqrt(delta);
				res.roots.push_back((-b - sqrtDelta) / twoA);
				res.roots.push_back((-b + sqrtDelta) / twoA);
			}
			else if (delta == 0)
				res.roots.push_back(-b / twoA);
			else
			{
				Real	re = -b / twoA;
				Real	im = maths::sqrt(-delta) / twoA;
				res.roots.push_back(Complex(re, -im));
				res.roots.push_back(Complex(re, im));
			}
			return res;
		}

	public:
		constexpr Polynomial() = default;
		constexpr Polynomial( std::initializer_list<Real> list ) : m_coeffs(list) { trim(); }
		constexpr Polynomial( Real x ) { if (x != 0) m_coeffs.push_back(x); }

		constexpr Polynomial( const Polynomial& ) = default;
		constexpr Polynomial&	operator=( const Polynomial& ) = default;
		constexpr Polynomial( Polynomial&& ) noexcept = default;
		constexpr Polynomial&	operator=( Polynomial&& ) noexcept = default;

		constexpr bool						empty() const noexcept { return m_coeffs.empty(); }
		constexpr std::size_t				size() const noexcept { return m_coeffs.size(); }
		constexpr const std::vector<Real>&	getCoeffs() const noexcept { return m_coeffs; }
		constexpr int						degree() const noexcept { return empty() ? 0 : static_cast<int>(size()) - 1; }
		constexpr void						trim() noexcept
		{
			while (!m_coeffs.empty() && maths::abs(m_coeffs.back().getValue()) < maths::eps)
				m_coeffs.pop_back();
		}
		constexpr std::string_view			getName() const noexcept { return m_variable; }
		constexpr void						setName( std::string_view name ) { m_variable = std::string(name); }

		friend std::ostream&				operator<<( std::ostream&, const Polynomial& );


		template<typename T>
		constexpr T							eval( const T& x ) const
		{
			if (empty()) return (T(0));
			T	result = m_coeffs.back();
			for (std::size_t i = size() - 1; i-- > 0;)
				result = result * x + m_coeffs[i];
			return result;
		}
		template<real_complex K>
		Matrix<K>							eval( const Matrix<K>& ) const;

		constexpr Root						eval( const Root& r ) const
		{
			return std::visit([this]( const auto& val ) -> Root { return eval(val); }, r);
		}

		constexpr SolveResult					solve() const
		{
			switch (degree())
			{
				case 0: return (solveDegree0());
				case 1: return (solveDegree1());
				case 2: return (solveDegree2());
				default:
					throw std::runtime_error("Math Error: Polynomial evaluation for degree > 2 not supported.");
			}
		}

		constexpr Polynomial				pow( int exp ) const
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

		constexpr Polynomial&		operator+=( const Polynomial& other )
		{
			if (size() < other.size()) m_coeffs.resize(other.size(), 0);

			for (std::size_t i = 0; i < other.size(); i++)
				m_coeffs[i] = m_coeffs[i] + other.m_coeffs[i];
			trim();
			return *this;
		}
		friend constexpr Polynomial	operator+( Polynomial a, const Polynomial& b ) { a += b; return a; }
		constexpr Polynomial&		operator+=( Real x )
		{
			if (empty())
			{
				if (x != 0) m_coeffs.push_back(x);
				return *this;
			}
			m_coeffs[0] += x;
			trim();
			return *this;
		}
		friend constexpr Polynomial	operator+( Polynomial p, Real x ) { p += x; return p; }
		friend constexpr Polynomial	operator+( Real x, Polynomial p ) { p += x; return p; }

		constexpr Polynomial&		operator-=( const Polynomial& other )
		{
			if (size() < other.size()) m_coeffs.resize(other.size(), 0);

			for (std::size_t i = 0; i < other.size(); i++)
				m_coeffs[i] = m_coeffs[i] - other.m_coeffs[i];
			trim();
			return *this;
		}
		friend constexpr Polynomial	operator-( Polynomial a, const Polynomial& b ) { a -= b; return a; }
		constexpr Polynomial&		operator-=( Real x )
		{
			if (empty())
			{
				if (x != 0) m_coeffs.push_back(-x);
				return *this;
			}
			m_coeffs[0] -= x;
			trim();
			return *this;
		}
		friend constexpr Polynomial			operator-( Polynomial p, Real x ) { p -= x; return p; }
		friend constexpr Polynomial			operator-( Real x, Polynomial p ) { return -p + x; }
		constexpr Polynomial				operator-() const
		{
			Polynomial	result;
			result.m_coeffs.reserve(size());
			for (auto val : m_coeffs) result.m_coeffs.push_back(-val);
			return result;
		}

		constexpr Polynomial&				operator*=( const Polynomial& other )
		{
			if (empty() || other.empty())
			{
				m_coeffs.clear();
				return *this;
			}
			const std::size_t	newSize = size() + other.size() - 1;
			std::vector<Real>	res(newSize, 0);
			for (std::size_t i = 0; i < size(); ++i)
				for (std::size_t j = 0; j < other.size(); ++j)
					res[i + j] += m_coeffs[i] * other.m_coeffs[j];
			m_coeffs = std::move(res);
			trim();
			return *this;
		}
		friend constexpr Polynomial			operator*( Polynomial a, const Polynomial& b ) { a *= b; return a; }
	
		constexpr Polynomial&				operator*=( Real x )
		{
			if (x == 0)
			{
				m_coeffs.clear();
				return *this;
			}
			for (std::size_t i = 0; i < size(); ++i)
				m_coeffs[i] *= x;
			return *this;
		}
		friend constexpr Polynomial			operator*( Polynomial p, Real x ) { p *= x; return p; }
		friend constexpr Polynomial			operator*( Real x, Polynomial p ) { p *= x; return p; }
		
		constexpr Polynomial				operator/( const Polynomial& other ) const { return euclideanDiv(other).first; }
		constexpr Polynomial&				operator/=( const Polynomial& other ) { return *this = euclideanDiv(other).first; }
		constexpr Polynomial&				operator/=( Real x )
		{
			if (x == 0) throw std::runtime_error("Error: Division by zero.");
			return *this *= x.inv();
		}
		friend constexpr Polynomial			operator/( Polynomial p, Real x ) { p /= x; return p; }
		friend constexpr Polynomial			operator/( Real x, const Polynomial& p )
		{
			if (p.degree() > 0) throw std::runtime_error("Math Error: Division by a polynomial (rational function) is not supported.");
			return Polynomial(x) / p;
		}

		constexpr Polynomial				operator%( const Polynomial& other ) const { return euclideanDiv(other).second; }
		constexpr Polynomial				operator%=( const Polynomial& other ) { return *this = euclideanDiv(other).second; }

		constexpr bool						operator==( const Polynomial& other ) const { return m_coeffs == other.m_coeffs; }
		
};
