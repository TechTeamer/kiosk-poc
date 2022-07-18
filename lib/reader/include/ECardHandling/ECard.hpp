#ifndef PR22_ECARD_INCL
#define PR22_ECARD_INCL
/******************************************************************************/
/**	\file
 *	Electronically readable card (Smartcard, RFID card or Magnetic stripe card).
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Document.hpp"
#include "ECardHandling/Certificates.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/// Electronically readable card (Smartcard, RFID card or Magnetic stripe card).
class ECard {
public:

	/// Class for storing information on an ECard.
	class Information {
	public:
		inline Util::Variant ToVariant() const;				// Returns the internal variant data to query special ECard information.
		inline PrOutputString GetCardType() const;			// Returns the ECard type identifier string.

	private:

		Util::Variant Info;				// Card information.

		friend class Pr22::PrIns::Invoker;
		inline Information(const PrIns::gxModule & handle, const std::wstring & card);	// Constructor for internal usage only.
	};

	ECard() { }												// Default constructor.
	inline Information GetInfo() const;						// Returns the card information.
	inline const std::wstring & GetSerial() const;			// Returns the serial number of the card.
	inline Processing::BinData GetAuthReferenceData() const;	// Returns data necessary for the next authentication process.
	inline void Disconnect(void);							// Finishes usage of the card.
	inline std::vector<ECardHandling::Application> GetApplications(void);	// Returns a list of available ecard application.
	inline std::vector<ECardHandling::File> GetFiles(void);					// Returns a list of identifiers of the available Files.
	inline Processing::BinData GetFile(const ECardHandling::File & fileId);	// Returns the data of a file.
	inline bool CheckHash(const ECardHandling::File & fileId);				// Checks the integrity of a file.
	inline ECardHandling::AuthProcess GetNextAuthentication(bool forceNext);	// Returns the identifier of the next suggested authentication to proceed.
	inline void Authenticate(const ECardHandling::AuthProcess & authentication,
		const Processing::BinData & additionalData, int selector);			// Executes an authentication process.
	inline ECardHandling::Certificates GetCertificateManager();				// Returns the certificate manager that stores certificates for the current ECard only.
	inline ECardHandling::File ConvertFileId(const ECardHandling::File & file);	// Converts file identifier values.
	inline Processing::Document GetAuthResult(void);		//	Returns the ECard authentication results in document form.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	std::wstring Serial;				// Serial number of the card.
	Processing::BinData authData;		// Auth data for the next auth process.

	friend class Pr22::PrIns::Invoker;
	ECard(const PrIns::gxModule & handle, const Util::InputString & serialNo): ApiHandle(handle),
		Serial(serialNo.GetString()), authData() { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the ECard interface. Now the implementation follows     *
 ******************************************************************************/

// Constructor for internal usage only.
ECard::Information::Information(const PrIns::gxModule & handle, const std::wstring & card) {

	PrIns::gxModule ApiHandle = handle;
	struct prGetRfidCardInfo {
		const wchar_t *Card;
		PrIns::gxVARIANT CardInfo;
	} op = { card.c_str(), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetRfidCardInfo, &op);
	Info = Util::Variant(op.CardInfo);
}

/** Returns the internal variant data to query special ECard information.
 *
 * @return			The data.
 */
Util::Variant ECard::Information::ToVariant() const {
	return Info;
}

/// Returns the ECard type identifier string.
PrOutputString ECard::Information::GetCardType() const {
	return Info.GetChild(Util::VariantId::CardType, 0).ToString();
}

//------------------------------------------------------------------------------

/// Returns the card information.
ECard::Information ECard::GetInfo() const {
	return PrIns::Invoker().Construct(ApiHandle, Serial, (Information*)0);
}

/** Returns the serial number of the card.
 *
 * In numerous cases, the serial number is random. In some other cases, cards have no serial at all.
 * For such cards, the system generates a fictional number.
 */
const std::wstring & ECard::GetSerial() const {
	return Serial;
}

/** Returns data necessary for the next authentication process.
 *
 * @return			Authentication data.
 */
Processing::BinData ECard::GetAuthReferenceData() const {
	return authData;
}

/// Finishes usage of the card.
void ECard::Disconnect(void) {
	struct prRfidCard {
		const wchar_t* Card;
	} op = { Serial.c_str() };
	ApiHandle.Call(PrIns::ApiCallCodes::DisconnectRfidCard, &op);
}

/** Returns a list of available ecard application.
 *
 * When the list is empty or contains only ECardHandling::Application::None that
 * means that the list cannot be requested from the card, but the system tries to select
 * application on card.
 */
std::vector<ECardHandling::Application> ECard::GetApplications(void) {
	std::vector<ECardHandling::Application> apps;
	struct prRfidGetApps {
		const wchar_t* Card;
		int* Apps;
		int nApps;
	} op = { Serial.c_str(), 0, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetApplications, &op);
	for(int i=0; i<op.nApps; ++i) apps.push_back((ECardHandling::Application)op.Apps[i]);
	PrIns::GlobalData(op.Apps);
	return apps;
}

/// Returns a list of identifiers of the available ECardHandling::File%s.
std::vector<ECardHandling::File> ECard::GetFiles(void) {
	std::vector<ECardHandling::File> files;
	struct prRfidGetFiles {
		const wchar_t* Card;
		int* Files;
		int nFiles;
	} op = { Serial.c_str(), 0, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetFileList, &op);
	for(int i=0; i<op.nFiles; ++i) files.push_back((ECardHandling::File)op.Files[i]);
	PrIns::GlobalData(op.Files);
	return files;
}

/** Returns the data of a file.
 *
 * @param fileId		File identifier.
 * @return		Binary data.
 */
 Processing::BinData ECard::GetFile(const ECardHandling::File & fileId) {
	struct prRfidGetFile {
		const wchar_t* Card;
		int FileId;
		void* Data;
		int SData;
	} op = { Serial.c_str(), fileId.Id, 0, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetRfidFile, &op);
	std::wstring comment = Serial + Util::ToString(L":%i", fileId.Id).operator const Util::InputString().GetString();
	return PrIns::GlobalData(op.Data, op.SData).BinData(comment);
}

/** Checks the integrity of a file.
 *
 * @param fileId		File identifier.
 * @return		Ok or failed.
 */
bool ECard::CheckHash(const ECardHandling::File & fileId) {
	struct prCheckRfidFileHash {
		const wchar_t* Card;
		int FileId;
	} op = { Serial.c_str(), fileId.Id };
	try {
		ApiHandle.Call(PrIns::ApiCallCodes::CheckFileHash, &op);
	} catch(const Exceptions::General &) {
		return false;
	}
	return true;
}

/** Returns the identifier of the next suggested authentication to proceed.
 *
 * @param forceNext		Skip the next authentication and return the one suggested after that.
 * @return		Authentication identifier.
 */
ECardHandling::AuthProcess ECard::GetNextAuthentication(bool forceNext) {
	struct prGetNextAuthentication {
		const wchar_t* Card;
		int ProcessName;
		int ForceNext;
	} op = { Serial.c_str(), 0, forceNext ? 1 : 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetNextAuth, &op);
	return (ECardHandling::AuthProcess)op.ProcessName;
}

/** Executes an authentication process.
 *
 * In some special cases, additional system information is necessary for computing
 * @c additionalData.
 * These pieces of information can be requested by the GetAuthReferenceData() method.
 *
 * @param authentication	Authentication identifier.
 * @param additionalData	Data needed for authentication.
 * @param selector			Selector parameter for certain authentications.
 */
void ECard::Authenticate(const ECardHandling::AuthProcess & authentication,
						 const Processing::BinData & additionalData, int selector) {
	if(authentication == ECardHandling::AuthProcess::InitTerminal) {
		struct prTerminalAuth {
			const wchar_t* Card;
			void* Data;
			int Sdata;
		} op = { Serial.c_str(), 0, 0 };
		ApiHandle.Call(PrIns::ApiCallCodes::InitTerminalAuth, &op);
		authData = PrIns::GlobalData(op.Data, op.Sdata).BinData();

	} else {
		struct prDoAuthentication {
			const wchar_t* Card;
			int ProcessName;
			const void* AuthData;
			int Lines;
			int PasswordType;
		} op = { Serial.c_str(), authentication, 0, 0, selector };
		const void *tmpptr = 0;

		if(authentication == ECardHandling::AuthProcess::CompleteTerminal) {
			op.Lines = additionalData.RawSize();
			op.AuthData = additionalData.RawData();
		}
		else {
			op.Lines = 1;
			tmpptr = additionalData.RawData();
			op.AuthData = &tmpptr;
		}
		ApiHandle.Call(PrIns::ApiCallCodes::DoAuth, &op);
	}
}

/// Returns the certificate manager that stores certificates for the current ECard only.
ECardHandling::Certificates ECard::GetCertificateManager() {
	return PrIns::Invoker().Construct(ApiHandle, PrIns::CertScope::Card, Serial, (ECardHandling::Certificates*)0);
}

/** Converts file identifier values: general IDs to data group number and vice versa.
 *
 * @param file		File Id to convert.
 * @return			Converted file Id.
 */
ECardHandling::File ECard::ConvertFileId(const ECardHandling::File & file) {
	struct prConvertFileNames {
		const wchar_t* Card;
		int Fid;
	} op = { Serial.c_str(), file.Id };
	if(ApiHandle.Call(PrIns::ApiCallCodes::ConvertFileNames, &op))
		return ECardHandling::File(0);
	return ECardHandling::File(op.Fid);
}

///	Returns the ECard authentication results in document form.
Processing::Document ECard::GetAuthResult(void) {
	struct prRfidGetAuthResult {
		const wchar_t* Card;
		PrIns::gxVARIANT Doc;
	} gar = { Serial.c_str(), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetAuthResult, &gar);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(gar.Doc), (Processing::Document*)0);
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_ECARD_INCL
