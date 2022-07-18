#ifndef PR22_ENUMINFO_INCL
#define PR22_ENUMINFO_INCL
/******************************************************************************/
/** \file
 *	Enum information classes.
 */
/******************************************************************************/
#ifndef __cplusplus
#error C++ compiler required.
#endif
#define PR22_VERSION 2.2.0.19
#include "Util/EnumInfo.inl"
#include "PrIns/gxtypes.hpp"
#include <stdexcept>
//------------------------------------------------------------------------------
namespace Pr22 { namespace Util {

/// Enumeration info storage data type
struct ENUM_INFO {
	const wchar_t *name;		///< Name of an enum value
	int value;					///< Integer representation of an enum value
};
//------------------------------------------------------------------------------
/// Returns information about an enumeration
template<typename ENUM>
class EnumInfo {
public:
	/// Returns the number of values in the enum
	static int Count() { return int(ENUM::gi(-1)-ENUM::gi(0)); }

	/// Returns a value of the enum by ordinal number
	static ENUM GetValue(int ix) {
		ENUM_INFO *info = ENUM::gi(ix);
		if(!info->name) throw std::out_of_range("[pr22] Enum index");
		return (typename ENUM::values)info->value;
	}

	/// Returns the string representation of a value of the enum by ordinal number
	static const wchar_t * GetName(int ix) {
		ENUM_INFO *info = ENUM::gi(ix);
		if(!info->name) throw std::out_of_range("[pr22] Enum index");
		return info->name;
	}

	/// Converts enum value from its string representation.
	template<class CHAR>
	static ENUM Parse(const CHAR *str) {
		ENUM_INFO *info;
		for(int ix=0; (info=ENUM::gi(ix))->name; ix++)
			if(!cmp(info->name, str)) return ENUM(info->value);
		throw std::invalid_argument("[pr22] Enum string not found!");
	}

private:

	template<class T1, class T2>
	static int cmp(const T1 *s1, const T2 *s2) {
		while(*s1==*s2 && *s1) s1++, s2++;
		return *s1-*s2;
	}
};

} // namespace Util
} // namespace Pr22
//------------------------------------------------------------------------------
#define GX_ENUM_BEGIN		GXM_CNC(GX_ENUM_DECLARE_, ENUMSTEP)		///< Macro for enum class generator
#define GXEV				GXM_CNC(GXEV_, ENUMSTEP)				///< Macro for enum class generator
#define GX_ENUM_END			GXM_CNC(GX_ENUM_END_DECL_, ENUMSTEP)	///< Macro for enum class generator
//------------------------------------------------------------------------------
#endif //PR22_ENUMINFO_INCL
