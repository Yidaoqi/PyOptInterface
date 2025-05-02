#pragma once

#include <memory>

#include "solvers/clpp/ClppStdCInterface.h"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/nleval.hpp"
#include "pyoptinterface/dylib.hpp"

#define APILIST            \
	B(CreateClppProblem); \
	B(FreeClppProblem);   \
	B(ClppIsSolved);	  \
	B(ClppSolve);			\
	B(ClppGetX);			\
	B(ClppGetLam);			\
	B(ClppGetS);			\
	B(ClppGetA);			\
	B(ClppGetC);			\
	B(ClppGetB);			\
	B(ClppSetA);			\
	B(ClppSetC);			\
	B(ClppSetB);			\
	B(ClppSetMaxItr);		\
	B(ClppSetTol);			\
	B(ClppGetRows);			\
	B(ClppGetCols);

namespace clpp
{
#define B DYLIB_EXTERN_DECLARE
APILIST
#undef B

bool is_library_loaded();

bool load_library(const std::string &path);
} // namespace clpp

struct ClppfreeproblemT
{
	void operator()(ClppProblemInfo *model) const
	{
		clpp::FreeClppProblem(model);
	};
};

class ClppModel
{
  public:
	ClppModel();
	ClppModel(const ClppModel &) = delete;
	ClppModel &operator=(const ClppModel &) = delete;

	void solve();
	bool is_solved() const;

	const std::vector<double> get_x() const;
	const std::vector<double> get_lam() const;
	const std::vector<double> get_s() const;
	const std::vector<double> get_A() const;
	const std::vector<double> get_c() const;
	const std::vector<double> get_b() const;

    const int get_rows() const;
	const int get_cols() const;

	void set_A(std::vector<double> &A, size_t rows, size_t cols);
	void set_c(std::vector<double> &c, size_t length);
	void set_b(std::vector<double> &b, size_t length);

	void set_max_itr(const size_t max_itr);
	void set_tol(const double tol);
  private:
	std::unique_ptr<ClppProblemInfo, ClppfreeproblemT> m_model;
};
