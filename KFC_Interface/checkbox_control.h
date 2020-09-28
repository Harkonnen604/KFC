#ifndef checkbox_control_h
#define checkbox_control_h

#include <KFC_KTL\text_container.h>
#include <KFC_Graphics\sprite.h>
#include <KFC_Graphics\text_sprite.h>
#include <KFC_Sound\sound.h>
#include "interactive_control.h"
#include "check_container.h"
#include "control_interpolated_state.h"

// ---------------------------------
// Checkbox control creation struct
// ---------------------------------
struct TCheckBoxControlCreationStruct :

    public TInteractiveControlCreationStruct,
    public TCheckContainer
{
    bool m_bChecked;

    bool m_bPushClickCheckToggling;

    float m_fCheckTransitionDelay;


    TCheckBoxControlCreationStruct();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);

    void SetImmediateCheckTransition();

    bool GetCheck(bool* pRSuccess = NULL) const;

    bool SetCheck(bool bCheck);
};

// ----------------------------------
// Checkbox control sprites provider
// ----------------------------------
struct TCheckBoxControlSpritesProvider : public TTextContainer
{
    TObjectPointer<TSprite> m_Sprite;


    TCheckBoxControlSpritesProvider();

    void Load(TInfoNodeConstIterator InfoNode);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);
};

// ---------------------------------
// Checkbox control sounds provider
// ---------------------------------
struct TCheckBoxControlSoundsProvider :
    public TInteractiveControlSoundsProvider
{
    TCheckBoxControlSoundsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -----------------
// Checkbox control
// -----------------
class TCheckBoxControl :    public TInteractiveControl,
                            public TTextContainer,
                            public TCheckContainer
{
public:
    // Check state
    enum TCheckState
    {
        CS_NONE         = 0,
        CS_CHECKED      = 1,
        CS_FORCE_UINT   = UINT_MAX,
    };

protected:
    // Allocation
    void OnAllocate();

    void OnSetInitialValues();

    // Suspension
    bool OnResume   ();
    bool OnSuspend  ();

    // Update/render events
    void OnPreUpdate();

    void OnRender() const;

    // Click events
    virtual void OnClick(bool bFromMouse);

    virtual void OnPushClick(bool bFromMouse);

    // Check events
    virtual void OnCheck(bool bFromMouse);

private:
    TControlInterpolatedState<TCheckState, 2> m_CheckState;

    bool m_bChecked;


    // Check methods
    void Check(bool bFromMouse);

public:
    bool m_bPushClickCheckToggling;

    float m_fCheckTransitionDelay;

    TObjectPointer<TSprite> m_Sprite;


    static TControl* LoadControl(   type_t                  tpType,
                                    TInfoNodeConstIterator  InfoNode,
                                    const TControl*         pParentControl,
                                    const FRECT&            Resolution);

    TCheckBoxControl(   const TCheckBoxControlCreationStruct&   CreationStruct,
                        TCheckBoxControlSpritesProvider&        SpritesProvider,
                        TCheckBoxControlSoundsProvider&         SoundsProvider);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);

    bool GetCheck(bool* pRSuccess = NULL) const;

    bool SetCheck(bool bCheck);

    TCheckState GetCheckState() const
        { return m_CheckState; }

    float GetInterpolatedCheckState() const
        { return m_CheckState.GetInterpolatedState(); }

    bool IsChecked() const
        { return GetCheckState() == CS_CHECKED; }
};

// --------------------------------------
// Easy checkbox control creation struct
// --------------------------------------
struct TEasyCheckBoxControlCreationStruct : TCheckBoxControlCreationStruct,
                                            TTextSpriteCreationStruct
{
    float m_fTextOffset;

    TD3DColor m_TextColors[2][4]; // check x interactive


    TEasyCheckBoxControlCreationStruct();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentConrol,
                const FRECT&            Resolution);

    void SetTextColors( size_t              szIndex,
                        const TD3DColor*    pSTextColors);

    void SetTextColors(const TD3DColor STextColors[2][4]);
};

// ---------------------------------------
// Easy checkbox control sprites provider
// ---------------------------------------
struct TEasyCheckBoxControlSpritesProvider : TCheckBoxControlSpritesProvider
{
    TEasyCheckBoxControlSpritesProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -------------------------------------
// Easy checkbox control fonts provider
// -------------------------------------
struct TEasyCheckBoxControlFontsProvider : TTextSpriteFontsProvider
{
    TEasyCheckBoxControlFontsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// --------------------------------------
// Easy checkbox control sounds provider
// --------------------------------------
struct TEasyCheckBoxControlSoundsProvider : TCheckBoxControlSoundsProvider
{
    TEasyCheckBoxControlSoundsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// ----------------------
// Easy checkbox control
// ----------------------
struct TEasyCheckBoxControl : public TCheckBoxControl
{
public:
    static TControl* LoadControl(   type_t                  tpType,
                                    TInfoNodeConstIterator  InfoNode,
                                    const TControl*         pParentControl,
                                    const FRECT&            Resolution);

    TEasyCheckBoxControl(   const TEasyCheckBoxControlCreationStruct&   CreationStruct,
                            TEasyCheckBoxControlSpritesProvider&        SpritesProvider,
                            TEasyCheckBoxControlFontsProvider&          FontsProvider,
                            TEasyCheckBoxControlSoundsProvider&         SoundsProvider);
};

#endif // checkbox_control_h
