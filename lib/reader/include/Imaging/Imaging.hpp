#ifndef PR22_IMAGING_INCL
#define PR22_IMAGING_INCL 1
#endif //PR22_IMAGING_INCL
/******************************************************************************/
/**	\file
 *	General enumerations for imaging.
 *	\namespace Pr22::Imaging
 *	Image-related enumerations and classes.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#define ENUMSTEP PR22_IMAGING_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Imaging {


/// Usable illuminations.
GX_ENUM_BEGIN(Light)
GXEV( Off, 0 )						///< %Light off.
GXEV( All, 0 )						///< All the lights.
GXEV( White, 1 )					///< White light. (Reflection removing is applied if this technology is supported by the scanner.)
GXEV( Infra, 2 )					///< Infrared light. ~900nm
GXEV( UV, 3 )						///< Ultraviolet light. ~365nm
GXEV( Coaxial, 4 )					///< Specially directed (coaxial) white light for examination of certain security foils.
GXEV( OVD, 5 )						///< White light with accumulated hologram effects. It is designed for visualization of holograms.
GXEV( Photo, 6 )					///< High resolution image targeted and focused on the photo area of passports.
GXEV( Blue, 16 )					///< Blue light.
GXEV( Red, 17 )						///< Red light.
GXEV( CleanUV, 19 )					///< Modified UV image with backgroud masking.
GXEV( CleanOVD, 21 )				///< Modified OVD image with background masking for hologram visualization.
GX_ENUM_END(Light)

/// Page selection values.
GX_ENUM_BEGIN(PagePosition)
GXEV( Next, 0 )						///< The next page will be scanned. i.e. another page of the current document even another document.
GXEV( Current, 1 )					///< The images to be scanned will be appended to the current page.
GXEV( First, 2 )					///< The first page will be scanned. It means that all the internally stored data (images, documents) will be removed from the system.
GX_ENUM_END(PagePosition)


/// Finger position codes as specified in ANSI/NIST-ITL 1 documentation.
GX_ENUM_BEGIN(FingerPosition)
GXEV( Unknown, 0 )					///< Unknown.
GXEV( RightThumb, 1 )				///< Right thumb.
GXEV( RightIndex, 2 )				///< Right index finger.
GXEV( RightMiddle, 3 )				///< Right middle finger.
GXEV( RightRing, 4 )				///< Right ring finger.
GXEV( RightLittle, 5 )				///< Right little finger.
GXEV( LeftThumb, 6 )				///< Left thumb.
GXEV( LeftIndex, 7 )				///< Left index finger.
GXEV( LeftMiddle, 8 )				///< Left middle finger.
GXEV( LeftRing, 9 )					///< Left ring finger.
GXEV( LeftLittle, 10 )				///< Left little finger.
GXEV( PlainRightThumb, 11 )			///< Plain right thumb.
GXEV( PlainLeftThumb, 12 )			///< Plain left thumb.
GXEV( PlainRight4Fingers, 13 )		///< Plain right 4 fingers.
GXEV( PlainLeft4Fingers, 14 )		///< Plain left 4 fingers.
GXEV( PlainThumbs, 15 )				///< Left and right thumbs.
GXEV( BothIndexes, 46 )				///< Left and right index fingers.
GX_ENUM_END(FingerPosition)


} // namespace Imaging
} // namespace Pr22
//------------------------------------------------------------------------------
#undef ENUMSTEP
#if PR22_IMAGING_INCL == 1
#undef PR22_IMAGING_INCL
#define PR22_IMAGING_INCL 2
#include "Imaging/Imaging.hpp"
#undef PR22_IMAGING_INCL
#define PR22_IMAGING_INCL 3
#endif
