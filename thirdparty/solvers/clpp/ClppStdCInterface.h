#ifndef __CLPPSTDCINTERFACE_H__
#define __CLPPSTDCINTERFACE_H__

#include <stdbool.h>
#include "lp.h"
#include "ClppConfig.h"


#ifdef __cplusplus
extern "C"
{
#endif

struct ClppProblemInfo;
typedef struct ClppProblemInfo* ClppProblem;

CLPPLIB_EXPORT ClppProblem __cdecl CreateClppProblem(double *c, double *A, double *b, size_t rows, size_t cols);

CLPPLIB_EXPORT void __cdecl FreeClppProblem(ClppProblem lp_problem);

CLPPLIB_EXPORT bool __cdecl ClppIsSolved(ClppProblem lp_problem);

CLPPLIB_EXPORT void __cdecl ClppSolve(ClppProblem lp_problem);

CLPPLIB_EXPORT const double* __cdecl ClppGetX(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT const double* __cdecl ClppGetLam(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT const double* __cdecl ClppGetS(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT const double* __cdecl ClppGetA(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT const double* __cdecl ClppGetC(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT const double* __cdecl ClppGetB(ClppProblem lp_problem, double* buff);

CLPPLIB_EXPORT void __cdecl ClppSetA(ClppProblem lp_problem, double* A, size_t rows, size_t cols);

CLPPLIB_EXPORT void __cdecl ClppSetC(ClppProblem lp_problem, double* vec, size_t length);

CLPPLIB_EXPORT void __cdecl ClppSetB(ClppProblem lp_problem, double* vec, size_t length);

CLPPLIB_EXPORT void __cdecl ClppSetMaxItr(ClppProblem lp_problem, int max_itr);

CLPPLIB_EXPORT void __cdecl ClppSetTol(ClppProblem lp_problem, double tol);

CLPPLIB_EXPORT const size_t _cdecl ClppGetRows(ClppProblem lp_problem);

CLPPLIB_EXPORT const size_t _cdecl ClppGetCols(ClppProblem lp_problem);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __CLPPSTDCINTERFACE_H__ */