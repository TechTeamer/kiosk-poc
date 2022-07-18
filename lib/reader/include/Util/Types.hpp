#ifndef PR22_TYPES_INCL
#define PR22_TYPES_INCL
/******************************************************************************/
/** \file
 *	General types.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "PrIns/gxCstring.hpp"
#include <sstream>
#include <malloc.h>
#include <stdlib.h>
//------------------------------------------------------------------------------
#ifndef PrOutputString
/// Typedef for PR output string format. It is std::wstring by default and can be
/// overridden by a define directive. e.g. @c @#define @c PrOutputString @c QString.
typedef std::wstring PrOutputString;
#endif
#ifndef ToPrOutputString
/// Conversion class can be declared for PR output string. It is for the case when the
/// PrOutputString class cannot be converted from @c wchar_t array or @c std::wstring data.
/// e.g. @c @#define @c ToPrOutputString @c Util::ToOutString.
#define ToPrOutputString PrOutputString
#endif
//------------------------------------------------------------------------------
namespace Pr22 { namespace Util {

/** Used for automatic conversion of standard string types to PR input string
 * format.
 * \warning Do not create instances of this class. It does not store the string
 * just a pointer to it.
 */
class InputString {
	mutable const void *ptr;
	mutable int type;
	InputString() { }
	InputString(const InputString &) { }
	friend class ToString;
public:
	/// Automatically called conversion
	InputString(const char *str) : ptr(str), type(1) { }
	/// Automatically called conversion
	InputString(const wchar_t *str) : ptr(str), type(2) { }
	/// Automatically called conversion
	InputString(const std::string &str) : ptr(str.c_str()), type(1) { }
	/// Automatically called conversion
	InputString(const std::wstring &str) : ptr(str.c_str()), type(2) { }
#if defined(QSTRING_H) || defined(GX_MANUAL)
	/// Automatically called conversion
	InputString(const QString &str) : ptr(0), type(2) {
		if(sizeof(wchar_t)==sizeof(QChar)) ptr = str.data();
		else {
			ptr = malloc(sizeof(wchar_t)*(str.length()+1));
			str.toWCharArray((wchar_t*)ptr)[(wchar_t*)ptr] = 0;
			type = 3;
		}
	}
#endif
#if defined(_AFX) || defined(__CSTRINGT_H__) || defined(GX_MANUAL)
	/// Automatically called conversion
	InputString(const CString &str) : ptr((LPCTSTR)str), type(sizeof(TCHAR)) { }
#endif

	~InputString() { if(type==3) free((void*)ptr); }

	/// Returns the string
	const char * GetAString() const {
		return type==1 ? (const char*)ptr : 0;
	}

	/// Returns the string
	const wchar_t * GetString() const {
		if(type==1 && ptr) {
			wchar_t *dst;
			size_t len;
			#if _MSC_VER >= 1400
				mbstowcs_s(&len, 0, 0, (const char*)ptr, 0);
			#else
				len = mbstowcs(0, (const char*)ptr, 0);
			#endif
			if(len==(size_t)-1) return 0;
			dst = (wchar_t*)malloc(++len*sizeof(wchar_t));
			#if _MSC_VER >= 1400
				mbstowcs_s(0, dst, len, (const char*)ptr, _TRUNCATE);
			#else
				mbstowcs(dst, (const char*)ptr, len);
			#endif
			type = 3;
			ptr = dst;
		}
		return (const wchar_t*)ptr;
	}
};
//------------------------------------------------------------------------------
/// Used for conversion of integer and floating point values to PR input string format.
class ToString {
	wchar_t buff[64];
public:
	/// Converts integer to PR input string.
	ToString(int i) { PrIns::snwprintf(buff, gx_arraycnt(buff), L"%i", i); }
	/// Converts floating point value to PR input string.
	ToString(float f) { PrIns::snwprintf(buff, gx_arraycnt(buff), L"%g", f); }
	/// Converts floating point value to PR input string.
	ToString(double f) { PrIns::snwprintf(buff, gx_arraycnt(buff), L"%g", f); }
	/// Internally used conversion.
	ToString(const wchar_t *str, int i, int j = 0, int k = 0) {
		PrIns::snwprintf(buff, gx_arraycnt(buff), str, i, j, k);
	}
	/// Returns a PR input string.
	operator const InputString() const { return InputString(buff); }
};
//------------------------------------------------------------------------------
/** Used for conversion to Qt QString or MFC CString.
 * \see ToPrOutputString
 * \warning Do not create instances of this class. It does not store the string
 * just a pointer to it.
 */
class ToOutString {
	const wchar_t *str;
	ToOutString() { }
	ToOutString(const ToOutString &) { }
public:
	/// Converts string data to special output String.
	ToOutString(const wchar_t *str) : str(str) { }
	/// Converts standard string to special output String.
	ToOutString(const std::wstring &str) : str(str.c_str()) { }
#if defined(QSTRING_H) || defined(GX_MANUAL)
	/// Returns Qt QString.
	operator QString() const {
		if(sizeof(wchar_t)==sizeof(QChar))
			return QString((const QChar*)str);
		else
			return QString::fromUcs4((const uint*)str);
	}
#endif
#if defined(_AFX) || defined(__CSTRINGT_H__) || defined(GX_MANUAL)
	/// Returns MFC CString.
	operator CString() const {
		return CString(str);
	}
#endif
};
//------------------------------------------------------------------------------
/// Converts string to integer.
class ToInt {
	int i;
public:
	/// Converts string to integer.
	ToInt(const InputString &str) {
		if(str.GetAString()) i = strtol(str.GetAString(), 0, 0);
		else i = wcstol(str.GetString(), 0, 0);
	}
	/// Returns integer value.
	operator int() const { return i; }
};
//------------------------------------------------------------------------------
/// Converts string to floating point value.
class ToFloat {
	float f;
public:
	/// Converts string to floating point value.
	ToFloat(const InputString &str) {
		if(str.GetAString()) f = (float)strtod(str.GetAString(), 0);
		else f = (float)wcstod(str.GetString(), 0);
	}
	/// Returns floating point value.
	operator float() const { return f; }
};
//------------------------------------------------------------------------------
/** Data for a quadrangle. A quadrangle is in normal orientation if the coordinates
 * of its top left corner (x1, y1) have the lowest values.
 */
struct Frame
{
	int x1;						///< X coordinate of the top left corner
	int y1;						///< Y coordinate of the top left corner
	int x2;						///< X coordinate of the top right corner
	int y2;						///< Y coordinate of the top right corner
	int x3;						///< X coordinate of the right bottom corner
	int y3;						///< Y coordinate of the right bottom corner
	int x4;						///< X coordinate of the left bottom corner
	int y4;						///< Y coordinate of the left bottom corner

	Frame() : x1(0), y1(0), x2(0), y2(0), x3(0), y3(0), x4(0), y4(0) { }

	/// Returns string representation of the frame
	PrOutputString ToString() const {
		std::wostringstream ws;
		ws << L"(" << x1 << L"," << y1 << L"),(" << x2 << L"," << y2 << L"),("
			<< x3 << L"," << y3 << L"),(" << x4 << L"," << y4 << L")";
		return ToPrOutputString(ws.str());
	}
};

} // namespace Util
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_TYPES_INCL
