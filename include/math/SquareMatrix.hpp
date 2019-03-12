// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2019  the GQCG developers
// 
// GQCG-gqcp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GQCG-gqcp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-gqcp.  If not, see <http://www.gnu.org/licenses/>.
// 
#ifndef SquareMatrix_hpp
#define SquareMatrix_hpp


#include "math/Matrix.hpp"

#include "utilities/miscellaneous.hpp"
#include "JacobiRotationParameters.hpp"

#include <boost/numeric/conversion/converter.hpp>


namespace GQCP {


/**
 *  A dynamic square extension of the Matrix class
 *
 *  @tparam _Scalar      the scalar type
 */
template<typename _Scalar>
class SquareMatrix : public MatrixX<_Scalar> {
public:

    using Scalar = _Scalar;

    using Base = MatrixX<Scalar>;
    using Self = SquareMatrix<Scalar>;


    /*
     *  CONSTRUCTORS
     */

    /**
     *  Default constructor
     */
    SquareMatrix() : Base() {}    


    /**
     *  Construct a square matrix given a dimension
     *
     *  @param dim      the dimension of the matrix
     */
    SquareMatrix(size_t dim) :
        Base(dim, dim)
    {}


    /**
     *  A basic constructor from a GQCP::MatrixX that checks if the given matrix is square
     *
     *  @param matrix       the matrix that should be square
     */
    SquareMatrix(const Base& matrix) :
        Base(matrix)
    {
        // Check if the given matrix is square
        if (this->cols() != this->rows()) {
            throw std::invalid_argument("SquareMatrix::SquareMatrix(Base): The given matrix is not square.");
        }
    }


    /**
     *  Constructor from Eigen::Matrix expressions
     *
     *  @tparam ExpDerived      the type of the Eigen::Matrix expression (normally generated by the compiler)
     *
     *  @param exp              the Eigen::Matrix expression
     */
    template <typename ExpDerived>
    SquareMatrix(const Eigen::MatrixBase<ExpDerived>& exp) :
        Self(Base(exp))  // the Base constructor returns the required type for the square-checking constructor
    {}


    /*
     *  NAMED CONSTRUCTORS
     */

    /**
     *  Constructor from a strict lower/upper triangle, and the other elements are set to zero
     *
     *  @param a        the strict (not including the diagonal) lower/upper triangle of the matrix in column major form
     */
    static Self FromStrictTriangle(const VectorX<Scalar>& a) {

        // Check for valid input
        auto N = static_cast<size_t>(a.size());  // dimension of the vector
        double K_ = 0.5 + 0.5 * std::sqrt(1 + 8*N);  // dimension of the matrix
        if (std::abs(K_ - std::floor(K_)) > 1.0e-12) {  // if K is not an integer, within the given precision, i.e. N is not a triangular number
            throw std::invalid_argument("SquareMatrix::FromStrictTriangle(VectorX<Scalar>): The given vector cannot be stored in the strict lower triangle of a matrix.");
        }

        // After the input checking, we are safe to cast K into size_t
        auto K = static_cast<size_t>(K_);
        Self A = Self::Zero(K, K);

        size_t column_index = 0;
        size_t row_index = column_index + 1;  // fill the lower triangle
        for (size_t vector_index = 0; vector_index < N; vector_index++) {
            A(row_index,column_index) = a(vector_index);

            if (row_index == K-1) {  // -1 because of computers
                column_index++;
                row_index = column_index + 1;
            } else {
                row_index++;
            }
        }
        return A;
    }


    /**
     *  @param v    the upper triangle (including the diagonal) of a matrix
     *
     *  @return the full, symmetric matrix corresponding to the given upper triangle
     */
    static Self FullFromTriangle(const VectorX<Scalar>& v) {

        size_t x = v.size();
        size_t N = (static_cast<size_t>(sqrt(1 + 8*x) - 1))/2;

        if (N * (N+1) != 2*x) {
            throw std::invalid_argument("SquareMatrix::FullFromTriangle(VectorX<Scalar>): The given vector is does not correspond to the upper triagonal of a square matrix.");
        }


        Self A = Self::Zero(N, N);

        size_t k = 0;  // vector index
        for (size_t i = 0; i < N; i++) {  // row index
            for (size_t j = i; j < N; j++) {  // column index
                if (i != j) {
                    A(i,j) = v(k);
                    A(j,i) = v(k);
                } else {
                    A(i,i) = v(k);
                }

                k++;
            }
        }

        return A;
    }


    /**
     *  @param jacobi_rotation_parameters       the parameters that define the Jacobi rotation matrix
     *  @param M                                the dimension of the resulting matrix
     *
     *  @return the corresponding Jacobi rotation matrix. Note that we work with the (cos, sin, -sin, cos) definition
     */
    static Self FromJacobi(const JacobiRotationParameters& jacobi_rotation_parameters, size_t M) {

        double c = std::cos(jacobi_rotation_parameters.get_angle());
        double s = std::sin(jacobi_rotation_parameters.get_angle());

        // We'll start the construction with an identity matrix
        Self J = Self::Identity(M, M);

        // And apply the Jacobi rotation as J = I * jacobi_rotation (cfr. B' = B T)
        J.applyOnTheRight(jacobi_rotation_parameters.get_p(), jacobi_rotation_parameters.get_q(), Eigen::JacobiRotation<double> (c, s));
        return J;
    }


    /*
     *  GETTERS
     */

    size_t get_dim() const {
        return this->cols();  // equals this->rows()
    }


    /*
     *  PUBLIC METHODS
     */

    /**
     *  @return the strict lower triangular matrix (i.e. without the diagonal elements) as a vector in column-major form
     *
     *          5
     *          1   5       -> (1, 2, 3)
     *          2   3   5
     */
    VectorX<Scalar> strictLowerTriangle() const {

        auto dim = this->get_dim();

        VectorX<Scalar> m = VectorX<Scalar>::Zero((dim*(dim-1)/2));  // strictly lower triangle has dim(dim-1)/2 parameters

        size_t vector_index = 0;
        for (size_t q = 0; q < dim; q++) {  // "column major" ordering for, so we do p first, then q
            for (size_t p = q+1; p < dim; p++) {  // strict lower triangle means p > q
                m(vector_index) = this->operator()(p,q);
                vector_index++;
            }
        }

        return m;
    }


    /**
     *  @return the permanent using a combinatorial algorithm
     */
    double permanent_combinatorial() const {

        // The recursion ends when the given 'matrix' is just a number
        if ((this->rows() == 1) && (this->cols() == 1)) {
            return this->operator()(0,0);
        }

        size_t j = 0;  // develop by the first column
        double value = 0.0;
        for (size_t i = 0; i < this->rows(); i++) {
            value += this->operator()(i,j) * SquareMatrix<Scalar>(this->matrixMinor(i,j)).permanent_combinatorial();  // need to convert because 'minor' is a Base function and isn't guaranteed to be square
        }

        return value;
    }


    /**
     *  @return the permanent of the given square matrix using the Ryser algorithm
     *
     *  Note that this algorithm does not work for dimensions larger than 64: see https://www.codeproject.com/Articles/21282/%2FArticles%2F21282%2FCompute-Permanent-of-a-Matrix-with-Ryser-s-Algorit
     */
    double permanent_ryser() const {

        size_t n = this->get_dim();

        // Loop over all submatrices of A
        double value = 0.0;  // value of the permanent
        size_t number_of_submatrices = boost::numeric::converter<size_t, double>::convert(std::pow(2, n));
        for (size_t S = 1; S < number_of_submatrices; S++) {  // there are no 'chosen columns' in S=0

            // Generate the current submatrix through the Gray code of S: if the bit is 1, the column is chosen
            size_t gray_code_value = gray_code(S);
            size_t k = __builtin_popcountll(gray_code_value);  // number of columns

            Base X = Base::Zero(n, k);
            size_t j = 0;  // the column index in X
            while (gray_code_value != 0) {  // loop over the set bits in the Gray code
                size_t index = __builtin_ctzll(gray_code_value);  // the index in the original matrix

                X.col(j) = this->col(index);

                gray_code_value ^= gray_code_value & -gray_code_value;  // flip the first set bit
                j++;
            }


            // Calculate the product of all the row sums and multiply by the sign
            double product_of_rowsums = X.array().rowwise().sum().prod();

            size_t t = n - k;  // number of deleted columns
            int sign = std::pow(-1, t);
            value += sign * product_of_rowsums;
        }

        return value;
    }
};


}  // namespace GQCP


#endif /* SquareMatrix_hpp */
