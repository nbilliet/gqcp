#ifndef GQCP_ONV_HPP
#define GQCP_ONV_HPP


#include <Eigen/Dense>

#include "common.hpp"



namespace GQCP {


/**
 *      An ONV in quantum chemistry is a string of creation operators acting on top of a vacuum state.
 *      An example for 3 alpha electrons in a Fock space spanned by 4 spatial orbitals is
 *          a_1^\dagger a_2^\dagger a_3^\dagger |vac> = |1,1,1,0>
 *
 *      In this code, we are using REVERSE LEXICAL notation, i.e. bitstrings are read from right to left. This means that the
 *      least significant bit relates to the first orbital. Using this notation is how normally bits are read, leading
 *      to more efficient code. As is also usual, the least significant bit has index 0.
 *          The previous example is then represented by the bit string "0111" (7).
 */
class ONV {
private:
    const size_t K;  // number of spatial orbitals
    const size_t N;  // number of electrons
    size_t unsigned_representation;  // unsigned representation
    VectorXs occupation_indices;  // the occupied orbital electron indexes
                                  // it is a vector of N elements in which occupation_indices[j]
                                  // gives the occupied orbital index for electron j


public:
    // CONSTRUCTORS
    /**
     *  Constructor from a @param K orbitals, N electrons and an @param unsigned_representation
     */
    ONV(size_t K, size_t N, size_t unsigned_representation);


    // OPERATORS
    /**
     *  Overloading of operator<< for a GQCP::ONV to be used with streams
     */
    friend std::ostream& operator<<(std::ostream& os, const GQCP::ONV& onv);

    /**
     *  @return if this->unsigned_representation equals @param other.unsigned_representation
     */
    bool operator==(ONV& other) const;

    /**
     *  @return if this->unsigned_representation does not equal @param other.unsigned_representation
     */
    bool operator!=(ONV& other) const;


    // GETTERS & SETTERS
    /**
     *  @set to a new representation and calls this->updateOccupationIndices()
     */
    void set_representation(size_t unsigned_representation);
    size_t get_unsigned_representation() const { return unsigned_representation; }
    VectorXs get_occupation_indices() const { return occupation_indices; }

    /**
     *  @return index of occupied orbital based on the @param electron index
     *
     */
    size_t get_occupied_index(size_t electron_index) const { return occupation_indices(electron_index); }


    // PUBLIC METHODS
    /**
     *  Extracts the positions of the set bits from the this->unsigned_representation
     *  and places them in the this->occupation_indices
     */
    void updateOccupationIndices();

    /**
     *  @return if the @param p-th spatial orbital is occupied, starting from 0
     *  @param p is the lexical index (i.e. read from right to left)
     */
    bool isOccupied(size_t p) const;

    /**
     *  @return if we can apply the annihilation operator (i.e. 1->0) for the @param p-th spatial orbital
     *  Subsequently perform an in-place annihilation on the orbital @param p
     *
     *  !!! IMPORTANT: does not update this->occupation_indices if required call this->updateOccupationIndices !!!
     */
    bool annihilate(size_t p);

    /**
     *  @return if we can apply the annihilation operator (i.e. 1->0) for the @param p-th spatial orbital
     *  Subsequently perform an in-place annihilation on the orbital @param p
     *
     *  Furthermore, the @param sign is changed according to the sign change (+1 or -1) of the spin string after annihilation.
     *
     *  !!! IMPORTANT: does not update this->occupation_indices if required call this->updateOccupationIndices !!!
     */
    bool annihilate(size_t p, int& sign);

    /**
     * @return if we can apply the creation operator (i.e. 0->1) for the @param p-th spatial orbital
     * Subsequently perform an in-place creation on the orbital @param p
     *
     *  !!! IMPORTANT: does not update this->occupation_indices if required call this->updateOccupationIndices !!!
     */
    bool create(size_t p);

    /**
     *  @return if we can apply the creation operator (i.e. 0->1) for the @param p-th spatial orbital
     *  Subsequently perform an in-place creation on the orbital @param p
     *
     *  Furthermore, the @param sign is changed according to the sign change (+1 or -1) of the spin string after annihilation.
     *
     *  !!! IMPORTANT: does not update this->occupation_indices if required call this->updateOccupationIndices !!!
     */
    bool create(size_t p, int& sign);

    /**
     *  @return the phase factor (+1 or -1) that arises by applying an annihilation or creation operator on orbital @param p, starting from 0 in reverse lexical ordering.
     *
     *  Let's say that there are m electrons in the orbitals up to p (not included). If m is even, the phase factor is (+1) and if m is odd, the phase factor is (-1), since electrons are fermions.
     */
    int operatorPhaseFactor(size_t p) const;


    /**
     *  @return the representation of a slice (i.e. a subset) of the spin string between @param index_start (included)
     *  and @param index_end (not included).
     *
     *  Both @param index_start and @param index_end are 'lexical' (i.e. from right to left), which means that the slice
     *  occurs 'lexically' as well (i.e. from right to left).
     *
     *      Example:
     *          "010011".slice(1, 4) => "01[001]1" -> "001"
     *
     */
    size_t slice(size_t index_start, size_t index_end) const;


    // FRIEND CLASSES
    friend class FockSpace;
};


}  // namespace GQCP

#endif  // GQCP_ONV_HPP
