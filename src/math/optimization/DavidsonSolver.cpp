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
#include "math/optimization/DavidsonSolver.hpp"
#include <iostream>



#include <chrono>



namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param matrixVectorProduct                  a vector function that returns the matrix-vector product (i.e. the matrix-vector product representation of the matrix)
 *  @param diagonal                             the diagonal of the matrix
 *  @param V_0                                  the (set of) initial guess(es) specified as a vector (matrix of column vectors)
 *  @param number_of_requested_eigenpairs       the number of eigenpairs the solver should find
 *  @param convergence_threshold                the tolerance on the norm of the residual vector
 *  @param correction_threshold                 the threshold used in solving the (approximated) residue correction equation
 *  @param maximum_subspace_dimension           the maximum dimension of the Davidson subspace before collapsing
 *  @param collapsed_subspace_dimension         the dimension of the subspace after collapse
 *  @param maximum_number_of_iterations         the maximum number of Davidson iterations
 */
DavidsonSolver::DavidsonSolver(const VectorFunction& matrixVectorProduct, const VectorX<double>& diagonal, const MatrixX<double>& V_0, size_t number_of_requested_eigenpairs, double convergence_threshold, double correction_threshold, size_t maximum_subspace_dimension, size_t collapsed_subspace_dimension, size_t maximum_number_of_iterations) :
    BaseEigenproblemSolver(static_cast<size_t>(V_0.rows()), number_of_requested_eigenpairs),
    matrixVectorProduct (matrixVectorProduct),
    diagonal (diagonal),
    V_0 (V_0),
    convergence_threshold (convergence_threshold),
    correction_threshold (correction_threshold),
    maximum_subspace_dimension (maximum_subspace_dimension),
    collapsed_subspace_dimension (collapsed_subspace_dimension),
    maximum_number_of_iterations (maximum_number_of_iterations)
{
    if (V_0.cols() < this->number_of_requested_eigenpairs) {
        throw std::invalid_argument("DavidsonSolver::DavidsonSolver(VectorFunction, VectorX<double>, MatrixX<double>, size_t, double, double, size_t, size_t, size_t): You have to specify at least as many initial guesses as number of requested eigenpairs.");
    }

    if (this->collapsed_subspace_dimension < this->number_of_requested_eigenpairs) {
        throw std::invalid_argument("DavidsonSolver::DavidsonSolver(VectorFunction, VectorX<double>, MatrixX<double>, size_t, double, double, size_t, size_t, size_t): The collapsed subspace dimension must be at least the number of requested eigenpairs.");
    }

    if (this->collapsed_subspace_dimension >= this->maximum_subspace_dimension) {
        throw std::invalid_argument("DavidsonSolver::DavidsonSolver(VectorFunction, VectorX<double>, MatrixX<double>, size_t, double, double, size_t, size_t, size_t): The collapsed subspace dimension must be smaller than the maximum subspace dimension.");
    }
}


/**
 *  @param A                                    the matrix to be diagonalized
 *  @param V_0                                  the (set of) initial guess(es) specified as a vector (matrix of column vectors)
 *  @param number_of_requested_eigenpairs       the number of eigenpairs the solver should find
 *  @param convergence_threshold                    the tolerance on the norm of the residual vector
 *  @param correction_threshold                 the threshold used in solving the (approximated) residue correction equation
 *  @param maximum_subspace_dimension           the maximum dimension of the Davidson subspace before collapsing
 *  @param collapsed_subspace_dimension         the dimension of the subspace after collapse
 *  @param maximum_number_of_iterations         the maximum number of Davidson iterations
 */
DavidsonSolver::DavidsonSolver(const SquareMatrix<double>& A, const MatrixX<double>& V_0, size_t number_of_requested_eigenpairs, double convergence_threshold, double correction_threshold, size_t maximum_subspace_dimension, size_t collapsed_subspace_dimension, size_t maximum_number_of_iterations) :
    DavidsonSolver([A](const VectorX<double>& x) { return A * x; },  // lambda matrix-vector product function created from the given matrix A
                   A.diagonal(), V_0, number_of_requested_eigenpairs, convergence_threshold, correction_threshold, maximum_subspace_dimension, collapsed_subspace_dimension, maximum_number_of_iterations)
{}


/**
 *  @param matrixVectorProduct          a vector function that returns the matrix-vector product (i.e. the matrix-vector product representation of the matrix)
 *  @param diagonal                     the diagonal of the matrix
 *  @param davidson_solver_options      the options specified for solving the Davidson eigenvalue problem
 */
DavidsonSolver::DavidsonSolver(const VectorFunction& matrixVectorProduct, const VectorX<double>& diagonal,
                               const DavidsonSolverOptions& davidson_solver_options) :
   DavidsonSolver(matrixVectorProduct, diagonal, davidson_solver_options.X_0, davidson_solver_options.number_of_requested_eigenpairs, davidson_solver_options.convergence_threshold, davidson_solver_options.correction_threshold, davidson_solver_options.maximum_subspace_dimension, davidson_solver_options.collapsed_subspace_dimension, davidson_solver_options.maximum_number_of_iterations)
{}


/**
 *  @param A                            the matrix to be diagonalized
 *  @param davidson_solver_options      the options specified for solving the Davidson eigenvalue problem
 */
DavidsonSolver::DavidsonSolver(const SquareMatrix<double>& A, const DavidsonSolverOptions& davidson_solver_options) :
    DavidsonSolver([A](const VectorX<double>& x) { return A * x; },  // lambda matrix-vector product function created from the given matrix A
                   A.diagonal(), davidson_solver_options)
{}



/*
 *  GETTERS
 */

size_t DavidsonSolver::get_number_of_iterations() const {

    if (this->_is_solved) {
        return this->number_of_iterations;
    } else {
        throw std::invalid_argument("DavidsonSolver::get_number_of_iterations(): The Davidson hasn't converged (yet) and you are trying to get the number of iterations.");
    }
}



/*
 *  PUBLIC METHODS
 */

/**
 *  Solve the eigenvalue problem related to the given matrix-vector product
 *
 *  If successful, it sets
 *      - _is_solved to true
 *      - the number of requested eigenpairs
 */
void DavidsonSolver::solve() {

    // Calculate the expensive matrix-vector products for all given initial guesses, and store them in VA
    MatrixX<double> VA = MatrixX<double>::Zero(this->dim, this->V_0.cols());

    for (size_t j = 0; j < this->V_0.cols(); j++) {
        VA.col(j) = this->matrixVectorProduct(this->V_0.col(j));
    }

    // Calculate the initial subspace matrix S
    MatrixX<double> V = this->V_0;
    MatrixX<double> S = V.transpose() * VA;


    // this->number_of_iterations starts at 0
    while (!(this->_is_solved)) {
        // Diagonalize the subspace matrix and find the r (this->number_of_requested_eigenpairs) lowest eigenpairs
        // Lambda contains the requested number of eigenvalues, Z contains the corresponding eigenvectors
        // Z is a (subspace_dimension x number_of_requested_eigenpairs)- matrix
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver (S);
        VectorX<double> Lambda = eigensolver.eigenvalues().head(this->number_of_requested_eigenpairs);
        MatrixX<double> Z = eigensolver.eigenvectors().topLeftCorner(S.cols(), this->number_of_requested_eigenpairs);


        // Calculate new guesses for the eigenvectors
        // X is a (dim x number_of_requested_eigenpairs)-matrix
        MatrixX<double> X = V * Z;


        // Calculate the residual vectors and solve the residual equations
        //  Calculate the residual vectors in the matrix R (dim x number_of_requested_eigenpairs)
        //  Calculate the correction vectors in the matrix Delta (dim x number_of_requested_eigenpairs)
        MatrixX<double> R = MatrixX<double>::Zero(this->dim, this->number_of_requested_eigenpairs);
        MatrixX<double> Delta = MatrixX<double>::Zero(this->dim, this->number_of_requested_eigenpairs);
        for (size_t column_index = 0; column_index < R.cols(); column_index++) {

            // Calculate the residual vectors
            R.col(column_index) = VA * Z.col(column_index) - Lambda(column_index) * X.col(column_index);

            // Solve the residual equations
            // The implementation of these equations is adapted from Klaas Gunst's DOCI code (https://github.com/klgunst/doci)
            VectorX<double> denominator = this->diagonal - VectorX<double>::Constant(this->dim, Lambda(column_index));
            Delta.col(column_index) = (denominator.array().abs() > this->correction_threshold).select(R.col(column_index).array() / denominator.array().abs(),
                                                                                                      R.col(column_index) / this->correction_threshold);
            Delta.col(column_index).normalize();
        }


        // Check for convergence on each of the residual vectors
        //  If all residual norms are smaller than the threshold, the algorithm is considered converging
        //  We use !any() because it's possibly smaller than all()
        if (!((R.colwise().norm().array() > this->convergence_threshold).any())) {  // CLion can give errors that .any() is not found, but it compiles
            this->_is_solved = true;

            // Set the eigenvalues and eigenvectors in this->eigenpairs
            for (size_t i = 0; i < this->number_of_requested_eigenpairs; i++) {
                double eigenvalue = Lambda(i);
                VectorX<double> eigenvector = X.col(i);

                this->eigenpairs.emplace_back(eigenvalue, eigenvector);  // already reserved in the base constructor
            }
	
	    break;  // because we don't want the flow to continue to after the if-statement
        }

        else {  // if not yet converged
            this->number_of_iterations++;

            // If we reach more than this->maximum_number_of_iterations, the system is considered not to be converging
            if (this->number_of_iterations >= this->maximum_number_of_iterations) {
                throw std::runtime_error("DavidsonSolver::solve(): The Davidson algorithm did not converge.");
            }
        }


        // Calculate new subspace vectors by projecting the correction vectors (in Delta) onto the orthogonal complement of V
        for (size_t column_index = 0; column_index < Delta.cols(); column_index++) {

            // Project the correction vectors on the orthogonal complement of V
            VectorX<double> v = Delta.col(column_index) - V * (V.transpose() * Delta.col(column_index));

            double norm = v.norm();  // calculate the norm before normalizing: if the norm is large enough, we include it in the subspace
            v.normalize();

            if (norm > 1.0e-03) {  // include in the new subspace

                // If needed, do a subspace collapse
                if (V.cols() == this->maximum_subspace_dimension) {
                    MatrixX<double> lowest_eigenvectors = eigensolver.eigenvectors().topLeftCorner(S.cols(), this->collapsed_subspace_dimension);

                    // The new subspace vectors are linear combinations of current subspace vectors, with coefficients found in the lowest eigenvectors of the subspace matrix
                    V = V * lowest_eigenvectors;
                    VA = VA * lowest_eigenvectors;

                    S = V.transpose() * VA;
                }

                V.conservativeResize(Eigen::NoChange, V.cols()+1);
                V.col(V.cols()-1) = v;

                VectorX<double> vA = this->matrixVectorProduct(v);  // calculate the expensive matrix-vector product if a new vector is added to the subspace
                VA.conservativeResize(Eigen::NoChange, VA.cols()+1);
                VA.col(VA.cols()-1) = vA;
            }
            assert((V.transpose() * V).isApprox(MatrixX<double>::Identity(V.cols(), V.cols()), 1.0e-08));  // make sure that the subspace vectors are orthonormal
        }

        // Calculate the new subspace matrix
        //  After an iteration, we have enlarged V with new guess vectors, so our subspace matrix S should also increase
        //  S's dimension is too short by (V.cols - S.cols)
        auto previous_subspace_dimension = static_cast<size_t>(S.cols());
        auto current_subspace_dimension = static_cast<size_t>(V.cols());
        auto dimension_difference = current_subspace_dimension - previous_subspace_dimension;

        S.conservativeResize(S.rows()+dimension_difference, S.cols()+dimension_difference);

        for (auto j = previous_subspace_dimension-1; j < current_subspace_dimension; j++) {  // -1 because of computers
            // Only calculate the rows of S that haven't been calculated yet
            VectorX<double> s_j = V.transpose() * VA.col(j);  // s_j = V^T vA_j
            S.col(j) = s_j;
            S.row(j) = s_j;
        }
    }
}


}  // namespace GQCP
