#include "kfc_graphics_pch.h"
#include "text_params.h"

// ------------
// Text params
// ------------
TTextParams& TTextParams::Reset()
{
	m_Text.Empty();

	m_Alignment.Set(ALIGNMENT_MIN, ALIGNMENT_MID);

	m_flFlags = 0;

	return *this;
}

void TTextParams::Load(TInfoNodeConstIterator InfoNode)
{
	TInfoParameterConstIterator PIter;

	// Text
	ReadText(InfoNode, TEXT("Text"), m_Text);

	// Alignment
	if((PIter = InfoNode->FindParameter(TEXT("TextAlignment"))).IsValid())
	{
		ReadBiTypeValue(PIter->m_Value,
						m_Alignment,
						TEXT("text alignment"));
	}
}

FPOINT TTextParams::GetOrigin(	const FRECT& Rect,
								const FSIZE& TextSize) const
{
	return GetAlignedOrigin(Rect, TextSize, m_Alignment);
}

FPOINT TTextParams::GetOrigin(	const FRECT& Rect,
								const TFont& Font) const
{
	return GetOrigin(Rect, Font.GetTextSize(m_Text));
}

KString TTextParams::GetText(bool* pRSuccess) const
{
	if(pRSuccess)
		*pRSuccess = true;

	return m_Text;
}

bool TTextParams::SetText(const KString& Text)
{
	m_Text = Text;

	return true;
}