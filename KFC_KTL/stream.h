#ifndef stream_h
#define stream_h

// ----------------------------------
// Stream read/write variable macros
// ----------------------------------
#define STREAM_READ_VAR(Stream, Var)	{ (Stream).StreamRead	(&(Var), sizeof(Var)); }
#define STREAM_WRITE_VAR(Stream, Var)	{ (Stream).StreamWrite	(&(Var), sizeof(Var)); }

// -------------------------------
// Stream read/write array macros
// -------------------------------
#define STREAM_READ_ARR(Stream, Arr, NumItems, ItemType)	{ (Stream).StreamRead	((Arr), (NumItems) * sizeof(ItemType)); }
#define STREAM_WRITE_ARR(Stream, Arr, NumItems, ItemType)	{ (Stream).StreamWrite	((Arr), (NumItems) * sizeof(ItemType)); }

// ----------------------------
// Basic streaming declaration
// ----------------------------
#define DECLARE_BASIC_STREAMING(Type)									\
	inline TStream& operator >> (TStream& Stream, Type& RValue)			\
	{																	\
		STREAM_READ_VAR(Stream, RValue);								\
		return Stream;													\
	}																	\
																		\
	inline TStream& operator << (TStream& Stream, const Type& Value)	\
	{																	\
		STREAM_WRITE_VAR(Stream, Value);								\
		return Stream;													\
	}																	\

#define DECLARE_FRIENDLY_BASIC_STREAMING(Type)										\
	friend inline TStream& operator >> (TStream& Stream, Type&			RValue);	\
	friend inline TStream& operator << (TStream& Stream, const Type&	Value);		\

// ----------------------
// Streaming declaration
// ----------------------
#define DECLARE_STREAMING(Type)										\
	TStream& operator >> (TStream& Stream, Type&		RValue);	\
	TStream& operator << (TStream& Stream, const Type&	Value);		\

#define DECLARE_FRIENDLY_STREAMING(Type)									\
	friend TStream& operator >> (TStream& Stream, Type&			RValue);	\
	friend TStream& operator << (TStream& Stream, const Type&	Value);		\

// -------
// Stream
// -------
class TStream
{
public:
	virtual ~TStream() {}

	virtual void StreamRead	(void*			pRData,	size_t szLength) = 0;
	virtual void StreamWrite(const void*	pData,	size_t szLength) = 0;
};

template <class t>
inline t ReadVar(TStream& Stream)
	{ t v; return Stream >> v, v; }

#endif // stream_h
