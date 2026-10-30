#include "Nodes.hpp"
#include "TokenStream.hpp"

class Parser
{
	private:
		TokenStream	stream;
		
		NodePtr		parseExpression();
		NodePtr		parseTerm();
		NodePtr		parseFactor();
		NodePtr		parsePower();
		NodePtr		parsePrimary();

		NodePtr		parseNumber();
		NodePtr		parseImaginary();
		NodePtr		parseParenthesized();
		NodePtr		parseVariableOrCall();
		NodePtr		parseMatrix();

	public:
		explicit Parser( std::span<const Token> t ) : stream(t) {}

		NodePtr		parse();
		static bool	isBuiltinFunction( std::string_view );
};
