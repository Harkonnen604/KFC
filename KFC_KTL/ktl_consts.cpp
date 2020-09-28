#include "kfc_ktl_pch.h"
#include "ktl_consts.h"

T_KTL_Consts g_KTL_Consts;

// -----------
// KTL consts
// -----------
T_KTL_Consts::T_KTL_Consts()
{
    #ifdef _MSC_VER
    {
        m_bConsole = GetConsoleWindow();
    }
    #else // _MSC_VER
    {
        m_bConsole = true;
    }
    #endif // _MSC_VER

    m_bStdoutConsoleErrors = false;

    m_bTopmost = false;

    m_szStringBufferSize = 16384;
}
