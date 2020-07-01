#ifndef sprite_h
#define sprite_h

#include <KFC_KTL\subobject_lister.h>
#include <KFC_Common\structured_info.h>
#include "sprite_defs.h"
#include "d3d_color.h"
#include "color_defs.h"
#include "sprite_helpers.h"

// Default sprite state
#define DEFAULT_SPRITE_STATE	(0.0f)

// --------------
// Sprite states
// --------------
struct TSpriteStates
{
private:
	union
	{
		const float*	m_pStates;
		float			m_fState;
	};

	size_t m_szN;

public:
	TSpriteStates();

	TSpriteStates(const float* pSStates, size_t szSN);

	TSpriteStates(float fSState);

	~TSpriteStates()
		{ Release(); }

	bool IsAllocated() const
		{ return m_szN > 0; }

	void Release();

	void Allocate(const float* pSStates, size_t szSN);

	void Allocate(float fSState);

	float GetCurrent() const;

	float operator * () const { return GetCurrent(); }

	TSpriteStates& operator ++ ();	

	TSpriteStates operator + (size_t szStep) const;

	TSpriteStates Inced() const { return ++TSpriteStates(*this); }
	
	// ---------------- TRIVIALS ----------------
	size_t GetN() const { return m_szN; }
};

// -----------------------
// Sprite creation struct
// -----------------------
struct TSpriteCreationStruct
{
	TD3DColor m_Color;


	TSpriteCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);
};

// -------
// Sprite
// -------
class TSprite : public TSubObjectLister<TSprite>
{
private:
	bool m_bAllocated;
	
protected:
	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(const TSpriteCreationStruct& CreationStruct);

public:
	TD3DColor m_Color;


	TSprite();

	virtual ~TSprite()
		{ Release(); }

	virtual void Load(TInfoNodeConstIterator InfoNode) = 0;

	virtual void DrawNonScaled(	const FPOINT&			DstCoords,
								const TD3DColor&		Color	= WhiteColor(),
								const TSpriteStates&	States	= TSpriteStates()) const = 0;

	virtual void DrawRect(	const FRECT&			DstRect,
							const TD3DColor&		Color	= WhiteColor(),
							const TSpriteStates&	States	= TSpriteStates()) const = 0;

	virtual bool HasDefaultSize() const = 0;

	virtual void GetDefaultSize(FSIZE& RSize) const = 0;

	FSIZE GetDefaultSize() const;
};

// ----------------
// Global routines
// ----------------

// Point helpers
FPOINT& ReadPointResized(	const KString&	String,
							FPOINT&			RPoint,
							LPCTSTR			pValueName,
							const FRECT&	ParentRect,
							const FRECT&	Resolution);

// Rect heplers
FRECT& ReadRectResized(	const KString&	String,
						FRECT&			RRect,
						LPCTSTR			pValueName,
						const FRECT&	ParentRect,
						const FRECT&	Resolution);

// Size helpers
FSIZE& ReadSizeResized(	const KString&	String,
						FSIZE&			RSize,
						LPCTSTR			pValueName,
						const FRECT&	ParentRect,
						const FRECT&	Resolution);

#endif // sprite_h