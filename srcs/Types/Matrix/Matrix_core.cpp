#include "Matrix.hpp"
#include "Maths.hpp"

template<real_complex K>
Matrix<K>::Matrix( std::size_t rows, std::size_t cols ) 
    : m_elements(rows * cols, K(0)), m_rows(rows), m_cols(cols) {}

template<real_complex K>
Matrix<K>::Matrix( std::size_t n ) : Matrix(n, n) {}

template<real_complex K>
Matrix<K>::Matrix( const std::vector<std::vector<K>>& values )
    : m_rows(values.size()), m_cols(values.empty() ? 0 : values[0].size())
{
	if (values.empty())
	{
		m_rows = 0;
		m_cols = 0;
		m_elements.clear();
		return ;
	}

	m_elements.reserve(m_rows * m_cols);

	for (const auto& row : values)
	{
		if (row.size() != m_cols) throw std::runtime_error("Error: inconsistent row size");
        m_elements.insert(m_elements.end(), row.begin(), row.end());
	}
}

template<real_complex K>
std::size_t	Matrix<K>::getSize() const
{
	return (this->m_cols * this->m_rows);
}

template<real_complex K>
std::size_t	Matrix<K>::getRows() const
{
	return (this->m_rows);
}

template<real_complex K>
std::size_t	Matrix<K>::getCols() const
{
	return (this->m_cols);
}

template<real_complex K>
bool	Matrix<K>::empty() const
{
	return (m_elements.empty());
}

template<real_complex K>
Matrix<K>	Matrix<K>::identity() const
{
	Matrix<K>	id(getRows(), getCols());
	
	for (size_t r = 0; r < getRows(); r++)
		id(r, r) = K(1);
	return (id);
}

template <real_complex K>
Real	Matrix<K>::norm() const
{
	Real	sum = 0.0;

	for (std::size_t r = 0; r < getRows(); r++)
	{
		for (std::size_t c = 0; c < getCols(); c++)
		{
			Real val = maths::abs(at(r, c));
			sum += val * val;
		}
	}
	return (maths::sqrt(sum));
}

template class Matrix<Real>;
template class Matrix<Complex>;
