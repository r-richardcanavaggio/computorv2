#include <sstream>

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
	Parser parser(tokens);
	VarType result = parser.parse()->eval(ctx);

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
		Parser parser(lhs);

		std::cout << parser.parse()->eval(ctx) << "\n";
		return;
	}

	Evaluator::Target target = parseLHS();
	if (!target.isFunction())
		throw std::runtime_error("Error: Equation resolution expects a function on the left side (e.g. 'f(x) = 0 ?').");

	auto    it = ctx.find(target.name);
	if (it == ctx.end())
		throw std::runtime_error("Error: Unknown function: '" + target.name + "'");

	Parser	parser(rhs);
	VarType	rhsResult = parser.parse()->eval(ctx);

	VarType	equation = std::visit(Visitor{Token::Operator::SUB}, it->second, rhsResult);

	std::cout << std::visit(FunctionSolveVisitor{}, equation);
}

void Evaluator::saveResult( const std::string& name, const VarType& result )
{
	ctx[name] = result;

	std::ostringstream oss;
	oss << result;
	history.push_back(rawLine + " -> " + oss.str());

	std::cout << result << "\n";
}

void Evaluator::assignVariable(const std::string &name)
{
	Parser parser(rhs);
	VarType result = parser.parse()->eval(ctx);

	saveResult(name, result);
}

void Evaluator::assignFunction(const std::string &name, const std::string &param)
{
	Context evalCtx = ctx;

	evalCtx[name] = Polynomial({0, 1});

	Parser parser(rhs);
	VarType result = parser.parse()->eval(evalCtx);

	if (const auto value = std::get_if<Real>(&result))
		result = Polynomial(*value);

	if (const auto value = std::get_if<Polynomial>(&result))
	{
		value->setName(param);
		saveResult(name, *value);
	}
	else if (const auto value = std::get_if<RationalFunction>(&result))
		saveResult(name, *value);
	else
		throw std::runtime_error("Error: Function must evaluate to a Polynomial or Rational Function.");
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
