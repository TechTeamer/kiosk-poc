#ifndef PR22_EXCEPTION_INCL
#define PR22_EXCEPTION_INCL 1
#endif//PR22_EXCEPTION_INCL
/******************************************************************************/
/**	\file
 *	Exception classes and error identification constants.
 *	\namespace Pr22::Exceptions
 *	Exception classes and error identification constants.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#define ENUMSTEP PR22_EXCEPTION_INCL
#include "Util/EnumInfo.hpp"
#include "PrIns/gxCall.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Exceptions {

/// System error codes
GX_ENUM_BEGIN(ErrorCodes)

	/* System error codes */
	GXEV(NoErr, 		0x0000 ) ///< No error

	GXEV(NoEnt, 		0x0002 ) ///< Entry not found (ENOENT)
	GXEV(NoMem ,		0x000c ) ///< Memory allocation error (ENOMEM)
	GXEV(Acces,			0x000d ) ///< Permission denied (EACCES)
	GXEV(Fault,			0x000e ) ///< Bad address or program error (EFAULT)
	GXEV(Busy,			0x0010 ) ///< Resource busy (EBUSY)
	GXEV(Exist,			0x0011 ) ///< File already exists (EEXIST)
	GXEV(NoDev, 		0x0013 ) ///< No such device (ENODEV)
	GXEV(Inval, 		0x0016 ) ///< Invalid parameter (EINVAL)
	GXEV(Range, 		0x0022 ) ///< Data out of range (ERANGE)
	GXEV(Data,			0x003D ) ///< No data available (Linux - ENODATA)
	GXEV(Comm,			0x0046 ) ///< Communication error (Linux - ECOMM)
	GXEV(TimedOut,		0x006E ) ///< Function timed out (Linux - ETIMEDOUT)

	/* General error codes */
	GXEV(Open,			0x1000 ) ///< File open error
	GXEV(Creat,			0x1001 ) ///< File creation error
	GXEV(Read,			0x1002 ) ///< File read error
	GXEV(Write,			0x1003 ) ///< File write error
	GXEV(File,			0x1004 ) ///< Invalid data content

	GXEV(InvImg,		0x1010 ) ///< Invalid image
	GXEV(InvFunc,		0x1011 ) ///< Invalid function

	GXEV(HWKey,			0x1012 ) ///< Hardware key does not work properly
	GXEV(Version,		0x1013 ) ///< Invalid version
	GXEV(Assert,		0x1014 ) ///< Assertion occurred
	GXEV(Discon,		0x1015 ) ///< Device is disconnected
	GXEV(ImgProc,		0x1016 ) ///< Image processing failed
	GXEV(Auth,			0x1017 ) ///< Authenticity cannot be determined

	GXEV(Capture,		0x20088001 ) ///< Image capture error
	GXEV(WeakDev,		0x20088005 ) ///< Insufficient hardware configuration e.g.USB 1.0 port
	GXEV(CertExpired,	0x20088030 ) ///< Certificate is expired
	GXEV(CertRevoked,	0x20088031 ) ///< Certificate is revoked
	GXEV(Check,			0x20088032 ) ///< Validation checking failed

GX_ENUM_END(ErrorCodes)

} // namespace Exceptions
} // namespace Pr22
#undef ENUMSTEP
#if PR22_EXCEPTION_INCL == 1
//------------------------------------------------------------------------------
namespace Pr22 { namespace Exceptions {

/// Base exception class for error handling
class General
{
public:
	General() : erc(ErrorCodes::NoErr), lasterror(0) { }
	/// New exception
	General(const wchar_t *const &errstr) : erc(-1), lasterror(0) {
		_set(errstr);
	}
	/// New exception
	General(ErrorCodes code, const wchar_t *const &errstr) : erc(code), lasterror(0) {
		_set(errstr);
	}
	/// New exception
	General(const General &ex) : erc(ex.erc), lasterror(0) {
		_set(ex.what());
	}
	/// Copies an exception
	General& operator = (const General &ex) {
		erc = ex.erc;
		_set(ex.what());
		return *this;
	}
	virtual ~General() {
		if(lasterror) free(lasterror);
	}
	/// Returns the last error message
	virtual const wchar_t * what() const {
		return lasterror ? lasterror : L"[pr22] No error message specified!";
	}
	/// Conversion operator
	operator ErrorCodes() const {
		return erc;
	}

	/// Throws an exception
	static void ThrowException(ErrorCodes ecode, const wchar_t *const &errstr);

#ifndef GX_MANUAL
	static void ThrowException();

protected:
	/// Sets the error message
	inline void _set(const wchar_t *errstr) {
		if(lasterror) free(lasterror);
		lasterror = PrIns::wcsdup(errstr);
	}
#endif

private:
	ErrorCodes erc;
	wchar_t *lasterror;
};
//------------------------------------------------------------------------------
/// Entry not found exception
class EntryNotFound : public General
{
public:
	/// New exception
	EntryNotFound(const wchar_t *const &errstr) : General(ErrorCodes::NoEnt, errstr) { }
	/// New exception
	EntryNotFound(const EntryNotFound &ex) : General(ex) { }
	/// Copies an exception
	EntryNotFound& operator = (const EntryNotFound &ex) { return _set(ex.what()), *this; }
};

/// Memory allocation exception
class MemoryAllocation : public General
{
public:
	/// New exception
	MemoryAllocation(const wchar_t *const &errstr) : General(ErrorCodes::NoMem, errstr) { }
	/// New exception
	MemoryAllocation(const MemoryAllocation &ex) : General(ex) { }
	/// Copies an exception
	MemoryAllocation& operator = (const MemoryAllocation &ex) { return _set(ex.what()), *this; }
};

/// Permission denied exception
class PermissionDenied : public General
{
public:
	/// New exception
	PermissionDenied(const wchar_t *const &errstr) : General(ErrorCodes::Acces, errstr) { }
	/// New exception
	PermissionDenied(const PermissionDenied &ex) : General(ex) { }
	/// Copies an exception
	PermissionDenied& operator = (const PermissionDenied &ex) { return _set(ex.what()), *this; }
};

/// Program fault exception
class ProgramFault : public General
{
public:
	/// New exception
	ProgramFault(const wchar_t *const &errstr) : General(ErrorCodes::Fault, errstr) { }
	/// New exception
	ProgramFault(const ProgramFault &ex) : General(ex) { }
	/// Copies an exception
	ProgramFault& operator = (const ProgramFault &ex) { return _set(ex.what()), *this; }
};

/// Resource busy exception
class ResourceBusy : public General
{
public:
	/// New exception
	ResourceBusy(const wchar_t *const &errstr) : General(ErrorCodes::Busy, errstr) { }
	/// New exception
	ResourceBusy(const ResourceBusy &ex) : General(ex) { }
	/// Copies an exception
	ResourceBusy& operator = (const ResourceBusy &ex) { return _set(ex.what()), *this; }
};

/// No such device exception
class NoSuchDevice : public General
{
public:
	/// New exception
	NoSuchDevice(const wchar_t *const &errstr) : General(ErrorCodes::NoDev, errstr) { }
	/// New exception
	NoSuchDevice(const NoSuchDevice &ex) : General(ex) { }
	/// Copies an exception
	NoSuchDevice& operator = (const NoSuchDevice &ex) { return _set(ex.what()), *this; }
};

/// Invalid parameter exception
class InvalidParameter : public General
{
public:
	/// New exception
	InvalidParameter(const wchar_t *const &errstr) : General(ErrorCodes::Inval, errstr) { }
	/// New exception
	InvalidParameter(const InvalidParameter &ex) : General(ex) { }
	/// Copies an exception
	InvalidParameter& operator = (const InvalidParameter &ex) { return _set(ex.what()), *this; }
};

/// Data out of range exception
class DataOutOfRange : public General
{
public:
	/// New exception
	DataOutOfRange(const wchar_t *const &errstr) : General(ErrorCodes::Range, errstr) { }
	/// New exception
	DataOutOfRange(const DataOutOfRange &ex) : General(ex) { }
	/// Copies an exception
	DataOutOfRange& operator = (const DataOutOfRange &ex) { return _set(ex.what()), *this; }
};

/// No data available exception
class NoDataAvailable : public General
{
public:
	/// New exception
	NoDataAvailable(const wchar_t *const &errstr) : General(ErrorCodes::Data, errstr) { }
	/// New exception
	NoDataAvailable(const NoDataAvailable &ex) : General(ex) { }
	/// Copies an exception
	NoDataAvailable& operator = (const NoDataAvailable &ex) { return _set(ex.what()), *this; }
};

/// Communication error exception
class CommunicationError : public General
{
public:
	/// New exception
	CommunicationError(const wchar_t *const &errstr) : General(ErrorCodes::Comm, errstr) { }
	/// New exception
	CommunicationError(const CommunicationError &ex) : General(ex) { }
	/// Copies an exception
	CommunicationError& operator = (const CommunicationError &ex) { return _set(ex.what()), *this; }
};

/// Function timed out exception
class FunctionTimedOut : public General
{
public:
	/// New exception
	FunctionTimedOut(const wchar_t *const &errstr) : General(ErrorCodes::TimedOut, errstr) { }
	/// New exception
	FunctionTimedOut(const FunctionTimedOut &ex) : General(ex) { }
	/// Copies an exception
	FunctionTimedOut& operator = (const FunctionTimedOut &ex) { return _set(ex.what()), *this; }
};

/// Invalid image exception
class InvalidImage : public General
{
public:
	/// New exception
	InvalidImage(const wchar_t *const &errstr) : General(ErrorCodes::InvImg, errstr) { }
	/// New exception
	InvalidImage(const InvalidImage &ex) : General(ex) { }
	/// Copies an exception
	InvalidImage& operator = (const InvalidImage &ex) { return _set(ex.what()), *this; }
};

/// Invalid function exception
class InvalidFunction : public General
{
public:
	/// New exception
	InvalidFunction(const wchar_t *const &errstr) : General(ErrorCodes::InvFunc, errstr) { }
	/// New exception
	InvalidFunction(const InvalidFunction &ex) : General(ex) { }
	/// Copies an exception
	InvalidFunction& operator = (const InvalidFunction &ex) { return _set(ex.what()), *this; }
};

/// Hardware key exception
class HardwareKey : public General
{
public:
	/// New exception
	HardwareKey(const wchar_t *const &errstr) : General(ErrorCodes::HWKey, errstr) { }
	/// New exception
	HardwareKey(const HardwareKey &ex) : General(ex) { }
	/// Copies an exception
	HardwareKey& operator = (const HardwareKey &ex) { return _set(ex.what()), *this; }
};

/// Invalid version exception
class InvalidVersion : public General
{
public:
	/// New exception
	InvalidVersion(const wchar_t *const &errstr) : General(ErrorCodes::Version, errstr) { }
	/// New exception
	InvalidVersion(const InvalidVersion &ex) : General(ex) { }
	/// Copies an exception
	InvalidVersion& operator = (const InvalidVersion &ex) { return _set(ex.what()), *this; }
};

/// Assertion occurred exception
class AssertionOccurred : public General
{
public:
	/// New exception
	AssertionOccurred(const wchar_t *const &errstr) : General(ErrorCodes::Assert, errstr) { }
	/// New exception
	AssertionOccurred(const AssertionOccurred &ex) : General(ex) { }
	/// Copies an exception
	AssertionOccurred& operator = (const AssertionOccurred &e) { return _set(e.what()), *this; }
};

/// Device is disconnected exception
class DeviceIsDisconnected : public General
{
public:
	/// New exception
	DeviceIsDisconnected(const wchar_t *const &errstr) : General(ErrorCodes::Discon, errstr) { }
	/// New exception
	DeviceIsDisconnected(const DeviceIsDisconnected &ex) : General(ex) { }
	/// Copies an exception
	DeviceIsDisconnected& operator = (const DeviceIsDisconnected &ex) { return _set(ex.what()), *this; }
};

/// Image processing failed exception
class ImageProcessingFailed : public General
{
public:
	/// New exception
	ImageProcessingFailed(const wchar_t *const &errstr) : General(ErrorCodes::ImgProc, errstr) { }
	/// New exception
	ImageProcessingFailed(const ImageProcessingFailed &ex) : General(ex) { }
	/// Copies an exception
	ImageProcessingFailed& operator = (const ImageProcessingFailed &ex) { return _set(ex.what()), *this; }
};

/// Authenticity failed exception
class AuthenticityFailed : public General
{
public:
	/// New exception
	AuthenticityFailed(const wchar_t *const &errstr) : General(ErrorCodes::Auth, errstr) { }
	/// New exception
	AuthenticityFailed(const AuthenticityFailed &ex) : General(ex) { }
	/// Copies an exception
	AuthenticityFailed& operator = (const AuthenticityFailed &ex) { return _set(ex.what()), *this; }
};

/// File exception
class FileException : public General
{
public:
	/// New exception
	FileException(const wchar_t *const &errstr) : General(ErrorCodes::File, errstr) { }
	/// New exception
	FileException(ErrorCodes code, const wchar_t *const &errstr) : General(code, errstr) { }
	/// New exception
	FileException(const FileException &ex) : General(ex) { }
	/// Copies an exception
	FileException& operator = (const FileException &ex) { return _set(ex.what()), *this; }
};

/// File exists exception
class FileExists : public FileException
{
public:
	/// New exception
	FileExists(const wchar_t *const &errstr) : FileException(ErrorCodes::Exist, errstr) { }
	/// New exception
	FileExists(const FileExists &ex) : FileException(ex) { }
	/// Copies an exception
	FileExists& operator = (const FileExists &ex) { return _set(ex.what()), *this; }
};

/// File open exception
class FileOpen : public FileException
{
public:
	/// New exception
	FileOpen(const wchar_t *const &errstr) : FileException(ErrorCodes::Open, errstr) { }
	/// New exception
	FileOpen(const FileOpen &ex) : FileException(ex) { }
	/// Copies an exception
	FileOpen& operator = (const FileOpen &ex) { return _set(ex.what()), *this; }
};

/// File creation exception
class FileCreation : public FileException
{
public:
	/// New exception
	FileCreation(const wchar_t *const &errstr) : FileException(ErrorCodes::Creat, errstr) { }
	/// New exception
	FileCreation(const FileCreation &ex) : FileException(ex) { }
	/// Copies an exception
	FileCreation& operator = (const FileCreation &ex) { return _set(ex.what()), *this; }
};

/// File read exception
class FileRead : public FileException
{
public:
	/// New exception
	FileRead(const wchar_t *const &errstr) : FileException(ErrorCodes::Read, errstr) { }
	/// New exception
	FileRead(const FileRead &ex) : FileException(ex) { }
	/// Copies an exception
	FileRead& operator = (const FileRead &ex) { return _set(ex.what()), *this; }
};

/// File write exception
class FileWrite : public FileException
{
public:
	/// New exception
	FileWrite(const wchar_t *const &errstr) : FileException(ErrorCodes::Write, errstr) { }
	/// New exception
	FileWrite(const FileWrite &ex) : FileException(ex) { }
	/// Copies an exception
	FileWrite& operator = (const FileWrite &ex) { return _set(ex.what()), *this; }
};

/// Invalid file content exception
class InvalidFileContent : public FileException
{
public:
	/// New exception
	InvalidFileContent(const wchar_t *const &errstr) : FileException(ErrorCodes::File, errstr) { }
	/// New exception
	InvalidFileContent(const InvalidFileContent &ex) : FileException(ex) { }
	/// Copies an exception
	InvalidFileContent& operator = (const InvalidFileContent &ex) { return _set(ex.what()), *this; }
};

/// Image scan failed exception
class ImageScanFailed : public General
{
public:
	/// New exception
	ImageScanFailed(const wchar_t *const &errstr) : General(ErrorCodes::Capture, errstr) { }
	/// New exception
	ImageScanFailed(const ImageScanFailed &ex) : General(ex) { }
	/// Copies an exception
	ImageScanFailed& operator = (const ImageScanFailed &ex) { return _set(ex.what()), *this; }
};

/// Insufficient hardware configuration
class InsufficientHardware : public General
{
public:
	/// New exception
	InsufficientHardware(const wchar_t *const &errstr) : General(ErrorCodes::WeakDev, errstr) { }
	/// New exception
	InsufficientHardware(const InsufficientHardware &ex) : General(ex) { }
	/// Copies an exception
	InsufficientHardware& operator = (const InsufficientHardware &ex) { return _set(ex.what()), *this; }
};

/// Certificate expired exception
class CertificateExpired : public General
{
public:
	/// New exception
	CertificateExpired(const wchar_t *const &errstr) : General(ErrorCodes::CertExpired, errstr) { }
	/// New exception
	CertificateExpired(const CertificateExpired &ex) : General(ex) { }
	/// Copies an exception
	CertificateExpired& operator = (const CertificateExpired &ex) { return _set(ex.what()), *this; }
};

/// Certificate revoked exception
class CertificateRevoked : public General
{
public:
	/// New exception
	CertificateRevoked(const wchar_t *const &errstr) : General(ErrorCodes::CertRevoked, errstr) { }
	/// New exception
	CertificateRevoked(const CertificateRevoked &ex) : General(ex) { }
	/// Copies an exception
	CertificateRevoked& operator = (const CertificateRevoked &ex) { return _set(ex.what()), *this; }
};

/// Validation checking failed exception
class ValidationCheckingFailed : public General
{
public:
	/// New exception
	ValidationCheckingFailed(const wchar_t *const &errstr) : General(ErrorCodes::Check, errstr) { }
	/// New exception
	ValidationCheckingFailed(const ValidationCheckingFailed &ex) : General(ex) { }
	/// Copies an exception
	ValidationCheckingFailed& operator = (const ValidationCheckingFailed &ex) { return _set(ex.what()), *this; }
};
//------------------------------------------------------------------------------
#ifndef GX_MANUAL
/// Throws an exception
inline void General::ThrowException()
{
	int		ecode;
	wchar_t	errstr[1024];
	PrIns::gxHANDLE nullhandle = { 0 };
	PrIns::GetErrorMessage me;
	me.errstr = errstr;
	me.maxlen = gx_arraycnt(errstr);
	ecode = PrIns::gxCall::Call(nullhandle, PrIns::GX_CALL_GETERROR, &me);
	ThrowException(ErrorCodes(ecode), errstr);
}
#endif

// Throws an exception
inline void General::ThrowException(ErrorCodes ecode, const wchar_t *const &errstr)
{
	switch(ecode)
	{
	case ErrorCodes::NoErr:
		throw General(ErrorCodes::NoErr, L"[pr22] Error is not specified!");
	case ErrorCodes::NoEnt:
		throw EntryNotFound(errstr);
	case ErrorCodes::NoMem:
		throw MemoryAllocation(errstr);
	case ErrorCodes::Acces:
		throw PermissionDenied(errstr);
	case ErrorCodes::Fault:
		throw ProgramFault(errstr);
	case ErrorCodes::Busy:
		throw ResourceBusy(errstr);
	case ErrorCodes::Exist:
		throw FileExists(errstr);
	case ErrorCodes::NoDev:
		throw NoSuchDevice(errstr);
	case ErrorCodes::Inval:
		throw InvalidParameter(errstr);
	case ErrorCodes::Range:
		throw DataOutOfRange(errstr);
	case ErrorCodes::Data:
		throw NoDataAvailable(errstr);
	case ErrorCodes::Comm:
		throw CommunicationError(errstr);
	case ErrorCodes::TimedOut:
		throw FunctionTimedOut(errstr);
	case ErrorCodes::InvImg:
		throw InvalidImage(errstr);
	case ErrorCodes::InvFunc:
		throw InvalidFunction(errstr);
	case ErrorCodes::HWKey:
		throw HardwareKey(errstr);
	case ErrorCodes::Version:
		throw InvalidVersion(errstr);
	case ErrorCodes::Assert:
		throw AssertionOccurred(errstr);
	case ErrorCodes::Discon:
		throw DeviceIsDisconnected(errstr);
	case ErrorCodes::ImgProc:
		throw ImageProcessingFailed(errstr);
	case ErrorCodes::Auth:
		throw AuthenticityFailed(errstr);
	case ErrorCodes::Open:
		throw FileOpen(errstr);
	case ErrorCodes::Creat:
		throw FileCreation(errstr);
	case ErrorCodes::Read:
		throw FileRead(errstr);
	case ErrorCodes::Write:
		throw FileWrite(errstr);
	case ErrorCodes::File:
		throw InvalidFileContent(errstr);
	case ErrorCodes::Capture:
		throw ImageScanFailed(errstr);
	case ErrorCodes::WeakDev:
		throw InsufficientHardware(errstr);
	case ErrorCodes::CertExpired:
		throw CertificateExpired(errstr);
	case ErrorCodes::CertRevoked:
		throw CertificateRevoked(errstr);
	case ErrorCodes::Check:
		throw ValidationCheckingFailed(errstr);
	default:
		throw General(ecode, errstr);
	}
}

} // namespace Exceptions
} // namespace Pr22
//------------------------------------------------------------------------------
#undef PR22_EXCEPTION_INCL
#define PR22_EXCEPTION_INCL 2
#include "Exceptions/Exceptions.hpp"
#undef PR22_EXCEPTION_INCL
#define PR22_EXCEPTION_INCL 3
#endif //PR22_EXCEPTION_INCL
