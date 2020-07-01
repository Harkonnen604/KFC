#include "kfc_graphics_pch.h"
#include "font.h"

// ---------------------
// Font creation struct
// ---------------------
TFontCreationStruct::TFontCreationStruct()
{
	m_Color = WhiteColor();
	
	m_CharSpacing.Set(0.0f, 0.0f);
}

void TFontCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	TInfoParameterConstIterator PIter;

	// Geting color
	if((PIter = InfoNode->FindParameter(TEXT("Color"))).IsValid())
	{
		ReadColor(	PIter->m_Value,
					m_Color,
					TEXT("font color"));
	}

	// Getting character spacing
	if((PIter = InfoNode->FindParameter(TEXT("CharSpacing"))).IsValid())
	{
		ReadBiTypeValue(PIter->m_Value,
						m_CharSpacing,
						TEXT("font character spacing"));
	}
}

// -----
// Font
// -----
TFont::TFont()
{
	m_bAllocated = false;
}

void TFont::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated =  false;
	}
}

void TFont::Allocate(	const TFontCreationStruct&	CreationStruct,
						const FSIZE*				SCharSizes,
						const FSIZE&				SMaxCharSize)
{
	Release();

	try
	{
		m_Color = CreationStruct.m_Color;

		m_CharSpacing = CreationStruct.m_CharSpacing;

		memcpy(m_CharSizes, SCharSizes, sizeof(m_CharSizes));
		m_MaxCharSize = SMaxCharSize;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TFont::GetTextSize(const KString& Text, FSIZE& RSize) const
{
	DEBUG_VERIFY_ALLOCATION;

	RSize.Set(0, 0);

	float fRowSize = 0.0f;

	for(size_t i = 0 ; i < Text.GetLength() ; i++)
	{
		if(KString::IsSpecChar(Text[i]))
		{
			if(Text[i] == TEXT('\n'))
			{
				if(fRowSize > RSize.cx)
					RSize.cx = fRowSize;

				RSize.cy += m_MaxCharSize.cy;

				fRowSize = 0.0f;
			}

			continue;
		}

		if(fRowSize > 0.0f)
			fRowSize += m_CharSpacing.cx, RSize.cy += m_CharSpacing.cy;

		fRowSize += m_CharSizes[(BYTE)Text[i]].cx;
	}

	// Last char width
	if(fRowSize > RSize.cx)
		RSize.cx = fRowSize;

	// Last char height
	if(!Text.IsEmpty())
		RSize.cy += m_MaxCharSize.cy;
}

FSIZE TFont::GetTextSize(const KString& Text) const
{
	FSIZE Size;
	GetTextSize(Text, Size);

	return Size;
}