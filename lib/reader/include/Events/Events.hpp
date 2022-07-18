#ifndef PR22_EVENTS_INCL
#define PR22_EVENTS_INCL 1
#endif //PR22_EVENTS_INCL
/******************************************************************************/
/**	\file
 *	Event-related enumerations and classes.
 *	\namespace Pr22::Events
 *	Event-related enumerations and classes.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "ECardHandling/ECardHandling.hpp"
#include "Exceptions/Exceptions.hpp"
#include "Imaging/Imaging.hpp"
#include "Processing/Processing.hpp"
#include "Util/Util.hpp"

#define ENUMSTEP PR22_EVENTS_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Events {

/// Event types.
GX_ENUM_BEGIN(Event)
GXEV( UVLedWarming, 0 )			///< Occurs when UV tube warming started.
GXEV( ScanFinished, 1 )			///< Occurs when a scanning task is finished.
GXEV( ImageScanned, 2 )			///< Occurs when an image is scanned.
GXEV( ScanStarted, 3 )			///< Occurs when scanning of a page is started.
GXEV( NoImageInfo, 4 )			///< Occurs when no image captured because it contains no information.
GXEV( PreviewCaptured, 5 )		///< Occurs when a preview (live) image is captured.
GXEV( PresenceDetection, 6 )	///< Occurs when the state of the presence detection is changed.
GXEV( Button, 7 )				///< Occurs when a button of the device is pressed or released.
GXEV( Power, 8 )				///< Occurs when the state of the power is changed.
GXEV( Connection, 9 )			///< Occurs when the used device is disconnected, or when the number of devices is changed while no device is used.
GXEV( DocFrameFound, 10 )		///< Occurs when a new cropped document image is available.
GXEV( AuthBegin, 12 )			///< Occurs when an authentication task of an ECard is started.
GXEV( AuthWaitForInput, 13 )	///< Occurs when an authentication task of an ECard is waiting for some input data.
GXEV( AuthFinished, 14 )		///< Occurs when an authentication task of an ECard is finished.
GXEV( ReadBegin, 15 )			///< Occurs when a reading task of an ECard is started.
GXEV( ReadFinished, 16 )		///< Occurs when a reading task of an ECard is finished.
GXEV( FileChecked, 17 )			///< Occurs when a file hash checking task of an ECard is finished.
GXEV( DeviceUpdate, 18 )		///< Occurs when reading or writing of a file stored on the device is started.
GX_ENUM_END(Event)

} // namespace Events
} // namespace Pr22
#undef ENUMSTEP
#if PR22_EVENTS_INCL == 1
//------------------------------------------------------------------------------
namespace Pr22 { namespace Events {


/** Callback handler base class. Derive the working callback handler from this class.
 *
 * \warning The Task::TaskControl.Wait() method must not be called directly inside
 * the ScanFinished and ReadFinished events. The intended use of that method is to schedule
 * it for later execution (e.g. PostMessage) or call it from another thread. Calling it from
 * a Qt slot is also an appropriate solution.
 *
 * \note The PreviewCaptured and PresenceStateChanged events occur only if the appropriate
 * Task::FreerunTask has been started.
 */
class DocEventHandler {
public:
	virtual ~DocEventHandler() { }

	/// Occurs when a scanning task is finished.
	inline virtual void OnScanFinished(int page, const Exceptions::ErrorCodes & status);

	/// Occurs when an image is scanned.
	inline virtual void OnImageScanned(int page, const Imaging::Light & light);

	/// Occurs when scanning of a page is started.
	inline virtual void OnScanStarted(int page, const Exceptions::ErrorCodes & status);

	/// Occurs when a preview (live) image is captured.
	inline virtual void OnPreviewCaptured(void);

	/// Occurs when the state of the presence detection is changed.
	inline virtual void OnPresenceStateChanged(const Util::PresenceState & state);

	/// Occurs when a button of the device is pressed or released.
	inline virtual void OnKeyPressed(int code, int state);

	/// Occurs when the state of the power is changed.
	inline virtual void OnPowerChanged(int state, int rfu);

	/// Occurs when the used device is disconnected, or when the number of devices is changed while no device is used.
	inline virtual void OnConnection(int devno);

	/// Occurs when a new cropped document image is available.
	inline virtual void OnDocFrameFound(int page, const Exceptions::ErrorCodes & status);

	/// Occurs when an authentication task of an ECard is started.
	inline virtual void OnAuthBegin(const ECardHandling::AuthProcess & authentication);

	/// Occurs when an authentication task of an ECard is waiting for some input data.
	inline virtual void OnAuthWaitForInput(const ECardHandling::AuthProcess & authentication);

	/// Occurs when an authentication task of an ECard is finished.
	inline virtual void OnAuthFinished(const ECardHandling::AuthProcess & authentication,
		const Exceptions::ErrorCodes & status);

	/// Occurs when a reading task of an ECard is started.
	inline virtual void OnReadBegin(const ECardHandling::File & fileId);

	/// Occurs when a reading task of an ECard is finished.
	inline virtual void OnReadFinished(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status);

	/// Occurs when a file hash checking task of an ECard is finished.
	inline virtual void OnFileChecked(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status);

	/// Occurs when reading or writing of a file stored on the device is started.
	inline virtual void OnDeviceUpdate(int part);
};


/******************************************************************************
 * This is the end of the Events interface. Now the implementation follows    *
 ******************************************************************************/

void DocEventHandler::OnScanFinished(int, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnImageScanned(int, const Imaging::Light &) { }
void DocEventHandler::OnScanStarted(int, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnPreviewCaptured(void) { }
void DocEventHandler::OnPresenceStateChanged(const Util::PresenceState &) { }
void DocEventHandler::OnKeyPressed(int, int) { }
void DocEventHandler::OnPowerChanged(int, int) { }
void DocEventHandler::OnConnection(int) { }
void DocEventHandler::OnDocFrameFound(int, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnAuthBegin(const ECardHandling::AuthProcess &) { }
void DocEventHandler::OnAuthWaitForInput(const ECardHandling::AuthProcess &) { }
void DocEventHandler::OnAuthFinished(const ECardHandling::AuthProcess &, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnReadBegin(const ECardHandling::File &) { }
void DocEventHandler::OnReadFinished(const ECardHandling::File &, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnFileChecked(const ECardHandling::File &, const Exceptions::ErrorCodes &) { }
void DocEventHandler::OnDeviceUpdate(int) { }


} // namespace Events
} // namespace Pr22
//------------------------------------------------------------------------------
#undef PR22_EVENTS_INCL
#define PR22_EVENTS_INCL 2
#include "Events/Events.hpp"
#undef PR22_EVENTS_INCL
#define PR22_EVENTS_INCL 3
#endif
