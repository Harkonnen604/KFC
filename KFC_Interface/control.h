#ifndef control_h
#define control_h

#include <KFC_Common\structured_info.h>
#include <KFC_Common\message.h>
#include "interface_defs.h"
#include "interface_device_globals.h"
#include "control_defs.h"
#include "interface_message_defs.h"

// ---------------------------
// Control combined parameter
// ---------------------------
template <class ObjectType>
struct TControlCombinedParameter
{
    ObjectType m_CommonPart;
    ObjectType m_OwnPart;
    ObjectType m_ChildrenPart;


    TControlCombinedParameter() {}

    TControlCombinedParameter(  const ObjectType& SComnnonPart,
                                const ObjectType& SOwnPart,
                                const ObjectType& SChildrenPart) :

        m_CommonPart    (SComnnonPart),
        m_OwnPart       (SOwnPart),
        m_ChildrenPart  (SChildrenPart)


    TControlCombinedParameter& operator -= (const TControlCombinedParameter& SParameter)
    {
        m_CommonPart    -= SParameter.m_CommonPart;
        m_OwnPart       -= SParameter.m_OwnPart;
        m_ChildrenPart  -= SParameter.m_ChildrenPart;

        return *this;
    }

    TControlCombinedParameter& operator += (const TControlCombinedParameter& SColor)
    {
        m_CommonPart    -= SParameter.m_CommonPart;
        m_OwnPart       -= SParameter.m_OwnPart;
        m_ChildrenPart  -= SParameter.m_ChildrenPart;

        return *this;
    }

    TControlCombinedParameter& operator *= (float fValue)
    {
        m_CommonPart    *= fValue;
        m_OwnPart       *= fValue;
        m_ChildrenPart  *= fValue;

        return *this;
    }

    TControlCombinedParameter& operator /= (float fValue)
    {
        return *this *= (1.0f / fValue);
    }

    bool operator == (const TControlCombinedParameter& SParameter) const
    {
        return  m_CommonPart    == SParameter.m_CommonPart  &&
                m_OwnPart       == SParameter.m_OwnPart     &&
                m_ChildrenPart  == SParameter.m_ChidrenPart;
    }

    bool operator != (const TControlCombinedParameter& SColor) const
    {
        return  m_CommonPart    != SParameter.m_CommonPart  ||
                m_OwnPart       != SParameter.m_OwnPart     ||
                m_ChildrenPart  != SParameter.m_ChidrenPart;
    }
};

// --------------
// Control state
// --------------
struct TControlState
{
    TControlCombinedParameter<FRECT>        m_Rect;
    FRECT                                   m_HoverRect;        // relative to own rect
    FRECT                                   m_HotPointerRect;   // relative to own rect
    TControlCombinedParameter<TD3DColor>    m_Color;
    TControlCombinedParameter<float>        m_Visibility;
    TControlCombinedParameter<float>        m_Enablement;


    void Reset();

    void SetScreenState(const TControlState* pParentScreenState,
                        const TControlState& ClientState);

    bool IsClientlyVisible() const
        { return Compare(m_Visibility.m_CommonPart * m_Visibility.m_OwnPart, 1.0f) == 0; }

    bool IsClientlyEnabled() const
        { return Compare(m_Enablement.m_CommonPart * m_Enablement.m_OwnPart, 1.0f) == 0; }

    bool IsVisible() const { return Compare(m_Visibility.m_OwnPart, 1.0f) == 0; }
    bool IsEnabled() const { return Compare(m_Enablement.m_OwnPart, 1.0f) == 0; }

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);
};

// --------------------
// Control input state
// --------------------
struct TControlInputState
{
    FPOINT  m_MouseCoords;
    bool    m_bMouseOwner;
    bool    m_bHovered;


    TControlInputState() {};

    void Update(TControl* pControl);
};

// ------------------------
// Control creation struct
// ------------------------
struct TControlCreationStruct
{
    TControlState           m_ClientState;
    size_t                  m_szID;
    THotPointerDirection    m_HotPointerDirection;
    bool                    m_bSuspendOnHide;


    TControlCreationStruct();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);
};

// --------
// Control
// --------
class TControl : public TSuspendable
{
private:
    TControlState       m_PrevClientState;
    TControlState       m_PrevScreenState;
    TControlInputState  m_PrevInputState;

    TControlState       m_CurrentClientState;
    TControlState       m_CurrentScreenState;
    TControlInputState  m_CurrentInputState;

    size_t  m_szID;
    bool    m_bSuspendOnHide;

    TInterface*             m_pInterface;
    TControls::TIterator    m_Iter;

protected:

    // Allocation
    virtual void OnAllocate();

    virtual void OnSetInitialValues();

    virtual void OnSetID() {}

    // Appearance events
    virtual void OnShow();

    virtual void OnHide();

    virtual void OnEnable();

    virtual void OnDisable();

    // Mouse events
    virtual void OnMouseButtonDown(size_t szButton);

    virtual void OnMouseButtonUp(size_t szButton);

    virtual void OnMouseMove();

    virtual void OnMouseEnter();

    virtual void OnMouseLeave();

    virtual void OnMouseCaptureLost();

    // Keyboard events
    virtual void OnKeyboardKeyDown(size_t szKey);

    virtual void OnKeyboardKeyUp(size_t szKey);

    virtual void OnFocusLost();

    virtual void OnFocusReceived();

    // Update/render events
    virtual void OnPreUpdate() {}

    virtual void OnPostUpdate() {}

    virtual void OnInterfaceUpdated() {}

    virtual void OnRender() const {}

    // Internal methods
    TControl& Allocate( TInterface&             SInterface,
                        TControls::TIterator    SIter); // returns reference to self

    void SetInitialValues();

    void Update();

    void Render() const;

public:
    THotPointerDirection m_HotPointerDirection;


    TControl(const TControlCreationStruct& CreationStruct);

    virtual ~TControl() {}

    virtual TControl* CreateCopy() const { return NULL; } // {{{

    void SetID(size_t szSID);

    // ---------------- TRIVIALS ----------------
    size_t GetID() const { return m_szID; }

    const TControlState&        GetPrevClientState  () const { return m_PrevClientState;    }
    const TControlState&        GetPrevScreenState  () const { return m_PrevScreenState;    }
    const TControlInputState&   GetPrevInputState   () const { return m_PrevInputState;     }

    const TControlState&        GetCurrentClientState   () const { return m_CurrentClientState; }
    const TControlState&        GetCurrentScreenState   () const { return m_CurrentScreenState; }
    const TControlInputState&   GetCurrentInputState    () const { return m_CurrentInputState;  }

    // Persistence access
    TControls::TIterator GetIter();

    TControls::TConstIterator GetIter() const;

    TInterface& GetInterface();

    const TInterface& GetInterface() const;

    // Messages
    virtual bool IsValidMessage(const TControlMessage* pMessage) const;

    // Message friends
    friend class TInterface;                // all events, handle access
    friend class TInterfaceDeviceGlobals;   // mouse capture loss event
};

// ----------------
// Global routines
// ----------------

// Common helpers
const FRECT& GetControlChildrenScreenRect(const TControl* pControl);

// Point helpers
FPOINT& ReadPointResized(   const KString&  String,
                            FPOINT&         RPoint,
                            LPCTSTR         pValueName,
                            const TControl* pParentControl,
                                            const FRECT&    Resolution);

// Rect helpers
FRECT& ReadRectResized( const KString&  String,
                        FRECT&          RRect,
                        LPCTSTR         pValueName,
                        const TControl* pParentControl,
                        const FRECT&    Resolution);

// Size helpers
FSIZE& ReadSizeResized( const KString&  String,
                        FSIZE&          RSize,
                        LPCTSTR         pValueName,
                        const TControl* pParentControl,
                        const FRECT&    Resolution);

#endif // control_h
