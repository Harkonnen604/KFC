#ifndef sound_tokens_h
#define sound_tokens_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\token.h>

#define SOUND_TYPE_TOKENS   (g_SoundTokens.m_SoundTypeTokens)
#define SOUND_INDEX_TOKENS  (g_SoundTokens.m_SoundIndexTokens)

// -------------
// Sound tokens
// -------------
class TSoundTokens : public TGlobals
{
private:
    TTokensRegisterer m_FileNameTokensRegisterer;
    TTokensRegisterer m_SoundTypeTokensRegisterer;
    TTokensRegisterer m_SoundIndexTokensRegisterer;


    void OnUninitialize ();
    void OnInitialize   ();

public:
    TTokens m_SoundTypeTokens;
    TTokens m_SoundIndexTokens;


    TSoundTokens();
};

extern TSoundTokens g_SoundTokens;

#endif // sound_tokens_h
