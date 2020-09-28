#ifndef interface_h
#define interface_h

#include <KFC_Common\structured_info.h>
#include <KFC_Common\message.h>
#include "interface_defs.h"
#include "interface_message_defs.h"
#include "control_defs.h"
#include "hot_pointer.h"
#include "control.h"

// ----------
// Interface
// ----------
class TInterface : public TSuspendable
{
private:
    bool m_bAllocated;

    TControls m_Controls;

    TMessageProcessor m_MessageProcessor;


    static void UpdateControlScreenState(TControl* pControl);

    void UpdateControl(TControl* pControl);

    void CallOnInterfaceUpdated(TControl* pControl);

    void RenderControl(const TControl* pControl) const;

    static TControl* InternalFindControl(   size_t      szID,
                                            TControl*   pParentControl,
                                            TControl*   pStartControl,
                                            bool        bRecursive = false);

    static const TControl* InternalFindControl( size_t          szID,
                                                const TControl* pParentControl,
                                                const TControl* pStartControl,
                                                bool            bRecursive = false);

    bool OnSuspend  ();
    bool OnResume   ();

public:
    TInterface();

    ~TInterface()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    // Returns desktop control (the one passed in)s
    TControl* Allocate(TControl* pDesktopControl);

private:
    TControl* MergeControl( TControl* pSrcControl,
                            TControl* pDstParentControl);

public:
    TControl* Merge(TControl* pSrcControl,
                    TControl* pDstParentControl);

private:
    void LoadControl(   TInfoNodeConstIterator  InfoNode,
                        TControl*               pParentControl,
                        const FRECT&            Resolution);

public:
    void Load(  TInfoNodeConstIterator  InfoNode,
                const FRECT&            Resolution);

    void Update();
    void Render() const;

    static void SetControlID(TControl* pControl, size_t szID);

    static void SetControlInitialValues(TControl* pControl);

    static void SuspendControl  (TControl* pControl);
    static void ResumeControl   (TControl* pControl);

    static void SetControlClientState(  TControl*               pControl,
                                        const TControlState&    ClientState,
                                        bool                    bSetInitialValues = false);

    // Deletion
private:
    void PreDeleteControl(TControl* pControl);

public:
    static void DeleteControl(TControl* pControl);

    static void DetachControl(TControl* pControl);

    // Fast handle visibility setting
    static void SetControlCommonVisibility( TControl*   pControl,
                                            float       fVisibility,
                                            bool        bSetInitialValues = false);

    static void SetControlOwnVisibility(TControl*   pControl,
                                        float       fVisibility,
                                        bool        bSetInitialValues = false);

    static void SetControlChildrenVisibility(   TControl*   pControl,
                                                float       fVisibility,
                                                bool        bSetInitialValues = false);

    // Fast ID visibility setting
    void SetControlCommonVisibility(size_t  szID,
                                    float   fVisibility,
                                    bool    bSetInitialValues = false)
    {
        SetControlCommonVisibility(FindControl(szID), fVisibility, bSetInitialValues);
    }

    void SetControlOwnVisibility(   size_t  szID,
                                    float   fVisibility,
                                    bool    bSetInitialValues = false)
    {
        SetControlOwnVisibility(FindControl(szID), fVisibility, bSetInitialValues);
    }

    void SetControlChildrenVisibility(  size_t  szID,
                                        float   fVisibility,
                                        bool    bSetInitialValues = false)
    {
        SetControlChildrenVisibility(FindControl(szID), fVisibility, bSetInitialValues);
    }

    // Fast handle enablement setting
    static void SetControlCommonEnablement( TControl*   pControl,
                                            float       fEnablement,
                                            bool        bSetInitialValues = false);

    static void SetControlOwnEnablement(TControl*   pControl,
                                        float       fEnablement,
                                        bool        bSetInitialValues = false);


    static void SetControlChildrenEnablement(   TControl*   pControl,
                                                float       fEnablement,
                                                bool        bSetInitialValues = false);


    // Fast ID enablement setting
    void SetControlCommonEnablement(size_t  szID,
                                    float   fEnablement,
                                    bool    bSetInitialValues = false)

    {
        SetControlCommonEnablement(FindControl(szID), fEnablement, bSetInitialValues);
    }

    void SetControlOwnEnablement(   size_t  szID,
                                    float   fEnablement,
                                    bool    bSetInitialValues = false)

    {
        SetControlOwnEnablement(FindControl(szID), fEnablement, bSetInitialValues);
    }

    void SetControlChildrenEnablement(  size_t  szID,
                                        float   fEnablement,
                                        bool    bSetInitialValues = false)

    {
        SetControlChildrenEnablement(FindControl(szID), fEnablement, bSetInitialValues);
    }

    // Fash handle text getting
    static KString GetControlText(  const TControl* pControl,
                                    bool*           pRSuccess = NULL);

    // Fast ID text getting
    KString GetControlText( size_t  szID,
                            bool*   pRSuccess = NULL) const
    {
        return GetControlText(FindControl(szID), pRSuccess);
    }

    // Fast handle text setting
    static bool SetControlText( TControl*       pControl,
                                const KString&  Text,
                                bool            bSetInitialValues = false);

    // Fast ID text setting
    bool SetControlText(size_t          szID,
                        const KString&  Text,
                        bool            bSetInitialValues = false)
    {
        return SetControlText(FindControl(szID), Text, bSetInitialValues);
    }

    // Fast handle check getting
    static bool GetControlCheck(const TControl* pControlIter,
                                bool*           pRSuccess = NULL);

    // Fast ID check getting
    bool GetControlCheck(   size_t  szID,
                            bool*   pRSuccess = NULL) const
    {
        return GetControlCheck(FindControl(szID), pRSuccess);
    }

    // Fast handle check setting
    static bool SetControlCheck(TControl*   pControl,
                                bool        bCheck,
                                bool        bSetInitialValues = false);

    // Fast ID check setting
    bool SetControlCheck(   size_t  szID,
                            bool    bCheck,
                            bool    bSetInitialValues = false)
    {
        return SetControlCheck(FindControl(szID), bCheck, bSetInitialValues);
    }

    // Messages accessing
    TMessageProcessor& GetMessageProcessor();

    const TMessageProcessor& GetMessageProcessor() const;

    TMessageIterator GetFirstMessage() const;

    // Addition
    TControl* AddChildControl(  TControl* pControl,
                                TControl* pParentControl,
                                TControl* pPrevControl,
                                TControl* pNextControl)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(pControl);

        TControls::TIterator Iter =
            m_Controls.Add( pParentControl->m_Iter,
                            pPrevControl ? pPrevControl->m_Iter : NULL,
                            pNextControl ? pNextControl->m_Iter : NULL);

        return &(*Iter = pControl)->Allocate(*this, Iter);
    }

    TControl* AddFirstChildControl( TControl* pControl,
                                    TControl* pParentControl)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(pControl);

        TControls::TIterator Iter =
            m_Controls.AddFirstChild(pParentControl->m_Iter);

        return &(*Iter = pControl)->Allocate(*this, Iter);
    }

    TControl* AddLastChildControl(  TControl* pControl,
                                    TControl* pParentControl)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(pControl);

        TControls::TIterator Iter =
            m_Controls.AddLastChild(pParentControl->m_Iter);

        return &(*Iter = pControl)->Allocate(*this, Iter);
    }

    // Enumeration
    TControl*       GetDesktopControl();
    const TControl* GetDesktopControl() const;

    TControl* FindControl(  size_t      szID,
                            TControl*   pParentControl  = NULL,
                            TControl*   pStartControl   = NULL,
                            bool        bRecursive      = true);

    const TControl* FindControl(size_t          szID,
                                const TControl* pParentControl  = NULL,
                                const TControl* pStartControl   = NULL,
                                bool            bRecursive      = true) const;

    TControl* GetControl(   size_t      szID,
                            TControl*   pParentControl  = NULL,
                            TControl*   pStartControl   = NULL,
                            bool        bRecursive      = true);

    const TControl* GetControl( size_t          szID,
                                const TControl* pParentControl  = NULL,
                                const TControl* pStartControl   = NULL,
                                bool            bRecursive      = true) const;
};

// ----------------
// Controls merger
// ----------------
class TControlsMerger
{
private:
    TControl* m_pControl;

public:
    TControlsMerger();

    ~TControlsMerger()
        { Release(); }

    bool IsAllocated() const
        { return m_pControl; }

    void Release();

    TControl* Allocate( TControl* pSrcControl,
                        TControl* pDstParentControl);
};

#endif // interface_h
