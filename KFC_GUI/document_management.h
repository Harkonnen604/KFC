#ifndef document_management_h
#define document_management_h

// --------------------
// Document management
// --------------------
class TDocumentManagement
{
private:
    KString m_UntitledFileName;

    KString m_FileName;

    bool m_bUntitled;

    bool m_bModified;

private:
    void ReportDocumentStateChange()
        { OnDocumentStateChange(m_FileName, m_bUntitled, m_bModified); }

    void MakeNewDocument();

protected:
    // Asks to create new document.
    virtual void OnNewDocument() = 0;

    virtual bool OnLoadDocument(LPCTSTR pFileName, bool bSilent, bool& bRDamaged) = 0;

    virtual bool OnSaveDocument(LPCTSTR pFileName, bool bSilent) = 0;

    virtual int OnConfirmSaveDocument(LPCTSTR pFileName, bool bUntitled) = 0;

    virtual KString OnGetLoadDocumentFileName() = 0;

    virtual KString OnGetSaveDocumentFileName(LPCTSTR pFileName, bool bUntitled) = 0;

    virtual void OnDocumentStateChange
        (LPCTSTR pFileName, bool bUntitled, bool bModified) {}

    virtual void OnDocumentChange() {}

public:
    TDocumentManagement();

    ~TDocumentManagement()
        { Release(); }

    bool IsAllocated() const
        { return !m_UntitledFileName.IsEmpty(); }

    void Release();

    void Allocate(LPCTSTR pUntitledFileName);

    void ModifyDocument(bool bModify = true);

    bool CheckSaveDocument();

    bool NewDocument();

    bool OpenDocument();

    bool ForceOpenDocument(LPCTSTR pFileName, bool bSilent = false);

    bool SaveDocument(bool bSaveAs = false);

    bool SaveDocumentAs()
        { return SaveDocument(true); }

    KString GetDocumentFileName() const
        { DEBUG_VERIFY_ALLOCATION; return m_FileName; }

    bool IsDocumentUntitled() const
        { DEBUG_VERIFY_ALLOCATION; return m_bUntitled; }

    bool IsDocumentModified() const
        { DEBUG_VERIFY_ALLOCATION; return m_bModified; }
};

#endif // document_management_h
