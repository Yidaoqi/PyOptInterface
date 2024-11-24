import pytest

ipopt_model_dict = {}

import pyoptinterface.ipopt as ipopt

if ipopt.is_library_loaded():

    def llvm():
        return ipopt.Model(jit="LLVM")

    def c():
        return ipopt.Model(jit="C")

    ipopt_model_dict["ipopt_llvm"] = llvm
    # On macOS, loading dynamic library of Gurobi/COPT/Mosek before loading libtcc will cause memory error
    # The reason is still unclear
    ipopt_model_dict["ipopt_c"] = c


@pytest.fixture(params=ipopt_model_dict.keys())
def ipopt_model_ctor(request):
    name = request.param
    ctor = ipopt_model_dict[name]
    return ctor


model_interface_dict = {}

from pyoptinterface import gurobi, copt, mosek, highs

if gurobi.is_library_loaded():
    model_interface_dict["gurobi"] = gurobi.Model
if copt.is_library_loaded():
    model_interface_dict["copt"] = copt.Model
if mosek.is_library_loaded():
    model_interface_dict["mosek"] = mosek.Model
if highs.is_library_loaded():
    model_interface_dict["highs"] = highs.Model


@pytest.fixture(params=model_interface_dict.keys())
def model_interface(request):
    name = request.param
    model_interface_class = model_interface_dict[name]
    return model_interface_class()
