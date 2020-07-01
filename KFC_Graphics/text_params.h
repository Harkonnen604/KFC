#ifndef text_params_h
#define text_params_h

#include <KFC_KTL\text_container.h>
#include <KFC_Common\structured_info.h>
#include "font.h"

// ------------
// Text params
// ------------
struct TTextParams : public TTextContainer
{
	KString	m_Text;
	ALSIZE	m_Alignment;
	flags_t	m_flFlags;


	TTextParams() { Reset(); }

	TTextParams& Reset();

	void Load(TInfoNodeConstIterator InfoNode);

	FPOINT GetOrigin(	const FRECT& Rect,
						const FSIZE& TextSize) const;

	FPOINT GetOrigin(	const FRECT& Rect,
						const TFont& Font) const;

	KString GetText(bool* pRSuccess = NULL) const;

	bool SetText(const KString& Text);

	void SetAlignment	(const ALSIZE& SAlignment)	{ m_Alignment	= SAlignment;	}
	void SetFlags		(flags_t flSFlags)			{ m_flFlags		= flSFlags;		}
};

#endif // text_params_h