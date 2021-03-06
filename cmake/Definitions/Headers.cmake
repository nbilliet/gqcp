# List all library headers (.hpp)


# Find the header folder
set(PROJECT_INCLUDE_FOLDER ${CMAKE_SOURCE_DIR}/include)


# Find the header files
set(PROJECT_INCLUDE_FILES
        ${PROJECT_INCLUDE_FOLDER}/Basis/AOBasis.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/CartesianDirection.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/CartesianExponents.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/CartesianGTO.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/LibintInterfacer.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/Shell.hpp
        ${PROJECT_INCLUDE_FOLDER}/Basis/ShellSet.hpp

        ${PROJECT_INCLUDE_FOLDER}/CISolver/CISolver.hpp

        ${PROJECT_INCLUDE_FOLDER}/FockSpace/BaseFockSpace.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/Configuration.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/FockPermutator.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/FockSpace.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/FrozenFockSpace.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/FrozenProductFockSpace.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/FockSpaceType.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/ONV.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/SelectedFockSpace.hpp
        ${PROJECT_INCLUDE_FOLDER}/FockSpace/ProductFockSpace.hpp

        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roG.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roGBivariationalSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roGGeminalCoefficients.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roGJacobiOrbitalOptimizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roGPSESolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/AP1roGVariables.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/APIGGeminalCoefficients.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/BaseAP1roGSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/BaseAPIGVariables.hpp
        ${PROJECT_INCLUDE_FOLDER}/geminals/GeminalCoefficientsInterface.hpp

        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/DOCI.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/FCI.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/FrozenCoreCI.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/FrozenCoreDOCI.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/FrozenCoreFCI.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/HamiltonianBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/Hubbard.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianBuilder/SelectedCI.hpp

        ${PROJECT_INCLUDE_FOLDER}/HamiltonianParameters/BaseHamiltonianParameters.hpp
        ${PROJECT_INCLUDE_FOLDER}/HamiltonianParameters/HamiltonianParameters.hpp

        ${PROJECT_INCLUDE_FOLDER}/Localization/BaseERLocalizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/Localization/ERJacobiLocalizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/Localization/ERNewtonLocalizer.hpp

        ${PROJECT_INCLUDE_FOLDER}/math/optimization/BaseEigenproblemSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/BaseMatrixSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/BaseMinimizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/BaseSystemOfEquationsSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/DavidsonSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/DenseSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/Eigenpair.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/EigenproblemSolverOptions.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/NewtonMinimizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/NewtonSystemOfEquationsSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/SparseSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/optimization/step.hpp

        ${PROJECT_INCLUDE_FOLDER}/math/LinearCombination.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/Matrix.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/ScalarFunction.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/SquareMatrix.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/SquareRankFourTensor.hpp
        ${PROJECT_INCLUDE_FOLDER}/math/Tensor.hpp

        ${PROJECT_INCLUDE_FOLDER}/Operator/OneElectronOperator.hpp
        ${PROJECT_INCLUDE_FOLDER}/Operator/Operator.hpp
        ${PROJECT_INCLUDE_FOLDER}/Operator/TwoElectronOperator.hpp

        ${PROJECT_INCLUDE_FOLDER}/properties/expectation_values.hpp
        ${PROJECT_INCLUDE_FOLDER}/properties/properties.hpp

        ${PROJECT_INCLUDE_FOLDER}/RDM/BaseRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/BaseSpinUnresolvedRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/DOCIRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/FCIRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/FrozenCoreDOCIRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/FrozenCoreFCIRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/FrozenCoreRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/OneRDM.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/RDMCalculator.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/RDMs.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/SelectedRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/SpinUnresolvedFCIRDMBuilder.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/SpinUnresolvedRDMCalculator.hpp
        ${PROJECT_INCLUDE_FOLDER}/RDM/TwoRDM.hpp

        ${PROJECT_INCLUDE_FOLDER}/RHF/DIISRHFSCFSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/RHF/PlainRHFSCFSolver.hpp
        ${PROJECT_INCLUDE_FOLDER}/RHF/RHF.hpp
        ${PROJECT_INCLUDE_FOLDER}/RHF/RHFSCFSolver.hpp

        ${PROJECT_INCLUDE_FOLDER}/utilities/linalg.hpp
        ${PROJECT_INCLUDE_FOLDER}/utilities/miscellaneous.hpp

        ${PROJECT_INCLUDE_FOLDER}/WaveFunction/SpinUnresolvedWaveFunction.hpp
        ${PROJECT_INCLUDE_FOLDER}/WaveFunction/WaveFunction.hpp

        ${PROJECT_INCLUDE_FOLDER}/Atom.hpp
        ${PROJECT_INCLUDE_FOLDER}/DOCINewtonOrbitalOptimizer.hpp
        ${PROJECT_INCLUDE_FOLDER}/elements.hpp
        ${PROJECT_INCLUDE_FOLDER}/HoppingMatrix.hpp
        ${PROJECT_INCLUDE_FOLDER}/JacobiRotationParameters.hpp
        ${PROJECT_INCLUDE_FOLDER}/Molecule.hpp
        ${PROJECT_INCLUDE_FOLDER}/RMP2.hpp
        ${PROJECT_INCLUDE_FOLDER}/typedefs.hpp
        ${PROJECT_INCLUDE_FOLDER}/units.hpp
    )
