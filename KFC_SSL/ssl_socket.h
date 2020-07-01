#ifndef ssl_socket_h
#define ssl_socket_h

#include <KFC_Networking\socket.h>
#include "ssl_atoms.h"

// -----------
// SSL socket
// -----------
class T_SSL_Socket : public TSocketBase
{
public:
	// Mode
	enum TMode { MODE_CLIENT, MODE_SERVER };

private:
	SSL* m_pSSL;

	TMode m_Mode;

	SOCKET m_Socket;

	HANDLE m_hTerminator;


	static void SetNonBlocking(SOCKET Socket);

	void WaitPending(int r, SOCKET Socket, LPCTSTR pErrorText);

public:
	T_SSL_Socket(bool bAllocateClient = false);

	~T_SSL_Socket()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pSSL; }

	void Release();

	void AllocateClient();

	void AllocateServer(const T_RSA_Key&			RSA_Key,
						const T_X509_Certificate&	X509_Certificate);

	HANDLE GetTerminator() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hTerminator; }

	void SetTerminator(HANDLE hTerminator)
		{ DEBUG_VERIFY_ALLOCATION; m_hTerminator = hTerminator; }

	void Connect(SOCKET Socket);

	void Accept(SOCKET Socket);
	
	void InitiateShutdown();

	void ReceiveShutdown();

	size_t ReceiveAvailable(void* pRData, size_t szLength);

	size_t SendAvailable(const void* pData, size_t szLength);

	SSL* GetSSL() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pSSL; }

	operator SSL* () const
		{ return GetSSL(); }

	TMode GetMode() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Mode; }	

	bool IsConnected() const
		{ DEBUG_VERIFY_ALLOCATION; return VALID_SOCKET(m_Socket); }
};

#endif // ssl_socket_h