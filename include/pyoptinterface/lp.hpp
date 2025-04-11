#ifndef LINEAR_IP_H
#define LINEAR_IP_H

/**
 *  @file lp.h
 *  @brief The class definition of an LP problem.
 *
 *  @author Chris Jordan-Squire
 *  @date 03/02/2013
 *
 */

#include <Eigen/Dense>

#include <string>

namespace linear_ip{

typedef   Eigen::MatrixXd Matrix;
typedef   Eigen::VectorXd Vector;
class   lp_impl;

/** @brief A linear programming problem.
 *
 *  A class for the linear programming problem
 *  min_x <c,x>
 *  s.t. Ax=b
 *      x>=0
 *
 *  It is solved using a Mehrota predictor-corrector
 *  interior point method. 
 */
class   lp{

public:
    /**
        * Constructs problem with cost vector x,
        * constraint matrix A, and constraint values b.
        * The constraint region is Ax=b, x>=0.
        *
        * @param c cost vector
        * @param A constraint matrix
        * @param b constraint values
        */
    lp();
    lp(const Vector &c, const Matrix &A, const Vector &b);
	//lp(const std::vector<double> &c_data, size_t c_len, const std::vector<double> &A_data,
	//    size_t rows,
	//    size_t cols, const std::vector<double> &b_data, size_t b_len);
	~lp();

    /** Has the problem been solved.
        * @return True is the problem has been solved,
        * False else.
        */
    bool is_solved() const;

    /** Solve the problem using an interior point method. */
    void solve();

    /** @return The optimal primal solution. */
    Vector get_x() const;

    /** @return The optimal dual solution for the constraints
        * Ax=b. */
    Vector get_lam() const;

    /** @return The optimal dual solution for the constraints
        * x>=0. */
    Vector get_s() const;

    /** @return The constraint maxtrix A. */
    Matrix get_A() const;

    void set_A(const std::vector<double> & A, int rows, int cols);
    /** @return The cost vector c.*/
    Vector get_c() const;

    void set_c(const std::vector<double> &c, size_t length);
    /** @return The constraint vector b.*/
    Vector get_b() const;

    void set_b(const std::vector<double> &b, size_t length);


    const int get_rows() const;
	const int get_cols() const;

    /** Set the maximum number of iterations in the interior 
        * point solver.
        * @param max_itr The maximum number of iterations. Must
        * be greater than 0.
        */
    void set_max_itr(const int max_itr);

    /** Set the tolerance used to check for convergence.
        * @param tol The new tolerance.
        */
    void set_tol(const double tol);

    lp(const lp&) = delete;
    lp& operator=(const lp&) = delete;

private:
    lp_impl *ip;
    //C++98 compliant non-copyable class
    //In C++11 I'd simply delete these functions publicly
    //and use a shared pointer for lp_impl.
    //lp(const lp&) {}
    //lp& operator=(const lp&) {}
};

}
#endif
