#pragma once

#include "Types/Token.hpp"
#include <span>
#include <stdexcept>
#include <optional>

class TokenStream
{
private:
    std::span<const Token> tokens;
    std::size_t pos;

public:
    explicit TokenStream( std::span<const Token> );

    bool                            isAtEnd() const;
    const Token&                    peek() const;
    const Token&                    advance();
    bool                            check( Token::Type ) const;
    bool                            match( Token::Type );
    bool                            matchOp( Token::Operator );
    std::optional<Token::Operator>  matchAnyOp( std::initializer_list<Token::Operator> );
    const Token&                    consume( Token::Type );
};
