#include "kfc_ssl_pch.h"
#include "ssl_socket.h"

#include "ssl_device_globals.h"

// -----------
// SSL Socket
// -----------
T_SSL_Socket::T_SSL_Socket(bool bAllocateClient)
{
	m_pSSL = NULL;

	if(bAllocateClient)
		AllocateClient();
}

void T_SSL_Socket::Release()
{
	if(m_pSSL)
		SSL_free(m_pSSL), m_pSSL = NULL;
}

void T_SSL_Socket::AllocateClient()
{
	Release();

	try
	{
		if(!(m_pSSL = SSL_new(g_SSL_DeviceGlobals.m_pClient_SSL_Context)))
			INITIATE_DEFINED_FAILURE(TEXT("Error preparing client SSL connection."));

		m_Mode = MODE_CLIENT;

		m_Socket = INVALID_SOCKET;

		m_hTerminator = NULL;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void T_SSL_Socket::AllocateServer(	const T_RSA_Key&			RSA_Key,
									const T_X509_Certificate&	X509_Certificate)
{
	Release();

	try
	{
		DEBUG_VERIFY(RSA_Key.IsAllocated());

		DEBUG_VERIFY(X509_Certificate.IsAllocated());

		if(!(m_pSSL = SSL_new(g_SSL_DeviceGlobals.m_pServer_SSL_Context)))
			INITIATE_DEFINED_FAILURE(TEXT("Error preparing server SSL connection."));

		if(SSL_use_certificate(m_pSSL, X509_Certificate) != 1)
			INITIATE_DEFINED_FAILURE(TEXT("Error assigning X509 certificate to server SSL connection."));

		if(SSL_use_RSAPrivateKey(m_pSSL, RSA_Key) != 1)
			INITIATE_DEFINED_FAILURE(TEXT("Error assigning RSA key to server SSL connection."));

		m_Mode = MODE_SERVER;

		m_Socket = INVALID_SOCKET;

		m_hTerminator = NULL;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void T_SSL_Socket::SetNonBlocking(SOCKET Socket)
{
	u_long v = 1;

	if(ioctlsocket(Socket, FIONBIO, &v))
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error switching blocking mode of network socket"),
										WSAGetLastError());
	}
}

void T_SSL_Socket::WaitPending(int r, SOCKET Socket, LPCTSTR pErrorText)
{
	const int e = SSL_get_error(m_pSSL, r);

	switch(e)
	{
	case SSL_ERROR_WANT_READ:
		{
			SOCKET rs = Socket;
			SOCKET es = Socket;

			if(	!WaitForSockets(&rs, 1, NULL, 0, &es, 1, UINT_MAX, m_hTerminator) ||
				!VALID_SOCKET(rs) || VALID_SOCKET(es))
			{
				INITIATE_DEFINED_CODE_FAILURE(pErrorText, 0);
			}
		}

		break;

	case SSL_ERROR_WANT_WRITE:
		{
			SOCKET ws = Socket;
			SOCKET es = Socket;

			if(	!WaitForSockets(NULL, 0, &ws, 1, &es, 1, UINT_MAX, m_hTerminator) ||
				!VALID_SOCKET(ws) || VALID_SOCKET(es))
			{
				INITIATE_DEFINED_CODE_FAILURE(pErrorText, 0);
			}
		}

		break;

	default:
		{
			char buf[128];

			ERR_error_string_n(e, buf, sizeof(buf));

			INITIATE_DEFINED_CODE_FAILURE((KString)pErrorText + TEXT("\r\n(") + buf + TEXT(")"), e);
		}
	}
}

void T_SSL_Socket::Connect(SOCKET Socket)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_Mode == MODE_CLIENT);

	DEBUG_VERIFY(!IsConnected());

	DEBUG_VERIFY(VALID_SOCKET(Socket));	

	SetNonBlocking(Socket);	

	if(SSL_set_fd(m_pSSL, Socket) != 1)
		INITIATE_DEFINED_FAILURE(TEXT("Error assigning socket to client SSL connection."));

	int r;

	while((r = SSL_connect(m_pSSL)) != 1)
		WaitPending(r, Socket, TEXT("Error initiating SSL connection"));

	m_Socket = Socket;
}

void T_SSL_Socket::Accept(SOCKET Socket)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_Mode == MODE_SERVER);

	DEBUG_VERIFY(!IsConnected());

	DEBUG_VERIFY(VALID_SOCKET(Socket));

	SetNonBlocking(Socket);

	if(SSL_set_fd(m_pSSL, Socket) != 1)
		INITIATE_DEFINED_FAILURE(TEXT("Error assignijng socket to server SSL connection."));

	int r;

	while((r = SSL_accept(m_pSSL)) != 1)
		WaitPending(r, Socket, TEXT("Error accepting SSL connection"));

	m_Socket = Socket;
}

void T_SSL_Socket::InitiateShutdown()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsConnected());

	int r;

	for(;;)
	{
		while((r = SSL_shutdown(m_pSSL)) < 0)
			WaitPending(r, m_Socket, TEXT("Error shutting down SSL connection"));

		if(r)
			break;

		CHECK_POLLED_TERMINATION(m_hTerminator);
	}
}

void T_SSL_Socket::ReceiveShutdown()
{
	char c;

	if(ReceiveAvailable(&c, sizeof(c)) > 0)
		INITIATE_DEFINED_FAILURE(TEXT("Data arrived instead of shutdown request on SSL connection."));
}

size_t T_SSL_Socket::ReceiveAvailable(void* pRData, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsConnected());

	DEBUG_VERIFY(szLength == 0 || pRData);

	if(szLength == 0)
		return 0;

	int r;

	while((r = SSL_read(m_pSSL, pRData, szLength)) < 0)
		WaitPending(r, m_Socket, TEXT("Error receiving data from SSL connection"));

	DEBUG_VERIFY((size_t)r <= szLength);

	return (size_t)r;
}

size_t T_SSL_Socket::SendAvailable(const void* pData, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsConnected());

	DEBUG_VERIFY(szLength == 0 || pData);

	if(szLength == 0)
		return 0;

	int r;

	while((r = SSL_write(m_pSSL, pData, szLength)) < 0)
		WaitPending(r, m_Socket, TEXT("Error sending data through SSL connection"));

	DEBUG_VERIFY((size_t)r <= szLength);

	return (size_t)r;
}
