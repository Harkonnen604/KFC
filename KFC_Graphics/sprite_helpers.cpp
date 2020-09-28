#include "kfc_graphics_pch.h"
#include "sprite_helpers.h"

#include <KFC_KTL\text_container.h>
#include "sprite.h"

// ----------------
// Global routines
// ----------------
KString GetSpriteText(  const TSprite*  pSprite,
                        bool*           pRSuccess,
                        bool            bRecursive)
{
    if(pRSuccess)
        *pRSuccess = false;

    if(pSprite == NULL)
        return KString();

    // Direct
    const TTextContainer* pTextContainer =
        dynamic_cast<const TTextContainer*>(pSprite);

    if(pTextContainer)
    {
        bool bSuccess;

        const KString Text = pTextContainer->GetText(&bSuccess);

        if(bSuccess)
        {
            if(pRSuccess)
                *pRSuccess = true;

            return Text;
        }
    }

    // Recursive
    if(bRecursive)
    {
        for(size_t i = 0 ; i < pSprite->GetNSubObjects() ; i++)
        {
            bool bSuccess;

            const KString Text = GetSpriteText( pSprite->GetSubObject(i),
                                                &bSuccess,
                                                bRecursive);

            if(bSuccess)
            {
                if(pRSuccess)
                    *pRSuccess = true;

                return Text;
            }
        }
    }

    return KString();
}

bool SetSpriteText( TSprite*        pSprite,
                    const KString&  Text,
                    bool            bRecursive)
{
    if(pSprite == NULL)
        return false;

    // Direct
    bool bSuccess = false;

    TTextContainer* pTextContainer =
        dynamic_cast<TTextContainer*>(pSprite);

    if(pTextContainer)
        bSuccess |= pTextContainer->SetText(Text);

    // Recursive
    if(bRecursive)
    {
        for(size_t i = 0 ; i < pSprite->GetNSubObjects() ; i++)
            bSuccess |= SetSpriteText(pSprite->GetSubObject(i), Text, bRecursive);
    }

    return bSuccess;
}
