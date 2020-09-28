#include "kfc_ssl_pch.h"
#include "ssl_device_globals.h"

T_SSL_DeviceGlobals g_SSL_DeviceGlobals;

// -------------------
// SSL device globals
// -------------------
T_SSL_DeviceGlobals::T_SSL_DeviceGlobals() : TGlobals(TEXT("SSL device globals"))
{
    m_pServer_SSL_Context = NULL;
    m_pClient_SSL_Context = NULL;
}

void T_SSL_DeviceGlobals::OnUninitialize()
{
    if(m_pClient_SSL_Context)
        SSL_CTX_free(m_pClient_SSL_Context), m_pClient_SSL_Context = NULL;

    if(m_pServer_SSL_Context)
        SSL_CTX_free(m_pServer_SSL_Context), m_pServer_SSL_Context = NULL;
}

void T_SSL_DeviceGlobals::OnInitialize()
{
    static bool ls_bInited = false;

    if(!ls_bInited)
    {
        SSL_load_error_strings();

        SSL_library_init();

        ls_bInited = true;
    }

    if(!(m_pServer_SSL_Context = SSL_CTX_new(SSLv23_server_method())))
        INITIATE_DEFINED_FAILURE(TEXT("Error initializing SSL23 server method."));

    if(!(m_pClient_SSL_Context = SSL_CTX_new(SSLv23_client_method())))
        INITIATE_DEFINED_FAILURE(TEXT("Error initializing SSL23 client method."));
}
