#ifndef pow2_h
#define pow2_h

// ----------------
// Global routines
// ----------------
inline size_t HighestBit1(UINT uiValue)
{
	if(!uiValue)
		return UINT_MAX;

	const size_t	szUINTHighestBit		= (sizeof(UINT) << 3) - 1;
	const UINT		uiUINTHighestBitValue	= 1 << szUINTHighestBit;

	for(size_t szRet = szUINTHighestBit ; ; szRet--)
	{
		if(uiValue & uiUINTHighestBitValue)
			return szRet;

		uiValue <<= 1;
	}
}

inline size_t HighestBit1(QWORD qwValue)
{
	if(!qwValue)
		return UINT_MAX;

	const size_t	szQWORDHighestBit		= (sizeof(QWORD) << 3) - 1;
	const QWORD		qwQWORDHighestBitValue	= (QWORD)1 << szQWORDHighestBit;

	for(size_t szRet = szQWORDHighestBit ; ; szRet--)
	{
		if(qwValue & qwQWORDHighestBitValue)
			return szRet;

		qwValue <<= 1;
	}
}

inline size_t LowestBit1(UINT uiValue)
{
	if(!uiValue)
		return UINT_MAX;

	for(size_t szRet = 0 ; ; szRet++)
	{
		if(uiValue & 1)
			return szRet;

		uiValue >>= 1;
	}
}

inline size_t LowestBit1(QWORD qwValue)
{
	if(!qwValue)
		return UINT_MAX;

	for(size_t szRet = 0 ; ; szRet++)
	{
		if(qwValue & 1)
			return szRet;

		qwValue >>= 1;
	}
}

inline bool IsPow2(UINT uiValue)
{
	if(!uiValue)
		return false;

	for( ; !(uiValue & 1) ; uiValue >>= 1);

	return !(uiValue >> 1);
}

inline bool IsPow2(QWORD qwValue)
{
	if(!qwValue)
		return false;

	for( ; !(qwValue & 1) ; qwValue >>= 1);

	return !(qwValue >> 1);
}

inline UINT MaxPow2LessEq(UINT uiValue)
{
	if(!uiValue)
		return UINT_MAX;

	UINT uiRet;

	for(uiRet = 1 << ((sizeof(UINT) << 3) - 1) ;
		uiRet > uiValue ;
		uiRet >>= 1);

	return uiRet;
}

inline QWORD MaxPow2LessEq(QWORD qwValue)
{
	if(!qwValue)
		return QWORD_MAX;

	QWORD qwRet;

	for(qwRet = (QWORD)1 << ((sizeof(QWORD) << 3) - 1) ;
		qwRet > qwValue ;
		qwRet >>= 1);

	return qwRet;
}

inline UINT MinPow2GreaterEq(UINT uiValue)
{
	if(uiValue > 1u << 31)
		return UINT_MAX;

	UINT uiRet;

	for(uiRet = 1 ; uiRet < uiValue ; uiRet <<= 1);

	return uiRet;
}

inline QWORD MinPow2GreaterEq(QWORD qwValue)
{
	if(qwValue > (QWORD)1 << 63)
		return QWORD_MAX;

	QWORD qwRet;

	for(qwRet = 1 ; qwRet < qwValue ; qwRet <<= 1);

	return qwRet;
}

#endif // pow2_h
