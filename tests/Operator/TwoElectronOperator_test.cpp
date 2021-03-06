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
#define BOOST_TEST_MODULE "TwoElectronOperator"


#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain

#include "Operator/TwoElectronOperator.hpp"

#include "utilities/linalg.hpp"

#include "utilities/miscellaneous.hpp"


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_constructor ) {

    // Check a correct constructor
    GQCP::Tensor<double, 4> tensor (3, 3, 3, 3);
    GQCP::TwoElectronOperator<double> O (tensor);


    // Check a faulty constructor
    GQCP::Tensor<double, 4> tensor2 (3, 3, 3, 2);
    BOOST_CHECK_THROW(GQCP::TwoElectronOperator<double> O2 (tensor2), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_transform_trivial ) {

    // Let's test a trivial transformation: i.e. with T being a unit matrix
    GQCP::TwoElectronOperator<double> G (3);

    auto G_copy = G;

    GQCP::SquareMatrix<double> T = GQCP::SquareMatrix<double>::Identity(3, 3);
    G.transform(T);

    BOOST_CHECK(G_copy.isApprox(G, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_transform_olsens ) {

    // We can find a reference algorithm in the olsens module from Ayer's lab
    size_t dim = 2;
    GQCP::TwoElectronOperator<double> g_transformed_ref = GQCP::TwoElectronOperator<double>::FromFile("data/rotated_two_electron_integrals_olsens.data", dim);

    // Set an example transformation matrix and two-electron integrals tensor
    GQCP::SquareMatrix<double> T (dim);
    T << 1, 2,
         3, 4;

    GQCP::TwoElectronOperator<double> G (dim);
    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    G(i, j, k, l) = l + 2*k + 4*j + 8*i;
                }
            }
        }
    }
    G.transform(T);

    BOOST_CHECK(G.isApprox(g_transformed_ref, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_rotate_throws ) {

    // Create a random TwoElectronOperator
    size_t dim = 3;
    GQCP::SquareRankFourTensor<double> g (dim);
    g.setRandom();
    GQCP::TwoElectronOperator<double> G (g);


    // Check if a non-unitary matrix as transformation matrix causes a throw
    GQCP::SquareMatrix<double> U = GQCP::SquareMatrix<double>::Random(dim, dim);
    BOOST_CHECK_THROW(G.rotate(GQCP::SquareMatrix<double>(U)), std::invalid_argument);


    // Check if a unitary matrix as transformation matrix is accepted
    GQCP::SquareMatrix<double> T = GQCP::SquareMatrix<double>::Identity(dim, dim);
    G.rotate(T);
}


BOOST_AUTO_TEST_CASE ( TwoElectronOperator_rotate_JacobiRotationParameters ) {

    // Create a random TwoElectronOperator
    size_t dim = 5;
    GQCP::SquareRankFourTensor<double> g (dim);
    g.setRandom();
    GQCP::TwoElectronOperator<double> G1 (g);
    GQCP::TwoElectronOperator<double> G2 (g);


    // Check that using a Jacobi transformation (rotation) matrix as U is equal to the custom transformation (rotation)
    // with custom JacobiRotationParameters
    GQCP::JacobiRotationParameters jacobi_rotation_parameters (4, 2, 56.81);

    auto U = GQCP::SquareMatrix<double>::FromJacobi(jacobi_rotation_parameters, dim);


    G1.rotate(jacobi_rotation_parameters);
    G2.rotate(GQCP::SquareMatrix<double>(U));


    BOOST_CHECK(G1.isApprox(G2, 1.0e-12));
}
