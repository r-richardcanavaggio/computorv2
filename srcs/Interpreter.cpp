
#include "Interpreter.hpp"
#include <type_traits>
#include <sstream>

Interpreter::Interpreter( Context& ctx ) : _ctx(ctx) {}

Interpreter::~Interpreter() {}

int		Interpreter::findEqualIndex( const std::vector<Token>& tokens ) const
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].type == TokenType::EQUAL)
			return (static_cast<int>(i));
	}
	return (-1);
}

Target	Interpreter::parseLHS( const std::vector<Token>& tokens ) const
{
	if (tokens.size() == 1 && tokens[0].type == TokenType::VARIABLE)
		return (Target(tokens[0].value, "", false));
	if (tokens.size() == 4 && 
		tokens[0].type == TokenType::VARIABLE 	  &&
		tokens[1].type == TokenType::BRACKET_OPEN &&
		tokens[2].type == TokenType::VARIABLE     &&
		tokens[3].type == TokenType::BRACKET_CLOSE)
	{
		return (Target(tokens[0].value, tokens[2].value, true));
	}
	throw std::runtime_error("Syntax Error: Left side must be a variable or function declaration.");
}

void	Interpreter::executeQuery( const Target& target, const std::vector<Token>& rhsTokens ) const
{
	auto it = _ctx.find(target.name);
	if (it == _ctx.end())
		throw std::runtime_error("Unknown variable or function: " + target.name);

	if (!rhsTokens.empty())
	{
		if (!target.isFunction)
			throw std::runtime_error("Cannot evaluate an expression on a simple variable. Did you mean '==' ?");

		const Polynomial*	poly = std::get_if<Polynomial>(&it->second);
		if (!poly)
			throw std::runtime_error(target.name + " is not a polynomial function.");

		Parser	parser(rhsTokens);
		VarType	rhsResult = parser.parse()->eval(_ctx);
		
		if (std::holds_alternative<Real>(rhsResult))
		{
			Real		val = std::get<Real>(rhsResult);
			Polynomial	equation = *poly;

			equation -= val;
			std::vector<Root>	solutions = equation.solve(true);

			if (solutions.empty())
				std::cout << "No solution" << std::endl;
		}
		else
			throw std::runtime_error("Equation resolution expects a scalar value on the right side.");
	}
	else 
	{
		if (target.isFunction)
		{
			auto	itParam = _ctx.find(target.param);
			if (itParam == _ctx.end())
				throw std::runtime_error("Parameter '" + target.param + "' is not defined in context.");

			const Polynomial*	poly = std::get_if<Polynomial>(&it->second);

			if (!poly)
				throw std::runtime_error(target.name + " is not a polynomial function.");
			std::cout << poly->eval(itParam->second) << std::endl;
		}
		else
			std::cout << it->second << std::endl;
	}
}

void	Interpreter::executeAssignment( const Target& target, const std::vector<Token>& rhsTokens, const std::string& rawLine )
{
	if (Parser::isBuiltinFunction(target.name))
		throw std::runtime_error("Forbidden: '" + target.name + "' is a built-in function and cannot be redefined.");
	Context evalCtx = _ctx;

	if (target.isFunction)
		evalCtx[target.param] = Polynomial({0, 1});

	Parser	parser(rhsTokens);
	VarType	result = parser.parse()->eval(evalCtx);

	if (target.isFunction)
	{
		if (!std::holds_alternative<Polynomial>(result))
			throw std::runtime_error("Function must evaluate to a Polynomial");
		std::get<Polynomial>(result).setVarName(target.param);
	}

	_ctx[target.name] = result;

	std::ostringstream	oss;
	oss << result;

	std::string			resultStr = oss.str();
	_history.push_back(rawLine + "  ->  " + resultStr);
	std::cout << result << std::endl;
}

bool	Interpreter::handleSystemCommand( const std::vector<Token>& tokens ) const
{
	if (tokens.empty())	return (false);

	std::string	cmd = tokens[0].value;

	if (tokens.size() == 1 && cmd == "list")
	{
		if (_ctx.empty())
			std::cout << "No Variables.\n";
		else
		{
			for (const auto& pair : _ctx)
				std::cout << pair.first << " : " << pair.second << std::endl;
		}
		return (true);
	}
	if (tokens.size() == 1 && cmd == "history")
	{
		if (_history.empty())
			std::cout << "History is empty." << std::endl;
		else
		{
			for (std::size_t i = 0; i < _history.size(); i++)
				std::cout << i + 1 << ": " << _history[i] << std::endl;
		}
		return (true);
	}
	if (cmd == "clear")
	{
		std::string	varToDelete = tokens[1].value;
	
		auto		it = _ctx.find(varToDelete);

		if (it != _ctx.end())
		{
			_ctx.erase(it);
			std::cout << "Variable/Function '" << varToDelete << "' has been deleted.\n";
		}
		else
			std::cout << "Error: '" << varToDelete << "' is not defined.\n";
		return (true);
	}
	return (false);
}

void	Interpreter::processLine( std::vector<Token>& tokens, const std::string& rawLine )
{
	if (tokens.empty())
		return;

	bool isQuery = false;
	if (tokens.back().type == TokenType::QUERY)
	{
		isQuery = true;
		tokens.pop_back();
		if (tokens.empty())
			return;
	}

	if (handleSystemCommand(tokens))
	{
		_history.push_back(rawLine);
		return ;
	}

	prePassArity(tokens);
	prePassImplMulti(tokens);

	int eq = findEqualIndex(tokens);

	if (eq == -1)
	{
		Parser parser(tokens);
		VarType result = parser.parse()->eval(_ctx);
		std::cout << result << std::endl;
		return;
	}

	std::vector<Token>	lhsTokens(tokens.begin(), tokens.begin() + eq);
	std::vector<Token>	rhsTokens(tokens.begin() + eq + 1, tokens.end());

	if (isQuery)
	{
		if (rhsTokens.empty())
		{
			Parser	parser(lhsTokens);
			std::cout << parser.parse()->eval(_ctx) << std::endl;
		}
		else
		{
			Target	target = parseLHS(lhsTokens);
			executeQuery(target, rhsTokens);
		}
	}
	else
	{
		Target	target = parseLHS(lhsTokens);
		executeAssignment(target, rhsTokens, rawLine);
	}
}

void	Interpreter::prePassArity( std::vector<Token>& tokens ) const
{
	bool	expects_operands = true;

	for (auto& token : tokens)
	{
		if (token.type == TokenType::VARIABLE || token.type == TokenType::NUMBER || token.type == TokenType::IMAGINARY)
			expects_operands = false;
		else if (token.type == TokenType::BRACKET_OPEN)
			expects_operands = true;
		else if (token.type == TokenType::BRACKET_CLOSE)
			expects_operands = false;
		else if (token.type == TokenType::OPERATOR)
		{
			token.arity = expects_operands ? Arity::UNARY : Arity::BINARY;
			expects_operands = true;
		}
	}
}

bool	Interpreter::endsExpression( const Token& token ) const
{
	return (token.type == TokenType::NUMBER    ||
			token.type == TokenType::VARIABLE  || 
			token.type == TokenType::IMAGINARY || 
			token.type == TokenType::BRACKET_CLOSE);
}

bool	Interpreter::startsExpression( const Token& token ) const
{
	return (token.type == TokenType::NUMBER    ||
			token.type == TokenType::VARIABLE  ||
			token.type == TokenType::IMAGINARY ||
			token.type == TokenType::BRACKET_OPEN);
}

void	Interpreter::prePassImplMulti( std::vector<Token>& tokens ) const
{
	size_t	i = 0;

	while (i + 1 < tokens.size())
	{
		if (endsExpression(tokens[i])              &&
			startsExpression(tokens[i + 1])        &&
			!(tokens[i].type == TokenType::VARIABLE &&
			tokens[i + 1].type == TokenType::BRACKET_OPEN))
		{
			tokens.insert(tokens.begin() + (i + 1), Token("*", TokenType::OPERATOR, Arity::BINARY, OpKind::MUL));
			i++;
		}
		else
			i++;
	}
}
