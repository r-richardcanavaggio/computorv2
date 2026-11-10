#include "Parser/Visitor.hpp"

VarType Visitor::operator()( Real a, Real b ) const
{
	switch (op)
	{
		case OpKind::ADD: return a + b;
		case OpKind::SUB: return a - b;
		case OpKind::MUL: return a * b;
		case OpKind::DIV: return a / b;
        case OpKind::MOD: return a % b;
        case OpKind::POW: return maths::pow(a, static_cast<int>(b));
        case OpKind::DOT:
            throw std::runtime_error("Error: Operation not supported between Real and Real types.");
        default:
			throw std::runtime_error("Error: Unknown operator.");
	}
}

VarType	Visitor::operator()( Complex z1, Complex z2 ) const
{
    Complex z;

	switch (op)
	{
		case OpKind::ADD:
            z = z1 + z2;
            break;
		case OpKind::SUB:
            z = z1 - z2;
            break;
		case OpKind::MUL:
            z = z1 * z2;
            break;
		case OpKind::DIV:
            z = z1 / z2;
            break;
        case OpKind::NONE:
            throw std::runtime_error("Error: Unknown operator.");
		default:
            throw std::runtime_error("Error: Operation not supported between Complex and Complex types.");
	}
    if (z.isStrictlyReal())
        return Real(z.real);
    return z;
}

VarType Visitor::operator()( Complex z, Real b ) const
{
	switch (op)
	{
		case OpKind::ADD: return z + b;
		case OpKind::SUB: return z - b;
		case OpKind::MUL: return z * b;
		case OpKind::DIV: return z / b;
        case OpKind::POW: return maths::pow(z, static_cast<int>(b));
        case OpKind::NONE:
            throw std::runtime_error("Error: Unknown operator.");
        default:
            throw std::runtime_error("Error: Operation not supported between Complex and Real types.");
	}
}

VarType Visitor::operator()( Real a, Complex z ) const
{
	switch (op)
	{
		case OpKind::ADD: return a + z;
		case OpKind::SUB: return a - z;
		case OpKind::MUL: return a * z;
		case OpKind::DIV: return a / z;
		case OpKind::NONE:
			throw std::runtime_error("Error: Unknown operator");
        default:
            throw std::runtime_error("Error: Operation not supported between Real and Complex types.");
	}
}

VarType Visitor::operator()( const Polynomial& a, const Polynomial& b ) const
{
    switch (op)
    {
        case OpKind::ADD: return a + b;
        case OpKind::SUB: return a - b;
        case OpKind::MUL: return a * b;
        case OpKind::DIV: return a / b;
        case OpKind::MOD: return a % b;
        case OpKind::NONE:
            throw std::runtime_error("Error: Unknown operator.");
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Polynomial types.");
    }
}

VarType Visitor::operator()( const Polynomial& a, Real scl ) const
{
    switch (op)
    {
        case OpKind::ADD: return a + scl;
        case OpKind::SUB: return a - scl;
        case OpKind::MUL: return a * scl;
        case OpKind::DIV: return a / scl;
        case OpKind::MOD: return a % scl;
        case OpKind::POW: return a.pow(static_cast<int>(scl));
        case OpKind::NONE:
            throw std::runtime_error("Error: Unknown operator.");
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Real types.");
    }
}

VarType Visitor::operator()( Real scl, const Polynomial& a ) const
{
    switch (op)
    {
        case OpKind::ADD: return scl + a;
        case OpKind::SUB: return scl - a;
        case OpKind::MUL: return scl * a;
        case OpKind::DIV: return scl / a;
        case OpKind::MOD: return scl % a;
        case OpKind::NONE:
            throw std::runtime_error("Error: Unknown operator.");
        default:
            throw std::runtime_error("Error: Operation not supported between Real and Polynomial types.");
    }
}
