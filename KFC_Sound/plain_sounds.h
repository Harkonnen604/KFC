#ifndef plain_sounds_h
#define plain_sounds_h

#include "dmusic_inc.h"
#include "sound.h"

// ------------------------------------
// Music segment sound creation struct
// ------------------------------------
struct TMusicSegmentSoundCreationStruct : TSoundCreationStruct
{
    KString m_FileName;
    size_t  m_szNRepeats;
    bool    m_bAllowOnlyOneInstance;
    bool    m_bPrimary;
    bool    m_bFailOnNoPort;


    TMusicSegmentSoundCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);

    bool SetFileName(const KString& SFileName);
};

// --------------------
// Music segment sound
// --------------------
class TMusicSegmentSound : public TSound
{
private:
    bool m_bAllocated;

    IDirectMusicSegment8* m_pSegment;

    bool    m_bSynthezied;
    size_t  m_szNRepeats;
    bool    m_bAllowOnlyOneInstance;
    bool    m_bPrimary;
    bool    m_bFailOnNoPort;

    mutable bool m_bPlaying;

public:
    static TSound* Create(type_t tpType);

    TMusicSegmentSound();

    ~TMusicSegmentSound()
        { Release(); }

    bool IsAllocated() const
        { return TSound::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const TMusicSegmentSoundCreationStruct& CreationStruct);

    void Load(TInfoNodeConstIterator InfoNode);

    void Play() const;
    void Stop() const;

    bool IsPlaying() const;

    IDirectMusicSegment8* GetSegment();

    size_t GetNRepeats() const;

    void SetNRepeats(size_t szSNRepeats);

    // ---------------- TRIVIALS ----------------
    bool IsSynthezied() const { return m_bSynthezied; }

    bool DoesAllowOnlyOneInstance() const { return m_bAllowOnlyOneInstance; }
};

#endif // plain_sounds_h
