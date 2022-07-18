#ifndef PR22_PAGE_INCL
#define PR22_PAGE_INCL
/******************************************************************************/
/**	\file
 *	Collection of images taken by the DocScanner.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/Document.hpp"
namespace Pr22 { namespace Imaging { class DocImage; } }
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {

/** Stores the images taken by the DocScanner.Scan method.
 *
 * Images scanned earlier are also available with the DocScanner.GetPage
 * method.
 */
class Page {
public:

	Page() : PageNumber(0) { }								// Default constructor.
	inline Page& Del(const Imaging::Light & light);			// Removes an image from the page.
	inline Imaging::DocImage Select(const Imaging::Light & light);	// Returns an image.
	inline Imaging::DocImage SelectByIndex(int index);		// Returns an image.
	inline Document Analyze(const Task::EngineTask & task);	// Analyzes the image data of the page.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	int PageNumber;						// Page index.
	Util::Variant ImageList;			// List of images.

	friend class Pr22::PrIns::Invoker;
	Page(const PrIns::gxModule & handle, int pageNum, const Util::Variant & imageList) :
		ApiHandle(handle), PageNumber(pageNum), ImageList(imageList)
	{ }	// Constructor for internal usage only.
};

} // namespace Processing
} // namespace Pr22
/******************************************************************************
 * This is the end of the Page interface. Now the implementation follows      *
 ******************************************************************************/

#include "Imaging/DocImage.hpp"

namespace Pr22 {
namespace Processing {

/** Removes an image from the page.
 *
 * This removal is made only with the current data object. Other calls
 * of DocScanner.GetPage method will contain all the scanned images.
 * This removal is used for omitting certain illuminations from an analyzing
 * process.
 * @param light		Id of the Light to remove.
 * @return			This object.
 */
Page& Page::Del(const Imaging::Light & light) {
	Util::Variant imagelist(Util::VariantId::ImageList, Util::Variant::List);
	for(int i=0; i<ImageList.NItems(); i++) {
		if(ImageList[i].GetChild(Util::VariantId::Light, 0).ToInt() != light)
			imagelist.AddListItem(ImageList[i]);
	}
	ImageList = imagelist;
	return *this;
}

/** Returns an image.
 *
 * @param light		Id of the Imaging::Light.
 * @return			A Imaging::DocImage.
 */
Imaging::DocImage Page::Select(const Imaging::Light & light) {
	for(int i=0; i<ImageList.NItems(); i++) {
		if(ImageList[i].GetChild(Util::VariantId::Light, 0).ToInt() == light) {
			return PrIns::Invoker().Construct(ApiHandle, ImageList[i], PageNumber, (Imaging::DocImage*)0);
		}
	}
	throw Exceptions::EntryNotFound(L"[pr22] (light)");
}

/** Returns an image.
 *
 * @param index		Ordinal number of the image.
 * @return			A Imaging::DocImage.
 */
Imaging::DocImage Page::SelectByIndex(int index) {
	if(ImageList.NItems()>=index) {
		return PrIns::Invoker().Construct(ApiHandle, ImageList[index], PageNumber, (Imaging::DocImage*)0);
	}
	throw Exceptions::DataOutOfRange(L"[pr22] (index)");
}

/** Analyzes the image data of the page and returns the textual
 * and logical data according to task.
 *
 * This method has the same role as Engine.Analyze.
 *
 * @param task		Specifies data field references to analyze.
 * @return			The result data structure.
 */
Document Page::Analyze(const Task::EngineTask & task) {
	struct prAnalyze {
		PrIns::gxVARIANT	Page;
		int					Tasklen;
		const int			*Task;
		PrIns::gxVARIANT	Doc;
	} an = { ImageList._variant, (int)task.fields.size(), (int*)PrIns::vector_data(task.fields), 0 };
	ApiHandle.Call(PrIns::ApiCallCodes::Analyze, &an);
	return PrIns::Invoker().Construct(ApiHandle, Util::Variant(an.Doc), (Document*)0);
}

} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_PAGE_INCL
