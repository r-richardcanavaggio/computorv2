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

struct  FunctionSolveVisitor
{
    Polynomial::Result operator()( const Polynomial& p ) const { return p.solve(); }
    Polynomial::Result operator()( const RationalFunction& f ) const { return f.solve(); }

    template < typename T >
    Polynomial::Result operator()( const T& ) const
    {
        throw std::runtime_error("Error: what are you doing ?");
    }
};
