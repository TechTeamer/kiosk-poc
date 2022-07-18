#ifndef PR22_DOCUMENTTYPE_INCL
#define PR22_DOCUMENTTYPE_INCL
/******************************************************************************/
/**	\file
 *	Document identifier name converter class.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Util/Types.hpp"
#include <string>
//------------------------------------------------------------------------------
namespace Pr22 { namespace Extension {

/// Document identifier name converter.
class DocumentType {
public:

	/** Returns the name of a general document.
	 *
	 * @param id		The document code.
	 * @return			The name of a general document.
	 */
	static PrOutputString GetDocumentName(int id) {
		const wchar_t *values[] = {
			L"Unknown document",
			L"ICAO standard Passport (MRP)",
			L"ICAO standard 2 row Travel Document (TD-2)",
			L"ICAO standard 3 row Travel Document (TD-1)",
			L"ICAO standard visa (MRV-A) (MRV-B)",
			L"French ID card",
			L"Pre ICAO standard 3 row Travel Document",
			L"Slovak ID card",
			L"AAMVA standard driving license",
			L"Belgian ID",
			L"Swiss driving license",
			L"ID of Cote d'Ivoire",
			L"Financial Transaction Card",
			L"IATA boarding pass",
			L"ICAO Travel Document (TD-1, front page, named)",
			L"ICAO Travel Document (TD-1, front page, typed)",
			L"ISO standard driving license",
			L"Mail item",
			L"ICAO standard electronic document (Passport/ID)",
			L"EID",
			L"ESign",
			L"NFC",
			L"European standard driving license",
			L"Portuguese ID",
			L"Ecuadorian ID",
			L"ID card with MRZ",
			L"USA military ID"
		};
		return id<0 || id>=(int)gx_arraycnt(values) ? PrOutputString() : ToPrOutputString(values[id]);
	}

	/** Returns the document type name.
	 *
	 * @param doc_type	Document type identifier string.
	 * @return			The name of the document type.
	 */
	static PrOutputString GetDocTypeName(const std::wstring & doc_type) {
		const wchar_t *ret = L"";
		std::wstring dt2 = doc_type.substr(0, 2);
		if(dt2 == L"DL") {
			if(doc_type == L"DLL") ret = L"driving license for learner";
			else ret = L"driving license";
		}
		else if(dt2 == L"ID") {
			if(doc_type == L"IDF") ret = L"ID card for foreigner";
			else if(doc_type == L"IDC") ret = L"ID card for children";
			else ret = L"ID card";
		}
		else if(dt2 == L"PP") {
			if(doc_type == L"PPD") ret = L"diplomatic passport";
			else if(doc_type == L"PPS") ret = L"service passport";
			else if(doc_type == L"PPE") ret = L"emergency passport";
			else if(doc_type == L"PPC") ret = L"passport for children";
			else ret = L"passport";
		}
		else if(dt2 == L"TD") ret = L"travel document";
		else if(dt2 == L"RP") ret = L"residence permit";
		else if(dt2 == L"VS") ret = L"visa";
		else if(dt2 == L"WP") ret = L"work permit";
		else if(dt2 == L"SI") ret = L"social insurance document";
		else ret = L"document";
		return ToPrOutputString(ret);
	}

	/** Returns the page name.
	 *
	 * @param doc_page	Document page identifier.
	 * @return			Document page name.
	 */
	static PrOutputString GetPageName(const std::wstring & doc_page)
	{
		const wchar_t *ret = L"";
		if(doc_page == L"F") ret = L"front";
		else if(doc_page == L"B") ret = L"back";
		else ret = L"";
		return ToPrOutputString(ret);
	}
};

} // namespace Extension
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_DOCUMENTTYPE_INCL
