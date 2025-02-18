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
} // namespace clp


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

void ClpModel::set_normalized_coefficient(const ConstraintIndex &constraint,
                                          const VariableIndex &variable, double value)
{
	if (constraint.type != ConstraintType::Linear)
	{
		throw std::runtime_error("Only linear constraint supports set_normalized_coefficient");
	}
	int row = _checked_constraint_index(constraint);
	int col = _checked_variable_index(variable);
	clp::Clp_modifyCoefficient(m_model.get(), col, row, value);
}

double ClpModel::get_objective_coefficient(const VariableIndex &variable)
{
	int col = _checked_variable_index(variable);
	const double *coefficients = clp::Clp_getObjCoefficients(m_model.get());
	return coefficients[col];
}

void ClpModel::set_objective_coefficient(const VariableIndex &variable, double value)
{
	auto col = _checked_variable_index(variable);
	double *coefficients = const_cast<double*>(clp::Clp_getObjCoefficients(m_model.get()));
	int coloumns = clp::Clp_getNumCols(m_model.get());
	coefficients[col] = value;
	clp::Clp_chgObjCoefficients(m_model.get(), coefficients);
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

void ClpModel::set_callback(const clp_callback &callback, int cbctx)
{
	m_callback_userdata.model = this;
	m_callback_userdata.callback = callback;

	clp::Clp_registerCallBack(m_model.get(), callback);

	has_callback = true;
}

//static void check_error(int error)
//{
//	if (error)
//	{
//		const int BUFFSIZE = 1000;
//		char errmsg[BUFFSIZE] = {0};
//
//		clp::Clp_GetRetcodeMsg(error, errmsg, BUFFSIZE);
//		throw std::runtime_error(errmsg);
//	}
//}

static char clp_con_sense(ConstraintSense sense)
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
		return CLP_MINIMIZE;
	case ObjectiveSense::Maximize:
		return CLP_MAXIMIZE;
	default:
		throw std::runtime_error("Unknown objective sense");
	}
}

double ClpModel::get_variable_info(const VariableIndex &variable, const char *info_name)
{
	auto column = _checked_variable_index(variable);
	double retval = static_cast<double>(clp::Clp_getColumnStatus(m_model.get(), column));
	return retval;
}

std::string ClpModel::get_variable_name(const VariableIndex &variable)
{
	auto column = _checked_variable_index(variable);
	int reqsize = clp::Clp_lengthNames(m_model.get());
	std::string retval(reqsize + 1, '\0');
	clp::Clp_columnName(m_model.get(), column, retval.data());
	return retval;
}

void ClpModel::set_variable_name(const VariableIndex &variable, char *name)
{
	auto column = _checked_variable_index(variable);
	clp::Clp_setColumnName(m_model.get(), column, name);
}

void ClpModel::set_variable_lower_bound(const VariableIndex &variable, double lb)
{
	auto column = _checked_variable_index(variable);
	double *lower = clp::Clp_columnLower(m_model.get());
	lower[column] = lb;
	clp::Clp_chgColumnLower(m_model.get(), lower);
}

void ClpModel::set_variable_upper_bound(const VariableIndex &variable, double ub)
{
	auto column = _checked_variable_index(variable);
	double *upper = clp::Clp_columnUpper(m_model.get());
	upper[column] = ub;
	clp::Clp_chgColumnLower(m_model.get(), upper);
}

double ClpModel::get_constraint_info(const ConstraintIndex &constraint, const char *info_name)
{
	int row = _checked_constraint_index(constraint);
	double retval;
	int num = 1;
	//int error;
	switch (constraint.type)
	{
	case ConstraintType::Linear:
		retval = clp::Clp_getRowStatus(m_model.get(), row);
		break;
	default:
		throw std::runtime_error("Unknown constraint type");
	}
	//check_error(error);
	return retval;
}

std::string ClpModel::get_constraint_name(const ConstraintIndex &constraint)
{
	int row = _checked_constraint_index(constraint);
	int reqsize = clp::Clp_lengthNames(m_model.get());
	std::string retval(reqsize + 1, '\0');
	switch (constraint.type)
	{
	case ConstraintType::Linear:
		clp::Clp_rowName(m_model.get(), row, retval.data());
		break;
	case ConstraintType::Quadratic:
		//error = copt::COPT_GetQConstrName(m_model.get(), row, retval.data(), reqsize, &reqsize);
		//break;
	}
	return retval;
}

void ClpModel::set_constraint_name(const ConstraintIndex &constraint, char *name)
{
	int row = _checked_constraint_index(constraint);
	switch (constraint.type)
	{
	case ConstraintType::Linear:
		clp::Clp_setRowName(m_model.get(), row, name);
		break;
	default:
		throw std::runtime_error("Unknown constraint type");
	}
}

void ClpModel::set_obj_sense(ObjectiveSense sense)
{
	int obj_sense = clp_obj_sense(sense);
	clp::Clp_setObjSense(m_model.get(), obj_sense);
}

double ClpModel::get_normalized_rhs(const ConstraintIndex &constraint)
{
	int row = _checked_constraint_index(constraint);
	int num = 1;
	switch (constraint.type)
	{
	case ConstraintType::Linear: {
		double lb, ub;
		const double *lowers = clp::Clp_getRowLower(m_model.get());
		const double *uppers = clp::Clp_getRowUpper(m_model.get());
		lb = lowers[row];
		ub = uppers[row];

		bool lb_inf = lb < -CLP_INFINITY + 1.0;
		bool ub_inf = ub > CLP_INFINITY - 1.0;

		if (!lb_inf)
			return lb;
		if (!ub_inf)
			return ub;

		throw std::runtime_error("Constraint has no finite bound");
	}
	break;
	default:
		throw std::runtime_error("Unknown constraint type to get_normalized_rhs");
	}
}

void ClpModel::set_normalized_rhs(const ConstraintIndex &constraint, double value)
{
	int row = _checked_constraint_index(constraint);
	switch (constraint.type)
	{
	case ConstraintType::Linear: {
		double lb, ub;

		double *lowers = const_cast<double *>(clp::Clp_getRowLower(m_model.get()));
		double *uppers = const_cast<double *>(clp::Clp_getRowUpper(m_model.get()));
		lb = lowers[row];
		ub = uppers[row];

		bool lb_inf = lb < -CLP_INFINITY + 1.0;
		bool ub_inf = ub > CLP_INFINITY - 1.0;

		if (!lb_inf)
		{
			lowers[row] = value;
			clp::Clp_chgRowLower(m_model.get(), lowers);
		}
		if (!ub_inf)
		{
			uppers[row] = value;
			clp::Clp_chgRowUpper(m_model.get(), uppers);
		}

		if (lb_inf && ub_inf)
		{
			throw std::runtime_error("Constraint has no finite bound");
		}
	}
	break;
	default:
		throw std::runtime_error("Unknown constraint type to set_normalized_rhs");
	}
}

void ClpModel::cb_exit()
{
	clp::Clp_clearCallBack(m_model.get());
}

double ClpModel::cb_get_solution(const VariableIndex &variable)
{
	auto index = _variable_index(variable);

	const double *solutions = clp::Clp_getColSolution(m_model.get());
	return solutions[index];
}

void ClpModel::cb_set_solution(const VariableIndex &variable, double value)
{	
	int index = _variable_index(variable);
	double *solutions = const_cast<double *>(clp::Clp_getColSolution(m_model.get()));
	solutions[index] = value;
	clp::Clp_setColSolution(m_model.get(), solutions);
}
