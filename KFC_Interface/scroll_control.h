#ifndef scroll_control_h
#define scroll_control_h

#include <KFC_Sound\sound.h>
#include "control.h"
#include "interface_interpolators.h"
#include "interface_message_map.h"

// -------------------------------
// Scroll control creation struct
// -------------------------------
struct TScrollControlCreationStruct : public TControlCreationStruct
{
    size_t  m_szNVisibleItems;
    FRECT   m_FirstVisibleItemRect;
    FSIZE   m_VisibleItemRectsOffset;
    float   m_fScrollDelay;


    TScrollControlCreationStruct() {}

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);
};

// -------------------------------
// Scroll control sounds provider
// -------------------------------
struct TScrollControlSoundsProvider
{
    TObjectPointer<TSound> m_DescendingScrollSound;
    TObjectPointer<TSound> m_AscendingScrollSound;


    TScrollControlSoundsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// ---------------------------------
// Scroll control controls provider
// ---------------------------------
class TScrollControlControlsProvider
{
private:
    bool m_bAllocated;

    TInterpolatorProcessor m_InterpolatorProcessor;

public:
    TControlPointer     m_DescendingArrow;
    TControlPointer     m_AscendingArrow;
    TControlPointers    m_Items;


    TScrollControlControlsProvider();

    ~TScrollControlControlsProvider()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const TControl*         pParentControl,
                const FRECT&            Resolution);
};

// ---------------
// Scroll control
// ---------------
class TScrollControl : public TControl
{
public:
    // Items
    typedef TArray<TControl*, true> TItems;

private:
    // Interpolator type defs
    typedef
        FLOAT_TIME_SEGMENTED_CONTROL_VALUE_INTERPOLATOR(CommonVisibility, float, TInversedPowerSegmentValueMapper<1.5f>, 2)
            TItemHiderInterpolator;

    typedef
        FLOAT_TIME_SEGMENTED_CONTROL_VALUE_INTERPOLATOR(CommonVisibility, float, TPowerSegmentValueMapper<1.5f>, 2)
            TItemShowerInterpolator;

    typedef
        FLOAT_TIME_SEGMENTED_CONTROL_VALUE_INTERPOLATOR(CommonRect, FRECT, TLinearSegmentValueMapper, 2)
            TItemSliderInterpolator;


    TScrollControlControlsProvider m_ControlsProvider;

    TControl*   m_pDescendingArrow;
    TControl*   m_pAscendingArrow;
    TItems      m_Items;

    size_t  m_szNVisibleItems;
    FRECT   m_FirstVisibleItemRect;
    FSIZE   m_VisibleItemRectsOffset;
    float   m_fScrollDelay;

    SZSEGMENT   m_VisibleItems;
    int         m_iCurrentScrolling;
    int         m_iAccumulatedScrolling;
    float       m_fCurrentScrollerSpeedCoef;
    bool        m_bScrollerHeadingBack;

    // Interpolators
    TCompoundInterpolator m_ScrollInterpolator;

    // Sub-interpolators
    TInterpolatorHandle     m_ItemHider;
    TInterpolatorHandle     m_ItemShower;
    TInterpolatorHandles    m_ItemSliders;

protected:
    // Allocation
    virtual void OnAllocate();

    virtual void OnSetInitialValues();

    // Update/render events
    virtual void OnPreUpdate();

    virtual void OnInterfaceUpdated();

    virtual bool OnSuspend();

    virtual bool OnResume();

    // Arrow click events
    virtual void OnDescendingArrowClick();

    virtual void OnAscendingArrowClick();

    // Scrolling events
    virtual void OnScrollerFinished();

    // Scrolling methods
    void UpdateArrowsAppearance(bool bFirstCall = false);

    void DescendingScroll();

    void AscendingScroll();

    void ScrollItems(   const SZSEGMENT&    SlideSegment,
                        size_t              szHideItemIndex,
                        size_t              szShowItemIndex,
                        const FSIZE&        RectsDelta);

    void HeadScrollerBack();

    void SetScrollerSpeedCoef(float fSpeedCoef);

public:
    TObjectPointer<TSound> m_DescendingScrollSound;
    TObjectPointer<TSound> m_AscendingScrollSound;


    static TControl* LoadControl(   type_t                  tpType,
                                    TInfoNodeConstIterator  InfoNode,
                                    const TControl*         pParentControl,
                                    const FRECT&            Resolution);

    TScrollControl();

    TScrollControl( const TScrollControlCreationStruct& CreationStruct,
                    TScrollControlSoundsProvider&       SoundsProvider,
                    TScrollControlControlsProvider&     ControlsProvider);

    // ---------------- TRIVIALS ----------------
    const TItems& GetItems() const { return m_Items; }
};

#endif // scroll_control_h
