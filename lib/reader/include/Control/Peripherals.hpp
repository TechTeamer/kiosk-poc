#ifndef PR22_PERIPHERALS_INCL
#define PR22_PERIPHERALS_INCL
/******************************************************************************/
/**	\file
 *	Peripheral controller device module.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Control/Control.hpp"
#include "Util/Util.hpp"
#include "Util/Variant.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/// Peripheral controller device module.
class Peripherals {

public:
	/// Class for storing information on Peripherals.
	class Information {
	public:
		inline PrOutputString GetDeviceName() const;		// Returns the name of the device.
		inline Util::Variant ToVariant() const;				// Returns the internal variant data to query special device information.

	private:

		Util::Variant Info;									// Device information.
		PrOutputString DeviceName;							// Device name.

		friend class Pr22::PrIns::Invoker;
		inline Information(const PrIns::gxModule & handle);	// Returns the user storage data controller class.
	};

public:

	inline PrOutputString ToString() const;					// Returns the name of the device.
	inline Information GetInfo() const;						// Returns the device information.
	inline Control::PowerControl GetPowerControl(void);		// Returns the Control::PowerControl module of the device.
	inline std::vector<Control::StatusLed> GetStatusLeds(void);	// Returns the array of the controllable StatusLed%s.
	inline Control::UserData GetUserData(void);				// Returns the user storage data controller class.

private:

	PrIns::gxModule ApiHandle;								// Api handler.

	friend class Pr22::PrIns::Invoker;
	Peripherals(const PrIns::gxModule & handle): ApiHandle(handle) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Peripherals interface. Now the implementation follows
 ******************************************************************************/

// Constructor for internal usage only.
Peripherals::Information::Information(const PrIns::gxModule & handle) {
	PrIns::CommonGetDeviceInfo op = { 0 };
	PrIns::gxModule(handle).Call(PrIns::ApiCallCodes::GetDeviceInfo, &op);
	Util::Variant Dinfo(op.DevInfo);
	DeviceName = Dinfo.GetListItem(0).ToString();
	Info = Dinfo.GetItemByPath(Util::ToString(L"L/V=%i", (int)Util::HWType::Peripherals));
}

/// Returns the name of the device.
PrOutputString Peripherals::Information::GetDeviceName() const {
	return DeviceName;
}

/// Returns the internal variant data to query special device information.
Util::Variant Peripherals::Information::ToVariant() const {
	return Info;
}

//------------------------------------------------------------------------------

/// Returns the name of the device.
PrOutputString Peripherals::ToString() const {
	return GetInfo().GetDeviceName();
}

/// Returns the device information.
Peripherals::Information Peripherals::GetInfo() const {
	return PrIns::Invoker().Construct(ApiHandle, (Information*)0);
}

/// Returns the Control::PowerControl module of the device.
Control::PowerControl Peripherals::GetPowerControl(void) {
	return PrIns::Invoker().Construct(ApiHandle, (Control::PowerControl*)0);
}

/// Returns the array of the controllable StatusLed%s.
std::vector<Control::StatusLed> Peripherals::GetStatusLeds(void) {
	std::vector<Control::StatusLed> leds;
	std::vector<int> tmp = GetInfo().ToVariant().GetChild(Util::VariantId::StatledList, 0).ToIntArray();
	for(size_t i=0; i<tmp.size(); ++i) {
		leds.push_back(PrIns::Invoker().Construct(ApiHandle, (Control::StatusLed::Color)(tmp[i]), 1<<i, (Control::StatusLed*)0));
	}
	return leds;
}

/// Returns the user storage data controller class.
Control::UserData Peripherals::GetUserData(void) {
	return PrIns::Invoker().Construct(ApiHandle, (Control::UserData*)0);
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_PERIPHERALS_INCL
