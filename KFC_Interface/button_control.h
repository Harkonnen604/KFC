#ifndef button_control_h
#define button_control_h

#include <KFC_KTL\text_container.h>
#include <KFC_Graphics\sprite.h>
#include <KFC_Graphics\sprite_helpers.h>
#include "interactive_control.h"

// -------------------------------
// Button control creation struct
// -------------------------------
struct TButtonControlCreationStruct : public TInteractiveControlCreationStruct
{
    TButtonControlCreationStruct();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);
};

// --------------------------------
// Button control sprites provider
// --------------------------------
struct TButtonControlSpritesProvider : public TTextContainer
{
    TObjectPointer<TSprite> m_Sprite;


    TButtonControlSpritesProvider();

    void Load(TInfoNodeConstIterator InfoNode);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);
};

// -------------------------------
// Button control sounds provider
// -------------------------------
struct TButtonControlSoundsProvider :
    public TInteractiveControlSoundsProvider
{
    TButtonControlSoundsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// ---------------
// Button control
// ---------------
class TButtonControl :  public TInteractiveControl,
                        public TTextContainer
{
protected:
    // Update/render events
    virtual void OnRender() const;

public:
    TObjectPointer<TSprite> m_Sprite;


    static TControl* LoadControl(   type_t                  tpType,
                                    TInfoNodeConstIterator  InfoNode,
                                    const TControl*         pParentControl,
                                    const FRECT&            Resolution);

    TButtonControl( const TButtonControlCreationStruct&     CreationStruct,
                    TButtonControlSpritesProvider&          SpritesProvider,
                    TButtonControlSoundsProvider&           SoundsProvider);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);
};

#endif // button_control_h
