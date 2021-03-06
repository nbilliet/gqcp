# List all library sources (.cpp)


# Find the source folder
set(PROJECT_SOURCE_FOLDER ${CMAKE_SOURCE_DIR}/src)


# Find the source files
set(PROJECT_SOURCE_FILES
        ${PROJECT_SOURCE_FOLDER}/Basis/AOBasis.cpp
        ${PROJECT_SOURCE_FOLDER}/Basis/CartesianExponents.cpp
        ${PROJECT_SOURCE_FOLDER}/Basis/CartesianGTO.cpp
        ${PROJECT_SOURCE_FOLDER}/Basis/LibintInterfacer.cpp
        ${PROJECT_SOURCE_FOLDER}/Basis/Shell.cpp
        ${PROJECT_SOURCE_FOLDER}/Basis/ShellSet.cpp

        ${PROJECT_SOURCE_FOLDER}/CISolver/CISolver.cpp

        ${PROJECT_SOURCE_FOLDER}/FockSpace/BaseFockSpace.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/FockSpace.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/FrozenFockSpace.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/FrozenProductFockSpace.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/ONV.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/ProductFockSpace.cpp
        ${PROJECT_SOURCE_FOLDER}/FockSpace/SelectedFockSpace.cpp

        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roG.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roGBivariationalSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roGGeminalCoefficients.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roGJacobiOrbitalOptimizer.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roGPSESolver.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/AP1roGVariables.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/APIGGeminalCoefficients.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/BaseAP1roGSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/BaseAPIGVariables.cpp
        ${PROJECT_SOURCE_FOLDER}/geminals/GeminalCoefficientsInterface.cpp

        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/DOCI.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/FCI.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/FrozenCoreCI.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/FrozenCoreDOCI.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/FrozenCoreFCI.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/HamiltonianBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/Hubbard.cpp
        ${PROJECT_SOURCE_FOLDER}/HamiltonianBuilder/SelectedCI.cpp

        ${PROJECT_SOURCE_FOLDER}/HamiltonianParameters/BaseHamiltonianParameters.cpp

        ${PROJECT_SOURCE_FOLDER}/Localization/BaseERLocalizer.cpp
        ${PROJECT_SOURCE_FOLDER}/Localization/ERJacobiLocalizer.cpp
        ${PROJECT_SOURCE_FOLDER}/Localization/ERNewtonLocalizer.cpp

        ${PROJECT_SOURCE_FOLDER}/math/optimization/BaseEigenproblemSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/BaseMatrixSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/BaseMinimizer.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/BaseSystemOfEquationsSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/DavidsonSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/DenseSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/Eigenpair.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/NewtonMinimizer.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/NewtonSystemOfEquationsSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/SparseSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/math/optimization/step.cpp

        ${PROJECT_SOURCE_FOLDER}/properties/expectation_values.cpp
        ${PROJECT_SOURCE_FOLDER}/properties/properties.cpp

        ${PROJECT_SOURCE_FOLDER}/RDM/BaseRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/BaseSpinUnresolvedRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/DOCIRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/FCIRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/FrozenCoreDOCIRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/FrozenCoreFCIRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/FrozenCoreRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/RDMCalculator.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/SelectedRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/SpinUnresolvedFCIRDMBuilder.cpp
        ${PROJECT_SOURCE_FOLDER}/RDM/SpinUnresolvedRDMCalculator.cpp

        ${PROJECT_SOURCE_FOLDER}/RHF/DIISRHFSCFSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/RHF/PlainRHFSCFSolver.cpp
        ${PROJECT_SOURCE_FOLDER}/RHF/RHF.cpp
        ${PROJECT_SOURCE_FOLDER}/RHF/RHFSCFSolver.cpp

        ${PROJECT_SOURCE_FOLDER}/utilities/linalg.cpp
        ${PROJECT_SOURCE_FOLDER}/utilities/miscellaneous.cpp

        ${PROJECT_SOURCE_FOLDER}/WaveFunction/SpinUnresolvedWaveFunction.cpp
        ${PROJECT_SOURCE_FOLDER}/WaveFunction/WaveFunction.cpp
        ${PROJECT_SOURCE_FOLDER}/WaveFunction/WaveFunctionReader.cpp

        ${PROJECT_SOURCE_FOLDER}/Atom.cpp
        ${PROJECT_SOURCE_FOLDER}/DOCINewtonOrbitalOptimizer.cpp
        ${PROJECT_SOURCE_FOLDER}/elements.cpp
        ${PROJECT_SOURCE_FOLDER}/HoppingMatrix.cpp
        ${PROJECT_SOURCE_FOLDER}/JacobiRotationParameters.cpp
        ${PROJECT_SOURCE_FOLDER}/Molecule.cpp
        ${PROJECT_SOURCE_FOLDER}/RMP2.cpp
    )
