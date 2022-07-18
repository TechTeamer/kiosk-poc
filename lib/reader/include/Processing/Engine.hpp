#ifndef PR22_ENGINE_INCL
#define PR22_ENGINE_INCL
/******************************************************************************/
/**	\file
 *	Data analyzer module.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Imaging/DocImage.hpp"
//------------------------------------------------------------------------------
namespace Pr22 {

/// Data analyzer module.
class Engine {
public:
	/// Class for storing information on an Engine.
	class Information {
	public:
		inline PrOutputString GetVersion(char component) const;		// Returns version information of the engine.
		inline Processing::EngineLicense GetRequiredLicense(void) const;	// Returns the required EngineLicense for the used engine.
		inline PrOutputString GetRequiredLicenseDate(void) const;	// Returns the date of the used engine.
		inline PrOutputString GetLicenseDate(const Processing::EngineLicense & license) const;	// Returns the engine acceptance date of a license.
		inline std::vector<Processing::EngineLicense> GetAvailableLicenses(void) const;	// Returns a list of the available EngineLicense%s.
		inline PrOutputString GetDirectory(void) const;				// Returns the location of the OCR engines.

	private:

		PrIns::gxModule ApiHandle;								// Api handler.
		Util::Variant Info;										// Engine information.

		friend class Pr22::PrIns::Invoker;
		inline Information(const PrIns::gxModule & handle);		// Constructor for internal usage only.
	};

public:

	inline Engine::Information GetInfo() const;					// Returns the engine information.
	inline Processing::Document Analyze(Processing::Page & page,
		const Task::EngineTask & task);							// Analyzes the image data of an already scanned Page.
	inline Processing::Document Analyze(const Processing::BinData & data);	// Analyzes binary data.
	inline Imaging::RawImage GenerateLatentImage(Imaging::DocImage & image,
		const Util::InputString & decodingParams);				// Generates a decoded IPI image for visual inspection.
	inline Processing::Document ReadField(Imaging::DocImage & image,
		const Util::Variant & readingParams);					// Reads character or barcode data from an image.
	inline Processing::Document GetRootDocument(void) const;	// Returns the root Document containing all data to save.

private:

	PrIns::gxModule ApiHandle;								// Api handler.

	friend class Pr22::PrIns::Invoker;
	Engine(const PrIns::gxModule & handle) : ApiHandle(handle) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Engine interface. Now the implementation follows    *
 ******************************************************************************/

// Constructor for internal usage only.
Engine::Information::Information(const PrIns::gxModule & handle) : ApiHandle(handle) {
	struct prGetEngineInfo {
		PrIns::gxVARIANT Info;
	} ge = { 0 };
	try {
		ApiHandle.Call(PrIns::ApiCallCodes::GetEngineInfo, &ge);
		Info = Util::Variant(ge.Info);
	} catch(const Exceptions::FileOpen &) { }
}

/** Returns version information of the engine.
 *
 * @param component	Character @c 'E' for version of the engine.
 * @return			Version information of the engine.
 */
PrOutputString Engine::Information::GetVersion(char component) const {
	try {
		if(component == 'E') return Info.ToString();
	} catch(const Exceptions::General &) { }
	return ToPrOutputString(L"-");
}

/// Returns the required EngineLicense for the used engine.
Processing::EngineLicense Engine::Information::GetRequiredLicense(void) const {
	try {
		if(Info.Type()!=Util::Variant::Types::Null)
			return (Processing::EngineLicense)Info.GetChild(Util::VariantId::EngineLicense, 0).ToInt();
	}
	catch(const Exceptions::EntryNotFound &) { }
	return Processing::EngineLicense::Unknown;
}

/// Returns the date of the used engine.
PrOutputString Engine::Information::GetRequiredLicenseDate(void) const {
	try {
		if(Info.Type()!=Util::Variant::Types::Null)
			return Info.GetChild(Util::VariantId::EngineLicDate, 0).ToString();
	}
	catch(const Exceptions::EntryNotFound &) { }
	return ToPrOutputString(L"-");
}

/** Returns the engine acceptance date of a license.
 *
 * @param license	The license to query for the date.
 * @return			The date of the license.
 */
PrOutputString Engine::Information::GetLicenseDate(const Processing::EngineLicense & license) const {
	struct prGetLicenseDate {
		int License;
		wchar_t* Date;
	} gld = { license, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetLicenseDate, &gld);
	PrIns::GlobalData tmp(gld.Date);
	return ToPrOutputString(gld.Date ? gld.Date : L"");
}

/// Returns a list of the available EngineLicense%s.
std::vector<Processing::EngineLicense> Engine::Information::GetAvailableLicenses(void) const {
	std::vector<Processing::EngineLicense> licenses;
	struct prGetLicenses {
		int *Licenses;
		int NLicenses;
	} gl;
	ApiHandle.Call(PrIns::ApiCallCodes::GetLicenses, &gl);
	for(int i=0; i<gl.NLicenses; i++) licenses.push_back((Processing::EngineLicense)gl.Licenses[i]);
	PrIns::GlobalData(gl.Licenses);
	return licenses;
}

/// Returns the location of the OCR engines.
PrOutputString Engine::Information::GetDirectory(void) const {
	return ApiHandle.GetProperty(L"module_dir");
}

//------------------------------------------------------------------------------

/// Returns the engine information.
Engine::Information Engine::GetInfo() const {
	return PrIns::Invoker().Construct(ApiHandle, ( Engine::Information*)0);
}

/** Analyzes the image data of an already scanned Page and returns the textual
 * and logical data according to task.
 *
 * This method has the same role as Processing::Page.Analyze.
 *
 * @param page		A set of images to analyze.
 * @param task		Specifies data field references to analyze.
 * @return			The result data structure.
 */
Processing::Document Engine::Analyze(Processing::Page & page, const Task::EngineTask & task) {
	return page.Analyze(task);
}

/** Analyzes binary data.
 *
 * @param data		Data to analyze.
 * @return			The result data structure.
 */
Processing::Document Engine::Analyze(const Processing::BinData & data) {
	struct prAnalyzeC {
		const void		*Buffer;
		int				Bufflen;
		PrIns::gxVARIANT Doc;
		const wchar_t	*comment;
	} an = { data.RawData(), data.RawSize(), 0, data.GetComment().c_str() };
	ApiHandle.Call(PrIns::ApiCallCodes::AnalyzeC, &an);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(an.Doc), (Processing::Document*)0);
}

/** Generates a decoded IPI image for visual inspection.
 *
 * This method has the same role as Imaging::DocImage.GenerateLatentImage.
 *
 * @param image				Source image.
 * @param decodingParams	IPI image decoding parameters.
 * @return					Result image.
 */
Imaging::RawImage Engine::GenerateLatentImage(Imaging::DocImage & image, const Util::InputString & decodingParams) {
	return image.GenerateLatentImage(decodingParams);
}

/** Reads character or barcode data from an image.
 *
 * This method has the same role as Imaging::DocImage.ReadField.
 *
 * @param image			The input image to read from.
 * @param readingParams	Reading parameters that are described in the Passport Reader
 *						reference manual.
 * @return				The result data structure.
 */
Processing::Document Engine::ReadField(Imaging::DocImage & image, const Util::Variant & readingParams) {
	return image.ReadField(readingParams);
}

/// Returns the root Document containing all data to save.
Processing::Document Engine::GetRootDocument(void) const {
	PrIns::prGetDocumentRootV gdr = { 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetDocRootV, &gdr);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(gdr.Document), (Processing::Document*)0);
}

} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_ENGINE_INCL
