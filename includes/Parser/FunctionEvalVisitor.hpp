#pragma once

#include "RuntimeTypes.hpp"

struct FunctionEvalVisitor
{
    RationalFunction*   f;

    VarType operator()( Real v ) const { return f->eval(v); }
    VarType operator()( const Polynomial& p ) const { return f->eval(p); }
    VarType operator()( const RationalFunction& other ) const
    {
        return f->eval(other);
    }

    template < typename T >
    VarType operator()( const T& ) const
    {
        throw std::runtime_error("Error: Rational function argument only takes Real/Polynomial types.");
    }
};
