#include "kfc_scene_pch.h"
#include "scene_object.h"

// -------------------------
// Scene object representer
// -------------------------
TSceneObjectRepresenter::TSceneObjectRepresenter()
{
	m_WorldMatrix.SetIdentity();
}

void TSceneObjectRepresenter::Render() const
{
	size_t i;

	for(i = 0 ; i < m_VisualBinders.GetN() ; i++)
		m_VisualBinders[i]->Bind();

	m_VisualPart->Render();
}

// -------------
// Scene object
// -------------
TSceneObject::TSceneObject()
{
	m_bAllocated = false;
}

void TSceneObject::Release()
{
	m_bAllocated = false;

	m_Representers.Clear();

	m_VisualBinders.Clear();
}

void TSceneObject::Allocate()
{
	Release();

	m_bAllocated = true;
}

bool TSceneObject::OnSuspend()
{
	if(!IsAllocated())
		return false;

	if(!TSuspendable::OnSuspend())
		return false;

	return true;
}

bool TSceneObject::OnResume()
{
	if(!IsAllocated())
		return false;

	if(!TSuspendable::OnResume())
		return false;

	return true;
}

void TSceneObject::PreUpdate()
{
	DEBUG_VERIFY_ALLOCATION;

	// {{{
}

void TSceneObject::PostUpdate()
{
	DEBUG_VERIFY_ALLOCATION;

	// {{{
}

void TSceneObject::Render() const
{
	DEBUG_VERIFY_ALLOCATION;

	for(TRepresenters::TConstIterator Iter = m_Representers.GetFirst() ; Iter.IsValid() ; ++Iter)
		Iter->Render();
}