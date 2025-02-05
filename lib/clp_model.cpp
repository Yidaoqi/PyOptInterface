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

ConstraintIndex ClpModel::add_quadratic_constraint(const ScalarQuadraticFunction &function,
                                                   ConstraintSense sense, CoeffT rhs,
                                                   const char *name)
{
	return ConstraintIndex();
}

void ClpModel::delete_constraint(const ConstraintIndex &constraint)
{
	int error = -1;
	int constraint_row = _constraint_index(constraint);
	if (constraint_row >= 0)
	{
		switch (constraint.type)
		{
		case ConstraintType::Linear:
			m_linear_constraint_index.delete_index(constraint.index);
			clp::Clp_deleteRows(m_model.get(), 1, &constraint_row);
			error = 0;
			break;
		//case ConstraintType::Quadratic:
		//	m_quadratic_constraint_index.delete_index(constraint.index);
		//	error = copt::COPT_DelQConstrs(m_model.get(), 1, &constraint_row);
		//	break;
		//case ConstraintType::SOS:
		//	m_sos_constraint_index.delete_index(constraint.index);
		//	error = copt::COPT_DelSOSs(m_model.get(), 1, &constraint_row);
		//	break;
		//case ConstraintType::Cone:
		//	m_cone_constraint_index.delete_index(constraint.index);
		//	error = copt::COPT_DelCones(m_model.get(), 1, &constraint_row);
		//	break;
		//case ConstraintType::COPT_ExpCone:
		//	m_exp_cone_constraint_index.delete_index(constraint.index);
		//	error = copt::COPT_DelExpCones(m_model.get(), 1, &constraint_row);
		//	break;
		default:
			throw std::runtime_error("Unknown constraint type");
		}
	}
	check_error(error);
}

bool ClpModel::is_constraint_active(const ConstraintIndex &constraint)
{
	switch (constraint.type)
	{
	case ConstraintType::Linear:
		return m_linear_constraint_index.has_index(constraint.index);
	case ConstraintType::Quadratic:
		return m_quadratic_constraint_index.has_index(constraint.index);
	case ConstraintType::SOS:
		return m_sos_constraint_index.has_index(constraint.index);
	case ConstraintType::Cone:
		return m_cone_constraint_index.has_index(constraint.index);
	case ConstraintType::COPT_ExpCone:
		return m_exp_cone_constraint_index.has_index(constraint.index);
	default:
		throw std::runtime_error("Unknown constraint type");
	}
}

void ClpModel::_set_affine_objective(const ScalarAffineFunction &function, ObjectiveSense sense,
                                     bool clear_quadratic)
{
	int error = 0;
	if (clear_quadratic)
	{
		// First delete all quadratic terms
		error = clp::Clp_DelQuadObj(m_model.get());
		check_error(error);
	}

	// Set Obj attribute of each variable
	int n_variables = get_raw_attribute_int(COPT_INTATTR_COLS);
	std::vector<int> ind_v(n_variables);
	for (int i = 0; i < n_variables; i++)
	{
		ind_v[i] = i;
	}
	std::vector<double> obj_v(n_variables, 0.0);

	int numnz = function.size();
	for (int i = 0; i < numnz; i++)
	{
		auto column = _variable_index(function.variables[i]);
		if (column < 0)
		{
			throw std::runtime_error("Variable does not exist");
		}
		obj_v[column] = function.coefficients[i];
	}

	clp::Clp_chgObjCoefficients(m_model.get(), obj_v.data());
	check_error(error);
	clp::COPT_SetObjConst(m_model.get(), function.constant.value_or(0.0));
	check_error(error);

	int obj_sense = clp_obj_sense(sense);
	clp::Clp_setObjSense(m_model.get(), obj_sense);
	check_error(error);
}

void ClpModel::set_objective(const ScalarAffineFunction &function, ObjectiveSense sense)
{
	_set_affine_objective(function, sense, true);
}

void ClpModel::set_objective(const ScalarQuadraticFunction &function, ObjectiveSense sense)
{
	
	int error = 0;
	// First delete all quadratic terms
	//error = clp::COPT_DelQuadObj(m_model.get());
	//check_error(error);

	// Add quadratic term
	int numqnz = function.size();
	if (numqnz > 0)
	{
		QuadraticFunctionPtrForm<int, int, double> ptr_form;
		ptr_form.make(this, function);
		int numqnz = ptr_form.numnz;
		int *qrow = ptr_form.row;
		int *qcol = ptr_form.col;
		double *qval = ptr_form.value;

		clp::Clp_loadQuadraticObjective(m_model.get(), numqnz, qrow, qcol, qval);
	}

	// Affine part
	const auto &affine_part = function.affine_part;
	if (affine_part)
	{
		const auto &affine_function = affine_part.value();
		_set_affine_objective(affine_function, sense, false);
	}
	else
	{
		ScalarAffineFunction zero;
		_set_affine_objective(zero, sense, false);
	}
}

void ClpModel::set_objective(const ExprBuilder &function, ObjectiveSense sense)
{
	auto deg = function.degree();
	if (deg <= 1)
	{
		ScalarAffineFunction f(function);
		set_objective(f, sense);
	}
	else if (deg == 2)
	{
		ScalarQuadraticFunction f(function);
		set_objective(f, sense);
	}
	else
	{
		throw std::runtime_error("Objective must be linear or quadratic");
	}
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

void ClpModel::optimize()
{
	if (has_callback)
	{
		// Store the number of variables for the callback
		m_callback_userdata.n_variables = get_raw_attribute_int("Cols");
	}
	clp::Clp_initialSolve(m_model.get());
	double* solutions = clp::Clp_primalColumnSolution(m_model.get());
	int error = clp::Clp_status(m_model.get());
	check_error(error);
}

void *ClpModel::get_raw_model()
{
	return m_model.get();
}

std::string ClpModel::version_string()
{
	const char* buffer = clp::Clp_Version();
	return buffer;
}

static int clp_obj_sense(ObjectiveSense sense)
{
	switch (sense)
	{
	case ObjectiveSense::Minimize:
		return COPT_MINIMIZE;
	case ObjectiveSense::Maximize:
		return COPT_MAXIMIZE;
	default:
		throw std::runtime_error("Unknown objective sense");
	}
}
