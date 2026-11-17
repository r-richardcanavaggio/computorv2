#include "Parser/Visitor.hpp"

using Opr = Token::Operator;

VarType Visitor::operator()( Real a ) const
{
    if (op == Opr::ADD)
        return a;
    if (op == Opr::SUB)
        return -a;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( Complex z ) const
{
    if (op == Opr::ADD)
        return z;
    if (op == Opr::SUB)
        return -z;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( const Polynomial& p ) const
{
    if (op == Opr::ADD)
        return p;
    if (op == Opr::SUB)
        return -p;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( const RationalFunction& f ) const
{
    if (op == Opr::ADD)
        return f;
    if (op == Opr::SUB)
        return -f;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( Real a, Real b ) const
{
    switch (op)
    {
        case Opr::ADD:
            return a + b;
        case Opr::SUB:
            return a - b;
        case Opr::MUL:
            return a * b;
        case Opr::DIV:
            return a / b;
        case Opr::MOD:
            return a % b;
        case Opr::POW:
            return maths::pow(a, static_cast<int>(b));
        case Opr::DOT:
            throw std::runtime_error("Error: Operation not supported between Real and Real types.");
        default:
            throw std::runtime_error("Error: Unknown operator.");
    }
}

VarType Visitor::operator()( Complex z1, Complex z2 ) const
{
    Complex z;

    switch (op)
    {
        case Opr::ADD:
            z = z1 + z2;
            break;
        case Opr::SUB:
            z = z1 - z2;
            break;
        case Opr::MUL:
            z = z1 * z2;
            break;
        case Opr::DIV:
            z = z1 / z2;
            break;
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
        case Opr::ADD:
            return z + b;
        case Opr::SUB:
            return z - b;
        case Opr::MUL:
            return z * b;
        case Opr::DIV:
            return z / b;
        case Opr::POW:
            return maths::pow(z, static_cast<int>(b));
        default:
            throw std::runtime_error("Error: Operation not supported between Complex and Real types.");
    }
}

VarType Visitor::operator()( Real a, Complex z ) const
{
    switch (op)
    {
        case Opr::ADD:
            return a + z;
        case Opr::SUB:
            return a - z;
        case Opr::MUL:
            return a * z;
        case Opr::DIV:
            return a / z;
        default:
            throw std::runtime_error("Error: Operation not supported between Real and Complex types.");
    }
}

VarType Visitor::operator()( const Polynomial& a, const Polynomial& b ) const
{
    switch (op)
    {
        case Opr::ADD:
            return a + b;
        case Opr::SUB:
            return a - b;
        case Opr::MUL:
            return a * b;
        case Opr::DIV:
            return RationalFunction(a, b);
        // case Opr::MOD:
        //     return a % b;
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Polynomial types.");
    }
}

VarType Visitor::operator()( const Polynomial& a, Real scl ) const
{
    switch (op)
    {
        case Opr::ADD:
            return a + scl;
        case Opr::SUB:
            return a - scl;
        case Opr::MUL:
            return a * scl;
        case Opr::DIV:
            return a / scl;
        case Opr::POW:
            return a.pow(static_cast<int>(scl));
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Real types.");
    }
}

VarType Visitor::operator()( Real scl, const Polynomial& a ) const
{
    switch (op)
    {
        case Opr::ADD:
            return scl + a;
        case Opr::SUB:
            return scl - a;
        case Opr::MUL:
            return scl * a;
        case Opr::DIV:
            return RationalFunction(scl, a);
        // case Opr::MOD:
        //     return scl % a;
        default:
            throw std::runtime_error("Error: Operation not supported between Real and Polynomial types.");
    }
}

VarType Visitor::operator()( const RationalFunction& a, const RationalFunction& b ) const
{
    switch (op)
    {
        case Opr::ADD:
            return a + b;
        case Opr::SUB:
            return a - b;
        case Opr::MUL:
            return a * b;
        case Opr::DIV:
            return a / b;
        default:
            throw std::runtime_error("Error: Operation not supported between RationalFunction and RationalFunction types.");
    }
}
