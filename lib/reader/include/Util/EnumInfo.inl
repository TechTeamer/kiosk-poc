#define PR22_VERSION 2.2.0.19
//------------------------------------------------------------------------------

#define GX_ENUM_DECLARE_1(ENUM_NAME)							\
class ENUM_NAME {													\
	friend class Util::EnumInfo<ENUM_NAME>;							\
	inline static Util::ENUM_INFO * gi(int ix);						\
public:																\
	ENUM_NAME() { Value = (values)0; }								\
	/** Constructor for integer conversion */						\
	explicit ENUM_NAME(int v) { Value = (values)v; }				\
	/** Conversion operator to integer */							\
	operator int() const { return Value; }							\
	/** Returns the string representation of the enum */			\
	const wchar_t * ToString() const { Util::ENUM_INFO *info;		\
		for(int ix=0; (info=gi(ix))->name; ix++)					\
		if((values)info->value==Value) break; return info->name;	\
	}																\
	/** Values of the enum. */										\
	enum values {

#define GXEV_1(NAME, VALUE)		NAME = (int)VALUE,

#define GX_ENUM_END_DECL_1(ENUM_NAME)			\
	} Value; /**< Value of the enum */				\
	/** Constructor for value conversion */			\
	ENUM_NAME(const values &v) { Value = v; }		\
	};

//------------------------------------------------------------------------------

#define GX_ENUM_DECLARE_2(ENUM_NAME)				\
	inline Util::ENUM_INFO * ENUM_NAME::gi(int ix) {	\
	static Util::ENUM_INFO info[] = {

#define GXEV_2(NAME, VALUE)		{ GXM_CNC(L, #NAME), (int)VALUE },

#define GX_ENUM_END_DECL_2(ENUM_NAME) { 0, 0 } };	\
	int size = sizeof(info)/sizeof(info[0]);			\
	return info+(ix>=0 && ix<size ? ix : size-1); }

//------------------------------------------------------------------------------
#define GX_ENUM_DECLARE_3(ENUM_NAME)
#define GXEV_3(NAME, VALUE)
#define GX_ENUM_END_DECL_3(ENUM_NAME)
//------------------------------------------------------------------------------
#define GXM_F1(X) #X
#define GXM_F2(X, Y) X ## Y
#define GXM_I2S(X) GXM_F1(X)
#define GXM_CNC(X, Y) GXM_F2(X, Y)
//------------------------------------------------------------------------------
