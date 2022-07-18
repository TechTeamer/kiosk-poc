#ifndef PR22_PRINS_INCL
#define PR22_PRINS_INCL 1
#endif//PR22_PRINS_INCL
/******************************************************************************/
/**	\file
 *	Internally used api call codes and parameters.
 *	\namespace Pr22::PrIns
 *	Internally used api call codes and parameters.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19

#define ENUMSTEP PR22_PRINS_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace PrIns {

GX_ENUM_BEGIN(ImageType)
GXEV( Original, 0 )
GXEV( Document, 1 )
GXEV( Preview, 2 )
GXEV( Finger, 3)
GX_ENUM_END(ImageType)

GX_ENUM_BEGIN(CertScope)
GXEV( Global, 0 )
GXEV( DocumentReader, 1 )
GXEV( CardReader, 2)
GXEV( Card, 3)
GX_ENUM_END(CertScope)

} // namespace PrIns
} // namespace Pr22
#undef ENUMSTEP
#if PR22_PRINS_INCL == 1
//------------------------------------------------------------------------------
namespace Pr22 { namespace PrIns {

struct GX_VARIANT;
typedef struct GX_VARIANT* gxVARIANT;

struct CommonGetDeviceList {
	const wchar_t* filter;
	void* devices;
	int ndevices;
};

struct CommonUseDevice {
	const wchar_t* DeviceName;
	int Mode;
};

struct CommonGetDeviceInfo {
	gxVARIANT DevInfo;
};

struct prGetDocumentRootV {
	gxVARIANT Document;
};

struct prResetDocument {
	int lastpage;
};

struct prGetImage {
	int Page;
	int Light;
	int Type;
	gxVARIANT Img;
};

struct prIsCalibrated {
	int WindowId;
};

struct prRfidOpenDevice {
	const wchar_t* Device;
	gxVARIANT DevInfo;
};

struct cmProcess {
	int ProcessId;
	int Status;
};

struct prSetEventFunction {
	void *func;
	void *param;
};

struct prGetUserDataInfo {
	int NBlocks;
	int SBlock;
};

struct ApiCallCodes {
enum CommonApi {
	GROUPCOMMON = 0x2008c000,
	GetDeviceList = GROUPCOMMON,
	UseDevice = GROUPCOMMON + 1,
	SetEventFunction = GROUPCOMMON + 2,
	GetDeviceInfo = GROUPCOMMON + 3,
	SelfTest = GROUPCOMMON + 4,
	GetProcStatus = GROUPCOMMON + 5,
	StopProc = GROUPCOMMON + 6,
	WaitProc = GROUPCOMMON + 7,
	Suspend = GROUPCOMMON + 8,
	WakeUp = GROUPCOMMON + 9,
	SetStatus = GROUPCOMMON + 10,
	BlinkStatus = GROUPCOMMON + 11,
	StartFrvTask = GROUPCOMMON + 15,
};

enum PrApi {
	GROUPPRAPI = 0x20080000,
	Capture = GROUPPRAPI + 13,
	CaptureStart = GROUPPRAPI + 14,
	Iscalibrated = GROUPPRAPI + 61,
	SetPagelight = GROUPPRAPI + 62,
	GetLightmask = GROUPPRAPI + 65,
	GetRfidFile = GROUPPRAPI + 73,
	UserDataInfo = GROUPPRAPI + 94,
	ReadUserData = GROUPPRAPI + 95,
	WriteUserData = GROUPPRAPI + 96,
	GetOCRV = GROUPPRAPI + 97,
	GetFieldImageV = GROUPPRAPI + 107,
	CompareFaceV = GROUPPRAPI + 108,
	GetDocRootV = GROUPPRAPI + 113,
	ConnectRfidCard = GROUPPRAPI + 118,
	DisconnectRfidCard = GROUPPRAPI + 120,
	CheckFileHash = GROUPPRAPI + 128,
	GetImage = GROUPPRAPI + 130,
	GetRfidCardInfo = GROUPPRAPI + 134,
	InitTerminalAuth = GROUPPRAPI + 142,
	CompleteTerminalAuth = GROUPPRAPI + 144,
	DecodeLatentImage = GROUPPRAPI + 154,
	GetNextAuth = GROUPPRAPI + 159,
	LoadCertFromStore = GROUPPRAPI + 165,
	DoAuth = GROUPPRAPI + 170,
	GetApplications = GROUPPRAPI + 174,
	ResetDocument = GROUPPRAPI + 176,
	GetFileStart = GROUPPRAPI + 177,
	SaveDocumentToMem = GROUPPRAPI + 182,
	LoadDocumentFromMem = GROUPPRAPI + 183,
	Analyze = GROUPPRAPI + 184,
	GetReaderCardList = GROUPPRAPI + 186,
	GetFileList = GROUPPRAPI + 187,
	ClearCertListScope = GROUPPRAPI + 191,
	LoadCertScope = GROUPPRAPI + 192,
	OpenCardReader = GROUPPRAPI + 193,
	CloseCardReader = GROUPPRAPI + 194,
	ConvertFileNames = GROUPPRAPI + 195,
	GetAuthResult = GROUPPRAPI + 196,
	GetLicenses = GROUPPRAPI+197,
	GetLicenseDate = GROUPPRAPI+198,
	GetEngineInfo = GROUPPRAPI+199,
	MergeDocuments = GROUPPRAPI+200,
	AnalyzeC = GROUPPRAPI + 201,
	GetDataFormat = GROUPPRAPI + 202,
	StartScanning = GROUPPRAPI+203,
	StartEcardReading = GROUPPRAPI+204,
	Scan = GROUPPRAPI+207,
};
};


class Invoker {
public:

	template<class T, class P1>
	T Construct(const P1 & p1, const T *) {
		return T(p1);
	}

	template<class T, class P1, class P2>
	T Construct(const P1 & p1, const P2 & p2, const T *) {
		return T(p1, p2);
	}

	template<class T, class P1, class P2, class P3>
	T Construct(const P1 & p1, const P2 & p2, const P3 & p3, const T *) {
		return T(p1, p2, p3);
	}

	template<class T, class P1, class P2, class P3, class P4>
	T Construct(const P1 & p1, const P2 & p2, const P3 & p3, const P4 & p4, const T *) {
		return T(p1, p2, p3, p4);
	}
};

} // namespace PrIns
} // namespace Pr22
//------------------------------------------------------------------------------
#undef PR22_PRINS_INCL
#define PR22_PRINS_INCL 2
#include "PrIns/PrIns.hpp"
#undef PR22_PRINS_INCL
#define PR22_PRINS_INCL 3
#endif
