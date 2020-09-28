#include "kfc_ktl_pch.h"
#include "process.h"

#include "file_names.h"

// ----------------
// Global routines
// ----------------
HANDLE StartThread( TThreadProc*    pThreadProc,
                    void*           pParam,
                    bool            bSuspended,
                    UINT*           pRID,
                    DWORD*          pRError)
{
    #ifdef _MSC_VER
    {
        kflags_t flFlags = 0;

        if(bSuspended)
            flFlags |= CREATE_SUSPENDED;

        UINT uiID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, pThreadProc, pParam, flFlags, &uiID);

        if(pRID)
            *pRID = uiID;

        if(hThread == INVALID_HANDLE_VALUE)
        {
            if(pRError)
                *pRError = GetLastError();

            INITIATE_DEFINED_FAILURE(TEXT("Error starting new thread."));
        }

        if(pRError)
            *pRError = 0;

        return hThread;
    }
    #else // _MSC_VER
    {
        kfc_static_assert(sizeof(pthread_t) == sizeof(HANDLE));

        pthread_t Thread;

        if(pthread_create(&Thread, NULL, pThreadProc, pParam))
            INITIATE_DEFINED_FAILURE(TEXT("Error starting new thread."));

        return (HANDLE)(new TThreadSyncObject(Thread));
    }
    #endif // _MSC_VER
}

void StartGuardedThread(volatile LONG&  lNThreads,
                        TThreadProc*    pThreadProc,
                        void*           pParam,
                        bool            bSuspended,
                        UINT*           pRID,
                        DWORD*          pRError)
{
    InterlockedIncrement(&lNThreads);

    TEST_BLOCK_BEGIN
    {
        CloseHandle(StartThread(pThreadProc, pParam, bSuspended, pRID, pRError));
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        InterlockedDecrement(&lNThreads);
        throw;
    }
    TEST_BLOCK_END
}

void WaitGuardedThreads(volatile LONG& lNThreads, size_t szPollDelay)
{
    while(InterlockedExchangeAdd(&lNThreads, 0))
        Sleep((DWORD)szPollDelay);
}

#ifdef _MSC_VER

DWORD StartProcess( const KString&  FileName,
                    const KString&  Params,
                    const KString&  FolderName,
                    bool            bInheritHandles,
                    DWORD           dwFlags,
                    STARTUPINFO&    StartupInfo,
                    HANDLE*         pRProcess,
                    HANDLE*         pRThread,
                    DWORD*          pRError)
{
    PROCESS_INFORMATION ProcessInfo;
    memset(&ProcessInfo, 0, sizeof(ProcessInfo));

    if(!CreateProcess(  FileName,
                        KString::Formatted( TEXT("\"%s\" %s"),
                                                (LPCTSTR)FileName,
                                                (LPCTSTR)Params).GetDataPtr(),
                        NULL,
                        NULL,
                        bInheritHandles,
                        dwFlags,
                        NULL,
                        FolderName,
                        &StartupInfo,
                        &ProcessInfo))
    {
        if(pRError)
            *pRError = GetLastError();

        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error starting process: \"") +
                                            FileName +
                                            TEXT("\""),
                                        GetLastError());
    }

    if(pRError)
        *pRError = 0;

    if(pRProcess)
        *pRProcess = ProcessInfo.hProcess;
    else
        CloseHandle(ProcessInfo.hProcess);

    if(pRThread)
        *pRThread = ProcessInfo.hThread;
    else
        CloseHandle(ProcessInfo.hThread);

    return ProcessInfo.dwProcessId;
}

DWORD StartProcessAtUser(const KString& FileName,
                         const KString& Params,
                         const KString& FolderName,
                         LPCWSTR        pUser,
                         LPCWSTR        pDomain,
                         LPCWSTR        pPassword,
                         HANDLE*        pRProcess,
                         HANDLE*        pRThread,
                         DWORD*         pRError)
{
    DEBUG_VERIFY(pUser && pPassword);

    STARTUPINFOW StartupInfo;
    memset(&StartupInfo, 0, sizeof(StartupInfo)), StartupInfo.cb = sizeof(StartupInfo);

    PROCESS_INFORMATION ProcessInfo;
    memset(&ProcessInfo, 0, sizeof(ProcessInfo));

    if(!CreateProcessWithLogonW(pUser,
                                pDomain,
                                pPassword,
                                0,
                                TWideString(FileName),
                                TWideString(KString::Formatted( TEXT("\"%s\" %s"),
                                                                    (LPCTSTR)FileName,
                                                                    (LPCTSTR)Params).GetDataPtr()),
                                0,
                                NULL,
                                TWideString(FolderName),
                                &StartupInfo,
                                &ProcessInfo))
    {
        if(pRError)
            *pRError = GetLastError();

        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error starting process: \"") +
            FileName +
            TEXT("\""),
            GetLastError());
    }

    if(pRError)
        *pRError = 0;

    if(pRProcess)
        *pRProcess = ProcessInfo.hProcess;
    else
        CloseHandle(ProcessInfo.hProcess);

    if(pRThread)
        *pRThread = ProcessInfo.hThread;
    else
        CloseHandle(ProcessInfo.hThread);

    return ProcessInfo.dwProcessId;
}

void OpenDocument(LPCTSTR pName)
{
    ShellExecute(   NULL,
                    TEXT("open"),
                    pName,
                    NULL,
                    TEXT(""),
                    SW_SHOW);
}

#endif // _MSC_VER
