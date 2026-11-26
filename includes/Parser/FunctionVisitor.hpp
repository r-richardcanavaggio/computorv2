#pragma once

#include "RuntimeTypes.hpp"

using SolveResult = std::variant<Polynomial::Result, RationalFunction::Result>;

inline std::ostream& operator<<( std::ostream& os, const SolveResult& res )
{
    std::visit([&os]< typename T >( const T& r ) { os << r; }, res);
    return os;
}

struct FunctionEvalVisitor
{
    Context&    ctx;

    VarType operator()( const Polynomial& p, Real v ) const { return p.eval(v); }
    VarType operator()( const Polynomial& p, const Polynomial& other ) const { return p.eval(other); }
    VarType operator()( const Polynomial& p, const RationalFunction& f ) const { return compose(p, f); }

    template < typename K >
    VarType operator()( const Polynomial& p, const Matrix<K>& m ) const { return p.eval(m); }

    VarType operator()( const RationalFunction& f, Real v ) const { return f.eval(v); }
    VarType operator()( const RationalFunction& f, const Polynomial& p ) const { return f.eval(p); }
    VarType operator()( const RationalFunction& f, const RationalFunction& other ) const { return f.eval(other); }

    template < typename K >
    VarType operator()( const RationalFunction& f, const Matrix<K>& m ) const { return f.eval(m); }

    template < typename Arg >
    VarType operator()( const UserFunction& f, const Arg& arg ) const
    {
        if (ctx.callDepth > 64)
            throw std::runtime_error("Error: Maximum recursion depth exceeded.");

        Context callCtx{ f.capturedVars, ctx.funcs, ctx.isDegreeMode, ctx.callDepth + 1 };
        callCtx.vars[f.param] = arg;
        return f.body->eval(callCtx);
    }

    template < typename Arg >
    VarType operator()( const Polynomial&, const Arg& ) const
    {
        throw std::runtime_error("Error: Invalid argument type for Polynomial.");
    }
    template < typename Arg >
    VarType operator()( const RationalFunction&, const Arg& ) const
    {
        throw std::runtime_error("Error: Invalid argument type for RationalFunction.");
    }
    template < typename Fn, typename Arg >
    VarType operator()( const Fn&, const Arg& ) const
    {
        throw std::runtime_error("Error: Not a function.");
    }
};

struct  FunctionSolveVisitor
{
    SolveResult operator()( const Polynomial& p ) const { return p.solve(); }
    SolveResult operator()( const RationalFunction& f ) const { return f.solve(); }

    template < typename T >
    SolveResult operator()( const T& ) const
    {
        throw std::runtime_error("Error: Equation cannot be solved.");
    }
};

struct SaveVisitor
{
	FuncType	operator()( Real v ) const { return Polynomial(v); }
	FuncType	operator()( const Polynomial& p ) const { return Polynomial(p); }
	FuncType	operator()( const RationalFunction& f ) const { return RationalFunction(f); }

	template < typename T >
	FuncType	operator()( const T& ) const { throw std::runtime_error("Error: wrong type."); }
};
