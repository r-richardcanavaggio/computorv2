#pragma once

#include <memory>
#include "RuntimeTypes.hpp"
#include "Visitors/Visitors.hpp"

struct BaseNode;
using NodePtr = std::unique_ptr<BaseNode>;

struct BaseNode
{
    virtual ~BaseNode() = default;
    virtual VarType	eval( Context& ) const = 0;
};

struct NumberNode : public BaseNode
{
    Real	value;

    explicit NumberNode( Real r ) : value(r) {}
    VarType	eval( Context& ) const override { return value; }
};

struct ImaginaryNode : public BaseNode
{
	VarType	eval( Context& ) const override { return Complex(0, 1); }
};

struct VariableNode: public BaseNode
{
    std::string	name;

    explicit VariableNode( const std::string& s ) : name(s) {}
    VarType	eval( Context& ctx ) const override
    {
        auto    it = ctx.find(name);
        if (it != ctx.end()) return it->second;
        Polynomial sym({0, 1});
        sym.setName(name);
        return sym;
    }
};

struct UnaryOpNode : public BaseNode
{
    OpKind	op;
    NodePtr	child;

    UnaryOpNode( OpKind o, NodePtr c ) : op(o), child(std::move(c)) {}

    VarType	eval( Context& ctx ) const override { return applyUnaryOp(op, child->eval(ctx)); }
};

struct BinaryOpNode : public BaseNode
{
    OpKind	op;
    NodePtr	left, right;

    BinaryOpNode( OpKind o, NodePtr l, NodePtr r ) : op(o), left(std::move(l)), right(std::move(r)) {}
    
    VarType	eval( Context& ctx ) const override { return applyBinaryOp(op, left->eval(ctx), right->eval(ctx)); }
};

struct FunctionCallNode : public BaseNode
{
    std::string	name;
    NodePtr		child;

    FunctionCallNode( const std::string& s, NodePtr c ) : name(s), child(std::move(c)) {}

    VarType	eval( Context& ctx ) const override;
};

struct MatrixNode : public BaseNode
{
    std::size_t             rows = 0;
    std::size_t             cols = 0;
    std::vector<NodePtr>    elements;

    explicit MatrixNode( std::size_t r, std::size_t c, std::vector<NodePtr> elems ) : rows(r), cols(c), elements(std::move(elems)) {}
    
    VarType eval( Context& ) const override;
};

struct BuiltinFunctionNode : public BaseNode
{
    std::string	name;
    NodePtr		child;

    BuiltinFunctionNode( const std::string& s, NodePtr c ) : name(s), child(std::move(c)) {}

    VarType	eval( Context& ) const override;
};
