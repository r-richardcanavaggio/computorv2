#include <type_traits>

#include "Parser/BuiltinVisitor.hpp"
#include "Parser/FunctionEvalVisitor.hpp"
#include "Parser/Nodes.hpp"
#include "Parser/PolyEvalVisitor.hpp"

VarType	UnaryOpNode::eval( Context& ctx ) const
{
	return std::visit( Visitor{ op }, child->eval( ctx ) );
}

VarType	BinaryOpNode::eval( Context& ctx ) const
{
	return std::visit( Visitor{ op }, left->eval( ctx ), right->eval( ctx ) );
}

VarType	VariableNode::eval( Context& ctx ) const
{
	auto	it = ctx.find( name );

	if (it != ctx.end())
		return it->second;

	return Polynomial::createSymbolicPolynomial( name );
}

VarType	FunctionCallNode::eval( Context& ctx ) const
{
	if (!child)
		throw std::runtime_error( "Error: Invalid function call." );

	auto	it = ctx.find( name );

	if (it == ctx.end())
		throw std::runtime_error( "Error: Unknown function." );

	if (Polynomial* p = std::get_if<Polynomial>( &it->second ))
		return std::visit( PolyEvalVisitor( *p ), child->eval( ctx ) );
	if (RationalFunction* f = std::get_if<RationalFunction>( &it->second ))
		return std::visit( FunctionEvalVisitor{ f }, child->eval( ctx ) );

	throw std::runtime_error( "Error: Not a function." );
}

VarType MatrixNode::eval( Context& ctx ) const
{
	if (elements.empty())
		return Matrix<Real>( 0, 0 );

	bool				 hasComplex = false;
	std::vector<VarType> evaluated;

	evaluated.reserve( elements.size() );
	for (const auto& expr : elements)
	{
		VarType	v = expr->eval( ctx );

		if (std::holds_alternative<Complex>( v ))
			hasComplex = true;
		else if (!std::holds_alternative<Real>( v ))
			throw std::runtime_error( "Matrix elements must be Real or Complex." );
		evaluated.push_back( std::move( v ) );
	}

	if (!hasComplex)
	{
		Matrix<Real>	mat( rows, cols );

		for (std::size_t i = 0; i < evaluated.size(); ++i)
			mat[i] = std::get<Real>( evaluated[i] );
		return mat;
	}

	Matrix<Complex> mat( rows, cols );

	for (std::size_t i = 0; i < evaluated.size(); i++)
	{
		if (std::holds_alternative<Complex>( evaluated[i] ))
			mat[i] = std::get<Complex>( evaluated[i] );
		else
			mat[i] = Complex( std::get<Real>( evaluated[i] ) );
	}
	return mat;
}

VarType BuiltinFunctionNode::eval( Context& ctx ) const
{
	return std::visit( BuiltinVisitor( ctx, name ), child->eval( ctx ) );
}
