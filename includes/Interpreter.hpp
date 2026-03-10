#pragma once

#include "Parser.hpp"
#include <vector>
#include <string>

struct Target
{
	std::string	name;
	std::string	param;
	bool		isFunction;

	Target( const std::string& n, const std::string& p, bool f ) : name(n), param(p), isFunction(f) {}
};

class Interpreter
{
	private:
		Context&					_ctx;
		std::vector<std::string>	_history;

		int		findEqualIndex( const std::vector<Token>& ) const;
		Target	parseLHS( const std::vector<Token>& ) const;
		void	executeQuery( const Target&, const std::vector<Token>& ) const;
		void	executeAssignment( const Target&, const std::vector<Token>&, const std::string& );
		bool	handleSystemCommand( const std::vector<Token>& ) const;
		
		bool	endsExpression( const Token& ) const;
		bool	startsExpression( const Token& ) const;
		void	prePassArity( std::vector<Token>& ) const;
		void	prePassImplMulti( std::vector<Token>& ) const;

	public:
		Interpreter( Context& );
		~Interpreter();

		void	processLine( std::vector<Token>&, const std::string& );
};
