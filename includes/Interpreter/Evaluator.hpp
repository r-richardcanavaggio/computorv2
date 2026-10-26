#pragma once

#include "RuntimeTypes.hpp"
#include <vector>
#include <string>

class Evaluator
{
	private:
		struct Target
		{
			std::string                 name;
			std::optional<std::string>	param;

			bool    isFunction() const { return param.has_value(); }
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
		void						normalize ();

		std::optional<std::size_t>	findEqualIndex() const;
		Target						parseLHS() const;
		void						assignVariable( const std::string& );
		void						assignFunction( const std::string&, const std::string& );
		void						saveResult( const std::string&, const VarType& );

		void						executeExpression() const;
		void						executeQuery() const;
		void						executeAssignment();
	
	public:
		Evaluator( Context&, std::vector<std::string>&, std::vector<Token>, const std::string& );

		void	evaluate();
};
