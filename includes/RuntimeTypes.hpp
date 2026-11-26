#pragma once

#include <map>
#include <string>
#include <variant>
#include <memory>

#include "Types/Complex.hpp"
#include "Types/Matrix.hpp"
#include "Types/Real.hpp"
#include "Types/Token.hpp"
#include "Types/Polynomial.hpp"
#include "Types/RationalFunction.hpp"

using VarType = std::variant<Real, Complex, Polynomial, RationalFunction, Matrix<Real>, Matrix<Complex>>;
using VarCtx = std::map<std::string, VarType>;

struct BaseNode;

struct UserFunction
{
	std::string						param;
	VarCtx							capturedVars;
	std::shared_ptr<const BaseNode>	body;

	void	type() const noexcept
	{
		std::cout << "UserFunction";
	}
};

using FuncType = std::variant<Polynomial, RationalFunction, UserFunction>;

struct Context
{
	VarCtx							vars;
	std::map<std::string, FuncType>	funcs;
	bool							isDegreeMode = false;
	int								callDepth = 0;
};

struct BaseNode
{
    virtual ~BaseNode() = default;
    virtual VarType eval( Context& ) const = 0;
    virtual void    print( std::ostream& os, const VarCtx& vars ) const = 0;
	virtual int		precedence() const { return 100; }
	void			printWrapped( std::ostream& os, const VarCtx& vars, bool wrap ) const
	{
		if (wrap)
			os << "(";
		print(os, vars);
		if (wrap)
			os << ")";
	}
};

using NodePtr = std::unique_ptr<BaseNode>;

inline std::ostream&	operator<<( std::ostream& os, const VarType& v )
{
	std::visit([&os]( const auto& value ) { os << value; }, v);
	return (os);
}

inline std::ostream&	operator<<( std::ostream& os, const UserFunction& func )
{
	func.body->print(os, func.capturedVars);
	return os;
}

inline std::ostream&	operator<<( std::ostream& os, const FuncType& f )
{
	std::visit([&]( const auto& value ) { os << value; }, f);
	return os;
}
