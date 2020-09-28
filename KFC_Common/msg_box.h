#ifndef msg_box_h
#define msg_box_h

#ifdef _MSC_VER

// ---------------
// Msg box params
// ---------------
struct TMsgBoxParams
{
public:
    HWND    m_hWnd;
    KString m_Text;
    KString m_Caption;
    ktype_t m_tpType;

public:
    TMsgBoxParams(  HWND    hSWnd,
                    LPCTSTR pSText,
                    LPCTSTR pSCaption,
                    ktype_t tpSType) :  m_hWnd      (hSWnd),
                                        m_Text      (pSText),
                                        m_Caption   (pSCaption),
                                        m_tpType    (tpSType) {}

    int Spawn() const;
};

// ----------------
// Global routines
// ----------------
void ThreadedMessageBox(const TMsgBoxParams& Params);

#endif // _MSC_VER

#endif // msg_box_h
