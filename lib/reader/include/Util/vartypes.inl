
	/* Basic types (size = size of one element in bytes, nitems = number of items) */
	GXEV( Null,				0x00 )		///< NULL type (size: 0; nitems: 1)
	GXEV( Int,				0x01 )		///< Integer type (size: 0, 1, 2, 4, 8; nitems: 1)
	GXEV( Uint,				0x02 )		///< Unsigned integer type (size: 0, 1, 2, 4, 8; nitems: 1)
	GXEV( Float,			0x03 )		///< Floating point type (size: 0, 4, 8; nitems: 1)
	GXEV( DateTime,			0x04 )		///< Datetime type (size: 0, 8; nitems: 1).

	/* Array types, bit 6 set defines the array type.
	 * (size = size of the element in bytes, nitems = number of items) */
	GXEV( Array,			0x40 )		///< NULL type array /NULL array/ (size: 0; nitems: x)
	GXEV( IntArray,			0x41 )		///< Integer type array (size: 0, 1, 2, 4, 8; nitems: x)
	GXEV( UintArray,		0x42 )		///< Unsigned integer type array (size: 0, 1, 2, 4, 8; nitems: x)
	GXEV( FloatArray,		0x43 )		///< Floating point type array (size: 0, 4, 8; nitems: x)
	GXEV( DateTimeArray,	0x44 )		///< Datetime type array (size: 0, 8; nitems: x).

	/* Other dynamically sized types */
	GXEV( List,				0x80 )		///< List type (size=nitems: number of items)
	GXEV( Binary,			0x81 )		///< Binary type (size=number of bytes; nitems: 1)
	GXEV( Ascii,			0x82 )		///< String type in ASCII format (size=number of characters; nitems: 1)
	GXEV( Unicode,			0x83 )		///< String type in unicode format (size=number of characters; nitems: 1)
	GXEV( Container,		0x84 )		///< Internally used binary structure (size=number of codes; nitems: 1).

	/* Types for creation operation only */
	GXEV( ACAscii,			0x1082 )	///< String type in ASCII format (size=number of characters; nitems: 1) (see note above)
	GXEV( ACUnicode,		0x1083 )	///< String type in unicode format (size=number of characters; nitems: 1) (see note above)
	GXEV( ASAscii,			0x2082 )	///< String type in ASCII format (size=number of characters; nitems: 1) (see note above)
	GXEV( ASUnicode,		0x2083 )	///< String type in unicode format (size=number of characters; nitems: 1) (see note above).
