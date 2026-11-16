#include "Interpreter/Interpreter.hpp"
#include "Interpreter/Evaluator.hpp"
#include <regex>

namespace Lexer
{

bool	isVarStart( char c )
{
	return std::isalpha(static_cast<unsigned char>(c));
}

bool	isVarChar( char c )
{
	return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::optional<std::string>	scanVariable( std::string_view src, std::size_t& i )
{
	if (i >= src.size() || !isVarStart(src[i]))
		return std::nullopt;

	std::size_t	start = i;
	++i;
	while (i < src.size() && isVarChar(src[i]))
		++i;
	return std::string(src.substr(start, i - start));
}

bool	isNumberStart( char c )
{
	return std::isdigit(static_cast<unsigned char>(c));
}

bool	isNumberChar( char c )
{
	return std::isdigit(static_cast<unsigned char>(c)) || c == '.';
}

std::optional<std::string>	scanNumber( std::string_view src, std::size_t& i )
{
	if (i >= src.size() || !isNumberStart(src[i]))
		return std::nullopt;
	
	bool		dot = false;
	std::size_t	start = i;
	++i;
	while (i < src.size() && isNumberChar(src[i]) && !(dot && src[i] == '.'))
	{
		if (src[i] == '.')
			dot = true;
        ++i;
	}
	return std::string(src.substr(start, i - start));
}

void	addToken( std::vector<Token>& tokens, Token tok, std::size_t& i )
{
	tokens.push_back(tok);
	++i;
}

std::vector<Token> tokenize( std::string_view input )
{
	std::vector<Token>	tokens;
	std::size_t			i = 0;

	while (i < input.length())
	{
		char c = input[i];

		switch (c)
		{
			case '-': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::SUB}, i); break;
			case '+': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::ADD}, i); break;
			case '/': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::DIV}, i); break;
			case '%': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::MOD}, i); break;
			case '^': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::POW}, i); break;
			case '=': addToken(tokens, {Token::Type::EQUAL}, i); break;
			case '(': addToken(tokens, {Token::Type::BRACKET_OPEN}, i); break;
			case ')': addToken(tokens, {Token::Type::BRACKET_CLOSE}, i); break;
			case '[': addToken(tokens, {Token::Type::BRACKET_MATRIX_OPEN}, i); break;
			case ']': addToken(tokens, {Token::Type::BRACKET_MATRIX_CLOSE}, i); break;
			case ',': addToken(tokens, {Token::Type::COMMA}, i); break;
			case ';': addToken(tokens, {Token::Type::SEMICOLON}, i); break;
			case '?': addToken(tokens, {Token::Type::QUERY}, i); break;
			case '*':
			{
				++i;
				if (i < input.size() && input[i] == '*')
					addToken(tokens, {Token::Type::OPERATOR, Token::Operator::DOT}, i);
				else
					tokens.push_back({Token::Type::OPERATOR, Token::Operator::MUL});
				break;
			}
			default:
			{
				if (isVarStart(c))
				{
					auto	variable = scanVariable(input, i);
					if (variable)
                    {
                        if (*variable == "i")
                            tokens.push_back({Token::Type::IMAGINARY});
                        else
						    tokens.push_back({Token::Type::VARIABLE, Token::Operator::NONE, *variable});
                    }
					break;
				}
				else if (isNumberStart(c))
				{
					auto	number = scanNumber(input, i);
					if (number)
						tokens.push_back({Token::Type::NUMBER, Token::Operator::NONE, *number});
					break;
				}
				else if (std::isspace(c))
					++i;
				else
					addToken(tokens, {Token::Type::UNKOWN, Token::Operator::NONE, std::string(1, c)}, i);
				break;
			}
		}
	}
	return tokens;
}

}

Interpreter::Interpreter() : running(true)
{
	ctx["pi"] = Real(maths::pi);
	ctx["e"] = Real(maths::e);
	ctx["sqrt2"] = Real(maths::sqrt2);
	ctx["i"] = Complex(0, 1);

	commands = {
		{"list",    [this]( const Token& arg ) { commandList(arg); }},
		{"clear",   [this]( const Token& arg ) { commandClear(arg); }},
		{"history", [this]( const Token& arg ) { commandHistory(arg); }},
		{"deg",     [this]( const Token& arg ) { commandDeg(arg); }},
		{"rad",     [this]( const Token& arg ) { commandRad(arg); }},
		{"exit",    [this]( const Token& arg ) { commandExit(arg); }},
		{"quit",    [this]( const Token& arg ) { commandExit(arg); }},
		{"help",    [this]( const Token& arg ) { commandHelp(arg); }}
	};
}

bool	Interpreter::isRunning() const
{
	return running;
}

bool	Interpreter::executeCommand( std::span<const Token> tokens )
{
	if (tokens.empty())
		return false;
	
	auto	it = commands.find(tokens.front().value);

	if (it != commands.end())
	{
		if (tokens.size() > 2)
			throw std::runtime_error("Usage: <Command> (argument).");

		it->second(tokens.back());
		return true;
	}
	return false;
}

struct LexerStreamTest {
    std::string name;
    std::string input;
    std::vector<Token> expected;
};

#include <algorithm> // Pour std::max

// Fonction utilitaire pour afficher précisément où les deux vecteurs divergent
void printTokenDiff(const std::vector<Token>& expected, const std::vector<Token>& actual) {
    size_t max_len = std::max(expected.size(), actual.size());

    std::cout << "  --------------------------------------------------\n";
    for (size_t i = 0; i < max_len; ++i) {
        bool has_exp = i < expected.size();
        bool has_act = i < actual.size();

        // Vérifie si ce token précis est identique des deux côtés
        bool match = (has_exp && has_act && (expected[i] == actual[i]));

        std::cout << "  [" << i << "] " << (match ? "  " : "❌ ");

        std::cout << "Attendu: ";
        if (has_exp) std::cout << expected[i];
        else         std::cout << "<RIEN (Fin)>";

        std::cout << "   |   Reçu: ";
        if (has_act) std::cout << actual[i];
        else         std::cout << "<RIEN (Fin)>";

        std::cout << "\n";
    }
    std::cout << "  --------------------------------------------------\n\n";
}

void runStressTests()
{
    using T = Token::Type;
	using O = Token::Operator;

    std::vector<LexerStreamTest> suite = {
        {
            "Expression compacte sans espaces",
            "x=12.5+y_1*2",
            { {T::VARIABLE, O::NONE, "x"}, {T::EQUAL}, {T::NUMBER, O::NONE, "12.5"}, {T::OPERATOR, O::ADD}, {T::VARIABLE, O::NONE, "y_1"}, {T::OPERATOR, O::MUL}, {T::NUMBER, O::NONE, "2"} }
        },
        {
            "Opérateurs doubles et simples collés (Maximal Munch)",
            "A**B*C",
            { {T::VARIABLE, O::NONE, "A"}, {T::OPERATOR, O::DOT}, {T::VARIABLE, O::NONE, "B"}, {T::OPERATOR, O::MUL}, {T::VARIABLE, O::NONE, "C"} }
        },
        {
            "Matrice condensée",
            "[[1,2];[3,4]]",
            { 
                {T::BRACKET_MATRIX_OPEN}, {T::BRACKET_MATRIX_OPEN}, {T::NUMBER, O::NONE, "1"}, {T::COMMA}, {T::NUMBER, O::NONE, "2"}, {T::BRACKET_MATRIX_CLOSE}, 
                {T::SEMICOLON}, 
                {T::BRACKET_MATRIX_OPEN}, {T::NUMBER, O::NONE, "3"}, {T::COMMA}, {T::NUMBER, O::NONE, "4"}, {T::BRACKET_MATRIX_CLOSE}, {T::BRACKET_MATRIX_CLOSE} 
            }
        },
        {
            "Lookahead en toute fin de chaîne (Anti-Segfault)",
            "x = 42*",
            { {T::VARIABLE, O::NONE, "x"}, {T::EQUAL}, {T::NUMBER, O::NONE, "42"}, {T::OPERATOR, O::MUL} }
        },
        {
            "Chaîne blanche uniquement",
            "   \t   ",
            {} // Ou {{T::EndOfFile}} selon ton choix de design
        }
    };

    for (const auto& test : suite) {
        try {
            std::vector<Token> actual = Lexer::tokenize(test.input);

            if (actual == test.expected) {
                std::cout << "[OK] " << test.name << "\n";
            } else {
                std::cout << "[FAIL] " << test.name << "\n";
                std::cout << "  Entrée : \"" << test.input << "\"\n";
                std::cout << "  Taille : Attendu = " << test.expected.size() 
                          << " vs Reçu = " << actual.size() << "\n";
                
                // Affiche le comparatif détaillé token par token
                printTokenDiff(test.expected, actual);
            }
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << test.name << "\n";
            std::cout << "  Entrée : \"" << test.input << "\"\n";
            std::cout << "  Exception imprévue : " << e.what() << "\n\n";
        }
    }
}

void runAdvancedStressTests() {
    using T = Token::Type;
    using O = Token::Operator;

    // Helpers rapides pour rendre la lecture des tableaux de tests limpide
    auto Num = [](const std::string& val) { return Token(T::NUMBER, O::NONE, val); };
    auto Var = [](const std::string& val) { return Token(T::VARIABLE, O::NONE, val); };
    auto Op  = [](O op)                   { return Token(T::OPERATOR, op); };
    // Si ton IMAGINARY garde "i" en string, ajoute (T::IMAGINARY, O::NONE, "i") ici :
    auto Img = []()                       { return Token(T::IMAGINARY); }; 

    std::vector<LexerStreamTest> valid_suite = {
        // --- 1. Le piège du 'i' (IMAGINARY vs VARIABLE) ---
        {
            "Imaginaire pur vs Variables contenant 'i'",
            "i + i_1 + in + sin(i) - 2i",
            {
                Img(), Op(O::ADD), Var("i_1"), Op(O::ADD), Var("in"), Op(O::ADD),
                Var("sin"), Token(T::BRACKET_OPEN), Img(), Token(T::BRACKET_CLOSE),
                Op(O::SUB), Num("2"), Img()
            }
        },
        {
            "Imaginaire collé à une variable (Doit être une seule variable !)",
            "xi + ix + i",
            { Var("xi"), Op(O::ADD), Var("ix"), Op(O::ADD), Img() }
        },

        // --- 2. Conflits d'opérateurs : MUL (*) vs DOT (**) vs POW (^) ---
        {
            "Combinaisons de * et ** (Maximal Munch)",
            "A**B*C**D",
            { Var("A"), Op(O::DOT), Var("B"), Op(O::MUL), Var("C"), Op(O::DOT), Var("D") }
        },
        {
            "Triple étoile (Doit découper en DOT puis MUL)",
            "A***B",
            { Var("A"), Op(O::DOT), Op(O::MUL), Var("B") }
        },
        {
            "Tous les opérateurs mathématiques enchaînés",
            "a+b-c*d/e%f^2**M",
            {
                Var("a"), Op(O::ADD), Var("b"), Op(O::SUB), Var("c"), Op(O::MUL),
                Var("d"), Op(O::DIV), Var("e"), Op(O::MOD), Var("f"), Op(O::POW),
                Num("2"), Op(O::DOT), Var("M")
            }
        },

        // --- 3. Matrices et Expressions imbriquées dans les matrices ---
        {
            "Matrice 2x3 avec expressions internes et espaces irréguliers",
            "matA = [[ 1+2 , -3.5 , 4i ] ; [ x_1 , 0 , (2^3) ]]",
            {
                Var("matA"), Token(T::EQUAL),
                Token(T::BRACKET_MATRIX_OPEN),
                    Token(T::BRACKET_MATRIX_OPEN),
                        Num("1"), Op(O::ADD), Num("2"), Token(T::COMMA),
                        Op(O::SUB), Num("3.5"), Token(T::COMMA),
                        Num("4"), Img(),
                    Token(T::BRACKET_MATRIX_CLOSE),
                    Token(T::SEMICOLON),
                    Token(T::BRACKET_MATRIX_OPEN),
                        Var("x_1"), Token(T::COMMA),
                        Num("0"), Token(T::COMMA),
                        Token(T::BRACKET_OPEN), Num("2"), Op(O::POW), Num("3"), Token(T::BRACKET_CLOSE),
                    Token(T::BRACKET_MATRIX_CLOSE),
                Token(T::BRACKET_MATRIX_CLOSE)
            }
        },

        // --- 4. Résolutions et Requêtes (EQUAL et QUERY '?') ---
        {
            "Résolution de fonction compacte (=?)",
            "f(x)=?",
            {
                Var("f"), Token(T::BRACKET_OPEN), Var("x"), Token(T::BRACKET_CLOSE),
                Token(T::EQUAL), Token(T::QUERY)
            }
        },
        {
            "Équation polynomiale avec Query",
            "x^2 + 3.5*x - 4 = 0 ?",
            {
                Var("x"), Op(O::POW), Num("2"), Op(O::ADD),
                Num("3.5"), Op(O::MUL), Var("x"), Op(O::SUB), Num("4"),
                Token(T::EQUAL), Num("0"), Token(T::QUERY)
            }
        },

        // --- 5. Nombres décimaux aux limites ---
        {
            "Flottants collés à des opérateurs et parenthèses",
            "(0.001+99.99)/0.5",
            {
                Token(T::BRACKET_OPEN), Num("0.001"), Op(O::ADD), Num("99.99"), Token(T::BRACKET_CLOSE),
                Op(O::DIV), Num("0.5")
            }
        },
        {
            "Signes unaires multiples (ex: x - -5)",
            "x--5+-3",
            {
                Var("x"), Op(O::SUB), Op(O::SUB), Num("5"),
                Op(O::ADD), Op(O::SUB), Num("3")
            }
        }
    };

    // --- 6. Tests de rejet / Caractères illégaux (Doivent throw ou sortir UNKOWN) ---
    std::vector<std::string> invalid_suite = {
        "12.34.56",       // Double point dans un nombre
        "12..34",         // Deux points consécutifs
        "var$1",          // Symbole interdit au milieu d'une variable
        "_var",           // Commence par un underscore
        "x = 42 & y",     // Opérateur bitwise non supporté
        "a . b",          // Si '.' seul n'est pas l'opérateur DOT (et que DOT est '**')
        "3.i",            // Point décimal suivi directement de 'i' sans chiffre après le point
        ".",              // Point tout seul
    };

	for (const auto& test : valid_suite) {
        try {
            std::vector<Token> actual = Lexer::tokenize(test.input);

            if (actual == test.expected) {
                std::cout << "[OK] " << test.name << "\n";
            } else {
                std::cout << "[FAIL] " << test.name << "\n";
                std::cout << "  Entrée : \"" << test.input << "\"\n";
                std::cout << "  Taille : Attendu = " << test.expected.size() 
                          << " vs Reçu = " << actual.size() << "\n";
                
                // Affiche le comparatif détaillé token par token
                printTokenDiff(test.expected, actual);
            }
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << test.name << "\n";
            std::cout << "  Entrée : \"" << test.input << "\"\n";
            std::cout << "  Exception imprévue : " << e.what() << "\n\n";
        }
    }

	// for (const auto& test : invalid_suite) {
    //     try {
    //         std::vector<Token> actual = Lexer::tokenize(test.input);

    //         if (actual == test.expected) {
    //             std::cout << "[OK] " << test.name << "\n";
    //         } else {
    //             std::cout << "[FAIL] " << test.name << "\n";
    //             std::cout << "  Entrée : \"" << test.input << "\"\n";
    //             std::cout << "  Taille : Attendu = " << test.expected.size() 
    //                       << " vs Reçu = " << actual.size() << "\n";
                
    //             // Affiche le comparatif détaillé token par token
    //             printTokenDiff(test.expected, actual);
    //         }
    //     } catch (const std::exception& e) {
    //         std::cout << "[FAIL] " << test.name << "\n";
    //         std::cout << "  Entrée : \"" << test.input << "\"\n";
    //         std::cout << "  Exception imprévue : " << e.what() << "\n\n";
    //     }
    // }
    
}

void	Interpreter::processLine( const std::string& line )
{
	if (line == "test")
    {
		runAdvancedStressTests();
        return ;
    }
	try
	{
		std::vector<Token>	tokens = Lexer::tokenize(line);

		if (tokens.empty())
			return ;

		if (executeCommand(tokens))
			return ;

		Evaluator evaluator(ctx, history, std::move(tokens), line);

		evaluator.evaluate();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
