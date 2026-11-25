#include "Interpreter/Evaluator.hpp"
#include "Parser/Parser.hpp"
#include "Parser/FunctionVisitor.hpp"

Evaluator::Evaluator( Context& _ctx, std::vector<std::string>& _history, std::vector<Token> _tokens, const std::string& line )
	: ctx(_ctx), history(_history), tokens(std::move(_tokens)), rawLine(line), isQuery(false), hasEqual(false) {}

void Evaluator::evaluate()
{
	if (!tokens.empty() && tokens.back().type == Token::Type::QUERY)
	{
		isQuery = true;
		tokens.pop_back();
		if (tokens.empty())
			throw std::runtime_error("Error: missing statement.");
	}

	normalize();

	if (auto equal = findEqualIndex())
	{
		hasEqual = true;
		lhs.assign(tokens.begin(), tokens.begin() + *equal);
		rhs.assign(tokens.begin() + *equal + 1, tokens.end());
	}
	else
		lhs = tokens;

	if (!hasEqual)
		executeExpression();
	else if (isQuery)
		executeQuery();
	else
		executeAssignment();
}

std::optional<std::size_t> Evaluator::findEqualIndex() const
{
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i].type == Token::Type::EQUAL)
			return i;
	}
	return std::nullopt;
}

void Evaluator::executeExpression() const
{
	Parser	parser(tokens);
	VarType	result = parser.parse()->eval(ctx);

	std::cout << result << "\n";
}

Evaluator::Target Evaluator::parseLHS() const
{
	if (lhs.size() == 1 && lhs[0].type == Token::Type::VARIABLE)
		return (Target(lhs[0].value));
	if (lhs.size() == 4 &&
		lhs[0].type == Token::Type::VARIABLE &&
		lhs[1].type == Token::Type::BRACKET_OPEN &&
		lhs[2].type == Token::Type::VARIABLE &&
		lhs[3].type == Token::Type::BRACKET_CLOSE)
	{
		return (Target(lhs[0].value, lhs[2].value));
	}
	throw std::runtime_error("Error: Left side must be a variable ('x') or function declaration ('f(x)').");
}

void Evaluator::executeQuery() const
{
	if (rhs.empty())
	{
		if (lhs.size() == 4 && lhs[0].type == Token::Type::VARIABLE && lhs[1].type == Token::Type::BRACKET_OPEN && lhs[2].type == Token::Type::VARIABLE && lhs[3].type == Token::Type::BRACKET_CLOSE)
		{
			auto	funcIt = ctx.funcs.find(lhs[0].value);
			auto	varIt = ctx.vars.find(lhs[2].value);

			if (funcIt != ctx.funcs.end() && varIt == ctx.vars.end())
			{
				std::cout << funcIt->second << '\n';
				return ;
			}
		}

		Parser parser(lhs);

		std::cout << parser.parse()->eval(ctx) << "\n";
		return;
	}
	Evaluator::Target target = parseLHS();

	if (!target.isFunction())
		throw std::runtime_error("Error: Equation resolution expects a function on the left side (e.g. 'f(x) = 0 ?').");

	auto    it = ctx.funcs.find(target.name);
	if (it == ctx.funcs.end())
		throw std::runtime_error("Error: Unknown function: '" + target.name + "'");

	Parser	parser(rhs);
	VarType	rhsResult = parser.parse()->eval(ctx);

	VarType	equation = std::visit(Visitor{Token::Operator::SUB}, it->second, rhsResult);

	std::cout << std::visit(FunctionSolveVisitor{}, equation);
}

void Evaluator::assignVariable( const std::string& name )
{
	Parser	parser(rhs);
	VarType	result = parser.parse()->eval(ctx);

	saveResult(ctx.vars, name, result);
}

struct SaveVisitor
{
	FuncType	operator()( Real v ) const { return Polynomial(v); }
	FuncType	operator()( const Polynomial& p ) const { return Polynomial(p); }
	FuncType	operator()( const RationalFunction& f ) const { return RationalFunction(f); }

	template < typename T >
	FuncType	operator()( const T& ) const { throw std::runtime_error("Error: wrong type."); }
};

std::ostream&	operator<<( std::ostream& os, const UserFunction& func )
{
	func.body->print(os, func.capturedVars);
	return os;
}

void Evaluator::assignFunction( const std::string& name, const std::string& param )
{
	Parser	parser(rhs);
	auto	ast = parser.parse();
	Context	evalCtx = ctx;

	evalCtx.vars[param] = Polynomial::createSymbolicPolynomial(param);

	try
	{
		VarType	result = ast->eval(evalCtx);

		saveResult(ctx.funcs, name, std::visit(SaveVisitor{}, result));
	}
	catch ( const std::exception& )
	{
		UserFunction	fn{param, std::move(ast), ctx.vars};

		fn.capturedVars.erase(param);
		saveResult(ctx.funcs, name, fn);
	}
}

void Evaluator::executeAssignment()
{
	Evaluator::Target target = parseLHS();

	if (Parser::isBuiltinFunction(target.name))
		throw std::runtime_error("Forbidden: '" + target.name + "' is a built-in function and cannot be redefined.");

	if (target.isFunction())
		assignFunction(target.name, target.param);
	else
		assignVariable(target.name);
}
