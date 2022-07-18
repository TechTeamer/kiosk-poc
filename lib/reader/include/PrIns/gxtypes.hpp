#ifndef GX_TYPES_INCL
#define GX_TYPES_INCL
/******************************************************************************/
/** \file
 *	System type definitions.
 */
/******************************************************************************/
#ifndef __cplusplus
#error C++ compiler required.
#endif
//------------------------------------------------------------------------------
#ifdef _MSC_VER
//	#pragma warning(disable : 4710)			// function not inlined
//	#pragma warning(disable : 4786)			// truncated debug info
#endif
#ifdef __INTEL_COMPILER
//	#pragma warning(disable : 271)			// trailing comma is nonstandard
//	#pragma warning(disable : 383)			// value copied to temporary, reference to temporary used
//	#pragma warning(disable : 981)			// operands are evaluated in unspecified order
#endif
//------------------------------------------------------------------------------
#if !defined(LINUX) && (defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__))
	#define LINUX
#endif
#if !defined(WIN64) && defined(_WIN64)
	#define WIN64
#endif
#if !defined(WIN32) && (defined(_WIN32) || defined(__WIN32__) || defined(WIN64))
	#define WIN32
#endif

#ifdef LINUX
	#include <stdint.h>
	#if __WORDSIZE == 64
		#define LINUX64
	#endif
#endif
//------------------------------------------------------------------------------
#ifdef WIN32
	#define GXAPI __stdcall
#else
	#define GXAPI
#endif
//------------------------------------------------------------------------------
/// Function signature.
#if defined(__GNUC__) || (__INTEL_COMPILER >= 910)
	#define _GX_FUNCTION_	__PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
	#define _GX_FUNCTION_	__FUNCSIG__
#elif (__BORLANDC__ >= 0x550)
	#define _GX_FUNCTION_	__FUNC__
#else
	#define _GX_FUNCTION_	"(unknown)"
#endif
//------------------------------------------------------------------------------
/// Declared if rvalue references are usable.
#ifndef GX_RVALUE_REF
#if (__cpp_rvalue_references >= 200610) || (_MSC_VER >= 1600)
#define GX_RVALUE_REF 1
#elif __clang__
#define GX_RVALUE_REF __has_feature(cxx_rvalue_references)
#else
#define GX_RVALUE_REF 0
#endif
#endif
//------------------------------------------------------------------------------
typedef		signed char			gxi8;	///< 8 bit signed integer.
typedef		unsigned char		gxu8;	///< 8 bit unsigned integer.
typedef		signed short		gxi16;	///< 16 bit signed integer.
typedef		unsigned short		gxu16;	///< 16 bit unsigned integer.
typedef		signed int			gxi32;	///< 32 bit signed integer.
typedef		unsigned int		gxu32;	///< 32 bit unsigned integer.

#ifdef __GNUC__
	typedef signed long long	gxi64;	///< 64 bit signed integer.
	typedef unsigned long long	gxu64;	///< 64 bit unsigned integer.
#else
	typedef	signed __int64		gxi64;	///< 64 bit signed integer.
	typedef	unsigned __int64	gxu64;	///< 64 bit unsigned integer.
#endif

typedef		float				gxf32;	///< 32 bit floating-point number.
typedef		double				gxf64;	///< 64 bit floating-point number.
//------------------------------------------------------------------------------
/// Number of items in a C array.
#define gx_arraycnt(X)		(sizeof(X)/sizeof((X)[0]))
//------------------------------------------------------------------------------
#endif //GX_TYPES_INCL
