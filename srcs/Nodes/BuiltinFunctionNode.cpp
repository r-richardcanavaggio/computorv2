#include "BuiltinFunctionNode.hpp"
#include "Maths/Maths.hpp"
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <string>

namespace
{

template <typename T>
T	toRadians( const T& x, const Context& ctx )
{
	if (ctx.isDegreeMode)
		return x * maths::pi / 180.0;
	return x;
}

VarType	applyMath( const VarType& arg, auto mathFunc, std::string_view funcName )
{
	auto	computeMath = [&]( const auto& val ) -> VarType
	{
		using T = std::decay_t<decltype(val)>;

		if constexpr (std::is_same_v<T, Real> || std::is_same_v<T, Complex>)
			return mathFunc(val);
		throw std::runtime_error("Math Error: " + std::string(funcName) + "() requires a scalar (Real or Complex).");
	};
	return (std::visit(computeMath, arg));
}

VarType builtinCos( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( const auto& x ) { return maths::cos(toRadians(x, ctx)); }, "cos");
}

VarType	builtinSin( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( const auto& x ) { return maths::sin(toRadians(x, ctx)); }, "sin");
}

VarType builtinTan( const VarType& v, const Context& ctx )
{
	return applyMath(v, [&]( const auto& x ) { return maths::tan(toRadians(x, ctx)); }, "tan");
}

VarType builtinSqrt( const VarType& v, const Context& )
{
	return applyMath(v, []( const auto& x ) { return maths::sqrt(x); }, "sqrt");
}

VarType builtinAbs( const VarType& v, const Context& )
{
	return applyMath(v, []( const auto& x ) { return maths::abs(x); }, "abs");
}

VarType builtinExp( const VarType& v, const Context& )
{
	return applyMath(v, []( const auto& x ) { return maths::exp(x); }, "exp");
}

VarType	builtinInv( const VarType& v, const Context& )
{
	auto invert = []( const auto& val ) -> VarType {
		using T = std::decay_t<decltype(val)>;

		if constexpr (std::is_same_v<T, Matrix<Real>> || std::is_same_v<T, Matrix<Complex>>)
		{
			if (auto opt = val.inverse())
				return *opt;
			throw std::runtime_error("Matrix is singular.");
		}
		throw std::runtime_error("Math Error: inv() requires a Matrix.");
	};

	return std::visit(invert, v);
}

VarType builtinNorm( const VarType& v, const Context& )
{
	auto computNorm = []( const auto& val ) -> VarType {
		if constexpr (requires { val.norm(); })
		{
			return val.norm();
		}
		else
			throw std::runtime_error("Math Error: norm() cannor be applied to this type.");
	};
	return std::visit(computNorm, v);
}

using BuiltinHandler = VarType (*)( const VarType&, const Context& );

static const std::unordered_map<std::string_view, BuiltinHandler> builtins = {
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

BuiltinFunctionNode::BuiltinFunctionNode( std::string n, NodePtr a ) : name(std::move(n)), arg(std::move(a)) {}

NodePtr	BuiltinFunctionNode::clone() const
{
	return (std::make_unique<BuiltinFunctionNode>(name, arg ? arg->clone() : nullptr));
}

VarType	BuiltinFunctionNode::eval( Context& ctx ) const
{
	VarType	argValue = arg->eval(ctx);

	auto	it = builtins.find(name);
	if (it == builtins.end())
		throw std::runtime_error("Unknown builtin function: '" + name + "'");

	return (it->second(argValue, ctx));
}
