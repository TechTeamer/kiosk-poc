#ifndef PR22_CONTROL_INCL
#define PR22_CONTROL_INCL
/******************************************************************************/
/**	\file
 *	Controller classes of miscellaneous hardware modules.
 *	\namespace Pr22::Control
 *	Controller classes of miscellaneous hardware modules.
 *	\namespace Pr22
 *	%Control classes of device modules.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/BinData.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Control {

/// Power controller module.
class PowerControl {
public :
	inline PowerControl & Suspend(void);	// Suspends the device.
	inline PowerControl & WakeUp(void);		// Wakes up the device.

private:

	PrIns::gxModule ApiHandle;				// Api handler.

	friend class Pr22::PrIns::Invoker;
	PowerControl(const PrIns::gxModule & handle) : ApiHandle(handle) { }	// Constructor for internal usage only.
};

//------------------------------------------------------------------------------

/// User storage data controller class.
class UserData {
public:
	inline int GetSize(void);				// Returns the size of the user data.
	inline std::vector<gxu8> Read(void);	// Returns the stored user data.
	inline UserData & Write(const gxu8 *source, int srcsize);	// Writes the user data to the device.

private:

	PrIns::gxModule ApiHandle;				// Api handler.

	friend class Pr22::PrIns::Invoker;
	UserData(const PrIns::gxModule & handle) : ApiHandle(handle) { }	// Constructor for internal usage only.
};

//------------------------------------------------------------------------------

/// Controller for the status leds and buzzers.
class StatusLed {
public:
	#define ENUMSTEP 1
	/**	Usable status led colors. Both led types and led states are enumerated.
	*	Led type provides information and cannot be set. Led state is for setting.
	*/
	GX_ENUM_BEGIN(Color)
	#include "Control/slc.inl"
	GX_ENUM_END(Color)
	#undef ENUMSTEP

	inline Color GetLight() const;		// Returns the led type.
	inline StatusLed & Turn(Color color = Color::On);	// Sets the led state.
	inline StatusLed & Blink(const int* sample, int ssample, int iteration);	// Starts a led blinking or buzzer beeping background task.
	inline StatusLed & Blink(const Color & color, const int* sample, int ssample,
		int iteration);					// Starts a led blinking or buzzer beeping background task.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	Color _color;						// Color capability.
	int index;							// Ordinal number of led.

	friend class Pr22::PrIns::Invoker;
	StatusLed(const PrIns::gxModule & handle, const Color & color, int index) : ApiHandle(handle),
		_color(color), index(index) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Control interface. Now the implementation follows   *
 ******************************************************************************/

/// Suspends the device.
PowerControl & PowerControl::Suspend(void) {
	ApiHandle.Call(PrIns::ApiCallCodes::Suspend, NULL);
	return *this;
}

/// Wakes up the device.
PowerControl & PowerControl::WakeUp(void) {
	ApiHandle.Call(PrIns::ApiCallCodes::WakeUp, NULL);
	return *this;
}

//------------------------------------------------------------------------------

/// Returns the size of the user data.
int UserData::GetSize(void) {
	PrIns::prGetUserDataInfo udi = { 0, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::UserDataInfo, &udi);
	return udi.NBlocks * udi.SBlock;
}

/// Returns the stored user data.
std::vector<gxu8> UserData::Read(void) {
	PrIns::prGetUserDataInfo udi = { 0, 0 };
	if(ApiHandle.Call(PrIns::ApiCallCodes::UserDataInfo, &udi))
		return std::vector<gxu8>();

	struct prReadUserData {
		int FirstBlock;
		int NBlocks;
		gxu8* Data;
	} rud = { 0, udi.NBlocks, 0 };
	if(ApiHandle.Call(PrIns::ApiCallCodes::ReadUserData, &rud))
		return std::vector<gxu8>();

	PrIns::GlobalData tmp(rud.Data);
	return std::vector<gxu8>(rud.Data, rud.Data + udi.NBlocks * udi.SBlock);
}

/** Writes the user data to the device.
 *
 * @param source	The data to store.
 * @param srcsize	Size of the input data.
 * @return			This object.
 */
UserData & UserData::Write(const gxu8 *source, int srcsize) {
	PrIns::prGetUserDataInfo udi = { 0, 0 };
	if(ApiHandle.Call(PrIns::ApiCallCodes::UserDataInfo, &udi))
		return *this;

	int size = udi.NBlocks * udi.SBlock;
	if(srcsize > size) {
		throw Exceptions::DataOutOfRange(L"[pr22] data length");
	}

	std::vector<gxu8> tb;
	if(srcsize < size) {
		tb.insert(tb.end(), source, source+srcsize);
		tb.insert(tb.end(), size-srcsize, 0);
		source = (gxu8*)PrIns::vector_data(tb);
	}

	struct prWriteUserData {
		int FirstBlock;
		int NBlocks;
		const void* Data;
	} wud = { 0, udi.NBlocks, source };
	ApiHandle.Call(PrIns::ApiCallCodes::WriteUserData, &wud);
	return *this;
}

//------------------------------------------------------------------------------

/// Returns the led type.
StatusLed::Color StatusLed::GetLight() const {
	return _color;
}

/** Sets the led state.
 *
 * @param color		Color to switch to.
 */
StatusLed & StatusLed::Turn(StatusLed::Color color /* Color::On */) {
	if(_color == Color::System) {
		throw Exceptions::ProgramFault(L"[pr22] This led is controlled by the system!");
	}
	if(_color == Color::Buzzer) {
		throw Exceptions::InvalidFunction(L"[pr22] Buzzer state cannot set!");
	}
	if(color != Color::On && color != Color::Off) {
		switch(_color) {
		case Color::OG:
			if(color != Color::Orange && color != Color::Green) {
				throw Exceptions::InvalidParameter(L"[pr22] Invalid color!");
			}
			break;
		case Color::ORG:
			if(color != Color::Orange && color != Color::Red && color != Color::Green) {
				throw Exceptions::InvalidParameter(L"[pr22] Invalid color!");
			}
			break;
		default:
			if(color != _color) {
				throw Exceptions::InvalidParameter(L"[pr22] Invalid color!");
			}
			break;
		}
	}
	struct prSetStatusLed {
		int LedMask;
		int Color;
	} op = { index, color };
	ApiHandle.Call(PrIns::ApiCallCodes::SetStatus, &op);
	return *this;
}

/** Starts a led blinking or buzzer beeping background task.
 *
 * @param sample	Durations of ON and OFF states in milliseconds.
 * @param ssample	Number of entries in sample array.
 * @param iteration	Number of repetitions (0 stands for infinite).
 */
StatusLed & StatusLed::Blink(const int* sample, int ssample, int iteration) {
	return Blink(Color::On, sample, ssample, iteration);
}

/** Starts a led blinking or buzzer beeping background task.
 *
 * @param color		The Color to blink.
 * @param sample	Durations of ON and OFF states in milliseconds.
 * @param ssample	Number of entries in sample array.
 * @param iteration	Number of repetitions (0 stands for infinite).
 */
StatusLed & StatusLed::Blink(const Color & color, const int* sample, int ssample, int iteration) {
	struct prBlinkStatus {
		int Coding;
		int Iteration;
		int Mask;
		int Type;
		int DataLength;
		const int* Data;
	} op = { 1, iteration, index, color, ssample, sample };
	ApiHandle.Call(PrIns::ApiCallCodes::BlinkStatus, &op);
	return *this;
}

#define ENUMSTEP 2
GX_ENUM_BEGIN(StatusLed::Color)
#include "Control/slc.inl"
GX_ENUM_END(StatusLed::Color)
#undef ENUMSTEP

} // namespace Control
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_CONTROL_INCL
