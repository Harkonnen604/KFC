#ifndef physics_cfg_h
#define physics_cfg_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ------------
// Physics cfg
// ------------
class TPhysicsCfg :
	public TGlobals,
	public TAssignmentsList
{
private:
	void OnUninitialize	();
	void OnInitialize	();

	void LoadItems(KRegistryKey& Key);
	void SaveItems(KRegistryKey& Key) const;

public:	
	double m_dInitialTickTime;	// seconds
	size_t m_szMaxTimeSubdivision;


	TPhysicsCfg();

	void Load();
	void Save() const;
};

extern TPhysicsCfg g_PhysicsCfg;

#endif // physics_cfg_h