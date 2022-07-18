#ifndef PR22_PROCESSING_INCL
#define PR22_PROCESSING_INCL 1
#endif//PR22_PROCESSING_INCL
/******************************************************************************/
/**	\file
 *	Enumerations and classes for data processing.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "ECardHandling/ECardHandling.hpp"
#include "Processing/FieldId.hpp"
#include "Exceptions/Exceptions.hpp"
namespace Pr22 { namespace PrIns { class Invoker; } }
#include "Util/Types.hpp"

#define ENUMSTEP PR22_PROCESSING_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {


/// Field source IDs.
GX_ENUM_BEGIN(FieldSource)
GXEV( All, 0 )								///< Any source.
GXEV( Mrz, 1000 )							///< Machine Readable Zone.
GXEV( Viz, 2000 )							///< Visual Inspection Zone.
GXEV( Barcode, 3000 )						///< Barcode.
GXEV( MagStripe, 4000 )						///< Magnetic stripe.
GXEV( ECard, 0x400000 )						///< SmartCard (contact or contactless).
GX_ENUM_END(FieldSource)

/// Field checking details.
GX_ENUM_BEGIN(Checking)
GXEV( Missing, (int)0xD0080003 )			///< Mandatory entity is missing.
GXEV( SelfCheckOk, (int)0xD0080010 )		///< Self checking is succeeded.
GXEV( SelfCheckInvalid, SelfCheckOk + 2 )	///< Self checking cannot be performed due to invalid data format.
GXEV( SelfCheckFailed, SelfCheckOk + 3 )	///< Self checking is failed.
GXEV( FormatOk, (int)0xD0080020 )			///< Format test is succeeded.
GXEV( FormatInvalid, FormatOk + 3 )			///< Format test is failed.
GXEV( DateOk, (int)0xD0080030 )				///< Datetime value is not expired.
GXEV( DateInvalid, DateOk + 2 )				///< Past datetime value is greater than current date.
GXEV( DateExpired, DateOk + 3 )				///< Datetime value is expired.
GXEV( Revoked, (int)0xD0080043 )			///< Certificate has been revoked.
GX_ENUM_END(Checking)

/** Field status values. Result of checksum calculations, data integrity and environmental specific
* (e.g. expiry) tests.
*/
GX_ENUM_BEGIN(Status)
GXEV( Ok, 0 )								///< All checks are passed successfully.
GXEV( NoChecksum, 1 )						///< There is no checksum for the field.
GXEV( Warning, 100 )						///< Warning occurred.
GXEV( DataIntegrity, 101 )					///< Data integrity failed.
GXEV( OCRWarning, 102 )						///< OCR warning occurred.
GXEV( Error, 200 )							///< Error occurred.
GXEV( InvalidChar, 201 )					///< Invalid character found.
GXEV( InvalidChecksum, 202 )				///< Invalid checksum value found.
GXEV( ChecksumError, 203 )					///< Checksum failed.
GX_ENUM_END(Status)

/// Engine license IDs.
GX_ENUM_BEGIN(EngineLicense)
GXEV( MrzOcrBarcodeReading, 0xF4 )			///< MRZ OCR+Barcode Reading license.
GXEV( MrzOcrBarcodeReadingL, 0x74 )			///< MRZ OCR+Barcode Reading license (variable).
GXEV( MrzOcrBarcodeReadingF, 0x84 )			///< MRZ OCR+Barcode Reading license (fixed).
GXEV( VizOcrCountry, 0x10 )					///< VIZ OCR Level1-Country license.
GXEV( VizOcrRegion, 0x11 )					///< VIZ OCR Level2-Region license.
GXEV( VizOcrWorld, 0x12 )					///< VIZ OCR Level3-World license.
GXEV( VizOcrAuthCountry, 0x13 )				///< VIZ OCR+AUTH Level1-Country license.
GXEV( VizOcrAuthRegion, 0x14 )				///< VIZ OCR+AUTH Level2-Region license.
GXEV( VizOcrAuthWorld, 0x15 )				///< VIZ OCR+AUTH Level3-World license.
GXEV( PRSoftware, 0xA1 )					///< PR Software license.
GXEV( PRAutoFill, 0xA2 )					///< PR auto fill license.
GXEV( ECard, 0xEC )							///< ECard Features license.
GXEV( HotelInterface, 0xFA )				///< Hotel Reader Interface license.
GXEV( Unknown, 0 )							///< License information is not available.
GX_ENUM_END(EngineLicense)


} // namespace Processing
} // namespace Pr22
#undef ENUMSTEP
#if PR22_PROCESSING_INCL == 1
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {


/// Class for storing Field identification data.
class FieldReference {
public:

	int code;		///< Internally used value of Field identification.
	int index;		///< Ordinal number of the Field with the same code.

	/// A new Field identification data.
	FieldReference() : code(0), index(0) { }
	inline FieldReference(const FieldSource & source, const FieldId & id);	// A new Field identification data.
	inline FieldSource GetSource() const;							// Returns the Id of the FieldSource.
	inline FieldId GetId() const;									// Returns the FieldId.
	inline PrOutputString ToString() const;							// Returns string representation of the field reference.
	inline PrOutputString ToString(const Util::InputString & insert) const;	// Returns string representation of the field reference.
	inline FieldReference(const ECardHandling::File & f);			// Converts from ECard file identifier.
	inline operator ECardHandling::File() const;					// Converts to ECard file identifier.
	inline FieldReference(const ECardHandling::AuthProcess & p);	// Converts from ECard authentication identifier.
	inline operator ECardHandling::AuthProcess() const;				// Converts to ECard authentication identifier.
};

/// Result data of Field comparison.
class FieldCompare {
public:
	/// Reference of the compared field #1.
	FieldReference Field1;
	/// Reference of the compared field #2.
	FieldReference Field2;
	/// Result of comparison in 0-1000 range.
	int Confidence;
};


/******************************************************************************
 * This is the end of the Processing interface. Now the implementation follows
 ******************************************************************************/


/** A new Field identification data.
 *
 * @param source	Id of the FieldSource.
 * @param id		Field Id.
 */
FieldReference::FieldReference(const FieldSource & source, const FieldId & id) {
	index = 0;
	if(source == FieldSource::ECard) {
		switch(id) {
		case FieldId::CompositeMrz:	code = 0x410000; return;
		case FieldId::Face:			code = 0x420000; return;
		case FieldId::Fingerprint:	code = 0x430000; return;
		case FieldId::Iris:			code = 0x440000; return;
		case FieldId::Signature:	code = 0x450000; return;
		default: ;
		}
		if(id>=FieldId::COM && id<FieldId::COM+100) { code = id; return; }
	}
	if(id==FieldId::B900 || id==FieldId::CompositeVizAuth || id==FieldId::DullCheck
		|| id==FieldId::SecurityFibres) {
			code = id;
			return;
	}
	code = source + id;
}

/// Returns the Id of the FieldSource.
FieldSource FieldReference::GetSource() const {
	if(code<FieldSource::ECard) {
		return FieldSource((code / 1000) * 1000);
	}
	else {
		return FieldSource::ECard;
	}
}

/// Returns the FieldId.
FieldId FieldReference::GetId() const {
	if(code>FieldSource::ECard) {
		switch(code) {
		case 0x410000: return FieldId::CompositeMrz;
		case 0x420000: return FieldId::Face;
		case 0x430000: return FieldId::Fingerprint;
		case 0x440000: return FieldId::Iris;
		case 0x450000: return FieldId::Signature;
		}
		if(code>=FieldId::COM && code<FieldId::COM+100) return (FieldId)code;
	}
	switch(code) {
	case 1551: return FieldId::B900;
	case 2500: return FieldId::CompositeVizAuth;
	case 2551: return FieldId::DullCheck;
	case 2553: return FieldId::SecurityFibres;
	}
	return FieldId((code - GetSource()) % 1000);
}

/// Returns string representation of the field reference.
PrOutputString FieldReference::ToString() const {
	return ToString(L"");
}

/** Returns string representation of the field reference.
 *
 * @param insert	String to insert between the source and the id strings.
 * @return			String representation of the FieldReference.
 */
PrOutputString FieldReference::ToString(const Util::InputString & insert) const
{
	std::wostringstream tmp;
	const wchar_t *src = GetSource().ToString();
	src ? tmp << src : tmp << GetSource() / 1000;
	if(insert.GetString()) tmp << insert.GetString();

	FieldId fid = GetId();
	const wchar_t * sid = fid.ToString();
	if(sid) tmp << sid;
	else if(fid/100==5) tmp << L"Authenticity" << (fid-500);
	else if(fid/100==0) tmp << L"Composite" << fid;
	else tmp << fid;

	if(index>0) tmp << L"(" << index << L")";
	return ToPrOutputString(tmp.str());
}

/** Converts ECard file identifier to ECard data group field reference.
 *
 * @param f			ECard file identifier.
 */
FieldReference::FieldReference(const ECardHandling::File & f) {
	if(f.Id < 0 || f.Id >= 50) throw Exceptions::DataOutOfRange(L"[pr22] No data group!");
	*this = FieldReference(FieldSource::ECard, (FieldId)(FieldId::COM + f.Id));
}

/// Converts ECard data group field reference to ECard file identifier.
FieldReference::operator ECardHandling::File() const {
	int v = code - FieldId::COM;
	if(v < 0 || v >= 50) throw Exceptions::DataOutOfRange(L"[pr22] No file!");
	return ECardHandling::File(v);
}

/** Converts ECard authentication identifier to ECard digital authentication field
 * reference.
 *
 * @param p			ECard authentication identifier.
 */
FieldReference::FieldReference(const ECardHandling::AuthProcess & p) {
	*this = FieldReference(FieldSource::ECard, (FieldId)(FieldId::COM + 50 + p));
}

/** Converts ECard digital authentication field reference to
 * ECard authentication identifier.
 *
 * @return	ECard authentication identifier.
 */
FieldReference::operator ECardHandling::AuthProcess() const {
	int v = code - FieldId::COM - 50;
	if(v < 0 || v >= 50) throw Exceptions::DataOutOfRange(L"[pr22] No auth!");
	return (ECardHandling::AuthProcess)v;
}


} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#undef PR22_PROCESSING_INCL
#define PR22_PROCESSING_INCL 2
#include "Processing/Processing.hpp"
#undef PR22_PROCESSING_INCL
#define PR22_PROCESSING_INCL 3
#endif
