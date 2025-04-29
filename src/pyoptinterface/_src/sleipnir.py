from jormungandr.optimization import Problem
import numpy as np
from collections.abc import Collection

from .attributes import (
    VariableAttribute,
    ConstraintAttribute,
    ModelAttribute,
    ResultStatusCode,
    TerminationStatusCode,
)
from .core_ext import (
    VariableIndex,
    ScalarAffineFunction,
    ScalarQuadraticFunction,
    ExprBuilder,
    VariableDomain,
    ConstraintType,
    ConstraintSense,
    ObjectiveSense,
)
from .solver_common import (
    _get_model_attribute,
    _set_model_attribute,
    _get_entity_attribute,
    _direct_get_entity_attribute,
    _set_entity_attribute,
    _direct_set_entity_attribute,
)

def make_varable(model, lb=None, ub=None, domain=None, name=None):
    x = model.decision_variable()
    if lb is not None:
        model.subject_to(x >= lb)
    if ub is not None:
        model.subject_to(x <= ub)
    return x
'''
    if domain is not None:
        model.set_variable_attribute(x, VariableAttribute.Domain, domain)
    if name is not None:
        model.set_variable_attribute(x, VariableAttribute.Name, name)
'''

def make_varables(model, *coords: Collection, lb=None, ub=None, domain=None, name=None):
    rows, cols = len(coords[0]), len(coords[1])
    X = model.decision_variable(rows, cols)
    if lb is not None:
        model.subject_to(X >= np.full(X.shape, lb))
    if ub is not None:
        model.subject_to(X <= np.full(X.shape, ub))
    return X
'''
    if domain is not None:
        model.set_variable_attribute(x, VariableAttribute.Domain, domain)
    if name is not None:
        model.set_variable_attribute(x, VariableAttribute.Name, name)
'''
    

class Model(Problem):
    def __init__(self):
        super().__init__()

    def set_model_attribute(self, attr: ModelAttribute, value) -> None:
        pass

    def get_model_attribute(attr: ModelAttribute):
        pass

    def add_variable(self, *args, **kwargs):
        return make_varable(self, *args, **kwargs)

    def add_variables(self, *args, **kwargs):
        return make_varables(self, *args, **kwargs)

    def add_m_variables(self, shape, lb=None, ub=None, domain=VariableDomain.Continuous, name=""):
        X = self.decision_variable(shape[0], shape[1])
        if lb is not None:
            model.subject_to(X >= np.full(X.shape, lb))
        if ub is not None:
            model.subject_to(X <= np.full(X.shape, ub))
        return X

    def get_value(self, expr_or_var):
        return expr_or_var.value()

    def add_m_linear_constraints(self, A, vars, sense, b, name=""):
        rows, cols = A.shape
        for i in range(rows):
            y = 0.0
            for j, x in zip(A[i, :], vars):
                y += j * x
            if sense == ConstraintSense.LessEqual:
                self.subject_to(y <= b[i])
            elif sense == ConstraintSense.Equal:
                self.subject_to(y == b[i])
            else:
                self.subject_to(y >= b[i])

    def set_objective(self, expr, sense=ObjectiveSense.Minimize):
        if sense == ObjectiveSense.Minimize:
            self.minimize(expr)
        else:
            self.maximize(expr)

    def optimize(self):
        self.solve()






