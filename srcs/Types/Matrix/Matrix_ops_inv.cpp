#include "Matrix.hpp"
#include "Maths.hpp"

template<real_complex K>
void	Matrix<K>::swap_rows( std::size_t row1, std::size_t row2 )
{
	if (row1 == row2)
		return ;

	for (std::size_t c = 0; c < getCols(); c++)
		std::swap(at(row1, c), at(row2, c));
}

template<real_complex K>
void	Matrix<K>::scale_row( std::size_t row, K scale )
{
	for (std::size_t c = 0; c < getCols(); c++)
		at(row, c) *= scale;
}

template<real_complex K>
void	Matrix<K>::add_row_multiple( std::size_t dest, std::size_t source, K k )
{
	for (std::size_t c = 0; c < getCols(); c++)
		at(dest, c) -= at(source, c) * k;
}

template<real_complex K>
std::optional<std::size_t>	Matrix<K>::findPivotRow( std::size_t col, std::size_t start_row )
{
	Real		bestNorm2 = 0.0;
	std::size_t	bestIndex;
	Real		eps = 1e-9;

	for (std::size_t r = start_row; r < getRows(); r++)
	{
		Real n2 = maths::scalar_norm2(at(r, col));
		if (n2 > bestNorm2)
		{
			bestNorm2 = n2;
			bestIndex = r;
		}
	}

	if (bestNorm2 < eps * eps)
		return (std::nullopt);
	return (bestIndex);
}

template<real_complex K>
std::optional<Matrix<K>>	Matrix<K>::inverse() const
{
	Matrix<K>					result = *this;
	Matrix<K>					id = result.identity();
	std::size_t					row = 0, col = 0;
	std::vector<std::size_t>	pivot_col;

	while (row < getRows() && col < result.getCols())
	{
		auto	pivotRow = result.findPivotRow(col, row);

		if (!pivotRow)
			col++;
		else
		{
			result.swap_rows(row, *pivotRow);
			id.swap_rows(row, *pivotRow);

			K	pivot = result(row, col);
			result.scale_row(row, K(1) / pivot);
			id.scale_row(row, K(1) / pivot);

			pivot_col.push_back(col);

			for (std::size_t i = row + 1; i < result.getRows(); ++i)
			{
				K	scale = result(i, col);
				result.add_row_multiple(i, row, scale);
				id.add_row_multiple(i, row, scale);
			}
			col++;
			row++;
		}
	}
	for (std::size_t	r = row; r-- > 0;)
	{
		std::size_t	c = pivot_col[r];
		for (std::size_t i = 0; i < r; i++)
		{
			K factor = result(i, c);
			if (factor == K(0))
				continue;
			result.add_row_multiple(i, r, factor);
			id.add_row_multiple(i, r, factor);
		}
	}
	if (pivot_col.size() != result.getCols())
		return (std::nullopt);
	return (id);
}

template class Matrix<Real>;
template class Matrix<Complex>;
