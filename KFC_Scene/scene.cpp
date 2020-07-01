#include "kfc_scene_pch.h"
#include "scene.h"

#include <KFC_Graphics\graphics_state_manager.h>

// ------
// Scene
// ------
TScene::TScene()
{
	m_bAllocated = false;
}

void TScene::Release()
{
	m_bAllocated = false;

	m_Lights.Clear();

	m_Objects.Clear();

	m_PhysicalWorld.Release();
}

void TScene::Allocate()
{
	Release();

	m_PhysicalWorld.Allocate();

	m_AmbientLight = BlackColor();

	m_bAllocated = true;
}

bool TScene::OnSuspend()
{
	if(!IsAllocated())
		return false;

	if(!TSuspendable::OnSuspend())
		return false;

	for(TObjects::TIterator Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->Suspend();

	return true;
}

bool TScene::OnResume()
{
	if(!IsAllocated())
		return false;

	if(!TSuspendable::OnResume())
		return false;

	for(TObjects::TIterator Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->Resume();

	return true;
}

void TScene::Update()
{
	DEBUG_VERIFY_ALLOCATION;

	TObjects::TIterator Iter;

	// Objects pre-update
	for(Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->PreUpdate();

	// Physical world update
	m_PhysicalWorld.Update();

	// Objects post-update
	for(Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->PostUpdate();
}

void TScene::Render() const
{
	// Ambient light
	g_GraphicsStateManager.SetAmbientLight(m_AmbientLight);

	// Lights
	{
		TLights::TConstIterator Iter;

		size_t i;

		for(Iter = m_Lights.GetFirst(), i = 0 ;
			Iter.IsValid() && i < g_GraphicsDeviceGlobals.m_szMaxActiveLights ;
			++Iter, ++i)
		{
			Iter->Install(i);
		}
	}

	// Camera
	m_Camera.Install();

	// Objects
	for(TObjects::TConstIterator Iter = m_Objects.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->Render();
}