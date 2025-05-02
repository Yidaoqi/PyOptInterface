#include "pyoptinterface/clpp_model.hpp"
#include <stdexcept>
#include <iostream>

#include "fmt/core.h"
#include "fmt/ranges.h"
#include "pyoptinterface/dylib.hpp"
namespace clpp
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
}  // namespace clpp

ClppModel::ClppModel()
{
	if (!clpp::is_library_loaded())
	{
		throw std::runtime_error("Clpp library is not loaded");
	}
	double c[1] = {0};
	double A[1] = {0};
	double b[1] = {0};
	ClppProblemInfo* solver = clpp::CreateClppProblem(c, A, b, 1, 1);
	m_model.reset(solver);
}

void ClppModel::solve()
{
	try
	{
		clpp::ClppSolve(m_model.get());
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

bool ClppModel::is_solved() const
{
	return clpp::ClppIsSolved(m_model.get());
}

const std::vector<double> ClppModel::get_x() const
{
	size_t length = clpp::ClppGetCols(m_model.get());
	double *buff = new double[length];
	const double *x = clpp::ClppGetX(m_model.get(), buff);
	std::vector<double> X(x, x + length);
	delete[] buff;
	return X;
}

const std::vector<double> ClppModel::get_lam() const
{
	size_t length = clpp::ClppGetRows(m_model.get());
	double *buff = new double[length];
	const double *temp = clpp::ClppGetLam(m_model.get(), buff);
	std::vector<double> lam(temp, temp + length);
	delete[] buff;
	return lam;
}

const std::vector<double> ClppModel::get_s() const
{
	size_t length = clpp::ClppGetCols(m_model.get());
	double *buff = new double[length];
	const double *temp = clpp::ClppGetS(m_model.get(), buff);
	std::vector<double> s(temp, temp + length);
	delete[] buff;
	return s;
}

const std::vector<double> ClppModel::get_A() const
{
	size_t cols = clpp::ClppGetCols(m_model.get());
	size_t rows = clpp::ClppGetRows(m_model.get());
	double *buff = new double[rows*cols];
	const double* temp = clpp::ClppGetA(m_model.get(), buff);
	std::vector<double> A(temp, temp + (rows * cols));
	delete[] buff;
	return A;
}

const std::vector<double> ClppModel::get_c() const
{
	size_t length = clpp::ClppGetCols(m_model.get());
	double *buff = new double[length];
	const double* temp = clpp::ClppGetC(m_model.get(), buff);
	std::vector<double> c(temp, temp + length);
	delete[] buff;
	return c;
}

const std::vector<double> ClppModel::get_b() const
{
	size_t length = clpp::ClppGetRows(m_model.get());
	double *buff = new double[length];
	const double* temp = clpp::ClppGetB(m_model.get(), buff);
	std::vector<double> b(temp, temp + length);
	delete[] buff;
	return b;
}

const int ClppModel::get_rows() const
{
	return clpp::ClppGetRows(m_model.get());
}

const int ClppModel::get_cols() const
{
	return clpp::ClppGetCols(m_model.get());
}

void ClppModel::set_A(std::vector<double> &A, size_t rows, size_t cols)
{
	clpp::ClppSetA(m_model.get(), A.data(), rows, cols);
}

void ClppModel::set_c(std::vector<double> &c, size_t length)
{
	clpp::ClppSetC(m_model.get(), c.data(), length);
}

void ClppModel::set_b(std::vector<double> &b, size_t length)
{
	clpp::ClppSetB(m_model.get(), b.data(), length);
}

void ClppModel::set_max_itr(const size_t max_itr)
{
	clpp::ClppSetMaxItr(m_model.get(), max_itr);
}

void ClppModel::set_tol(const double tol)
{
	clpp::ClppSetTol(m_model.get(), tol);
}

