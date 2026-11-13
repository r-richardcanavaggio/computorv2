#pragma once

#include "RuntimeTypes.hpp"

struct Visitor
{
	Token::Operator op;

	Visitor() = default;
	explicit Visitor( Token::Operator _op ) : op(_op) {}

	VarType operator()( Real ) const;
	VarType operator()( Complex ) const;
	VarType operator()( const Polynomial& ) const;

	template < real_complex K >
	VarType operator()( const Matrix<K>& m ) const
	{
		if (op == Token::Operator::ADD)
			return m;
		if (op == Token::Operator::SUB)
			return -m;
		throw std::runtime_error("Error: Unkown operator.");
	}

	VarType operator()( Real, Real ) const;
	VarType operator()( Complex, Real ) const;
	VarType operator()( Real, Complex ) const;
	VarType operator()( Complex, Complex ) const;

	template < real_complex K >
	VarType operator()( const Matrix<K>& a, const Matrix<K>& b ) const
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
				return a.dot(b);
			default:
				throw std::runtime_error("Error: Operation not supported between Matrix and Matrix types.");
		}
	}
	template < real_complex K >
	VarType operator()( Real scl, const Matrix<K>& m ) const
	{
		switch (op)
		{
			case Token::Operator::MUL:
				return scl * m;
			default:
				throw std::runtime_error("Error: Operation not supported between Real and Matrix types.");
		}
	}
	template < real_complex K >
	VarType operator()( const Matrix<K>& m, Real scl ) const
	{
		switch (op)
		{
			case Token::Operator::MUL:
				return m * scl;
			case Token::Operator::POW:
				if (scl < 0. || Real(static_cast<int>(scl)) != scl)
					throw std::runtime_error("Math Error: Exponent must be a positive integer.");
				return m.pow(static_cast<int>(scl));
			default:
				throw std::runtime_error("Error: Operation not supported between Matrix and Real types.");
		}
	}

	VarType operator()( const Polynomial&, const Polynomial& ) const;
	VarType operator()( const Polynomial&, Real ) const;
	VarType operator()( Real, const Polynomial& ) const;

	template < typename T, typename U >
	VarType operator()( const T&, const U& ) const
	{
		throw std::runtime_error("Error: Invalid type operation");
	}
};
