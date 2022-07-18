#ifndef PR22_PRTASK_INCL
#define PR22_PRTASK_INCL
/******************************************************************************/
/**	\file
 *	Document scanner tasks.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Processing.hpp"
#include "Imaging/Imaging.hpp"
#include <vector>
namespace Pr22 { namespace Processing { class Document; class Page; } class DocScanner; }
//------------------------------------------------------------------------------
namespace Pr22 { namespace Task {

/// List of image scanning requirements.
class DocScannerTask {
public:

	/** The object window.
	 * Most devices have only one object window, so this value should not
	 * be modified.
	 */
	int objectWindow;

	DocScannerTask() : objectWindow(0) { }

	// Appends a Light to the list.
	inline DocScannerTask & Add(const Imaging::Light & light);
	// Removes a Light from the list.
	inline DocScannerTask & Del(const Imaging::Light & light);

private:

	std::vector<int> lights;			// List of light requirements.
	friend class Pr22::DocScanner;
};

//------------------------------------------------------------------------------

/// List of field references to read by the engine.
class EngineTask {
public:
	EngineTask() { }
	// Appends a data field reference to the list.
	inline EngineTask & Add(const Processing::FieldSource & group, const Processing::FieldId & id);
	inline EngineTask & Add(const Processing::FieldReference & field);
	// Removes a data field reference from the list.
	inline EngineTask & Del(const Processing::FieldSource & group, const Processing::FieldId & id);
	inline EngineTask & Del(const Processing::FieldReference & field);

private:

	std::vector<int> fields;			// List of field requirements.
	friend class Pr22::Processing::Document;
	friend class Pr22::Processing::Page;
};

//------------------------------------------------------------------------------

/// DocScanner freerun mode tasks.
class FreerunTask {
public:

	/// Declares a live stream scanning task.
	static FreerunTask LiveStream(void) { return FreerunTask(0x579ea8); }
	/// Declares a document presence detection task.
	static FreerunTask Detection(void) { return FreerunTask(0xde7ec7); }

private:

	int task;							// Task Id.
	friend class Pr22::DocScanner;
	FreerunTask(int t) : task(t) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Pr Task interface. Now the implementation follows   *
 ******************************************************************************/

/** Appends a Light to the list.
 *
 * @param light		The requested Light.
 * @return			This object.
 */
DocScannerTask & DocScannerTask::Add(const Imaging::Light & light) {
	if(light == Imaging::Light::All) {
		lights.clear();
	}
	lights.push_back(light);
	return *this;
}

/** Removes a Light from the list.
 *
 * @param light		The Light not to scan.
 * @return			This object.
 */
DocScannerTask & DocScannerTask::Del(const Imaging::Light & light) {
	if(light == Imaging::Light::All) {
		lights.clear();
	} else {
		lights.push_back(-light);
	}
	return *this;
}

/** Appends a data field reference to the list.
 *
 * @param group		Id of the FieldSource.
 * @param id		FieldId.
 * @return			This object.
 */
EngineTask & EngineTask::Add(const Processing::FieldSource & group, const Processing::FieldId & id) {
	return Add(Processing::FieldReference(group, id));
}

/** Appends a data field reference to the list.
 *
 * @param field		Data FieldReference.
 * @return			This object.
 */
EngineTask & EngineTask::Add(const Processing::FieldReference & field) {
	if(!field.code) {
		fields.clear();
	}
	fields.push_back(field.code);
	return *this;
}

/** Removes a data field reference from the list.
 *
 * @param group		Id of the FieldSource.
 * @param id		FieldId.
 * @return			This object.
 */
EngineTask & EngineTask::Del(const Processing::FieldSource & group, const Processing::FieldId & id) {
	return Del(Processing::FieldReference(group, id));
}

/** Removes a data field reference from the list.
 *
 * @param field		Data FieldReference.
 * @return			This object.
 */
EngineTask & EngineTask::Del(const Processing::FieldReference & field) {
	if(!field.code) {
		fields.clear();
	} else {
		fields.push_back(-field.code);
	}
	return *this;
}

} // namespace Task
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_PRTASK_INCL
