#include "Types/Matrix.hpp"
#include "Constants.hpp"

template < real_complex K >
std::optional< std::size_t >    Matrix< K >::findPivotRow( std::size_t col,  std::size_t startRow )
{
    Real        bestNorm2 = 0.;
    std::size_t bestIndex;

    for (std::size_t r = startRow; r < m_rows; ++r)
    {
        Real n2 = maths::norm2(at(r, col));
        if (n2 > bestNorm2)
        {
            bestNorm2 = n2;
            bestIndex = r;
        }
    }
    if (bestNorm2 < maths::eps * maths::eps)
        return std::nullopt;
    return bestIndex;
}

template < real_complex K >
void    Matrix<K>::swapRows( std::size_t r1, std::size_t r2 )
{
    if (r1 == r2)
        return;

    for (std::size_t c = 0; c < m_cols; ++c)
        std::swap(at(r1, c), at(r2, c));
}

template < real_complex K >
void	Matrix<K>::scaleRow( std::size_t row, K x )
{
    for (std::size_t c = 0; c < m_cols; ++c)
        at(row, c) *= x;
}

template < real_complex K >
void    Matrix<K>::addRowMultiple( std::size_t dest, std::size_t src, K factor )
{
    for (std::size_t c = 0; c < m_cols; ++c)
        at(dest, c) -= at(src, c) * factor;
}

template < real_complex K >
Matrix<K>::Matrix( std::size_t rows, std::size_t cols )
    : m_elements(rows * cols, K(0)), m_rows(rows), m_cols(cols) {}

template < real_complex K >
Matrix<K>::Matrix( std::size_t n ) : Matrix(n, n) {}

template < real_complex K >
std::size_t Matrix<K>::getRows() const noexcept
{
    return m_rows;
}

template < real_complex K >
std::size_t Matrix<K>::getCols() const noexcept
{
    return m_cols;
}

template < real_complex K >
K&  Matrix<K>::at(std::size_t row, std::size_t col)
{
    if (row >= m_rows || col >= m_cols)
        throw std::out_of_range("Matrix::at: index out of range.");
    return m_elements[row * m_cols + col];
}

template < real_complex K >
const K&    Matrix<K>::at(std::size_t row, std::size_t col) const
{
    if (row >= m_rows || col >= m_cols)
        throw std::out_of_range("Matrix::at: index out of range.");
    return m_elements[row * m_cols + col];
}

template < real_complex K >
K&  Matrix<K>::operator()( std::size_t row, std::size_t col )
{
    return at(row, col);
}

template < real_complex K >
const K&    Matrix<K>::operator()( std::size_t row, std::size_t col ) const
{
    return at(row, col);
}

template < real_complex K >
K&  Matrix<K>::operator[]( std::size_t index )
{
	if (index >= m_elements.size())
        throw std::out_of_range("Error: index out of bound.");
	return m_elements[index];
}

template < real_complex K >
const K&    Matrix<K>::operator[]( std::size_t index ) const
{
    if (index >= m_elements.size())
        throw std::out_of_range("Error: index out of bound.");
    return m_elements[index];
}

template < real_complex K >
std::optional<Matrix<K>> Matrix<K>::inverse() const
{
    if (m_rows != m_cols)
        throw std::runtime_error("Error: Matrix must be square to be inverted.");
    if (m_rows == 0)
        return std::nullopt;

    Matrix<K>                   result = *this;
    Matrix<K>                   id = result.identity(m_rows);
    std::size_t                 row = 0, col = 0;
	std::vector<std::size_t>    pivotCol;

	while (row < m_rows && col < result.m_cols)
    {
        auto pivotRow = result.findPivotRow(col, row);

	    if (!pivotRow)
	        ++col;
	    else
        {
            result.swapRows(row, *pivotRow);
            id.swapRows(row, *pivotRow);

            K pivot = result(row, col);
            result.scaleRow(row, K(1) / pivot);
            id.scaleRow(row, K(1) / pivot);

            pivotCol.push_back(col);

            for (std::size_t i = row + 1; i < result.getRows(); ++i)
            {
                K scale = result(i, col);
                result.addRowMultiple(i, row, scale);
                id.addRowMultiple(i, row, scale);
            }
            ++col;
            ++row;
        }
	}
	for (std::size_t r = row; r-- > 0;)
    {
        std::size_t c = pivotCol[r];
        for (std::size_t i = 0; i < r; i++)
        {
            K factor = result(i, c);
            if (factor == K(0))
                continue;
            result.addRowMultiple(i, r, factor);
            id.addRowMultiple(i, r, factor);
        }
	}
	if (pivotCol.size() != result.m_cols)
        return (std::nullopt);
	return id;
}

template < real_complex K >
Matrix<K>   Matrix<K>::dot( const Matrix& other ) const
{
    if (m_cols != other.m_rows)
        throw std::runtime_error("Error: wrong dimensions for matrix multiplication");
    Matrix<K>  result(m_rows, other.m_cols);

    for (std::size_t r = 0; r < m_rows; ++r)
    {
        for (std::size_t c = 0; c < other.m_cols; ++c)
        {
            K sum{};
            for (std::size_t k = 0; k < other.m_rows; k++)
                sum += at(r, k) * other(k, c);
            result(r, c) = sum;
        }
    }
    return result;
}

template < real_complex K >
Matrix<K>  Matrix<K>::pow( int p ) const
{
	if (m_rows != m_cols)
	    throw std::runtime_error("Math Error: Power only for square matrices.");
	if (p < 0)
	    throw std::runtime_error("Error: Negative matrix power not supported.");

	Matrix<K>   res = identity(m_rows);
	Matrix<K>   base = *this;

	while (p > 0)
    {
	    if (p % 2 == 1)
            res = res.dot(base);
	    base = base.dot(base);
	    p /= 2;
	}
	return res;
}

template < real_complex K >
Matrix<K>    Matrix<K>::identity( std::size_t n )
{
    Matrix<K>   id(n);
    for (std::size_t i = 0; i < n; ++i)
        id(i, i) = K(1);
    return id;
}

template < real_complex K >
Real    Matrix<K>::norm() const noexcept
{
    Real sum = 0.;
    for (std::size_t i = 0; i < m_elements.size(); ++i)
        sum += maths::square(maths::abs(m_elements[i]));
    return maths::sqrt(sum);
}

template < real_complex K >
Matrix<K>&		Matrix<K>::operator+=( const Matrix<K>& other )
{
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw std::runtime_error("Error: Matrices must have the same dimensions for addition.");
    for (std::size_t i = 0; i < m_elements.size(); ++i)
        m_elements[i] += other.m_elements[i];
    return *this;
}

template < real_complex K >
Matrix<K>&		Matrix<K>::operator-=( const Matrix& other )
{
    if (m_rows != other.m_rows || m_cols != other.m_cols)
		throw std::runtime_error("Error: Matrices must have the same dimensions for addition");
    for (std::size_t i = 0; i < m_elements.size(); ++i)
		m_elements[i] -= other.m_elements[i];
    return *this;
}

template < real_complex K >
Matrix<K>	Matrix<K>::operator-() const
{
    Matrix<K> m = *this;
    for (std::size_t i = 0; i < m_elements.size(); ++i)
        m.m_elements[i] = -m.m_elements[i];
    return m;
}

template < real_complex K >
Matrix<K>&  Matrix<K>::operator*=( const Matrix<K>& other )
{
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw std::runtime_error("Error: dimensions must be identical for element-wise multiplication");

    for (std::size_t i = 0; i < m_elements.size(); ++i)
        m_elements[i] *= other.m_elements[i];
    return *this;
}

template < real_complex K >
Matrix<K>&  Matrix<K>::operator*=( K factor )
{
    for (std::size_t i = 0; i < m_elements.size(); ++i)
        m_elements[i] *= factor;
    return *this;
}
