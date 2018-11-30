// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2018  the GQCG developers
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
#include "miscellaneous.hpp"

#include <iostream>

#include <boost/numeric/conversion/converter.hpp>


namespace GQCP {

    
/**
 *  @param jacobi_rotation_parameters       the parameters that define the Jacobi rotation matrix
 *  @param M                                the dimension of the resulting matrix
 *
 *  @return the corresponding Jacobi rotation matrix. Note that we work with the (cos, sin, -sin, cos) definition of the Jacobi rotation matrix
 */
Eigen::MatrixXd jacobiRotationMatrix(const GQCP::JacobiRotationParameters& jacobi_rotation_parameters, size_t M) {

    double c = std::cos(jacobi_rotation_parameters.get_angle());
    double s = std::sin(jacobi_rotation_parameters.get_angle());

    // We'll start the construction with an identity matrix
    Eigen::MatrixXd J = Eigen::MatrixXd::Identity(M, M);

    // And apply the Jacobi rotation as J = I * jacobi_rotation (cfr. B' = B T)
    J.applyOnTheRight(jacobi_rotation_parameters.get_p(), jacobi_rotation_parameters.get_q(), Eigen::JacobiRotation<double> (c, s));
    return J;
}


/**
 *  @param A    the matrix
 *  @param i    row index (starting from 0)
 *  @param j    column index (starting from 0)
 *
 *  @return the i-j minor of the matrix A (i.e. delete the i-th row and j-th column)
 */
Eigen::MatrixXd minor(const Eigen::MatrixXd& A, size_t i, size_t j) {

    // Delete the i-th row
    Eigen::MatrixXd A_i = Eigen::MatrixXd::Zero(A.rows() - 1, A.cols());
    for (size_t i2 = 0; i2 < A.rows(); i2++) {  // loop over A's rows
        if (i2 < i) {
            A_i.row(i2) = A.row(i2);
        } else if (i2 == i) {
            continue;
        } else if (i2 > i) {
            A_i.row(i2-1) = A.row(i2);
        }
    }

    // Delete the j-th column
    Eigen::MatrixXd A_ij = Eigen::MatrixXd::Zero(A.rows() - 1, A.cols() - 1);
    for (size_t j2 = 0; j2 < A.cols(); j2++) {  // loop over A's columns
        if (j2 < j) {
            A_ij.col(j2) = A_i.col(j2);
        } else if (j2 == j) {
            continue;
        } else if (j2 > j) {
            A_ij.col(j2-1) = A_i.col(j2);
        }
    }

    return A_ij;
}

    

/**
 *  @param A        the square matrix
 *
 *  @return the permanent of the given square matrix using a combinatorial algorithm
 */
double permanent_combinatorial(const Eigen::MatrixXd& A) {

    if (A.rows() != A.cols()) {
        throw std::invalid_argument("The given matrix must be square.");
    }


    // The recursion ends when the given 'matrix' is just a number
    if ((A.rows() == 1) && (A.cols() == 1)) {
        return A(0,0);
    }

    size_t j = 0;  // develop by the first column
    double value = 0.0;
    for (size_t i = 0; i < A.rows(); i++) {
            value += A(i,j) * permanent_combinatorial(minor(A, i,j));
    }

    return value;
}


/**
 *  @param S    the positive integer to be converted to Gray code
 *
 *  @return the Gray code of the given integer number as a bitset
 */
size_t gray_code(size_t S) {

    // See (https://en.wikipedia.org/wiki/Gray_code#Converting_to_and_from_Gray_code)
    return S ^ (S >> 1);
}


/**
 *  @param A        the square matrix
 *
 *  @return the permanent of the given square matrix using the Ryser algorithm (see https://www.codeproject.com/Articles/21282/%2FArticles%2F21282%2FCompute-Permanent-of-a-Matrix-with-Ryser-s-Algorit)
 */
double permanent_ryser(const Eigen::MatrixXd& A) {

    if (A.rows() != A.cols()) {
        throw std::invalid_argument("The given matrix must be square.");
    }


    size_t n = A.rows();

    // Loop over all submatrices of A
    double value = 0.0;  // value of the permanent
    size_t number_of_submatrices = boost::numeric::converter<double, size_t>::convert(std::pow(2, n));
    for (size_t S = 1; S < number_of_submatrices; S++) {  // there are no 'chosen columns' in S=0

        // Generate the current submatrix through the Gray code of S: if the bit is 1, the column is chosen
        size_t gray_code_value = gray_code(S);
        size_t k = __builtin_popcountll(gray_code_value);  // number of columns

        Eigen::MatrixXd X = Eigen::MatrixXd::Zero(n, k);
        size_t j = 0;  // the column index in X
        while (gray_code_value != 0) {  // loop over the set bits in the Gray code
            size_t index = __builtin_ctzll(gray_code_value);  // the index in the original matrix

            X.col(j) = A.col(index);

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



}  // namespace GQCP
