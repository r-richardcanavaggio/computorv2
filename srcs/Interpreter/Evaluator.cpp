#include "Evaluator.hpp"
#include "Parser.hpp"
#include <sstream>

Evaluator::Evaluator( Context& _ctx, std::vector<std::string>& _history, std::vector<Token> _tokens, const std::string& line ) : ctx(_ctx), history(_history), tokens(_tokens), rawLine(line) {}

void    Evaluator::evaluate()
{
    if (!tokens.empty() && tokens.back().type == TokenType::QUERY)
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
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == TokenType::EQUAL)
            return (i);
    }
    return (std::nullopt);
}

void    Evaluator::executeExpression() const
{
    Parser  parser(tokens);
    VarType result = parser.parse()->eval(ctx);

    std::cout << result << "\n";
}

Evaluator::Target	Evaluator::parseLHS() const
{
    if (lhs.size() == 1 && lhs[0].type == TokenType::VARIABLE)
        return (Target(lhs[0].value, std::nullopt));
    if (lhs.size() == 4 && 
        lhs[0].type == TokenType::VARIABLE 	  &&
        lhs[1].type == TokenType::BRACKET_OPEN &&
        lhs[2].type == TokenType::VARIABLE     &&
        lhs[3].type == TokenType::BRACKET_CLOSE)
    {
        return (Target(lhs[0].value, lhs[2].value));
    }
    throw std::runtime_error("Error: Left side must be a variable ('x') or function declaration ('f(x)').");
}


void    Evaluator::executeQuery() const
{
    if (rhs.empty())
    {
        Parser  parser(lhs);

        std::cout << parser.parse()->eval(ctx) << "\n";
        return ;
    }

    Evaluator::Target   target = parseLHS();
    if (!target.isFunction()) throw std::runtime_error("Equation resolution expects a function on the left side (e.g. 'f(x) = 0 ?').");

    auto    it = ctx.find(target.name);
    if (it == ctx.end()) throw std::runtime_error("Unknown function: '" + target.name + "'");

    auto	poly = std::get_if<Polynomial>(&it->second);
    if (!poly) throw std::runtime_error("'" + target.name + "' is not a polynomial function.");

    Parser	parser(rhs);
    VarType	rhsResult = parser.parse()->eval(ctx);
    if (!std::holds_alternative<Real>(rhsResult)) throw std::runtime_error("Equation resolution expects a scalar value on the right side.");

    Polynomial	equation = *poly;

    equation -= std::get<Real>(rhsResult);

    auto	solutions = equation.solve(true);
    if (solutions.empty()) std::cout << "No solution\n";
}

void    Evaluator::executeAssignment()
{
    Evaluator::Target   target = parseLHS();

    if (Parser::isBuiltinFunction(target.name)) throw std::runtime_error("Forbidden: '" + target.name + "' is a built-in function and cannot be redefined.");

    if (target.isFunction())
        assignFunction(target.name, *target.param);
    else
        assignVariable(target.name);

}

void    Evaluator::assignVariable( const std::string& name )
{
    for (auto t : rhs)
        std::cout << t.value << std::endl;

    for (auto t : lhs)
        std::cout << t.value << std::endl;
    Parser  parser(rhs);
    VarType result = parser.parse()->eval(ctx);

    saveResult(name, result);
}

void    Evaluator::assignFunction( const std::string& name, const std::string& param )
{
    Context evalCtx = ctx;

    evalCtx[param] = Polynomial({0, 1});

    Parser  parser(rhs);
    VarType result = parser.parse()->eval(evalCtx);

    if (std::holds_alternative<Real>(result)) result = Polynomial({std::get<Real>(result)});
    if (!std::holds_alternative<Polynomial>(result)) throw std::runtime_error("Function must evaluate to a Polynomial.");

    std::get<Polynomial>(result).setVarName(param);

    saveResult(name, result);
}

void    Evaluator::saveResult( const std::string& name, const VarType& result )
{
    ctx[name] = result;

    std::ostringstream oss;
    oss << result;
    history.push_back(rawLine + " -> " + oss.str());

    std::cout << result << "\n";
}
