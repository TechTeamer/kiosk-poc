#ifndef PR22_FIELD_INCL
#define PR22_FIELD_INCL
/******************************************************************************/
/**	\file
 *	Base element of document analyzing results.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Processing.hpp"
#include "Imaging/RawImage.hpp"
#include "Util/Util.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {

/** The @c Field is the base element of document analyzing results.
 *
 * The @c Field always contains an Id to give meaning for the data, the read
 * data that can be binary or textual (or numeric values can be handled as textual
 * data), and the status information of checking algorithms. If the field
 * is image-related then image data or image frame can be requested.
 */
class Field {
public:

	Field() { }													// Default constructor.

	/// @name Reference
	/// @{
	inline FieldReference GetFieldReference() const;			// Returns field identification data.
	/// @}

	/// @name Value
	/// @{
	inline PrOutputString GetRawStringValue(void) const;		// Returns the string data of the field as it is read.
	inline PrOutputString GetFormattedStringValue(void) const;	// Returns the formatted string data of the field.
	inline PrOutputString GetStandardizedStringValue(void) const;	// Returns the data of the field converted to a standard format.
	inline PrOutputString GetBasicStringValue(void) const;		// Returns the string data of the field in the least modified format.
	inline PrOutputString GetBestStringValue(void) const;		// Returns the string data of the field in the most convenient format.
	inline std::vector<gxu8> GetBinaryValue(void) const;		// Returns the binary data of the field.
	/// @}

	/// @name Status
	/// @{
	inline Status GetStatus(void) const;						// Returns the status information of checking algorithms for the field.
	inline std::vector<Checking> GetDetailedStatus(void) const;	// Returns a list of detailed status information of checking algorithms for the field.
	/// @}

	/// @name Other
	/// @{
	inline Imaging::RawImage GetImage(void) const;				// Returns the image of the field.
	inline Util::Frame GetFrame(void) const;					// Returns the geometric location of the field.
	inline Util::Variant ToVariant() const;						// Returns the internal variant data to query special field information.
	/// @}

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	Util::Variant Parent;				// Low level data of the document.
	Util::Variant Variant;				// Low level data of the field.
	FieldReference fieldRef;			// Field identification data.

	friend class Pr22::PrIns::Invoker;
	Field(const PrIns::gxModule & handle, const Util::Variant & variant, const FieldReference & id,
		const Util::Variant & parent): ApiHandle(handle), Parent(parent), Variant(variant),
		fieldRef(id) { }				// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Field interface. Now the implementation follows     *
 ******************************************************************************/

/// Returns field identification data.
FieldReference Field::GetFieldReference() const {
	return fieldRef;
}

/// Returns the string data of the field as it is read, with separator
/// characters, including checksum.
PrOutputString Field::GetRawStringValue(void) const {
	return Variant.GetChild(Util::VariantId::FieldValue, 0).ToString();
}

/// Returns the formatted string data of the field. The separator characters
/// are replaced with spaces and checksum values are not included.
PrOutputString Field::GetFormattedStringValue(void) const {
	return Variant.GetChild(Util::VariantId::FormattedValue, 0).ToString();
}

/// Returns the data of the field converted to a standard format. Even
/// the whole value can be replaced. E.g. converted from inches to centimeters.
PrOutputString Field::GetStandardizedStringValue(void) const {
	return Variant.GetChild(Util::VariantId::StandardizedValue, 0).ToString();
}

/// Returns the string data of the field in the least modified format.
PrOutputString Field::GetBasicStringValue(void) const {
	try {
		return Variant.GetChild(Util::VariantId::FieldValue, 0).ToString();
	} catch(const Exceptions::EntryNotFound &) {
	}
	try {
		return Variant.GetChild(Util::VariantId::FormattedValue, 0).ToString();
	} catch(const Exceptions::EntryNotFound &) {
	}
	return Variant.GetChild(Util::VariantId::StandardizedValue, 0).ToString();
}

/// Returns the string data of the field in the most convenient format.
PrOutputString Field::GetBestStringValue(void) const {
	try {
		return Variant.GetChild(Util::VariantId::StandardizedValue, 0).ToString();
	} catch(const Exceptions::EntryNotFound &) {
	}
	try {
		return Variant.GetChild(Util::VariantId::FormattedValue, 0).ToString();
	} catch(const Exceptions::EntryNotFound &) {
	}
	return Variant.GetChild(Util::VariantId::FieldValue, 0).ToString();
}

/// Returns the binary data of the field.
std::vector<gxu8> Field::GetBinaryValue(void) const {
	return Variant.GetChild(Util::VariantId::FieldValue, 0).ToByteArray();
}

/// Returns the status information of checking algorithms for the field.
Status Field::GetStatus(void) const {
	try { return (Status)Variant.GetChild(Util::VariantId::Checksum, 0).ToInt(); }
	catch(const Exceptions::General &) { return Status::NoChecksum; }
}

/// Returns a list of detailed status information of checking algorithms for the field.
std::vector<Checking> Field::GetDetailedStatus(void) const {
	std::vector<Checking> list;
	try {
		std::vector<int> arr = Variant.GetChild(Util::VariantId::CheckDetails, 0).ToIntArray();
		for(size_t i=0; i<arr.size(); i++) list.push_back((Checking)arr[i]);
	}
	catch(const Exceptions::General &) {
	}
	return list;
}

/// Returns the image of the field.
Imaging::RawImage Field::GetImage(void) const {
	struct prGetFieldImageV {
		PrIns::gxVARIANT Doc;
		int				FieldCode;
		int				Index;
		PrIns::gxVARIANT Img;
	} param = { Parent._variant, fieldRef.code, fieldRef.index, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::GetFieldImageV, &param);
	return Imaging::RawImage().FromVariant(Util::Variant(param.Img));
}

/// Returns the geometric location of the field.
Util::Frame Field::GetFrame(void) const {
	return Variant.GetChild(Util::VariantId::WindowFrame, 0).ToFrame();
}

/// Returns the internal variant data to query special field information.
Util::Variant Field::ToVariant() const {
	return Variant;
}

} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_FIELD_INCL
