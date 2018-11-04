#ifndef pop3_smtp_h
#define pop3_smtp_h

#include <KFC_Networking/socket.h>

// Ports
#define POP3_PORT	(110)
#define SMTP_PORT	(25)

// -----------------
// POP3 SMTP socket
// -----------------
class T_POP3_SMTP_Socket : public TSocket
{
private:
	TArray<char, true> m_Buffer;

	size_t m_szHead, m_szTail;


	void LoadBuffer(size_t szWrapLength);

	KString ReceiveUntilSubString(LPCTSTR pText, size_t szCutAmt);

public:
	T_POP3_SMTP_Socket(bool bAllocate = false);

	~T_POP3_SMTP_Socket()
		{ Release(); }

	bool IsAllocated() const
		{ return !m_Buffer.IsEmpty(); }
	
	void Release();

	void Allocate();

	// Appends \r\n
	void SendCmd(LPCSTR pCmd);

	// Receives until \r\n
	KString ReceiveSingle();

	// Receives .\r\n or until \r\n.\r\n
	KString ReceiveMulti();

	// Checks for +OK{EOL|SPACE}
	void Verify_POP3_Response(KString* pRPostfix = NULL);
	
	// Checks for 2xx code
	void Verify_SMTP_Response(KString* pRPostfix = NULL);
};

#endif // pop3_smtp_h
