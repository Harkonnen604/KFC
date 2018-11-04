#ifndef globals_collector_h
#define globals_collector_h

#include "suspendable.h"
#include "globals.h"
#include "array.h"

// ------------------
// Globals collector
// ------------------
class TGlobalsCollector : public TSuspendable
{
private:
	TArray<TGlobals*, true> m_GlobalsList;

	bool OnSuspend	();
	bool OnResume	();
	
	bool PreUpdate();

	void PostUpdate(size_t	szStartIndex,
					bool	bFailureRollBack);

	bool PreRender() const;

	void PostRender(size_t	szStartIndex,
					bool	bFailureRollBack) const;

public:
	void RegisterGlobals	(TGlobals* pGlobals);
	void UnregisterGlobals	(TGlobals* pGlobals);

	bool Update();
	bool Render() const;

	void ChangeMode();

	void Terminate();
};

extern TGlobalsCollector g_GlobalsCollector;

#endif // globals_collector_h
