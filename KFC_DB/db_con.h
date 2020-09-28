#ifndef db_con_h
#define db_con_h

// --------------
// DB connection
// --------------
class T_DB_Connection
{
private:
    bool m_bAllocated;

    SQLHDBC m_hCon;

    bool m_bConnected;

public:
    T_DB_Connection(bool bAllocate = false);

    T_DB_Connection(LPCTSTR pAliasName,
                    LPCTSTR pLogin,
                    LPCTSTR pPassword);

    ~T_DB_Connection()
        { Release() ;}

    bool IsAllocated() const
        { return m_bAllocated; }

    bool IsConnected() const
        { return m_bConnected; }

    void Release(bool bFromAllocatorException = false);

    void Allocate();

    void Allocate(  LPCTSTR pAliasName,
                    LPCTSTR pLogin,
                    LPCTSTR pPassword);

    void Disconnect();

    void Connect(   LPCTSTR pAliasName,
                    LPCTSTR pLogin,
                    LPCTSTR pPassword);

    SQLHDBC GetCon() const
        { DEBUG_VERIFY_ALLOCATION; return m_hCon; }

    operator SQLHDBC () const
        { return GetCon(); }
};

#endif // db_con_h;
