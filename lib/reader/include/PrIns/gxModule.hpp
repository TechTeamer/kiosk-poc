#ifndef PR22_MODULE_INCL
#define PR22_MODULE_INCL
/******************************************************************************/
/** \file
 *	Low level module control.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "PrIns/gxCall.hpp"
#include "Util/Types.hpp"
#include <assert.h>
//------------------------------------------------------------------------------
namespace Pr22 { namespace PrIns {

/// Low level module control class.
class gxModule {
public:

	gxHANDLE handle;					///< Internal variable to store class data.

	inline gxModule();										// Default constructor.

	inline gxModule(const Util::InputString & modulename,
		const Util::InputString & groupname);				// Constructor with module opening.

	inline ~gxModule();										// Destructor.
	inline gxModule(const gxModule &h);						// Copy constructor.
	inline gxModule& operator = (const gxModule &h);		// Assignment operator.
#if GX_RVALUE_REF || defined(GX_MANUAL)
	inline gxModule(gxModule &&h);							// Move constructor.
	inline gxModule& operator = (gxModule &&h);				// Move assignment operator.
#endif
	inline bool IsValid() const;							// Checks if the handle of a module is valid.

	inline PrOutputString GetProperty(const Util::InputString & propname) const;	// Returns a property value.
	inline int SetProperty(const Util::InputString & propname,
		const Util::InputString & propvalue) const;			// Sets a property value.
	inline int SaveProperties(const Util::InputString & propgroup, int level) const;	// Saves the properties of a module

	inline int Call(int cmd, void* params) const;			// Calls the gx system to do some functionality.
	inline static int CallDirect(unsigned int hcode, int cmd, void* params);	// Direct calling of a gx core function.

private:

	inline gxModule& Open(const Util::InputString & modulename,
		const Util::InputString & groupname);				// Opens a gx system compatible module.
	inline void Close();									// Closes a previously opened module.
	inline gxModule& RefHandle();							// References a handle.
};

} //namespace PrIns
} // namespace Pr22
/******************************************************************************
 * This is the end of the gxmodule interface. Now the implementation follows  *
 ******************************************************************************/

#include "Exceptions/Exceptions.hpp"

namespace Pr22 {
namespace PrIns {
#ifndef GX_MANUAL

/* GX call codes */
struct ModuleCallCodes {
enum codes {
	CallCodes		=	0x10030000,

	OpenModule				= CallCodes | 0x0000,
	CloseHandle				= CallCodes | 0x0002,

	RefHandle				= CallCodes | 0x0006,
	GetModuleProperty		= CallCodes | 0x0008,
	SetModuleProperty		= CallCodes | 0x000A,

	SaveModuleProperties	= CallCodes | 0x000e,
	GetModuleProperties		= CallCodes | 0x0008,

	MemoryCallCodes	=	0x10020000,
	LoadBinary				= MemoryCallCodes | 0x0008,
	SaveBinary				= MemoryCallCodes | 0x0009,
};
};
#endif /* GX_MANUAL */

// Default constructor.
gxModule::gxModule() {
	handle.handle = 0;
}

/// Constructor with module opening.
gxModule::gxModule(const Util::InputString & modulename, const Util::InputString & groupname) {
	handle.handle = 0;
	Open(modulename, groupname);
}

// Destructor.
gxModule::~gxModule() {
	try {
		Close();
	}
	catch(...) { }
}

/// Copy constructor.
gxModule::gxModule(const gxModule &h) {
	handle.handle = 0;
	if(h.IsValid()) handle = ((gxModule &)h).RefHandle().handle;
}

/// Assignment operator.
gxModule& gxModule::operator = (const gxModule &h) {
	if(handle.handle==h.handle.handle) return *this;
	Close();
	handle.handle = 0;
	if(h.IsValid()) handle = ((gxModule &)h).RefHandle().handle;
	return *this;
}

#if GX_RVALUE_REF || defined(GX_MANUAL)
/// Move constructor.
gxModule::gxModule(gxModule &&h) {
	handle.handle = h.handle.handle;
	h.handle.handle = 0;
}

/// Move assignment operator.
gxModule& gxModule::operator = (gxModule &&h) {
	if(handle.handle==h.handle.handle) return *this;
	Close();
	handle.handle = h.handle.handle;
	h.handle.handle = 0;
	return *this;
}
#endif

/** Checks if the handle of a module is valid.
 *
 * @return			In case of existing handle the return value is true.
 */
bool gxModule::IsValid() const {
	return handle.handle ? true : false;
}

/** Opens a gx system compatible module.
 *
 * @param modulename	Name of the module.
 * @param groupname		Property place in the property tree.
 * @return				The handle of the opened module.
 */
gxModule& gxModule::Open(const Util::InputString & modulename,
						 const Util::InputString & groupname) {

	Close();
	struct OpenModule {
		const wchar_t	*modulename;
		const wchar_t	*groupname;
		gxHANDLE	 handle;
	} op = { modulename.GetString(), groupname.GetString(), { 0 } };
	CallDirect(0, ModuleCallCodes::OpenModule, &op);
	handle.handle = op.handle.handle;
	return *this;
}

/// Closes a previously opened module.
void gxModule::Close() {
	if(handle.handle) {
		struct CloseHandle {
			gxHANDLE	 handle;
		} cl = { { handle.handle } };
		int err = CallDirect(0, ModuleCallCodes::CloseHandle, &cl);
		assert(!err);
		(void) err;
	}
	handle.handle = 0;
}

/** Returns a property value.
 *
 * @param propname	Name of the property.
 * @return			The string value of the property.
 */
PrOutputString gxModule::GetProperty(const Util::InputString & propname) const {
	wchar_t value[1024] = L"";
	struct GetModuleProperty {
		const wchar_t*		name;
		wchar_t*			value;
		int					maxlen;
	} gp = { propname.GetString(), value, gx_arraycnt(value) };
	if(CallDirect(handle.handle, ModuleCallCodes::GetModuleProperty, &gp))
		return PrOutputString();
	return ToPrOutputString(value);
}

/** Sets a property value.
 *
 * @param propname	Name of the property.
 * @param propvalue	Value of the property.
 * @return			On error thr error code is returned.
 */
int gxModule::SetProperty(const Util::InputString & propname,
								const Util::InputString & propvalue) const {
	struct SetModuleProperty {
		const wchar_t	*name;
		const wchar_t	*value;
	} sp = { propname.GetString(), propvalue.GetString() };
	return CallDirect(handle.handle, ModuleCallCodes::SetModuleProperty, &sp);
}

/** Saves the properties of a module stored in the memory to a branch of the
 * property tree stored on a mass storage device.
 *
 * @param propgroup	Place of property in the tree.
 * @param level		Higher numbers give more details (0 = basic properties, 100 = all properties, -1 = only the changes).
 * @return			On error thr error code is returned.
 */
int gxModule::SaveProperties(const Util::InputString & propgroup, int level) const {
	struct SaveModuleProperties {
		const wchar_t	*name;
		int				 level;
	} mp = { propgroup.GetString(), level };
	return CallDirect(handle.handle, ModuleCallCodes::SaveModuleProperties, &mp);
}

/// Calls the gx system to do some functionality.
int gxModule::Call(int cmd, void* params) const {
	return CallDirect(handle.handle, cmd, params);
}

/// Direct calling of a gx core function.
int gxModule::CallDirect(unsigned int hcode, int cmd, void* params) {
	gxHANDLE qhandle = { hcode }, nullhandle = { 0 };
	if(gxCall::Call(qhandle, cmd, params)) return 0;
	Exceptions::General::ThrowException();
	return gxCall::Call(nullhandle, GX_CALL_GETERROR, 0);
}

/// References a handle.
gxModule& gxModule::RefHandle() {
	assert(handle.handle);
	CallDirect(0, ModuleCallCodes::RefHandle, &handle);
	return *this;
}

} // namespace PrIns
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_MODULE_INCL
