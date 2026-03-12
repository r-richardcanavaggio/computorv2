
#include "BuiltinFunctionNode.hpp"
#include "Maths.hpp"
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <string>

BuiltinFunctionNode::BuiltinFunctionNode( std::string n, NodePtr a ) : name(std::move(n)), arg(std::move(a)) {}

NodePtr	BuiltinFunctionNode::clone() const
{
	return (std::make_unique<BuiltinFunctionNode>(name, arg ? arg->clone() : nullptr));
}

template <typename Func>
VarType	apply_math( const VarType& arg, Func mathFunc, const std::string& funcName )
{
	return (std::visit([&]( const auto& val ) -> VarType
	{
		using T = std::decay_t<decltype(val)>;

		if constexpr (std::is_same_v<T, Real> || std::is_same_v<T, Complex>)
		{
			return (mathFunc(val));
		}
		else
		{
			throw std::runtime_error("Math Error: Cannot apply '" + funcName + "' to a Matrix or Polynomial.");
		}
	}, arg));
}

VarType	BuiltinFunctionNode::eval( Context& ctx ) const
{
	VarType	argValue = arg->eval(ctx);

	static const std::unordered_map<std::string, std::function<VarType(const VarType&)>> builtin_funcs = {
		{"cos", [](const VarType& v) {
			return apply_math(v, [](const auto& x) { return maths::cos(x); }, "cos");
		}},
		{"sin", [](const VarType& v) {
			return apply_math(v, [](const auto& x) { return maths::sin(x); }, "sin");
		}},
		{"sqrt", [](const VarType& v) {
			return apply_math(v, [](const auto& x) { return maths::sqrt(x); }, "sqrt");
		}},
		{"abs", [](const VarType& v) {
			return apply_math(v, [](const auto& x) { return maths::abs(x); }, "abs");
		}}
	};

	auto	it = builtin_funcs.find(name);
	if (it != builtin_funcs.end())
		return (it->second(argValue));
	throw std::runtime_error("Unknown builtin function: " + name);
}