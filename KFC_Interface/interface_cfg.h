#ifndef interface_cfg_h
#define interface_cfg_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// --------------
// Interface cfg
// --------------
class TInterfaceCfg :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:
	TInterfaceCfg();

	void Load();
	void Save() const;
};

extern TInterfaceCfg g_InterfaceCfg;

#endif // interface_cfg_h
