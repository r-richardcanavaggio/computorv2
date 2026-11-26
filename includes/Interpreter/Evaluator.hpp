#pragma once

#include <sstream>

#include "RuntimeTypes.hpp"
#include <vector>
#include <string>

class Evaluator
{
private:
	struct Target
	{
		std::string	name;
		std::string	param;

		bool    isFunction() const { return !param.empty(); }
		Target( const std::string& _name, const std::string& _param = "" ) : name(_name), param(_param) {}
	};

	Context&					ctx;
	std::vector<std::string>&	history;
	std::vector<Token>			tokens;
	std::string					rawLine;
	std::vector<Token>			lhs;
	std::vector<Token>			rhs;

	bool						isQuery;
	bool						hasEqual;

	void						insertImplicitMultiplications();
	void						resolveOperatorArity();
	bool						isEndExpression( const Token& );
	bool						isStartExpression( const Token& );
	void						normalize();

	std::optional<std::size_t>	findEqualIndex() const;
	Target						parseLHS() const;
	void						checkUserFunc( const std::string&, const std::string& ) const;
	void						assignVariable( const std::string& );
	void						assignFunction( const std::string&, const std::string& );

	template < typename MapValue, typename Value >
	void						saveResult( std::map<std::string, MapValue>& target, const std::string& name, const Value& result )
	{
		target[name] = result;

		std::ostringstream oss;
		oss << result;
		history.push_back(rawLine + " -> " + oss.str());

		std::cout << result << "\n";
	}

	void						executeExpression() const;
	void						executeQuery() const;
	void						executeAssignment();
	
public:
	Evaluator( Context&, std::vector<std::string>&, std::vector<Token>, const std::string& );

	void	evaluate();
};
