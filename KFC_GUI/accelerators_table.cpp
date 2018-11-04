#include "kfc_gui_pch.h"
#include "accelerators_table.h"

TAcceleratorsTable::TAcceleratorsTable()
{
	m_hAccel = NULL;
}

TAcceleratorsTable::TAcceleratorsTable(HACCEL hAccel)
{
	m_hAccel = NULL;

	Allocate(hAccel);
}

TAcceleratorsTable::TAcceleratorsTable(HINSTANCE hInstance, LPCTSTR pName)
{
	m_hAccel = NULL;

	Allocate(hInstance, pName);
}

TAcceleratorsTable::TAcceleratorsTable(int iID)
{
	m_hAccel = NULL;

	Allocate(iID);
}

void TAcceleratorsTable::Release()
{
	if(m_hAccel)
		DestroyAcceleratorTable(m_hAccel), m_hAccel = NULL;
}

void TAcceleratorsTable::Allocate(HACCEL hAccel)
{
	Release();

	KFC_VERIFY(hAccel);

	m_hAccel = hAccel;
}

void TAcceleratorsTable::Allocate(HINSTANCE hInstance, LPCTSTR pName)
{
	Release();

	m_hAccel = LoadAccelerators(hInstance, pName);

	if(!m_hAccel)
		INITIATE_DEFINED_CODE_FAILURE(TEXT("Error loading accelerators table"), GetLastError());
}

bool TAcceleratorsTable::Translate(HWND hWnd, LPMSG pMsg) const
{
	DEBUG_VERIFY_ALLOCATION;

	return TranslateAccelerator(hWnd, m_hAccel, pMsg) ? true : false;
}

HACCEL TAcceleratorsTable::GetAccel() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hAccel;
}
