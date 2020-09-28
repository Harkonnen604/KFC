#ifndef sound_consts_h
#define sound_consts_h

#include <KFC_KTL\consts.h>
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_KTL\array.h>

// -------------
// Sound consts
// -------------
class TSoundConsts : public TConsts
{
public:
    // Filenames
    KString m_SoundsFolderName;

    // Storages
    size_t m_szNSoundsRegistrationManagerFixedEntries;


    TSoundConsts();
};

extern TSoundConsts g_SoundConsts;

#endif // sound_consts_h
