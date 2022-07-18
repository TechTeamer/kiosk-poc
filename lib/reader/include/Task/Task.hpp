#ifndef PR22_TASK_INCL
#define PR22_TASK_INCL
/******************************************************************************/
/**	\file
 *	%Task data and task control classes.
 *	\namespace Pr22::Task
 *	%Task data and task control classes.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "PrIns/PrIns.hpp"
#include "PrIns/gxModule.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Task {

/// Process controller class.
class TaskControl {
public:

	TaskControl() : pid(0) { }			// Default constructor.
	inline TaskControl& Stop(void);		// Stops a previously started background task.
	inline void Wait(void);				// Waits for a previously started background task.
	inline int GetState(void) const;	// Returns the state of the previously started background task.
	inline bool IsValid() const;		// Checks if the task control is valid.

private:

	PrIns::gxModule apiHandle;			// Api handler.
	int pid;							// Task Id.

	friend class Pr22::PrIns::Invoker;
	TaskControl(const PrIns::gxModule & handle, int id) : apiHandle(handle), pid(id) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Task Control interface. Now the implementation follows
 ******************************************************************************/

/// Stops a previously started background task.
TaskControl& TaskControl::Stop(void) {
	if(!pid) return *this;
	PrIns::cmProcess op = { pid, 0 };
	apiHandle.Call(PrIns::ApiCallCodes::StopProc, &op);
	pid = op.ProcessId;
	return *this;
}

/**	Waits for a previously started background task.
 * \note This method has to be called always, even when the task has already finished.
 * \warning This method must not be called directly inside the ScanFinished and
 * ReadFinished events. The intended use of the method is to schedule it for later
 * execution (e.g. PostMessage) or call it from another thread. Calling it from
 * a Qt slot is also an appropriate solution.
 */
void TaskControl::Wait(void) {
	if(!pid) return;
	PrIns::cmProcess op = { pid, 0 };
	apiHandle.Call(PrIns::ApiCallCodes::WaitProc, &op);
	pid = 0;
}

/// Returns the state of the previously started background task in percentage.
int TaskControl::GetState(void) const {
	try {
		PrIns::cmProcess op = { pid, -1 };
		apiHandle.Call(PrIns::ApiCallCodes::GetProcStatus, &op);
		return op.Status;
	}
	catch(const Exceptions::General &) {
		return 0;
	}
}

/** Checks if the task control is valid.
 *
 * @return			In case of running task the return value is true.
 */
bool TaskControl::IsValid() const {
	return pid!=0;
}

} // namespace Task
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_TASK_INCL
