#pragma once

#include "Types/Token.hpp"
#include <span>
#include <stdexcept>
#include <optional>

class TokenStream
{
    private:
        std::span<const Token>  tokens;
        std::size_t             pos;

    public:
        explicit TokenStream( std::span<const Token> );

        bool                    isAtEnd() const;
        const Token&            peek() const;
        const Token&            advance();
        bool                    check( TokenType ) const;
        bool                    match( TokenType );
        bool                    matchOp( OpKind );
        std::optional<OpKind>   matchAnyOp( std::initializer_list<OpKind> );
        const Token&            consume( TokenType );
};
