#ifndef PR22_RAWIMAGE_INCL
#define PR22_RAWIMAGE_INCL
/******************************************************************************/
/** \file
 * Low level image data.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Util/Variant.hpp"
#include "Processing/BinData.hpp"
#ifdef QT_CORE_LIB
#include <QBuffer>
#include <QByteArray>
#endif
//------------------------------------------------------------------------------
namespace Pr22 {

namespace PrIns { struct gxIMAGE; }

namespace Imaging {

/// Low level image data of the system that can be saved.
class RawImage {
public:

	#define ENUMSTEP 1
	/// Available pixel formats
	GX_ENUM_BEGIN(PixelFormat)
	#include "Imaging/pixels.inl"
	GX_ENUM_END(PixelFormat)
	/// Image file formats
	GX_ENUM_BEGIN(FileFormat)
	#include "Imaging/imgfiles.inl"
	GX_ENUM_END(FileFormat)
	#undef ENUMSTEP

	/// @name General RawImage functionality
	/// @{
	inline RawImage();										// Default constructor.
	inline ~RawImage();										// Destructor.
	inline RawImage& Leave(void);							// Finishes working with the actual image.
	inline bool IsValid() const;							// Checks if the image is valid.
	inline RawImage(const RawImage& source);				// Copy constructor.
	inline RawImage& operator = (const RawImage& source);	// Assignment operator.
#if GX_RVALUE_REF || defined(GX_MANUAL)
	inline RawImage(RawImage&& source);						// Move constructor.
	inline RawImage& operator = (RawImage&& source);		// Move assignment operator.
#endif
	inline RawImage& Ref(const RawImage& source);			// References an existing image.
	template<class TSIZE> inline TSIZE Size() const;		// Returns the size of the image in pixels.
	inline int HRes() const;								// Returns the horizontal resolution of the image
	inline int VRes() const;								// Returns the vertical resolution of the image
	inline RawImage Duplicate(void) const;					// Duplicates the image
	/// @}

	/// @name Image conversion
	/// @{
	inline RawImage& Load(const Processing::BinData &buffer);	// Loads an image from the memory.
	inline Processing::BinData Save(const FileFormat &fileformat) const;	// Saves the image to the memory.
	inline Processing::BinData Save(const FileFormat &fileformat, const Util::Variant &properties,
		const Util::InputString &comments) const;				// Saves the image to the memory.
	inline Util::Variant ToVariant(int varid, const Util::InputString &varname) const;	// Embeds an image into a variant.
	inline RawImage& FromVariant(const Util::Variant &source);	// Gets an embedded image from a variant.
	template<class QIMAGE> inline QIMAGE ToQImage() const;		// Returns the image as a Qt image.
	template<class QIMAGE> inline RawImage& FromQt(const QIMAGE &source);	// Converts a a Qt image to RawImage.
	/// @}

	/// @name Low level data manipulation
	/// @{
	inline operator const PrIns::gxIMAGE *() const;			// Conversion operator to gxIMAGE*.
	inline PixelFormat Format() const;						// Returns the PixelFormat of the image.
	inline int	 SLine() const;								// Returns the length of a row in bytes.
	inline void* RawData() const;							// Returns the address of the buffer of the raw pixel data.
	inline int	 RawSize() const;							// Returns the size of the raw pixel data.
	inline RawImage& Create(PixelFormat format, int xsize, int ysize, int sline);	// Creates an image.
	inline RawImage Convert(PixelFormat format, int xsize, int ysize, int sline) const;	// Converts the image to a specific pixel format and size.
	/// @}

private:

	inline PrIns::GlobalData & Save(const FileFormat &format, PrIns::GlobalData &buffer,
		PrIns::gxVARIANT properties, PrIns::gxVARIANT comments) const;	// Saves the image to the memory.
	inline RawImage& Alloc();								// Allocates and initializes a blank image.
	inline static int Call(int cmd, void* params);			// Calls the gx system to do some RawImage functionality.

	PrIns::gxIMAGE*	pimage;				// Internal variable to store object data.
};
} // namespace Imaging
/******************************************************************************
 * This is the end of the RawImage interface. Now the implementation follows  *
 ******************************************************************************/

#ifndef GX_MANUAL
namespace PrIns {

// Structure to store data of a raw image
struct gxIMAGE {
	gxi32	gxid;
	gxi32	ref;
	void*	image;
	gxi32	simage;
	gxi32	format;
	gxi32	xsize;
	gxi32	ysize;
	gxi32	sline;
	gxi32	xres;
	gxi32	yres;
};

/* GX call codes */
struct RawImageCallCodes {
enum codes {
	CallCodes			= 0x11040000,
	GetPixelSize		= CallCodes ,
	AllocImage			= CallCodes | 0x0007,
	RefImage			= CallCodes | 0x0009,
	UnrefImage			= CallCodes | 0x000A,
	CreateImage			= CallCodes | 0x000B,
	ConvertImage		= CallCodes | 0x000C,
	LoadRawPixels		= CallCodes | 0x0014,
	IsValidImage		= CallCodes | 0x0016,
	DuplicateImage		= CallCodes | 0x0021,
	ImageToVariant		= CallCodes | 0x0022,
	ImageFromVariant	= CallCodes | 0x0023,
	SaveImage			= CallCodes | 0x0029,
};
};

} // namespace PrIns
#endif /* GX_MANUAL */
//------------------------------------------------------------------------------

namespace Imaging {

/// Default constructor, defines an empty image object.
RawImage::RawImage() : pimage(0) { }
											
/// Destructor.
RawImage::~RawImage() {
	try {
		Leave();
	}
	catch(...) { }
}

/** Finishes working with the actual image.
 *
 * @return			The image object.
 */
RawImage& RawImage::Leave(void) {
	if(pimage) {
		int err = Call(PrIns::RawImageCallCodes::UnrefImage, (void *)pimage);
		assert(!err);
		(void) err;
	}
	pimage = 0;
	return *this;
}

/** Checks if the image is valid.
 *
 * @return			In case of valid image the return value is true.
 */
bool RawImage::IsValid() const {
	try {
		return !Call(PrIns::RawImageCallCodes::IsValidImage, (void *)pimage);
	}
	catch(const Exceptions::General &) {
		return false;
	}
}

/** Copy constructor, references an existing image.
 *
 * @param source	Source image.
 */
RawImage::RawImage(const RawImage& source) : pimage(0) {
	Ref(source);
}

/// Assignment operator, references an existing image.
RawImage& RawImage::operator = (const RawImage& source) {
	return Ref(source);
}

#if GX_RVALUE_REF || defined(GX_MANUAL)
/// Move constructor, references an existing image.
RawImage::RawImage(RawImage&& source) : pimage(0) {
	*this = source;
}

/// Move assignment operator, references an existing image.
RawImage& RawImage::operator = (RawImage&& source) {
	if(pimage==source.pimage) return *this;

	if(pimage) {
		int err = Call(PrIns::RawImageCallCodes::UnrefImage, (void *)pimage);
		assert(!err);
		(void) err;
	}

	pimage = source.pimage;
	source.pimage = 0;
	return *this;
}
#endif

/** References an existing image.
 *
 * @param source	Source image.
 * @return			The image object.
 */
RawImage& RawImage::Ref(const RawImage& source) {
	if(pimage==source.pimage) return *this;

	if(source.pimage && Call(PrIns::RawImageCallCodes::RefImage, source.pimage))
		return *this;

	PrIns::gxIMAGE *tmp = pimage;
	pimage = source.pimage;

	if(tmp) {
		int err = Call(PrIns::RawImageCallCodes::UnrefImage, tmp);
		assert(!err);
		(void) err;
	}
	return *this;
}

/// Returns the size of the image in pixels.
template<class TSIZE>
TSIZE RawImage::Size() const {
	if(pimage) return TSIZE(pimage->xsize, pimage->ysize);
	return TSIZE(-1, -1);
}

/// Returns the horizontal resolution of the image in pixel/meters.
int RawImage::HRes() const {
	if(pimage) return pimage->xres;
	return -1;
}

/// Returns the vertical resolution of the image in pixel/meters.
int RawImage::VRes() const {
	if(pimage) return pimage->yres;
	return -1;
}

/** Duplicates the image.
 *
 * @return			The duplicated image.
 */
RawImage RawImage::Duplicate(void) const {
	RawImage img;
	img.Alloc();
	struct DuplicateImage {
		PrIns::gxIMAGE*	target;
		PrIns::gxIMAGE*	source;
	} dp = { img.pimage, pimage };
	if(!img.Call(PrIns::RawImageCallCodes::DuplicateImage, &dp))
		img.pimage = dp.target;
	return img;
}

/** Loads an image from the memory.
 *
 * @param buffer	Image data.
 * @return			This object.
 */
RawImage& RawImage::Load(const Processing::BinData &buffer) {
	Alloc();
	struct LoadRawPixels {
		PrIns::gxIMAGE*	pimage;
		const void*		buffer;
		gxi32			buflen;
		gxi32			format;
	} lrp = { pimage, buffer.RawData(), buffer.RawSize(), 0 };
	Call(PrIns::RawImageCallCodes::LoadRawPixels, &lrp);
	return *this;
}

/** Saves the image to the memory in a specific file format.
 *
 * @param fileformat	The FileFormat.
 * @return				Output raw memory data.
 */
Processing::BinData RawImage::Save(const FileFormat &fileformat) const {
	PrIns::GlobalData buffer;
	return Save(fileformat, buffer, 0, 0).BinData();
}

/** Saves the image to the memory in a specific file format.
 *
 * @param fileformat	The FileFormat.
 * @param properties	Properties for saving. The properties are described in the
 *						reference manual.
 * @param comments		Comments for save.
 * @return				Output raw memory data.
 */
Processing::BinData RawImage::Save(const FileFormat &fileformat,
			const Util::Variant &properties, const Util::InputString &comments) const
{
	PrIns::GlobalData buffer;
	Util::Variant varcomm;
	if(comments.GetString()) {
		varcomm.CreateList(0, (wchar_t*)0);
		varcomm << Util::Variant(0, comments.GetString());
	}
	return Save(fileformat, buffer, properties, varcomm).BinData();
}

PrIns::GlobalData & RawImage::Save(const FileFormat &format, PrIns::GlobalData &buffer,
							PrIns::gxVARIANT properties, PrIns::gxVARIANT comments) const
{
	assert(format>=FileFormat::Bmp && format<=FileFormat::Wsq);
	const wchar_t *formats[] = { L"", L"bmp", L"jpeg", L"jpeg2k_jpc", L"jpeg2k",
		L"raw", L"png", L"wsq" };
	struct SaveImageToMem2 {
		const PrIns::gxIMAGE*	pimage;
		void**			buffer;
		gxi32*			buflen;
		const wchar_t*	fileformat;
		void*			properties;
		void*			comments;
	} sim = { pimage, &buffer.buffer, &buffer.buflen, formats[format],
		properties, comments };
	Call(PrIns::RawImageCallCodes::SaveImage, &sim);
	return buffer;
}

/** Embeds an image into a variant.
 *
 * @param varid		The Id for the created variant.
 * @param varname	The name for the created variant.
 * @return			The created Util::Variant.
 */
Util::Variant RawImage::ToVariant(int varid, const Util::InputString &varname) const {
	struct ImageToVariant {
		PrIns::gxIMAGE*		pimage;
		int				varid;
		PrIns::gxVARIANT	variant;
	} itv = { pimage, varid, 0 };
	Call(PrIns::RawImageCallCodes::ImageToVariant, &itv);
	if(varname.GetString()) {
		return Util::Variant(itv.variant).SetName(varname);
	}
	return Util::Variant(itv.variant);
}

/** Gets an embedded image from a variant.
 *
 * @param source	Variant with an embedded image.
 * @return			This object.
 */
RawImage& RawImage::FromVariant(const Util::Variant &source) {
	Leave();
	struct ImageFromVariant {
		PrIns::gxIMAGE*		pimage;
		PrIns::gxVARIANT	variant;
	} ifv = { pimage, source._variant };
	if(!Call(PrIns::RawImageCallCodes::ImageFromVariant, &ifv))
		pimage = ifv.pimage;
	return *this;
}

/** Returns the image as a Qt QImage or QPixmap object.
 *
 * @return			QImage or QPixmap object.
 */
template<class QIMAGE>
QIMAGE RawImage::ToQImage() const {
	PrIns::GlobalData buffer;
	Save(FileFormat::Bmp, buffer, 0, 0);
	QIMAGE img;
	img.loadFromData((const gxu8*)buffer.buffer, buffer.buflen, "bmp");
	return img;
}

#if defined(QT_CORE_LIB) || defined(GX_MANUAL)
/** Converts a a Qt QImage or QPixmap object to RawImage.
 *
 * @param source	The Qt image object to be converted to RawImage.
 * @return			This object.
 */
template<class QIMAGE>
RawImage& RawImage::FromQt(const QIMAGE &source) {
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	source.save(&buffer, "bmp");
	return Load(Processing::BinData((gxu8*)bytes.data(), bytes.size()));
}
#endif

/** Conversion operator to gxIMAGE*.
 *
 * \note This operator is for internal usage only.
 * 
 * @return Pointer to the internally stored gxIMAGE data.
 */
RawImage::operator const PrIns::gxIMAGE *() const {
	return pimage;
}

/// Returns the PixelFormat of the image.
RawImage::PixelFormat RawImage::Format() const {
	if(pimage) return (PixelFormat)pimage->format;
	return PixelFormat::Undef;
}

/// Returns the length of a row in bytes.
int RawImage::SLine() const {
	if(pimage) return pimage->sline;
	return -1;
}

/// Returns the address of the buffer of the raw pixel data.
void* RawImage::RawData() const {
	if(pimage) return pimage->image;
	return NULL;
}

/// Returns the size of the raw pixel data.
int RawImage::RawSize() const {
	if(pimage) return pimage->simage;
	return -1;
}

RawImage& RawImage::Alloc() {
	Leave();
	Call(PrIns::RawImageCallCodes::AllocImage, &pimage);
	return *this;
}

/** Creates an image.
 *
 * @param format	PixelFormat.
 * @param xsize		Horizontal size of image in pixels.
 * @param ysize		Vertical size of image in pixels.
 * @param sline		Length of a row in bytes (min.: xsize * size of a pixel in
 *					bytes). If this parameter is 0, the system computes the
 *					correct value.
 * @return			This object.
 */
RawImage& RawImage::Create(PixelFormat format, int xsize, int ysize, int sline) {
	assert(xsize>0 && ysize>0);
	Alloc();
	struct CreateImage {
		PrIns::gxIMAGE*		pimage;
		gxi32				format;
		gxi32				xsize;
		gxi32				ysize;
		gxi32				sline;
	} ci = { pimage, format, xsize, ysize, sline };
	Call(PrIns::RawImageCallCodes::CreateImage, &ci);
	return *this;
}

/** Converts the image to a specific pixel format and size.
 *
 * @param format	PixelFormat of the target image. If this parameter is
 *					PixelFormat::Undef, the format remains the same.
 * @param xsize		Horizontal size of the target image in pixels. If this
 *					parameter is 0, the width remains the same.
 * @param ysize		Vertical size of the target image in pixels. If this
 *					parameter is 0, the height remains the same.
 * @param sline		Length of a row in bytes (min.: xsize * size of a pixel in
 *					bytes). If this parameter is 0, the system computes the
 *					correct value.
 * @return			The converted image in the desired pixel format and size.
 */
RawImage RawImage::Convert(PixelFormat format, int xsize, int ysize, int sline) const {
	assert(sline >= 0);
	RawImage img;
	img.Alloc();
	struct ConvertImage {
		PrIns::gxIMAGE*		pdest;
		PrIns::gxIMAGE*		psrc;
		gxi32				format;
		gxi32				xsize;
		gxi32				ysize;
		gxi32				sline;
	} cp = { img.pimage, pimage, (format!=PixelFormat::Undef ? format : Format()),
		(xsize ? xsize : pimage->xsize), (ysize ? ysize : pimage->ysize), sline };
	if(!img.Call(PrIns::RawImageCallCodes::ConvertImage, &cp))
		img.pimage = cp.pdest;
	return img;
}

int RawImage::Call(int cmd, void* params) {
	return PrIns::gxModule::CallDirect(0xffff1104, cmd, params);
}

#define ENUMSTEP 2
GX_ENUM_BEGIN(RawImage::PixelFormat)
#include "Imaging/pixels.inl"
GX_ENUM_END(RawImage::PixelFormat)
GX_ENUM_BEGIN(RawImage::FileFormat)
#include "Imaging/imgfiles.inl"
GX_ENUM_END(RawImage::FileFormat)
#undef ENUMSTEP

} // namespace Imaging
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_RAWIMAGE_INCL
