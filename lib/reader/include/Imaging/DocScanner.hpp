#ifndef PR22_DOCSCANNER_INCL
#define PR22_DOCSCANNER_INCL
/******************************************************************************/
/**	\file
 *	Document scanner module.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Page.hpp"
#include "Task/Task.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/// Document scanner module.
class DocScanner {
public:

	/// Class for storing information on a DocScanner.
	class Information {
	public:
		inline std::vector<Imaging::Light> GetLights() const;	// Returns the list of usable Lights.
		inline PrOutputString GetVersion(char component) const;	// Returns version information of the scanner.
		inline bool IsCalibrated() const;						// Returns if the scanner is calibrated.
		inline bool IsCalibrated(int objectWindow) const;		// Returns if an object window of the scanner is calibrated.
		template<class TRECT> inline TRECT GetSize(int objectWindow, bool whtype = true) const;	// Returns the size of an object window of the scanner.
		inline int GetWindowCount() const;						// Returns the number of object windows of the scanner.

	private:

		PrIns::gxModule ApiHandle;							// Api handler.
		Util::Variant Info;									// Scanner information.

		friend class Pr22::PrIns::Invoker;
		inline Information(const PrIns::gxModule &handle);	// Constructor for internal usage only.
	};

	inline Information GetInfo() const;						// Returns the scanner information.
	inline Processing::Page Scan(const Task::DocScannerTask & task,
						const Imaging::PagePosition & pos);	// Takes images from an object window of the scanner.
	inline Imaging::RawImage GetPreview(void);				// Returns the last captured preview image.
	inline Processing::Page GetPage(int pageindex);			// Returns a previously scanned page.
	inline Task::TaskControl StartScanning(const Task::DocScannerTask & task,
						const Imaging::PagePosition & pos);	// Takes images from an object window of the scanner.
	inline Task::TaskControl StartTask(const Task::FreerunTask & task);	// Starts a task declared by the task parameter.
	inline void CleanUpData(void);							// Removes all the internally stored data (images, documents) from the system.
	inline void CleanUpLastPage(void);						// Removes the internally stored data (images, documents) of the last page from the system.
	inline Processing::Document LoadDocument(const Processing::BinData & document);	// Loads the data of a "Root document" saved earlier.

private:

	PrIns::gxModule ApiHandle;								// Api handler.

	friend class Pr22::PrIns::Invoker;
	DocScanner(const PrIns::gxModule & handle) : ApiHandle(handle) { }	// Constructor for internal usage only.
};
/******************************************************************************
 * This is the end of the DocScanner interface. Now the implementation follows
 ******************************************************************************/

// Constructor for internal usage only.
DocScanner::Information::Information(const PrIns::gxModule &handle) : ApiHandle(handle) {
	PrIns::CommonGetDeviceInfo op = { 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetDeviceInfo, &op);
	Info = Util::Variant(op.DevInfo).GetItemByPath(Util::ToString(L"L/V=%i", Util::HWType::Scanner));
}

/** Returns the list of usable Light%s.
 *
 * @return			The Imaging::Light list.
 */
std::vector<Imaging::Light> DocScanner::Information::GetLights() const {
	std::vector<Imaging::Light> Lights;
	std::vector<int> tmp = Info.GetChild(Util::VariantId::Light, 0).ToIntArray();
	for(size_t i=0; i<tmp.size(); i++) Lights.push_back((Imaging::Light)tmp[i]);
	return Lights;
}

/** Returns version information of the scanner.
 *
 * @param component	\li Character @c 'H' for version of the hardware, \li character @c 'F' for version of
 *					the firmware, \li or character @c 'S' for version of the device handling software library.
 * @return			Version information of the scanner.
 */
PrOutputString DocScanner::Information::GetVersion(char component) const {
	try {
		return Info.GetChild(Util::VariantId::Version, 0).GetListItem(Util::VariantPos::ById(component)).ToString();
	}
	catch(const Exceptions::General &) {
		return ToPrOutputString(L"-");
	}
}

/** Returns if the scanner is calibrated.
 *
 * @return			True if the scanner is calibrated.
 */
bool DocScanner::Information::IsCalibrated() const {
	return IsCalibrated(-1);
}

/** Returns if an object window of the scanner is calibrated.
 *
 * See also GetWindowCount().
 *
 * @param objectWindow	Ordinal number of the object window of the scanner.
 * @return				True if the object window is calibrated.
 */
bool DocScanner::Information::IsCalibrated(int objectWindow) const {
	PrIns::prIsCalibrated op = { (objectWindow + 1) << 24 };
	try {
		return !ApiHandle.Call(PrIns::ApiCallCodes::Iscalibrated, &op);
	}
	catch(const Exceptions::EntryNotFound &) {
		return false;
	}
}

/** Returns the size of an object window of the scanner.
 *
 * See also GetWindowCount().
 *
 * @param objectWindow	Ordinal number of the object window of the scanner.
 * @param whtype		Set this value to true if class TRECT has a constructor
 *						with four integers as left, top, width, height like Qt QRect.
 *						Set this value to false if class TRECT has a constructor
 *						with four integers as left, top, right, bottom like MFC CRect.
 * @return				A rectangle of the object window in micrometers.
 */
template<class TRECT>
TRECT DocScanner::Information::GetSize(int objectWindow, bool whtype /* true */) const {

	Util::Variant vfr = Info.GetChild(Util::VariantId::WindowList, 0)[objectWindow];
	Util::Frame fr = vfr.ToFrame();
	if(whtype)
		return TRECT(fr.x1, fr.y1, fr.x3 - fr.x1, fr.y3 - fr.y1);
	else
		return TRECT(fr.x1, fr.y1, fr.x3, fr.y3);
}


/** Returns the number of object windows of the scanner.
 *
 * More object windows are available only in double-page document readers
 * and in devices equipped with photo camera. For scanning with photo camera, use
 * Imaging::Light::Photo instead of handling object window.
 * @return	The number of object windows of the scanner.
 */
int DocScanner::Information::GetWindowCount() const {
	return Info.GetChild(Util::VariantId::WindowList, 0).NItems();
}

//------------------------------------------------------------------------------

/// Returns the scanner information.
DocScanner::Information DocScanner::GetInfo() const {
	return PrIns::Invoker().Construct(ApiHandle, (Information*)0);
}

/** Takes images from an object window of the scanner.
 *
 * See also Information.GetWindowCount.
 *
 * @param task		List of lights and the object window Id.
 * @param pos		The target page.
 * @return			List of the scanned images.
 */
Processing::Page DocScanner::Scan(const Task::DocScannerTask & task, const Imaging::PagePosition & pos) {
	struct prScan {
		const int *lights;
		int nlight;
		int window;
		int position;
		int reqid;
	} op = { (int*)PrIns::vector_data(task.lights), (int)task.lights.size(), task.objectWindow, pos, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::Scan, &op);
	return GetPage((Util::ToInt) ApiHandle.GetProperty(L"act_page"));
}

/** Returns the last captured preview image.
 *
 * \note To get preview images a Task::FreerunTask.LiveStream task should be
 * started before.
 *
 * @return			The requested Imaging::RawImage.
 */
Imaging::RawImage DocScanner::GetPreview(void) {
	PrIns::prGetImage gi = { 0, 0, PrIns::ImageType::Preview, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetImage, &gi);
	return Imaging::RawImage().FromVariant(Util::Variant(gi.Img));
}

/** Returns a previously scanned page.
 *
 * @param pageindex		Ordinal number of the requested page.
 * @return				The Processing::Page.
 */
Processing::Page DocScanner::GetPage(int pageindex) {
	PrIns::prGetDocumentRootV gdr = { 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetDocRootV, &gdr);
	Util::Variant imagelist = Util::Variant(gdr.Document).GetChild(Util::VariantId::PageList, 0)[pageindex];
	return PrIns::Invoker().Construct(ApiHandle, pageindex, imagelist, (Processing::Page*)0);
}

/** Takes images from an object window of the scanner in a background thread.
 *
 * \note After the process is finished the Task::TaskControl.Wait method must be called.
 *
 * @param task		List of lights and the object window Id.
 * @param pos		The target page selector.
 * @return			A Task::TaskControl object.
 */
Task::TaskControl DocScanner::StartScanning(const Task::DocScannerTask & task, const Imaging::PagePosition & pos) {
	struct prStartScanning {
		const int *lights;
		int nlight;
		int window;
		int position;
		int reqid;
	} op = { (int*)PrIns::vector_data(task.lights), (int)task.lights.size(), task.objectWindow, pos, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::StartScanning, &op);
	return PrIns::Invoker().Construct(ApiHandle, op.reqid, (Task::TaskControl*)0);
}

/** Starts a task declared by the task parameter.
 *
 * \note After the process is finished the Task::TaskControl.Wait method must be called.
 *
 * @param task		The Task::FreerunTask to start.
 * @return			A Task::TaskControl object.
 */
Task::TaskControl DocScanner::StartTask(const Task::FreerunTask & task) {
	PrIns::cmProcess op = { task.task, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::StartFrvTask, &op);
	return PrIns::Invoker().Construct(ApiHandle, op.ProcessId, (Task::TaskControl*)0);
}

/// Removes all the internally stored data (images, documents) from the system.
void DocScanner::CleanUpData(void) {
	PrIns::prResetDocument rd = { 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::ResetDocument, &rd);
}

/** Removes the internally stored data (images, documents) of the last page from the system.
 *
 * Removing the last page is useful when a wrong page has scanned, e.g. because
 * the scanned image is blurred or accidentally wrong document page has been
 * scanned.
 */
void DocScanner::CleanUpLastPage(void) {
	PrIns::prResetDocument rd = { 1 };
	ApiHandle.Call(PrIns::ApiCallCodes::ResetDocument, &rd);
}

/** Loads the data of a @e "Root document" saved earlier.
 *
 * \note This method removes all the internally stored data (images, documents)
 * from the system.
 *
 * @param document	Data to load.
 * @return			Root document represents all the loaded data.
 * @see Processing.Document.Save
 */
Processing::Document DocScanner::LoadDocument(const Processing::BinData & document) {
	struct prLoadDocumentFromMem {
		PrIns::gxVARIANT Doc;
		const gxu8*		Buffer;
		int				BufLen;
	} op = { 0, document.RawData(), document.RawSize() };
	ApiHandle.Call(PrIns::ApiCallCodes::LoadDocumentFromMem, &op);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(op.Doc), (Processing::Document*)0);
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_DOCSCANNER_INCL
