import os
import platform
from pathlib import Path
import logging
from typing import Dict

from .clpp_model_ext import RawModel, load_library
# from .attributes import (
#     VariableAttribute,
#     ConstraintAttribute,
#     ModelAttribute,
#     ResultStatusCode,
#     TerminationStatusCode,
# )
# from .core_ext import ConstraintType, VariableIndex, ObjectiveSense
# from .solver_common import (
#     _direct_get_model_attribute,
#     _direct_set_model_attribute,
#     _direct_get_entity_attribute,
#     _direct_set_entity_attribute,
# )
# from .aml import make_variable_tupledict, make_variable_ndarray
# from .matrix import add_matrix_constraints


def detected_libraries():
    libs = []

    subdir = {
        "Linux": "lib",
        "Darwin": "lib",
        "Windows": "bin",
    }[platform.system()]
    libname = {
        "Linux": "libclpp.so",
        "Darwin": "libclpp.dylib",
        "Windows": "clpp.dll",
    }[platform.system()]

    # Environment
    home = os.environ.get("CLPP_HOME", None)
    if home and os.path.exists(home):
        lib = Path(home) / subdir / libname
        if lib.exists():
            libs.append(str(lib))

    # default names
    default_libname = libname
    libs.append(default_libname)

    return libs


def autoload_library():
    libs = detected_libraries()
    for lib in libs:
        ret = load_library(lib)
        if ret:
            logging.info(f"Loaded Clpp library: {lib}")
            return True
    return False


autoload_library()

class Model(RawModel):
    def __init__(self):
        super().__init__()
    
