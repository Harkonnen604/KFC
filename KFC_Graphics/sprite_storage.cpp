#include "kfc_graphics_pch.h"
#include "sprite_storage.h"

#include "graphics_consts.h"
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_tokens.h"
#include "graphics_device_globals.h"
#include "2d_sprites.h"
#include "transition_sprite.h"
#include "composite_sprite.h"
#include "border_sprite.h"
#include "effect_sprites.h"
#include "text_sprite.h"
#include "dummy_sprites.h"

TSpriteStorage g_SpriteStorage;

// ---------------
// Sprite storage
// ---------------
TSpriteStorage::TSpriteStorage() : TGlobals(TEXT("Sprite storage"))
{
    AddSubGlobals(g_GraphicsCfg);
    AddSubGlobals(g_GraphicsInitials);
    AddSubGlobals(g_GraphicsTokens);
    AddSubGlobals(g_GraphicsDeviceGlobals);
}

void TSpriteStorage::OnUninitialize()
{
    m_SystemSpriteTypesRegisterer.Release();

    TStorage<TSprite>::Release();
}

void TSpriteStorage::OnInitialize()
{
    // Storage
    try
    {
        TStorage<TSprite>::Allocate(SPRITE_TYPE_TOKENS,
                                    g_GraphicsConsts.m_szNSpritesRegistrationManagerFixedEntries,
                                    SPRITE_INDEX_TOKENS);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error allocating sprites storage."));
    }

    // System sprite types
    try
    {
        m_SystemSpriteTypesRegisterer.Allocate(SPRITES_FACTORY);

        m_SystemSpriteTypesRegisterer.Add(TTextureImageSprite:: Create, SPRITE_TYPE_FLAT);
        m_SystemSpriteTypesRegisterer.Add(TTransitionSprite::   Create, SPRITE_TYPE_TRANSITION);
        m_SystemSpriteTypesRegisterer.Add(TCompositeSprite::    Create, SPRITE_TYPE_COMPOSITE);
        m_SystemSpriteTypesRegisterer.Add(TBorderSprite::       Create, SPRITE_TYPE_BORDER);
        m_SystemSpriteTypesRegisterer.Add(TRectEffectSprite::   Create, SPRITE_TYPE_RECT_EFFECT);
        m_SystemSpriteTypesRegisterer.Add(TTextSprite::         Create, SPRITE_TYPE_TEXT);
        m_SystemSpriteTypesRegisterer.Add(TDummySprite::        Create, SPRITE_TYPE_DUMMY);
        m_SystemSpriteTypesRegisterer.Add(TStateSkipperSprite:: Create, SPRITE_TYPE_STATE_SKIPPER);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error registering system sprite types."));
    }

    // System sprites
    try
    {
        TStructuredInfo Info(FILENAME_TOKENS.
            Process(TEXT("[StartFolder][DefinitionsFolder]System.Definition")));

        TInfoNodeConstIterator InfoNode =
            Info.GetNode(Info.GetRootNode(), TEXT("Sprites"));

        TObjectPointer<TSprite> Sprite;
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error loading system sprites."));
    }
}

void TSpriteStorage::LoadByDirectValue( const KString&              FileName,
                                        TObjectPointer<TSprite>&    RObject,
                                        bool                        bOmittable)
{
    TTextureImageSpriteCreationStruct CreationStruct;

    if(!CreationStruct.SetFileName(FileName))
    {
        if(!bOmittable)
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Invalid direct value sprite filename \"") +
                                            FileName +
                                            TEXT("\"."));
        }
    }
    else
    {
        ((TTextureImageSprite*)RObject.
            Allocate(new TTextureImageSprite, false))->
                Allocate(CreationStruct);
    }
}
