#include "pyoptinterface/clpp_model.hpp"


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
} // namespace copt

ClppModel::ClppModel(linear_ip::Vector& c, linear_ip::Matrix& A, linear_ip::Vector& b)
{
	m_model = std::unique_ptr<linear_ip::lp, ClppfreemodelT>(CreateClppProblem(c, A, b));
}

bool ClppModel::is_solved() const
{
	return ClppIsSolved(m_model.get());
}

void ClppModel::solve()
{
	ClppSolve(m_model.get());
}

linear_ip::Vector ClppModel::get_x() const
{
	linear_ip::Vector x;
	ClppGetX(m_model.get(), x);
	return x;
}

linear_ip::Vector ClppModel::get_lam() const
{
	linear_ip::Vector lam;
	ClppGetLam(m_model.get(), lam);
	return lam;
}

linear_ip::Vector ClppModel::get_s() const
{
	linear_ip::Vector s;
	ClppGetS(m_model.get(), s);
	return s;
}

linear_ip::Matrix ClppModel::get_A() const
{
	linear_ip::Matrix A;
	ClppGetA(m_model.get(), A);
	return A;
}

linear_ip::Vector ClppModel::get_c() const
{
	linear_ip::Vector c;
	ClppGetC(m_model.get(), c);
	return c;
}

linear_ip::Vector ClppModel::get_b() const
{
	linear_ip::Vector b;
	ClppGetB(m_model.get(), b);
	return b;
}

void ClppModel::set_max_itr(const int max_itr)
{
	ClppSetMaxItr(m_model.get(), max_itr);
}

void ClppModel::set_tol(const double tol)
{
	ClppSetTol(m_model.get(), tol);
}

void *ClppModel::get_raw_model()
{
	return m_model.get();
}
