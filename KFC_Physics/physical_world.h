#ifndef physical_world_h
#define physical_world_h

#include "physical_object.h"

// ---------------
// Physical world
// ---------------
class TPhysicalWorld
{
private:
	bool m_bAllocated;

	double m_dCurTime; // seconds
	
public:
	// Objects
	typedef TList< TPtrHolder<TPhysicalObject> > TObjects;
	
	TObjects m_Objects;


	TPhysicalWorld();

	~TPhysicalWorld()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(double dSCurTime = 0.0);

	void ResetTime(double dSCurTime = 0.0);

	// Returns new time
	double Update();

	double GetCurTime() const
		{ DEBUG_VERIFY_ALLOCATION; return m_dCurTime; }
};

#endif // physical_world_h