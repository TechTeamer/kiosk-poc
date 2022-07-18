#ifndef PR22_VARIANT_INCL
#define PR22_VARIANT_INCL
/******************************************************************************/
/** \file
 * The Variant is a complex data structure
 *
 * The Variant object was introduced to easily form dynamically expandable
 * complex data structures that are usable in different programming languages
 * under various operational environments.
 * The Variant object is the basic unit of these complex data structures, and it
 * has the following important features:
 * \li Stores basic data types as well as arrays or strings.
 * \li Uses run-time type information for automatic data conversion on user request.
 * \li Stores Id or name that can be used to identify data in a complex structure.
 * \li Stores children. The variants can be connected together in a way that they
 * form a tree through parent-child relationships.
 * \li Uses reference counting to save memory and to form a closed tree.
 * \li It can be a list, in which the items are connected according to parent-children
 * hierarchy. The list and list items can have children too.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Exceptions/Exceptions.hpp"
#include "Util/Types.hpp"
#include <assert.h>
#include <vector>
//------------------------------------------------------------------------------
namespace Pr22 {

namespace PrIns {
	/// Empty structure type to map the variant type.
	struct GX_VARIANT;

	/// Defining the variant type as a pointer to an empty structure (no automatic typecasts).
	typedef struct GX_VARIANT* gxVARIANT;
}

namespace Util {

/// Container of Variant list item and child indexing parameters.
class VariantPos {
public:

	inline static VariantPos ByIndex(int ix);				// Select the ix-th item.
	inline static VariantPos ById(int id, int ix = 0);		// Search for the item with ordinal number of ix among items having the same Ids.
	inline static VariantPos ByName(const InputString &name, int ix = 0);	// Search for the item with ordinal number of ix among items having the same names.
	inline static VariantPos Last(void);					// Select the last item.
	inline VariantPos & Append(void);						// Inserting after the index position.

	/// Copy constructor
	VariantPos(const VariantPos &source) : flags(source.flags), ix(source.ix), id(source.id), name(0) {
		if(source.name) name = PrIns::wcsdup(source.name);
	}
	inline VariantPos & operator =(const VariantPos &other);

#if GX_RVALUE_REF || defined(GX_MANUAL)
	/// Move constructor
	VariantPos(VariantPos &&source) : flags(source.flags), ix(source.ix), id(source.id), name(0) {
		name = source.name; source.name = 0;
	}
	inline VariantPos & operator =(VariantPos &&other);
#endif

	~VariantPos() {
		if(name) free(name);
	}

private:

	friend class Variant;

	int flags;							// Indexing flag
	int ix;								// Index of the requested item
	int id;								// Id of the requested item
	wchar_t *name;						// Name of the requested item

#ifndef GX_MANUAL
	/// Flags for list item and child indexing functions.
	struct vpFlags {
		enum values {

			ByIndex			= 0x0002,	// Select the ix-th item.

			ById			= 0x0001,	// Search by Id.
			ByIndexAndId	= 0x0003,	// Search for an item with Id, starting from the ix-th item.
			ByOrderAndId	= 0x0004,	// Search for the ix-th item with the specified Id.

			ByName			= 0x0008,	// Search by name.
			ByIndexAndName	= 0x000a,	// Search for an item with name, starting from the ix-th item.
			ByOrderAndName	= 0x0020,	// Search for the ix-th item with the specified name.

			Append			= 0x0100,	// Appending after the index position instead of inserting.
			Last			= 0x0200,	// Select the last item.
		};
	};
#endif

	VariantPos(int flags, int ix, int id) : flags(flags), ix(ix), id(id), name(0) { }
};

/** Class for variant data management.
 * The Variant class stores references to the variant data objects.
 */
class Variant {
public:

	class Types;

	/// Value used for constructing a NULL type variant.
	enum NullT { Null };

	/// Value used for constructing a list type variant.
	enum ListT { List };

	/// @name General variant functionality
	/// @{
	inline Variant();										// Default constructor.
	inline ~Variant();										// Destructor.
	inline Variant& Leave(void);							// Finishes working with the actual variant object.
	inline bool IsValid() const;							// Returns if the variant is valid or empty.
	inline int ID(void) const;								// Returns the Id of the variant.
	inline Variant& SetID(int id);							// Sets the Id of the variant.
	inline PrOutputString Name(void) const;					// Returns the name of the variant.
	inline Variant& SetName(const InputString &name);		// Sets the name of the variant.
	inline Types Type(void) const;							// Returns the type of the variant.
	inline int Size(void) const;							// Returns the item size of the variant.
	inline int NItems(void) const;							// Returns the number of items in the variant.
	/// @}

	/// @name Copying variants
	/// @{
	inline Variant(const Variant& source);					// Copy constructor.
	inline Variant& operator = (const Variant& source);		// Assignment operator.
#if GX_RVALUE_REF || defined(GX_MANUAL)
	inline Variant(Variant&& source);						// Move constructor.
	inline Variant& operator = (Variant&& source);			// Move assignment operator.
#endif
	inline Variant& Ref(const Variant& source);				// References an existing variant.
	inline explicit Variant(PrIns::gxVARIANT source);		// Copy constructor for variant structure.
	inline operator PrIns::gxVARIANT & ();					// Conversion operator to gxVARIANT.
	inline operator const PrIns::gxVARIANT & () const;		// Conversion operator to gxVARIANT.
	inline Variant Duplicate(void) const;					// Duplicates, copies the variant and all of its descendants.
	inline Variant operator ~ () const;						// This operator is a shortcut to the Duplicate() method.
	/// @}

#ifndef GX_MANUAL

	/* Nulldata functions */

	inline Variant(int id, NullT);							// Constructor, defines a NULL type variant object.
	inline Variant(const InputString &name, NullT);			// Constructor, defines a NULL type variant object.
	inline Variant& Create(int id, const InputString &name);	// Creates a NULL type variant structure.
	inline Variant& Clear(void);							// Modifies an existing variant to NULL type.

	/* gxi32 functions */

	inline Variant(int id, gxi32 val);						// Constructor, defines an integer type variant object.
	inline Variant(const InputString &name, gxi32 val);		// Constructor, defines an integer type variant object.
	inline Variant& operator = (gxi32 val);					// Assignment operator, updates the data of the variant.
	inline operator int() const;							// Conversion operator to integer.
	inline Variant& Create(int id, const InputString &name, gxi32 data);	// Creates an integer type variant structure.
	inline Variant& Update(gxi32 data);						// Modifies the type and the value of an existing variant.
	inline int ToInt(void) const;							// Returns the value of the variant converted to integer.

	/* gxi64 functions */

	inline Variant(int id, gxi64 val);						// Constructor, defines an integer type variant object.
	inline Variant(const InputString &name, gxi64 val);		// Constructor, defines an integer type variant object.
	inline Variant& operator = (gxi64 val);					// Assignment operator, updates the data of the variant.
	inline operator gxi64() const;							// Conversion operator to integer.
	inline Variant& Create(int id, const InputString &name, gxi64 data);	// Creates an integer type variant structure.
	inline Variant& Update(gxi64 data);						// Modifies the type and the value of an existing variant.
	inline gxi64 ToLong(void) const;						// Returns the value of the variant converted to integer.

	/* gxu32 functions */

	inline Variant(int id, gxu32 val);						// Constructor, defines an unsigned integer type variant object.
	inline Variant(const InputString &name, gxu32 val);		// Constructor, defines an unsigned integer type variant object.
	inline Variant& operator = (gxu32 val);					// Assignment operator, updates the data of the variant.
	inline operator unsigned int() const;					// Conversion operator to unsigned integer.
	inline Variant& Create(int id, const InputString &name, gxu32 data);	// Creates an unsigned integer type variant structure.
	inline Variant& Update(gxu32 data);						// Modifies the type and the value of an existing variant.
	inline unsigned int ToUInt(void) const;					// Returns the value of the variant converted to unsigned integer.

	/* gxu64 functions */

	inline Variant(int id, gxu64 val);						// Constructor, defines an unsigned integer type variant object.
	inline Variant(const InputString &name, gxu64 val);		// Constructor, defines an unsigned integer type variant object.
	inline Variant& operator = (gxu64 val);					// Assignment operator, updates the data of the variant.
	inline operator gxu64() const;							// Conversion operator to unsigned integer. */
	inline Variant& Create(int id, const InputString &name, gxu64 data);	// Creates an unsigned integer type variant structure.
	inline Variant& Update(gxu64 data);						// Modifies the type and the value of an existing variant.
	inline gxu64 ToULong(void) const;						// Returns the value of the variant converted to unsigned integer.

	/* gxf32 functions */

	inline Variant(int id, gxf32 val);						// Constructor, defines a floating point type variant object.
	inline Variant(const InputString &name, gxf32 val);		// Constructor, defines a floating point type variant object.
	inline Variant& operator = (gxf32 val);					// Assignment operator, updates the data of the variant.
	inline operator float() const;							// Conversion operator to floating point.
	inline Variant& Create(int id, const InputString &name, gxf32 data);	// Creates a floating point type variant structure.
	inline Variant& Update(gxf32 data);						// Modifies the type and the value of an existing variant.
	inline float ToFloat(void) const;						// Returns the value of the variant converted to floating point.

	/* gxf64 functions */

	inline Variant(int id, gxf64 val);						// Constructor, defines a floating point type variant object.
	inline Variant(const InputString &name, gxf64 val);		// Constructor, defines a floating point type variant object.
	inline Variant& operator = (gxf64 val);					// Assignment operator, updates the data of the variant.
	inline operator double() const;							// Conversion operator to floating point.
	inline Variant& Create(int id, const InputString &name, gxf64 data);	// Creates a floating point type variant structure.
	inline Variant& Update(gxf64 data);						// Modifies the type and the value of an existing variant.
	inline double ToDouble(void) const;						// Returns the value of the variant converted to floating point.

	/* ascii string functions */

	inline Variant(int id, const char * val);					// Constructor, defines a string type variant object.
	inline Variant(const InputString &name, const char * val);	// Constructor, defines a string type variant object.
	inline Variant& operator = (const char * val);				// Assignment operator, updates the data of the variant.
	inline operator std::string() const;						// Conversion operator to string.
	inline Variant& Create(int id, const InputString &name, const char *data);	// Creates a string type variant structure.
	inline Variant& Update(const char *data);					// Modifies the type and the value of an existing variant.
	inline std::string ToAscii(void) const;						// Returns the value of the variant converted to string.

	/* unicode string functions */

	inline Variant(int id, const wchar_t * val);					// Constructor, defines a string type variant object.
	inline Variant(const InputString &name, const wchar_t * val);	// Constructor, defines a string type variant object.
	inline Variant& operator = (const wchar_t * val);				// Assignment operator, updates the data of the variant.
	inline operator std::wstring() const;							// Conversion operator to string.
	inline Variant& Create(int id, const InputString &name, const wchar_t *data);	// Creates a string type variant structure.
	inline Variant& Update(const wchar_t *data);					// Modifies the type and the value of an existing variant.
	inline std::wstring ToUnicode(void) const;						// Returns the value of the variant converted to string.
	inline PrOutputString ToString() const;							// Returns the value of the variant converted to string.

	/* integer array functions */

	inline Variant(int id, std::vector<int> &val);					// Constructor, defines an array type variant object.
	inline Variant(const InputString &name, std::vector<int> &val);	// Constructor, defines an array type variant object.
	inline Variant& operator = (std::vector<int> &val);				// Assignment operator, updates the data of the variant.
	inline operator std::vector<int>() const;						// Conversion operator to array.
	inline Variant& CreateArray(int id, const InputString &name, const gxi32 *varray, int nitems);	// Creates an array type variant structure.
	inline Variant& UpdateArray(const gxi32 *varray, int nitems);	// Modifies the type and the value of an existing variant.
	inline std::vector<int> ToIntArray(void) const;					// Returns the value of the variant converted to array.

	/* byte array functions */

	inline Variant(int id, std::vector<gxu8> &val);					// Constructor, defines an array type variant object.
	inline Variant(const InputString &name, std::vector<gxu8> &val);	// Constructor, defines an array type variant object.
	inline Variant& operator = (std::vector<gxu8> &val);			// Assignment operator, updates the data of the variant.
	inline operator std::vector<gxu8>() const;						// Conversion operator to array.
	inline Variant& CreateArray(int id, const InputString &name, const gxu8 *varray, int nitems);	// Creates an array type variant structure.
	inline Variant& UpdateArray(const gxu8 *varray, int nitems);	// Modifies the type and the value of an existing variant.
	inline std::vector<gxu8> ToByteArray(void) const;				// Returns the value of the variant converted to array.

	/* frame functions */

	inline Variant(int id, const Frame& val);					// Constructor, defines a frame type variant object.
	inline Variant(const InputString &name, const Frame& val);	// Constructor, defines a frame type variant object.
	inline Variant& operator = (const Frame& val);				// Assignment operator, updates the data of the variant.
	inline operator Frame() const;								// Conversion operator to frame.
	inline Variant& Create(int id, const InputString &name, const Frame& frame);	// Creates a frame type variant structure.
	inline Variant& Update(const Frame& frame);					// Modifies the type and the value of an existing variant.
	inline Frame ToFrame(void) const;							// Returns the value of the variant converted to frame.

	/* Variant list functions */

	inline Variant(int id, ListT);							// Constructor, defines a list type variant object.
	inline Variant(const InputString &name, ListT);			// Constructor, defines a list type variant object.
#else
	inline Variant(int id, type val);
	inline Variant(const InputString &name, type val);
	inline Variant& operator = (type val);
	inline operator type() const;
	inline Variant& Create(int id, const InputString &name);
	inline Variant& Create(int id, const InputString &name, type data);
	inline Variant& CreateArray(int id, const InputString &name, const type *varray, int nitems);
	inline Variant& Clear(void);
	inline Variant& Update(type data);
	inline Variant& UpdateArray(const type *varray, int nitems);
	inline int ToInt(void) const;
	inline gxi64 ToLong(void) const;
	inline unsigned int ToUInt(void) const;
	inline gxu64 ToULong(void) const;
	inline float ToFloat(void) const;
	inline double ToDouble(void) const;
	inline std::string ToAscii(void) const;
	inline std::wstring ToUnicode(void) const;
	inline PrOutputString ToString() const;
	inline std::vector<int> ToIntArray(void) const;
	inline std::vector<gxu8> ToByteArray(void) const;
	inline Frame ToFrame(void) const;
#endif

	/// @name List functions
	/// For list functionality see the constructors Variant(int id, type val),
	/// Variant(const InputString &name, type val) and the NItems() method too.
	/// @{
	inline Variant& operator << (const Variant& var);				// Appends an item to the list type variant.
	inline Variant operator [] (int ix) const;						// Returns an item of the list type variant.
	inline Variant& CreateList(int id, const InputString &name);	// Creates a list type variant structure.
	inline Variant& ChangeToList(void);								// Modifies an existing variant to list type.
	inline Variant& AddListItem(const Variant& item,
		const VariantPos& position = VariantPos::Last());			// Adds an item to the list type variant.
	inline Variant GetListItem(int ix) const;						// Returns an item of the list type variant.
	inline Variant GetListItem(const VariantPos& position) const;	// Returns an item of the list type variant.
	/// @}

	/// @name Child functionality
	/// @{
	inline Variant operator () (int id, int ix = 0);				// Returns a child of a variant.
	inline Variant operator () (const InputString &name, int ix = 0);	// Returns a child of a variant.
	inline int NChildren(void) const;								// Returns the number of children in the variant.
	inline Variant& AddChild(const Variant& item,
		const VariantPos& position = VariantPos::Last());			// Adds a child element to the variant.
	inline Variant GetChildByIndex(int ix) const;					// Returns a child of a variant.
	inline Variant GetChild(int id, int ix) const;					// Returns a child of a variant.
	inline Variant GetChild(const InputString &name, int ix) const;	// Returns a child of a variant.
	inline Variant GetChild(const VariantPos& position) const;		// Returns a child of a variant.
	/// @}

	/// @name Other functions
	/// @{
	inline Variant& Create(int id, const InputString &name, Types type,
		int size, const void *data, int nitems = 1);				// Creates a variant structure.
	inline Variant& Update(Types type, int size, const void *data, int nitems = 1);	// Modifies the type and the value of an existing variant.
	inline int GetData(Types type, void *target, int tgtlen,
		int size = 0, int nitems = 0) const;						// Returns the value of the variant converted to a specified type.
	inline Variant GetItemByPath(const InputString &path) const;	// Returns a variant from a tree.
	/// @}

private:

	// Calls the gx system to do some variant functionality
	inline static int Call(int cmd, void* params);

public:
	PrIns::gxVARIANT	_variant;		///< Internal variable to store object data.

	#define ENUMSTEP 1
	/** Available variant types.
	 * \note The values with \e \b _AC_ tag are usable for creation only. For
	 * these types the value is automatically converted to the proper data type.
	 * \note The values with \e \b _AS_ tag are usable for creation only. For
	 * these types the value is automatically converted to the proper string type.
	 */
	GX_ENUM_BEGIN(Types)
	#include "Util/vartypes.inl"
	GX_ENUM_END(Types)
	#undef ENUMSTEP
};
} // namespace Util
/******************************************************************************
 * This is the end of the Variant interface. Now the implementation follows   *
 ******************************************************************************/

#ifndef GX_MANUAL
namespace PrIns {

/* GX call codes */
struct VariantCallCodes {
enum codes {
	CallCodes	= 0x10040000,

	Ref			= CallCodes,
	Unref		= CallCodes+1,
	Duplicate	= CallCodes+4,

	Convert		= CallCodes+8,

	ChangeId	= CallCodes+29,
	Create		= CallCodes+30,
	Update		= CallCodes+31,
	UpdateData	= CallCodes+32,
	ChangeName	= CallCodes+33,
	GetInfo		= CallCodes+34,
	GetItem		= CallCodes+35,
	CutItem		= CallCodes+36,
	FindItem	= CallCodes+37,
	AddItem		= CallCodes+38,
	GetChild	= CallCodes+39,
	CutChild	= CallCodes+40,
	FindChild	= CallCodes+41,
	AddChild	= CallCodes+42,
	GetValue	= CallCodes+19,
	CutValue	= CallCodes+20,
	AddValue	= CallCodes+22,
	GetByPath	= CallCodes+24,
	ConvertByPath = CallCodes+26,
};
};

/* GX call parameter structures. */

struct VariantUpdate {
	gxVARIANT		 variant;
	int				 id;
	const wchar_t	*name;
	int				 type;
	int				 size;
	int				 nitems;
	const void		*data;
};

struct VariantGetInfo {
	gxVARIANT		 variant;
	int				 id;
	wchar_t			*namebuff;
	int				 namebuffsize;
	int				 type;
	int				 size;
	int				 nitems;
	int				 nchildren;
};

struct VariantItem {
	gxVARIANT		 parent;
	int				 flags;
	int				 id;
	int				 ix;
	const wchar_t	*name;
	gxVARIANT		 item;
};

} // namespace PrIns
#endif /* GX_MANUAL */
//------------------------------------------------------------------------------

namespace Util {

/// Assignment operator
VariantPos & VariantPos::operator =(const VariantPos &other) {
	if(this!=&other) {
		if(name) free(name);
		memcpy(this, &other, sizeof(VariantPos));
		if(name) name = PrIns::wcsdup(name);
	}
	return *this;
}

#if GX_RVALUE_REF || defined(GX_MANUAL)
/// Move assignment operator
VariantPos & VariantPos::operator =(VariantPos &&other) {
	if(name) free(name);
	memcpy(this, &other, sizeof(VariantPos));
	other.name = 0;
	return *this;
}
#endif

/** Select the item with ordinal number of ix.
 * @param ix	The index.
 */
VariantPos VariantPos::ByIndex(int ix) {
	return VariantPos(vpFlags::ByIndex, ix, 0);
}

/** Search for the item with ordinal number of ix among items having the same Ids.
 * @param id	The Id.
 * @param ix	The index.
 */
VariantPos VariantPos::ById(int id, int ix /* 0 */) {
	return VariantPos(vpFlags::ByOrderAndId, ix, id);
}

/** Search for the item with ordinal number of ix among items having the same names.
 * @param name	The name.
 * @param ix	The index.
 */
VariantPos VariantPos::ByName(const InputString &name, int ix /* 0 */) {
	VariantPos vp(vpFlags::ByOrderAndName, ix, 0);
	if(name.GetString()) vp.name = PrIns::wcsdup(name.GetString());
	return vp;
}

/// Select the last item.
VariantPos VariantPos::Last(void) {
	return VariantPos(vpFlags::Last, 0, 0);
}

/// Inserting after the index position.
VariantPos & VariantPos::Append(void) {
	flags |= vpFlags::Append;
	return *this;
}

//------------------------------------------------------------------------------

/** Default constructor, defines an empty variant object.
 *
 * \note Empty variants cannot be modified without using a Create method or an
 * assignment operator.
 */
Variant::Variant() : _variant(0) { }

/// Destructor.
Variant::~Variant() {
	try {
		Leave();
	}
	catch(...) { }
}

/** Finishes working with the actual variant object.
 *
 * The object became an empty variant object.
 *
 * \note Empty variants cannot be modified without using a Create method or an
 * assignment operator.
 *
 * @return			The variant object.
 */
Variant& Variant::Leave(void) {
	if(_variant) {
		int err = Call(PrIns::VariantCallCodes::Unref, _variant);
		assert(!err);
		(void) err;
	}
	_variant = 0;
	return *this;
}

/** Returns if the variant is valid or empty.
 *
 * return			True if the variant is valid or false if it is empty.
 */
bool Variant::IsValid() const {
	return _variant != 0;
}

/** Returns the Id of the variant.
 *
 * @return			The Id of the variant.
 */
int Variant::ID(void) const {
	try {
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = 0;
		gvi.namebuffsize = 0;
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return -1;
		return gvi.id;
	}
	catch(const Exceptions::General &) {
		return -1;
	}
}

/** Sets the Id of the variant.
 *
 * @param id		The new Id of the variant.
 * @return			The variant object.
 */
Variant& Variant::SetID(int id) {
	PrIns::VariantUpdate cv;
	cv.variant = _variant;
	cv.id = id;
	Call(PrIns::VariantCallCodes::ChangeId, &cv);
	return *this;
}

/** Returns the name of the variant.
 *
 * @return			The name of the variant.
 */
PrOutputString Variant::Name(void) const {
	try {
		wchar_t namebuff[256];
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = namebuff;
		gvi.namebuffsize = gx_arraycnt(namebuff);
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return PrOutputString();
		return ToPrOutputString(namebuff);
	}
	catch(const Exceptions::General &) {
		return PrOutputString();
	}
}

/** Sets the name of the variant.
 *
 * @param name		The new name of the variant.
 * @return			The variant object.
 */
Variant& Variant::SetName(const InputString &name) {
	PrIns::VariantUpdate cv;
	cv.variant = _variant;
	cv.name = name.GetString();
	Call(PrIns::VariantCallCodes::ChangeName, &cv);
	return *this;
}

/** Returns the type of the variant.
 *
 * @return			The type of the variant.
 */
Variant::Types Variant::Type(void) const {
	try{
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = 0;
		gvi.namebuffsize = 0;
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return (Types)-1;
		return (Types)gvi.type;
	}
	catch(const Exceptions::General &) {
		return Types::Null;
	}
}

/** Returns the item size of the variant.
 *
 * @return			The item size of the variant.
 */
int Variant::Size(void) const {
	try {
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = 0;
		gvi.namebuffsize = 0;
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return -1;
		return gvi.size;
	}
	catch(const Exceptions::General &) {
		return 0;
	}
}

/** Returns the number of items in the variant.
 *
 * This value is useful for array and list type variants.
 *
 * @return			The number of items in the variant.
 */
int Variant::NItems(void) const {
	try {
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = 0;
		gvi.namebuffsize = 0;
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return -1;
		return gvi.nitems;
	}
	catch(const Exceptions::General &) {
		return 0;
	}
}

/** Copy constructor, references an existing variant.
 *
 * @param source	Source variant.
 */
Variant::Variant(const Variant& source) : _variant(0) {
	Ref(source);
}

/** Assignment operator, references an existing variant.
 *
 * @param source	Source variant.
 * @return			The variant object.
 */
Variant& Variant::operator = (const Variant& source) {
	return Ref(source);
}

#if GX_RVALUE_REF || defined(GX_MANUAL)
/// Move constructor, references an existing variant.
Variant::Variant(Variant&& source) : _variant(0) {
	_variant = source._variant;
	source._variant = 0;
}

/// Move assignment operator, references an existing variant.
Variant& Variant::operator = (Variant&& source) {
	if(_variant==source._variant) return *this;

	if(_variant) {
		int err = Call(PrIns::VariantCallCodes::Unref, _variant);
		assert(!err);
		(void) err;
	}

	_variant = source._variant;
	source._variant = 0;
	return *this;
}
#endif

/** References an existing variant.
 *
 * @param source	Source variant.
 * @return			The variant object.
 */
Variant& Variant::Ref(const Variant& source) {
	if(_variant==source._variant) return *this;

	if(source._variant && Call(PrIns::VariantCallCodes::Ref, source._variant))
		return *this;

	PrIns::gxVARIANT tmp = _variant;
	_variant = source._variant;

	if(tmp) {
		int err = Call(PrIns::VariantCallCodes::Unref, tmp);
		assert(!err);
		(void) err;
	}
	return *this;
}

/** Copy constructor for variant structure.
 *
 * \note This constructor is for internal usage only.
 *
 * @param source	Source variant.
 */
Variant::Variant(PrIns::gxVARIANT source) : _variant(source) { }

/** Conversion operator to gxVARIANT.
 *
 * \note This operator is for internal usage only.
 *
 * @return			The internally stored gxVARIANT data.
 */
Variant::operator PrIns::gxVARIANT & () {
	return _variant;
}

/** Conversion operator to gxVARIANT.
 *
 * \note This operator is for internal usage only.
 *
 * @return			The internally stored gxVARIANT data.
 */
Variant::operator const PrIns::gxVARIANT & () const {
	return _variant;
}

/** Duplicates, copies the variant and all of its descendants.
 *
 * The Variant class stores references to variant data objects.
 * Modifying the data structure of a variant affects all the variables that references the
 * same memory area. In order to avoid this behaviour the variant can be duplicated
 * before modification.
 *
 * @return			The variant object references the copy of this variant.
 */
Variant Variant::Duplicate(void) const {
	Variant var;
	struct DuplicateVariant {
		PrIns::gxVARIANT	 source;
		PrIns::gxVARIANT	 target;
	} dv;
	dv.source = _variant;
	if(!var.Call(PrIns::VariantCallCodes::Duplicate, &dv)) var._variant = dv.target;
	return var;
}

/** This operator is a shortcut to the Duplicate() method.
 *
 * The Variant class stores references to variant data objects.
 * Modifying the data structure of a variant affects all the variables that references the
 * same memory area. In order to avoid this behaviour the variant can be duplicated
 * before modification.
 *
 * @return			The variant object references the copy of this variant.
 */
Variant Variant::operator ~ () const {
	return Duplicate();
}

/** @name Data functions
 * @{
 */

#ifdef GX_MANUAL
/** Constructor, defines a typed variant object.
 *
 * @param id		The Id of the variant.
 * @param val		The value of the variant. The currently handled types of the value are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c const \c char \c * \li \c const \c wchar_t \c *
 *					\li \c std::vector<int> \c & \li \c std::vector<gxu8> \c &
 *					\li \c const \c Frame \c &
 *
 *					or the value can be: \li \c Null to designate a NULL type variant
 *					\li \c List to designate a list type variant
 */
Variant::Variant(int id, type val);
#else
Variant::Variant(int id, NullT) : _variant(0) {
	Create(id, (wchar_t*)0);
}
Variant::Variant(int id, gxi32 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, gxi64 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, gxu32 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, gxu64 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, gxf32 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, gxf64 val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, const char * val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, const wchar_t * val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, std::vector<int> &val) : _variant(0) {
	CreateArray(id, (wchar_t*)0, (int*)PrIns::vector_data(val), (int)val.size());
}
Variant::Variant(int id, std::vector<gxu8> &val) : _variant(0) {
	CreateArray(id, (wchar_t*)0, (gxu8*)PrIns::vector_data(val), (int)val.size());
}
Variant::Variant(int id, const Frame& val) : _variant(0) {
	Create(id, (wchar_t*)0, val);
}
Variant::Variant(int id, ListT) : _variant(0) {
	CreateList(id, (wchar_t*)0);
}
#endif

#ifdef GX_MANUAL
/** Constructor, defines a typed variant object.
 *
 * @param name		The name of the variant.
 * @param val		The value of the variant. The currently handled types of the value are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c const \c char \c * \li \c const \c wchar_t \c *
 *					\li \c std::vector<int> \c & \li \c std::vector<gxu8> \c &
 *					\li \c const \c Frame \c &
 *
 *					or the value can be: \li \c Null to designate a NULL type variant
 *					\li \c List to designate a list type variant
 */
Variant::Variant(const InputString &name, type val);
#else
Variant::Variant(const InputString &name, NullT) : _variant(0) {
	Create(0, name);
}
Variant::Variant(const InputString &name, gxi32 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, gxi64 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, gxu32 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, gxu64 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, gxf32 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, gxf64 val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, const char * val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, const wchar_t * val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, std::vector<int> &val) : _variant(0) {
	CreateArray(0, name, (int*)PrIns::vector_data(val), (int)val.size());
}
Variant::Variant(const InputString &name, std::vector<gxu8> &val) : _variant(0) {
	CreateArray(0, name, (gxu8*)PrIns::vector_data(val), (int)val.size());
}
Variant::Variant(const InputString &name, const Frame& val) : _variant(0) {
	Create(0, name, val);
}
Variant::Variant(const InputString &name, ListT) : _variant(0) {
	CreateList(0, name);
}
#endif

#ifdef GX_MANUAL
/** Assignment operator, updates the data of the variant.
 *
 * @param val		The new value of the variant. The currently handled types of the value are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c const \c char \c * \li \c const \c wchar_t \c *
 *					\li \c std::vector<int> \c & \li \c std::vector<gxu8> \c &
 *					\li \c Frame \c &
 * @return			The variant object.
 */
Variant& Variant::operator = (type val);
#else
Variant& Variant::operator = (gxi32 val) {
	return Update(val);
}
Variant& Variant::operator = (gxi64 val) {
	return Update(val);
}
Variant& Variant::operator = (gxu32 val) {
	return Update(val);
}
Variant& Variant::operator = (gxu64 val) {
	return Update(val);
}
Variant& Variant::operator = (gxf32 val) {
	return Update(val);
}
Variant& Variant::operator = (gxf64 val) {
	return Update(val);
}
Variant& Variant::operator = (const char * val) {
	return Update(val);
}
Variant& Variant::operator = (const wchar_t * val) {
	return Update(val);
}
Variant& Variant::operator = (std::vector<int> &val) {
	return UpdateArray((int*)PrIns::vector_data(val), (int)val.size());
}
Variant& Variant::operator = (std::vector<gxu8> &val) {
	return UpdateArray((gxu8*)PrIns::vector_data(val), (int)val.size());
}
Variant& Variant::operator = (const Frame& val) {
	return Update(val);
}
#endif

#ifdef GX_MANUAL
/** Conversion operator to type.
 *
 * @return			The value of the variant converted to type. The currently handled types are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c std::string \li \c std::wstring
 *					\li \c std::vector<int> \li \c std::vector<gxu8>
 *					\li \c Frame
 */
Variant::operator type() const;
#else
Variant::operator int() const {
	return ToInt();
}
Variant::operator gxi64() const {
	return ToLong();
}
Variant::operator unsigned int() const {
	return ToUInt();
}
Variant::operator gxu64() const {
	return ToULong();
}
Variant::operator float() const {
	return ToFloat();
}
Variant::operator double() const {
	return ToDouble();
}
Variant::operator std::string() const {
	return ToAscii();
}
Variant::operator std::wstring() const {
	return ToUnicode();
}
Variant::operator std::vector<int>() const {
	return ToIntArray();
}
Variant::operator std::vector<gxu8>() const {
	return ToByteArray();
}
Variant::operator Frame() const {
	return ToFrame();
}
#endif

/** Creates a NULL type variant structure.
 *
 * @param id		The Id of the variant.
 * @param name		The name of the variant.
 * @return			The variant object.
 */
Variant& Variant::Create(int id, const InputString &name) {
	return Create(id, name, Types::Null, 0, 0, 0);
}

#ifdef GX_MANUAL
/** Creates a typed variant structure.
 *
 * @param id		The Id of the variant.
 * @param name		The name of the variant.
 * @param data		The value of the variant. The currently handled types of the value are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c const \c char \c * \li \c const \c wchar_t \c *
 *					\li \c Frame \c &
 * @return			The variant object.
 */
Variant& Variant::Create(int id, const InputString &name, type data);
#else
Variant& Variant::Create(int id, const InputString &name, gxi32 data) {
	return Create(id, name, Types::Int, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, gxi64 data) {
	return Create(id, name, Types::Int, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, gxu32 data) {
	return Create(id, name, Types::Uint, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, gxu64 data) {
	return Create(id, name, Types::Uint, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, gxf32 data) {
	return Create(id, name, Types::Float, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, gxf64 data) {
	return Create(id, name, Types::Float, sizeof(data), &data);
}
Variant& Variant::Create(int id, const InputString &name, const char *data) {
	return Create(id, name, Types::Ascii, (data ? (int)strlen(data)+1 : 0), data);
}
Variant& Variant::Create(int id, const InputString &name, const wchar_t *data) {
	return Create(id, name, Types::Unicode, (data ? (int)wcslen(data)+1 : 0), data);
}
Variant& Variant::Create(int id, const InputString &name, const Frame& frame) {
	return Create(id, name, Types::IntArray, sizeof(int), &frame, 8);
}
#endif

#ifdef GX_MANUAL
/** Creates a typed variant array structure.
 *
 * @param id		The Id of the variant array.
 * @param name		The name of the variant array.
 * @param varray	Pointer to the array. The currently handled types of the values are:
 *					\li \c gxi32 \li \c gxu8
 * @param nitems	Number of items in the array.
 * @return			The variant object.
 */
Variant& Variant::CreateArray(int id, const InputString &name, const type *varray, int nitems);
#else
Variant& Variant::CreateArray(int id, const InputString &name, const gxi32 *varray, int nitems) {
	return Create(id, name, Types::IntArray, sizeof(gxi32), varray, nitems);
}
Variant& Variant::CreateArray(int id, const InputString &name, const gxu8 *varray, int nitems) {
	return Create(id, name, Types::UintArray, sizeof(gxu8), varray, nitems);
}
#endif

/** Modifies an existing variant to NULL type.
 *
 * @return			The variant object.
 */
Variant& Variant::Clear(void) {
	return Update(Types::Null, 0, 0, 0);
}

#ifdef GX_MANUAL
/** Modifies the type and the value of an existing variant.
 *
 * @param data		The value of the variant. The currently handled types of the data are:
 *					\li \c gxi32 \li \c gxi64 \li \c gxu32 \li \c gxu64 \li \c gxf32 \li \c gxf64
 *					\li \c const \c char \c * \li \c const \c wchar_t \c *
 *					\li \c Frame \c &
 * @return			The variant object.
 */
Variant& Variant::Update(type data);
#else
Variant& Variant::Update(gxi32 data) {
	return Update(Types::Int, sizeof(data), &data);
}
Variant& Variant::Update(gxi64 data) {
	return Update(Types::Int, sizeof(data), &data);
}
Variant& Variant::Update(gxu32 data) {
	return Update(Types::Uint, sizeof(data), &data);
}
Variant& Variant::Update(gxu64 data) {
	return Update(Types::Uint, sizeof(data), &data);
}
Variant& Variant::Update(gxf32 data) {
	return Update(Types::Float, sizeof(data), &data);
}
Variant& Variant::Update(gxf64 data) {
	return Update(Types::Float, sizeof(data), &data);
}
Variant& Variant::Update(const char *data) {
	return Update(Types::Ascii, (data ? (int)strlen(data)+1 : 0), data);
}
Variant& Variant::Update(const wchar_t *data) {
	return Update(Types::Unicode, (data ? (int)wcslen(data)+1 : 0), data);
}
Variant& Variant::Update(const Frame& frame) {
	return Update(Types::IntArray, sizeof(int), &frame, 8);
}
#endif

#ifdef GX_MANUAL
/** Modifies the type and the value of an existing variant.
 *
 * @param varray	Pointer to the array. The currently handled types of the values are:
 *					\li \c gxi32 \li \c gxu8
 * @param nitems	Number of items in the array.
 * @return			The variant object.
 */
Variant& Variant::UpdateArray(const type *varray, int nitems);
#else
Variant& Variant::UpdateArray(const gxi32 *varray, int nitems) {
	return Update(Types::IntArray, sizeof(gxi32), varray, nitems);
}
Variant& Variant::UpdateArray(const gxu8 *varray, int nitems) {
	return Update(Types::UintArray, sizeof(gxu8), varray, nitems);
}
#endif

/** Returns the value of the variant converted to integer.
 *
 * @return			The value of the variant converted to integer.
 */
int Variant::ToInt(void) const {
	int val;
	if(GetData(Types::Int, &val, sizeof(val))) return 0;
	return val;
}

/** Returns the value of the variant converted to integer.
 *
 * @return			The value of the variant converted to integer.
 */
gxi64 Variant::ToLong(void) const {
	gxi64 val;
	if(GetData(Types::Int, &val, sizeof(val))) return 0;
	return val;
}

/** Returns the value of the variant converted to unsigned integer.
 *
 * @return			The value of the variant converted to unsigned integer.
 */
unsigned int Variant::ToUInt(void) const {
	unsigned int val = 0;
	if(GetData(Types::Uint, &val, sizeof(val))) return 0;
	return val;
}

/** Returns the value of the variant converted to unsigned integer.
 *
 * @return			The value of the variant converted to unsigned integer.
 */
gxu64 Variant::ToULong(void) const {
	gxu64 val;
	if(GetData(Types::Uint, &val, sizeof(val))) return 0;
	return val;
}

/** Returns the value of the variant converted to floating point.
 *
 * @return			The value of the variant converted to floating point.
 */
float Variant::ToFloat(void) const {
	float val;
	if(GetData(Types::Float, &val, sizeof(val))) return 0;
	return val;
}

/** Returns the value of the variant converted to floating point.
 *
 * @return			The value of the variant converted to floating point.
 */
double Variant::ToDouble(void) const {
	double val;
	if(GetData(Types::Float, &val, sizeof(val))) return 0;
	return val;
}

#ifndef GX_MANUAL

template<class CHAR>
CHAR * VarToStr(const Variant *var, CHAR*) {
	typedef Variant::Types Types;
	Types type = var->Type();
	int size = type==Types::Ascii || type==Types::Unicode ? var->Size() : 64;
	if(type<0 || size<=0) {
		throw Exceptions::InvalidParameter(L"[pr22] (size)");
	}
	CHAR *tmp = new CHAR[(size_t)size];
	if(!tmp) return 0;
	try {
		if(var->GetData((sizeof(CHAR)==1?Types::Ascii:Types::Unicode), tmp, size*(int)sizeof(CHAR))) {
			delete [] tmp;
			return 0;
		}
	}
	catch(const Exceptions::General &) {
		delete [] tmp;
		throw;
	}
	return tmp;
}
#endif

/** Returns the value of the variant converted to string.
 *
 * @return			The value of the variant converted to string.
 */
std::string Variant::ToAscii(void) const {
	char *tmp = VarToStr(this, (char*)0);
	if(!tmp) return std::string();
	std::string ts(tmp);
	delete [] tmp;
	return ts;
}

/** Returns the value of the variant converted to string.
 *
 * @return			The value of the variant converted to string.
 */
std::wstring Variant::ToUnicode(void) const {
	wchar_t *tmp = VarToStr(this, (wchar_t*)0);
	if(!tmp) return std::wstring();
	std::wstring ts(tmp);
	delete [] tmp;
	return ts;
}

/** Returns the value of the variant converted to string.
 *
 * @return			The value of the variant converted to string.
 */
PrOutputString Variant::ToString() const {
	wchar_t *tmp = VarToStr(this, (wchar_t*)0);
	if(!tmp) return PrOutputString();
	PrOutputString ts = ToPrOutputString(tmp);
	delete [] tmp;
	return ts;
}

/** Returns the value of the variant converted to integer array.
 *
 * @return			The value of the variant converted to integer array.
 */
std::vector<int> Variant::ToIntArray(void) const {
	std::vector<int> vect;
	int nitems = NItems();
	if(nitems<=0) return vect;
	vect.resize((size_t)nitems);
	if(GetData(Types::IntArray, (void*)PrIns::vector_data(vect), (int)sizeof(int)*nitems, sizeof(int), nitems))
		vect.clear();
	return vect;
}

/** Returns the value of the variant converted to byte array.
 *
 * @return			The value of the variant converted to byte array.
 */
std::vector<gxu8> Variant::ToByteArray(void) const {
	std::vector<gxu8> vect;
	int nitems = NItems();
	int size = Size();
	if(nitems*size<=0) return vect;
	nitems *= size;
	vect.resize((size_t)nitems);
	if(GetData(Types::UintArray, (void*)PrIns::vector_data(vect), nitems, 1, nitems)) vect.clear();
	return vect;
}

/** Returns the value of the variant converted to frame.
 *
 * @return			The value of the variant converted to Frame.
 */
Frame Variant::ToFrame(void) const {
	Frame frame;
	frame.x1 = frame.y1 = frame.x2 = frame.y2 = frame.x3 = frame.y3 = frame.x4 = frame.y4 = 0;
	GetData(Types::IntArray, &frame, sizeof(frame), sizeof(int), 8);
	return frame;
}

/** @} */

/** Appends an item to the list type variant.
 *
 * If the variant type is not list but NULL type, this method automatically convert
 * it. For other types an error is reported.
 *
 * \note The variant lists contains references to variants, so if the appended item
 * is changed the modification affects the item in the list too. To avoid this
 * behaviour the item can be duplicated before appending with the operator ~().
 * \code
 * list << ~(item = 2);
 * \endcode
 *
 * @param var		The item to add to the list.
 * @return			The variant list object.
 */
Variant& Variant::operator << (const Variant& var) {
	try {
		return AddListItem(var);
	}
	catch(const Exceptions::InvalidParameter &) {
		if(Type()!=Types::Null) throw;
	}
	ChangeToList();
	return AddListItem(var);
}

/** Returns an item of the list type variant.
 *
 * @param ix		The index of the requested item.
 * @return			The requested item.
 */
Variant Variant::operator [] (int ix) const {
	return GetListItem(ix);
}

/** Creates a list type variant structure.
 *
 * @param id		The Id of the variant.
 * @param name		The name of the variant.
 * @return			The variant list object.
 */
Variant& Variant::CreateList(int id, const InputString &name) {
	return Create(id, name, Types::List, 0, 0, 0);
}

/** Modifies an existing variant to list type.
 *
 * @return			The variant object.
 */
Variant& Variant::ChangeToList(void) {
	return Update(Types::List, 0, 0, 0);
}

/** Adds an item to the list type variant.
 *
 * \note The variant lists contains references to variants, so if the appended item
 * is changed the modification affects the item in the list too. To avoid this
 * behaviour the item can be duplicated before appending with the Duplicate() method.
 *
 * @param item		The item to add to the list.
 * @param position	The position where to insert or append the new item.
 * @return			The variant list object.
 */
Variant& Variant::AddListItem(const Variant& item, const VariantPos& position /* VariantPos::Last() */) {
	PrIns::VariantItem iv;
	iv.parent = _variant;
	iv.flags = position.flags;
	iv.id = position.id;
	iv.name = position.name;
	iv.ix = position.ix;
	iv.item = item._variant;
	Call(PrIns::VariantCallCodes::AddItem, &iv);
	return *this;
}

/** Returns an item of the list type variant.
 *
 * @param ix		The index of the requested item.
 * @return			The requested item.
 */
Variant Variant::GetListItem(int ix) const {
	return GetListItem(VariantPos::ByIndex(ix));
}

/** Returns an item of the list type variant.
 *
 * @param position	The position of the requested item.
 * @return			The requested item.
 */
Variant Variant::GetListItem(const VariantPos& position) const {
	Variant var;
	PrIns::VariantItem gi;
	gi.parent = _variant;
	gi.flags = position.flags;
	gi.id = position.id;
	gi.name = position.name;
	gi.ix = position.ix;
	gi.item = 0;
	if(!var.Call(PrIns::VariantCallCodes::GetItem, &gi)) var._variant = gi.item;
	return var;
}

/** Returns a child of a variant.
 *
 * If the requested child is missing and the ix parameter is 0 this method creates
 * the child automatically.
 *
 * @param id		The Id of the requested child.
 * @param ix		The index of the requested child.
 * @return			The requested child.
 */
Variant Variant::operator () (int id, int ix /* 0 */) {
	try {
		return GetChild(id, ix);
	}
	catch(const Exceptions::EntryNotFound &) {
		if(ix) throw;
	}
	Variant var;
	var.Create(id, (wchar_t*)0);
	AddChild(var);
	return var;
}

/** Returns a child of a variant.
 *
 * If the requested child is missing and the ix parameter is 0 this method creates
 * the child automatically.
 *
 * @param name		The name of the requested child.
 * @param ix		The index of the requested child.
 * @return			The requested child.
 */
Variant Variant::operator () (const InputString &name, int ix /* 0 */) {
	try {
		return GetChild(name, ix);
	}
	catch(const Exceptions::EntryNotFound &) {
		if(ix) throw;
	}
	Variant var;
	var.Create(0, name);
	AddChild(var);
	return var;
}

/** Returns the number of children in the variant.
 *
 * @return			The number of children in the variant.
 */
int Variant::NChildren(void) const {
	try {
		PrIns::VariantGetInfo gvi;
		gvi.variant = _variant;
		gvi.namebuff = 0;
		gvi.namebuffsize = 0;
		if(Call(PrIns::VariantCallCodes::GetInfo, &gvi)) return -1;
		return gvi.nchildren;
	}
	catch(const Exceptions::General &) {
		return 0;
	}
}

/** Adds a child element to the variant.
 *
 * @param item		The new child to add to the children list.
 * @param position	The position to insert or append the new child.
 * @return			The variant object.
 */
Variant& Variant::AddChild(const Variant& item, const VariantPos& position /* VariantPos::Last() */) {
	PrIns::VariantItem ic;
	ic.parent = _variant;
	ic.flags = position.flags;
	ic.id = position.id;
	ic.name = position.name;
	ic.ix = position.ix;
	ic.item = item._variant;
	Call(PrIns::VariantCallCodes::AddChild, &ic);
	return *this;
}

/** Returns a child of a variant.
 *
 * @param ix		The index of the requested child.
 * @return			The requested child.
 */
Variant Variant::GetChildByIndex(int ix) const {
	return GetChild(VariantPos::ByIndex(ix));
}

/** Returns a child of a variant.
 *
 * @param id		The Id of the requested child.
 * @param ix		The index of the requested child.
 * @return			The requested child.
 */
Variant Variant::GetChild(int id, int ix) const {
	return GetChild(VariantPos::ById(id, ix));
}

/** Returns a child of a variant.
 *
 * @param name		The name of the requested child.
 * @param ix		The index of the requested child.
 * @return			The requested child.
 */
Variant Variant::GetChild(const InputString &name, int ix) const {
	return GetChild(VariantPos::ByName(name, ix));
}

/** Returns a child of a variant.
 *
 * @param position	The position of the requested child.
 * @return			The requested child.
 */
Variant Variant::GetChild(const VariantPos& position) const {
	Variant var;
	PrIns::VariantItem gc;
	gc.parent = _variant;
	gc.flags = position.flags;
	gc.id = position.id;
	gc.name = position.name;
	gc.ix = position.ix;
	gc.item = 0;
	if(!var.Call(PrIns::VariantCallCodes::GetChild, &gc)) var._variant = gc.item;
	return var;
}

/** Creates a variant structure.
 *
 * @param id		The Id of the variant.
 * @param name		The name of the variant.
 * @param type		The type of the variant.
 * @param size		The size of the variant.
 * @param data		The data of the variant.
 * @param nitems	The number of items of the variant. This value is usually
 *					1, except for list and array types.
 * @return			The variant object.
 */
Variant& Variant::Create(int id, const InputString &name, Types type, int size, const void *data,
											int nitems /* 1 */) {

	Leave();
	PrIns::VariantUpdate cv;
	cv.variant = 0;
	cv.id = id;
	cv.name = name.GetString();
	cv.type = type;
	cv.size = size;
	cv.nitems = nitems;
	cv.data = data;
	if(!Call(PrIns::VariantCallCodes::Create, &cv)) _variant = cv.variant;
	return *this;
}

/** Modifies the type and the value of an existing variant.
 *
 * @param type		The type of the variant.
 * @param size		The size of the variant.
 * @param data		The data of the variant.
 * @param nitems	The number of items of the variant. This value is usually
 *					1, except for list and array types.
 * @return			The variant object.
 */
Variant& Variant::Update(Types type, int size, const void *data, int nitems /* 1 */) {
	PrIns::VariantUpdate uv;
	uv.variant = _variant;
	uv.type = type;
	uv.size = size;
	uv.nitems = nitems;
	uv.data = data;
	Call(PrIns::VariantCallCodes::UpdateData, &uv);
	return *this;
}

/** Returns the value of the variant converted to a specified type.
 *
 * @param type		Type to convert to.
 * @param target	Preallocated buffer for the resulted data.
 * @param tgtlen	Size of the target buffer.
 * @param size		The item size. This value is useful in case of array handling.
 *					Otherwise it can be 0.
 * @param nitems	The number of items. This value is useful in case of array handling.
 *					Otherwise it can be 0.
 * @return			Error code.
 */
int Variant::GetData(Types type, void *target, int tgtlen, int size /* 0 */, int nitems /* 0 */) const {
	struct ConvertVariant {
		PrIns::gxVARIANT	 variant;
		int					 id;
		int					 type;
		int					 size;
		int					 nitems;
		void				*target;
		int					 tgtlen;
	} cv = { _variant, 0, type, (size ? size : tgtlen), (nitems ? nitems : 1), target, tgtlen };
	return Call(PrIns::VariantCallCodes::Convert, &cv);
}

/** Returns a variant from a tree.
 *
 * The variant data type can constitute a tree structure: every item in the tree is a variant.
 * These variants can have child items that are also variants. The trees can be traversed
 * level by level with the help of GetChild() and GetListItem() methods or the special
 * operators (operator []() and operator()()).
 * If there is a need to reach a
 * descendant variant located on a lower level, this method should be used instead. The item
 * in the tree can be obtained by the path string that defines the exact location of the
 * requested variant and has the following format:
 *
 * condition[=value] [,condition[=value]] [/condition[=value] [,condition[=value]]]
 *
 * The path contains separators:
 * \li the '/' separates the levels of the tree,
 * \li the ',' separates the search conditions of a node,
 * \li the '=' signs that a value follows,
 *
 * and identifiers:
 * \li the 'C' selects a child item,
 * \li the 'L' selects a list item,
 * \li the 'X' signs that the value defines the index of the item,
 * \li the 'D' signs that the value defines the Id of the item,
 * \li the 'N' signs that the value defines the name of the item,
 * \li the 'V' signs that the value defines the value of the item.
 *
 * The values are used as integers except 'name' values that have to be quoted with " or ' character;
 * and 'value' values that may be quoted with " or ' character to sign that string comparison is
 * required. Indexing is made by order.
 *
 * @param path		The path string.
 * @return			The requested descendant item.
 */
Variant Variant::GetItemByPath(const InputString &path) const {
	Variant var;
	struct GetVariantByPath {
		PrIns::gxVARIANT	 root;
		const wchar_t		*path;
		PrIns::gxVARIANT	 item;
	} gp = { _variant, path.GetString(), 0 };
	if(!var.Call(PrIns::VariantCallCodes::GetByPath, &gp)) var._variant = gp.item;
	return var;
}

int Variant::Call(int cmd, void* params) {
	PrIns::gxHANDLE nullhandle = { 0 };
	if(PrIns::gxCall::Call(nullhandle, cmd, params)) return 0;
	Exceptions::General::ThrowException();
	return PrIns::gxCall::Call(nullhandle, PrIns::GX_CALL_GETERROR, 0);
}

#define ENUMSTEP 2
GX_ENUM_BEGIN(Variant::Types)
#include "Util/vartypes.inl"
GX_ENUM_END(Variant::Types)
#undef ENUMSTEP

} // namespace Util
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_VARIANT_INCL
