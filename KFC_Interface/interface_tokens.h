#ifndef interface_tokens_h
#define interface_tokens_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\token.h>

#define CONTROL_TYPE_TOKENS		(g_InterfaceTokens.m_ControlTypeTokens)
#define CONTROL_ID_TOKENS		(g_InterfaceTokens.m_ControlIDTokens)

// -----------------
// Interface tokens
// -----------------
class TInterfaceTokens : public TGlobals
{
private:
	TTokensRegisterer m_FileNameTokensRegisterer;
	TTokensRegisterer m_SpriteIndexTokensRegisterer;
	TTokensRegisterer m_FontIndexTokensRegisterer;
	TTokensRegisterer m_SoundIndexTokensRegisterer;
	TTokensRegisterer m_ControlTypeTokensRegisterer;
	TTokensRegisterer m_ControlIDTokensRegisterer;


	void OnUninitialize	();
	void OnInitialize	();
	
public:
	TTokens m_ControlTypeTokens;
	TTokens m_ControlIDTokens;


	TInterfaceTokens();
};

extern TInterfaceTokens g_InterfaceTokens;

#endif // interface_tokens_h
