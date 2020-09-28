#include "kfc_hooks_pch.h"
#include "api_hook.h"

#error Update opcodes from civ4_memfix

// --------------
// API hook base
// --------------

// Protector
T_API_HookBase::TProtector::TProtector()
{
    m_pData = NULL;
}

T_API_HookBase::TProtector::TProtector(void* pSData, size_t szSSize, DWORD dwProtect)
{
    m_pData = NULL;

    Allocate(pSData, szSSize, dwProtect);
}

void T_API_HookBase::TProtector::Release()
{
    if(m_pData)
    {
        DWORD dwTempProtect;
        VirtualProtect(m_pData, m_szSize, m_dwOldProtect, &dwTempProtect);

        m_pData = NULL;
    }
}

void T_API_HookBase::TProtector::Allocate(void* pSData, size_t szSSize, DWORD dwProtect)
{
    Release();

    DEBUG_VERIFY(pSData && szSSize > 0);

    if(!VirtualProtect(m_pData = pSData, m_szSize = szSSize, dwProtect, &m_dwOldProtect))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Virtual protection error."), GetLastError());
}

// API hook
T_API_HookBase::T_API_HookBase()
{
    m_bAllocated = false;
}

void T_API_HookBase::Release()
{
    if(!m_bAllocated)
        return;

    m_bAllocated = false;

    if(m_pTrampProc == m_TrampCode)
    {
        TProtector Protector(m_pOrigProc, 5, PAGE_READWRITE);

        memcpy(m_pOrigProc, m_TrampCode, 5);
    }

    m_OuterCallWrapperProtector.Release();

    m_TrampCodeProtector.Release();
}

// ASM processors
static size_t GetCodeLength(const BYTE* pData,
                            size_t      szCoverSize,
                            size_t*     pRJMPs,
                            size_t&     szRNJMPs)
{
    const size_t szMaxJMP = szRNJMPs;

    szRNJMPs = 0;

    size_t i;

    for(i = 0 ; i < szCoverSize ; )
    {
        if(pData[i] == 0xE9) // JMP rel32
        {
            if(szRNJMPs == szMaxJMP)
                return UINT_MAX;

            pRJMPs[szRNJMPs++] = i;

            i += 5;
        }
        else if(pData[i] >= 0x50 && pData[i] <= 0x58) // PUSH r32
            i++;
        else if(pData[i] == 0x6A) // push imm8
            i += 2;
        else if(pData[i] == 0x68) // push imm32
            i += 5;
        else if(pData[i] == 0x8B && pData[i+1] == 0xEC) // MOV EBP, ESP
            i += 2;
        else if(pData[i] == 0x8B && pData[i+1] == 0x45) // MOV EAX, DWORD PTR [EBP + imm8]
            i += 3;
        else if(pData[i] == 0x8B && pData[i+1] == 0x74 && pData[i+2] == 0x24) // MOV .....
            i += 4;
        else if(pData[i] == 0x8B && pData[i+1] == 0x7C && pData[i+2] == 0x24) // MOV EDI, DWORD PTR [ESP + imm8]
            i += 4;
        else if(pData[i] == 0x8B && pData[i+1] == 0x5D) // MOV EBX, DWORD PTR[EBP + imm8]
            i += 3;
        else if(pData[i] == 0x83 && pData[i+1] == 0x65 && pData[i+2] == 0xFC && pData[i+3] == 0x00) // AND DWORD PTR [EBP-4], 0
            i += 4;
        else if(pData[i] == 0x83 && pData[i+1] == 0xEC) // SUB ESP, imm8
            i += 3;
        else if(pData[i] == 0x81 && pData[i+1] == 0xEC) // SUB esp, imm32
            i += 6;
        else if(pData[i] == 0xB8) // MOV EAX, imm32
            i += 5;
        else if(pData[i] == 0x24) // AND AL, imm8
            i += 2;
        else
            return UINT_MAX;
    }

    return i;
}

static inline void PlaceJMP(BYTE* src, const BYTE* dst)
{
    DWORD r;

    src[0] = 0xE9;
    r = dst - (src + 5), memcpy(src+1, &r, sizeof(r));
}

static inline void AdjustJMP(BYTE* src, const BYTE* old_src)
{
    DWORD r;

    DEBUG_VERIFY(src[0] == 0xE9 && old_src[0] == 0xE9);
    memcpy(&r, old_src+1, sizeof(r)), r -= src - old_src, memcpy(src+1, &r, sizeof(r));
}

// Outer call code
static const BYTE s_OuterCallCode[] =
{
    // *get = pInHookCounterGetter
    // *trm = m_TrampCode
    // *edi = offsetof(TInHookCounter, m_dwEDI)
    // *esi = offsetof(TInHookCounter, m_dwESI)

    /* 0x00: */ 0xE8, 0xCC, 0xCC, 0xCC, 0xCC,   // call *get (rel32)
    /* 0x05: */ 0x83, 0x38, 0x00,               // cmp  DWORD PTR [eax], 0
    /* 0x08: */ 0x75, 0x36,                     // jnz  *trm (rel8)
    /* 0x0A: */ 0xFF, 0x00,                     // inc  DWORD PTR [eax]
    /* 0x0C: */ 0x89, 0x78, 0x04,               // mov  [eax+*edi], edi
    /* 0x0F: */ 0x89, 0x70, 0x08,               // mov  [eax+*esi], esi
    /* 0x12: */ 0x8B, 0xF0,                     // mov  esi, eax
    /* 0x14: */ 0x5F,                           // pop  edi
    /* 0x15: */ 0xE8, 0xCC, 0xCC, 0xCC, 0xCC,   // call *hook (rel32)
    /* 0x1A: */ 0x57,                           // push edi
    /* 0x1B; */ 0xFF, 0x0E,                     // dec  DWORD PTR [esi]
    /* 0x1D: */ 0x8B, 0x7E, 0x04,               // mov  edi, [esi+*edi]
    /* 0x20: */ 0x8B, 0x76, 0x08,               // mov  esi, [esi+*esi]
    /* 0x23: */ 0xC3                            // ret
};

bool T_API_HookBase::Allocate(  LPCTSTR                     pName,
                                void*                       pSOrigProc,
                                void*                       pHookProc,
                                T_TLS_InHookCounterGetter*  pInHookCounterGetter,
                                bool                        bSafe)
{
    Release();

    try
    {
        m_pOrigProc = pSOrigProc;

        size_t szJMP;
        size_t szNJMPs = 1;

        const size_t sz = GetCodeLength((const BYTE*)m_pOrigProc, 5, &szJMP, szNJMPs);

        assert(szNJMPs < 2);

        if(sz == UINT_MAX || sz >= sizeof(m_TrampCode))
        {
            if(!bSafe)
                INITIATE_DEFINED_FAILURE((KString)TEXT("Error placing API hook for \"") + pName + TEXT("\"."));

            m_pTrampProc = m_pOrigProc;
        }
        else
        {
            m_pTrampProc = m_TrampCode;

            m_TrampCodeProtector.Allocate(  m_TrampCode,
                                            sizeof(m_TrampCode),
                                            PAGE_EXECUTE_READWRITE);

            if(pInHookCounterGetter)
            {
                m_OuterCallWrapperProtector.Allocate(   m_OuterCallWrapper,
                                                        sizeof(m_OuterCallWrapper),
                                                        PAGE_EXECUTE_READWRITE);

                DWORD r;

                // Body
                memcpy(m_OuterCallWrapper, s_OuterCallCode, sizeof(s_OuterCallCode));

                r = (BYTE*)pInHookCounterGetter - (m_OuterCallWrapper + 0x05);
                memcpy(m_OuterCallWrapper + 0x01, &r, sizeof(r));

                r = (BYTE*)pHookProc - (m_OuterCallWrapper + 0x1A);
                memcpy(m_OuterCallWrapper + 0x16, &r, sizeof(r));
            }

            {
                TProtector Protector0(m_pOrigProc, sz, PAGE_READWRITE);

                memcpy(m_TrampCode, m_pOrigProc, sz);

                if(szNJMPs)
                    AdjustJMP(m_TrampCode + szJMP, (const BYTE*)m_pOrigProc + szJMP);

                PlaceJMP(m_TrampCode + sz, (const BYTE*)m_pOrigProc + sz);

                PlaceJMP(   (BYTE*)m_pOrigProc,
                            pInHookCounterGetter ?
                                (const BYTE*)m_OuterCallWrapper :
                                (const BYTE*)pHookProc);
            }
        }

        m_bAllocated = true;

        return IsHooked();
    }

    catch(...)
    {
        Release();
        throw;
    }
}
