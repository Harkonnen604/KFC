#ifndef scene_h
#define scene_h

#include <KFC_KTL\suspendable.h>
#include <KFC_Graphics\d3d_light.h>
#include <KFC_Graphics\camera.h>
#include <KFC_Physics\physical_world.h>
#include "scene_object.h"

// ------
// Scene
// ------
class TScene : public TSuspendable
{
private:
	bool m_bAllocated;


	bool OnSuspend();
	
	bool OnResume();

public:
	typedef TList<TSceneObject> TObjects;

	typedef TList<TD3DLight> TLights;


	TPhysicalWorld m_PhysicalWorld;

	TObjects m_Objects;	

	TD3DColor m_AmbientLight;

	TLights m_Lights;

	TCamera m_Camera;


	TScene();

	~TScene()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate();

	void Update();

	void Render() const;
};

#endif // scene_h