#ifndef sound_h
#define sound_h

#include <KFC_Common\structured_info.h>
#include "sound_defs.h"

// ----------------------
// Sound creation struct
// ----------------------
struct TSoundCreationStruct
{
	TSoundCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);
};

// ------
// Sound
// ------
class TSound
{
private:
	bool m_bAllocated;	

public:
	TSound();

	virtual ~TSound()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(const TSoundCreationStruct& CreationStruct);	

	virtual void Load(TInfoNodeConstIterator InfoNode) = 0;

	virtual void Play() const = 0;
	virtual void Stop() const = 0;

	virtual bool IsPlaying() const = 0;
};

#endif // sound_h
