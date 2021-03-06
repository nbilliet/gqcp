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
#include "math/optimization/NewtonMinimizer.hpp"

#include "typedefs.hpp"
#include "math/optimization/NewtonSystemOfEquationsSolver.hpp"
#include "math/SquareMatrix.hpp"

#include <iostream>


namespace GQCP {


/*
 *  CONSTRUCTORS
 */
/**
 *  @param x0                               the initial guess
 *  @param grad                             the callable gradient function
 *  @param H                                the callable Hessian function
 *  @param convergence_threshold            the threshold used for establishing convergence
 *  @param maximum_number_of_iterations     the maximum number of iterations in the algorithm
 */
NewtonMinimizer::NewtonMinimizer(const VectorX<double>& x0, const VectorFunction& grad, const MatrixFunction& H, double convergence_threshold, size_t maximum_number_of_iterations) :
    BaseMinimizer(x0, convergence_threshold, maximum_number_of_iterations),
    grad (grad),
    H (H)
{}



/*
 *  PUBLIC OVERRIDDEN FUNCTIONS
 */
/**
 *  Minimize the function f(x)
 *
 *  If successful, sets
 *      - is_solved to true
 *      - the found solution
 */
void NewtonMinimizer::solve() {

    // Requiring the gradient to vanish, and then calculating the corresponding Newton step, is equivalent to solving
    // the system of equations grad(f(x)) = 0 using a Newton step

    // For mathematical correctness, the Jacobian of the gradient is the transpose of the Hessian of the scalar function
    // behind it
    MatrixFunction H_t = [this](const VectorX<double>& x) {
        SquareMatrix<double> H = this->H(x);
        H.transposeInPlace();
        return H;
    };


    // For previously established reasons, we can use the NewtonSystemOfEquationsSolver as an implementation of this
    // minimization problem
    NewtonSystemOfEquationsSolver newton_syseq_solver (this->x, this->grad, H_t, this->convergence_threshold);
    newton_syseq_solver.solve();


    // If we haven't found a solution, the error is raised inside the NewtonSystemOfEquationsSolver, so we are free to
    // assert that if the data flow gets to here, a solution is found
    this->is_solved = true;
    this->x = newton_syseq_solver.get_solution();
}


}  // namespace GQCP
