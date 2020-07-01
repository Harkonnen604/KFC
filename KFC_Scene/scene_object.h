#ifndef scene_object_h
#define scene_object_h

#include <KFC_KTL\suspendable.h>
#include <KFC_Graphics\visual_part.h>
#include "visual_binder.h"

// -------------------------
// Scene object representer
// -------------------------
class TSceneObjectRepresenter
{
public:
	TObjectPointer<TVisualPart> m_VisualPart;

	TD3DMatrix m_WorldMatrix;

	TArray< TPtrHolder<TVisualBinder> > m_VisualBinders;


	TSceneObjectRepresenter();

	void Render() const;
};

// -------------
// Scene object
// -------------
class TSceneObject : public TSuspendable
{
private:
	bool m_bAllocated;


	bool OnSuspend();

	bool OnResume();

public:
	typedef TList<TSceneObjectRepresenter> TRepresenters;


	TRepresenters m_Representers;


	TSceneObject();

	~TSceneObject()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate();

	void PreUpdate();

	void PostUpdate();

	void Render() const;
};

#endif // scene_object_h