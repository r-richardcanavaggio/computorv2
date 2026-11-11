#pragma once

#include "RuntimeTypes.hpp"

class BuiltinVisitor
{
private:
    Context&    ctx;
    std::string funcName;

    template < real_complex K >
	K	toRadians( K x ) const
	{
		if (ctx.isDegreeMode)
			return x * Real(maths::pi / 180.0);
		return x;
	}

public:
    BuiltinVisitor( Context& _ctx, const std::string& _funcName ) : ctx(_ctx), funcName(_funcName) {}

    VarType operator()( Real v ) const
    {
        if (funcName == "cos")  return maths::cos(toRadians(v));
        if (funcName == "sin")  return maths::sin(toRadians(v));
        if (funcName == "tan")  return maths::tan(toRadians(v));
        if (funcName == "sqrt") return maths::sqrt(v);
        if (funcName == "abs")  return maths::abs(v);
        if (funcName == "exp")  return maths::exp(v);
        if (funcName == "norm") return maths::norm(v);
        throw std::runtime_error("Error: Unkown math function for Real type.");
    }

    VarType operator()( Complex v ) const
    {
        if (funcName == "cos")  return maths::cos(toRadians(v));
        if (funcName == "sin")  return maths::sin(toRadians(v));
        if (funcName == "tan")  return maths::tan(toRadians(v));
        if (funcName == "sqrt") return maths::sqrt(v);
        if (funcName == "abs")  return maths::abs(v);
        if (funcName == "exp")  return maths::exp(v);
        if (funcName == "norm") return maths::norm(v);
        throw std::runtime_error("Error: Unkown math function for Complex type.");
    }

    template < real_complex K >
    VarType operator()( const Matrix<K>& m ) const
    {
        if (funcName == "inv")
        {
            auto    inv = m.inverse();
    
            if (inv)
                return *inv;
            throw std::runtime_error("Error: Matrix is singular.");
        }
        if (funcName == "norm")
            return m.norm();
        throw std::runtime_error("Error: Unkown math function for Matrix type.");
    }

    VarType operator()( const Polynomial& ) const
    {
        throw std::runtime_error("Error: Unkown math function for Polynomial type.");
    }

    template < typename T >
	VarType operator()( const T& ) const
	{
		throw std::runtime_error("Error: Invalid math function.");
	}
};
