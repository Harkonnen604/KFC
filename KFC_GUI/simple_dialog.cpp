#include "kfc_gui_pch.h"
#include "simple_dialog.h"

// --------------
// Simple dialog
// --------------
HINSTANCE TSimpleDialog::GetInstance() const
{
	return m_hInstance;
}

int TSimpleDialog::GetTemplateID() const
{
	return m_iTemplateID;
}
