#pragma once

#include <memory>
#include "solvers/clpp/ClppStdCInterface.h"

#include "pyoptinterface/core.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/dylib.hpp"


#define APILIST				\
    B(CreateClppProblem);	\
	B(FreeClppProblem);		\
	B(ClppIsSolved);		\
	B(ClppSolve);			\
	B(ClppGetX);			\
	B(ClppGetLam);			\
	B(ClppGetS);			\
	B(ClppGetA);			\
	B(ClppGetC);			\
	B(ClppGetB);			\
	B(ClppSetMaxItr);		\
	B(ClppSetTol);	

struct ClppfreemodelT
{
	void operator()(linear_ip::lp *model) const
	{
		FreeClppProblem(model);
	};
};

namespace clpp
{
    #define B DYLIB_EXTERN_DECLARE
    APILIST
    #undef B

    bool is_library_loaded();

    bool load_library(const std::string &path);
} // namespace clpp

class ClppModel
{
  public:
	ClppModel(linear_ip::Vector& c, linear_ip::Matrix& A, linear_ip::Vector& b);
	ClppModel(const ClppModel&) = delete;
	ClppModel &operator=(const ClppModel&) = delete;

    bool is_solved() const;
	void solve();
	linear_ip::Vector get_x() const;
	linear_ip::Vector get_lam() const;
	linear_ip::Vector get_s() const;
	linear_ip::Matrix get_A() const;
	linear_ip::Vector get_c() const;
	linear_ip::Vector get_b() const;
	void set_max_itr(const int max_itr);
	void set_tol(const double tol);


	void *get_raw_model();

  private:
	std::unique_ptr<linear_ip::lp, ClppfreemodelT> m_model;
};

using ClppModelMixin = CommercialSolverMixin<ClppModel>;
