#ifndef common_initials_h
#define common_initials_h

#include <KFC_KTL/globals.h>
#include "assignments.h"

#ifdef _MSC_VER

// ----------------
// Common initials
// ----------------
class TCommonInitials :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:
	TCommonInitials();

	void Load();
	void Save() const;
};

extern TCommonInitials g_CommonInitials;

#endif // _MSC_VER

#endif // common_initials_h
