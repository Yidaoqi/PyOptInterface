#pragma once

#include <memory>

#include "solvers/clp/Clp_C_Interface.h"

#include "pyoptinterface/core.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/dylib.hpp"

// define CLP C APIs
#define APILIST					\
	B(Clp_Version);				\
	B(Clp_VersionMajor);		\
	B(Clp_VersionMinor);		\
	B(Clp_VersionRelease);		\
	B(Clp_newModel);			\
	B(Clp_deleteModel);         \
	B(ClpSolve_new);            \
	B(ClpSolve_delete);			\
	B(Clp_loadProblem);			\
	B(Clp_loadQuadraticObjective);		\
	B(Clp_readMps);             \
	B(Clp_writeMps);            \
	B(Clp_copyInIntegerInformation);	\
	B(Clp_deleteIntegerInformation);    \
	B(Clp_resize);				\
	B(Clp_deleteRows);			\
	B(Clp_addRows);				\
	B(Clp_deleteColumns);       \
	B(Clp_addColumns);          \
	B(Clp_chgRowLower);			\
	B(Clp_chgRowUpper);         \
	B(Clp_chgColumnLower);      \
	B(Clp_chgColumnUpper);      \
	B(Clp_chgObjCoefficients);  \
	B(Clp_modifyCoefficient);   \
	B(Clp_dropNames);			\
	B(Clp_copyNames);			\
	B(Clp_numberRows);			\
	B(Clp_numberColumns);       \
	B(Clp_primalTolerance);     \
	B(Clp_setPrimalTolerance);  \
	B(Clp_dualTolerance);       \
	B(Clp_setDualTolerance);    \
	B(Clp_dualObjectiveLimit);  \
	B(Clp_setDualObjectiveLimit);   \
	B(Clp_objectiveOffset);         \
	B(Clp_problemName);				\
	B(Clp_setProblemName);			\
	B(Clp_numberIterations);		\
	B(Clp_setNumberIterations);		\
	B(maximumIterations);			\
	B(Clp_setMaximumIterations);	\
	B(Clp_maximumSeconds);			\
	B(Clp_setMaximumSeconds);		\
	B(Clp_hitMaximumIterations);	\
	B(Clp_status);					\
	B(Clp_setProblemStatus);		\
	B(Clp_secondaryStatus);         \
	B(Clp_setSecondaryStatus);      \
	B(Clp_optimizationDirection);   \
	B(Clp_setOptimizationDirection);		\
	B(Clp_primalRowSolution);				\
	B(Clp_primalColumnSolution);            \
	B(Clp_dualRowSolution);					\
	B(Clp_dualColumnSolution);				\
	B(Clp_rowLower);						\
	B(Clp_rowUpper);						\
	B(Clp_objective);						\
	B(Clp_columnLower);						\
	B(Clp_columnUpper);						\
	B(Clp_getNumElements);					\
	B(Clp_getVectorStarts);					\
	B(Clp_getIndices);						\
	B(Clp_getVectorLengths);	\
	B(Clp_getElements);			\
	B(Clp_objectiveValue);		\
	B(Clp_integerInformation);	\
	B(Clp_infeasibilityRay);	\
	B(Clp_unboundedRay);		\
	B(Clp_freeRay);				\
	B(Clp_statusExists);		\
	B(Clp_statusArray);			\
	B(Clp_copyinStatus);		\
	B(Clp_getColumnStatus);		\
	B(Clp_getRowStatus);		\
	B(Clp_setColumnStatus);		\
	B(Clp_setRowStatus);		\
	B(Clp_setUserPointer);		\
	B(Clp_getUserPointer);		\
	B(Clp_registerCallBack);	\
	B(Clp_clearCallBack);		\
	B(Clp_setLogLevel);			\
	B(Clp_logLevel);			\
	B(Clp_lengthNames);			\
	B(Clp_rowName);				\
	B(Clp_columnName);			\
	B(Clp_setRowName);			\
	B(Clp_setColumnName);		\
	B(Clp_initialSolve);		\
	B(Clp_initialSolveWithOptions);	\
	B(Clp_initialDualSolve);		\
	B(Clp_initialPrimalSolve);		\
	B(Clp_initialBarrierSolve);		\
	B(Clp_initialBarrierNoCrossSolve);	\
	B(Clp_dual);						\
	B(Clp_primal);						\
	B(Clp_idiot);						\
	B(Clp_scaling);						\
	B(Clp_scalingFlag);					\
	B(Clp_crash);						\
	B(Clp_primalFeasible);				\
	B(Clp_dualFeasible);				\
	B(Clp_dualBound);					\
	B(Clp_setDualBound);				\
	B(Clp_infeasibilityCost);			\
	B(Clp_setInfeasibilityCost);		\
	B(Clp_perturbation);				\
	B(Clp_setPerturbation);				\
	B(Clp_algorithm);					\
	B(Clp_setAlgorithm);				\
	B(Clp_sumDualInfeasibilities);		\
	B(Clp_numberDualInfeasibilities);	\
	B(Clp_sumPrimalInfeasibilities);	\
	B(Clp_numberPrimalInfeasibilities);	\
	B(Clp_saveModel);					\
	B(Clp_restoreModel);				\
	B(Clp_checkSolution);				\
	B(Clp_getNumRows);					\
	B(Clp_getNumCols);					\
	B(Clp_getIterationCount);			\
	B(Clp_isAbandoned);					\
	B(Clp_isProvenOptimal);				\
	B(Clp_isProvenPrimalInfeasible);	\
	B(Clp_isProvenDualInfeasible);		\
	B(Clp_isPrimalObjectiveLimitReached);\
	B(Clp_isDualObjectiveLimitReached);	\
	B(Clp_isIterationLimitReached);		\
	B(Clp_getObjSense);					\
	B(Clp_setObjSense);					\
	B(Clp_getRowActivity);				\
	B(Clp_getColSolution);				\
	B(Clp_setColSolution);				\
	B(Clp_getRowPrice);					\
	B(Clp_getReducedCost);				\
	B(Clp_getRowLower);					\
	B(Clp_getRowUpper);					\
	B(Clp_getObjCoefficients);			\
	B(Clp_getColLower);					\
	B(Clp_getColUpper);					\
	B(Clp_getObjValue);					\
	B(Clp_printModel);					\
	B(Clp_getSmallElementValue);		\
	B(Clp_setSmallElementValue);		\
	B(ClpSolve_setSpecialOption);		\
	B(ClpSolve_getSpecialOption);		\
	B(ClpSolve_setSolveType);			\
	B(ClpSolve_getSolveType);			\
	B(ClpSolve_setPresolveType);		\
	B(ClpSolve_getPresolveType);		\
	B(ClpSolve_getPresolvePasses);		\
	B(ClpSolve_getExtraInfo);			\
	B(ClpSolve_setInfeasibleReturn);	\
	B(ClpSolve_infeasibleReturn);		\
	B(ClpSolve_doDual);					\
	B(ClpSolve_setDoDual);				\
	B(ClpSolve_doSingleton);			\
	B(ClpSolve_setDoSingleton);			\
	B(ClpSolve_doDoubleton);			\
	B(ClpSolve_setDoDoubleton);			\
	B(ClpSolve_doTripleton);			\
	B(ClpSolve_setDoTripleton);			\
	B(ClpSolve_doTighten);				\
	B(ClpSolve_setDoTighten);			\
	B(ClpSolve_doForcing);				\
	B(ClpSolve_setDoForcing);			\
	B(ClpSolve_doImpliedFree);			\
	B(ClpSolve_setDoImpliedFree);		\
	B(ClpSolve_doDupcol);				\
	B(ClpSolve_setDoDupcol);			\
	B(ClpSolve_doDuprow);				\
	B(ClpSolve_setDoDuprow);			\
	B(ClpSolve_doSingletonColumn);		\
	B(ClpSolve_setDoSingletonColumn);	\
	B(ClpSolve_presolveActions);		\
	B(ClpSolve_setPresolveActions);		\
	B(ClpSolve_substitution);			\
	B(ClpSolve_setSubstitution);

namespace clp
{
#define B DYLIB_EXTERN_DECLARE
APILIST
#undef B

bool is_library_loaded();

bool load_library(const std::string &path);
}

class ClpEnvConfig
{
  public:
	ClpEnvConfig();
	~ClpEnvConfig();

	void set(const char *param_name, const char *value);

  private:
	clp_env_config *m_config;

	friend class COPTEnv;
};

class ClpEnv
{
  public:
	ClpEnv();
	ClpEnv(ClpEnvConfig &config);
	~ClpEnv();

  private:
	clp_env *m_env;

	friend class ClpModel;
};

struct ClpfreemodelT
{
	void operator()(Clp_Simplex *model) const
	{
		clp::Clp_deleteModel(model);
	};
};

class ClpModel;
//using ClpCallback = std::function<void(ClpModel *, int)>;

struct ClpCallbackUserdata
{
	void *model = nullptr;
	clp_callback callback;
	int n_variables = 0;
	int where = 0;
	// store result of clp
	bool cb_get_mipsol_called = false;
	std::vector<double> mipsol;
	bool cb_get_mipnoderel_called = false;
	std::vector<double> mipnoderel;
	bool cb_get_mipincumbent_called = false;
	std::vector<double> mipincumbent;
	// Cache for cbsolution
	bool cb_set_solution_called = false;
	std::vector<double> heuristic_solution;
	bool cb_requires_submit_solution = false;
};


class ClpModel
{
  public:
	ClpModel();

	void init();

	void write(const std::string &filename);

	VariableIndex add_variable(VariableDomain domain = VariableDomain::Continuous,
	                           double lb = -COIN_DBL_MAX, double ub = COIN_DBL_MAX,
	                           char *name = nullptr);
	void delete_variable(const VariableIndex &variable);
	void delete_variables(const Vector<VariableIndex> &variables);

	bool is_variable_active(const VariableIndex &variable);
	double get_variable_value(const VariableIndex &variable);
	std::string pprint_variable(const VariableIndex &variable);
	void set_variable_bounds(const VariableIndex &variable, double lb, double ub);

	ConstraintIndex add_linear_constraint(const ScalarAffineFunction &function,
	                                      ConstraintSense sense, CoeffT rhs,
	                                      char *name = nullptr);
	//ConstraintIndex add_quadratic_constraint(const ScalarQuadraticFunction &function,
	//                                         ConstraintSense sense, CoeffT rhs,
	//                                         const char *name = nullptr);

	void delete_constraint(const ConstraintIndex &constraint);
	bool is_constraint_active(const ConstraintIndex &constraint);

	void set_objective(const ScalarAffineFunction &function, ObjectiveSense sense);
	void set_objective(const ScalarQuadraticFunction &function, ObjectiveSense sense);
	void set_objective(const ExprBuilder &function, ObjectiveSense sense);

	void optimize();
	void *get_raw_model();
	std::string version_string();

	/*
	 * Returns the type of a clp parameter or attribute, given its name.
	 * -1: unknown
	 *  0: double parameter
	 *  1: int parameter
	 *  2: double attribute
	 *  3: int attribute
	 *
	 */
	int raw_parameter_attribute_type(const char *name);

	// parameter
	void set_raw_parameter_int(const char *param_name, int value);
	void set_raw_parameter_double(const char *param_name, double value);
	int get_raw_parameter_int(const char *param_name);
	double get_raw_parameter_double(const char *param_name);

	// attribute
	int get_raw_attribute_int(const char *attr_name);
	double get_raw_attribute_double(const char *attr_name);

	// Accessing information of problem
	double get_variable_info(const VariableIndex &variable, const char *info_name);
	std::string get_variable_name(const VariableIndex &variable);
	void set_variable_name(const VariableIndex &variable, char *name);
	VariableDomain get_variable_type(const VariableIndex &variable);
	void set_variable_type(const VariableIndex &variable, VariableDomain domain);
	void set_variable_lower_bound(const VariableIndex &variable, double lb);
	void set_variable_upper_bound(const VariableIndex &variable, double ub);

	double get_constraint_info(const ConstraintIndex &constraint, const char *info_name);
	std::string get_constraint_name(const ConstraintIndex &constraint);
	void set_constraint_name(const ConstraintIndex &constraint, char *name);

	void set_obj_sense(ObjectiveSense sense);

	// Modifications of model
	// 1. set/get RHS of a constraint
	double get_normalized_rhs(const ConstraintIndex &constraint);
	void set_normalized_rhs(const ConstraintIndex &constraint, double value);
	// 2. set/get coefficient of variable in constraint
	double get_normalized_coefficient(const ConstraintIndex &constraint,
	                                  const VariableIndex &variable);
	void set_normalized_coefficient(const ConstraintIndex &constraint,
	                                const VariableIndex &variable, double value);
	// 3. set/get linear coefficient of variable in objective
	double get_objective_coefficient(const VariableIndex &variable);
	void set_objective_coefficient(const VariableIndex &variable, double value);

	int _variable_index(const VariableIndex &variable);
	int _checked_variable_index(const VariableIndex &variable);
	int _constraint_index(const ConstraintIndex &constraint);
	int _checked_constraint_index(const ConstraintIndex &constraint);

	// Callback
	void set_callback(const clp_callback &callback, int cbctx);

	// For callback
	bool has_callback = false;
	void *m_cbdata = nullptr;
	ClpCallbackUserdata m_callback_userdata;

	int cb_get_info_int(const std::string &what);
	double cb_get_info_double(const std::string &what);
	void cb_get_info_doublearray(const std::string &what);

	double cb_get_solution(const VariableIndex &variable);
	double cb_get_relaxation(const VariableIndex &variable);
	double cb_get_incumbent(const VariableIndex &variable);
	void cb_set_solution(const VariableIndex &variable, double value);
	double cb_submit_solution();

	void cb_exit();

	void cb_add_lazy_constraint(const ScalarAffineFunction &function, ConstraintSense sense,
	                            CoeffT rhs);
	void cb_add_lazy_constraint(const ExprBuilder &function, ConstraintSense sense, CoeffT rhs);
	void cb_add_user_cut(const ScalarAffineFunction &function, ConstraintSense sense, CoeffT rhs);
	void cb_add_user_cut(const ExprBuilder &function, ConstraintSense sense, CoeffT rhs);


  private:
	MonotoneIndexer<int> m_variable_index;

	MonotoneIndexer<int> m_linear_constraint_index;

	MonotoneIndexer<int> m_quadratic_constraint_index;

	MonotoneIndexer<int> m_sos_constraint_index;

	MonotoneIndexer<int> m_cone_constraint_index;

	MonotoneIndexer<int> m_exp_cone_constraint_index;

	std::unique_ptr<Clp_Simplex, ClpfreemodelT> m_model;
};