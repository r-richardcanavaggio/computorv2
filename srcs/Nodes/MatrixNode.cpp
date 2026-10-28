#include "MatrixNode.hpp"

MatrixNode::MatrixNode( std::vector<std::vector<NodePtr>> elems ) : elements(std::move(elems)) {}

NodePtr	MatrixNode::clone() const
{
	std::vector<std::vector<NodePtr>>	cloned;
	cloned.reserve(elements.size());

	for (const auto& row : elements)
	{
		std::vector<NodePtr>	crow;

		crow.reserve(row.size());
		for (const auto& expr : row)
			crow.push_back(expr ? expr->clone() : nullptr);
		cloned.push_back(std::move(crow));
	}
	return (std::make_unique<MatrixNode>(std::move(cloned)));
}

VarType MatrixNode::eval( Context& ctx ) const
{
	if (elements.empty()) return Matrix<Real>(0, 0);

	const std::size_t		rows = elements.size();
	const std::size_t		cols = elements[0].size();
	bool					hasComplex = false;

	std::vector<VarType>	evaluated;
	evaluated.reserve(rows * cols);
	
	for (const auto& row : elements)
	{
		if (row.size() != cols) throw std::runtime_error("Error: inconsistent row size in matrix definition.");

		for (const auto& expr : row)
		{
			VarType	v = expr->eval(ctx);
			if (std::holds_alternative<Complex>(v))
				hasComplex = true;
			else if (!std::holds_alternative<Real>(v))
				throw std::runtime_error("Matrix elements must be Real or Complex.");
			evaluated.push_back(std::move(v));
		}
	}
	if (!hasComplex)
	{
		Matrix<Real>	mat(rows, cols);
		for (std::size_t i = 0; i < evaluated.size(); ++i)
			mat[i] = std::get<Real>(evaluated[i]);
		return mat;
	}
	
	Matrix<Complex>	mat(rows, cols);
	for (std::size_t i = 0; i < evaluated.size(); i++)
	{
		if (std::holds_alternative<Complex>(evaluated[i]))
			mat[i] = std::get<Complex>(evaluated[i]);
		else
			mat[i] = Complex(std::get<Real>(evaluated[i]));
	}
	return mat;
}
