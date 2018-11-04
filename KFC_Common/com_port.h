#ifndef com_port_h
#define com_port_h

#include <KFC_KTL/event.h>

#ifdef _MSC_VER

// ---------
// COM port
// ---------
class T_COM_Port : public TStream
{
public:
	// DCB
	struct T_DCB : public DCB
	{
		T_DCB()
			{ memset(this, 0, sizeof(DCB)), DCBlength = sizeof(DCB); }

		T_DCB(LPCTSTR pSettings)
			{ Build(pSettings); }

		T_DCB& Build(LPCTSTR pSettings);
	};

	// Settings
	struct TSettings
	{
	private:
		TCHAR GetParityChar() const;

	public:
		size_t	m_szPortNumber;
		size_t	m_szBaudRate;
		size_t	m_szDataBits;
		ktype_t	m_tpParity;
		size_t	m_szStopBits;

	public:
		TSettings(size_t szPortNumber = 1);

		TSettings(LPCTSTR pString)
			{ *this = ReadFromString<TSettings>(pString); }

		operator KString () const;

		KString GetDCB_String() const;
	};

private:
	HANDLE m_hFile;

public:
	T_COM_Port();

	T_COM_Port(size_t szPortNumber, const DCB& dcb, bool bOverlapped = false);

	T_COM_Port(size_t szPortNumber, LPCTSTR pSettings, bool bOverlapped = false);

	T_COM_Port(const TSettings& Settings, bool bOverlapped = false);

	~T_COM_Port()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hFile != INVALID_HANDLE_VALUE; }

	void Release();

	void Allocate(size_t szPortNumber, const DCB& dcb, bool bOverlapped = false);

	void Allocate(size_t szPortNumber, LPCTSTR pSettings, bool bOverlapped = false);

	void Allocate(const TSettings& Settings, bool bOverlapped = false)
		{ Allocate(Settings.m_szPortNumber, Settings.GetDCB_String(), bOverlapped); }

	void Purge();

	void Receive(void* pRData, size_t szLength);

	void Send(const void* pData, size_t szLength);

	void Receive(void* pRData, size_t szLength, TEvent& Event, HANDLE hTerminator = NULL);

	void Send(const void* pData, size_t szLength, TEvent& Event, HANDLE hTerminator = NULL);

	void StreamRead(void* pRData, size_t szLength)
		{ Receive(pRData, szLength); }

	void StreamWrite(const void* pData, size_t szLength)
		{  Send(pData, szLength); }

	HANDLE GetHandle() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hFile; }

	operator HANDLE () const
		{ return GetHandle(); }
};

inline bool operator == (	const T_COM_Port::TSettings& Settings1,
							const T_COM_Port::TSettings& Settings2)
{
	return !memcmp(&Settings1, &Settings2, sizeof(T_COM_Port::TSettings));
}

inline bool operator != (	const T_COM_Port::TSettings& Settings1,
							const T_COM_Port::TSettings& Settings2)
{
	return memcmp(&Settings1, &Settings2, sizeof(T_COM_Port::TSettings));
}

bool FromString(KString String, T_COM_Port::TSettings& RSettings);

#endif // _MSC_VER

#endif // com_port_h
