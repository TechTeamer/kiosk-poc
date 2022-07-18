#ifndef PR22_INCL
#define PR22_INCL
/******************************************************************************/
/**	\file
 *	Central class for a document reader device.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Imaging/DocScanner.hpp"
#include "Processing/Engine.hpp"
#include "Control/Peripherals.hpp"
#include "ECardHandling/ECardReader.hpp"
#include "Events/Events.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/** Central class for a document reader device.
 *
 * A device is composed of several modules. These modules can be
 * accessed through this class.
 */
class DocumentReaderDevice {
public:

	inline DocumentReaderDevice();										// Initializes the Document Reader software.
	inline DocumentReaderDevice(const Util::InputString & proppath);	// Initializes the Document Reader software.
	inline static std::vector<PrOutputString> GetDeviceList(void);		// Collects a list of device names.
	inline DocumentReaderDevice & UseDevice(int devno);					// Starts using a device.
	inline DocumentReaderDevice & UseDevice(const Util::InputString & devname);	// Starts using a device.
	inline PrOutputString GetDeviceName();								// Returns the name of the device.
	inline PrOutputString GetVersion(char component);					// Returns version information of the API.
	inline void Close(void);											// Ends using of the current device.
	inline void SetProperty(const Util::InputString & property, const Util::InputString & value);	// Sets a property value of the Passport Reader system.
	inline PrOutputString GetProperty(const Util::InputString & property);	// Returns a property value of the Passport Reader system.
	inline ECardHandling::Certificates GetGlobalCertificateManager();	// Returns the global certificate manager.
	inline ECardHandling::Certificates GetCertificateManager();			// Returns the certificate manager.
	inline Peripherals GetPeripherals();								// Returns the device peripheral controller.
	inline DocScanner & GetScanner();									// Returns the document scanner.
	inline std::vector<ECardReader> & GetReaders();						// Returns a list of the card readers.
	inline Engine & GetEngine();										// Returns the data analyzer.
	inline void AddEventHandler(const Events::Event & event, Events::DocEventHandler * handler);	// Sets an event handler object.
	inline void RemoveEventHandler(const Events::Event & event, Events::DocEventHandler * handler);	// Removes an event handler object.
	inline void TriggerEvent(const Events::Event & event);				// Triggers an event.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	std::vector<DocScanner> scanner;	// The scanner module.
	std::vector<ECardReader> readers;	// List of card reader modules.
	std::vector<Engine> engine;			// The data analyzer module.
	int event_filter;					// Used events.
	std::vector<Events::DocEventHandler*> events[19];		// The registered event handlers.

	inline void init(const wchar_t * proppath);				// Initializes the class.
	inline static void GXAPI EventFunction(int event, int param1, int param2, void* uparam);	// Global event function.
};

/******************************************************************************
 * This is the end of the DocumentReaderDevice interface. Now the implementation follows
 ******************************************************************************/

/// Initializes the Document Reader software.
DocumentReaderDevice::DocumentReaderDevice() : event_filter(0) {
	init(L"default");
}

/** Initializes the Document Reader software with a specified property path.
 *
 * Specified path means the initial data are read from the gxsd.dat file
 * from the specified xml node.
 *
 * @param proppath	Name of the xml node of the gxsd.dat file from
 *					where the initial data should be read.
 */
DocumentReaderDevice::DocumentReaderDevice(const Util::InputString & proppath) : event_filter(0) {
	init(proppath.GetString());
}

// Initializes the class.
void DocumentReaderDevice::init(const wchar_t * proppath) {

	ApiHandle = PrIns::gxModule(L"prapi", proppath);
	ApiHandle.SetProperty(L"sdk", L"2.2");
	ApiHandle.SetProperty(L"event_filter", Util::ToString(event_filter));
	ApiHandle.SetProperty(L"async_callback", L"1");
	PrIns::prSetEventFunction sef = { (void*)&EventFunction, this };
	ApiHandle.Call(PrIns::ApiCallCodes::SetEventFunction, &sef);
}

/** Collects a list of device names for UseDevice(const Util::InputString & devname) method.
 *
 * @return			List of device names.
 */
std::vector<PrOutputString> DocumentReaderDevice::GetDeviceList(void) {
	std::vector<PrOutputString> devices;
	PrIns::CommonGetDeviceList gdl = { L"PR", 0, 0 };
	PrIns::gxModule prHandle(L"prapi", L"default");
	if(prHandle.Call(PrIns::ApiCallCodes::GetDeviceList, &gdl))
		return devices;
	for(int i=0; i<gdl.ndevices; i++) {
		devices.push_back(ToPrOutputString(((wchar_t**)gdl.devices)[i]));
	}
	PrIns::GlobalData(gdl.devices);
	return devices;
}

/** Starts using a device.
 *
 * @param devno		Ordinal number of the device to use.
 * @return			This object.
 */
DocumentReaderDevice & DocumentReaderDevice::UseDevice(int devno) {
	std::vector<PrOutputString> DevNames = GetDeviceList();
	if(devno<0) {
		throw Exceptions::InvalidParameter(L"[pr22] (devno)");
	}
	if(devno>=(int)DevNames.size()) {
		throw Exceptions::NoSuchDevice(L"[pr22]");
	}
	return UseDevice(DevNames[(size_t)devno]);
}

/** Starts using a device.
 *
 * The list of available names can be requested from the GetDeviceList().
 *
 * @param devname	Name of the device to use.
 * @return			This object.
 */
DocumentReaderDevice & DocumentReaderDevice::UseDevice(const Util::InputString & devname) {
	scanner.clear();
	engine.clear();
	readers.clear();

	PrIns::CommonUseDevice param = { devname.GetString(), 2 /* FULL CONTROL */ };
	ApiHandle.Call(PrIns::ApiCallCodes::UseDevice, &param);

	scanner.push_back(PrIns::Invoker().Construct(ApiHandle, (DocScanner*)0));
	engine.push_back(PrIns::Invoker().Construct(ApiHandle, (Engine*)0));

	PrIns::CommonGetDeviceInfo gdi = { 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetDeviceInfo, &gdi);

	Util::Variant vm = Util::Variant(gdi.DevInfo)[0];
	for(int ix=0; ix<vm.NChildren(); ix++) {
		Util::Variant v = vm.GetChildByIndex(ix);
		switch(v.ToInt()) {
		case Util::HWType::RFIDReader:
		case Util::HWType::ContactReader:
			{
			std::wstring CardReaderName = v.GetChild(Util::VariantId::DeviceId, 0).ToUnicode();
			if(!CardReaderName.length()) continue;
			readers.push_back(PrIns::Invoker().Construct(ApiHandle, CardReaderName, v, (ECardReader*)0));
			}
			break;
		case Util::HWType::MagneticReader:
			readers.push_back(PrIns::Invoker().Construct(ApiHandle, L"MS Reader", v, (ECardReader*)0));
			break;
		}
	}
	return *this;
}

/// Returns the name of the device.
PrOutputString DocumentReaderDevice::GetDeviceName() {
	try {
		PrIns::CommonGetDeviceInfo gdi = { 0 };
		if(ApiHandle.Call(PrIns::ApiCallCodes::GetDeviceInfo, &gdi))
			return PrOutputString();
		return Util::Variant(gdi.DevInfo)[0].ToString();
	}
	catch(const Exceptions::General &) {
		return PrOutputString();
	}
}

/** Returns version information of the API.
 *
 * @param component	\li Character @c 'A' for version of the interface files,
 *					\li or character @c 'S' for version of the Passport Reader system.
 * @return			Version string.
 */
PrOutputString DocumentReaderDevice::GetVersion(char component) {
	if(component=='A')
		return ToPrOutputString(GXM_CNC(L, GXM_I2S(PR22_VERSION)));
	if(component=='S')
		return ApiHandle.GetProperty(L"api_version");
	return ToPrOutputString(L"-");
}

/// Ends using of the current device.
void DocumentReaderDevice::Close(void) {
	scanner.clear();
	engine.clear();
	readers.clear();
	if(ApiHandle.IsValid()) {
		PrIns::CommonUseDevice ud = { 0, 0 };
		ApiHandle.Call(PrIns::ApiCallCodes::UseDevice, &ud);
	}
}

/** Sets a property value of the Passport Reader system.
 *
 * The names and values of the properties are documented in the
 * Passport Reader reference manual.
 *
 * @param property	The name of the property.
 * @param value		The value of the property.
 */
void DocumentReaderDevice::SetProperty(const Util::InputString & property, const Util::InputString & value) {
	ApiHandle.SetProperty(property, value);
}

/** Returns a property value of the Passport Reader system.
 *
 * The names and values of the properties are documented in the
 * Passport Reader reference manual.
 *
 * @param property	The name of the property.
 * @return			The value of the property.
 */
PrOutputString DocumentReaderDevice::GetProperty(const Util::InputString & property) {
	return ApiHandle.GetProperty(property);
}

/** Returns the certificate manager that stores certificates for
 * all task of the entire process.
 */
ECardHandling::Certificates DocumentReaderDevice::GetGlobalCertificateManager() {
	return PrIns::Invoker().Construct(ApiHandle, PrIns::CertScope::Global, L"", (ECardHandling::Certificates*)0);
}

/** Returns the certificate manager that stores certificates for
 * task of the current DocumentReaderDevice device.
 */
ECardHandling::Certificates DocumentReaderDevice::GetCertificateManager() {
	return PrIns::Invoker().Construct(ApiHandle, PrIns::CertScope::DocumentReader, GetPeripherals().ToString(), (ECardHandling::Certificates*)0);
}

/** Returns the device peripheral controller module of the used
 * DocumentReaderDevice.
 */
Peripherals DocumentReaderDevice::GetPeripherals() {
	return PrIns::Invoker().Construct(ApiHandle, (Peripherals*)0);
}

/// Returns the document scanner module of the used DocumentReaderDevice.
DocScanner & DocumentReaderDevice::GetScanner() {
	return scanner[0];
}

/** Returns a list of all the electronic card reader (Smartcard, RFID card or
* Magnetic stripe card) module(s) of the used DocumentReaderDevice.
*/
std::vector<ECardReader> & DocumentReaderDevice::GetReaders() {
	return readers;
}

/// Returns the data analyzer module of the used DocumentReaderDevice.
Engine & DocumentReaderDevice::GetEngine() {
	return engine[0];
}

void GXAPI DocumentReaderDevice::EventFunction(int event, int param1, int param2, void* uparam) {

	DocumentReaderDevice *drd = (DocumentReaderDevice*)uparam;
	if(event<0 || event>=(int)gx_arraycnt(drd->events)) return;

	std::vector<Events::DocEventHandler*> &hlist = drd->events[event];

	using Events::Event;
	for(size_t ix=0; ix<hlist.size(); ix++) {

		if(event == Event::ScanFinished)
			hlist[ix]->OnScanFinished(param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::ImageScanned)
			hlist[ix]->OnImageScanned(param1, (Imaging::Light)param2);
		if(event == Event::ScanStarted)
			hlist[ix]->OnScanStarted(param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::PreviewCaptured)
			hlist[ix]->OnPreviewCaptured();
		if(event == Event::PresenceDetection)
			hlist[ix]->OnPresenceStateChanged((Util::PresenceState)param1);
		if(event == Event::Button)
			hlist[ix]->OnKeyPressed(param1, param2);
		if(event == Event::Power)
			hlist[ix]->OnPowerChanged(param1, param2);
		if(event == Event::Connection)
			hlist[ix]->OnConnection(param1);
		if(event == Event::DocFrameFound)
			hlist[ix]->OnDocFrameFound(param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::AuthBegin)
			hlist[ix]->OnAuthBegin((ECardHandling::AuthProcess)param1);
		if(event == Event::AuthWaitForInput)
			hlist[ix]->OnAuthWaitForInput((ECardHandling::AuthProcess)param1);
		if(event == Event::AuthFinished)
			hlist[ix]->OnAuthFinished((ECardHandling::AuthProcess)param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::ReadBegin)
			hlist[ix]->OnReadBegin((ECardHandling::File)param1);
		if(event == Event::ReadFinished)
			hlist[ix]->OnReadFinished((ECardHandling::File)param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::FileChecked)
			hlist[ix]->OnFileChecked((ECardHandling::File)param1, (Exceptions::ErrorCodes)param2);
		if(event == Event::DeviceUpdate)
			hlist[ix]->OnDeviceUpdate(param1);

	}
}

/** Sets an event handler object to a specified event.
 *
 * @param event		The specified Events::Event to handle.
 * @param handler	An object derived from Events::DocEventHandler class that contains
 *					the involved event handler method.
 */
void DocumentReaderDevice::AddEventHandler(const Events::Event & event, Events::DocEventHandler * handler) {
	if(event<0 || event>=(int)gx_arraycnt(events) || !handler) return;

	bool no_old_handlers = events[event].empty();
	events[event].push_back(handler);
	TriggerEvent(event);

	if(no_old_handlers) {
		event_filter |= 1 << event;
		if(ApiHandle.IsValid()) ApiHandle.SetProperty(L"event_filter", Util::ToString(event_filter));
	}
}

/** Removes an event handler object from a specified event.
 *
 * @param event		The specified Events::Event to handle.
 * @param handler	A previously registered object.
 */
void DocumentReaderDevice::RemoveEventHandler(const Events::Event & event, Events::DocEventHandler * handler) {
	if(event<0 || event>=(int)gx_arraycnt(events)) return;

	std::vector<Events::DocEventHandler*> &ev = events[event];
	std::vector<Events::DocEventHandler*>::iterator it;
	for(it=ev.begin(); it!=ev.end(); ++it) if(*it==handler) { ev.erase(it); break; }
	if(ev.empty()) {
		event_filter &= ~(1 << event);
		if(ApiHandle.IsValid()) ApiHandle.SetProperty(L"event_filter", Util::ToString(event_filter));
	}
}

/** Triggers an event.
 *
 * @param event		The specified Events::Event to trigger. Not all the events can be triggered.
 */
void DocumentReaderDevice::TriggerEvent(const Events::Event & event) {
	if(event<0 || event>=(int)gx_arraycnt(events) || events[event].empty()) return;
	if(ApiHandle.IsValid()) ApiHandle.SetProperty(L"trigger_event", Util::ToString(1 << event));
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_INCL
