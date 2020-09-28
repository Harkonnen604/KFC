#include "kfc_physics_pch.h"
#include "physical_world.h"

#include "physics_cfg.h"

// ---------------
// Physical world
// ---------------
TPhysicalWorld::TPhysicalWorld()
{
    m_bAllocated = false;
}

void TPhysicalWorld::Release()
{
    m_bAllocated = false;

    m_Objects.Clear();
}

void TPhysicalWorld::Allocate(double dSCurTime)
{
    Release();

    m_dCurTime = dSCurTime;

    m_bAllocated = true;
}

void TPhysicalWorld::ResetTime(double dSCurTime)
{
    DEBUG_VERIFY_ALLOCATION;

    m_dCurTime = dSCurTime;
}

double TPhysicalWorld::Update()
{
    DEBUG_VERIFY_ALLOCATION;

    // {{{ perform time-subdivision here

    double dStepTime = g_PhysicsCfg.m_dInitialTickTime;

    TObjects::TIterator Iter;

    for(Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
        (*Iter)->BeginFrame();

    for(Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
        (*Iter)->EndFrame(dStepTime);

    m_dCurTime += dStepTime;

    return m_dCurTime;
}
