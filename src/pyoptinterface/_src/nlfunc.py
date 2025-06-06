from .nlexpr_ext import (
    ExpressionGraph,
    ExpressionHandle,
    UnaryOperator,
    BinaryOperator,
    TernaryOperator,
)
from .core_ext import (
    VariableIndex,
    ScalarAffineFunction,
    ScalarQuadraticFunction,
    ExprBuilder,
    ConstraintSense,
)
from .comparison_constraint import ComparisonConstraint
import inspect
import functools
import math
import types
import threading

# Trace an ordinary Python function to get its expression graph

Vars = types.SimpleNamespace
Params = types.SimpleNamespace


class ExpressionGraphContext:
    _thread_local = threading.local()

    def __enter__(self):
        _thread_local = self._thread_local
        graph = ExpressionGraph()
        if not hasattr(_thread_local, "_graph_stack"):
            _thread_local._graph_stack = [graph]
        else:
            _thread_local._graph_stack.append(graph)
        return graph

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._thread_local._graph_stack.pop()

    @classmethod
    def current_graph_no_exception(cls):
        _thread_local = cls._thread_local
        if not hasattr(_thread_local, "_graph_stack"):
            return None
        stack = _thread_local._graph_stack
        if not stack:
            return None
        return stack[-1]

    @classmethod
    def current_graph(cls):
        _thread_local = cls._thread_local
        if not hasattr(_thread_local, "_graph_stack"):
            raise RuntimeError("No active expression graph context")
        stack = _thread_local._graph_stack
        if not stack:
            raise RuntimeError("No active expression graph context")
        return stack[-1]


compare_op_map = {
    ConstraintSense.LessEqual: BinaryOperator.LessEqual,
    ConstraintSense.GreaterEqual: BinaryOperator.GreaterEqual,
    ConstraintSense.Equal: BinaryOperator.Equal,
}


def convert_to_expressionhandle(graph, expr):
    if isinstance(expr, ExpressionHandle):
        return expr
    elif isinstance(expr, (int, float)):
        return graph.add_constant(expr)
    elif isinstance(expr, VariableIndex):
        return graph.merge_variableindex(expr)
    elif isinstance(expr, ScalarAffineFunction):
        return graph.merge_scalaraffinefunction(expr)
    elif isinstance(expr, ScalarQuadraticFunction):
        return graph.merge_scalarquadraticfunction(expr)
    elif isinstance(expr, ExprBuilder):
        return graph.merge_exprbuilder(expr)
    elif isinstance(expr, ComparisonConstraint):
        lhs = convert_to_expressionhandle(graph, expr.lhs)
        rhs = convert_to_expressionhandle(graph, expr.rhs)
        if isinstance(lhs, ExpressionHandle) and isinstance(rhs, ExpressionHandle):
            compare_op = compare_op_map[expr.sense]
            expr = graph.add_binary(compare_op, lhs, rhs)
            return expr
        else:
            raise TypeError(f"Unsupported expression type in comparison constraint")
    else:
        return expr


def to_nlexpr(expr):
    if isinstance(expr, ExpressionHandle):
        return expr
    graph = ExpressionGraphContext.current_graph()
    if isinstance(expr, (int, float)):
        return graph.add_constant(expr)
    elif isinstance(expr, VariableIndex):
        return graph.merge_variableindex(expr)
    elif isinstance(expr, ScalarAffineFunction):
        return graph.merge_scalaraffinefunction(expr)
    elif isinstance(expr, ScalarQuadraticFunction):
        return graph.merge_scalarquadraticfunction(expr)
    elif isinstance(expr, ExprBuilder):
        return graph.merge_exprbuilder(expr)
    else:
        raise TypeError(f"Unsupported expression type: {type(expr)}")


# Implement unary mathematical functions
def unary_mathematical_function(math_function, op: UnaryOperator):
    @functools.wraps(math_function)
    def f(expr):
        if isinstance(expr, (int, float)):
            return math_function(expr)
        graph = ExpressionGraphContext.current_graph()
        expr = convert_to_expressionhandle(graph, expr)
        if isinstance(expr, ExpressionHandle):
            new_expression = graph.add_unary(op, expr)
            return new_expression
        else:
            return NotImplemented

    return f


sin = unary_mathematical_function(math.sin, UnaryOperator.Sin)
cos = unary_mathematical_function(math.cos, UnaryOperator.Cos)
tan = unary_mathematical_function(math.tan, UnaryOperator.Tan)
asin = unary_mathematical_function(math.asin, UnaryOperator.Asin)
acos = unary_mathematical_function(math.acos, UnaryOperator.Acos)
atan = unary_mathematical_function(math.atan, UnaryOperator.Atan)
abs = unary_mathematical_function(math.fabs, UnaryOperator.Abs)
sqrt = unary_mathematical_function(math.sqrt, UnaryOperator.Sqrt)
exp = unary_mathematical_function(math.exp, UnaryOperator.Exp)
log = unary_mathematical_function(math.log, UnaryOperator.Log)
log10 = unary_mathematical_function(math.log10, UnaryOperator.Log10)


# Implement binary mathematical functions
def binary_mathematical_function(math_function, op: BinaryOperator):
    @functools.wraps(math_function)
    def f(expr1, expr2):
        is_number1 = isinstance(expr1, (int, float))
        is_number2 = isinstance(expr2, (int, float))
        if is_number1 and is_number2:
            return math_function(expr1, expr2)

        graph = ExpressionGraphContext.current_graph()

        expr1 = convert_to_expressionhandle(graph, expr1)
        expr2 = convert_to_expressionhandle(graph, expr2)

        if isinstance(expr1, ExpressionHandle) and isinstance(expr2, ExpressionHandle):
            new_expression = graph.add_binary(op, expr1, expr2)
            return new_expression
        else:
            return NotImplemented

    return f


pow = binary_mathematical_function(math.pow, BinaryOperator.Pow)


def ifelse(condition, true_expr, false_expr):
    graph = ExpressionGraphContext.current_graph()
    if isinstance(condition, bool):
        if condition:
            return true_expr
        else:
            return false_expr

    condition = convert_to_expressionhandle(graph, condition)
    true_expr = convert_to_expressionhandle(graph, true_expr)
    false_expr = convert_to_expressionhandle(graph, false_expr)

    new_expression = graph.add_ternary(
        TernaryOperator.IfThenElse, condition, true_expr, false_expr
    )
    return new_expression


class DynamicTable:
    def __init__(self):
        self._data = {}

    def __getattr__(self, name):
        if name not in self._data:
            self._data[name] = self._create()
        return self._data[name]

    def __len__(self):
        return len(self._data)

    def _names(self):
        return list(self._data.keys())

    def _create(self):
        pass


class VariableTable(DynamicTable):
    def _create(self):
        graph = ExpressionGraphContext.current_graph()
        variable = graph.add_variable(len(self))
        return variable


class ParameterTable(DynamicTable):
    def _create(self):
        graph = ExpressionGraphContext.current_graph()
        parameter = graph.add_parameter(len(self))
        return parameter


class FunctionTracingResult:
    def __init__(self, graph, results, variable_names, parameter_names):
        self.graph = graph
        self.results = results
        self.variable_names = variable_names
        self.parameter_names = parameter_names

        # record the inverse mapping from name to index
        self.variable_indices_map = {name: i for i, name in enumerate(variable_names)}
        self.parameter_indices_map = {name: i for i, name in enumerate(parameter_names)}

    def n_variables(self):
        return len(self.variable_names)

    def n_parameters(self):
        return len(self.parameter_names)

    def n_outputs(self):
        return len(self.results)


def trace_function(f):
    with ExpressionGraphContext() as graph:
        vars = VariableTable()
        params = ParameterTable()

        # detect the number of arguments f receives
        # 1: f(vars)
        # 2: f(vars, params)
        # >2: error
        n_args = len(inspect.signature(f).parameters)
        if n_args == 1:
            results = f(vars)
        elif n_args == 2:
            results = f(vars, params)
        else:
            raise ValueError("Function to trace must receive either 1 or 2 arguments")

        # The results might be a scalar expressions or a list of scalar expressions
        if isinstance(results, ExpressionHandle):
            results = [results]

        return FunctionTracingResult(graph, results, vars._names(), params._names())
