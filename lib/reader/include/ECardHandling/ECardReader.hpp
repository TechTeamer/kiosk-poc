#ifndef PR22_ECARDREADER_INCL
#define PR22_ECARDREADER_INCL
/******************************************************************************/
/**	\file
 *	Electronic card reader (Smartcard, RFID card or Magnetic stripe card) module.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "ECardHandling/ECard.hpp"
#include "Task/ECardTask.hpp"
#include "Task/Task.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/// Electronic card reader (Smartcard, RFID card or Magnetic stripe card) module.
class ECardReader {
public:

	/// Class for storing information on an ECardReader.
	class Information {
	public:
		inline Util::HWType GetHwType() const;				// Type of the reader.
		inline PrOutputString GetVersion(char component) const;	// Returns version information of the reader.

	private:

		Util::Variant Info;									// Reader information.

		friend class Pr22::PrIns::Invoker;
		Information(const Util::Variant & info) : Info(info) { }	// Constructor for internal usage only.
	};

	inline Information GetInfo() const;						// Returns the card reader information.
	inline const std::vector<PrOutputString> & GetCards(void);	// Returns a list of the available ECard%s.
	inline ECard ConnectCard(int cardNo);					// Starts using an ECard.
	inline void Open(void);									// Starts using the current ECardReader.
	inline void Close(void);								// Ends the usage of the current ECardReader.
	inline ECardHandling::Certificates GetCertificateManager();	// Returns the certificate manager of the current ECardReader device.
	inline Task::TaskControl StartRead(const ECard & card, const Task::ECardTask & task);
															// Begins the authentication and reading process in a background thread.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	Util::Variant Info;					// Reader information.
	std::vector<PrOutputString> cards;	// List of ecards.
	std::wstring ReaderName;			// Name of the reader.
	bool opened;						// Reader is opened.

	friend class Pr22::PrIns::Invoker;
	ECardReader(const PrIns::gxModule & handle, const std::wstring & devName, const Util::Variant & info) :
		ApiHandle(handle), Info(info), cards(), ReaderName(devName), opened(true) { }	// Constructor for internal usage only.

};

/******************************************************************************
 * This is the end of the ECardReader interface. Now the implementation follows
 ******************************************************************************/

/// Type of the reader.
Util::HWType ECardReader::Information::GetHwType() const {
	return (Util::HWType)Info.ToInt();
}

/** Returns version information of the reader.
 *
 * @param component	\li Character @c 'H' for version of the hardware, \li character @c 'F' for version of
 *						the firmware, \li or character @c 'S' for version of the device handling software library.
 * @return				Version information of the reader.
 */
PrOutputString ECardReader::Information::GetVersion(char component) const {
	try {
		return Info.GetChild(Util::VariantId::Version, 0).GetListItem(Util::VariantPos::ById(component)).ToString();
	}
	catch(const Exceptions::General &) {
		return ToPrOutputString(L"-");
	}
}

//------------------------------------------------------------------------------

/// Returns the card reader information.
ECardReader::Information ECardReader::GetInfo() const {
	return PrIns::Invoker().Construct(Info, (Information*)0);
}

/** Returns a list of the available ECard%s.
 *
 * @return	List of ECard%s.
 */
const std::vector<PrOutputString> & ECardReader::GetCards(void) {
	cards.clear();
	struct prGetReaderCardList {
		const wchar_t* Reader;
		wchar_t** Cards;
		int NCards;
	} gcl = { ReaderName.c_str(), 0, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetReaderCardList, &gcl);
	for(int i=0; i<gcl.NCards; ++i) cards.push_back(ToPrOutputString(gcl.Cards[i]));
	PrIns::GlobalData(gcl.Cards);
	return cards;
}

/** Starts using an ECard.
 *
 * @param cardNo	The ordinal number of the card in the last requested cardlist.
 * @return			The ECard handling class.
 */
ECard ECardReader::ConnectCard(int cardNo) {
	if(cardNo<0 || cardNo>=(int)cards.size()) {
		throw Exceptions::DataOutOfRange(L"[pr22] (cardNo)");
	}
	ECard tmp = PrIns::Invoker().Construct(ApiHandle, cards[(size_t)cardNo], (ECard*)0);
	struct prRfidCard {
		const wchar_t* Card;
	} op = { tmp.GetSerial().c_str() };
	ApiHandle.Call(PrIns::ApiCallCodes::ConnectRfidCard, &op);
	return tmp;
}

/// Starts using the current ECardReader.
void ECardReader::Open(void) {
	if(opened == true) {
		return;
	}
	PrIns::prRfidOpenDevice op = { ReaderName.c_str(), 0 };
	if(ApiHandle.Call(PrIns::ApiCallCodes::OpenCardReader, &op))
		return;
	opened = true;
}

/** Ends the usage of the current ECardReader.
 *
 * This method is usable to communicate with an ECardReader outside our SDK.
 */
void ECardReader::Close(void) {
	if(opened == false || !ApiHandle.IsValid()) {
		return;
	}
	PrIns::prRfidOpenDevice op = { ReaderName.c_str(), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::CloseCardReader, &op);
	opened = false;
}

/// Returns the certificate manager that stores certificates for the current ECardReader device only.
ECardHandling::Certificates ECardReader::GetCertificateManager() {
	return PrIns::Invoker().Construct(ApiHandle, PrIns::CertScope::CardReader, ReaderName, (ECardHandling::Certificates*)0);
}

/** Begins the authentication and reading process in a background thread.
 *
 * \note After the process is finished the Task::TaskControl.Wait method must be called.
 *
 * @param card		The ECard to process.
 * @param task		List of files and authentication level.
 * @return			A Task::TaskControl object.
 */
Task::TaskControl ECardReader::StartRead(const ECard & card, const Task::ECardTask & task) {
	struct prStartEcardReading {
		const wchar_t *card;
		const int *files;
		int nfile;
		int authlevel;
		int reqid;
	} op = { card.GetSerial().c_str(), (int*)PrIns::vector_data(task.files), (int)task.files.size(),
		task.GetAuthLevel(), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::StartEcardReading, &op);
	return PrIns::Invoker().Construct(ApiHandle, op.reqid, (Task::TaskControl*)0);
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_ECARDREADER_INCL
