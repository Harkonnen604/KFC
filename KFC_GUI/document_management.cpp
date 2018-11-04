#include "kfc_gui_pch.h"
#include "document_management.h"

// --------------------
// Document management
// --------------------
TDocumentManagement::TDocumentManagement()
{
}

void TDocumentManagement::Release()
{
	m_UntitledFileName.Empty();
}

void TDocumentManagement::Allocate(LPCTSTR pUntitledFileName)
{
	Release();

	DEBUG_VERIFY(pUntitledFileName && *pUntitledFileName);

	m_UntitledFileName = pUntitledFileName;

	MakeNewDocument();
}

void TDocumentManagement::ModifyDocument(bool bModify)
{
	DEBUG_VERIFY_ALLOCATION;

	bool bOldModified = m_bModified;

	m_bModified = bModify;

	if(bModify)
		OnDocumentChange();

	if(m_bModified != bOldModified)
		ReportDocumentStateChange();	
}

bool TDocumentManagement::CheckSaveDocument()
{
	DEBUG_VERIFY_ALLOCATION;

	if(!m_bModified)
		return true;

	int r = OnConfirmSaveDocument(m_FileName, m_bUntitled);

	if(r == IDCANCEL)
		return false;

	if(r == IDNO)
		return true;

	return SaveDocument();
}

void TDocumentManagement::MakeNewDocument()
{
	m_FileName = m_UntitledFileName, m_bUntitled = true;

	m_bModified = false;

	OnNewDocument();

	OnDocumentChange();

	ReportDocumentStateChange();
}

bool TDocumentManagement::NewDocument()
{
	DEBUG_VERIFY_ALLOCATION;

	if(!CheckSaveDocument())
		return false;

	MakeNewDocument();

	return true;
}

bool TDocumentManagement::OpenDocument()
{
	DEBUG_VERIFY_ALLOCATION;

	if(!CheckSaveDocument())
		return false;

	bool bWasDamaged = false;

	for(;;)
	{
		KString FileName = OnGetLoadDocumentFileName();

		if(FileName.IsEmpty())
		{
			if(bWasDamaged)
				MakeNewDocument();

			return false;
		}

		bool bDamaged = false;

		if(!OnLoadDocument(FileName, false, bDamaged))
		{
			bWasDamaged |= bDamaged;
			continue;
		}

		DEBUG_VERIFY(!bDamaged);

		m_FileName = FileName, m_bUntitled = false;
		
		m_bModified = false;

		OnDocumentChange();

		ReportDocumentStateChange();

		return true;		
	}
}

bool TDocumentManagement::ForceOpenDocument(LPCTSTR pFileName, bool bSilent)
{
	DEBUG_VERIFY_ALLOCATION;

	bool bDamaged;

	if(!OnLoadDocument(pFileName, bSilent, bDamaged))
	{
		if(bDamaged)
			MakeNewDocument();

		return false;
	}

	m_FileName = pFileName, m_bUntitled = false;

	m_bModified = false;

	OnDocumentChange();

	ReportDocumentStateChange();

	return true;
}

bool TDocumentManagement::SaveDocument(bool bSaveAs)
{
	DEBUG_VERIFY_ALLOCATION;

	if(m_bUntitled)
		bSaveAs = true;

	for(;;)
	{
		KString FileName;

		if(bSaveAs)
		{
			FileName = OnGetSaveDocumentFileName(m_FileName, m_bUntitled);

			if(FileName.IsEmpty())
				return false;
		}
		else
		{
			FileName = m_FileName;
		}

		if(!OnSaveDocument(FileName, false))
		{
			bSaveAs = true;
			continue;
		}

		m_FileName = FileName, m_bUntitled = false;

		m_bModified = false;

		ReportDocumentStateChange();

		return true;
	}
}
