#pragma once

#include "RuntimeTypes.hpp"

struct Visitor
{
	Token::Operator	op = Token::Operator::NONE;

	// Unary +/-
	VarType operator()( Real a ) const
	{
		if (op == Token::Operator::SUB)
			return -a;
		return a;
	}

	VarType operator()( Complex z ) const
	{
		if (op == Token::Operator::SUB)
			return -z;
		return z;
	}

	VarType operator()( const Polynomial& p ) const
	{
		if (op == Token::Operator::SUB)
			return -p;
		return p;
	}
	VarType operator()( const RationalFunction& f ) const
	{
		if (op == Token::Operator::SUB)
			return -f;
		return f;
	}

	template < typename K >
	VarType operator()( const Matrix<K>& m ) const
	{
		if (op == Token::Operator::SUB)
			return -m;
		return m;
	}

	// Binary + / - / * / / (div)
	VarType operator()( Real a, Real b ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
            	return a + b;
			case Token::Operator::SUB:
				return a - b;
			case Token::Operator::MUL:
				return a * b;
			case Token::Operator::DIV:
				return a / b;
			case Token::Operator::MOD:
				return a % b;
			case Token::Operator::POW:
				return maths::pow(a, static_cast<int>(b));
			case Token::Operator::DOT:
				throw std::runtime_error("Error: Operation not supported between Real and Real types.");
			default:
				throw std::runtime_error("Error: Unknown operator.");
		}
	}

	VarType operator()( Complex z, Real b ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return z + b;
			case Token::Operator::SUB:
				return z - b;
			case Token::Operator::MUL:
				return z * b;
			case Token::Operator::DIV:
				return z / b;
			case Token::Operator::POW:
				return maths::pow(z, static_cast<int>(b));
			default:
				throw std::runtime_error("Error: Operation not supported between Complex and Real types.");
		}
	}

	VarType operator()( Real a, Complex z ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return a + z;
			case Token::Operator::SUB:
				return a - z;
			case Token::Operator::MUL:
				return a * z;
			case Token::Operator::DIV:
				return a / z;
			default:
				throw std::runtime_error("Error: Operation not supported between Real and Complex types.");
		}
	}

	VarType operator()( Complex z1, Complex z2 ) const
	{
		Complex z;

		switch (op)
		{
			case Token::Operator::ADD:
				z = z1 + z2;
				break;
			case Token::Operator::SUB:
				z = z1 - z2;
				break;
			case Token::Operator::MUL:
				z = z1 * z2;
				break;
			case Token::Operator::DIV:
				z = z1 / z2;
				break;
			default:
				throw std::runtime_error("Error: Operation not supported between Complex and Complex types.");
		}
		if (z.isStrictlyReal())
			return Real(z.real);
		return z;
	}

	template < typename K1, typename K2 >
	VarType operator()( const Matrix<K1>& a, const Matrix<K2>& b ) const
	{
		if constexpr (!std::same_as<K1, K2>)
		{
			using R = std::common_type_t<K1, K2>;
			return (*this)(Matrix<R>(a), Matrix<R>(b));
		}
		else
		{
			switch (op)
			{
				case Token::Operator::ADD:
					return a + b;
				case Token::Operator::SUB:
					return a - b;
				case Token::Operator::MUL:
					return a * b;
				case Token::Operator::DOT:
					if (a.getRows() == 1 && b.getRows() == 1 && a.getCols() == b.getCols())
						return a.dot(b.transpose())(0, 0);
					if (a.getCols() != b.getRows() && b.getRows() == 1 && a.getCols() == b.getCols())
						return a.dot(b.transpose()).transpose();
					return a.dot(b);
				default:
					throw std::runtime_error("Error: Operation not supported between Matrix and Matrix types.");
			}
		}
	}

	template < typename K >
	VarType operator()( Real scl, const Matrix<K>& m ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				if (!m.isSquare())
					throw std::runtime_error("Math Error: Matrix addition by a scalar requires a square matrix.");
				return (Matrix<K>::identity(m.getRows()) * scl) + m;
			case Token::Operator::SUB:
				if (!m.isSquare())
					throw std::runtime_error("Math Error: Matrix substraction by a scalar requires a square matrix.");
				return (Matrix<K>::identity(m.getRows()) * scl) - m;
			case Token::Operator::MUL:
				return scl * m;
			default:
				throw std::runtime_error("Error: Operation not supported between Real and Matrix types.");
		}
	}

	template < typename K >
	VarType operator()( const Matrix<K>& m, Real scl ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				if (!m.isSquare())
					throw std::runtime_error("Math Error: Matrix addition by a scalar requires a square matrix.");
				return m + Matrix<K>::identity(m.getRows()) * scl;
			case Token::Operator::SUB:
				if (!m.isSquare())
					throw std::runtime_error("Math Error: Matrix substraction by a scalar requires a square matrix.");
				return m - Matrix<K>::identity(m.getRows()) * scl;
			case Token::Operator::MUL:
				return m * scl;
			case Token::Operator::POW:
				if (scl < 0. || Real(static_cast<int>(scl)) != scl)
					throw std::runtime_error("Math Error: Exponent must be a positive integer.");
				return m.pow(static_cast<int>(scl));
			case Token::Operator::DIV:
				return m / scl;
			default:
				throw std::runtime_error("Error: Operation not supported between Matrix and Real types.");
		}
	}

	template <typename K>
	VarType	operator()( const Matrix<K>& m, Complex z ) const
	{
		if constexpr (!std::same_as<K, Complex>)
			return (*this)(Matrix<Complex>(m), z);
		else
		{
			switch (op)
			{
				case Token::Operator::ADD:
					if (!m.isSquare())
						throw std::runtime_error("Math Error: Matrix addition by a complex number requires a square matrix.");
					return m + (Matrix<K>::identity(m.getRows()) * z);
				case Token::Operator::SUB:
					if (!m.isSquare())
						throw std::runtime_error("Math Error: Matrix substraction by a complex number requires a square matrix.");
					return m - (Matrix<K>::identity(m.getRows()) * z);
				case Token::Operator::MUL:
					return m * z;
				case Token::Operator::DIV:
					return m / z;
				default:
					throw std::runtime_error("Error: Operator not supported between Matrix and Real types.");
			}
		}
	}

	template < typename K >
	VarType operator()( Complex z, const Matrix<K>& m ) const
	{
		if constexpr (!std::same_as<K, Complex>)
			return (*this)(z, Matrix<Complex>(m));
		else
		{
			switch (op)
			{
				case Token::Operator::ADD:
					if (!m.isSquare())
						throw std::runtime_error("Math Error: Matrix addition by a scalar requires a square matrix.");
					return (Matrix<K>::identity(m.getRows()) * z) + m;
				case Token::Operator::SUB:
					if (!m.isSquare())
						throw std::runtime_error("Math Error: Matrix substraction by a scalar requires a square matrix.");
					return (Matrix<K>::identity(m.getRows()) * z) - m;
				case Token::Operator::MUL:
					return z * m;
				default:
					throw std::runtime_error("Error: Operation not supported between Real and Matrix types.");
			}
		}
	}

	VarType operator()( const Polynomial& a, const Polynomial& b ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return a + b;
			case Token::Operator::SUB:
				return a - b;
			case Token::Operator::MUL:
				return a * b;
			case Token::Operator::DIV:
				return RationalFunction(a, b);
			default:
				throw std::runtime_error("Error: Operation not supported between Polynomial and Polynomial types.");
		}
	}

	VarType operator()( const Polynomial& p, Real scl ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return p + scl;
			case Token::Operator::SUB:
				return p - scl;
			case Token::Operator::MUL:
				return p * scl;
			case Token::Operator::DIV:
				return p / scl;
			case Token::Operator::MOD:
				return p % scl;
			case Token::Operator::POW:
				return maths::pow(p, static_cast<int>(scl));
			default:
				throw std::runtime_error("Error: Operation not supported between Polynomial and Real types.");
		}
	}

	VarType operator()( Real scl, const Polynomial& p ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return scl + p;
			case Token::Operator::SUB:
				return scl - p;
			case Token::Operator::MUL:
				return scl * p;
			case Token::Operator::DIV:
				return RationalFunction(scl, p);
			case Token::Operator::MOD:
			    return scl % p;
			default:
				throw std::runtime_error("Error: Operation not supported between Real and Polynomial types.");
		}
	}

	VarType	operator()( const RationalFunction& a, const RationalFunction& b ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return a + b;
			case Token::Operator::SUB:
				return a - b;
			case Token::Operator::MUL:
				return a * b;
			case Token::Operator::DIV:
				return a / b;
			default:
				throw std::runtime_error("Error: Operation not supported between RationalFunction and RationalFunction types.");
		}
	}

	VarType	operator()( const RationalFunction& a, Real scl ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return a + scl;
			case Token::Operator::SUB:
				return a - scl;
			case Token::Operator::MUL:
				return a * scl;
			case Token::Operator::DIV:
				return a / scl;
			case Token::Operator::POW:
				return pow(a, static_cast<int>(scl));
			default:
				throw std::runtime_error("Error: Operation not supported between RationalFunction and Real types.");
		}
	}

	VarType	operator()( Real scl, const RationalFunction& a ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return scl + a;
			case Token::Operator::SUB:
				return scl - a;
			case Token::Operator::MUL:
				return scl * a;
			case Token::Operator::DIV:
				return scl / a;
			default:
				throw std::runtime_error("Error: Operation not supported between RationalFunction and Real types.");
		}
	}

	VarType	operator()( const RationalFunction& lhs, const Polynomial& rhs ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return lhs + rhs;
			case Token::Operator::SUB:
				return lhs - rhs;
			case Token::Operator::MUL:
				return lhs * rhs;
			case Token::Operator::DIV:
				return lhs / rhs;
			default:
				throw std::runtime_error("Error: Operation not supported between RationalFunction and Polynomial types.");
		}
	}

	VarType	operator()( const Polynomial& lhs, const RationalFunction& rhs ) const
	{
		switch (op)
		{
			case Token::Operator::ADD:
				return lhs + rhs;
			case Token::Operator::SUB:
				return lhs - rhs;
			case Token::Operator::MUL:
				return lhs * rhs;
			case Token::Operator::DIV:
				return lhs / rhs;
			default:
				throw std::runtime_error("Error: Operation not supported between Polynomial and RationalFunction types.");
		}
	}

	template < typename T, typename U >
	VarType operator()( const T&, const U& ) const
	{
		throw std::runtime_error("Error: Invalid type operation");
	}
};
