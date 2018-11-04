#ifndef rpc_message_registerer_h
#define rpc_message_registerer_h

#ifdef _MSC_VER

// -----------------------
// RPC message registerer
// -----------------------
class TRPCMessageRegisterer
{
private:
	KString m_Name;

	size_t m_szMessage;

public:
	TRPCMessageRegisterer();

	bool IsAllocated() const
		{ return m_szMessage != UINT_MAX; }

	void Allocate(LPCTSTR pName);
	
	size_t GetMessage() const;

	operator size_t () const { return GetMessage(); }
};

#endif // _MSC_VER

#endif // rpc_message_registerer_h
