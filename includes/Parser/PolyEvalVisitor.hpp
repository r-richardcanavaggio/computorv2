#pragma once

#include "RuntimeTypes.hpp"

class PolyEvalVisitor
{
private:
    const Polynomial&   p;

public:
    PolyEvalVisitor( const Polynomial& _p ) : p(_p) {}

    VarType operator()( Real v ) const { return p.eval(v); }
    VarType operator()( Complex z ) const { return p.eval(z); }

    template < real_complex K >
    VarType operator()( const Matrix<K>& m ) const { return p.eval(m); }

    VarType operator()( const Polynomial& other ) const { return p.eval(other); }

    template < typename T >
    VarType operator()( const T& ) const
    {
        throw std::runtime_error("Error: Invalid function argument.");
    }
};
