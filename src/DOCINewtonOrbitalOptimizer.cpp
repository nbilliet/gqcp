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
#include "DOCINewtonOrbitalOptimizer.hpp"

#include <unsupported/Eigen/MatrixFunctions>

#include "CISolver/CISolver.hpp"
#include "RDM/RDMCalculator.hpp"

#include "utilities/linalg.hpp"
#include "math/optimization/step.hpp"
#include "math/optimization/EigenproblemSolverOptions.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */
/**
 *  @param doci         the DOCI HamiltonianBuilder
 *  @param ham_par      the Hamiltonian parameters in an orthonormal basis
 */
DOCINewtonOrbitalOptimizer::DOCINewtonOrbitalOptimizer(const DOCI& doci, const HamiltonianParameters<double>& ham_par) :
    doci (doci),
    ham_par (ham_par)
{}


/*
 *  GETTERS
 */
const std::vector<Eigenpair>& DOCINewtonOrbitalOptimizer::get_eigenpairs() const {
    if (this->is_converged) {
        return this->eigenpairs;
    } else {
        throw std::logic_error("DOCINewtonOrbitalOptimizer::get_eigenpairs(): You are trying to get eigenpairs but the orbital optimization hasn't converged (yet).");
    }
}

const Eigenpair& DOCINewtonOrbitalOptimizer::get_eigenpair(size_t index) const {
    if (this->is_converged) {
        return this->eigenpairs[index];
    } else {
        throw std::logic_error("DOCINewtonOrbitalOptimizer::get_eigenpair(size_t): You are trying to get eigenpairs but the orbital optimization hasn't converged (yet).");
    }
}



/*
 *  PUBLIC METHODS
 */
/**
 *  Do the orbital optimization for DOCI
 *
 *  @param solver_options       solver options for the CI solver
 *  @param oo_options           options for the orbital optimization
 */
void DOCINewtonOrbitalOptimizer::solve(BaseSolverOptions& solver_options, const OrbitalOptimizationOptions& oo_options) {
    this->is_converged = false;
    auto K = this->ham_par.get_K();
    RDMCalculator rdm_calculator(*this->doci.get_fock_space());  // make the RDMCalculator beforehand, it doesn't have to be constructed in every iteration
    size_t oo_iterations = 0;
    while (!(this->is_converged)) {

        // Solve the DOCI eigenvalue equation, using the options provided
        CISolver doci_solver (this->doci, this->ham_par);  // update the CI solver with the rotated Hamiltonian parameters
        doci_solver.solve(solver_options);
        rdm_calculator.set_coefficients(doci_solver.get_eigenpair().get_eigenvector());

        // Calculate the 1- and 2-RDMs
        auto D = rdm_calculator.calculate1RDMs().one_rdm;  // spin-summed 1-RDM
        auto d = rdm_calculator.calculate2RDMs().two_rdm;  // spin-summed 2-RDM


        // Calculate the electronic gradient at kappa = 0
        auto F = this->ham_par.calculateGeneralizedFockMatrix(D, d);
        SquareMatrix<double> gradient_matrix = 2 * (F - F.transpose());
        VectorX<double> gradient_vector = gradient_matrix.strictLowerTriangle();  // gradient vector with the free parameters, at kappa = 0


        // Calculate the electronic Hessian at kappa = 0
        auto W = this->ham_par.calculateSuperGeneralizedFockMatrix(D, d);
        SquareRankFourTensor<double> hessian_tensor (K);
        hessian_tensor.setZero();

        for (size_t p = 0; p < K; p++) {
            for (size_t q = 0; q < K; q++) {
                for (size_t r = 0; r < K; r++) {
                    for (size_t s = 0; s < K; s++) {
                        hessian_tensor(p,q,r,s) = W(p,q,r,s) - W(p,q,s,r) + W(q,p,s,r) - W(q,p,r,s) + W(r,s,p,q) - W(r,s,q,p) + W(s,r,q,p) - W(s,r,p,q);
                    }
                }
            }
        }
        auto hessian_matrix = hessian_tensor.pairWiseStrictReduce();  // hessian matrix with only the free parameters, at kappa = 0

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> hessian_solver (hessian_matrix);


        // Perform a Newton-step to find orbital rotation parameters kappa
        VectorFunction gradient_function = [gradient_vector](const VectorX<double>& x) { return gradient_vector; };
        MatrixFunction hessian_function = [hessian_matrix](const VectorX<double>& x) { return hessian_matrix; };

        VectorX<double> kappa_vector = newtonStep(VectorX<double>::Zero(K), gradient_function, hessian_function);  // with only the free parameters


        // If the calculated norm is zero, we have reached a critical point
        if (gradient_vector.norm() < oo_options.convergence_threshold) {

            // If we have found a critical point, but we have a negative eigenvalue for the Hessian, continue in that direction
            if (hessian_solver.eigenvalues()(0) < 0) {
                kappa_vector = hessian_solver.eigenvectors().col(0);
            }
            else {  // the Hessian is confirmed to be positive definite, so we have reached a minimum
                this->is_converged = true;

                // Set solutions
                this->eigenpairs = doci_solver.get_eigenpairs();

                break;  // no need to continue if we have converged
            }


        } else {
            oo_iterations++;

            if (oo_iterations >= oo_options.maximum_number_of_iterations) {
                throw std::runtime_error("DOCINewtonOrbitalOptimizer::solve(BaseSolverOptions, OrbitalOptimizationOptions): The OO-DOCI procedure failed to converge in the maximum number of allowed iterations.");
            }
        }


        // Change kappa back to a matrix
        auto kappa_matrix = GQCP::SquareMatrix<double>::FromStrictTriangle(kappa_vector);  // containing all parameters, so this is in anti-Hermitian (anti-symmetric) form
        SquareMatrix<double> kappa_matrix_transpose = kappa_matrix.transpose();  // store the transpose in an auxiliary variable to avoid aliasing issues
        kappa_matrix -= kappa_matrix_transpose;  // FromStrictTriangle only returns the lower triangle, so we must construct the anti-Hermitian (anti-symmetric) matrix


        // Calculate the unitary rotation matrix that we can use to rotate the basis
        SquareMatrix<double> U = (-kappa_matrix).exp();


        // Transform the integrals to the new orthonormal basis
        this->ham_par.rotate(U);  // this checks if U is actually unitary


        // If we're using a Davidson solver, we should update the initial guesses in the solver_options to be the current eigenvectors
        if (solver_options.get_solver_type() == SolverType::DAVIDSON) {
            auto davidson_solver_options = dynamic_cast<DavidsonSolverOptions&>(solver_options);

            for (size_t i = 0; i < solver_options.number_of_requested_eigenpairs; i++) {
                davidson_solver_options.X_0.col(i) = doci_solver.makeWavefunction(i).get_coefficients();
            }

            solver_options = davidson_solver_options;
        }
    }  // while not converged
}


/**
 *  @param index        the index of the index-th excited state
 *
 *  @return the index-th excited state after doing the OO-DOCI calculation
 */
WaveFunction DOCINewtonOrbitalOptimizer::makeWavefunction(size_t index) const {
    if (index > this->eigenpairs.size()) {
        throw std::logic_error("DOCINewtonOrbitalOptimizer::makeWavefunction(size_t): Not enough requested eigenpairs for the given index.");
    }
    return WaveFunction(*this->doci.get_fock_space(), this->eigenpairs[index].get_eigenvector());
}


}  // namespace GQCP
