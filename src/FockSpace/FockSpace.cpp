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
#include "FockSpace/FockSpace.hpp"

#include <boost/numeric/conversion/converter.hpp>
#include <boost/math/special_functions.hpp>


namespace GQCP {



/*
 *  CONSTRUCTORS
 */

/**
 *  @param K        the number of orbitals
 *  @param N        the number of electrons
 */
FockSpace::FockSpace(size_t K, size_t N) :
        BaseFockSpace (K, FockSpace::calculateDimension(K, N)),
        FockPermutator (N)
{
    // Create a zero matrix of dimensions (K+1)x(N+1)
    this->vertex_weights = Matrixu(this->K + 1, Vectoru(this->N + 1, 0));

    // K=5   N=2
    // [ 0 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 0 ]


    // The largest (reverse lexical) string is the one that includes the first (K-N+1) vertices of the first column
    //      This is because every vertical move from (p,m) to (p+1,m+1) corresponds to "orbital p+1 is unoccupied".
    //      Therefore, the largest reverse lexical string is the one where the first (K-N) orbitals are unoccupied.
    //      This means that there should be (K-N) vertical moves from (0,0).
    // Therefore, we may only set the weights of first (K-N+1) vertices of the first column to 1.
    for (size_t p = 0; p < this->K - this->N + 1; p++) {
        this->vertex_weights[p][0] = 1;
    }

    // K=5   N=2
    // [ 1 0 0 ]
    // [ 1 0 0 ]
    // [ 1 0 0 ]
    // [ 1 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 0 ]


    // The recurrence relation for the vertex weights is as follows:
    //      Every element is the sum of the values of the element vertically above and the element left diagonally above.
    //      W(p,m) = W(p-1,m) + W(p-1,m-1)

    for (size_t m = 1; m < this->N + 1; m++) {
        for (size_t p = m; p < (this->K - this->N + m) + 1; p++) {
            this->vertex_weights[p][m] = this->vertex_weights[p - 1][m] + this->vertex_weights[p - 1][m - 1];
        }
    }

    // K=5   N=2
    // [ 1 0 0 ]
    // [ 1 1 0 ]
    // [ 1 2 1 ]
    // [ 1 3 3 ]
    // [ 0 4 6 ]
    // [ 0 0 10]
}



/*
 *  STATIC PUBLIC METHODS
 */

/**
 *  @param K        the number of orbitals
 *  @param N        the number of electrons
 *
 *  @return the dimension of the Fock space
 */
size_t FockSpace::calculateDimension(size_t K, size_t N) {
    auto dim_double = boost::math::binomial_coefficient<double>(static_cast<unsigned>(K), static_cast<unsigned>(N));
    try {
        return boost::numeric::converter<size_t, double>::convert(dim_double);
    } catch (boost::numeric::bad_numeric_cast &e) {
        throw std::overflow_error("FockSpace::calculateDimension(size_t, size_t): "+ std::string(e.what()));
    }
}



/*
 *  PUBLIC METHODS
 */

/**
 *  @param representation       a representation of an ONV
 *
 *  @return the next bitstring permutation
 *
 *      Examples:
 *          011 -> 101
 *          101 -> 110
 */
size_t FockSpace::ulongNextPermutation(size_t representation) const {

    // t gets this->representation's least significant 0 bits set to 1
    unsigned long t = representation | (representation - 1UL);

    // Next set to 1 the most significant bit to change,
    // set to 0 the least significant ones, and add the necessary 1 bits.
    return (t + 1UL) | (((~t & (t+1UL)) - 1UL) >> (__builtin_ctzl(representation) + 1UL));
}


/**
 *  @param representation      a representation of an ONV
 *
 *  @return the address (i.e. the ordering number) of the given ONV
 */
size_t FockSpace::getAddress(size_t unsigned_onv) const {
    // An implementation of the formula in Helgaker, starting the addressing count from zero
    size_t address = 0;
    size_t electron_count = 0;  // counts the number of electrons in the spin string up to orbital p
    while(unsigned_onv != 0) {  // we will remove the least significant bit each loop, we are finished when no bits are left
        size_t p = __builtin_ctzl(unsigned_onv);  // p is the orbital index counter (starting from 1)
        electron_count++;  // each bit is an electron hence we add it up to the electron count
        address += this->get_vertex_weights(p , electron_count);
        unsigned_onv ^= unsigned_onv & -unsigned_onv;  // flip the least significant bit
    }
    return address;

}


/**
 *  Calculate unsigned representation for a given address
 *
 *  @param address                 the address of the representation is calculated
 *
 *  @return unsigned representation of the address
 */
size_t FockSpace::calculateRepresentation(size_t address) const {
    size_t representation = 0;
    if (this->N != 0) {
        representation = 0;
        size_t m = this->N;  // counts the number of electrons in the spin string up to orbital p

        for (size_t p = this->K; p > 0; p--) {  // p is an orbital index
            size_t weight = get_vertex_weights(p-1, m);

            if (weight <= address) {  // the algorithm can move diagonally, so we found an occupied orbital
                address -= weight;
                representation |= ((1) << (p - 1));  // set the (p-1)th bit: see (https://stackoverflow.com/a/47990)

                m--;  // since we found an occupied orbital, we have one electron less
                if (m == 0) {
                    break;
                }
            }
        }
    }
    return representation;
}


/**
 *  @param onv       the ONV
 *
 *  @return the amount of ONVs (with a larger address) this ONV would couple with given a one electron operator
 */
size_t FockSpace::countOneElectronCouplings(const ONV& onv) const {
    size_t V = K-N;  // amount of virtual orbitals
    size_t coupling_count = 0;

    for (size_t e1 = 0; e1 < this->N; e1++) {
        size_t p = onv.get_occupation_index(e1);
        coupling_count += (V + e1 - p);  // amount of virtuals with an index larger than p
    }

    return coupling_count;
}


/**
 *  @param onv       the ONV
 *
 *  @return the amount of ONVs (with a larger address) this ONV would couple with given a two electron operator
 */
size_t FockSpace::countTwoElectronCouplings(const ONV& onv) const {

    size_t V = K-N; // amount of virtual orbitals
    size_t coupling_count = 0;

    for (size_t e1 = 0; e1 < this->N; e1++){

        size_t p = onv.get_occupation_index(e1);
        coupling_count += (V + e1 - p);  //  one electron part

        for (size_t e2 = e1+1; e2 < this->N; e2++){

            size_t q = onv.get_occupation_index(e2);
            size_t coupling_count2 = (V + e2 - q);
            coupling_count += (V-coupling_count2)*coupling_count2;

            if(coupling_count2 > 1 ){
                coupling_count += calculateDimension(coupling_count2, 2);
            }
        }
    }

    return coupling_count;
}


/**
 *  @return the amount non-zero couplings of a one electron coupling scheme in the Fock space
 */
size_t FockSpace::countTotalOneElectronCouplings() const {
    return (K-N)*N*(dim);
}


/**
 *  @return the amount non-zero couplings of a two electron coupling scheme in the Fock space
 */
size_t FockSpace::countTotalTwoElectronCouplings() const {

    size_t two_electron_permutation = 0; // all distributions for two electrons over the virtual orbitals
    if (K-N >= 2) {
        two_electron_permutation = calculateDimension(K-N, 2)*N*(N-1)*(dim)/2;
    }

    return two_electron_permutation + countTotalOneElectronCouplings();
}


}  // namespace GQCP
