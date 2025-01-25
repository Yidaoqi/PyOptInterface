#include "pyoptinterface/clp_model.hpp"
#include "fmt/core.h"


namespace clp
{
#define B DYLIB_DECLARE
APILIST
#undef B

static DynamicLibrary lib;
static bool is_loaded = false;

bool is_library_loaded()
{
	return is_loaded;
}

bool load_library(const std::string &path)
{
	bool success = lib.try_load(path.c_str());
	if (!success)
	{
		return false;
	}

	DYLIB_LOAD_INIT;

#define B DYLIB_LOAD_FUNCTION
	APILIST
#undef B

	// We have to deal with Clp 1.17.0 where some functions are absent from the dynamic library
	{
		int major, minor;
		auto Clp_versionmajor_p =
		    reinterpret_cast<decltype(Clp_VersionMajor)>(_function_pointers["Clp_VersionMajor"]);
		auto Clp_versionminor_p =
		    reinterpret_cast<decltype(Clp_VersionMinor)>(_function_pointers["Clp_VersionMinor"]);
		
		if (Clp_versionmajor_p != nullptr && Clp_versionminor_p)
		{
			major = Clp_versionmajor_p();
			minor = Clp_versionminor_p();
			if (major == 1 && minor == 17)
			{
			
				// Now check there is no nullptr in _function_pointers
				_load_success = true;
				for (auto &pair : _function_pointers)
				{
					if (pair.second == nullptr)
					{
						fmt::print("function {} is not loaded correctly\n", pair.first);
						_load_success = false;
					}
				}

				if (_load_success)
				{
					/*DYLIB_SAVE_FUNCTION(GRBloadenvinternal);
					DYLIB_SAVE_FUNCTION(GRBemptyenvinternal);*/
				}
			}
		}
	}

	if (IS_DYLIB_LOAD_SUCCESS)
	{
#define B DYLIB_SAVE_FUNCTION
		APILIST
#undef B
		is_loaded = true;
		return true;
	}
	else
	{
		return false;
	}
}
}  // namespace clp

ClpModel::ClpModel()
{
	init();
}

void ClpModel::init()
{

	if (!clp::is_library_loaded())
	{
		throw std::runtime_error("Clp library is not loaded");
	}
	Clp_Simplex *model = clp::Clp_newModel();
	m_model = std::unique_ptr<Clp_Simplex, ClpfreemodelT>(model);
}

void ClpModel::write(const std::string &filename)
{
	int error = -1;
	if (filename.ends_with(".mps"))
	{
		error = clp::Clp_writeMps(m_model.get(), filename.c_str(), 0, 1, 1);
	}
	check_error(error);
}

VariableIndex ClpModel::add_variable(VariableDomain domain, double lb, double ub, char *name)
{
	if (name != nullptr && name[0] == '\0')
	{
		name = nullptr;
	}
	IndexT index = m_variable_index.add_index();
	VariableIndex variable(index);
	double objective = 0;

	clp::Clp_addColumns(m_model.get(), 1, &lb, &ub, &objective, NULL, NULL, NULL);
	clp::Clp_setColumnName(m_model.get(), clp::Clp_getNumCols(m_model.get()) - 1, name);
	return variable;
}

void ClpModel::delete_variable(const VariableIndex &variable)
{

	if (!is_variable_active(variable))
	{
		throw std::runtime_error("Variable does not exist");
	}

	int variable_column = _variable_index(variable);
	clp::Clp_deleteColumns(m_model.get(), 1, &variable_column);

	m_variable_index.delete_index(variable.index);
}

void ClpModel::delete_variables(const Vector<VariableIndex> &variables)
{
	int n_variables = variables.size();
	if (n_variables == 0)
		return;

	std::vector<int> columns;
	columns.reserve(n_variables);
	for (int i = 0; i < n_variables; i++)
	{
		if (!is_variable_active(variables[i]))
		{
			continue;
		}
		auto column = _variable_index(variables[i]);
		columns.push_back(column);
	}

	clp::Clp_deleteColumns(m_model.get(), columns.size(), columns.data());

	for (int i = 0; i < n_variables; i++)
	{
		m_variable_index.delete_index(variables[i].index);
	}
}

bool ClpModel::is_variable_active(const VariableIndex &variable)
{
	return m_variable_index.has_index(variable.index);
}

double ClpModel::get_variable_value(const VariableIndex &variable)
{
	return get_variable_info(variable, COPT_DBLINFO_VALUE);
}

std::string ClpModel::pprint_variable(const VariableIndex &variable)
{
	return get_variable_name(variable);
}

void ClpModel::set_variable_bounds(const VariableIndex &variable, double lb, double ub)
{
	auto column = _checked_variable_index(variable);
	int cols = clp::Clp_getNumCols(m_model.get());
	double *columnLower = const_cast<double *>(clp::Clp_getColLower(m_model.get()));
	double *columnUpper = const_cast<double *>(clp::Clp_getColUpper(m_model.get()));

	columnLower[column] = lb;
	columnUpper[column] = ub;
	clp::Clp_chgColumnLower(m_model.get(), columnLower);
	clp::Clp_chgColumnUpper(m_model.get(), columnUpper);
}


ConstraintIndex ClpModel::add_linear_constraint(const ScalarAffineFunction &function,
                                                ConstraintSense sense, CoeffT rhs, char *name)
{
	IndexT index = m_linear_constraint_index.add_index();
	ConstraintIndex constraint_index(ConstraintType::Linear, index);

	AffineFunctionPtrForm<int, int, double> ptr_form;
	ptr_form.make(this, function);

	int numnz = ptr_form.numnz;
	int *cind = ptr_form.index;
	double *cval = ptr_form.value;
	double g_rhs = rhs - function.constant.value_or(0.0);

	if (name != nullptr && name[0] == '\0')
	{
		name = nullptr;
	}

	clp::Clp_addRows(m_model.get(), numnz, &g_rhs, &g_rhs, cind, NULL, cval);
	clp::Clp_setRowName(m_model.get(), clp::Clp_getNumRows(m_model.get()) - 1, name);
	return constraint_index;
}

int ClpModel::_variable_index(const VariableIndex &variable)
{
	return m_variable_index.get_index(variable.index);
}

int ClpModel::_checked_variable_index(const VariableIndex &variable)
{
	int column = _variable_index(variable);
	if (column < 0)
	{
		throw std::runtime_error("Variable does not exist");
	}
	return column;
}

int ClpModel::_constraint_index(const ConstraintIndex &constraint)
{
	switch (constraint.type)
	{
	case ConstraintType::Linear:
		return m_linear_constraint_index.get_index(constraint.index);
	case ConstraintType::Quadratic:
		return m_quadratic_constraint_index.get_index(constraint.index);
	case ConstraintType::SOS:
		return m_sos_constraint_index.get_index(constraint.index);
	case ConstraintType::Cone:
		return m_cone_constraint_index.get_index(constraint.index);
	case ConstraintType::COPT_ExpCone:
		return m_exp_cone_constraint_index.get_index(constraint.index);
	default:
		throw std::runtime_error("Unknown constraint type");
	}
}

int ClpModel::_checked_constraint_index(const ConstraintIndex &constraint)
{
	int row = _constraint_index(constraint);
	if (row < 0)
	{
		throw std::runtime_error("Constraint does not exist");
	}
	return row;
}

static void check_error(int error)
{
	if (error)
	{
		const int BUFFSIZE = 1000;
		char errmsg[BUFFSIZE] = {0};

		clp::Clp_GetRetcodeMsg(error, errmsg, BUFFSIZE);
		throw std::runtime_error(errmsg);
	}
}

static char copt_con_sense(ConstraintSense sense)
{
	switch (sense)
	{
	case ConstraintSense::LessEqual:
		return CLP_LESS_EQUAL;
	case ConstraintSense::Equal:
		return CLP_EQUAL;
	case ConstraintSense::GreaterEqual:
		return CLP_GREATER_EQUAL;
	default:
		throw std::runtime_error("Unknown constraint sense");
	}
}
