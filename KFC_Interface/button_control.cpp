#include "kfc_interface_pch.h"
#include "button_control.h"

#include <KFC_Graphics\sprite_storage.h>

// -------------------------------
// Button control creation struct
// -------------------------------
TButtonControlCreationStruct::TButtonControlCreationStruct()
{
}

void TButtonControlCreationStruct::Load(TInfoNodeConstIterator  InfoNode,
                                        const TControl*         pParentControl,
                                        const FRECT&            Resolution)
{
    TInteractiveControlCreationStruct::Load(InfoNode, pParentControl, Resolution);
}

// --------------------------------
// Button control sprites provider
// --------------------------------
TButtonControlSpritesProvider::TButtonControlSpritesProvider()
{
}

void TButtonControlSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
    g_SpriteStorage.LoadObject(InfoNode, TEXT("Sprite"), m_Sprite, true);
}

// ---------------------------------------
// Button control sounds provider
// ---------------------------------------
TButtonControlSoundsProvider::TButtonControlSoundsProvider()
{
}

void TButtonControlSoundsProvider::Load(TInfoNodeConstIterator InfoNode)
{
    TInteractiveControlSoundsProvider::Load(InfoNode);
}

KString TButtonControlSpritesProvider::GetText(bool* pRSuccess) const
{
    return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TButtonControlSpritesProvider::SetText(const KString& Text)
{
    return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}

// ---------------
// Button control
// ---------------
TControl* TButtonControl::LoadControl(  type_t                  tpType,
                                        TInfoNodeConstIterator  InfoNode,
                                        const TControl*         pParentControl,
                                        const FRECT&            Resolution)
{
    DEBUG_VERIFY(tpType == CONTROL_TYPE_BUTTON);

    DEBUG_VERIFY(InfoNode.IsValid());

    TButtonControlCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode, pParentControl, Resolution);

    TButtonControlSpritesProvider SpritesProvider;
    SpritesProvider.Load(InfoNode);

    TButtonControlSoundsProvider SoundsProvider;
    SoundsProvider.Load(InfoNode);

    return new TButtonControl(CreationStruct, SpritesProvider, SoundsProvider);
}

TButtonControl::TButtonControl( const TButtonControlCreationStruct& CreationStruct,
                                TButtonControlSpritesProvider&      SpritesProvider,
                                TButtonControlSoundsProvider&       SoundsProvider) :

    TInteractiveControl(CreationStruct, SoundsProvider)
{
    m_Sprite.ReOwn(SpritesProvider.m_Sprite);
}

// Update/render events
void TButtonControl::OnRender() const
{
    TInteractiveControl::OnRender();

    const TSprite* pSprite = m_Sprite.GetDataPtr();

    if(pSprite)
    {
        pSprite->DrawRect(  GetCurrentScreenState().m_Rect. m_OwnPart,
                            GetCurrentScreenState().m_Color.m_OwnPart,
                            GetInterpolatedInteractiveState());
    }
}

KString TButtonControl::GetText(bool* pRSuccess) const
{
    return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TButtonControl::SetText(const KString& Text)
{
    return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}
