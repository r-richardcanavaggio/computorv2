#pragma once

#include <memory>
#include <type_traits>

#include "RuntimeTypes.hpp"
#include "Parser/Visitor.hpp"
#include "Parser/BuiltinVisitor.hpp"
#include "Parser/FunctionVisitor.hpp"

struct NumberNode : public BaseNode
{
    Real value;

    explicit NumberNode( Real r ) : value(r) {}

    VarType eval( Context& ) const override { return value; }
    void    print( std::ostream& os, const VarCtx& ) const override { os << value; }
};

struct ImaginaryNode : public BaseNode
{
    VarType eval( Context& ) const override { return Complex(0, 1); }
    void    print( std::ostream& os, const VarCtx& ) const override { os << "i"; }
};

struct VariableNode : public BaseNode
{
    std::string name;

    explicit VariableNode(const std::string &s) : name(s) {}

    VarType eval( Context& ctx ) const override
    {
        auto	it = ctx.vars.find(name);

        if (it != ctx.vars.end())
            return it->second;

        return Polynomial::createSymbolicPolynomial(name);
    }
    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        auto    it = vars.find(name);

        if (it != vars.end())
            os << vars.at(name);
        else
            os << name;
        return;    
    }
};

struct UnaryOpNode : public BaseNode
{
    Token::Operator op;
    NodePtr         child;

    UnaryOpNode(Token::Operator o, NodePtr c) : op(o), child(std::move(c)) {}

    int precedence() const override { return 3; }

    VarType eval( Context& ctx ) const override { return std::visit(Visitor{op}, child->eval(ctx)); }
    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        os << op;
        child->printWrapped(os, vars, child->precedence() < precedence());
    }
};

struct BinaryOpNode : public BaseNode
{
    Token::Operator op;
    NodePtr         left, right;

    BinaryOpNode( Token::Operator o, NodePtr l, NodePtr r ) : op(o), left(std::move(l)), right(std::move(r)) {}

    int precedence() const override
    {
        if (op == Token::Operator::ADD || op == Token::Operator::SUB)
            return 1;
        if (op == Token::Operator::MUL || op == Token::Operator::DIV || op == Token::Operator::MOD || op == Token::Operator::DOT)
            return 2;
        if (op == Token::Operator::POW)
            return 4;
        return 100;
    }

    VarType eval( Context& ctx ) const override { return std::visit(Visitor{op}, left->eval(ctx), right->eval(ctx)); }

    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        int     p = precedence();
        bool    nonAssocRight = (op == Token::Operator::SUB || op == Token::Operator::DIV || op == Token::Operator::MOD);
        bool    wrapLeft = left->precedence() < p || (op == Token::Operator::POW && left->precedence() == p);
        bool    wrapRight = right->precedence() < p || (nonAssocRight && right->precedence() == p);

        left->printWrapped(os, vars, wrapLeft);
        os << " " << op << " ";
        right->printWrapped(os, vars, wrapRight);
    }
};

struct FunctionCallNode : public BaseNode
{
    std::string name;
    NodePtr     child;

    FunctionCallNode( const std::string& s, NodePtr c ) : name(s), child(std::move(c)) {}

    VarType eval( Context& ctx ) const override
    {
    	if (!child)
            throw std::runtime_error("Error: Invalid function call.");

        auto	it = ctx.funcs.find(name);
        if (it == ctx.funcs.end())
            throw std::runtime_error("Error: Unknown function.");

        return std::visit(FunctionEvalVisitor{ctx}, it->second, child->eval(ctx));
    }
    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        os << name << "(";
        child->print(os, vars);
        os << ")";
    }
};

struct MatrixNode : public BaseNode
{
    std::size_t             rows = 0;
    std::size_t             cols = 0;
    std::vector<NodePtr>    elements;

    explicit MatrixNode( std::size_t r, std::size_t c, std::vector<NodePtr> elems ) : rows(r), cols(c), elements(std::move(elems)) {}

    VarType eval( Context& ctx ) const override
    {
        if (elements.empty())
            return Matrix<Real>(0, 0);

        bool				 hasComplex = false;
        std::vector<VarType> evaluated;

        evaluated.reserve(elements.size());
        for (const auto& expr : elements)
        {
            VarType	v = expr->eval(ctx);

            if (std::holds_alternative<Complex>(v))
                hasComplex = true;
            else if (!std::holds_alternative<Real>(v))
                throw std::runtime_error("Matrix elements must be Real or Complex.");
            evaluated.push_back(std::move(v));
        }

        if (!hasComplex)
        {
            Matrix<Real>	mat(rows, cols);

            for (std::size_t i = 0; i < evaluated.size(); ++i)
                mat[i] = std::get<Real>(evaluated[i]);
            return mat;
        }

        Matrix<Complex> mat(rows, cols);

        for (std::size_t i = 0; i < evaluated.size(); i++)
        {
            if (std::holds_alternative<Complex>(evaluated[i]))
                mat[i] = std::get<Complex>(evaluated[i]);
            else
                mat[i] = Complex(std::get<Real>(evaluated[i]));
        }
        return mat;
    }
    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        if (elements.empty())
            os << "[]";
        if (rows > 1)
            os << "[";
        for (std::size_t r = 0; r < rows; ++r)
        {
            os << "[";
            for (std::size_t c = 0; c < cols; ++c)
            {
                elements[r * cols + c]->print(os, vars);
                if (c + 1 < cols)
                    os << ", ";
            }
            os << "]";
            if (r + 1 < rows)
                os << "; ";
        }
        if (rows > 1)
            os << "]";
    }
};

struct BuiltinFunctionNode : public BaseNode
{
    std::string name;
    NodePtr     child;

    BuiltinFunctionNode( const std::string& s, NodePtr c ) : name(s), child(std::move(c)) {}

    VarType eval( Context& ctx ) const override { return std::visit(BuiltinVisitor(ctx, name), child->eval(ctx)); }
    void    print( std::ostream& os, const VarCtx& vars ) const override
    {
        os << name << "(";
        child->print(os, vars);
        os << ")";
    }
};
