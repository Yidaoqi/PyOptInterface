#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>

#include "pyoptinterface/nleval.hpp"

namespace nb = nanobind;

NB_MODULE(nleval_ext, m)
{
	nb::class_<AutodiffSymbolicStructure>(m, "AutodiffSymbolicStructure")
	    .def(nb::init<>())
	    .def_ro("nx", &AutodiffSymbolicStructure::nx)
	    .def_ro("np", &AutodiffSymbolicStructure::np)
	    .def_ro("ny", &AutodiffSymbolicStructure::ny)
	    .def_ro("m_jacobian_rows", &AutodiffSymbolicStructure::m_jacobian_rows)
	    .def_ro("m_jacobian_cols", &AutodiffSymbolicStructure::m_jacobian_cols)
	    .def_ro("m_jacobian_nnz", &AutodiffSymbolicStructure::m_jacobian_nnz)
	    .def_ro("m_hessian_rows", &AutodiffSymbolicStructure::m_hessian_rows)
	    .def_ro("m_hessian_cols", &AutodiffSymbolicStructure::m_hessian_cols)
	    .def_ro("m_hessian_nnz", &AutodiffSymbolicStructure::m_hessian_nnz)
	    .def_ro("has_parameter", &AutodiffSymbolicStructure::has_parameter)
	    .def_ro("has_jacobian", &AutodiffSymbolicStructure::has_jacobian)
	    .def_ro("has_hessian", &AutodiffSymbolicStructure::has_hessian);

	nb::class_<ConstraintAutodiffEvaluator>(m, "ConstraintAutodiffEvaluator")
	    .def(nb::init<>())
	    .def(nb::init<bool, uintptr_t, uintptr_t, uintptr_t>());

	nb::class_<ObjectiveAutodiffEvaluator>(m, "ObjectiveAutodiffEvaluator")
	    .def(nb::init<>())
	    .def(nb::init<bool, uintptr_t, uintptr_t, uintptr_t>());
}
