#ifndef PR22_FIELDID_INCL
#define PR22_FIELDID_INCL 1
#endif //PR22_FIELDID_INCL
/******************************************************************************/
/**	\file
 *	Field IDs.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#define ENUMSTEP PR22_FIELDID_INCL
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace Processing {

/// @c Field IDs.
GX_ENUM_BEGIN(FieldId)
GXEV( All, 0)									///< All fields.
GXEV( Composite1, 1)							///< Not specified / composite data #1.
GXEV( Composite2, 2)							///< Not specified / composite data #2.
GXEV( Composite3, 3)							///< Not specified / composite data #3.
GXEV( Composite4, 4)							///< Not specified / composite data #4.
GXEV( Composite5, 5)							///< Not specified / composite data #5.
GXEV( Name, 101)								///< Full name.
GXEV( Givenname, 102)							///< Given name.
GXEV( Surname, 103)								///< Family name.
GXEV( Surname2, 104)							///< Family name 2nd.
GXEV( MiddleName, 105)							///< Middle name.
GXEV( MaidenName, 107)							///< Full name at the time of birth.
GXEV( MaidenGivenname, 108)						///< Given name at the time of birth.
GXEV( MaidenSurname, 109)						///< Family name at the time of birth.
GXEV( BirthDate, 113)							///< Date of birth.
GXEV( BirthPlace, 114)							///< Place of birth.
GXEV( BirthCountry, 115)						///< Country of birth.
GXEV( MotherName, 116)							///< Mother's full name.
GXEV( MotherGivenname, 117)						///< Mother's given name.
GXEV( MotherSurname, 118)						///< Mother's family name.
GXEV( Nationality, 119)							///< Nationality.
GXEV( Sex, 120)									///< Gender.
GXEV( BirthProvince, 121)						///< Province of birth.
GXEV( ParentsName, 122)							///< Given name of the parents.
GXEV( FatherName, 123)							///< Father's full name.
GXEV( MaritalStatus, 124)						///< Marital status.
GXEV( FatherGivenname, 125)						///< Father's given name.
GXEV( FatherSurname, 126)						///< Father's family name.
GXEV( DocumentNumber, 130)						///< %Document number.
GXEV( Type, 131)								///< Type of document as stated by the document.
GXEV( IssueCountry, 132)						///< Issuing country.
GXEV( IssuePlace, 133)							///< Place of issue.
GXEV( IssueState, 134)							///< State of issue.
GXEV( IssueDate, 135)							///< Date of issue.
GXEV( ExpiryDate, 136)							///< Date of expiry.
GXEV( IssueOrg, 137)							///< Issuing organization.
GXEV( ProvisionalEndDate, 138)					///< End of provisional period.
GXEV( PersonalNumber, 140)						///< Personal identification number.
GXEV( HealthInsNumber, 141)						///< Health insurance identification number.
GXEV( RegistrationNumber, 142)					///< Registration number.
GXEV( DUFNumber, 143)							///< DUF number.
GXEV( CAN, 144)									///< Card access number.
GXEV( DocType, 149)								///< Type of document as recognized by the engine.
GXEV( DocPage, 150)								///< %Page of the document as recognized by the engine.
GXEV( DocTypeDisc, 151)							///< %Document type discriminator.
GXEV( PersonalData1, 160)						///< Additional personal data #1.
GXEV( PersonalData2, 161)						///< Additional personal data #2.
GXEV( Address, 200)								///< Full address.
GXEV( AddressZip, 201)							///< Address zip code.
GXEV( AddressCity, 202)							///< Address city.
GXEV( AddressStreet, 203)						///< Address street.
GXEV( AddressHouse, 204)						///< Address building number.
GXEV( AddressFlat, 205)							///< Address flat number.
GXEV( AddressDate, 206)							///< Registration date of the address.
GXEV( AddressState, 209)						///< Address state.
GXEV( AddressProvince, 210)						///< Address province.
GXEV( AddressMunicipality, 212)					///< Address municipality.
GXEV( TempAddress, 220)							///< Full temporary address.
GXEV( TempAddressZip, 221)						///< Temporary address zip code.
GXEV( TempAddressCity, 222)						///< Temporary address city.
GXEV( TempAddressStreet, 223)					///< Temporary address street.
GXEV( TempAddressHouse, 224)					///< Temporary address building number.
GXEV( TempAddressFlat, 225)						///< Temporary address flat number.
GXEV( TempAddressDate, 226)						///< Registration date of temporary address.
GXEV( TempAddressDateEnd, 227)					///< Expiry date of temporary address.
GXEV( TempAddressState, 229)					///< Temporary address State.
GXEV( TempAddressProvince, 230)					///< Temporary address province.
GXEV( TempAddressMunicipality, 232)				///< Temporary address municipality.
GXEV( ValidArea, 240)							///< Validity area.
GXEV( StartOfValidity, 241)						///< Start of validity.
GXEV( NumberOfEntries, 242)						///< Number of entries.
GXEV( DurationOfStay, 243)						///< Duration of stay.
GXEV( BankAccount, 302)							///< Bank account number.
GXEV( FromAddress, 320)							///< Source address.
GXEV( DestAddress, 321)							///< Destination address.
GXEV( FlightNo, 322)							///< Flight number.
GXEV( Compartment, 323)							///< Compartment.
GXEV( SeatNo, 324)								///< Seat number.
GXEV( CheckInSequenceNo, 325)					///< Check in sequence number.
GXEV( Face, 400)								///< Face.
GXEV( CompositeMrz, 410)						///< Composite Mrz data stored in other sources.
GXEV( Signature, 420)							///< Signature.
GXEV( Fingerprint, 430)							///< Fingerprint.
GXEV( Iris, 440)								///< Iris.
GXEV( BiometricData, 450)						///< Biometric data block.
GXEV( Height, 490)								///< Height.
GXEV( Weight, 491)								///< Weight.
GXEV( EyeColor, 492)							///< Eye color.
GXEV( HairColor, 493)							///< Hair color.
GXEV( Authenticity, 500)						///< General authenticity.
GXEV( Authenticity1, 501)						///< Authenticity #1.
GXEV( Authenticity2, 502)						///< Authenticity #2.
GXEV( Authenticity3, 503)						///< Authenticity #3.
GXEV( Authenticity4, 504)						///< Authenticity #4.
GXEV( Authenticity5, 505)						///< Authenticity #5.
GXEV( Telephone, 600)							///< Telephone number.
GXEV( Profession, 601)							///< Profession.
GXEV( Title, 602)								///< Title.
GXEV( PersonalSummary, 603)						///< Personal summary.
GXEV( ProofOfCitizenship, 604)					///< Proof of citizenship.
GXEV( OtherTdNumbers, 605)						///< Other travel document numbers.
GXEV( CustodyInformation, 606)					///< Custody information.
GXEV( OtherName, 607)							///< Other name.
GXEV( OtherPeople, 608)							///< Other people.
GXEV( Observations, 609)						///< Observations.
GXEV( Tax, 610)									///< Tax/exit requirements.
GXEV( FrontImage, 611)							///< Image of front page of the document.
GXEV( RearImage, 612)							///< Image of back page of the document.
GXEV( PersonalizationDate, 613)					///< Date of personalization.
GXEV( PersonalizationSystem, 614)				///< Serial number of personalization device.	
GXEV( OrganDonor, 615)							///< Organ donor.
GXEV( Remarks, 616)								///< Remarks.
GXEV( BloodType, 617)							///< Blood type.
GXEV( Rank, 618)								///< Rank.
GXEV( ServiceBranch, 619)						///< Service branch.
GXEV( CAR, 620)									///< Certification Authority Reference.
GXEV( CHR, 621)									///< Certificate Holder Reference.
GXEV( LowLevel, 997)							///< Signals that optical recognition level information are needed.
GXEV( FormatCheck, 998)							///< Signals that quality assurance information are needed.
GXEV( DocumentDescriptor, 999)					///< %ECard table of contents data.
GXEV( B900, 1551)								///< B900 ink check.
GXEV( CompositeVizAuth, 2500)					///< Composite VIZ authentication.
GXEV( DullCheck, 2551)							///< Dull paper check.
GXEV( SecurityFibres, 2553)						///< Security fibres.
GXEV( COM, 0x4001f4)							///< %ECard COM file.
GXEV( DG1, COM + 1)								///< %ECard DataGroup 1.
GXEV( DG2, COM + 2)								///< %ECard DataGroup 2.
GXEV( DG3, COM + 3)								///< %ECard DataGroup 3.
GXEV( DG4, COM + 4)								///< %ECard DataGroup 4.
GXEV( DG5, COM + 5)								///< %ECard DataGroup 5.
GXEV( DG6, COM + 6)								///< %ECard DataGroup 6.
GXEV( DG7, COM + 7)								///< %ECard DataGroup 7.
GXEV( DG8, COM + 8)								///< %ECard DataGroup 8.
GXEV( DG9, COM + 9)								///< %ECard DataGroup 9.
GXEV( DG10, COM + 10)							///< %ECard DataGroup 10.
GXEV( DG11, COM + 11)							///< %ECard DataGroup 11.
GXEV( DG12, COM + 12)							///< %ECard DataGroup 12.
GXEV( DG13, COM + 13)							///< %ECard DataGroup 13.
GXEV( DG14, COM + 14)							///< %ECard DataGroup 14.
GXEV( DG15, COM + 15)							///< %ECard DataGroup 15.
GXEV( DG16, COM + 16)							///< %ECard DataGroup 16.
GXEV( CardAccess, 0x4001f4 + 28)				///< %ECard Card Access file.
GXEV( CardSecurity, CardAccess + 1)				///< %ECard Card Security file.
GXEV( CVCA, CardAccess + 2)						///< %ECard CVCA file.
GXEV( SOD, CardAccess + 3)						///< %ECard SOD file.
GXEV( CertDS, CardAccess + 4)					///< %Document Signer certificate file.
GXEV( CertCSCA, CardAccess + 5)					///< Country Signing CA certificate file.
GXEV( AuthPassive, 0x400226 + 1)				///< %ECard passive authentication.
GXEV( AuthTerminal, AuthPassive + 1)			///< %ECard terminal authentication.
GXEV( AuthPACE, AuthPassive + 2)				///< %ECard password authenticated connection establishment.
GXEV( AuthBAC, AuthPassive + 3)					///< %ECard basic access control authentication.
GXEV( AuthChip, AuthPassive + 4)				///< %ECard chip authentication.
GXEV( AuthActive, AuthPassive + 5)				///< %ECard active authentication.
GXEV( AuthSelectApp, AuthPassive + 6)			///< %ECard select application.
GXEV( AuthBAP, AuthPassive + 7)					///< %ECard basic access protection.
GXEV( AuthBlock, AuthPassive + 8)				///< %ECard block authentication.
GXEV( AuthInitTerminal, AuthPassive + 9)		///< %ECard initialize terminal authentication.
GXEV( AuthCompleteTerminal, AuthPassive + 10)	///< %ECard complete terminal authentication.
GXEV( AuthStorageCard, AuthPassive + 11)		///< %ECard storage card authentication.
GX_ENUM_END(FieldId)

} // namespace Processing
} // namespace Pr22
//------------------------------------------------------------------------------
#undef ENUMSTEP
#if PR22_FIELDID_INCL == 1
#undef PR22_FIELDID_INCL
#define PR22_FIELDID_INCL 2
#include "Processing/FieldId.hpp"
#undef PR22_FIELDID_INCL
#define PR22_FIELDID_INCL 3
#endif
