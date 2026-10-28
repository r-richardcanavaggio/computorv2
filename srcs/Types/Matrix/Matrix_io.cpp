#include "Matrix.hpp"

template<real_complex K>
K&	Matrix<K>::at( std::size_t row, std::size_t col )
{
	if (row >= m_rows || col >= m_cols) throw std::out_of_range("Matrix::at: index out of range: ");
	return (m_elements[row * m_cols + col]);
}

template<real_complex K>
const K&    Matrix<K>::at( std::size_t row, std::size_t col ) const
{
	if (row >= m_rows || col >= m_cols) throw std::out_of_range("Matrix::at: index out of range: ");
	return (m_elements[row * m_cols + col]);
}

template<real_complex K>
K&	Matrix<K>::operator()( std::size_t row, std::size_t col )
{
	return (at(row, col));
}

template<real_complex K>
const K&	Matrix<K>::operator()( std::size_t row, std::size_t col ) const
{
	return (at(row, col));
}

template<real_complex K>
K&  Matrix<K>::operator[]( std::size_t index )
{
	if (index >= getSize()) throw std::runtime_error("Error: index out of bound.");
	return (m_elements[index]);
}

template<real_complex K>
const K&    Matrix<K>::operator[]( std::size_t index ) const
{
	if (index >= getSize()) throw std::runtime_error("Error: index out of bound.");
	return (m_elements[index]);
}

template<real_complex T>
std::ostream&	operator<<( std::ostream& os, const Matrix<T>& m )
{
    if (m.empty()) return os << "[]";

    os << "[";
	for (std::size_t r = 0; r < m.getRows(); r++)
	{
        os << "[";
        for (std::size_t c = 0; c < m.getCols(); c++)
        {
            os << m(r, c);
            if (c + 1 < m.getCols())
                os << ", ";
        }
        os << "]";
		if (r + 1 < m.getRows())
            os << "; ";
	}
    os << "]";
	return (os);
}

template class Matrix<Real>;
template class Matrix<Complex>;

template std::ostream& operator<<(std::ostream&, const Matrix<Real>&);
template std::ostream& operator<<(std::ostream&, const Matrix<Complex>&);
