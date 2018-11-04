#include "kfc_common_pch.h"
#include "random_globals.h"

#include "time_globals.h"

TRandomGlobals g_RandomGlobals;

// ----------------
// Helper routines
// ----------------
inline int big_rand()
{
#if RAND_MAX < 0x8000
	return rand() * (RAND_MAX + 1) + rand();
#else // RAND_MAX < 0x8000
	return rand();
#endif // RAND_MAX < 0x8000
}

// ---------------
// Random globals
// ---------------
TRandomGlobals::TRandomGlobals() : TGlobals(TEXT("Random globals"))
{
	AddSubGlobals(g_TimeGlobals);
}

void TRandomGlobals::OnUninitialize()
{
}

void TRandomGlobals::OnInitialize()
{
	srand((UINT)g_TimeGlobals.GetGlobalMSEC());
}

int TRandomGlobals::GenerateRandomInt(const ISEGMENT& Segment)
{
	DEBUG_VERIFY(IsInitialized());

	DEBUG_VERIFY(!Segment.IsFlat());

	return Segment.m_First + big_rand() % Segment.GetLength();
}

size_t TRandomGlobals::GenerateRandomUINT(const SZSEGMENT& Segment)
{
	DEBUG_VERIFY(IsInitialized());

	DEBUG_VERIFY(!Segment.IsFlat());

	return Segment.m_First + (size_t)big_rand() % Segment.GetLength();
}

float TRandomGlobals::GenerateRandomFloat(	const FSEGMENT&	Segment,
											size_t			szAccuracy)
{
	DEBUG_VERIFY(IsInitialized());

	float fValue = (float)GenerateRandomUINT(SZSEGMENT(0, szAccuracy));
	
	ResizeValue(FSEGMENT(0, (float)szAccuracy),
				Segment,
				fValue);

	return fValue;
}

bool TRandomGlobals::GenerateRandomBool(float fProbability)
{
	DEBUG_VERIFY(IsInitialized());

	DEBUG_VERIFY(HitsSegmentBounds(fProbability, FSEGMENT(0.0f, 1.0f)));

	return Compare(GenerateRandomFloat(FSEGMENT(0.0f, 1.0f)), fProbability) < 0;
}
