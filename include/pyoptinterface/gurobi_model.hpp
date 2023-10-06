#include <memory>

#include "gurobi_c.h"

#include "pyoptinterface/core.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/abstract_model.hpp"

class GurobiEnv
{
  public:
	GurobiEnv();
	~GurobiEnv();

  private:
	GRBenv *m_env;

	friend class GurobiModel;
};

struct GRBfreemodelT
{
	void operator()(GRBmodel *model) const
	{
		GRBfreemodel(model);
	};
};

class GurobiModel : AbstractModel
{
  public:
	GurobiModel() = default;
	GurobiModel(const GurobiEnv &env);
	void init(const GurobiEnv &env);

	VariableIndex add_variable(VariableDomain domain = VariableDomain::Continuous);
	void delete_variable(const VariableIndex &variable);
	bool is_variable_active(const VariableIndex &variable) const;

	ConstraintIndex add_linear_constraint(const ScalarAffineFunction function,
	                                      ConstraintSense sense, CoeffT rhs);
	ConstraintIndex add_quadratic_constraint(const ScalarQuadraticFunction function,
	                                         ConstraintSense sense, CoeffT rhs);
	ConstraintIndex add_sos1_constraint(const Vector<VariableIndex> &variables,
	                                    const Vector<CoeffT> &weights);
	ConstraintIndex add_sos2_constraint(const Vector<VariableIndex> &variables,
	                                    const Vector<CoeffT> &weights);
	ConstraintIndex _add_sos_constraint(const Vector<VariableIndex> &variables,
	                                    const Vector<CoeffT> &weights, int sos_type);

	void delete_constraint(const ConstraintIndex &constraint);
	bool is_constraint_active(const ConstraintIndex &constraint) const;

	void set_objective(const ScalarAffineFunction &function, ObjectiveSense sense);
	void set_objective(const ScalarQuadraticFunction &function, ObjectiveSense sense);

	void optimize();

	void update();

	void set_variable_raw_attribute_int(const VariableIndex &variable, const char *attr_name,
	                                    int value);
	void set_variable_raw_attribute_char(const VariableIndex &variable, const char *attr_name,
	                                     char value);
	void set_variable_raw_attribute_double(const VariableIndex &variable, const char *attr_name,
	                                       double value);
	void set_variable_raw_attribute_string(const VariableIndex &variable, const char *attr_name,
	                                       const char *value);
	int get_variable_raw_attribute_int(const VariableIndex &variable, const char *attr_name);
	char get_variable_raw_attribute_char(const VariableIndex &variable, const char *attr_name);
	double get_variable_raw_attribute_double(const VariableIndex &variable, const char *attr_name);
	char *get_variable_raw_attribute_string(const VariableIndex &variable, const char *attr_name);

  private:
	MonotoneVector<int> m_variable_index;

	MonotoneVector<int> m_linear_constraint_index;

	MonotoneVector<int> m_quadratic_constraint_index;

	MonotoneVector<int> m_sos_constraint_index;

	/* Gurobi part */
	GRBenv *m_env;
	std::unique_ptr<GRBmodel, GRBfreemodelT> m_model;
};