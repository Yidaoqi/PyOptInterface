#pragma once

#include <memory>

#include "pyoptinterface/lp.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/dylib.hpp"



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

	void set_A(const std::vector<double> &A, size_t rows, size_t cols);
	void set_c(const std::vector<double> &c, size_t length);
	void set_b(const std::vector<double> &b, size_t length);

	void set_max_itr(const size_t max_itr);
	void set_tol(const double tol);
  private:
	std::unique_ptr<linear_ip::lp> m_model;
};
