#ifndef GQCP_BASEFOCKSPACE_HPP
#define GQCP_BASEFOCKSPACE_HPP


#include "ONV.hpp"
#include "common.hpp"



namespace GQCP {


/**
 *  A base class for the Fock space
 *  Interfacing requires the Fock space to generate an ONV from a given address
 *  transform a given ONV into the next ONV (in the full or selected space)
 *  and retrieve the address of a given ONV in the space
 */
class BaseFockSpace {
protected:
    const size_t K;  // number of spatial orbitals
    const size_t dim;  // dimension of the Fock space


    // PROTECTED CONSTRUCTORS
    /**
     *  Protected constructor given a @param K and @param dim
     */
    explicit BaseFockSpace(size_t K, size_t dim);


public:
    // DESTRUCTOR
    /**
     *  Provide a pure virtual destructor to make the class abstract
     */
    virtual ~BaseFockSpace() = 0;


    // GETTERS
    size_t get_dimension() const { return dim; }
    size_t get_K() const { return K; }


    // PUBLIC METHODS
    /**
     *  Creates a Hartree-Fock coefficient expansion (single Slater expansion of the first configuration in the Fock space)
     */
    Eigen::VectorXd HartreeFockExpansion();
};


}  // namespace GQCP


#endif  // GQCP_BASEFOCKSPACE_HPP
