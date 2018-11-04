#ifndef obscure_h
#define obscure_h

#include "registry.h"

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------
void RandomRegistryScan(KRegistryKey& Key, size_t szTimes);

DWORD GetVolumeSerialNumber();

DWORD MultiplyBits(DWORD dwValue, const DWORD Matrix[32]);

#endif // _MSC_VER

#endif // obscure_h
