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
#include "LibintInterfacer.hpp"

#include <iostream>
#include <sstream>



namespace GQCP {


/*
 *  PRIVATE METHODS - SINGLETON
 */

/**
 *  Private constructor as required by the singleton class design
 */
LibintInterfacer::LibintInterfacer() {
    libint2::initialize();
}


/**
 *  Private destructor as required by the singleton class design
 */
LibintInterfacer::~LibintInterfacer() {
    libint2::finalize();
}


/*
 *  PUBLIC METHODS - SINGLETON
 */

/**
 *  @return the static singleton instance
 */
LibintInterfacer& LibintInterfacer::get() {  // need to return by reference since we deleted the relevant constructor
    static LibintInterfacer singleton_instance;  // instantiated on first use and guaranteed to be destroyed
    return singleton_instance;
}


/*
 *  PUBLIC METHODS - INTERFACING (GQCP TO LIBINT)
 */

/**
 *  @param atom         the GQCP-atom that should be interfaced
 *
 *  @return a libint2::Atom, interfaced from the given GQCP::Atom
 */
libint2::Atom LibintInterfacer::interface(const Atom& atom) const {

    libint2::Atom libint2_atom {static_cast<int>(atom.atomic_number), atom.position.x(), atom.position.y(), atom.position.z()};

    return libint2_atom;
}


/**
 *  @param atoms        the GQCP-atoms that should be interfaced
 *
 *  @return libint2-atoms, interfaced from the given atoms
 */
std::vector<libint2::Atom> LibintInterfacer::interface(const std::vector<Atom>& atoms) const {

    std::vector<libint2::Atom> libint_vector;  // start with an empty vector, we're doing push_backs later
    libint_vector.reserve(atoms.size());

    for (const auto& atom : atoms) {
        libint_vector.push_back(this->interface(atom));
    }

    return libint_vector;
}


/**
 *  @param shell        the GQCP shell that should be interfaced
 *
 *  @return a libint2::Shell, interfaced from the GQCP Shell
 */
libint2::Shell LibintInterfacer::interface(const Shell& shell) const {

    // Part 1: exponents
    std::vector<double> libint_alpha = shell.get_gaussian_exponents();  // libint::Shell::real_t is double, so no need to use real_t


    // Part 2: contractions
    auto libint_l = static_cast<int>(shell.get_l());
    bool libint_pure = false;  // our shells are Cartesian
    std::vector<double> libint_coeff = shell.get_contraction_coefficients();
    libint2::Shell::Contraction libint_contraction {libint_l, libint_pure, libint_coeff};

    std::vector<libint2::Shell::Contraction> libint_contr {libint_contraction};


    // Part 3: origin
    auto position = shell.get_atom().position;
    std::array<double, 3> libint_O {position.x(), position.y(), position.z()};

    return libint2::Shell(libint_alpha, libint_contr, libint_O);
}


/**
 *  @param shellset     the GQCP ShellSet that should be interfaced
 *
 *  @return a libint2::BasisSet, interfaced from the GQCP ShellSet
 */
libint2::BasisSet LibintInterfacer::interface(const ShellSet& shellset) const {

    libint2::BasisSet libint2_basisset;  // start with an empty vector, we're doing push_backs later
    libint2_basisset.reserve(shellset.size());


    for (const auto& shell : shellset) {
        libint2_basisset.push_back(this->interface(shell));
    }


    // At this point in the code, the libint2::BasisSet is 'uninitialized', i.e. its private member _nbf is -1, etc.
    // Therefore, we are using a hack to force the private libint2::BasisSet::init() being called
    libint2_basisset.set_pure(false);  // the shells inside GQCP::BasisSet are all Cartesian, so this effectively does nothing

    return libint2_basisset;
}


/*
 *  PUBLIC METHODS - INTERFACING (LIBINT TO GQCP)
 */

/**
 *  @param libint_shell         the libint2::Shell
 *
 *  @return the number of true shells that are contained in the libint shell
 */
size_t LibintInterfacer::numberOfShells(const libint2::Shell& libint_shell) const {
    return libint_shell.ncontr();
}


/**
 *  @param libint_basisset      the libint2::BasisSet
 *
 *  @return the number of true shells that are contained in the libint2::BasisSet
 */
size_t LibintInterfacer::numberOfShells(const libint2::BasisSet& libint_basisset) const {

    size_t nsh {};  // number of shells

    for (const auto& libint_shell : libint_basisset) {
        nsh += this->numberOfShells(libint_shell);
    }

    return nsh;
}


/**
 *  Interface a libint2::Shell to the corresponding list of GQCP::Shells. Note that there is no one-to-one libint -> GQCP conversion, since GQCP does not support 'linked' sp-'shells'
 *
 *  @param libint_shell     the libint2 Shell that should be interfaced
 *  @param atoms            the atoms that can serve as centers of the Shells
 *
 *  @return a vector of GQCP::Shells
 */
std::vector<Shell> LibintInterfacer::interface(const libint2::Shell& libint_shell, const std::vector<Atom>& atoms) const {

    std::vector<double> exponents = libint_shell.alpha;

    std::vector<Shell> shells;
    shells.reserve(this->numberOfShells(libint_shell));
    for (const auto& libint_contraction : libint_shell.contr) {

        // Angular momentum and coefficients
        size_t l = libint_contraction.l;
        std::vector<double> coefficients = libint_contraction.coeff;

        // Libint2 only stores the origin of the shell, so we have to find the atom corresponding to the copied shell's origin
        Atom corresponding_atom;
        for (const Atom& atom : atoms) {
            Eigen::Map<const Eigen::Matrix<double, 3, 1>> libint_origin_map (libint_shell.O.data());  // convert raw array data to Eigen
            if (atom.position.isApprox(libint_origin_map, 1.0e-06)) {  // tolerant comparison
                corresponding_atom = atom;
                break;
            }
        }

        if (corresponding_atom.atomic_number == 0) {
            throw std::invalid_argument("LibintInterfacer::interface(libint2::Shell, std::vector<Atom>): No given atom matches the center of the libint2::Shell");
        }

        shells.emplace_back(l, corresponding_atom, exponents, coefficients);
    }

    return shells;
}


/**
 *  Interface a libint2::BasisSet to the corresponding GQCP::ShellSet
 *
 *  @param libint_basisset      the libint2 Shell that should be interfaced
 *  @param atoms                the atoms that can serve as centers of the Shells
 *
 *  @return a GQCP::ShellSet corresponding to the libint2::BasisSet
 */
ShellSet LibintInterfacer::interface(const libint2::BasisSet& libint_basisset, const std::vector<Atom>& atoms) const {

    ShellSet shell_set;
    shell_set.reserve(this->numberOfShells(libint_basisset));
    for (const auto& libint_shell : libint_basisset) {
        for (const auto& shell : this->interface(libint_shell, atoms)) {
            shell_set.push_back(shell);
        }
    }

    return shell_set;
}


/*
 *  PUBLIC METHODS - INTEGRALS
 */

/**
 *  @param operator_type        the name of the operator as specified by the enumeration
 *  @param libint_basisset      the libint2 basis set representing the AO basis
 *
 *  @return the matrix representation of a two-electron operator in the given AO basis
 */
TwoElectronOperator<double> LibintInterfacer::calculateTwoElectronIntegrals(libint2::Operator operator_type, const libint2::BasisSet& libint_basisset) const {

    const auto nbf = static_cast<size_t>(libint_basisset.nbf());  // nbf: number of basis functions in the basisset

    // Initialize the rank-4 two-electron integrals tensor and set to zero
    TwoElectronOperator<double> g (nbf);
    g.setZero();


    // Construct the libint2 engine
    libint2::Engine engine(libint2::Operator::coulomb, libint_basisset.max_nprim(), static_cast<int>(libint_basisset.max_l()));  // libint2 requires an int

    const auto shell2bf = libint_basisset.shell2bf();  // maps shell index to bf index

    const auto &buffer = engine.results();  // vector that holds pointers to computed shell sets
    // actually, buffer.size() is always 1, so buffer[0] is a pointer to
    //      the first calculated integral of these specific shells
    // the values that buffer[0] points to will change after every compute() call


    // Two-electron integrals are between four basis functions, so we'll need four loops
    // Libint calculates integrals between libint2::Shells, so we will loop over the shells (sh) in the basisset
    const auto nsh = static_cast<size_t>(libint_basisset.size());  // nsh: number of shells in the basisset
    for (auto sh1 = 0; sh1 != nsh; ++sh1) {  // sh1: shell 1
        for (auto sh2 = 0; sh2 != nsh; ++sh2) {  // sh2: shell 2
            for (auto sh3 = 0; sh3 != nsh; ++sh3) {  // sh3: shell 3
                for (auto sh4 = 0; sh4 != nsh; ++sh4) {  //sh4: shell 4
                    // Calculate integrals between the two shells (obs is a decorated std::vector<libint2::Shell>)
                    engine.compute(libint_basisset[sh1], libint_basisset[sh2], libint_basisset[sh3], libint_basisset[sh4]);

                    auto calculated_integrals = buffer[0];

                    if (calculated_integrals == nullptr) {  // if the zeroth element is nullptr, then the whole shell has been exhausted
                        // or the libint engine predicts that the integrals are below a certain threshold
                        // in this case the value does not need to be filled in, and we are safe because we have properly initialized to zero
                        continue;
                    }

                    // Extract the calculated integrals from calculated_integrals.
                    // In calculated_integrals, the integrals are stored in row major form.
                    auto bf1 = static_cast<long>(shell2bf[sh1]);  // (index of) first bf in sh1
                    auto bf2 = static_cast<long>(shell2bf[sh2]);  // (index of) first bf in sh2
                    auto bf3 = static_cast<long>(shell2bf[sh3]);  // (index of) first bf in sh3
                    auto bf4 = static_cast<long>(shell2bf[sh4]);  // (index of) first bf in sh4


                    auto nbf_sh1 = static_cast<long>(libint_basisset[sh1].size());  // number of basis functions in first shell
                    auto nbf_sh2 = static_cast<long>(libint_basisset[sh2].size());  // number of basis functions in second shell
                    auto nbf_sh3 = static_cast<long>(libint_basisset[sh3].size());  // number of basis functions in third shell
                    auto nbf_sh4 = static_cast<long>(libint_basisset[sh4].size());  // number of basis functions in fourth shell

                    for (auto f1 = 0L; f1 != nbf_sh1; ++f1) {
                        for (auto f2 = 0L; f2 != nbf_sh2; ++f2) {
                            for (auto f3 = 0L; f3 != nbf_sh3; ++f3) {
                                for (auto f4 = 0L; f4 != nbf_sh4; ++f4) {
                                    auto computed_integral = calculated_integrals[f4 + nbf_sh4 * (f3 + nbf_sh3 * (f2 + nbf_sh2 * (f1)))];  // integrals are packed in row-major form

                                    // Two-electron integrals are given in CHEMIST'S notation: (11|22)
                                    g(f1 + bf1, f2 + bf2, f3 + bf3, f4 + bf4) = computed_integral;
                                }
                            }
                        }
                    } // data access loops

                }
            }
        }
    } // shell loops

    return g;
};


}  // namespace GQCP
