#include "pyoptinterface/clpp_model.hpp"

ClppModel::ClppModel(linear_ip::Vector &c, linear_ip::Matrix &A, linear_ip::Vector &b)
{
	m_model = std::make_unique<linear_ip::lp>(c, A, b);
}

bool ClppModel::is_solved() const
{
	return false;
}

bool clpp::is_library_loaded()
{
	return false;
}

bool clpp::load_library(const std::string &path)
{
	return false;
}
