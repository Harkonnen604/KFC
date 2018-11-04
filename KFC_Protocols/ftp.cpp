#include "kfc_protocols_pch.h"
#include "ftp.h"

#include <KFC_KTL/file_names.h>
#include <KFC_Common/file.h>

// Limits
#define FTP_BUFFER_SIZE		(8192)

// ---------------
// FTP connection
// ---------------
T_FTP_Connection::T_FTP_Connection()
{
}

T_FTP_Connection::T_FTP_Connection(	DWORD	dwIP,
									LPCTSTR	pLogin,
									LPCTSTR	pPassword,
									bool	bPassiveMode,
									HANDLE	hTerminator,
									size_t	szTimeout,
									size_t	szNAttempts,
									size_t	szRetryDelay)
{
	Allocate(	dwIP,
				pLogin,
				pPassword,
				bPassiveMode,
				hTerminator,
				szTimeout,
				szNAttempts,
				szRetryDelay);
}

void T_FTP_Connection::Release(bool bFromException)
{
	if(IsAllocated() && !bFromException)
	{
		SAFE_BLOCK_BEGIN
		{
			SendRequest(TEXT("QUIT"));
		}
		SAFE_BLOCK_END
	}

	if(!bFromException)
	{
		m_Password.Empty();

		m_Login.Empty();
	}

	m_Socket.Release();
}

void T_FTP_Connection::Allocate(DWORD	dwIP,
								LPCTSTR	pLogin,
								LPCTSTR	pPassword,
								bool	bPassiveMode,
								HANDLE	hTerminator,
								size_t	szTimeout,
								size_t	szNAttempts,
								size_t	szRetryDelay)
{
	Release();

	try
	{
		m_dwIP		= dwIP;
		m_Login		= pLogin;
		m_Password	= pPassword;

		m_bPassiveMode = bPassiveMode;

		m_hTerminator	= hTerminator;
		m_szTimeout		= szTimeout;

		m_szNAttempts	= szNAttempts;
		m_szRetryDelay	= szRetryDelay;

		Reconnect(false);
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void T_FTP_Connection::Reconnect(bool bIsRetry)
{
	if(bIsRetry && IsAllocated())
		return;

	for(size_t szAttempt = bIsRetry ? 1 : m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			m_Socket.Allocate();

			m_Socket.SetTerminator	(m_hTerminator);
			m_Socket.SetTimeout		(m_szTimeout);

			m_Socket.Connect(m_dwIP, FTP_PORT);

			ReceiveReply(2);

			SendRequest((KString)TEXT("USER ") + m_Login);

			int iCode;
			ReceiveReply(-1, &iCode);

			if(iCode/100 == 3)
			{
				SendRequest((KString)TEXT("PASS ") + m_Password);
				ReceiveReply(2);
			}
			else if(iCode/100 != 2)
			{
				INITIATE_DEFINED_FAILURE((KString)"Invalid login response code: " + iCode + ".");
			}

			SendRequest(TEXT("TYPE I"));
			ReceiveReply(2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::SetTerminator(HANDLE hTerminator)
{
	DEBUG_VERIFY_ALLOCATION;

	m_hTerminator = hTerminator;

	m_Socket.SetTerminator(hTerminator);
}

void T_FTP_Connection::SetTimeout(size_t szTimeout)
{
	DEBUG_VERIFY_ALLOCATION;

	m_szTimeout = szTimeout;
	
	m_Socket.SetTimeout(szTimeout);
}

void T_FTP_Connection::SetNAttempts(size_t szNAttempts)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szNAttempts > 0);

	m_szNAttempts = szNAttempts;
}

void T_FTP_Connection::SetRetryDelay(size_t szRetryDelay)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szRetryDelay != UINT_MAX);

	m_szRetryDelay = szRetryDelay;
}

void T_FTP_Connection::SendRequest(LPCTSTR pRequest)
{
	m_Socket.Send((KString)pRequest + TEXT("\r\n"), _tcslen(pRequest) + 2);
}

KString T_FTP_Connection::ReceiveReply(int iDesiredCodeLevel, int* pRCode)
{
	KString Reply;

	int iCode = -1;

	for(;;) // lines loop
	{
		KString Line;

		char pc = 0;

		for(;;) // chars loop
		{
			char c;

			m_Socket >> c;

			Line += c;

			if(pc == TEXT('\r') && c == TEXT('\n'))
				break;

			pc = c;
		}

		Reply += Line;

		LPCTSTR s = Line;

		for( ; _istdigit(*s) ; s++);

		if(iCode < 0)
		{
			iCode = _ttoi(Line);

			if(*s != TEXT('-'))
				break;			
		}
		else
		{
			for( ; _istdigit(*s) ; s++);

			if(*s == TEXT(' ') && _ttoi(Line) == iCode)
				break;
		}		
	}

	if(pRCode)
		*pRCode = iCode;

	if(iDesiredCodeLevel != -1 && iCode / 100 != iDesiredCodeLevel)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Errorneous FTP response: \"") + Reply + TEXT("\"."));

	return Reply;
}

void T_FTP_Connection::PrepareTransfer(LPCTSTR pRequest, TSocket& RDataSocket)
{
	if(m_bPassiveMode)
	{
		SendRequest(TEXT("PASV"));
		const KString Reply = ReceiveReply(2);

		LPCTSTR s = Reply;

		DWORD dwIP  = INADDR_NONE;
		WORD  wPort = 0;

		while(*s)
		{
			if(_istdigit(*s))
			{
				size_t a[6];

				for(size_t i = 0 ; ; i++)
				{
					a[i] = 0;

					do
					{
						a[i] *= 10, a[i] += *s++ - TEXT('0');

						if(a[i] >= 256)
							break;

					}while(_istdigit(*s));

					if(a[i] >= 256)
						break;

					if(i == 5)
					{
						dwIP  = (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
						wPort = (a[4] << 8) | a[5];

						break;
					}

					if(*s++ != TEXT(',') || !_istdigit(*s))
						break;
				}
			}
			else
			{
				s++;
			}
		}

		if(dwIP == INADDR_NONE || !wPort)
			INITIATE_DEFINED_FAILURE((KString)TEXT("Unable to parse FTP PASV reply: \"") + Reply + TEXT("\"."));

		RDataSocket.Allocate();

		RDataSocket.SetTerminator	(m_hTerminator);
		RDataSocket.SetTimeout		(m_szTimeout);

		RDataSocket.Connect(dwIP, wPort);

		SendRequest(pRequest);
		ReceiveReply(1);		
	}
	else
	{
		DWORD dwIP;
		WORD  wPort;

		m_Socket.GetLocalIP_Port(dwIP);

		TSocket ListenerSocket;
		ListenerSocket.Allocate();

		ListenerSocket.SetTerminator(m_hTerminator);
		ListenerSocket.SetTimeout	(m_szTimeout);

		ListenerSocket.Bind(dwIP, 0);		

		ListenerSocket.GetLocalIP_Port(dwIP, wPort);

		ListenerSocket.Listen(1);

		SendRequest(KString::Formatted(	TEXT("PORT %u,%u,%u,%u,%u,%u"),
											(dwIP >> 24),
											(dwIP >> 16) & 0xFF,
											(dwIP >> 8)  & 0xFF,
											(dwIP)       & 0xFF,
											(wPort >> 8),
											(wPort) & 0xFF));

		ReceiveReply(2);

		SendRequest(pRequest);
		ReceiveReply(1);

		RDataSocket.Allocate(ListenerSocket.Accept());

		RDataSocket.SetTerminator	(m_hTerminator);
		RDataSocket.SetTimeout		(m_szTimeout);
	}
}

void T_FTP_Connection::List(LPCTSTR				pMask,
							KStrings&			RItems,
							TArray<bool, true>&	RAreFolders,
							bool				bFullPaths)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			Reconnect(true);

			RItems.Clear();

			RAreFolders.Clear();

			KString Reply;

			TSocket DataSocket;

			PrepareTransfer((KString)TEXT("LIST ") +
								FixSlashes(temp<KString>(pMask), TEXT('/')),
							DataSocket);

			const KString Path =
				bFullPaths ?
					(LPCTSTR)FixSlashes(temp<KString>(GetFilePath(pMask)), TEXT('/')) :
					TEXT("");

			for(;;)
			{
				char buf[FTP_BUFFER_SIZE];

				const size_t l = DataSocket.ReceiveAvailable(buf, sizeof(buf));

				if(!l)
					break;

				Reply.Extend(buf, l);
			}

			DataSocket.Release();

			ReceiveReply(2);

			LPCTSTR s = Reply;

			for(;;)
			{
				LPCTSTR start = s;

				for( ; *s && *s != TEXT('\n') ; s++);

				LPCTSTR end = s--;

				bool bFolder = false;

				size_t i = 0;				

				for(s = start ; s < end && i < 8 ; i++)
				{
					while(_istspace(*s) && s < end)
						s++;

					if(i == 0)
						bFolder = *s == TEXT('d');

					while(!_istspace(*s) && s < end)
						s++;
				}

				while(_istspace(*s) && s < end)
					s++;

				LPCTSTR name_start = s;

				for(s = end ; s >= name_start && _istspace(*s) ; s--);

				LPCTSTR name_end = s + 1;

				if(name_start < name_end)
				{
					*RItems.AddLast() = Path + KString(name_start, name_end - name_start);

					if(RAreFolders.Add() = bFolder)
						*RItems.GetLast() += '/';
				}

				if(!*end)
					break;

				s = end + 1;
			}
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::ListFolders(LPCTSTR pMask, KStrings& RFolders, bool bFullPaths)
{
	DEBUG_VERIFY_ALLOCATION;

	TArray<bool, true> AreFolders;

	List(pMask, RFolders, AreFolders, bFullPaths);

	KStrings::TIterator Iter;

	size_t i;

	for(Iter = RFolders.GetFirst(), i = 0 ; Iter.IsValid() ; i++)
	{
		if(!AreFolders[i])
			RFolders.Del(Iter++);
		else
			++Iter;
	}
}

void T_FTP_Connection::ListFiles(LPCTSTR pMask, KStrings& RFiles, bool bFullPaths)
{
	DEBUG_VERIFY_ALLOCATION;

	TArray<bool, true> AreFolders;

	List(pMask, RFiles, AreFolders, bFullPaths);

	KStrings::TIterator Iter;

	size_t i;

	for(Iter = RFiles.GetFirst(), i = 0 ; Iter.IsValid() ; i++)
	{
		if(AreFolders[i])
			RFiles.Del(Iter++);
		else
			++Iter;
	}
}

void T_FTP_Connection::GetFile(LPCTSTR pRemoteFileName, LPCTSTR pLocalFileName)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = m_szNAttempts ; ; )
	{
		bool bFileException = false;

		TEST_BLOCK_BEGIN
		{
			TFile File;

			TEST_BLOCK_BEGIN
			{
				File.Allocate(pLocalFileName, FOF_BINARYWRITE);
			}
			TEST_BLOCK_KFC_EXCEPTION_HANDLER
			{
				bFileException = true;
				throw;
			}
			TEST_BLOCK_END

			Reconnect(true);

			TSocket DataSocket;

			PrepareTransfer((KString)TEXT("RETR ") +
								FixSlashes(temp<KString>(pRemoteFileName), TEXT('/')),
							DataSocket);

			for(;;)
			{
				BYTE buf[FTP_BUFFER_SIZE];

				const size_t l = DataSocket.ReceiveAvailable(buf, sizeof(buf));

				if(!l)
					break;
				
				TEST_BLOCK_BEGIN
				{				
					File.Write(buf, l);
				}
				TEST_BLOCK_KFC_EXCEPTION_HANDLER
				{
					bFileException = true;
					throw;
				}
				TEST_BLOCK_END
			}

			ReceiveReply(2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(bFileException || !--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::PutFile(LPCTSTR pLocalFileName, LPCTSTR pRemoteFileName)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = m_szNAttempts ; ; )
	{
		bool bFileException = false;

		TEST_BLOCK_BEGIN
		{
			TFile File;

			TEST_BLOCK_BEGIN
			{
				File.Allocate(pLocalFileName, FOF_BINARYREAD);
			}
			TEST_BLOCK_KFC_EXCEPTION_HANDLER
			{
				bFileException = true;
				throw;
			}
			TEST_BLOCK_END

			Reconnect(true);

			TSocket DataSocket;

			PrepareTransfer((KString)TEXT("STOR ") +
								FixSlashes(temp<KString>(pRemoteFileName), TEXT('/')),
							DataSocket);

			for(;;)
			{
				char buf[FTP_BUFFER_SIZE];

				size_t l;

				TEST_BLOCK_BEGIN
				{
					l = File.ReadAvailable(buf, sizeof(buf));
				}
				TEST_BLOCK_KFC_EXCEPTION_HANDLER
				{
					bFileException = true;
					throw;
				}
				TEST_BLOCK_END

				if(!l)
					break;

				DataSocket.Send(buf, l);				
			}

			DataSocket.InitiateShutdown();

			DataSocket.Release();

			ReceiveReply(2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(bFileException || !--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::LoadFile(LPCTSTR pRemoteFileName, TArray<BYTE, true>& RData)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			RData.Clear();

			Reconnect(true);

			TSocket DataSocket;

			PrepareTransfer((KString)TEXT("RETR ") +
								FixSlashes(temp<KString>(pRemoteFileName), TEXT('/')),
							DataSocket);

			for(;;)
			{
				char buf[FTP_BUFFER_SIZE];

				const size_t l = DataSocket.ReceiveAvailable(buf, sizeof(buf));

				if(!l)
					break;

				memcpy(&RData.Add(l), buf, l);
			}

			ReceiveReply(2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::SaveFile(LPCTSTR pRemoteFileName, const char* pData, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			Reconnect(true);

			TSocket DataSocket;

			PrepareTransfer((KString)TEXT("STOR ") +
								FixSlashes(temp<KString>(pRemoteFileName), TEXT('/')),
							DataSocket);

			DataSocket.Send(pData, szLength);

			DataSocket.InitiateShutdown();

			DataSocket.Release();

			ReceiveReply(2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);

			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::DeleteFile(LPCTSTR pRemoteFileName, bool bSafe, bool bOnce)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = bOnce ? 1 : m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			Reconnect(true);

			SendRequest((KString)TEXT("DELE ") +
							FixSlashes(temp<KString>(pRemoteFileName), TEXT('/')));

			ReceiveReply(bSafe ? -1 : 2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);
			
			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::CreateFolder(LPCTSTR pRemoteFolderName, bool bSafe, bool bOnce)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = bOnce ? 1 : m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			Reconnect(true);

			SendRequest((KString)TEXT("MKD ") +
							FixSlashes(temp<KString>(UnslashedFolderName(pRemoteFolderName)), TEXT('/')));

			ReceiveReply(bSafe ? - 1 : 2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);
			
			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}

void T_FTP_Connection::DeleteFolder(LPCTSTR pRemoteFolderName, bool bSafe, bool bOnce)
{
	DEBUG_VERIFY_ALLOCATION;

	for(size_t szAttempt = bOnce ? 1 : m_szNAttempts ; ; )
	{
		TEST_BLOCK_BEGIN
		{
			Reconnect(true);

			SendRequest((KString)TEXT("RMD ") +
							FixSlashes(temp<KString>(UnslashedFolderName(pRemoteFolderName)), TEXT('/')));

			ReceiveReply(bSafe ? -1 : 2);
		}
		TEST_BLOCK_KFC_EXCEPTION_HANDLER
		{
			Release(true);
			
			if(!--szAttempt)
				throw;

			CHECK_CUSTOM_TERMINATION(m_hTerminator, m_szRetryDelay);

			continue;
		}
		TEST_BLOCK_END

		break;
	}
}
