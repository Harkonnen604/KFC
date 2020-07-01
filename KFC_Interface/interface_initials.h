#ifndef interface_initials_h
#define interface_initials_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// -------------------
// Interface initials
// -------------------
class TInterfaceInitials :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:
	TInterfaceInitials();

	void Load();
	void Save() const;
};

extern TInterfaceInitials g_InterfaceInitials;

#endif // interface_initials_h
