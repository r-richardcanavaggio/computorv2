#pragma once

#include "RuntimeTypes.hpp"

class FunctionEvalVisitor
{
private:
    const RationalFunction& f;

public:
    FunctionEvalVisitor( const RationalFunction& _f ) : f(_f) {}

    VarType operator()( Real v ) const { return f.eval(v); }
    VarType operator()( const Polynomial& p ) const { return f.eval(p); }

    template < typename T >
    VarType operator()( const T& ) const
    {
        throw std::runtime_error("Error: Rational function argument only takes Real/Polynomial types.");
    }
};