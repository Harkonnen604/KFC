#ifndef protocols_consts_h
#define protocols_consts_h

#include <KFC_KTL/consts.h>

// -----------------
// Protocols consts
// -----------------
class TProtocolsConsts : public TConsts
{
public:
    bool m_bTryAllHTTP_IPs;

public:
    TProtocolsConsts();
};

extern TProtocolsConsts g_ProtocolsConsts;

#endif // protocols_consts_h
