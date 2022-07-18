#ifndef PR22_ECARDHANDLING_INCL
#define PR22_ECARDHANDLING_INCL 1
#endif//PR22_ECARDHANDLING_INCL
/******************************************************************************/
/**	\file
 *	ECard-related enumerations and classes.
 *	\namespace Pr22::ECardHandling
 *	ECard-related enumerations and classes.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Util/Types.hpp"
#include <sstream>

#define ENUMSTEP PR22_ECARDHANDLING_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace ECardHandling {

/// Applications on the ECard.
GX_ENUM_BEGIN(Application)
GXEV( None, 0 )						///< No application.
GXEV( Passport, 1 )					///< ePassport application.
GXEV( ID, 2 )						///< eID application.
GXEV( Sign, 3 )						///< eSign application.
GXEV( ISODL, 4 )					///< ISO Driving License application.
GXEV( NFC, 5 )						///< Near Field Communication application.
GXEV( EDL, 6 )						///< European Driving License application.
GX_ENUM_END(Application)

/// Level of the authentication.
GX_ENUM_BEGIN(AuthLevel)
GXEV( Max, 0 )						///< Maximal authentication executes all of the authentications, even if they are redundant or not necessary to read the files.
GXEV( Min, 1 )						///< Minimal authentication executes only those authentications that are essential to read the files.
GXEV( Opt, 2 )						///< Optimal authentication executes all of those authentications that are absolutely necessary to determine the authenticity of the document.
									///  This method avoids performing authentications that are redundant or not necessary to read the files.
GX_ENUM_END(AuthLevel)

/// Authentication IDs.
GX_ENUM_BEGIN(AuthProcess)
GXEV( None, 0 )						///< No authentication.
GXEV( Passive, 1 )					///< Passive Authentication.
GXEV( Terminal, 2 )					///< Terminal Authentication.
GXEV( PACE, 3 )						///< Password Authenticated Connection Establishment.
GXEV( BAC, 4 )						///< Basic Access %Control.
GXEV( Chip, 5 )						///< Chip Authentication.
GXEV( Active, 6 )					///< Active Authentication.
GXEV( SelectApp, 7 )				///< Select %Application.
GXEV( BAP, 8 )						///< Basic Access Protection.
GXEV( Block, 9 )					///< Block authentication.
GXEV( InitTerminal, 10 )			///< Init Terminal Authentication.
GXEV( CompleteTerminal, 11 )		///< Complete Terminal Authentication.
GXEV( StorageCard, 12 )				///< Storage Card authentication.
GX_ENUM_END(AuthProcess)

/// Files on the ECard.
GX_ENUM_BEGIN(FileId)
GXEV( Com, 0 )						///< Common content file.
GXEV( CardAccess, 28 )				///< Card Access parameter file.
GXEV( CardSecurity, 29 )			///< Card Security parameter file.
GXEV( Cvca, 30 )					///< Card Verifiable parameter file.
GXEV( Sod, 31 )						///< Security Object file.
GXEV( CertDS, 32 )					///< Document Signer certificate file.
GXEV( CertCSCA, 33 )				///< Country Signing CA certificate file.
GXEV( All, 999 )					///< All files.
GXEV( GeneralData, 1000 )			///< General data.
GXEV( PersonalDetails, 1001 )		///< Personal details.
GXEV( IssuerDetails, 1002 )			///< Issuer details.
GXEV( Portrait, 1003 )				///< Portrait.
GXEV( Signature, 1004 )				///< Signature.
GXEV( Face, 1005 )					///< Biometric face.
GXEV( Finger, 1006 )				///< Biometric finger.
GXEV( Iris, 1007 )					///< Biometric iris.
GXEV( OtherBiometricData, 1008 )	///< Other biometric data.
GXEV( DomesticData, 1009 )			///< Domestic data.
GXEV( AAData, 1010 )				///< Active authentication data.
GXEV( AnyFace, 1011 )				///< Any face or portrait.
GXEV( TocFile, 1012 )				///< Table of content data.
GXEV( EapData, 1013 )				///< Extended access control/proctection data.
GX_ENUM_END(FileId)

} // namespace ECardHandling
} // namespace Pr22
#undef ENUMSTEP
#if PR22_ECARDHANDLING_INCL == 1
//------------------------------------------------------------------------------
namespace Pr22 { namespace ECardHandling {

/** ECard file identifier class.
 *
 * Generally ECards contain data in files called @c Data @c Groups.
 * Different type of documents (e.g. ePassport and ISO-DL) stores the same data in
 * different data group. Our SDK contains FileId%s - logical file identifiers -
 * to identify data groups by their content next to the numerical file identifiers -
 * File::DG. Both of them are usable in certain situations. They can be
 * converted to each other by the ECard.ConvertFileId method.
 */
class File {
public:
	int Id;								///< Id of the file.

	// New file identifiers.
	inline explicit File(int FileIndex = 0);
	inline File(const FileId::values & fileId);
	inline File(const FileId & fileId);
	inline static File DG(int dgid);
	inline static File Block(int block, int memsize = 0);
	inline static File MemAddress(int addr, int memsize = 0);
	inline static File Sector(int sector, int memsize = 0);

	// Returns string representation of the file Id.
	inline PrOutputString ToString() const;
};

/******************************************************************************
 * This is the end of the ECardHandling interface. Now the implementation follows
 ******************************************************************************/

/** New file identifier.
 *
 * @param FileIndex	Id of the file.
 */
File::File(int FileIndex) : Id(FileIndex) {
}

/** New file identifier.
 *
 * @param fileId	Id of the file.
 */
File::File(const FileId::values & fileId) : Id(fileId) {
}

/** New file identifier.
 *
 * @param fileId	Id of the file.
 */
File::File(const FileId & fileId) : Id(fileId) {
}

/** New file identifier.
 *
 * @param dgid		Data group number.
 * @return			The file identifier.
 */
File File::DG(int dgid) {
	return File(dgid);
}

/** New file identifier of a storage card.
 *
 * @param block		Block number.
 * @param memsize	Number of bytes to read.
 * @return			The file identifier.
 */
File File::Block(int block, int memsize /* 0 */) {
	if(memsize<0 || memsize>=256) memsize = 0;
	return File(0x20000000 + (memsize << 16) + block);
}

/** New file identifier of a storage card.
 *
 * @param addr		Memory address to read from.
 * @param memsize	Number of bytes to read.
 * @return			The file identifier.
 */
File File::MemAddress(int addr, int memsize /* 0 */) {
	if(memsize<0 || memsize>=256) memsize = 0;
	return File(0x40000000 + (memsize << 16) + addr);
}

/** New file identifier of a storage card.
 *
 * @param sector	Sector number.
 * @param memsize	Number of bytes to read.
 * @return			The file identifier.
 */
File File::Sector(int sector, int memsize /* 0 */) {
	if(memsize<0 || memsize>=256) memsize = 0;
	return File(0x60000000 + (memsize << 16) + sector);
}

/// Returns string representation of the file Id.
PrOutputString File::ToString() const {
	FileId file(Id);
	const wchar_t *name = file.ToString();
	if(name) return (ToPrOutputString)name;
	const wchar_t *p[] = { L"DG", L"Block", L"Address", L"Sector" };
	int v = Id >> 29;
	std::wostringstream FileName;
	FileName << p[(v < 0 || v > 3) ? 0 : v] << (Id & 0xffff);
	return (ToPrOutputString)FileName.str();
}

} // namespace ECardHandling
} // namespace Pr22
//------------------------------------------------------------------------------
#undef PR22_ECARDHANDLING_INCL
#define PR22_ECARDHANDLING_INCL 2
#include "ECardHandling/ECardHandling.hpp"
#undef PR22_ECARDHANDLING_INCL
#define PR22_ECARDHANDLING_INCL 3
#endif
