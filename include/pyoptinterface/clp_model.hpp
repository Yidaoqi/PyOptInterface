#pragma once

#include <memory>

#include "solvers/clp/Clp_C_Interface.h"

#include "pyoptinterface/core.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/dylib.hpp"

// define CLP C APIs
#define APILIST               \
	B(Clp_Version);			\
	B(Clp_VersionMajor);		\
	B(Clp_VersionMinor);		\
	B(Clp_VersionRelease);	\
	B(Clp_newModel);           \
	B(Clp_deleteModel);          \
	B(ClpSolve_new);             \
	B(ClpSolve_delete);		\
	B(Clp_loadProblem);		\
	B(Clp_loadQuadraticObjective);              \
	B(Clp_readMps);             \
	B(Clp_writeMps);            \
	B(Clp_copyInIntegerInformation);          \
	B(Clp_deleteIntegerInformation);         \
	B(Clp_resize);             \
	B(Clp_deleteRows);         \
	B(Clp_addRows);        \
	B(Clp_deleteColumns);             \
	B(Clp_addColumns);               \
	B(Clp_chgRowLower);    \
	B(Clp_chgRowUpper);         \
	B(Clp_chgColumnLower);           \
	B(Clp_chgColumnUpper);        \
	B(Clp_chgObjCoefficients);       \
	B(Clp_modifyCoefficient);        \
	B(Clp_dropNames);        \
	B(Clp_copyNames);        \
	B(Clp_numberRows);        \
	B(Clp_numberColumns);        \
	B(Clp_primalTolerance);        \
	B(Clp_setPrimalTolerance);        \
	B(Clp_dualTolerance);         \
	B(Clp_setDualTolerance);         \
	B(Clp_dualObjectiveLimit);         \
	B(Clp_setDualObjectiveLimit);         \
	B(Clp_objectiveOffset);         \
	B(Clp_problemName);         \
	B(Clp_setProblemName);    \
	B(Clp_numberIterations);     \
	B(Clp_setNumberIterations);  \
	B(maximumIterations); \
	B(Clp_setMaximumIterations);  \
	B(Clp_maximumSeconds);  \
	B(Clp_setMaximumSeconds);  \
	B(Clp_hitMaximumIterations); \
	B(Clp_status);  \
	B(Clp_setProblemStatus);  \
	B(Clp_secondaryStatus);           \
	B(Clp_setSecondaryStatus);          \
	B(Clp_optimizationDirection);        \
	B(Clp_setOptimizationDirection);            \
	B(Clp_primalRowSolution);    \
	B(Clp_primalColumnSolution);              \
	B(Clp_dualRowSolution);          \
	B(Clp_dualColumnSolution);          \
	B(Clp_rowLower);         \
	B(Clp_rowUpper);             \
	B(Clp_objective);              \
	B(Clp_columnLower);           \
	B(Clp_columnUpper);            \
	B(Clp_getNumElements);            \
	B(Clp_getVectorStarts);           \
	B(Clp_getIndices);     \
	B(Clp_getVectorLengths);
