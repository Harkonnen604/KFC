#include "kfc_common_pch.h"
#include "com_port.h"

#ifdef _MSC_VER

// ---------
// COM port
// ---------

// DCB
T_COM_Port::T_DCB& T_COM_Port::T_DCB::Build(LPCTSTR pSettings)
{
	memset(this, 0, sizeof(DCB)), DCBlength = sizeof(DCB);

	if(!BuildCommDCB(pSettings, this))
	{
		INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Invalid DCB settings: \"") +
											pSettings + TEXT('"'),
										GetLastError());
	}

	fBinary = TRUE;

	return *this;
}

// Settings
T_COM_Port::TSettings::TSettings(size_t szPortNumber)
{
	m_szPortNumber	= szPortNumber;
	m_szBaudRate	= 9600;
	m_szDataBits	= 8;
	m_tpParity		= PARITY_NONE;
	m_szStopBits	= 1;
}

TCHAR T_COM_Port::TSettings::GetParityChar() const
{
	switch(m_tpParity)
	{
	case PARITY_NONE:
		return TEXT('N');

	case PARITY_ODD:
		return TEXT('O');
		break;

	case PARITY_EVEN:
		return TEXT('E');

	default:
		INITIATE_FAILURE;
	}
}

T_COM_Port::TSettings::operator KString () const
{
	return KString::Formatted(	TEXT("COM%u %u %u%c%u"),
									m_szPortNumber,
									m_szBaudRate,
									m_szDataBits,
									GetParityChar(),
									m_szStopBits);
}

KString T_COM_Port::TSettings::GetDCB_String() const
{
	return KString::Formatted(	TEXT("COM%u:baud=%u data=%u parity=%c stop=%u"),
									m_szPortNumber,
									m_szBaudRate,
									m_szDataBits,
									GetParityChar(),
									m_szStopBits);
}

bool FromString(KString String, T_COM_Port::TSettings& RSettings)
{
	String.TrimSingleSpace();

	if(String.GetLength() > 128 || !String.DoesStartNoCase(TEXT("COM")))
		return false;

	TCHAR cParityChar;

	if(_stscanf((LPCTSTR)String + 3,
				TEXT("%u %u %u%c%u"),
					&RSettings.m_szPortNumber,
					&RSettings.m_szBaudRate,
					&RSettings.m_szDataBits,
					&cParityChar,
					&RSettings.m_szStopBits) != 5)
	{
		return false;
	}

	if(RSettings.m_szPortNumber < 1 || RSettings.m_szPortNumber > 0x100)
		return false;

	switch(_totupper(cParityChar))
	{
	case TEXT('N'):
		RSettings.m_tpParity = PARITY_NONE;
		break;

	case TEXT('O'):
		RSettings.m_tpParity = PARITY_ODD;
		break;

	case TEXT('E'):
		RSettings.m_tpParity = PARITY_EVEN;
		break;

	default:
		return false;
	}

	return true;
}

// COM port
T_COM_Port::T_COM_Port()
{
	m_hFile = INVALID_HANDLE_VALUE;
}

T_COM_Port::T_COM_Port(size_t szPortNumber, const DCB& dcb, bool bOverlapped)
{
	m_hFile = INVALID_HANDLE_VALUE;

	Allocate(szPortNumber, dcb, bOverlapped);
}

T_COM_Port::T_COM_Port(size_t szPortNumber, LPCTSTR pSettings, bool bOverlapped)
{
	m_hFile = INVALID_HANDLE_VALUE;

	Allocate(szPortNumber, pSettings, bOverlapped);
}

T_COM_Port::T_COM_Port(const TSettings& Settings, bool bOverlapped)
{
	m_hFile = INVALID_HANDLE_VALUE;

	Allocate(Settings.m_szPortNumber, Settings.GetDCB_String(), bOverlapped);
}

void T_COM_Port::Release()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile), m_hFile = INVALID_HANDLE_VALUE;
}

void T_COM_Port::Allocate(size_t szPortNumber, const DCB& dcb, bool bOverlapped)
{
	Release();

	try
	{
		m_hFile = CreateFile(	(KString)"\\\\.\\\\COM" + szPortNumber,
								GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								bOverlapped ? FILE_FLAG_OVERLAPPED : 0,
								NULL);

		if(m_hFile == INVALID_HANDLE_VALUE)
		{
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error opening COM port ") + szPortNumber,
											GetLastError());
		}

		if(!SetCommState(m_hFile, (DCB*)&dcb))
		{
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error initializing COM port ") +
												szPortNumber,
											GetLastError());
		}

		SetCommMask(m_hFile, 0);

		COMMTIMEOUTS Timeouts;
		memset(&Timeouts, 0, sizeof(Timeouts));

		SetCommTimeouts(m_hFile, &Timeouts);

		Purge();		
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void T_COM_Port::Allocate(size_t szPortNumber, LPCTSTR pSettings, bool bOverlapped)
{
	Release();

	Allocate(szPortNumber, T_DCB(pSettings), bOverlapped);
}

void T_COM_Port::Purge()
{
	DEBUG_VERIFY_ALLOCATION;

	PurgeComm(m_hFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

void T_COM_Port::Receive(void* pRData, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	while(szLength > 0)
	{
		DWORD dwRead;

		if(!ReadFile(m_hFile, pRData, szLength, &dwRead, NULL) || dwRead > szLength)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error receiving data from COM port"),
											GetLastError());
		}

		(BYTE*&)pRData += dwRead, szLength -= dwRead;
	}
}

void T_COM_Port::Send(const void* pData, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	while(szLength > 0)
	{
		DWORD dwWritten;

		if(!WriteFile(m_hFile, pData, szLength, &dwWritten, NULL) || dwWritten > szLength)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error sending data through COM port"),
											GetLastError());
		}

		(BYTE*&)pData += dwWritten, szLength -= dwWritten;
	}
}

void T_COM_Port::Receive(void* pRData, size_t szLength, TEvent& Event, HANDLE hTerminator)
{
	Event.Reset();

	while(szLength > 0)
	{
		OVERLAPPED Overlapped;
		memset(&Overlapped, 0, sizeof(Overlapped));

		Overlapped.hEvent = Event;

		if(	!ReadFile(m_hFile, pRData, szLength, NULL, &Overlapped) &&
			GetLastError() != ERROR_IO_PENDING)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error receiving data from COM port"),
											GetLastError());
		}

		try
		{
			Event.WaitWithTermination(hTerminator);
		}

		catch(...)
		{
			CancelIo(m_hFile);
			throw;
		}

		DWORD dwRead;

		if(!GetOverlappedResult(m_hFile, &Overlapped, &dwRead, TRUE))
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error completing receiving data from COM port"),
											GetLastError());
		}

		(BYTE*&)pRData += dwRead, szLength -= dwRead;
	}
}

void T_COM_Port::Send(const void* pData, size_t szLength, TEvent& Event, HANDLE hTerminator)
{
	Event.Reset();

	while(szLength > 0)
	{
		OVERLAPPED Overlapped;
		memset(&Overlapped, 0, sizeof(Overlapped));

		Overlapped.hEvent = Event;

		if(	!WriteFile(m_hFile, pData, szLength, NULL, &Overlapped) &&
			GetLastError() != ERROR_IO_PENDING)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error sending data through COM port"),
											GetLastError());
		}

		try
		{
			Event.WaitWithTermination(hTerminator);
		}

		catch(...)
		{
			CancelIo(m_hFile);
			throw;
		}

		DWORD dwWritten;

		if(!GetOverlappedResult(m_hFile, &Overlapped, &dwWritten, TRUE))
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error completing sending data through COM port"),
											GetLastError());
		}

		(const BYTE*&)pData += dwWritten, szLength -= dwWritten;
	}
}

#endif // _MSC_VER
