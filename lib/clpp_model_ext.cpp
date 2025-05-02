#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/function.h>

#include "pyoptinterface/clpp_model.hpp"

namespace nb = nanobind;

NB_MODULE(clpp_model_ext, m)
{
	m.import_("pyoptinterface._src.core_ext");
	m.import_("pyoptinterface._src.nleval_ext");

	m.def("is_library_loaded", &clpp::is_library_loaded);
	m.def("load_library", &clpp::load_library);



#define BIND_F(f) .def(#f, &ClppModel::f)

	nb::class_<ClppModel>(m, "RawModel")
	    .def(nb::init<>()) 
		BIND_F(is_solved) 
		BIND_F(get_A)
		BIND_F(get_b) 
		BIND_F(get_c)
	    .def("set_A", &ClppModel::set_A, nb::arg("A"), nb::arg("rows"), nb::arg("cols"))
	    .def("set_b", &ClppModel::set_b, nb::arg("b"), nb::arg("b_length"))
	    .def("set_c", &ClppModel::set_c, nb::arg("c"), nb::arg("c_length")) 
		BIND_F(solve)
	    BIND_F(get_x) 
		BIND_F(get_lam) 
		BIND_F(get_s)
		BIND_F(get_rows) 
		BIND_F(get_cols)
	    .def("set_max_itr", &ClppModel::set_max_itr, nb::arg("max_itr"))
	    .def("set_tol", &ClppModel::set_tol, nb::arg("tolerance"));
}