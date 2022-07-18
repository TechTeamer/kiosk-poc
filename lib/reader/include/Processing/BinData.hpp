#ifndef PR22_BINDATA_INCL
#define PR22_BINDATA_INCL
/******************************************************************************/
/**	\file
 *	Class for storing binary or textual data.
 *	\namespace Pr22::Processing
 *	Enumerations and classes of data processing.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "PrIns/PrIns.hpp"
#include "PrIns/gxModule.hpp"
#include <vector>
namespace Pr22 { namespace PrIns { class GlobalData; } }
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {

/// Class for storing binary or textual data.
class BinData {
public:

	inline BinData();										// New storage for binary or textual data.
	inline ~BinData();
	inline BinData(const BinData &source);					// Copy constructor.
	inline BinData& operator = (const BinData &source);		// Assignment operator.
#if GX_RVALUE_REF || defined(GX_MANUAL)
	inline BinData(const BinData &&source);					// Move constructor.
	inline BinData& operator = (BinData &&source);			// Move assignment operator.
#endif

	inline BinData(const gxu8* Buffer, int Bufflen);		// New storage for binary data.

	inline const gxu8 * RawData() const;					// Returns const pointer to data.
	inline int RawSize() const;								// Returns the size of the data.

	inline PrOutputString GetDataFormat();					// Returns a file format identifier.

	inline BinData& Load(const Util::InputString & fileName);	// Loads data from a file.
	inline int Save(const Util::InputString & fileName) const;	// Saves data to a file.

	inline BinData& SetString(const Util::InputString & str);	// Generates binary data from a string.
	inline BinData& SetString(const std::vector<std::wstring> & strlist);	// Generates binary data from a string list.

	inline BinData& SetByteArray(const gxu8 *source, int srcsize);	// Sets binary data.
	inline std::vector<gxu8> ToByteArray() const;			// Stores data into a byte array.
	inline PrOutputString ToString() const;					// Stores data into a string.

	inline const std::wstring & GetComment() const;			// Returns comment for the data.

private:

	friend class Pr22::PrIns::GlobalData;
	inline int SetData(const void *source, int srcsize);	// Sets data.
	inline BinData(const gxu8* Buffer, int Bufflen, const std::wstring &comm);	// New storage for binary data.

	wchar_t dataformat[20];				// Temp for file format.
	gxu8* buffer;						// The data.
	int buffsize;						// Size of the data.
	std::wstring comment;				// Comment for the data.
};

} // namespace Processing
/******************************************************************************
 * This is the end of the BinData interface. Now the implementation follows   *
 ******************************************************************************/

namespace PrIns {

/// Internally used binary data.
class GlobalData {
	GlobalData(const GlobalData &source);
public:

	void *buffer;						///< Data pointer.
	int buflen;							///< Data size.

	/// Initialize global data.
	GlobalData(void *buf = NULL, int len = 0) : buffer(buf), buflen(len) { }
	~GlobalData() {
		if(!buffer) return;
		struct gxGlobalRealloc {
			void* buffer;
			int size;
		} gf = { buffer, 0 };
		gxHANDLE nullhandle = { 0 };
		if(!PrIns::gxCall::Call(nullhandle, 0x10020000, &gf)) {
			assert(0);
		}
	}

	/// Returns data converted to BinData.
	Processing::BinData BinData() const {
		return Processing::BinData((const gxu8*)buffer, buflen);
	}

	/// Returns data converted to BinData.
	Processing::BinData BinData(const std::wstring & comment) const {
		return Processing::BinData((const gxu8*)buffer, buflen, comment);
	}
};

} // namespace PrIns
//------------------------------------------------------------------------------

namespace Processing {

/// New storage for binary or textual data.
BinData::BinData() : buffer(NULL), buffsize(0), comment() {
	dataformat[0] = 0;
}

BinData::~BinData() {
	if(buffer) { free(buffer); buffer = NULL; }
}

/// Copy constructor, references an existing bindata.
BinData::BinData(const BinData &source) : buffer(NULL), buffsize(0), comment() {
	*this = source;
}

/// Assignment operator, copies an existing bindata.
BinData& BinData::operator = (const BinData &source) {
	memcpy(dataformat, source.dataformat, sizeof(dataformat));
	comment = source.comment;
	SetData(source.buffer, source.buffsize);
	return *this;
}

#if GX_RVALUE_REF || defined(GX_MANUAL)
/// Move constructor, references an existing bindata.
BinData::BinData(const BinData &&source) : buffer(NULL), buffsize(0), comment() {
	*this = source;
}

/// Move assignment operator, references an existing bindata.
BinData& BinData::operator = (BinData &&source) {
	if(buffer) { free(buffer); buffer = NULL; }
	memcpy(dataformat, source.dataformat, sizeof(dataformat));
	comment = std::move(source.comment);
	buffsize = source.buffsize;
	buffer = source.buffer;
	source.buffsize = 0;
	source.buffer = NULL;
	return *this;
}
#endif

/// New storage for binary data.
BinData::BinData(const gxu8* Buffer, int Bufflen) : buffer(NULL), buffsize(Bufflen), comment() {
	dataformat[0] = 0;
	SetData(Buffer, Bufflen);
}

BinData::BinData(const gxu8* Buffer, int Bufflen, const std::wstring &comm) : buffer(NULL), buffsize(Bufflen), comment(comm) {
	dataformat[0] = 0;
	SetData(Buffer, Bufflen);
}

/// Returns const pointer to data.
const gxu8 * BinData::RawData() const {
	return buffer;
}

/// Returns the size of the data.
int BinData::RawSize() const {
	return buffsize;
}

/// Returns a file format identifier.
PrOutputString BinData::GetDataFormat()
{
	if(dataformat[0]) return (ToPrOutputString)dataformat;
	PrIns::gxModule ApiModule(L"prapi", L"default");
	struct prGetDataFormat {
		const gxu8		*Buffer;
		int				BufLen;
		wchar_t			*Format;
		int				FormatLen;
	} gdf = { buffer, buffsize, dataformat, gx_arraycnt(dataformat) };
	if(!ApiModule.Call(PrIns::ApiCallCodes::GetDataFormat, &gdf))
		return (ToPrOutputString)dataformat;
	return PrOutputString();
}

/** Loads data from a file.
 *
 * @param fileName	File name to load from.
 * @return			This object.
 */
BinData& BinData::Load(const Util::InputString & fileName) {
	if(buffer) { free(buffer); buffer = NULL; }
	dataformat[0] = 0;
	comment = L"";

	struct LoadBinary {
		void		*buffer;
		const void	*filename;
		int			size;
		int			strtype;
	} lbd = { 0, 0, 0, 0 };

	if(fileName.GetAString()) {
		lbd.filename = fileName.GetAString();
		lbd.strtype = 1;
	}
	else {
		lbd.filename = fileName.GetString();
		lbd.strtype = 2;
	}

	int ret = PrIns::gxModule::CallDirect(0, PrIns::ModuleCallCodes::LoadBinary, &lbd);
	if(!ret) {
		SetData(lbd.buffer, lbd.size);
		PrIns::GlobalData(lbd.buffer);
	}
	return *this;
}

/** Saves data to a file.
 *
 * @param fileName	File name to save to.
 * @return			This object.
 */
int BinData::Save(const Util::InputString & fileName) const {

	struct SaveBinary {
		void		*buffer;
		const void	*filename;
		int			size;
		int			strtype;
	} sbd = { buffer, 0, buffsize, 0 };

	if(fileName.GetAString()) {
		sbd.filename = fileName.GetAString();
		sbd.strtype = 1;
	}
	else {
		sbd.filename = fileName.GetString();
		sbd.strtype = 2;
	}

	return PrIns::gxModule::CallDirect(0, PrIns::ModuleCallCodes::SaveBinary, &sbd);
}

/** Generates binary data from a string.
 *
 * @param str		The string to store.
 * @return			This object.
 */
BinData& BinData::SetString(const Util::InputString & str) {
	dataformat[0] = 0;
	comment = L"";
	const wchar_t *src = str.GetString();
	SetData(src, src ? int((wcslen(src)+1)*sizeof(wchar_t)) : 0);
	return *this;
}

/** Generates binary data from a string list.
 *
 * @param strlist	The string list to store.
 * @return			This object.
 */
BinData& BinData::SetString(const std::vector<std::wstring> & strlist) {
	std::wstring TextRep;
	for(unsigned int i=0; i<strlist.size(); i++) {
		if(i) TextRep += L"\n";
		TextRep += strlist[i];
	}
	return SetString(TextRep);
}

/** Sets binary data.
 *
 * @param source	Source binary data.
 * @param srcsize	Size of the source data.
 * @return			This object.
 */
BinData& BinData::SetByteArray(const gxu8 *source, int srcsize) {
	dataformat[0] = 0;
	comment = L"";
	SetData(source, srcsize);
	return *this;
}

/// Stores data into a byte array.
std::vector<gxu8> BinData::ToByteArray() const {
	return std::vector<gxu8>(buffer, buffer+buffsize);
}

/// Stores data into a string.
PrOutputString BinData::ToString() const {

	wchar_t *sb = (wchar_t*)buffer;
	if(buffer && buffsize && !(buffsize%sizeof(wchar_t)) &&
		!sb[buffsize/sizeof(wchar_t)-1])
			return (ToPrOutputString)sb;
	return ToPrOutputString(L"");
}

/// Returns comment for the data.
const std::wstring & BinData::GetComment() const {
	return comment;
}

int BinData::SetData(const void *source, int srcsize) {

	if(buffer) free(buffer);
	buffer = NULL;
	buffsize = srcsize;
	if(srcsize) {
		buffer = (gxu8*)malloc((size_t)buffsize);
		if(!buffer) {
			buffsize = 0;
			throw Exceptions::MemoryAllocation(L"[pr22] (BinData)");
		}
		else memcpy(buffer, source, (size_t)buffsize);
	}
	return 0;
}

} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_BINDATA_INCL
