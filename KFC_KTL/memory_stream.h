#ifndef memory_stream_h
#define memory_stream_h

#include "array.h"
#include "stream.h"
#include "debug.h"

// --------------
// Memory stream
// --------------
class TMemoryStream : public TStream
{
private:
	TArray<BYTE, true> m_Data;

	size_t m_szOffset;

public:
	TMemoryStream()
		{ m_szOffset = 0; }

	TMemoryStream(const TMemoryStream& Stream) : m_Data(Stream.m_Data)
		{ m_szOffset = 0; }

	TMemoryStream& operator = (const TMemoryStream& Stream)
	{
		m_Data = Stream.m_Data;

		m_szOffset = 0;

		return *this;
	}

	bool IsEmpty() const
	{
		return m_Data.IsEmpty();
	}

	bool IsOver() const
	{
		DEBUG_VERIFY(m_szOffset <= m_Data.GetN());

		return m_szOffset == m_Data.GetN();
	}

	void Clear(bool bFixAllocation = true)
	{
		m_Data.Clear(bFixAllocation);

		m_szOffset = 0;
	}

	const void* GetReadDataPtr(size_t szLength)
	{
		DEBUG_VERIFY(m_szOffset + szLength <= m_Data.GetN());

		m_szOffset += szLength;

		return m_Data.GetDataPtr() + (m_szOffset - szLength);
	}

	void RewindReader(size_t szOffset = 0)
	{
		DEBUG_VERIFY(szOffset <= m_Data.GetN());

		m_szOffset = szOffset;
	}

	void* GetWriteDataPtr(size_t szLength)
		{ return &m_Data.Add(szLength); }

	size_t GetNRemaining() const
		{ return m_Data.GetN() - m_szOffset; }

	size_t GetOffset() const
		{ return m_szOffset; }

	void StreamRead(void* pRData, size_t szLength)
		{ memcpy(pRData, GetReadDataPtr(szLength), szLength); }

	void StreamWrite(const void* pData, size_t szLength)
		{ memcpy(GetWriteDataPtr(szLength), pData, szLength); }

	TMemoryStream& Reown(TMemoryStream& Stream)
	{
		m_Data.Reown(Stream.m_Data);

		m_szOffset = Stream.m_szOffset, Stream.m_szOffset = 0;

		return *this;
	}

	TArray<BYTE, true>& GetData()
		{ return m_Data; }

	const TArray<BYTE, true>& GetData() const
		{ return m_Data; }

	DECLARE_FRIENDLY_STREAMING(TMemoryStream);
};

DECLARE_STREAMING(TMemoryStream);

// ---------------------
// Mapped memory stream
// ---------------------
class TMappedMemoryStream : public TStream
{
private:
	void* m_pData;

	size_t m_szLength;

	size_t m_szReadOffset;
	size_t m_szWriteOffset;

public:
	TMappedMemoryStream() : m_szLength(UINT_MAX) {}

	TMappedMemoryStream(void* pData, size_t szLength) : m_szLength(UINT_MAX)
		{ Allocate(pData, szLength); }

	~TMappedMemoryStream()
		{ Release(); }

	bool IsAllocated() const
		{ return m_szLength != UINT_MAX; }

	void Release()
		{ m_szLength = UINT_MAX; }

	void Allocate(void* pData, size_t szLength)
	{
		Release();

		DEBUG_VERIFY(szLength != UINT_MAX);

		m_pData = pData;

		m_szLength = szLength;

		m_szReadOffset = m_szWriteOffset = 0;
	}

	void RewindReader()
		{ DEBUG_VERIFY_ALLOCATION; m_szReadOffset = 0; }

	void RewindWriter()
		{ DEBUG_VERIFY_ALLOCATION; m_szWriteOffset = 0; }

	const void* GetReadDataPtr(size_t szLength)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(m_szReadOffset + szLength <= m_szLength);

		m_szReadOffset += szLength;

		return (const BYTE*)m_pData + (m_szReadOffset - szLength);
	}

	void* GetWriteDataPtr(size_t szLength)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(m_szWriteOffset + szLength <= m_szLength);

		m_szWriteOffset += szLength;

		return (BYTE*)m_pData + (m_szWriteOffset - szLength);
	}

	size_t GetReadOffset() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szReadOffset; }

	size_t GetWriteOffset() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szWriteOffset; }

	void* GetDataPtr()
		{ DEBUG_VERIFY_ALLOCATION; return m_pData; }

	const void* GetDataPtr() const 
		{ DEBUG_VERIFY_ALLOCATION; return m_pData; }

	size_t GetLength() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szLength; }

	size_t GetNReadRemaining() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szLength - m_szReadOffset; }

	size_t GetNWriteRemaining() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szLength - m_szWriteOffset; }

	void StreamRead(void* pRData, size_t szLength)
		{ memcpy(pRData, GetReadDataPtr(szLength), szLength); }

	void StreamWrite(const void* pData, size_t szLength)
		{ memcpy(GetWriteDataPtr(szLength), pData, szLength); }
};

#endif // memory_stream_h
