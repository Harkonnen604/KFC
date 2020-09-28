#include "kfc_ktl_pch.h"
#include "globals.h"

#include "globals_collector.h"

// #define REPORT_GLOBALS_INITIALIZATION_STEPS

// --------
// Globals
// --------
TGlobals::TGlobals(LPCTSTR pSName) : m_Name(pSName)
{
    m_szRefCount = 0;

    m_szNSubGlobalsInitialized = 0;

    m_bInitializing = false;
}

TGlobals::~TGlobals()
{
#ifdef _DEBUG
    if(m_szRefCount > 0)
        ShowErrorText(GLOBALS_FAILURE_TEXT(TEXT("were not uninitialized.")));
#endif // _DEBUG
}

void TGlobals::InternalUninitialize()
{
    // Unregistering and dropping reference counter
    if(m_szRefCount > 0)
    {
        g_GlobalsCollector.UnregisterGlobals(this);

        m_szRefCount = 0;
    }

    // Uninitializing self
    if(m_szNSubGlobalsInitialized == m_SubGlobals.GetN()) // all subs initialized
        OnUninitialize();

    // Unitializing subs
    while(m_szNSubGlobalsInitialized > 0)
        m_SubGlobals[--m_szNSubGlobalsInitialized]->Uninitialize();
}

void TGlobals::InternalInitialize()
{
#ifdef REPORT_GLOBALS_INITIALIZATION_STEPS
    printf("Initializing %s\n", (LPCTSTR)m_Name);
#endif // REPORT_GLOBALS_INITIALIZATION_STEPS

    ResetSuspendCount();

    try
    {
        // Initializing subs
        while(m_szNSubGlobalsInitialized < m_SubGlobals.GetN())
        {
            m_SubGlobals[m_szNSubGlobalsInitialized]->Initialize();
            m_szNSubGlobalsInitialized++;
        }

        // Initializing self
        try
        {
            OnInitialize();
        }

        catch(...)
        {
            INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + m_Name + TEXT("\" failed to initialize."));
        }

        // Registering
        g_GlobalsCollector.RegisterGlobals(this);

        // Setting reference counter
        m_szRefCount = 1;

        // Post-initialization mode change
        try
        {
            ChangeMode();
        }

        catch(...)
        {
            INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + m_Name + TEXT("\" failed to perform post-initialization mode change."));
        }
    }

    catch(...)
    {
        InternalUninitialize();
        throw;
    }

#ifdef REPORT_GLOBALS_INITIALIZATION_STEPS
    printf("Done initializing %s\n", (LPCTSTR)m_Name);
#endif // REPORT_GLOBALS_INITIALIZATION_STEPS
}

void TGlobals::Uninitialize()
{
    if(m_szRefCount == 0)
        return;

    else if(m_szRefCount == 1)
        InternalUninitialize();
    else
        m_szRefCount--;
}

void TGlobals::Initialize()
{
    if(m_bInitializing)
    {
        INITIATE_DEFINED_FAILURE(GLOBALS_FAILURE_TEXT(TEXT("was self-referenced through sub-globals lists.")));
        exit(0);
    }

    m_bInitializing = true;

    try
    {
        if(m_szRefCount == 0)
            InternalInitialize();
        else
            m_szRefCount++;
    }

    catch(...)
    {
        m_bInitializing = false;
        throw;
    }

    m_bInitializing = false;
}

void TGlobals::AddSubGlobals(TGlobals& Globals)
{
    m_SubGlobals.Add() = &Globals;
}

// --------------------
// Globals initializer
// --------------------
TGlobalsInitializer::TGlobalsInitializer()
{
    m_pGlobals = NULL;
}

TGlobalsInitializer::TGlobalsInitializer(TGlobals& SGlobals)
{
    m_pGlobals = NULL;

    Allocate(SGlobals);
}

void TGlobalsInitializer::Release()
{
    if(m_pGlobals)
        m_pGlobals->Uninitialize(), m_pGlobals = NULL;
}

void TGlobalsInitializer::Allocate(TGlobals& SGlobals)
{
    Release();

    SGlobals.Initialize();

    m_pGlobals = &SGlobals;
}
