#ifndef PR22_DOCIMAGE_INCL
#define PR22_DOCIMAGE_INCL
/******************************************************************************/
/**	\file
 *	Scanned document image data.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Imaging/RawImage.hpp"
#include "Imaging/Imaging.hpp"
namespace Pr22 { namespace Processing { class Page; class Document; } }
//------------------------------------------------------------------------------
namespace Pr22 { namespace Imaging {

/// Scanned document image data.
class DocImage {
public:

	DocImage() : PageNumber(0) { }							// Default constructor.
	inline Light GetLight() const;							// Returns the illumination used at image scanning.
	inline Util::Variant& GetVariant();						// Returns low level data of the image.
	inline RawImage GetImage(void);							// Returns the RawImage object.
	inline DocImage DocView(void);							// Returns a rotated and cropped image.
	inline Processing::Page ToPage(void);					// Converts image to Processing::Page object.
	inline RawImage GenerateLatentImage(const Util::InputString & decodingParams);	// Generates a decoded IPI image for visual inspection.
	inline Processing::Document ReadField(const Util::Variant & readingParams);		// Reads character or barcode data from an image.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	Light light;						// The illumination used at image scanning.
	PrIns::ImageType Type;				// Image type.
	int PageNumber;						// Page index.
	Util::Variant image;				// Low level data of the image.

	friend class Pr22::PrIns::Invoker;
	inline DocImage(const PrIns::gxModule & handle, const Util::Variant & _image,
		const PrIns::ImageType & imageType, int pageNumber);	// Constructor for internal usage only.
	inline DocImage(const PrIns::gxModule & handle, const Util::Variant & _image,
		int pageNumber);									// Constructor for internal usage only.

};

} // namespace Imaging
} // namespace Pr22
/******************************************************************************
 * This is the end of the DocImage interface. Now the implementation follows  *
 ******************************************************************************/

#include "Processing/Page.hpp"

namespace Pr22 {
namespace Imaging {

// Constructor for internal usage only.
DocImage::DocImage(const PrIns::gxModule & handle, const Util::Variant & _image, const PrIns::ImageType & imageType,
	int pageNumber) : ApiHandle(handle), Type(imageType), PageNumber(pageNumber), image(_image) {
	light = (Light)image.GetChild(Util::VariantId::Light, 0).ToInt();
}

// Constructor for internal usage only.
DocImage::DocImage(const PrIns::gxModule & handle, const Util::Variant & _image, int pageNumber) :
	ApiHandle(handle), Type(PrIns::ImageType::Original), PageNumber(pageNumber), image(_image) {
	light = (Light)image.GetChild(Util::VariantId::Light, 0).ToInt();
}

/// Returns the \ref Pr22::Imaging::Light "illumination" used at image scanning.
Light DocImage::GetLight() const {
	return light;
}

/// Returns low level data of the image.
Util::Variant& DocImage::GetVariant() {
	return image;
}

/// Returns the RawImage object.
RawImage DocImage::GetImage(void) {

	PrIns::prGetImage op = { PageNumber, light, Type, 0 };
	if(Type == PrIns::ImageType::Original) return RawImage().FromVariant(image);
	ApiHandle.Call(PrIns::ApiCallCodes::GetImage, &op);
	return RawImage().FromVariant(Util::Variant(op.Img));
}

/** Returns a rotated and cropped image.
 *
 * \note For top-bottom orientation, the Processing::Page.Analyze method must be called before.
 *
 * @return			A DocImage object.
 */
DocImage DocImage::DocView(void) {
	return DocImage(ApiHandle, image, PrIns::ImageType::Document, PageNumber);
}

/// Converts image to Processing::Page object.
Processing::Page DocImage::ToPage(void)
{
	Util::Variant imglist(Util::VariantId::ImageList, Util::Variant::List);
	imglist.AddListItem(image);
	return PrIns::Invoker().Construct(ApiHandle, PageNumber, imglist, (Processing::Page*)0);
}

/** Generates a decoded IPI image for visual inspection.
 *
 * This method has the same role as Engine.GenerateLatentImage.
 *
 * @param decodingParams	IPI image decoding parameters.
 * @return					Result image.
 */
RawImage DocImage::GenerateLatentImage(const Util::InputString & decodingParams) {

	struct prDecodeLatentImage {
		int Page;
		int Light;
		int Type;
		const wchar_t* Decpar;
		PrIns::gxVARIANT Image;
	} op = { PageNumber, light, Type, decodingParams.GetString(), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::DecodeLatentImage, &op);
	return RawImage().FromVariant(Util::Variant(op.Image));
}

/** Reads character or barcode data from an image.
 *
 * This method has the same role as Engine.ReadField.
 *
 * @param readingParams		Reading parameters that are described in the Passport
 *							Reader reference manual.
 * @return					The result data structure.
 */
Processing::Document DocImage::ReadField(const Util::Variant & readingParams) {

	struct prGetOCRV {
		int Page;
		int Light;
		int Type;
		PrIns::gxVARIANT Doc;
		PrIns::gxVARIANT Params;
	} op = { PageNumber, light, Type, 0, readingParams._variant };
	ApiHandle.Call(PrIns::ApiCallCodes::GetOCRV, &op);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(op.Doc), (Processing::Document*)0);
}

} // namespace Imaging
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_DOCIMAGE_INCL
