#ifndef db_res_h
#define db_res_h

// DB result getter def
#define DECLARE_DB_RESULT_GETTER(Type, Name)            \
    bool Get##Name(size_t szCol, Type& RValue) const    \

// DB result default getter def
#define DECLARE_DB_RESULT_DEFAULT_GETTER(Type, DefType, Name)               \
    void Get##Name(size_t szCol, Type& RValue, DefType DefaultValue) const  \

// ----------
// DB result
// ----------
class T_DB_Result
{
private:
    bool m_bAllocated;

    bool m_bHasData;

    HSTMT m_hStmt;


    void Fetch();

public:
    T_DB_Result();

    T_DB_Result(SQLHSTMT hSStmt);

    ~T_DB_Result()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(SQLHSTMT hSStmt);

    bool GetData(   size_t  szCol,
                    type_t  tpType,
                    void*   pRData,
                    size_t  szSize) const;

    // Getters
    DECLARE_DB_RESULT_GETTER(__int64,   Int64);
    DECLARE_DB_RESULT_GETTER(int,       Int);
    DECLARE_DB_RESULT_GETTER(long,      Long);
    DECLARE_DB_RESULT_GETTER(short,     Short);
    DECLARE_DB_RESULT_GETTER(char,      Char);

    DECLARE_DB_RESULT_GETTER(QWORD, QWORD);
    DECLARE_DB_RESULT_GETTER(UINT,  UINT);
    DECLARE_DB_RESULT_GETTER(DWORD, DWORD);
    DECLARE_DB_RESULT_GETTER(WORD,  WORD);
    DECLARE_DB_RESULT_GETTER(BYTE,  BYTE);

    DECLARE_DB_RESULT_GETTER(double,    Double);
    DECLARE_DB_RESULT_GETTER(float,     Float);

    DECLARE_DB_RESULT_GETTER(KString, String);

    // Default getters
    DECLARE_DB_RESULT_DEFAULT_GETTER(__int64,   __int64,    Int64);
    DECLARE_DB_RESULT_DEFAULT_GETTER(int,       int,        Int);
    DECLARE_DB_RESULT_DEFAULT_GETTER(long,      long,       Long);
    DECLARE_DB_RESULT_DEFAULT_GETTER(short,     short,      Short);
    DECLARE_DB_RESULT_DEFAULT_GETTER(char,      char,       Char);

    DECLARE_DB_RESULT_DEFAULT_GETTER(QWORD, QWORD,  QWORD);
    DECLARE_DB_RESULT_DEFAULT_GETTER(UINT,  UINT,   UINT);
    DECLARE_DB_RESULT_DEFAULT_GETTER(DWORD, DWORD,  DWORD);
    DECLARE_DB_RESULT_DEFAULT_GETTER(WORD,  WORD,   WORD);
    DECLARE_DB_RESULT_DEFAULT_GETTER(BYTE,  BYTE,   BYTE);

    DECLARE_DB_RESULT_DEFAULT_GETTER(double,    double, Double);
    DECLARE_DB_RESULT_DEFAULT_GETTER(float,     float,  Float);

    DECLARE_DB_RESULT_DEFAULT_GETTER(KString, LPCTSTR, String);

    KString GetString(size_t szCol);

    KString operator [] (size_t szCol)
        { return GetString(szCol); }

    void GetRow(TArray<KString>& RValues);

    // Other methods
    T_DB_Result& ToNext();

    T_DB_Result& operator ++ () { return ToNext(); }

    bool HasData() const;

    operator bool() const
        { return HasData(); }

    HSTMT GetStmt() const
        { DEBUG_VERIFY_ALLOCATION; return m_hStmt; }
};

#undef DECLARE_DB_RESULT_GETTER

#endif // db_res_h
