#include "pyoptinterface/clpp_model.hpp"
#include <stdexcept>
#include <iostream>

ClppModel::ClppModel()
{
	m_model = std::make_unique<linear_ip::lp>();
}

void ClppModel::solve()
{
	try
	{
		m_model->solve();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

bool ClppModel::is_solved() const
{
	return m_model->is_solved();
}

const std::vector<double> ClppModel::get_x() const
{
	linear_ip::Vector temp = m_model->get_x();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const std::vector<double> ClppModel::get_lam() const
{
	linear_ip::Vector temp = m_model->get_lam();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const std::vector<double> ClppModel::get_s() const
{
	linear_ip::Vector temp = m_model->get_s();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const std::vector<double> ClppModel::get_A() const
{
	linear_ip::Matrix temp = m_model->get_A();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const std::vector<double> ClppModel::get_c() const
{
	linear_ip::Vector temp = m_model->get_c();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const std::vector<double> ClppModel::get_b() const
{
	linear_ip::Vector temp = m_model->get_b();
	return std::vector<double>(temp.data(), temp.data() + temp.size());
}

const int ClppModel::get_rows() const
{
	return m_model->get_rows();
}

const int ClppModel::get_cols() const
{
	return m_model->get_cols();
}

void ClppModel::set_A(const std::vector<double> &A, size_t rows, size_t cols)
{
	m_model->set_A(A, rows, cols);
}

void ClppModel::set_c(const std::vector<double> &c, size_t length)
{
	m_model->set_c(c, length);
}

void ClppModel::set_b(const std::vector<double> &b, size_t length)
{
	m_model->set_b(b, length);
}

void ClppModel::set_max_itr(const size_t max_itr)
{
	m_model->set_max_itr(max_itr);
}

void ClppModel::set_tol(const double tol)
{
	m_model->set_tol(tol);
}


