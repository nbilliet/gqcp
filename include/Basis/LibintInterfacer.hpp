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
#ifndef GQCP_LIBINTINTERFACER_HPP
#define GQCP_LIBINTINTERFACER_HPP


#include "Basis/AOBasis.hpp"
#include "Molecule.hpp"
#include "Operator/OneElectronOperator.hpp"
#include "Operator/TwoElectronOperator.hpp"

#include <boost/preprocessor.hpp>  // include preprocessor before libint to fix libint-boost bug
#include <libint2.hpp>


namespace GQCP {


/**
 *  A singleton class that takes care of interfacing with the Libint2 (version 2.3.1) C++ API
 *
 *  Singleton class template from: https://stackoverflow.com/a/1008289
 */
class LibintInterfacer {
private:
    // PRIVATE METHODS - SINGLETON
    /**
     *  Private constructor as required by the singleton class design
     */
    LibintInterfacer();

    /**
     *  Private destructor as required by the singleton class design
     */
    ~LibintInterfacer();


    // PRIVATE STRUCTS
    typedef struct {} empty;  // empty_pod is a private typedef for libint2::Engine, so we copy it over


public:
    // PUBLIC METHODS - SINGLETON
    /**
     *  @return the static singleton instance
     */
    static LibintInterfacer& get();

    /**
     *  Remove the public copy constructor and the public assignment operator
     */
    LibintInterfacer(LibintInterfacer const& libint_communicator) = delete;
    void operator=(LibintInterfacer const& libint_communicator) = delete;


    // PUBLIC METHODS - INTERFACING (GQCP TO LIBINT)
    /**
     *  @param atom         the GQCP-atom that should be interfaced
     *
     *  @return a libint2::Atom, interfaced from the given GQCP::Atom
     */
    libint2::Atom interface(const Atom& atom) const;

    /**
     *  @param atoms        the GQCP-atoms that should be interfaced
     *
     *  @return libint2-atoms, interfaced from the given atoms
     */
    std::vector<libint2::Atom> interface(const std::vector<Atom>& atoms) const;

    /**
     *  @param shell        the GQCP shell that should be interfaced
     *
     *  @return a libint2::Shell whose renorm()alization has been undone, interfaced from the GQCP Shell
     */
    libint2::Shell interface(const Shell& shell) const;

    /**
     *  @param shellset     the GQCP ShellSet that should be interfaced
     *
     *  @return a libint2::BasisSet (whose underlying libint2::Shells have been re-renorm()alized), interfaced from the GQCP ShellSet. Note that it is not possible to create libint2-sp-shells from a GQCP ShellSet
     */
    libint2::BasisSet interface(const ShellSet& shellset) const;


    // PUBLIC METHODS - INTERFACING (LIBINT TO GQCP)
    /**
     *  Interface a libint2::Shell to the corresponding list of GQCP::Shells. Note that there is no one-to-one libint -> GQCP conversion, since GQCP does not support representing 'linked' sp-'shells'
     *
     *  @param libint_shell     the libint2 Shell that should be interfaced
     *  @param atoms            the atoms that can serve as centers of the Shells
     *  @param undo_renorm      if the libint2::Shell should be un-renorm()alized
     *
     *  @return a vector of GQCP::Shells corresponding to the given libint2::Shells
     */
    std::vector<Shell> interface(const libint2::Shell& libint_shell, const std::vector<Atom>& atoms, bool undo_renorm=true) const;

    /**
     *  Interface a libint2::BasisSet to the corresponding GQCP::ShellSet
     *
     *  @param libint_basisset      the libint2 Shell that should be interfaced
     *  @param atoms                the atoms that can serve as centers of the Shells
     *
     *  @return a GQCP::ShellSet corresponding to the libint2::BasisSet
     */
    ShellSet interface(const libint2::BasisSet& libint_basisset, const std::vector<Atom>& atoms) const;


    // PUBLIC METHODS - OTHER LIBINT2-RELATED FUNCTIONS
    /**
     *  @param libint_shell         the libint2::Shell
     *
     *  @return the number of true shells that are contained in the libint2::Shell
     */
    size_t numberOfShells(const libint2::Shell& libint_shell) const;

    /**
     *  @param libint_basisset      the libint2::BasisSet
     *
     *  @return the number of true shells that are contained in the libint2::BasisSet
     */
    size_t numberOfShells(const libint2::BasisSet& libint_basisset) const;

    /**
     *  Undo the libint2 default renormalization (see libint2::Shell::renorm())
     *
     *  @param libint_shell         the shell that should be un-renorm()alized
     */
    void undo_renorm(libint2::Shell& libint_shell) const;


    // PUBLIC METHODS - INTEGRALS
    /**
     *  @tparam N                   the number of operator components
     *
     *  @param operator_type        the name of the operator as specified by the enumeration
     *  @param libint_basisset      the libint2 basis set representing the AO basis
     *  @param parameters           the parameters for the integral engine, as specified by libint2
     *
     *  @return an array of N OneElectronOperators corresponding to the matrix representations of the N components of the given operator type
     */
    template <size_t N>
    std::array<OneElectronOperator<double>, N> calculateOneElectronIntegrals(libint2::Operator operator_type, const libint2::BasisSet& libint_basisset, const libint2::any& parameters = empty()) const {

        // Initialize the N components of the matrix representations of the operator
        const auto nbf = static_cast<size_t>(libint_basisset.nbf());  // number of basis functions
        std::array<OneElectronOperator<double>, N> operator_components;
        for (auto& op : operator_components) {
            op = OneElectronOperator<double>::Zero(nbf, nbf);
        }


        // Construct the libint2 engine
        libint2::Engine engine (operator_type, libint_basisset.max_nprim(), static_cast<int>(libint_basisset.max_l()));
        engine.set_params(parameters);

        const auto& shell2bf = libint_basisset.shell2bf();  // create a map between (shell index) -> (basis function index)

        const auto& calculated_integrals = engine.results();  // vector that holds pointers to computed shell sets
        assert(calculated_integrals.size() == N);             // its size is N, so it holds N pointers to the first computed integral of an integral set


        // One-electron integrals are between two basis functions, so we'll need two loops
        // Libint calculates integrals between libint2::Shells, so we will loop over the shells in the basisset
        const auto nsh = static_cast<size_t>(libint_basisset.size());  // number of shells
        for (auto sh1 = 0; sh1 != nsh; ++sh1) {  // shell 1
            for (auto sh2 = 0; sh2 != nsh; ++sh2) {  // shell 2
                // Calculate integrals between the two shells
                engine.compute(libint_basisset[sh1], libint_basisset[sh2]);  // this updates the pointers in calculated_integrals


                // Place the calculated integrals into the matrix representation(s): the integrals are stored in row-major form
                auto bf1 = shell2bf[sh1];  // (index of) first bf in sh1
                auto bf2 = shell2bf[sh2];  // (index of) first bf in sh2

                auto nbf_sh1 = libint_basisset[sh1].size();  // number of basis functions in first shell
                auto nbf_sh2 = libint_basisset[sh2].size();  // number of basis functions in second shell

                for (auto f1 = 0; f1 != nbf_sh1; ++f1) {  // f1: index of basis function within shell 1
                    for (auto f2 = 0; f2 != nbf_sh2; ++f2) { // f2: index of basis function within shell 2

                        for (size_t i = 0; i < N; i++) {
                            double computed_integral = calculated_integrals[i][f2 + f1 * nbf_sh2];  // integrals are packed in row-major form
                            operator_components[i](bf1 + f1, bf2 + f2) = computed_integral;
                        }

                    }
                }  // data access loops

            }
        }  // shell loops

        return operator_components;
    }


    /**
     *  @param operator_type        the name of the operator as specified by the enumeration
     *  @param libint_basisset      the libint2 basis set representing the AO basis
     *
     *  @return the matrix representation of a two-electron operator in the given AO basis
     */
    TwoElectronOperator<double> calculateTwoElectronIntegrals(libint2::Operator operator_type, const libint2::BasisSet& libint_basisset) const;
};


}  // namespace GQCP


#endif  // GQCP_LIBINTINTERFACER_HPP
