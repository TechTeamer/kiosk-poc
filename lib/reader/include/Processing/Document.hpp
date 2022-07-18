#ifndef PR22_DOCUMENT_INCL
#define PR22_DOCUMENT_INCL
/******************************************************************************/
/**	\file
 *	Stores the results of analyzing tasks.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Field.hpp"
#include "Task/PrTask.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {

/// Stores the results of analyzing tasks.
class Document {
public:
	#define ENUMSTEP 1
	/// Document data file formats.
	GX_ENUM_BEGIN(FileFormat)
	#include "Processing/docfiles.inl"
	GX_ENUM_END(FileFormat)
	#undef ENUMSTEP

	Document() { }													// Default constructor.
	inline std::vector<FieldReference> GetFields(void) const;		// Returns a list of field reference%s from the document.
	inline std::vector<FieldReference> GetFields(const FieldReference & filter) const;	// Returns a list of field reference%s from the document.
	inline std::vector<FieldReference> GetFields(const Task::EngineTask & filter) const;	// Returns a list of field reference%s from the document.
	inline Document GetPart(int index) const;						// Returns a simple document data of a "Root Document".
	inline Field GetField(const FieldSource & group, const FieldId & id, int index = 0) const;	// Returns a field of the document.
	inline Field GetField(const FieldReference & fieldCode) const;	// Returns a field of the document.
	inline Status GetStatus(void) const;							// Returns the overall status of the document.
	inline BinData Save(const FileFormat & fileFormat) const;		// Saves the document to the memory.
	inline Document operator +(const Document & other);				// Integrates documents.
	inline std::vector<FieldCompare> GetFieldCompareList(void) const;	// Returns the results of field value comparisons.
	inline Util::Variant ToVariant() const;							// Returns low level document data.

private:

	PrIns::gxModule ApiHandle;								// Api handler.
	Util::Variant Variant;									// Low level document data.

	friend class Pr22::PrIns::Invoker;
	Document(const PrIns::gxModule & handle, const Util::Variant & variant) :
		ApiHandle(handle), Variant(variant) { }				// Constructor for internal usage only.
	inline bool EnabledField(const FieldReference & field, const Task::EngineTask & filter) const;
};

/******************************************************************************
 * This is the end of the Document interface. Now the implementation follows  *
 ******************************************************************************/

/// Returns the internal variant data to access rarely requested information.
Util::Variant Document::ToVariant() const {
	return Variant;
}

bool Document::EnabledField(const FieldReference & field, const Task::EngineTask & filter) const {
	bool ret = false;
	FieldReference filti;
	if(filter.fields.empty()) return true;

	for(size_t ix=0; ix<filter.fields.size(); ++ix) {
		filti.code = filter.fields[ix];
		bool add = filti.code >= 0;
		if(!add) filti.code = -filti.code;
		int fid = field.GetId() % 0x400000 % 1000;
		if((filti.GetSource() == field.GetSource() || filti.GetSource() == FieldSource::All)
			&& (filti.GetId() == field.GetId() || filti.GetId() == FieldId::All
			|| (filti.GetId() == FieldId::Authenticity && fid >= 500 && fid < 600))) {
				ret = add;
		}
	}
	return ret;
}

/// Returns a list of field reference%s from the document.
std::vector<FieldReference> Document::GetFields(void) const {
	return GetFields(Task::EngineTask());
}

/** Returns a list of field reference%s from the document.
 *
 * @param filter	Filter for the list.
 * @return			List of FieldReferences.
 */
std::vector<FieldReference> Document::GetFields(const FieldReference & filter) const {
	return GetFields(Task::EngineTask().Add(filter));
}

/** Returns a list of field reference%s from the document.
 *
 * @param filter	Filter for the list.
 * @return			List of FieldReferences.
 */
std::vector<FieldReference> Document::GetFields(const Task::EngineTask & filter) const {
	std::vector<FieldReference> Fields;
	Util::Variant fieldlist;
	try { fieldlist = Variant.GetChild(Util::VariantId::FieldList, 0); }
	catch(const Exceptions::General &) { return Fields; }
	for(int i = 0; i < fieldlist.NItems(); ++i) {
		FieldReference field;
		try { field.code = fieldlist[i].ToInt(); }
		catch(const Exceptions::General &) { continue; }
		field.index = 0;
		if(!EnabledField(field, filter)) continue;
		for(int j=(int)Fields.size()-1; j>=0; j--) if(Fields[(size_t)j].code == field.code) { field.index = Fields[(size_t)j].index + 1; break; }
		Fields.push_back(field);
	}
	return Fields;
}

/** Returns a simple document data of a @e "Root Document". It represents the
 * result of one analyzer method called for the document before.
 *
 * The Engine.GetRootDocument method returns a so called
 * @e "Root document" that contains the results of all methods called for
 * the current document.
 *
 * @param index		The ordinal number of the document part.
 * @return			The document part.
 */
Document Document::GetPart(int index) const {
	Util::Variant part = Variant.GetChild(Util::VariantId::TaskList, 0)[index];
	return Document(ApiHandle, part);
}

/** Returns a field of the document.
 *
 * @param group		Id of the field source.
 * @param id		Field Id.
 * @param index		Index of the fields having the same Id.
 * @return			Field data.
 */
Field Document::GetField(const FieldSource & group, const FieldId & id, int index /* 0 */) const {
	FieldReference field(group, id);
	field.index = index;
	return GetField(field);
}

/** Returns a field of the document.
 *
 * @param fieldCode	A FieldReference returned by the GetFields method.
 * @return			Field data.
 */
Field Document::GetField(const FieldReference & fieldCode) const {
	Util::Variant field = Variant.GetItemByPath(
		Util::ToString(L"C,D=%i/L,V=%i,X=%i", Util::VariantId::FieldList,
			fieldCode.code, fieldCode.index));
	return PrIns::Invoker().Construct(ApiHandle, field, fieldCode, Variant, (Field*)0);
}

/// Returns the overall status of the document.
Status Document::GetStatus(void) const {
	try { return (Status)Variant.GetChild(Util::VariantId::Checksum, 0).ToInt(); }
	catch(const Exceptions::General &) { return Status::NoChecksum; }
}

/** Saves the document to the memory in a specific file format.
 *
 * \note Only @e "Root documents" returned by Engine.GetRootDocument method
 * can be loaded back to the system.
 *
 * @param fileFormat	The FileFormat.
 * @return				Output raw memory data.
 * @see DocScanner.LoadDocument
 */
BinData Document::Save(const FileFormat & fileFormat) const {
	PrIns::GlobalData buffer;
	struct prSaveDocumentToMem {
		PrIns::gxVARIANT Doc;
		int				FileType;
		void**			Buffer;
		int*			Bufflen;
	} op = { Variant._variant, fileFormat, &buffer.buffer, &buffer.buflen };
	if(ApiHandle.Call(PrIns::ApiCallCodes::SaveDocumentToMem, &op))
		return BinData();

	if(fileFormat == FileFormat::Xml || fileFormat==FileFormat::BsiAuthLog) buffer.buflen -= 4;
	return buffer.BinData();
}

/** With the help of this operator, an integrated document can be generated.
 * This document will contain field comparison results, summary data and all
 * data of the source documents.
 *
 * Currently, only the comparison of passport VIZ photo and RFID photo is supported.
 *
 * @param other		Other document.
 * @return			The integrated document.
 */
Document Document::operator +(const Document & other) {
	if(!this->Variant._variant) return other;
	if(!other.Variant._variant) return *this;
	struct prMergeDocuments {
		PrIns::gxVARIANT Doc1;
		PrIns::gxVARIANT NDoc;
		int Relation;
	} op = { this->Variant._variant, other.Variant._variant, 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::MergeDocuments, &op);
	if(op.NDoc) return Document(ApiHandle, Util::Variant(op.NDoc));
	return *this;
}

/// Returns the results of field value comparisons.
std::vector<FieldCompare> Document::GetFieldCompareList(void) const {
	std::vector<FieldCompare> list;
	std::vector<int> arr;
	try { arr = Variant.GetChild(Util::VariantId::FieldCompares, 0).ToIntArray(); }
	catch(const Exceptions::General &) { return list; }
	for(int ix=0; ix<(int)arr.size()-2; ix+=3) {
		FieldCompare fc;
		fc.Field1.code = arr[ix];
		fc.Field2.code = arr[ix + 1];
		fc.Confidence = arr[ix + 2];
		list.push_back(fc);
	}
	return list;
}

#define ENUMSTEP 2
GX_ENUM_BEGIN(Document::FileFormat)
#include "Processing/docfiles.inl"
GX_ENUM_END(Document::FileFormat)
#undef ENUMSTEP

} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_DOCUMENT_INCL
