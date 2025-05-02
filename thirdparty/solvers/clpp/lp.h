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
#include <vector>

#ifdef LP_EXPORT
#define LP_API_EXPORT __declspec(dllexport)
#else
#define LP_API_EXPORT __declspec(dllimport)
#endif

namespace linear_ip{

typedef LP_API_EXPORT Eigen::MatrixXd Matrix;
typedef LP_API_EXPORT Eigen::VectorXd Vector;
class lp_impl;

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
class LP_API_EXPORT lp{

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
        lp(const Vector &c, const Matrix &A, const Vector &b);

        ~lp();


        /** Has the problem been solved.
         * @return True is the problem has been solved,
         * False else.
         */
        bool is_solved() const;

        /** Solve the problem using an interior point method. */
        void solve();

        /** @return The optimal primal solution. */
        inline Vector get_x() const;

        /** @return The optimal dual solution for the constraints
         * Ax=b. */
        inline Vector get_lam() const;

        /** @return The optimal dual solution for the constraints
         * x>=0. */
        inline Vector get_s() const;

        /** @return The constraint maxtrix A. */
        inline Matrix get_A() const;

        /** @return The cost vector c.*/
        inline Vector get_c() const;

        /** @return The constraint vector b.*/
        inline Vector get_b() const;

        inline const int get_rows() const;
        inline const int get_cols() const;

        /** Set the maximum number of iterations in the interior 
         * point solver.
         * @param max_itr The maximum number of iterations. Must
         * be greater than 0.
         */
        inline void set_max_itr(int max_itr);

        /** Set the tolerance used to check for convergence.
         * @param tol The new tolerance.
         */
        inline void set_tol(double tol);

        inline void set_A(const Matrix& A);

        inline void set_b(const Vector& b);

        inline void set_c(const Vector& c);


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
