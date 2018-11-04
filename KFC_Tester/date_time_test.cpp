#include "pch.h"
#include "date_time_test.h"

#include <KFC_KTL/date_time.h>

// ----------------
// Global routines
// ----------------
void TestDateTime()
{
	puts("Testing date/time...");

	// Time
	for(QWORD i = 0 ; i < 24*60*60*4 ; i++)
	{
		const TDateTime DT(i);

		if((QWORD)DT != i)
		{
			printf("TIME (%s): " UINT64_FMT " != " UINT64_FMT "\n", (LPCTSTR)TO_STRING(DT), (QWORD)DT, i);
			tassert(false);
		}
	}

	// Day
	for(QWORD i = 0 ; i < (QWORD)1000*1000*24*60*60 ; i += 24*60*60)
	{
		const TDateTime DT(i);

		if((QWORD)DT != i)
		{
			printf("DATE (%s): " UINT64_FMT " != " UINT64_FMT "\n", (LPCTSTR)TO_STRING(DT), (QWORD)DT / (24*60*60), i / (24*60*60));
			tassert(false);
		}
	}

	// Random
	for(int q=16384;q;q--)
	{
		QWORD maxv = (QWORD)(TDateTime(10000, 1, 1, 0, 0, 0));

		#if RAND_MAX < 0x8000
			QWORD v = (rand() * (RAND_MAX+1) + rand()) % maxv;			
		#else // RAND_MAX < 0x8000
			QWORD v = rand() % maxv;
		#endif // RAND_MAX < 0x8000

		tassert((QWORD)TDateTime(v) == v);
	}

	puts("Done");
}
