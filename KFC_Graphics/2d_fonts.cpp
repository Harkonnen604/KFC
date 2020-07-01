#include "kfc_graphics_pch.h"
#include "2d_fonts.h"

#include <KFC_GUI\bitmap_image.h>
#include <KFC_GUI\gui_font_storage.h>
#include <KFC_GUI\gui.h>
#include "graphics_consts.h"
#include "graphics_device_globals.h"
#include "graphics_state_manager.h"

// ---------------
// 2D font source
// ---------------
T2DFontSource::T2DFontSource()
{
}

void T2DFontSource::Load(	TInfoNodeConstIterator	InfoNode,
							const KString&			NamePrefix,
							LPCTSTR					pValueName)
{
	TInfoParameterConstIterator PIter;

	// Loading GUI font
	g_GUIFontStorage.LoadObject(InfoNode,
								NamePrefix + TEXT("GUIFont"),
								m_GUIFont,
								true);

	// Getting filename
	if((PIter = InfoNode->FindParameter(NamePrefix + TEXT("FileName"))).IsValid())
	{
		if(m_GUIFont.IsAllocated())
		{
			INITIATE_DEFINED_FAILURE((KString)TEXT("Ambiguity: both ") + pValueName + TEXT("GUI font and filename provided."));
		}

		if(!SetFileName(PIter->m_Value))
		{
			INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid font ") + pValueName + TEXT("filename: \"") + m_FileName + TEXT("\"."));
		}
	}
	else if(!m_GUIFont.IsAllocated())
	{
		INITIATE_DEFINED_FAILURE((KString)TEXT("Neither ") + pValueName + TEXT("GUI font, nor ") + pValueName + TEXT("filename provided."));
	}
}

bool T2DFontSource::SetFileName(const KString& SFileName)
{
	m_FileName = FILENAME_TOKENS.Process(SFileName);

	return	FileExists(m_FileName + TEXT(".tga")) &&
			FileExists(m_FileName + TEXT(".txt"));
}

// -----------
// Font image
// -----------
TFontImage::TFontImage()
{
	m_bAllocated = false;
}

void TFontImage::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		TImage::Release();
	}
}

void TFontImage::Allocate(const T2DFontSource& Source)
{
	Release();

	try
	{
		if(!Source.m_FileName.IsEmpty()) // filename allocation
		{
			DEBUG_VERIFY(!Source.m_GUIFont.IsAllocated());

			TImage Image;
			Image.Load(Source.m_FileName + TEXT(".tga"));

			KString Alphabet;
			TFile(	Source.m_FileName + TEXT(".txt"),
					FOF_READ | FOF_TEXT).ReadString(Alphabet);

			size_t szCurX;
			size_t i;

			const DWORD* pSrcData = Image.GetDataPtr();

			// Retrieving char sizes
			m_MaxCharSize.Set(0, (float)Image.GetSize().cy);

			memset(m_CharSizes, 0, sizeof(m_CharSizes));

			szCurX = 0;
			for(i = 0 ; i < Alphabet.GetLength() ; i++)
			{
				BYTE bChar = (BYTE)Alphabet[i];

				size_t szStartX = szCurX;

				while(	szCurX < Image.GetSize().cx &&
						*pSrcData & 0x00FFFFFF)
				{
					szCurX++, pSrcData++;
				}
				
				m_CharSizes[bChar].cx = (float)(szCurX - szStartX);

				if(m_CharSizes[bChar].cx > m_MaxCharSize.cx)
					m_MaxCharSize.cx = m_CharSizes[bChar].cx;

				m_CharSizes[bChar].cy = (float)Image.GetSize().cy;

				if(szCurX == Image.GetSize().cx)
				{
					if(i < Alphabet.GetLength() - 1)
						INITIATE_DEFINED_FAILURE(TEXT("Not enough characters in font image."));

					break;
				}

				szCurX++, pSrcData++; // skipping the delimeter
			}

			if(szCurX < Image.GetSize().cx)
				INITIATE_DEFINED_FAILURE(TEXT("Redundant characters in font image."));

			// Allocating
			TImage::Allocate(SZSIZE((size_t)m_MaxCharSize.cx * 16, (size_t)m_MaxCharSize.cy * 16));

			// Clearing
			Fill(0x00FFFFFF);

			// Blitting chars
			szCurX = 0;
			
			for(i = 0 ; i < Alphabet.GetLength() ; i++)
			{
				BYTE bChar = (BYTE)Alphabet[i];

				const FSIZE& CharSize = m_CharSizes[bChar];

				const DWORD* pSrcData = Image.GetDataPtr(SZPOINT(szCurX, 0));

				DWORD* pDstData = GetDataPtr(SZPOINT(	(size_t)m_MaxCharSize.cx * (bChar &		0xF),
														(size_t)m_MaxCharSize.cy * (bChar >>	4)));
				
				for(size_t y = (size_t)CharSize.cy ; y ; y--)
				{
					memcpy(pDstData, pSrcData, (size_t)CharSize.cx * sizeof(DWORD));
					pSrcData += Image.GetSize().cx, pDstData += GetSize().cx;
				}

				szCurX += (size_t)CharSize.cx + 1;
			}
		}
		else // GUI font allocation
		{
			DEBUG_VERIFY(Source.m_FileName.IsEmpty());

			// Evaluating max char dimensions
			m_MaxCharSize.Set(0, 0);

			for(size_t i=0 ; i < 256 ; i++)
			{
				SZSIZE CharSize;
				Source.m_GUIFont->GetCharSize((TCHAR)i, CharSize);

				m_CharSizes[i].cx = (float)CharSize.cx;
				m_CharSizes[i].cy = (float)CharSize.cy;

				if(m_CharSizes[i].cx > m_MaxCharSize.cx)
					m_MaxCharSize.cx = m_CharSizes[i].cx;

				if(m_CharSizes[i].cy > m_MaxCharSize.cy)
					m_MaxCharSize.cy = m_CharSizes[i].cy;
			}

			// Allocating
			TImage::Allocate(SZSIZE((size_t)m_MaxCharSize.cx * 16, (size_t)m_MaxCharSize.cy * 16));

			// Allocating temp bitmap
			TBitmapImage Bitmap;
			Bitmap.Allocate(TBitmapImageDesc(SZSIZE((size_t)m_MaxCharSize.cx, (size_t)m_MaxCharSize.cy)));

			// Acquiring transfer pointers
			const DWORD* pSrc =	(DWORD*)Bitmap.GetDesc().m_pData +
									(Bitmap.GetDesc().m_Size.cy - 1) *
										Bitmap.GetDesc().m_Size.cx;

			const size_t szSrcDelta = Bitmap.GetDesc().m_Size.cx << 1;
			const size_t szDstDelta = GetSize().cx - Bitmap.GetDesc().m_Size.cx;

			DWORD* pDst = GetDataPtr();

			// Drawing context
			TDC ScreenDC((HWND)NULL);
			TDC DC(CreateCompatibleDC(ScreenDC));
			ScreenDC.Release();

			TGDIObjectSelector	Sel0(DC, Bitmap.GetBitmap());
			TGDIObjectSelector	Sel1(DC, *Source.m_GUIFont);
			TTextColorSelector	Sel2(DC, RGB(255, 255, 255));
			TBkColorSelector	Sel3(DC, RGB(0,   0,   0));
			TBkModeSelector		Sel4(DC, TRANSPARENT);

			// Drawing characters to image
			TCHAR Text[2];
			Text[1] = 0;			

			HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

			for(size_t y=0 ; y<16 ; y++)
			{
				for(size_t x=0 ; x<16 ; x++)
				{
					// Erasing background
					FillRect(	DC,
								SZRECT(	0,
										0,
										(size_t)m_MaxCharSize.cx,
										(size_t)m_MaxCharSize.cy),
								hBlackBrush);

					// Drawing the character
					Text[0] = (TCHAR)((y<<4) | x);
					TextOut(DC, 0, 0, Text, 1);

					// Copying bitmap contents to self
					const DWORD* pCurSrc = pSrc;
					DWORD* pCurDst =	pDst +
											(((y * (size_t)m_MaxCharSize.cy) * GetSize().cx) +
											x * (size_t)m_MaxCharSize.cx);
					for(size_t cy = (size_t)m_MaxCharSize.cy ; cy ; cy--)
					{
						for(size_t cx = (size_t)m_MaxCharSize.cx ; cx ; cx--)
							*pCurDst++ = ((*pCurSrc++ & 0xFF) << 24) | 0x00FFFFFF;

						pCurSrc -= szSrcDelta;
						pCurDst += szDstDelta;
					}
				}
			}
		}

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

// -----------------------------
// Texture font creation struct
// -----------------------------
TTextureFontCreationStruct::TTextureFontCreationStruct()
{	
}

void TTextureFontCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	TFontCreationStruct::Load(InfoNode);

	// Loading the sources
	m_Source.Load(InfoNode);
}

// -------------
// Texture font
// -------------
TFont* TTextureFont::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == FONT_TYPE_FLAT);

	return new TTextureFont;
}

TTextureFont::TTextureFont()
{
	m_bAllocated = false;
}

void TTextureFont::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_Texture.Release();

		TFont::Release();
	}
}

void TTextureFont::Allocate(const TTextureFontCreationStruct& CreationStruct)
{
	Release();

	try
	{
		TFontImage Image;
		
		Image.Allocate(CreationStruct.m_Source);

		TFont::Allocate(CreationStruct,
						Image.GetCharSizes(),
						Image.GetMaxCharSize());
		
		m_Texture.CreateFromImage(	Image,
									g_GraphicsDeviceGlobals.m_16BPPAlphaTextureFormat,
									NULL,
									D3DPOOL_MANAGED);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TTextureFont::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());
	
	TTextureFontCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode);

	Allocate(CreationStruct);
}

void TTextureFont::DrawChar(	TCHAR				cChar,
								FPOINT				DstCoords,
								const TD3DColor&	Color) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;

	if(Compare(TargetColor.m_fAlpha, 0.0f) == 0)
		return;

	RoundUp(DstCoords);

	const BYTE bChar = (BYTE)cChar;

	const FSIZE& CharSize = GetCharSizes()[bChar];

	const FPOINT CharOrigin(((size_t)bChar &	0xF)	* GetMaxCharSize().cx,
							((size_t)bChar >>	4)		* GetMaxCharSize().cy);

	FRECT SrcRect;
	SrcRect.m_Left		= m_Texture.GetTextureCoordX(CharOrigin.x);
	SrcRect.m_Top		= m_Texture.GetTextureCoordY(CharOrigin.y);
	SrcRect.m_Right		= m_Texture.GetTextureCoordX(CharOrigin.x + CharSize.cx);
	SrcRect.m_Bottom	= m_Texture.GetTextureCoordY(CharOrigin.y + CharSize.cy);

	m_Texture.DrawRect(	DstCoords,
						TargetColor,
						&CharSize,
						&SrcRect);
}

void TTextureFont::DrawText(const KString&		Text,
							FPOINT				DstCoords,
							const TD3DColor&	Color) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;

	if(Compare(TargetColor.m_fAlpha, 0.0f) == 0)
		return;

	RoundUp(DstCoords);

	size_t szLength = Text.GetNoSpecCharactersLength();
	if(szLength == 0)
		return;

	DEBUG_VERIFY(szLength <= g_GraphicsConsts.m_szMaxGraphicsStripRects);

	FPOINT CurDstCoords(DstCoords.x - 0.5f, DstCoords.y - 0.5f);

	// Preparing vertex buffer	
	{
		const TD3DColor TargetColor = Color * m_Color;

		TTLVertex<1>* pData;

		TRectsStripVertexBufferLocker Locker0(	g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer,
												pData,
												D3DLOCK_DISCARD,
												&SZSEGMENT(0, szLength << 2));

		for(size_t i=0 ; i < Text.GetLength() ; i++)
		{
			if(KString::IsSpecChar(Text[i]))
			{
				if(Text[i] == TEXT('\n'))
				{
					CurDstCoords.x =  DstCoords.x - 0.5f;
					CurDstCoords.y += GetMaxCharSize().cy;
				}

				continue;
			}
			
			const BYTE bChar = (BYTE)Text[i];

			const FSIZE& CharSize = GetCharSizes()[(BYTE)Text[i]];

			const FPOINT CharOrigin(((size_t)bChar &	0xF)	* GetMaxCharSize().cx,
									((size_t)bChar >>	4)		* GetMaxCharSize().cy);

			FRECT SrcRect;
			SrcRect.m_Left		= m_Texture.GetTextureCoordX(CharOrigin.x);
			SrcRect.m_Top		= m_Texture.GetTextureCoordY(CharOrigin.y);
			SrcRect.m_Right		= m_Texture.GetTextureCoordX(CharOrigin.x + CharSize.cx);
			SrcRect.m_Bottom	= m_Texture.GetTextureCoordY(CharOrigin.y + GetMaxCharSize().cy);

			pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y);
			pData->m_Color = TargetColor;
			pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Top);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x + CharSize.cx, CurDstCoords.y);
			pData->m_Color = TargetColor;
			pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Top);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y + CharSize.cy);
			pData->m_Color = TargetColor;
			pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Bottom);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x + CharSize.cx, CurDstCoords.y + CharSize.cy);
			pData->m_Color = TargetColor;
			pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Bottom);
			pData++;

			// To the next character
			CurDstCoords.x += CharSize.cx;
			
			CurDstCoords.x += GetCharSpacing().cx;
			CurDstCoords.y += GetCharSpacing().cy;
		}
	}

	// Rendering
	{
		m_Texture.Install();

		g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer.Install();

		g_GraphicsDeviceGlobals.m_RectsStripIndexBuffer.Install();
		
		g_GraphicsStateManager.SetStateBlockNode
			(	&g_GraphicsStateManager.m_Textured2DPrimitiveStateBlockNodes
				[true][true][Compare(TargetColor.m_fAlpha, 1.0f) < 0]);
	
		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST,
																			0,
																			0,
																			szLength << 2,
																			0,
																			szLength << 1));
	}
}

// ------------------------------------
// Texture shadow font creation struct
// ------------------------------------
TTextureShadowFontCreationStruct::TTextureShadowFontCreationStruct()
{
	m_ShadowColor = BlackColor();
	m_ShadowOffset.Set(0.0f, 0.0f);
}

void TTextureShadowFontCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	TFontCreationStruct::Load(InfoNode);

	TInfoParameterConstIterator PIter;

	// Loading the sources
	m_BaseSource.	Load(InfoNode, TEXT("Base"),	TEXT("base "));
	m_ShadowSource.	Load(InfoNode, TEXT("Shadow"),	TEXT("shadow "));

	// Getting shadow color
	if((PIter = InfoNode->FindParameter(TEXT("ShadowColor"))).IsValid())
	{
		ReadColor(	PIter->m_Value,
					m_ShadowColor,
					TEXT("font shadow color"));
	}

	// Getting shadow offset
	if((PIter = InfoNode->FindParameter(TEXT("ShadowOffset"))).IsValid())
	{
		ReadBiTypeValue(PIter->m_Value,
						m_ShadowOffset,
						TEXT("font shadow offset"));
	}
}

// --------------------
// Texture shadow font
// --------------------
TFont* TTextureShadowFont::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == FONT_TYPE_FLAT_SHADOW);

	return new TTextureShadowFont;
}

TTextureShadowFont::TTextureShadowFont()
{
	m_bAllocated = false;
}

void TTextureShadowFont::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_ShadowTexture.Release();
		m_BaseTexture.	Release();

		TFont::Release();
	}
}

void TTextureShadowFont::Allocate(const TTextureShadowFontCreationStruct& CreationStruct)
{
	Release();

	try
	{
		TFontImage Image;

		// Base
		Image.Allocate(CreationStruct.m_BaseSource);
		
		TFont::Allocate(CreationStruct,
						Image.GetCharSizes(),
						Image.GetMaxCharSize());

		m_BaseTexture.CreateFromImage(	Image,
										g_GraphicsDeviceGlobals.m_16BPPAlphaTextureFormat);

		// Shadow
		Image.Allocate(CreationStruct.m_ShadowSource);

		memcpy(m_ShadowCharSizes, Image.GetCharSizes(), sizeof(m_ShadowCharSizes));
		m_MaxShadowCharSize = Image.GetMaxCharSize();

		m_ShadowTexture.CreateFromImage(Image,
										g_GraphicsDeviceGlobals.m_16BPPAlphaTextureFormat);

		m_ShadowColor = CreationStruct.m_ShadowColor;

		RoundUp(m_ShadowOffset = CreationStruct.m_ShadowOffset);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TTextureShadowFont::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());

	TTextureShadowFontCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode);

	Allocate(CreationStruct);
}

void TTextureShadowFont::DrawChar(	TCHAR				cChar,
									FPOINT				DstCoords,
									const TD3DColor&	Color) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;

	if(Compare(TargetColor.m_fAlpha, 0.0f) == 0)
		return;

	const TD3DColor TargetShadowColor = TargetColor * m_ShadowColor;
	
	RoundUp(DstCoords);
	
	const BYTE bChar = (BYTE)cChar;

	// --- Shadow rendering ---
	if(Compare(TargetShadowColor.m_fAlpha, 0.0f) > 0)
	{
		const FSIZE& CharSize = m_ShadowCharSizes[bChar];

		const FPOINT CharOrigin(((size_t)bChar &	0xF)	* m_MaxShadowCharSize.cx,
								((size_t)bChar >>	4)		* m_MaxShadowCharSize.cy);

		FRECT SrcRect;
		SrcRect.m_Left		= m_ShadowTexture.GetTextureCoordX(CharOrigin.x);
		SrcRect.m_Top		= m_ShadowTexture.GetTextureCoordY(CharOrigin.y);
		SrcRect.m_Right		= m_ShadowTexture.GetTextureCoordX(CharOrigin.x + CharSize.cx);
		SrcRect.m_Bottom	= m_ShadowTexture.GetTextureCoordY(CharOrigin.y + CharSize.cy);

		m_ShadowTexture.DrawRect(	FPOINT(	DstCoords.x + m_ShadowOffset.cx,
											DstCoords.y + m_ShadowOffset.cy),
									TargetShadowColor,
									&CharSize,
									&SrcRect);
	}

	// --- Base rendering ---
	{
		const FSIZE& CharSize = GetCharSizes()[bChar];

		const FPOINT CharOrigin(((size_t)bChar &	0xF)	* GetMaxCharSize().cx,
								((size_t)bChar >>	4)		* GetMaxCharSize().cy);

		FRECT SrcRect;
		SrcRect.m_Left		= m_BaseTexture.GetTextureCoordX(CharOrigin.x);
		SrcRect.m_Top		= m_BaseTexture.GetTextureCoordY(CharOrigin.y);
		SrcRect.m_Right		= m_BaseTexture.GetTextureCoordX(CharOrigin.x + CharSize.cx);
		SrcRect.m_Bottom	= m_BaseTexture.GetTextureCoordY(CharOrigin.y + CharSize.cy);

		m_BaseTexture.DrawRect(	DstCoords,
								TargetColor,
								&CharSize,
								&SrcRect);
	}
}

void TTextureShadowFont::DrawText(	const KString&		Text,
									FPOINT				DstCoords,
									const TD3DColor&	Color) const
{
	DEBUG_VERIFY_ALLOCATION;

	const TD3DColor TargetColor = Color * m_Color;

	if(Compare(TargetColor.m_fAlpha, 0.0f) == 0)
		return;

	size_t szLength = Text.GetNoSpecCharactersLength();

	if(szLength == 0)
		return;

	DEBUG_VERIFY(szLength <= g_GraphicsConsts.m_szMaxGraphicsStripRects);

	RoundUp(DstCoords);	

	// --- Shadow rendering ---
	const TD3DColor TargetShadowColor = TargetColor * m_ShadowColor;

	if(Compare(TargetShadowColor.m_fAlpha, 0.0f) > 0)
	{
		// Preparing vertex buffer
		{
			TTLVertex<1>* pData;

			TRectsStripVertexBufferLocker Locker0(	g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer,
													pData,
													D3DLOCK_DISCARD,
													&SZSEGMENT(0, szLength << 2));

			FPOINT CurDstCoords(DstCoords.x + m_ShadowOffset.cx - 0.5f,
								DstCoords.y + m_ShadowOffset.cy - 0.5f);

			for(size_t i=0 ; i < Text.GetLength() ; i++)
			{
				if(KString::IsSpecChar(Text[i]))
				{
					if(Text[i] == TEXT('\n'))
					{
						CurDstCoords.x =	DstCoords.x + m_ShadowOffset.cx - 0.5f;
						CurDstCoords.y +=	GetMaxCharSize().cy;
					}

					continue;
				}
				
				const BYTE bChar = (BYTE)Text[i];

				const FSIZE& CharSize = GetCharSizes()[bChar];

				const FSIZE& ShadowCharSize = m_ShadowCharSizes[bChar];

				const FPOINT ShadowCharOrigin(	((size_t)bChar &	0xF)	* m_MaxShadowCharSize.cx,
												((size_t)bChar >>	4)		* m_MaxShadowCharSize.cy);

				FRECT SrcRect;
				SrcRect.m_Left		= m_ShadowTexture.GetTextureCoordX(ShadowCharOrigin.x);
				SrcRect.m_Top		= m_ShadowTexture.GetTextureCoordY(ShadowCharOrigin.y);
				SrcRect.m_Right		= m_ShadowTexture.GetTextureCoordX(ShadowCharOrigin.x + ShadowCharSize.cx);
				SrcRect.m_Bottom	= m_ShadowTexture.GetTextureCoordY(ShadowCharOrigin.y + m_MaxShadowCharSize.cy);

				pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y);
				pData->m_Color = TargetShadowColor;
				pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Top);
				pData++;

				pData->SetScreenCoords(CurDstCoords.x + ShadowCharSize.cx, CurDstCoords.y);
				pData->m_Color = TargetShadowColor;
				pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Top);
				pData++;

				pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y + ShadowCharSize.cy);
				pData->m_Color = TargetShadowColor;
				pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Bottom);
				pData++;

				pData->SetScreenCoords(CurDstCoords.x + ShadowCharSize.cx, CurDstCoords.y + ShadowCharSize.cy);
				pData->m_Color = TargetShadowColor;
				pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Bottom);
				pData++;

				// To the next character
				CurDstCoords.x += CharSize.cx;
				
				CurDstCoords.x += GetCharSpacing().cx;
				CurDstCoords.y += GetCharSpacing().cy;
			}
		}

		// Rendering
		{
			m_ShadowTexture.Install();

			g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer.Install();

			g_GraphicsDeviceGlobals.m_RectsStripIndexBuffer.Install();
			
			g_GraphicsStateManager.SetStateBlockNode
				(	&g_GraphicsStateManager.m_Textured2DPrimitiveStateBlockNodes
					[true][true][Compare(TargetColor.m_fAlpha, 1.0f) < 0]);
		
			DEBUG_EVALUATE_VERIFY
				(!g_GraphicsDeviceGlobals.m_pD3DDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST,
																				0,
																				0,
																				szLength << 2,
																				0,
																				szLength << 1));
		}
	}

	// --- Base rendering ---

	// Preparing vertex buffer
	{
		TTLVertex<1>* pData;

		TRectsStripVertexBufferLocker Locker0(	g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer,
												pData,
												D3DLOCK_DISCARD,
												&SZSEGMENT(0, szLength << 2));

		FPOINT CurDstCoords(DstCoords.x - 0.5f,
							DstCoords.y - 0.5f);

		for(size_t i=0 ; i < Text.GetLength() ; i++)
		{
			if(KString::IsSpecChar(Text[i]))
			{
				if(Text[i] == TEXT('\n'))
				{
					CurDstCoords.x =	DstCoords.x - 0.5f;
					CurDstCoords.y +=	GetMaxCharSize().cy;
				}

				continue;
			}
			
			const BYTE bChar = (BYTE)Text[i];

			const FSIZE& CharSize = GetCharSizes()[bChar];

			const FPOINT CharOrigin(((size_t)bChar &	0xF)	* GetMaxCharSize().cx,
									((size_t)bChar >>	4)		* GetMaxCharSize().cy);

			FRECT SrcRect;
			SrcRect.m_Left		= m_BaseTexture.GetTextureCoordX(CharOrigin.x);
			SrcRect.m_Top		= m_BaseTexture.GetTextureCoordY(CharOrigin.y);
			SrcRect.m_Right		= m_BaseTexture.GetTextureCoordX(CharOrigin.x + CharSize.cx);
			SrcRect.m_Bottom	= m_BaseTexture.GetTextureCoordY(CharOrigin.y + GetMaxCharSize().cy);

			pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y);
			pData->m_Color = Color;
			pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Top);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x + CharSize.cx, CurDstCoords.y);
			pData->m_Color = Color;
			pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Top);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x, CurDstCoords.y + CharSize.cy);
			pData->m_Color = Color;
			pData->m_TextureCoords[0].Set(SrcRect.m_Left, SrcRect.m_Bottom);
			pData++;

			pData->SetScreenCoords(CurDstCoords.x + CharSize.cx, CurDstCoords.y + CharSize.cy);
			pData->m_Color = Color;
			pData->m_TextureCoords[0].Set(SrcRect.m_Right, SrcRect.m_Bottom);
			pData++;

			// To the next character
			CurDstCoords.x += CharSize.cx;

			CurDstCoords.x += GetCharSpacing().cx;
			CurDstCoords.y += GetCharSpacing().cy;
		}
	}

	// Rendering
	{
		m_BaseTexture.Install();

		g_GraphicsDeviceGlobals.m_RectsStripVertexBuffer.Install();

		g_GraphicsDeviceGlobals.m_RectsStripIndexBuffer.Install();
		
		g_GraphicsStateManager.SetStateBlockNode
			(	&g_GraphicsStateManager.m_Textured2DPrimitiveStateBlockNodes
				[true][true][Compare(TargetColor.m_fAlpha, 1.0f) < 0]);
	
		DEBUG_EVALUATE_VERIFY
			(!g_GraphicsDeviceGlobals.m_pD3DDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST,
																			0,
																			0,
																			szLength << 2,
																			0,
																			szLength << 1));
	}
}
