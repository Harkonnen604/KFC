#ifndef gui_initials_h
#define gui_initials_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// -------------
// GUI initials
// -------------
class T_GUI_Initials :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:
	T_GUI_Initials();

	void Load();
	void Save() const;
};

extern T_GUI_Initials g_GUI_Initials;

#endif // gui_initials_h