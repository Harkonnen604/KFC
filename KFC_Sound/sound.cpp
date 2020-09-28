#include "kfc_sound_pch.h"
#include "sound.h"

// ----------------------
// Sound creation struct
// ----------------------
TSoundCreationStruct::TSoundCreationStruct()
{
}

void TSoundCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
}

// ------
// Sound
// ------
TSound::TSound()
{
    m_bAllocated = false;
}

void TSound::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;
    }
}

void TSound::Allocate(const TSoundCreationStruct& CreationStruct)
{
    Release();

    try
    {
        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}
