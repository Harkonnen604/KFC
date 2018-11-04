#include "kfc_sdl_pch.h"
#include "sdl_resource_storage.h"

#include <KFC_Common/file.h>
#include <KFC_Common/structured_info.h>
#include "sdl_consts.h"
#include "sdl_common.h"

// -------------------
// SDL resouce ID map
// -------------------
void T_SDL_ResourceID_Map::Parse(LPCTSTR pFileName)
{
	Clear();

	TFile File(pFileName, FOF_TEXTREAD);

	size_t szLine = 0;
	
	bool bLast = false;

	while(!File.IsEndOfFile())
	{
		KString Line = File.ReadString();

		if(Line.Find("/*") != UINT_MAX || Line.Find("*/") != UINT_MAX)
			INITIATE_DEFINED_FAILURE("C-styled '/* ... */' comments are not supported in SDL resource ID header files.");

		size_t i;

		if((i = Line.Find("//")) != UINT_MAX)
			Line.SetLeft(i);

		Line.TrimSingleSpace();
		
		if(Line.IsEmpty())
			continue;
			
		if(bLast)
			INITIATE_DEFINED_FAILURE("'#endif' is assumed to be the last non-empty line of SDL resource ID header file.");

		if(szLine == 0)
		{
			if(!Line.DoesStart("#ifndef "))
				INITIATE_DEFINED_FAILURE("'#ifndef ' assumed in the 1st non-empty line of SDL resource ID header file.");
		}
		else if(szLine == 1)
		{
			if(!Line.DoesStart("#define "))
				INITIATE_DEFINED_FAILURE("'#define ' assumed in the 2nd non-empty line of SDL resource ID header file.");
		}
		else
		{
			if(Line == "#endif")
			{
				bLast = true;
			}
			else
			{
				if(!Line.DoesStart("#define "))
					INITIATE_DEFINED_FAILURE("'#define' assumed in every internal non-empty line of SDL resource ID header file.");
					
				LPCTSTR s = (LPCTSTR)Line;
				
				i = 8;
		
				KString Name;
		
				for( ; _istalnum(s[i]) || s[i] == '_' ; i++)
					Name += s[i];
		
				if(!_istspace(s[i]))
					INITIATE_DEFINED_FAILURE("Space assumed after SDL resource ID name.");
		
				i++;
		
				size_t szValue = 0;
		
				bool hd = false;
		
				for( ; s[i] ; i++)
				{
					if(s[i] == '(' || s[i] == ')')
						continue;
						
					if(!isdigit(s[i]) && !isspace(s[i]))
						INITIATE_DEFINED_FAILURE("Only digits, brackets and spaces are allowed for SDL resource ID value.");

					if(isdigit(s[i]))
						szValue *= 10, szValue += s[i] - '0';

					if(szValue > g_SDL_Consts.m_szMaxResourceID)
						INITIATE_DEFINED_FAILURE((KString)"SDL resource IDs cannot be greater than " + g_SDL_Consts.m_szMaxResourceID + ".");
		
					hd = true;
				}
		
				if(!hd)
					INITIATE_DEFINED_FAILURE("No digits encountered within SDL resource ID value.");
					
				Add(Name, szValue);
			}
		}

		szLine++;		
	}

	if(!bLast)
		INITIATE_DEFINED_FAILURE("'#endif' is assumed to be the last non-empty line of SDL resource ID header file.");

	#ifdef _DEBUG
		TestDupes();
	#endif // _DEBUG
}

struct TResID_Pair
{
public:
	KString	m_Name;
	KString	m_Prefix;
	size_t	m_szID;

public:
	TResID_Pair& Set(const KString& Name, size_t szID)
	{
		m_Name		= Name;
		m_Prefix	= m_Name.Left(m_Name.Find('_'));
		m_szID		= szID;

		return *this;
	}
};

inline int Compare(const TResID_Pair& Pair1, const TResID_Pair& Pair2)
{
	int d;

	if(d = Compare(Pair1.m_Prefix, Pair2.m_Prefix))
		return d;

	if(d = Compare(Pair1.m_szID, Pair2.m_szID))
		return d;

	if(d = Compare(Pair1.m_Name, Pair2.m_Name))
		return d;

	return 0;
}

void T_SDL_ResourceID_Map::TestDupes() const
{
	TArray<TResID_Pair> Pairs;

	for(TConstTireWalker<size_t> Walker(m_Storage) ; Walker ; ++Walker)
		Pairs.Add().Set(Walker.GetPath(), *Walker);

	Pairs.Sort();

	for(size_t i = 1 ; i < Pairs.GetN() ; i++)
	{
		if(Pairs[i-1].m_Prefix == Pairs[i].m_Prefix && Pairs[i-1].m_szID == Pairs[i].m_szID)
		{
			INITIATE_DEFINED_FAILURE(	(KString)"Duplicate SDL resource ID detected: \"" +
											Pairs[i-1].m_Name + "\" and \"" + Pairs[i].m_Name + "\".");
		}
	}
}

// ---------------------
// SDL resource storage
// ---------------------
T_SDL_InterfaceDefinition T_SDL_ResourceStorage::ms_EmptyInterfaceDef;

void T_SDL_ResourceStorage::Clear()
{
	m_ImagesCache.	Clear();
	m_FontsCache.	Clear();

	m_InterfaceDefs.Clear();
	m_MultiImages.	Clear();
	m_Images.		Clear();
	m_Fonts.		Clear();
	m_Strings.		Clear();
	m_Colors.		Clear();
	m_Values.		Clear();
	m_StateValues.	Clear();
}

const T_SGE_Font* T_SDL_ResourceStorage::LoadFont(	LPCTSTR			pString,
													const TTokens&	ValueTokens,
													const TTokens&	ColorTokens,
													const TTokens&	StringTokens)
{
	KStrings Elements(pString, "|", false);

	if(Elements.GetN() != 2)
		INITIATE_DEFINED_FAILURE((KString)"Invalid SDL font definition format: \"" + pString + "\".");

	Elements.TrimAll();

	TCachedFont::TKey Key(StringTokens(Elements[0]), ReadFromString<size_t>(ValueTokens(Elements[1])));

	T_AVL_Storage<TCachedFont>::TIterator Iter = m_FontsCache.Find(Key);

	if(!Iter.IsValid())
		Iter = m_FontsCache.Add(Key);

	return &Iter->m_Font;
}

const T_SDL_Image* T_SDL_ResourceStorage::LoadImage(LPCTSTR			pString,
													const TTokens&	ValueTokens,
													const TTokens&	ColorTokens,
													const TTokens&	StringTokens)
{
	KStrings Elements(pString, "|", false);

	if(Elements.GetN() < 1 || Elements.GetN() > 2)
		INITIATE_DEFINED_FAILURE((KString)"Invalid SDL image definition format: \"" + pString + "\".");		

	Elements.TrimAll();

	TCachedImage::TKey Key(StringTokens(Elements[0]), 1 < Elements.GetN() ? ReadRGB(ColorTokens(Elements[1])) : UINT_MAX);

	T_AVL_Storage<TCachedImage>::TIterator Iter = m_ImagesCache.Find(Key);

	if(!Iter.IsValid())
		Iter = m_ImagesCache.Add(Key);

	return &Iter->m_Image;
}

const T_SDL_MultiImage* T_SDL_ResourceStorage::LoadMultiImage(	LPCTSTR			pString,
																const TTokens&	ValueTokens,
																const TTokens&	ColorTokens,
																const TTokens&	StringTokens)
{
	KStrings Elements(pString, "|", false);

	if(Elements.GetN() < 1 || Elements.GetN() > 2)
		INITIATE_DEFINED_FAILURE((KString)"Invalid SDL image definition format: \"" + pString + "\".");		

	Elements.TrimAll();

	bool bSingle = false;

	if(1 < Elements.GetN())
	{
		KFC_VERIFY(Elements[1] == "single");
		bSingle = true;
	}

	TCachedMultiImage::TKey Key(StringTokens(Elements[0]), bSingle);

	T_AVL_Storage<TCachedMultiImage>::TIterator Iter = m_MultiImagesCache.Find(Key);

	if(!Iter.IsValid())
		Iter = m_MultiImagesCache.Add(Key);

	return &Iter->m_Image;
}

void T_SDL_ResourceStorage::Load(LPCTSTR pFileName)
{
	Clear();

	TStructuredInfo Info(pFileName);

	T_SDL_ResourceID_Map ID_Map(GetFilePath(pFileName) + Info.GetRootNode()->GetParameterValue("ResourceID_Header"));
	
	TTokens ValueTokens;
	TTokens ColorTokens;
	TTokens StringTokens;

	ValueTokens.Add("[ResX]",	g_SDL_Consts.m_Resolution.cx);
	ValueTokens.Add("[ResY]",	g_SDL_Consts.m_Resolution.cy);
	ValueTokens.Add("[BPP]",	g_SDL_Consts.m_szBPP);

	StringTokens.Add("[ResX]",	g_SDL_Consts.m_Resolution.cx);
	StringTokens.Add("[ResY]",	g_SDL_Consts.m_Resolution.cy);
	StringTokens.Add("[BPP]",	g_SDL_Consts.m_szBPP);

	// State values
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "StateValues");

		size_t szMaxID = 0;

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);

		m_StateValues.SetN(szMaxID + 1);

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			m_StateValues[ID_Map[PIter->m_Name]] = ReadFromString<double>(ValueTokens(PIter->m_Value));
	}

	// Values
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "Values");

		size_t szMaxID = 0;

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);
			
		m_Values.SetN(szMaxID + 1);
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
		{
			double dValue = ReadFromString<double>(ValueTokens(PIter->m_Value));

			m_Values[ID_Map[PIter->m_Name]] = dValue;

			ValueTokens.Add((KString)'[' + PIter->m_Name + ']', dValue);
		}
	}

	// Colors
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "Colors");

		size_t szMaxID = 0;

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);

		m_Colors.SetN(szMaxID + 1);

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
		{
			UINT32 uiColor = ReadRGB(ColorTokens(PIter->m_Value));

			m_Colors[ID_Map[PIter->m_Name]] = uiColor;
			
			ColorTokens.Add((KString)'[' + PIter->m_Name + ']', WriteRGB(uiColor));
		}
	}

	// Strings
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "Strings");
		
		size_t szMaxID = 0;

		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);
			
		m_Strings.SetN(szMaxID + 1);
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
		{
			const KString& String = StringTokens(ValueTokens(PIter->m_Value));

			m_Strings[ID_Map[PIter->m_Name]] = String;

			StringTokens.Add((KString)'[' + PIter->m_Name + ']', String);
		}
	}

	// Fonts
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "Fonts");
		
		size_t szMaxID = 0;
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);
			
		m_Fonts.SetNAndZeroNewData(szMaxID + 1);
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			m_Fonts[ID_Map[PIter->m_Name]] = LoadFont(PIter->m_Value, ValueTokens, ColorTokens, StringTokens);
	}

	// Images
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "Images");
		
		size_t szMaxID = 0;
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);
			
		m_Images.SetNAndZeroNewData(szMaxID + 1);
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			m_Images[ID_Map[PIter->m_Name]] = LoadImage(PIter->m_Value, ValueTokens, ColorTokens, StringTokens);
	}

	// Multi-images
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "MultiImages");
		
		size_t szMaxID = 0;
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			UpdateMax(szMaxID, ID_Map[PIter->m_Name]);
			
		m_MultiImages.SetNAndZeroNewData(szMaxID + 1);
		
		FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
			m_MultiImages[ID_Map[PIter->m_Name]] = LoadMultiImage(PIter->m_Value, ValueTokens, ColorTokens, StringTokens);
	}

	// Interface defs
	{
		TInfoNodeConstIterator Node = Info.GetNode(Info.GetRootNode(), "InterfaceDefs");

		size_t szMaxID = 0;

		FOR_EACH_TREE_LEVEL(Node, TInfoNodeConstIterator, NIter)
			UpdateMax(szMaxID, ID_Map[NIter->m_Name]);

		m_InterfaceDefs.SetN(szMaxID + 1);

		FOR_EACH_TREE_LEVEL(Node, TInfoNodeConstIterator, NIter)
			m_InterfaceDefs[ID_Map[NIter->m_Name]].Load(NIter, *this, ID_Map, ValueTokens, ColorTokens, StringTokens);
	}
}
