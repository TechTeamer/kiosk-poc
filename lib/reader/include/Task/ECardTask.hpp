#ifndef PR22_ECARDTASK_INCL
#define PR22_ECARDTASK_INCL
/******************************************************************************/
/**	\file
 *	ECard reading task.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "ECardHandling/ECardHandling.hpp"
#include <vector>
namespace Pr22 { class ECardReader; }
//------------------------------------------------------------------------------
namespace Pr22 { namespace Task {

/// List of data reading and authentication requirements.
class ECardTask {
public:

	ECardTask() { }

	// Appends a file to the list.
	inline ECardTask & Add(const ECardHandling::File & file);
	inline ECardTask & Add(const std::vector<ECardHandling::File> & files);
	// Removes a file from the list.
	inline ECardTask & Del(const ECardHandling::File & file);
	/// Returns the required authentication level.
	ECardHandling::AuthLevel GetAuthLevel() const { return SecurityLevel; }
	/// Sets the required authentication level.
	void SetAuthLevel(const ECardHandling::AuthLevel & level) { SecurityLevel = level; }

private:

	ECardHandling::AuthLevel SecurityLevel;		// The required authentication level.
	std::vector<int> files;						// List of data reading requirements.
	friend class Pr22::ECardReader;
};


/** Appends a file to the list.
 *
 * @param file		File identifier.
 * @return			This object.
 */
ECardTask & ECardTask::Add(const ECardHandling::File & file) {
	if(file.Id == ECardHandling::FileId::All) {
		files.clear();
	}
	files.push_back(file.Id);
	return *this;
}

/** Appends an array of files to the list.
 *
 * @param files		Array of file identifiers.
 * @return			This object.
 */
ECardTask & ECardTask::Add(const std::vector<ECardHandling::File> & files) {
	for(size_t i=0; i<files.size(); ++i) Add(files[i]);
	return *this;
}

/** Removes a file from the list.
 *
 * @param file		File identifier.
 * @return			This object.
 */
ECardTask & ECardTask::Del(const ECardHandling::File & file) {
	if(file.Id == ECardHandling::FileId::All) {
		files.clear();
	} else {
		files.push_back(-file.Id);
	}
	return *this;
}

} // namespace Task
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_ECARDTASK_INCL
