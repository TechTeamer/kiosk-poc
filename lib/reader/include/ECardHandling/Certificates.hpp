#ifndef PR22_CERTIFICATES_INCL
#define PR22_CERTIFICATES_INCL
/******************************************************************************/
/**	\file
 *	Certificate management.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Processing/BinData.hpp"
#include "ECardHandling/ECardHandling.hpp"
#include "Util/EnumInfo.hpp"
//------------------------------------------------------------------------------
namespace Pr22 { namespace ECardHandling {

/** Certificate manager class.
 *
 * The system is equipped with different certificate manager objects that are
 * designed to realize different ways of certificate usage. Certificates can
 * be used globally (for all readers of all devices), or specifically for one
 * DocumentReaderDevice or one ECardReader or one ECard.
 */
class Certificates {
public:
	#define ENUMSTEP 1
	/// Certificate selection filtering constants.
	GX_ENUM_BEGIN(Property)
	#include "ECardHandling/certprop.inl"
	GX_ENUM_END(Property)
	#undef ENUMSTEP

public:

	inline int Load(const Processing::BinData & publicKey);	// Loads a certificate to the manager.
	inline int Load(const Processing::BinData & publicKey, const Processing::BinData & privateKey);	// Loads a certificate to the manager.
	inline void LoadFromStore(const Util::InputString & storeName);		// Loads certificates from a certificate store.
	inline void Clear(const AuthProcess & authentication, const Property & certprop);	// Removes certificates from the manager.
	inline void Del(int id);								// Removes certificates from the manager.

private:

	PrIns::gxModule ApiHandle;			// Api handler.
	PrIns::CertScope scope;				// Manager scope.
	std::wstring owner;					// Manager owner.

	friend class Pr22::PrIns::Invoker;
	Certificates(const PrIns::gxModule & handle, const PrIns::CertScope & scope,
		const Util::InputString & owner) : ApiHandle(handle), scope(scope),
		owner(owner.GetString()) { }	// Constructor for internal usage only.
};

/******************************************************************************
 * This is the end of the Certificates interface. Now the implementation follows
 ******************************************************************************/

/** Loads a certificate to the manager.
 *
 * @param publicKey		The certificate to load.
 * @return				Id of the certificate.
 */
int Certificates::Load(const Processing::BinData & publicKey) {
	return Load(publicKey, Processing::BinData());
}

/** Loads a certificate and its private key to the manager.
 *
 * @param publicKey		The certificate to load.
 * @param privateKey	The private key to load.
 * @return				Id of the certificate.
 */
int Certificates::Load(const Processing::BinData & publicKey, const Processing::BinData & privateKey) {
	struct prLoadCertScope {
		int Scope;
		const wchar_t* Owner;
		const void* CertificateData;
		int CertificateLength;
		const void* PrivateKeyData;
		int PrivateKeyLength;
		int Id;
	} op = { scope, owner.c_str(), publicKey.RawData(), publicKey.RawSize(),
		privateKey.RawData(), privateKey.RawSize(), 0 };
	if(ApiHandle.Call(PrIns::ApiCallCodes::LoadCertScope, &op)) return -1;
	return op.Id;
}

/** Loads certificates from a certificate store.
 *
 * @param storeName		The name of the certificate store.
 */
void Certificates::LoadFromStore(const Util::InputString & storeName) {
	struct prLoadCertFromStore {
		const wchar_t* StoreName;
	} op = { storeName.GetString() };
	ApiHandle.Call(PrIns::ApiCallCodes::LoadCertFromStore, &op);
}

/** Removes certificates from the manager by authentication type and/or validity status.
 *
 * @param authentication	Certificate selection filter for authentications. Use value
 *							@ref AuthProcess::None for selecting all the authentications.
 * @param certprop			Certificate selection filter constants.
 */
void Certificates::Clear(const AuthProcess & authentication, const Property & certprop) {
	Del(authentication + certprop);
}

/** Removes certificate from the manager by Id.
 *
 * @param id		Id of a certificate returned by the Load method.
 */
void Certificates::Del(int id) {
	struct prClearCertListScope {
		int Sel;
		int Scope;
		const wchar_t* Owner;
	} op = { id, scope, owner.c_str() };
	ApiHandle.Call(PrIns::ApiCallCodes::ClearCertListScope, &op);
}

#define ENUMSTEP 2
GX_ENUM_BEGIN(Certificates::Property)
#include "ECardHandling/certprop.inl"
GX_ENUM_END(Certificates::Property)
#undef ENUMSTEP

} // namespace ECardHandling
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_CERTIFICATES_INCL
