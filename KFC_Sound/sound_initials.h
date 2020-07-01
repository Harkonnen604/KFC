#ifndef sound_initials_h
#define sound_initials_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ---------------
// Sound initials
// ---------------
class TSoundInitials :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:
	TSoundInitials();

	void Load();
	void Save() const;
};

extern TSoundInitials g_SoundInitials;

#endif // sound_initials_h
