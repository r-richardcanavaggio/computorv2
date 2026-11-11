#include "Parser/Nodes.hpp"
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <string>

namespace
{

VarType	toRadians( const VarType& x, const Context& ctx )
{
	if (ctx.isDegreeMode)
		return x * maths::pi / 180.0;
	return x;
}

template < real_complex K >
VarType	applyMath( const VarType& arg, auto mathFunc, std::string_view funcName )
{
	auto	computeMath = [&]( K val ) -> VarType
	{
		if constexpr (requires { requires real_complex<K> })
			return mathFunc(val);
		throw std::runtime_error("Math Error: " + std::string(funcName) + "() requires a scalar (Real or Complex).");
	};
	return std::visit(computeMath, arg);
}

VarType builtinCos( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( const VarType& x ) { return maths::cos(toRadians(x, ctx)); }, "cos");
}

template < real_complex K >
VarType	builtinSin( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( K x ) { return maths::sin(toRadians(x, ctx)); }, "sin");
}

template < real_complex K >
VarType builtinTan( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( K x ) { return maths::tan(toRadians(x, ctx)); }, "tan");
}

template < real_complex K >
VarType builtinSqrt( const VarType& v, const Context& )
{
	return applyMath(v, []( K x ) { return maths::sqrt(x); }, "sqrt");
}

template < real_complex K >
VarType builtinAbs( const VarType& v, const Context& )
{
	return applyMath(v, []( K x ) { return maths::abs(x); }, "abs");
}

template < real_complex K >
VarType builtinExp( const VarType& v, const Context& )
{
	return applyMath(v, []( K x ) { return maths::exp(x); }, "exp");
}

template < real_complex K >
VarType	builtinInv( const VarType& v, const Context& )
{
	auto invert = []( const VarType& val ) -> VarType
	{	
		if constexpr (std::same_as<std::remove_cvref_t<decltype(val)>, Matrix<K>>)
		{
			if (auto opt = val.inverse())
				return *opt;
			throw std::runtime_error("Error: Matrix is singular.");
		}
		throw std::runtime_error("Math Error: inv() requires a Matrix.");
	};
	return std::visit(invert, v);
}

template < real_complex K >
VarType builtinNorm( const VarType& v, const Context& )
{
	auto computNorm = []( const VarType& val ) -> VarType
	{
		if constexpr (requires { val.norm(); })
			return val.norm();
		throw std::runtime_error("Math Error: norm() cannor be applied to this type.");
	};
	return std::visit(computNorm, v);
}

template < real_complex K >
using BuiltinHandler = VarType (*)( const VarType&, const Context& );

template < real_complex K >
static const std::unordered_map<std::string_view, BuiltinHandler<K>> builtins = {
	{"cos",  builtinCos},
	{"sin",  builtinSin},
	{"tan",  builtinTan},
	{"sqrt", builtinSqrt},
	{"abs",  builtinAbs},
	{"exp",  builtinExp},
	{"inv",  builtinInv},
	{"norm", builtinNorm}
};

}

VarType	UnaryOpNode::eval( Context& ctx ) const
{
	return std::visit(Visitor(op), child->eval(ctx));
}

VarType	BinaryOpNode::eval( Context& ctx ) const
{
	return std::visit(Visitor(op), left->eval(ctx), right->eval(ctx));
}

VarType FunctionCallNode::eval( Context& ctx ) const
{
	if (!child)
		throw std::runtime_error("Error: Invalid function call.");

	VarType	x = child->eval(ctx);
	auto	it = ctx.find(name);

	if (it == ctx.end())
		throw std::runtime_error("Error: Unknown function.");

	const auto   p = std::get_if<Polynomial>(&it->second);
	if (!p)
		throw std::runtime_error("Error: Not a function.");

	if (const auto v = std::get_if<Real>(&x)) return p->eval(*v);	
	if (const auto v = std::get_if<Complex>(&x)) return p->eval(*v);
	if (const auto v = std::get_if<Polynomial>(&x)) return p->eval(*v);

	throw std::runtime_error("Error: Invalid function argument.");
}

VarType MatrixNode::eval( Context& ctx ) const
{
	if (elements.empty()) return Matrix<Real>(0, 0);

	bool					hasComplex = false;
	std::vector<VarType>	evaluated;
	evaluated.reserve(elements.size());
	
	for (const auto& expr : elements)
	{
		VarType	v = expr->eval(ctx);
		if (std::holds_alternative<Complex>(v))
			hasComplex = true;
		else if (!std::holds_alternative<Real>(v))
			throw std::runtime_error("Matrix elements must be Real or Complex.");
		evaluated.push_back(std::move(v));
	}

	if (!hasComplex)
	{
		Matrix<Real>	mat(rows, cols);
		for (std::size_t i = 0; i < evaluated.size(); ++i)
			mat[i] = std::get<Real>(evaluated[i]);
		return mat;
	}

	Matrix<Complex>	mat(rows, cols);
	for (std::size_t i = 0; i < evaluated.size(); i++)
	{
		if (std::holds_alternative<Complex>(evaluated[i]))
			mat[i] = std::get<Complex>(evaluated[i]);
		else
			mat[i] = Complex(std::get<Real>(evaluated[i]));
	}
	return mat;
}

VarType	BuiltinFunctionNode::eval( Context& ctx ) const
{
	if (name == "cos") builtinCos(child->eval(ctx), ctx);
	if (name == "sin") bui
	if (it == builtins.end())
		throw std::runtime_error("Unknown builtin function: '" + name + "'");
	return it->second(child->eval(ctx), ctx);
}
