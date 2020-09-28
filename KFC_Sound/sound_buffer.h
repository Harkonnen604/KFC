#ifndef sound_buffer_h
#define sound_buffer_h

#include "dsound_inc.h"

// ------------
// Wave format
// ------------
struct TWaveFormat : public WAVEFORMATEX
{
    TWaveFormat()
        { cbSize = sizeof(*this); }

    TWaveFormat(const WAVEFORMATEX& SFormat) : WAVEFORMATEX(SFormat) {}

    TWaveFormat& operator = (const WAVEFORMATEX& SFormat)
        { static_cast<WAVEFORMATEX&>(*this) = SFormat; return *this; }
};

// -------------
// Sound buffer
// -------------
class TSoundBuffer
{
private:
    IDirectSoundBuffer* m_pBuffer;

public:
    TSoundBuffer();

    ~TSoundBuffer()
        { Release(); }

    bool IsAllocated() const
        { return m_pBuffer; }

    void Release();

    void Allocate(  const WAVEFORMATEX& WaveFormat,
                    size_t              szLengthSamples);

    void AllocatePrimary();

    void GetFormat(TWaveFormat& RFormat) const;

    void SetFormat(const TWaveFormat& Format);

    LPDIRECTSOUNDBUFFER GetBuffer() const;

    operator LPDIRECTSOUNDBUFFER () const { return GetBuffer(); }
};

#endif // sound_buffer_h
