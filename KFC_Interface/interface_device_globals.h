#ifndef interface_device_globals_h
#define interface_device_globals_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_KTL\registration_manager.h>
#include <KFC_Common\message.h>
#include <KFC_Common\interpolator.h>
#include <KFC_Graphics\font.h>
#include <KFC_Graphics\sprite.h>
#include <KFC_Graphics\d3d_color.h>
#include <KFC_Graphics\color_defs.h>
#include <KFC_Sound\sound.h>
#include "interface_defs.h"
#include "hot_pointer.h"

// -------------------------
// Interface device globals
// -------------------------
class TInterfaceDeviceGlobals : public TGlobals
{
private:
    // Messages
    TMessageProcessor m_InputMessageProcessor;

    // Sprites
    TObjectRegisterer<TSprite> m_MousePointerSpriteRegisterer;
    TObjectRegisterer<TSprite> m_HotPointerSpriteRegisterers[4];
    TObjectRegisterer<TSprite> m_CheckBoxSpriteRegisterer;
    TObjectRegisterer<TSprite> m_OkSpriteRegisterer;
    TObjectRegisterer<TSprite> m_CancelSpriteRegisterer;

    // Fonts
    TObjectRegisterer<TFont> m_HotMessageFontRegisterer;
    TObjectRegisterer<TFont> m_DefaultLabelFontRegisterer;

    // Sounds
    TObjectRegisterer<TSound> m_HotMessageSoundRegisterer;
    TObjectRegisterer<TSound> m_PushSoundRegisterer;
    TObjectRegisterer<TSound> m_ClickSoundRegisterer;

    // Hot message
    KString     m_HotMessageText;
    FPOINT      m_HotMessageCoords;
    TD3DColor   m_HotMessageColor;
    float       m_fHotMessageVisibilityDelay;

    // Hot pointers
    struct THotPointerEntry
    {
        FPOINT                  m_Coords;
        THotPointerDirection    m_Direction;


        void Set(   const FPOINT&           SCoords,
                    THotPointerDirection    SDirection)
        {
            m_Coords    = SCoords;
            m_Direction = SDirection;
        }
    };

    TArray<THotPointerEntry> m_HotPointers;

    typedef
        FLOAT_TIME_SEGMENTED_VALUE_MAPPER_INTERPOLATOR(float, TPersistentDstValueSetter<float>, TLinearSegmentValueMapper, 3)
            THotMessageInterpolator;

    THotMessageInterpolator m_HotMessageInterpolator;

    // Interface
    TControl* m_pMouseOwnerControl;
    TControl* m_pFocusOwnerControl;


    void OnUninitialize ();
    void OnInitialize   ();

    bool OnSuspend  ();
    bool OnResume   ();

    void OnUpdate();

    void OnPostRender(bool bFailureRollBack) const;

    void OnChangeMode();

    FPOINT GetWindowedMouseCoords();

    void CenterMouseCursor();

public:
    // Mouse pointer
    bool m_bMousePointerVisible;

    // Mouse data
    FRECT   m_MouseRect;
    FSIZE   m_MouseSensitivity;
    FSIZE   m_MouseDelta;
    FPOINT  m_MouseCoords;
    FSIZE   m_MouseCoordsDelta;


    TInterfaceDeviceGlobals();

    void SetMouseRect(const FRECT& SMouseRect);

    // Hot message
    void RemoveHotMessage();

    void SetHotMessage( const KString&              MessageText,
                        const FPOINT&               MessageCoords,
                        const TD3DColor&            Color,
                        float                       fVisibilityDelay,
                        const FRECT*                pPointerTargets,
                        const THotPointerDirection* pPointerDirections,
                        size_t                      szNPointers,
                        bool                        bPlayDefaultSound = true);

    void SetHotMessage( const KString&      MessageText,
                        const FPOINT&       MessageCoords,
                        const TD3DColor&    Color,
                        float               fVisibilityDelay,
                        bool                bPlayDefaultSound = true);

    void SetHotMessage( const KString&          MessageText,
                        const FPOINT&           MessageCoords,
                        const TD3DColor&        Color,
                        float                   fVisibilityDelay,
                        const FRECT&            PointerTarget,
                        THotPointerDirection    PointerDirection,
                        bool                    bPlayDefaultSound = true);

    void SetHotMessage( const KString&      MessageText,
                        const FPOINT&       MessageCoords,
                        const TD3DColor&    Color,
                        float               fVisibilityDelay,
                        const TControl*     pPointerTarget,
                        bool                bPlayDefaultSound = true);

    // Mouse ownership
    bool IsMouseOwned() const;

    TControl* GetMouseOwnerControl();

    TInterface* GetMouseOwnerInterface();

    void SetMouseOwnerControl(  TControl*   pControl,
                                bool        bCallMouseCaptureLostEvent);

    void InvalidateMouseOwnership(bool bCallMouseCaptureLostEvent);

    void InvalidateMouseOwnership(  TInterface* pInterface,
                                    bool        bCallMouseCaptureLostEvent);

    void InvalidateMouseOwnership(  TControl*   pControl,
                                    bool        bCallMouseCaptureLostEvent);

    // Focus ownership
    bool IsFocusOwned() const;

    TControl* GetFocusOwnerControl();

    TInterface* GetFocusOwnerInterface();

    void SetFocusOwnerControl(  TControl*   Control,
                                bool        bCallFocusLostEvent);

    void InvalidateFocusOwnership(bool bCallFocusLostEvent);

    void InvalidateFocusOwnership(  TInterface* pInterface,
                                    bool        bCallFocusLostEvent);

    void InvalidateFocusOwnership(  TControl*   pControl,
                                    bool        bCallFocusLostEvent);

    // Messages
    TMessageIterator GetFirstInputMessage();
};

extern TInterfaceDeviceGlobals g_InterfaceDeviceGlobals;

#endif // interface_device_globals_h
