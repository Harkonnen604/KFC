#include "kfc_ktl_pch.h"
#include "memory_stream.h"

// --------------
// Memory stream
// --------------
TStream& operator >> (TStream& Stream, TMemoryStream& RMemoryStream)
{
    RMemoryStream.m_szOffset = 0;

    return Stream >> RMemoryStream.m_Data;
}

TStream& operator << (TStream& Stream, const TMemoryStream& MemoryStream)
{
    return Stream << MemoryStream.m_Data;
}
