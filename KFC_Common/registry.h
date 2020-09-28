#ifndef registry_h
#define registry_h

#ifdef _MSC_VER

// -------------
// Registry key
// -------------
class KRegistryKey
{
private:
    HKEY m_hParentKey;

    KString m_Name;

    HKEY m_hKey;

public:
    KRegistryKey();

    KRegistryKey(   HKEY            hSParentKey,
                    const KString&  SName);

    KRegistryKey(   HKEY            hSParentKey,
                    const KString&  SName,
                    REGSAM          rsAccess);

    ~KRegistryKey() { Release(); }

    bool IsAllocated() const
        { return m_hParentKey; }

    void Release();

    void Allocate(  HKEY            hSParentKey,
                    const KString&  SName);

    void Allocate(  HKEY            hSParentKey,
                    const KString&  SName,
                    REGSAM          rsAccess);

    bool IsOpen() const { return m_hKey ? true : false; }

    void Close();

    KRegistryKey& Open(REGSAM rsAccess);

    bool Create(REGSAM rsAccess, kflags_t flOptions, bool bAutoOpen); // returns 'true' if new key was created, 'false' otherwise

    HKEY GetKey() const;

    operator HKEY() const { return GetKey(); }

    INT64 ReadInt64(const KString& ValueName) const
    {
        INT64 iValue;
        return ReadInt64(ValueName, iValue), iValue;
    }

    QWORD ReadQWORD(const KString& ValueName) const
    {
        QWORD qwValue;
        return ReadQWORD(ValueName, qwValue), qwValue;
    }

    int ReadInt(const KString& ValueName) const
    {
        int iValue;
        return ReadInt(ValueName, iValue), iValue;
    }

    UINT ReadUINT(const KString& ValueName) const
    {
        UINT uiValue;
        return ReadUINT(ValueName, uiValue), uiValue;
    }

    KString ReadString(const KString& ValueName) const
    {
        KString Value;
        return ReadString(ValueName, Value), Value;
    }

    bool ReadBool(const KString& ValueName) const
    {
        bool bValue;
        return ReadBool(ValueName, bValue), bValue;
    }

    GUID ReadGUID(const KString& ValueName) const
    {
        GUID Value;
        return ReadGUID(ValueName, Value), Value;
    }

    float ReadFloat(const KString& ValueName) const
    {
        float fValue;
        return ReadFloat(ValueName, fValue), fValue;
    }

    double ReadDouble(const KString& ValueName) const
    {
        double dValue;
        return ReadDouble(ValueName, dValue), dValue;
    }

    void ReadInt64  (const KString& ValueName, INT64&   iRValue)    const;
    void ReadQWORD  (const KString& ValueName, QWORD&   qwRValue)   const;
    void ReadInt    (const KString& ValueName, int&     iRValue)    const;
    void ReadUINT   (const KString& ValueName, UINT&    uiRValue)   const;
    void ReadString (const KString& ValueName, KString& RValue)     const;
    void ReadBool   (const KString& ValueName, bool&    bRValue)    const;
    void ReadGUID   (const KString& ValueName, GUID&    RValue)     const;
    void ReadFloat  (const KString& ValueName, float&   fRValue)    const;
    void ReadDouble (const KString& ValueName, double&  dRValue)    const;

    void ReadInt64  (const KString& ValueName, INT64&   iRValue,    const INT64     iDefaultValue)  const;
    void ReadQWORD  (const KString& ValueName, QWORD&   qwRValue,   const QWORD     qwDefaultValue) const;
    void ReadInt    (const KString& ValueName, int&     iRValue,    const int       iDefaultValue)  const;
    void ReadUINT   (const KString& ValueName, UINT&    uiRValue,   const UINT      uiDefaultValue) const;
    void ReadString (const KString& ValueName, KString& RValue,     const KString&  DefaultValue)   const;
    void ReadBool   (const KString& ValueName, bool&    bRValue,    const bool      bDefaultValue)  const;
    void ReadGUID   (const KString& ValueName, GUID&    RValue,     const GUID&     DefaultValue)   const;
    void ReadFloat  (const KString& ValueName, float&   fRValue,    const float     fDefaultValue)  const;
    void ReadDouble (const KString& ValueName, double&  dRValue,    const double    dDefaultValue)  const;

    void WriteInt64 (const KString& ValueName, const INT64      iValue);
    void WriteQWORD (const KString& ValueName, const QWORD      qwValue);
    void WriteInt   (const KString& ValueName, const int        iValue);
    void WriteUINT  (const KString& ValueName, const UINT       uiValue);
    void WriteString(const KString& ValueName, const KString&   Value);
    void WriteBool  (const KString& ValueName, const bool       bValue);
    void WriteGUID  (const KString& ValueName, const GUID&      Value);
    void WriteFloat (const KString& ValueName, const float      fValue);
    void WriteDouble(const KString& ValueName, const double     dValue);

    DWORD GetValueType(const KString& ValueName) const;

    bool HasKey     (const KString& KeyName)    const;
    bool HasValue   (const KString& ValueName)  const;

    void EnlistKeys     (KStrings& RKeys)   const;
    void EnlistValues   (KStrings& RValues) const;

    bool DeleteValue(const KString& ValueName, bool bSafe);

    bool Delete(bool bRecursive, bool bSafe);
};

template <class t>
void ReadViaString(KRegistryKey& Key, const KString& ValueName, t& RValue)
{
    KString String;

    Key.ReadString(ValueName, String);

    if(!FromString(String, RValue))
    {
        INITIATE_DEFINED_FAILURE(   (KString)TEXT("Registry value \"") +
                                        ValueName +
                                        TEXT("\" has wrong format."));
    }
}

template <class t>
void ReadViaString(KRegistryKey& Key, const KString& ValueName, t& RValue, const t& DefaultValue)
{
    try
    {
        ReadViaString(Key, ValueName, RValue);
    }

    catch(...)
    {
        RValue = DefaultValue;
    }
}

template <class t>
void WriteViaString(KRegistryKey& Key, const KString& ValueName, const t& Value)
{
    Key.WriteString(ValueName, ToString(Value));
}

// --------------------
// Registry key opener
// --------------------
class KRegistryKeyOpener
{
private:
    KRegistryKey& m_Key;

public:
    KRegistryKeyOpener( KRegistryKey&   SKey,
                        REGSAM          rsAccess);

    ~KRegistryKeyOpener();

    // --------------- TRIVIALS ------------------
    KRegistryKey&       GetKey()        { return m_Key; }
    const KRegistryKey& GetKey() const  { return m_Key; }
};

// ----------------
// Global routines
// ----------------

// Safety does not affect formatting errors
void ProcessMultiReg(LPCTSTR pText, bool bSafe, bool bUnroll);

// May throw due to formatting errors
void UnprocessMultiReg(LPCTSTR pText, bool bForceKeyDeletion);

#endif // _MSC_VER

#endif // registry_h
