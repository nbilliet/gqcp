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
#ifndef AP1roGGeminalCoefficients_hpp
#define AP1roGGeminalCoefficients_hpp


#include "geminals/AP1roGVariables.hpp"
#include "geminals/GeminalCoefficientsInterface.hpp"
#include "HamiltonianParameters/HamiltonianParameters.hpp"
#include "math/Matrix.hpp"
#include "WaveFunction/WaveFunction.hpp"


namespace GQCP {


/**
 *  A class that represents geminal coefficients for an AP1roG wave function
 */
class AP1roGGeminalCoefficients : public AP1roGVariables, public GeminalCoefficientsInterface {
public:
    // CONSTRUCTORS
    /**
     *  Default constructor setting everything to zero
     */
    AP1roGGeminalCoefficients();  // default constructor needed

    /**
     *  @param g        the geminal coefficients in a vector representation that is in row-major storage
     *
     *  @param N_P      the number of electron pairs (= the number of geminals)
     *  @param K        the number of spatial orbitals
     */
    AP1roGGeminalCoefficients(const VectorX<double>& g, size_t N_P, size_t K);

    /**
     *  Constructor that sets the geminal coefficients to zero
     *
     *  @param N_P      the number of electron pairs (= the number of geminals)
     *  @param K        the number of spatial orbitals
     */
    AP1roGGeminalCoefficients(size_t N_P, size_t K);


    // NAMED CONSTRUCTORS
    /**
     *  @param ham_par      the Hamiltonian parameters
     *  @param N_P          the number of electron pairs (= the number of geminals)
     *
     *  @return the AP1roG geminal coefficients in the weak interaction limit
     */
    static AP1roGGeminalCoefficients WeakInteractionLimit(const HamiltonianParameters<double>& ham_par, size_t N_P);


    // DESTRUCTOR
    ~AP1roGGeminalCoefficients() override;


    // STATIC PUBLIC METHODS
    /**
     *  @param N_P      the number of electron pairs (= the number of geminals)
     *  @param K        the number of spatial orbitals
     *
     *  @return the number of 'free' geminal coefficients
     */
    static size_t numberOfGeminalCoefficients(size_t N_P, size_t K);


    // PUBLIC METHODS
    /**
     *  @return the geminal coefficients in matrix form
     */
    MatrixX<double> asMatrix() const override;

    /**
     *  @param onv      the ONV that is being projected on
     *
     *  @return the overlap of the AP1roG wave function with the given on, i.e. the projection of the APIG wave function onto that ONV
     */
    double overlap(const ONV& onv) const override;
};


}  // namespace GQCP



#endif /* AP1roGGeminalCoefficients_hpp */
