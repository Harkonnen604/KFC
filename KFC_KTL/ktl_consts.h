#ifndef ktl_consts_h
#define ktl_consts_h

#include "consts.h"

// -----------
// KTL consts
// -----------
class T_KTL_Consts : public TConsts
{
public:
	// Affects fatal errors reporting
	bool m_bConsole;

	// Affects fatal errors reporting
	bool m_bStdoutConsoleErrors;

	// Affects fatal errors reporting
	bool m_bTopmost;

	size_t m_szStringBufferSize;

public:
	T_KTL_Consts();
};

extern T_KTL_Consts g_KTL_Consts;

#endif // ktl_consts_h
