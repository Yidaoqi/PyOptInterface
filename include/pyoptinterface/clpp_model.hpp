#pragma once

#include <memory>

#include "pyoptinterface/lp.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"
#include "pyoptinterface/dylib.hpp"

namespace clpp
{

bool is_library_loaded();

bool load_library(const std::string &path);
} // namespace clpp

class ClppModel
{
  public:
	ClppModel(linear_ip::Vector &c, linear_ip::Matrix &A, linear_ip::Vector &b);
	ClppModel(const ClppModel &) = delete;
	ClppModel &operator=(const ClppModel &) = delete;

	bool is_solved() const;

  private:
	std::unique_ptr<linear_ip::lp> m_model;
};
