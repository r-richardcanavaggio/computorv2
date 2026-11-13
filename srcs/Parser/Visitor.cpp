#include "Parser/Visitor.hpp"

VarType Visitor::operator()( Real a ) const
{
    if (op == Token::Operator::ADD)
        return a;
    if (op == Token::Operator::SUB)
        return -a;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( Complex z ) const
{
    if (op == Token::Operator::ADD)
        return z;
    if (op == Token::Operator::SUB)
        return -z;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( const Polynomial& p ) const
{
    if (op == Token::Operator::ADD)
        return p;
    if (op == Token::Operator::SUB)
        return -p;
    throw std::runtime_error("Error: Unknown operator.");
}

VarType Visitor::operator()( Real a, Real b ) const
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

VarType Visitor::operator()( Complex z1, Complex z2 ) const
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

VarType Visitor::operator()( Complex z, Real b ) const
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

VarType Visitor::operator()( Real a, Complex z ) const
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

VarType Visitor::operator()( const Polynomial& a, const Polynomial& b ) const
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
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Polynomial types.");
    }
}

VarType Visitor::operator()( const Polynomial& a, Real scl ) const
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
        case Token::Operator::MOD:
            return a % scl;
        case Token::Operator::POW:
            return a.pow(static_cast<int>(scl));
        default:
            throw std::runtime_error("Error: Operation not supported between Polynomial and Real types.");
    }
}

VarType Visitor::operator()( Real scl, const Polynomial& a ) const
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
        case Token::Operator::MOD:
            return scl % a;
        default:
            throw std::runtime_error("Error: Operation not supported between Real and Polynomial types.");
    }
}
