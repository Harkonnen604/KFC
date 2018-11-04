#ifndef common_tokens_h
#define common_tokens_h

#include <KFC_KTL/globals.h>
#include <KFC_KTL/tokens.h>

#define FILENAME_TOKENS		(g_CommonTokens.m_FileNameTokens)

// --------------
// Common tokens
// --------------
class TCommonTokens : public TGlobals
{
private:
	TTokensRegisterer m_FileNameTokensRegisterer;

private:
	void OnUninitialize	();
	void OnInitialize	();
	
public:
	TTokens m_FileNameTokens;
	
		
	TCommonTokens();
};

extern TCommonTokens g_CommonTokens;

#endif // common_tokens_h
