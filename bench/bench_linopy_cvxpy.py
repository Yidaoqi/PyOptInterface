import time
from numpy import arange
import numpy as np
import pandas as pd

from linopy import Model

import pyoptinterface as poi
from pyoptinterface import gurobi

import cvxpy


def create_linopy_model(N):
    m = Model()
    x = m.add_variables(coords=[arange(N), arange(N)])
    y = m.add_variables(coords=[arange(N), arange(N)])
    m.add_constraints(x - y >= arange(N))
    m.add_constraints(x + y >= 0)
    m.add_objective((2 * x).sum() + y.sum())
    return m


def create_cvxpy_model(N):
    x = cvxpy.Variable((N, N))
    y = cvxpy.Variable((N, N))
    constraints = []
    for i in range(N):
        constraints.append(x[:, i] - y[:, i] >= np.arange(N))
    constraints.append(x + y >= 0)
    objective = cvxpy.Minimize(2 * cvxpy.sum(x) + cvxpy.sum(y))
    return cvxpy.Problem(objective, constraints)


def create_poi_model(N):
    m = gurobi.Model()
    x = m.add_variables(range(N), range(N))
    y = m.add_variables(range(N), range(N))
    for i in range(N):
        for j in range(N):
            m.add_linear_constraint(x[i, j] - y[i, j], poi.Geq, i)
            m.add_linear_constraint(x[i, j] + y[i, j], poi.Geq, 0)
    expr = poi.ExprBuilder()
    poi.quicksum_(expr, x, lambda x: 2 * x)
    poi.quicksum_(expr, y)
    m.set_objective(expr)
    return m


def bench(N):
    results = {}

    t0 = time.time()
    model = create_poi_model(N)
    model.optimize()
    t1 = time.time()
    results["n_variables"] = 2 * N * N
    results["poi"] = t1 - t0

    t0 = time.time()
    model = create_linopy_model(N)
    model.solve(solver_name="gurobi", io_api="direct")
    t1 = time.time()
    results["linopy"] = t1 - t0

    t0 = time.time()
    model = create_cvxpy_model(N)
    model.solve(solver=cvxpy.GUROBI)
    t1 = time.time()
    results["cvxpy"] = t1 - t0

    return results


def main():
    Ns = range(100, 501, 100)
    results = []
    for N in Ns:
        results.append(bench(N))
    # create a DataFrame
    df = pd.DataFrame(results, index=Ns)

    # show result
    print(df)


if __name__ == "__main__":
    main()
