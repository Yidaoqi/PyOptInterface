#ifndef __CLPPSTDCINTERFACE_H__
#define __CLPPSTDCINTERFACE_H__

#include <stdbool.h>
#include "lp.h"
#include "ClppConfig.h"

#ifdef __cplusplus
extern "C"
{
#endif
CLPPLIB_EXPORT linear_ip::lp* __stdcall CreateClppProblem(linear_ip::Vector& c, linear_ip::Matrix& A, linear_ip::Vector& b);

CLPPLIB_EXPORT void __stdcall FreeClppProblem(linear_ip::lp* lp_problem);

CLPPLIB_EXPORT bool __stdcall ClppIsSolved(linear_ip::lp* lp_problem);

CLPPLIB_EXPORT void __stdcall ClppSolve(linear_ip::lp* lp_problem);

CLPPLIB_EXPORT void __stdcall ClppGetX(linear_ip::lp* lp_problem, linear_ip::Vector& vec);

CLPPLIB_EXPORT void __stdcall ClppGetLam(linear_ip::lp* lp_problem, linear_ip::Vector& vec);

CLPPLIB_EXPORT void __stdcall ClppGetS(linear_ip::lp* lp_problem, linear_ip::Vector& vec);

CLPPLIB_EXPORT void __stdcall ClppGetA(linear_ip::lp* lp_problem, linear_ip::Matrix& A);

CLPPLIB_EXPORT void __stdcall ClppGetC(linear_ip::lp* lp_problem, linear_ip::Vector& vec);

CLPPLIB_EXPORT void __stdcall ClppGetB(linear_ip::lp* lp_problem, linear_ip::Vector& vec);

CLPPLIB_EXPORT void __stdcall ClppSetMaxItr(linear_ip::lp* lp_problem, const int max_itr);

CLPPLIB_EXPORT void __stdcall ClppSetTol(linear_ip::lp* lp_problem, const double tol);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __CLPPSTDCINTERFACE_H__ */