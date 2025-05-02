/* src/Common/config_ipopt.h.  Generated from config_ipopt.h.in by configure.  */
/* src/Common/config_ipopt.h.in. */

#ifndef __CONFIG_CLPP_H__
#define __CONFIG_CLPP_H__


/* Define to the debug sanity check level (0 is no test) */
#define CLPP_CHECKLEVEL 0

/* Define to the debug verbosity level (0 is no output) */
#define CLPP_VERBOSITY 0

/* Define to 1 if using single precision floating point */
/* #undef CLPP_SINGLE */

/* Define to 1 if Ipopt index type is int64_t */
/* #undef CLPP_INT64 */

#if defined(_MSC_VER)

#if defined(_BUILD_CLPP)

/* Library Visibility Attribute */
#define CLPPAMPLINTERFACELIB_EXPORT __declspec(dllexport)

/* Library Visibility Attribute */
#define CLPPLIB_EXPORT __declspec(dllexport)

/* Library Visibility Attribute */
#define SCLPPLIB_EXPORT __declspec(dllexport)

#else
/* Library Visibility Attribute */
#define CLPPAMPLINTERFACELIB_EXPORT __declspec(dllimport)

/* Library Visibility Attribute */
#define CLPPLIB_EXPORT __declspec(dllimport)

/* Library Visibility Attribute */
#define SCLPPLIB_EXPORT __declspec(dllimport)
#endif

#else

#define CLPPAMPLINTERFACELIB_EXPORT
#define CLPPLIB_EXPORT
#define SCLPPLIB_EXPORT

#endif

#endif
