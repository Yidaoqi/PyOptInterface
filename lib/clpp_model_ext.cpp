#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>

#include "pyoptinterface/clpp_model.hpp"

namespace nb = nanobind;

extern void bind_copt_constants(nb::module_ &m);

NB_MODULE(clpp_model_ext, m)
{
	m.import_("pyoptinterface._src.core_ext");

	m.def("is_library_loaded", &clpp::is_library_loaded);
	m.def("load_library", &clpp::load_library);

	nb::class_<ClppModel>(m, "RawModel")
	    .def(nb::init<linear_ip::Vector &, linear_ip::Matrix &, linear_ip::Vector &>())
	    .def("is_solved", &ClppModel::is_solved);
}