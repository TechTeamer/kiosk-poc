#ifndef PR22_UTIL_INCL
#define PR22_UTIL_INCL 1
#endif //PR22_UTIL_INCL
/******************************************************************************/
/**	\file
 *	General enumerations.
 *	\namespace Pr22::Util
 *	General enumerations and classes.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#define ENUMSTEP PR22_UTIL_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Util {

/// IDs of Variant data.
GX_ENUM_BEGIN(VariantId)
GXEV( Root, 0 )								///< The root document structure.
GXEV( PageList, Root | 1 )					///< List of pages.
GXEV( TaskList, Root | 2 )					///< List of tasks.
GXEV( Document, 0xD0000000 )				///< Document type. Integer value to distinguish different document pages.
GXEV( FieldList, Document | 1 )				///< List of fields.
GXEV( Field, Document | 2 )					///< Field code. It encodes the @c FieldId and the @c FieldSource values.
GXEV( FieldValue, Document | 3 )			///< Raw field value data.
GXEV( Checksum, Document | 4 )				///< Field @c Status.
GXEV( Confidence, Document | 5 )			///< Field confidence. Value is in 0-1000 range, or -1 if not set.
GXEV( WindowFrame, Document | 6 )			///< Frame coordinates in micrometers.
GXEV( WindowFullFrame, Document | 7 )		///< Nominal Frame coordinates in micrometers.
GXEV( FormattedValue, Document | 9 )		///< Formatted field value data.
GXEV( StandardizedValue, Document | 10 )	///< Standardized field value data.
GXEV( Correction, Document | 11 )			///< Number of corrected errors.
GXEV( FieldCompares, Document | 12 )		///< Array of @c FieldCompare data.
GXEV( CheckDetails, Document | 13 )			///< Array of field @c Checking details.
GXEV( AMID, Document | 14)					///< Authentication method type.
GXEV( Image, 0xD0100000 )					///< Imaging::RawImage object.
GXEV( ImageList, Image | 1 )				///< List of images.
GXEV( ImageFrame, Image | 2 )				///< Frame coordinates in 1/16 pixel.
GXEV( Sample, Image | 3 )					///< Sample Imaging::RawImage for the field.
GXEV( DeviceInfo, 0xD0200000 )				///< The root of the device info data.
GXEV( DeviceId, DeviceInfo | 1 )			///< Device name.
GXEV( Light, DeviceInfo | 2 )				///< A @c Light value.
GXEV( CapTime, DeviceInfo | 3 )				///< Timestamp of the scan.
GXEV( Page, DeviceInfo | 4 )				///< Page Id.
GXEV( IdHWType, DeviceInfo | 9 )			///< Hardware module type Util::HWType.
GXEV( StatledList, DeviceInfo | 10 )		///< List of status led colors.
GXEV( Keyboard, DeviceInfo | 11 )			///< List of available key code values.
GXEV( Power, DeviceInfo | 12 )				///< Power testing capability.
GXEV( Version, DeviceInfo | 13 )			///< Version info data.
GXEV( WindowList, DeviceInfo | 14 )			///< List of object window sizes.
GXEV( Capability, DeviceInfo | 15 )			///< Scanning window capability info.
GXEV( Barcode, 0xD0300000 )					///< Barcode type.
GXEV( BarcodeValue, Barcode | 1 )			///< Barcode binary data.
GXEV( BarcodeList, Barcode | 2 )			///< List of barcodes.
GXEV( Ocr, 0xD0400000 )						///< OCR module version.
GXEV( OcrRowList, Ocr | 1 )					///< List of OCR rows.
GXEV( OcrRow, Ocr | 2 )						///< OCR row.
GXEV( OcrChar, Ocr | 3 )					///< OCR character.
GXEV( OQCA, Ocr | 0x00080000 )				///< Quality assurance alert signals.
GXEV( OQCRotation, OQCA | 1 )				///< Rotation value for quality assurance.
GXEV( OQCHeight, OQCA | 2 )					///< Height value for quality assurance.
GXEV( OQCWidth, OQCA | 3 )					///< Width value for quality assurance.
GXEV( OQCHangout, OQCA | 4 )				///< Hangout value for quality assurance.
GXEV( OQCDistance, OQCA | 5 )				///< Distance value for quality assurance.
GXEV( OQCContrast, OQCA | 6 )				///< Contrast value for quality assurance.
GXEV( OQCShape, OQCA | 7 )					///< Shape confidence for quality assurance.
GXEV( Engine, Ocr | 0x000A0000 )			///< Version of the engine.
GXEV( EngineLicense, Engine | 1 )			///< Required license of the engine.
GXEV( EngineLicDate, Engine | 2 )			///< License date of the engine.
GXEV( EngineLicType, Engine | 3)			///< License type of the engine.
GXEV( ECard, 0xD0500000 )					///< RFU
GXEV( ECardFileList, ECard | 2 )			///< List of @c ECard files.
GXEV( ECardFile, ECard | 3 )				///< @c ECard file Id.
GXEV( ECardFileData, ECard | 4 )			///< Data of an @c ECard file.
GXEV( CertIdList, ECard | 5 )				///< List of certificate IDs.
GXEV( DGList, ECard | 6 )					///< List of @c ECard file IDs.
GXEV( Card, 0xD0600000 )					///< Serial number of an @c ECard.
GXEV( CardType, Card | 1 )					///< @c ECard type string.
GXEV( CardAtr, Card | 2 )					///< @c ECard ATR data.
GXEV( MemorySize, Card | 4 )				///< Size of the memory area of a Storage Card in bytes.
GXEV( MemoryBlock, Card | 5 )				///< Size of one block of memory area of a Storage Card in bytes.
GXEV( CardCap, Card | 6 )					///< @c ECard capability data.
GXEV( ChipType, Card | 7 )					///< Chip type data.
GXEV( Common, 0xD0700000 )					///< RFU
GXEV( TimeStamp, Common | 1 )				///< Timestamp.
GXEV( Duration, Common | 2 )				///< Duration.
GXEV( WarnList, Common | 3 )				///< RFID communication warning list.
GXEV( Task, Common | 4 )					///< RFU
GXEV( OData, Common | 5 )					///< Optimization data.
GXEV( Finger, 0xD0800000 )					///< @c FingerPosition.
GXEV( FingerType, Finger | 1 )				///< Finger @c ImpressionType.
GXEV( FingerNistQuality, Finger | 2 )		///< NIST finger image quality value.
GXEV( FingerDryWet, Finger | 3 )			///< Finger humidity level.
GXEV( FingerFraction, Finger | 4 )			///< Finger fraction level.
GX_ENUM_END(VariantId)

/// Hardware module types.
GX_ENUM_BEGIN(HWType)
GXEV( Peripherals, 0 )						///< Peripheral controller device module.
GXEV( Scanner, 1 )							///< Document scanner module.
GXEV( RFIDReader, 2 )						///< RFID card reader module.
GXEV( ContactReader, 3 )					///< Contact smart card reader module.
GXEV( MagneticReader, 4 )					///< Magnetic stripe card reader module.
GX_ENUM_END(HWType)

/**	Presence detection statuses. Refers to the presence of documents,
 * @c ECard chips, fingers or face.
 * Not all the states will be reported by the different
 * kind of detectors.
 */
GX_ENUM_BEGIN(PresenceState)
GXEV( Empty, 0 )							///< No object detected.
GXEV( Moving, 1 )							///< Movement is detected.
GXEV( NoMove, 2 )							///< Document is still on the scanner's object window.
GXEV( Dirty, 3 )							///< Platen needs cleaning.
GXEV( Present, 4 )							///< Object is detected.
GXEV( Preparing, 5 )						///< Preparing to scan.
GX_ENUM_END(PresenceState)

} // namespace Util
} // namespace Pr22
//------------------------------------------------------------------------------
#undef ENUMSTEP
#if PR22_UTIL_INCL == 1
#undef PR22_UTIL_INCL
#define PR22_UTIL_INCL 2
#include "Util/Util.hpp"
#undef PR22_UTIL_INCL
#define PR22_UTIL_INCL 3
#endif
