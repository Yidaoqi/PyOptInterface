import os
import platform
from pathlib import Path
import logging
from typing import Dict

from .clpp_model_ext import RawModel
from .attributes import (
    VariableAttribute,
    ConstraintAttribute,
    ModelAttribute,
    ResultStatusCode,
    TerminationStatusCode,
)
from .core_ext import ConstraintType, VariableIndex, ObjectiveSense
from .solver_common import (
    _direct_get_model_attribute,
    _direct_set_model_attribute,
    _direct_get_entity_attribute,
    _direct_set_entity_attribute,
)
from .aml import make_variable_tupledict, make_variable_ndarray
from .matrix import add_matrix_constraints

class Model(RawModel):
    def __init__(self):
        super().__init__()
    
