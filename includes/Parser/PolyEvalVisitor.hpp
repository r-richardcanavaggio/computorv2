#pragma once

#include "RuntimeTypes.hpp"

class PolyEvalVisitor
{
private:
    const Polynomial&   p;

public:
    PolyEvalVisitor( const Polynomial& _p ) : p(_p) {}

    VarType operator()( Real v ) const { return p.eval(v); }

    template < real_complex K >
    VarType operator()( const Matrix<K>& m ) const { return p.eval(m); }

    template < typename T >
    VarType operator()( const T& ) const
    {
        throw std::runtime_error("Error: Invalid function argument.");
    }
};
