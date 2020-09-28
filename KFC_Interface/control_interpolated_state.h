#ifndef control_interpolated_state
#define control_interpolated_state

#include <KFC_KTL\suspendable.h>
#include <KFC_Common\interpolator.h>

// ---------------------------
// Control interpolated state
// ---------------------------
template <class StateEnumType, size_t szNStates>
class TControlInterpolatedState : public TSuspendable
{
private:
    typedef FLOAT_TIME_SEGMENTED_VALUE_MAPPER_INTERPOLATOR( float,
                                                            TPersistentDstValueSetter<float>,
                                                            TLinearSegmentValueMapper,
                                                            szNStates) TStateInterpolator;


    bool m_bAllocated;

    StateEnumType m_State;

    TStateInterpolator m_Interpolator;

public:
    TControlInterpolatedState();

    ~TControlInterpolatedState()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const float* pTransitionDelays);

    void SetState(  StateEnumType   SState,
                    bool            bImmediate);

    bool OnResume   ();
    bool OnSuspend  ();

    void Update();

    StateEnumType GetState() const;

    float GetInterpolatedState() const;

    bool IsTransitionActive() const;

    operator StateEnumType () const { return GetState(); }
};

template <class StateEnumType, size_t szNStates>
TControlInterpolatedState<StateEnumType, szNStates>::TControlInterpolatedState()
{
    m_bAllocated = false;
}

template <class StateEnumType, size_t szNStates>
void TControlInterpolatedState<StateEnumType, szNStates>::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        m_Interpolator.Release();
    }
}

template <class StateEnumType, size_t szNStates>
void TControlInterpolatedState<StateEnumType, szNStates>::Allocate(const float* pTransitionDelays)
{
    Release();

    try
    {
        DEBUG_VERIFY(szNStates > 0);

        DEBUG_VERIFY(pTransitionDelays);

        size_t i;

        // Suspendable
        ResetSuspendCount();

        // State
        m_State = (StateEnumType)0;

        // Interpolator
        float SrcValues[szNStates] = {0.0f};

        for(i = 1 ; i < szNStates ; i++)
            SrcValues[i] = SrcValues[i-1] + pTransitionDelays[i-1];

        float DstValues[szNStates];

        for(i = 0 ; i < szNStates ; i++)
            DstValues[i] = (float)i;

        m_Interpolator.Allocate();

        m_Interpolator.GetSrcValueGetter().Allocate(FSEGMENT(SrcValues[0], SrcValues[szNStates - 1]),
                                                    SrcValues[0]);

        m_Interpolator.GetDstValueSetter().Allocate(DstValues[0]);

        m_Interpolator.GetValueMapper().Allocate(SrcValues, DstValues);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}
template <class StateEnumType, size_t szNStates>
void TControlInterpolatedState<StateEnumType, szNStates>::SetState( StateEnumType   SState,
                                                                    bool            bImmediate)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(HitsSegmentBounds((float)SState, FSEGMENT(0.0f, (float)(szNStates - 1))));

    m_State = SState;

    const float fStateValue = m_Interpolator.GetSrcValues()[(size_t)m_State];

    if(bImmediate)
    {
        m_Interpolator.SetCurrentSrcValue(fStateValue);

        m_Interpolator.Activate(), m_Interpolator.Update();
    }
    else
    {
        m_Interpolator.Activate(fStateValue);
    }
}

template <class StateEnumType, size_t szNStates>
bool TControlInterpolatedState<StateEnumType, szNStates>::OnResume()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnResume())
        return false;

    m_Interpolator.Resume();

    return true;
}

template <class StateEnumType, size_t szNStates>
bool TControlInterpolatedState<StateEnumType, szNStates>::OnSuspend()
{
    if(!IsAllocated())
        return false;

    if(!TSuspendable::OnSuspend())
        return false;

    m_Interpolator.Suspend();

    return true;
}
template <class StateEnumType, size_t szNStates>
void TControlInterpolatedState<StateEnumType, szNStates>::Update()
{
    DEBUG_VERIFY_ALLOCATION;

    m_Interpolator.Update();
}

template <class StateEnumType, size_t szNStates>
StateEnumType TControlInterpolatedState<StateEnumType, szNStates>::GetState() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_State;
}

template <class StateEnumType, size_t szNStates>
float TControlInterpolatedState<StateEnumType, szNStates>::GetInterpolatedState() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_Interpolator.GetCurrentDstValue();
}

template <class StateEnumType, size_t szNStates>
bool TControlInterpolatedState<StateEnumType, szNStates>::IsTransitionActive() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_Interpolator.IsActive();
}

#endif // control_interpolated_state
