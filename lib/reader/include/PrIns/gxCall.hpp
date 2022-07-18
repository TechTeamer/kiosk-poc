#ifndef PR22_CALL_INCL
#define PR22_CALL_INCL
/******************************************************************************/
/** \file
 *	GX system loader.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "PrIns/gxCstring.hpp"
#include <errno.h>
#if defined WIN32
	#include <windows.h>
#elif defined LINUX
	#include <dlfcn.h>
#endif
//------------------------------------------------------------------------------
namespace Pr22 { namespace PrIns {

/// Handle of the GX module
struct gxHANDLE {
	gxu32 handle;	///< Handle of the module. If it is zero, it is invalid.
};

/// Calls the gx system to do some functionality.
class gxCall {
public:
	~gxCall() { Close(); }

	inline int CallFunc(gxHANDLE handle, int function, void *params);	// Calls a module process with the specified parameters.
	inline void Init(void);									// Initialize the gx system binary.
	inline void Close(void);								// Close the gx system binary.

	static int Call(gxHANDLE handle, int function, void *params) {
		static gxCall inst;
		return inst.CallFunc(handle, function, params);
	}

private:

	gxCall() : modulename(0), call(0), handle(0) {
		errstr[0] = 0;
	}
	inline bool SetError(const wchar_t *msg, const char *par = "", bool q = false);	// Sets gx system error message.
	inline bool Open(void);									// Opens the gx system binary.
	inline void * Address(const char *sig);					// Returns address from the gx system binary.

	const char *modulename;				// Name of the library.
	wchar_t errstr[256];				// Error buffer.
	void *call;							// Main function pointer.

	/// Handle of the opened gxsd library.
#ifdef LINUX
	void *handle;
#else
	HMODULE handle;
#endif

};
/******************************************************************************
 * This is the end of the gxcall interface. Now the implementation follows    *
 ******************************************************************************/

enum GX_CALL_ERROR_CODES {
	GX_CALL_GETERROR				= 0x10010000,
};

/// Error message retrieving structure.
struct GetErrorMessage {
	int				 errcode;
	wchar_t			*errstr;
	int				 maxlen;
};

/** Calls a module process with the specified parameters.
 *
 * @param handle	Handle of an opened module or special system handle.
 * @param function	Identifier of the function.
 * @param params	Parameters to give for the function.
 * @return			Module defined, usually non-zero on success.
 */
int gxCall::CallFunc(gxHANDLE handle, int function, void *params)
{
	if(!call) Init();
	if(call) return ((int(GXAPI*)(gxHANDLE, int, void*))call)(handle, function, params);	/* GXSD library is loaded */

	/* GXSD library not loaded */
	switch(function) {
	case GX_CALL_GETERROR:
		if(params) {
			GetErrorMessage *mp = (GetErrorMessage*)params;
			mp->errcode = ENOENT;
			if(mp->errstr && mp->maxlen>0) {
				PrIns::snwprintf(mp->errstr, (size_t)mp->maxlen, L"gxsdldr: open error in %hs (%ls)",
					modulename, errstr);
			}
		}
		return ENOENT;
	default:
		/* Unknown function */
		errno = EINVAL;
		return 0;
	}
}

/// Sets gx system error message.
bool gxCall::SetError(const wchar_t *msg, const char *par /* "" */, bool q /* false */) {
	PrIns::snwprintf(errstr, gx_arraycnt(errstr), msg, par);
#if defined(WIN32) && defined(_DEBUG)
	if(MessageBoxW(0, errstr, L"GXSD Loader", UINT((q?MB_RETRYCANCEL:MB_OK) | MB_TASKMODAL | MB_ICONERROR)) == IDRETRY) return true;
#else
	q = !q;
#endif
	return false;
}

/// Opens the gx system binary.
bool gxCall::Open(void) {
#ifdef LINUX
	if(!handle) {
		modulename = "libgxsd.so.7";
		handle = dlopen(modulename, RTLD_NOW | RTLD_GLOBAL);
	}
	if(!handle) {
#ifdef LINUX64
		modulename = "/usr/lib64/libgxsd.so.7";
#else
		modulename = "/usr/lib32/libgxsd.so.7";
#endif
		handle = dlopen(modulename, RTLD_NOW | RTLD_GLOBAL);
	}
	if(!handle) {
		modulename = "/usr/lib/libgxsd.so.7";
		handle = dlopen(modulename, RTLD_NOW | RTLD_GLOBAL);
	}
	if(!handle) SetError(L"%hs", dlerror());
	return handle!=0;
#else
	modulename = "gxsd7.dll";
	for(;;) {
		handle = (HMODULE)LoadLibraryA(modulename);
		if(handle) break;
		if(!SetError(L"Could not load: %hs", modulename, true)) return false;
	}
	return true;
#endif
}

/// Initialize the gx system binary.
void gxCall::Init(void)
{
	if(!Open()) return;
	errstr[0] = 0;

	call = Address("gx_call");
	if(!call) {
		SetError(L"No 'gx_call' entry found.");
		Close();
		return;
	}

	int *vi = (int*)Address("gxmoduleinfo");
	if(vi && (vi[2]<0x07000000)) {
		SetError(L"Old version of %hs.", modulename);
		Close();
		return;
	}

	errstr[0] = 0;
}

/// Close the gx system binary.
void gxCall::Close(void) {
	call = 0;
	if(handle) {
#ifdef LINUX
		dlclose(handle);
#else
		FreeLibrary(handle);
#endif
		handle = 0;
	}
}

/// Returns address from the gx system binary.
void * gxCall::Address(const char *sig) {
#ifdef LINUX
	return dlsym(handle, sig);
#else
	return (void*)GetProcAddress(handle, sig);
#endif
}

} // namespace PrIns
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_CALL_INCL
