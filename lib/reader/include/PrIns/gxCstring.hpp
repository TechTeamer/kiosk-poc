#ifndef PR22_CSTRING_INCL
#define PR22_CSTRING_INCL
/******************************************************************************/
/** \file
 *	C string manipulation.
 */
/******************************************************************************/
#ifndef __cplusplus
#error C++ compiler required.
#endif
#define PR22_VERSION 2.2.0.19
#include "PrIns/gxtypes.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
//------------------------------------------------------------------------------
namespace Pr22 { namespace PrIns {

inline int snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...) {
	int r;
	va_list args;
	va_start(args, format);
#if _MSC_VER >= 1400
	r = _vsnwprintf_s(buffer, count, _TRUNCATE, format, args);
#elif defined(WIN32)
	r = _vsnwprintf(buffer, count, format, args);
#else
	r = vswprintf(buffer, count, format, args);
#endif
	buffer[count-1] = 0;
	va_end(args);
	return r;
}

inline wchar_t * wcsdup(const wchar_t *src) {
#if defined(WIN32)
	return _wcsdup(src);
#else
	return ::wcsdup(src);
#endif
}

template<class VECT>
inline const void * vector_data(const VECT &vect) {
	return vect.empty() ? 0 : &vect.front();
}

} // namespace PrIns
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_CSTRING_INCL
