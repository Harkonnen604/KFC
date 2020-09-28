#include "kfc_common_pch.h"
#include "structured_info.h"

#include "common_consts.h"

// ----------------
// Structured info
// ----------------

// Node
TStructuredInfo::TNode::TParameters::TConstIterator
	TStructuredInfo::TNode::FindParameter(	const KString&				Name,
											TParameters::TConstIterator	After) const
{
	for(TParameters::TConstIterator Iter =	After.IsValid() ?
												After.GetNext() :
												m_Parameters.GetFirst() ;
		Iter.IsValid() ;
		++Iter)
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return TParameters::TConstIterator();
}

TStructuredInfo::TNode::TParameters::TConstIterator
	TStructuredInfo::TNode::GetParameter(	const KString&				Name,
											TParameters::TConstIterator	After) const
{
	const TParameters::TConstIterator Iter = FindParameter(Name, After);

	if(!Iter.IsValid())
		INITIATE_DEFINED_FAILURE((KString)TEXT("Parameter \"") + Name + TEXT("\" not found."));

	return Iter;
}

const KString& TStructuredInfo::TNode::GetParameterValue(const KString&					Name,
														 TParameters::TConstIterator	After) const
{
	return GetParameter(Name, After)->m_Value;
}

const KString TStructuredInfo::TNode::GetParameterValue(const KString&				Name,
														const KString&				DefaultValue,
														TParameters::TConstIterator	After) const
{
	const TParameters::TConstIterator PIter = FindParameter(Name, After);

	return PIter.IsValid() ? PIter->m_Value : DefaultValue;
}

bool TStructuredInfo::TNode::HasParameter(	const KString&				Name,
											TParameters::TConstIterator	After) const
{
	return FindParameter(Name, After).IsValid();
}

bool TStructuredInfo::TNode::HasTrueParameter(	const KString&				Name,
												TParameters::TConstIterator	After) const
{
	const TParameters::TConstIterator PIter = FindParameter(Name, After);

	bool bValue;

	return PIter.IsValid() && FromString(PIter->m_Value, bValue) && bValue;
}

TStructuredInfo::TNode::TParameters::TIterator
	TStructuredInfo::TNode::AddParameter(	const KString& Name,
											const KString& Value)
{
	const TParameters::TIterator PIter = m_Parameters.AddLast();

	PIter->Set(Name, Value);

	return PIter;
}

TStructuredInfo::TNode::TParameters::TIterator
	TStructuredInfo::TNode::GetPrevSame(TParameters::TIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToPrev() ; Iter.IsValid() ; Iter.ToPrev())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNode::TParameters::TIterator
	TStructuredInfo::TNode::GetNextSame(TParameters::TIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToNext() ; Iter.IsValid() ; Iter.ToNext())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNode::TParameters::TConstIterator
	TStructuredInfo::TNode::GetPrevSame(TParameters::TConstIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToPrev() ; Iter.IsValid() ; Iter.ToPrev())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNode::TParameters::TConstIterator
	TStructuredInfo::TNode::GetNextSame(TParameters::TConstIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToNext() ; Iter.IsValid() ; Iter.ToNext())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

// Structured info
void TStructuredInfo::Clear()
{
	m_Nodes.Clear(), m_Nodes.AddRoot();
}

void TStructuredInfo::SkipNode(KStrings::TConstIterator& StringIter)
{
	while(StringIter.IsValid())
	{
		const KString& String = *StringIter++;

		if(String.GetLength() == 1)
		{
			if(String[0] == TEXT('{'))
				SkipNode(StringIter);
			else if(String[0] == TEXT('}'))
				return;
		}
	}

	INITIATE_DEFINED_FAILURE(TEXT("Unexpected end of structured info file."));
}

bool TStructuredInfo::LoadNode(	TNodes::TIterator			Parent,
								LPCTSTR						pName,
								KStrings::TConstIterator&	StringIter,
								size_t						szDepth)
{
	TNodes::TIterator Iter;

	enum
	{
		STATE_START,
		STATE_INSIDE,
		STATE_END

	}State;

	if(szDepth == 0)
	{
		State = STATE_INSIDE;
		
		Iter = Parent;
	}
	else
	{
		State = STATE_START;
	}

	while(StringIter.IsValid() && State != STATE_END)
	{
		KStrings::TConstIterator PrevStringIter = StringIter;

		const KString& String = *StringIter++;

		if(State == STATE_START)
		{
			if(String != TEXT("{"))
			{
				StringIter = PrevStringIter;
				return false;
			}

			Iter = AddNode(Parent, pName);

			State = STATE_INSIDE;
		}
		else // STATE_INSIDE
		{
			// Checking for '}'
			if(String == TEXT("}"))
			{
				if(!szDepth)
					INITIATE_DEFINED_FAILURE(TEXT("Unexpected '}' at root level of structured info file."));

				State = STATE_END;
			}
			else if(String == TEXT("{")) // unnamed node
			{
				SkipNode(StringIter);
			}
			else if(!String.IsEmpty())
			{
				size_t szNameLength;
				const KString Name = DecodeFromSingleString(String, TEXT('='), &szNameLength);

				if(	szNameLength != String.GetLength() || // has '='
					!LoadNode(Iter, Name, StringIter, szDepth + 1)) // name not followed by '{'
				{
					Iter->AddParameter(Name, DecodeFromSingleString(String.Mid(Min(szNameLength + 1, String.GetLength()))));
				}
			}
		}
	}

	if(szDepth > 0 && State != STATE_END)
		INITIATE_DEFINED_FAILURE(TEXT("Unexpected end of structured info file."));

	return true;
}

void TStructuredInfo::LoadRec(	LPCTSTR pFileName,
								KStrings& RStrings,
								KStrings::TIterator InsertAfter,
								const TTokens& ParentTokens,
								size_t szInclusionDepth)
{
	if(szInclusionDepth > g_CommonConsts.m_szMaxStructuredInfoInclusionDepth)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Structured info maximum inclusion depth overrun by including \"") + pFileName + TEXT("\"."));

	KStrings::TIterator FirstIter = InsertAfter;

	{
		TFile File(pFileName, FOF_READ | FOF_TEXT);
	
		while(!File.IsEndOfFile())
			InsertAfter = RStrings.AddAfter(InsertAfter, File.ReadString());
	}

	KStrings::TIterator LastIter = InsertAfter;

	FirstIter = FirstIter.IsValid() ? FirstIter.GetNext() : RStrings.GetFirst();
	LastIter  = LastIter. IsValid() ? LastIter. GetNext() : RStrings.GetFirst();

	// Removing comments and trimming
	for(KStrings::TIterator Iter = FirstIter ; Iter != LastIter ; ++Iter)
		(*Iter = CutSingleStringComments(*Iter)).Trim();

	// Merging strings through trailing '\'
	for(KStrings::TIterator Iter = FirstIter ; Iter != LastIter ; ++Iter)
	{
		for(LPCTSTR s = *Iter ; *s ; s++)
		{
			if(*s == TEXT('\\') && !*++s)
			{
				Iter->SetLeft(Iter->GetLength() - 1);

				if(Iter.GetNext() != LastIter)
					*Iter += *Iter.GetNext(), RStrings.Del(Iter.GetNext());

				break;
			}
		}
	}

	// Preprocessing
	TTokens LocalTokens;

	KStrings::TIterator NIter;

	for(KStrings::TIterator Iter = FirstIter ; Iter != LastIter ; Iter = NIter)
	{
		NIter = Iter.GetNext();

		if((*Iter)[0] == TEXT('#'))
		{
			KString Tag;

			size_t i = 1;

			for( ; (*Iter)[i] && !_istspace((*Iter)[i]) ; i++)
				Tag += (*Iter)[i];

			if(Tag == TEXT("define"))
			{
				KString Name, Value;

				for( ; _istspace((*Iter)[i]) ; i++);

				for( ; (*Iter)[i] && !_istspace((*Iter)[i]) ; i++)
					Name += (*Iter)[i];

				Name.Trim();

				KFC_VERIFY("structured info #define" && !Name.IsEmpty());

				Value = ParentTokens(Iter->Mid(i)).Trim();

				LocalTokens(Name, Value);
			}
			else if(Tag == TEXT("include"))
			{
				KString FileName = DecodeFromSingleString(ParentTokens(Iter->Mid(1 + Tag.GetLength())));

				KFC_VERIFY("structured info #include" && !FileName.IsEmpty());

				FileName = FollowPath(GetFilePath(pFileName), GetFilePath(FileName)) + GetFileName(FileName);

				LoadRec(FileName, RStrings, Iter, LocalTokens, szInclusionDepth + 1);

				LocalTokens.Clear();
			}
			else
			{
				INITIATE_DEFINED_FAILURE((KString)TEXT("Unknown preprocessor tag \"#") + Tag + TEXT("\" inside structured info file."));
			}

			RStrings.Del(Iter);
		}
		else
		{
			*Iter = ParentTokens(*Iter);
		}
	}
}

void TStructuredInfo::Load(LPCTSTR pFileName)
{
	Clear();

	KStrings Strings;

	LoadRec(pFileName, Strings, NULL, TTokens(), 1);

	KStrings::TConstIterator StringIter = Strings.GetFirst();

	DEBUG_EVERIFY(LoadNode(m_Nodes.GetRoot(), TEXT(""), StringIter, 0));
}

void TStructuredInfo::SaveNode(TNodes::TConstIterator Iter, TFile& File) const
{
	KStrings::TConstIterator PNIter;
	KStrings::TConstIterator PVIter;

	TNode::TParameters::TConstIterator PIter;

	// Parameters
	KStrings ParameterNames;
	KStrings ParameterValues;

	size_t szMaxNameLength = 0;

	for(PIter = Iter->m_Parameters.GetFirst() ; PIter.IsValid() ; ++PIter)
	{
		*ParameterNames.	AddLast() = EncodeToSingleString(PIter->m_Name, TEXT('='));
		*ParameterValues.	AddLast() = EncodeToSingleString(PIter->m_Value);

		if(!PIter->m_Value.IsEmpty())
			szMaxNameLength = Max(szMaxNameLength, ParameterNames.GetLast()->GetLength());
	}

	DEBUG_VERIFY(ParameterNames.GetN() == ParameterValues.GetN());

	for(PIter = Iter->m_Parameters.GetFirst(),
			PNIter = ParameterNames.GetFirst(),
			PVIter = ParameterValues.GetFirst() ;
		PIter.IsValid() ;
		++PIter, ++PNIter, ++PVIter)
	{
		File.IndentString(Iter.GetDepth());

		if(PIter->m_Value.IsEmpty())
		{
			File.WriteString(*PNIter);
		}
		else
		{
			File.WriteString(*PNIter, FWSM_ORIGINAL);

			File.IndentString(szMaxNameLength - PNIter->GetLength(), TEXT(' '));

			File.WriteString(TEXT(" = "), FWSM_ORIGINAL);

			File.WriteString(*PVIter);
		}
	}

	// Separator
	if(!Iter->m_Parameters.IsEmpty() && !Iter.IsLeaf())
		File.WriteEOL();

	// Sub-nodes
	for(TNodes::TConstIterator Iter2 = Iter.GetFirstChild() ;
		Iter2.IsValid() ;
		Iter2.ToNextSibling())
	{
		File.IndentString(Iter.GetDepth());
		File.WriteString(EncodeToSingleString(Iter2->m_Name, TCHAR('=')));

		File.IndentString(Iter.GetDepth());
		File.WriteString(TEXT("{"));

		SaveNode(Iter2, File);

		File.IndentString(Iter.GetDepth());
		File.WriteString(TEXT("}"));

		// Separator
		if(Iter2 != Iter.GetLastChild())
			File.WriteEOL();
	}
}

void TStructuredInfo::Save(LPCTSTR pFileName) const
{
	TFile File(pFileName, FOF_WRITE | FOF_CREATE | FOF_NEWFILE | FOF_TEXT);

	SaveNode(m_Nodes.GetRoot(), File);
}

TStructuredInfo::TNodes::TIterator TStructuredInfo::GetRootNode()
{
	return m_Nodes.GetRoot();
}

TStructuredInfo::TNodes::TConstIterator TStructuredInfo::GetRootNode() const
{
	return m_Nodes.GetRoot();
}

TStructuredInfo::TNodes::TConstIterator
	TStructuredInfo::FindNode(	TNodes::TConstIterator	Parent,
								const KString&			Name,
								TNodes::TConstIterator	After)
{
	DEBUG_VERIFY(Parent.IsValid());

	DEBUG_VERIFY(!After.IsValid() || After.GetParent() == Parent);

	for(TNodes::TConstIterator Iter =	After.IsValid() ?
											After.GetNextSibling() :
											Parent.GetFirstChild() ;
		Iter.IsValid() ;
		Iter.ToNextSibling())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return TNodes::TConstIterator();
}

TStructuredInfo::TNodes::TConstIterator
	TStructuredInfo::FindNodeSafe(	TNodes::TConstIterator	Parent,
									const KString&			Name,
									TNodes::TConstIterator	After)
{
	return Parent.IsValid() ? FindNode(Parent, Name, After) : TNodes::TConstIterator();
}

TStructuredInfo::TNodes::TConstIterator
	TStructuredInfo::GetNode(	TNodes::TConstIterator	Parent,
								const KString&			Name,
								TNodes::TConstIterator	After)
{
	TNodes::TConstIterator Iter = FindNode(Parent, Name, After);

	if(!Iter.IsValid())
		INITIATE_DEFINED_FAILURE((KString)TEXT("Info node \"") + Name + TEXT("\" not found."));

	return Iter;
}

const KString& TStructuredInfo::GetSubNodeParameterValue(	TStructuredInfo::TNodes::TConstIterator	Parent,
															const KString&							SubNodeName,
															const KString&							ParameterName,															
															TStructuredInfo::TNodes::TConstIterator	SubNodeAfter)
{
	return GetNode(Parent, SubNodeName, SubNodeAfter)->GetParameterValue(ParameterName);
}

KString TStructuredInfo::GetSubNodeParameterValue(	TStructuredInfo::TNodes::TConstIterator	Parent,
													const KString&							SubNodeName,
													const KString&							ParameterName,
													const KString&							DefaultValue,													
													TStructuredInfo::TNodes::TConstIterator	SubNodeAfter)
{
	const TStructuredInfo::TNodes::TConstIterator NIter =
		FindNode(Parent, SubNodeName, SubNodeAfter);
	
	return NIter.IsValid() ? NIter->GetParameterValue(ParameterName, DefaultValue) : DefaultValue;
}

TStructuredInfo::TNodes::TIterator
	TStructuredInfo::AddNode(	TStructuredInfo::TNodes::TIterator	Parent,
								const KString&						Name)
{
	DEBUG_VERIFY(Parent.IsValid());

	const TNodes::TIterator NIter = TNodes::AddLastChild(Parent);

	NIter->Set(Name);

	return NIter;
}

void TStructuredInfo::DelNode(TNodes::TIterator Node)
{
	TNodes::Del(Node);
}

TStructuredInfo::TNode::TParameters::TIterator
	TStructuredInfo::AddSubNodeParameter(	TStructuredInfo::TNodes::TIterator	Parent,
											const KString&						SubNodeName,
											const KString&						ParameterName,
											const KString&						ParameterValue)
{
	return AddNode(Parent, SubNodeName)->AddParameter(ParameterName, ParameterValue);
}

TStructuredInfo::TNodes::TIterator TStructuredInfo::GetPrevSame(TNodes::TIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToPrevSibling() ; Iter.IsValid() ; Iter.ToPrevSibling())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNodes::TIterator TStructuredInfo::GetNextSame(TNodes::TIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToNextSibling() ; Iter.IsValid() ; Iter.ToNextSibling())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNodes::TConstIterator TStructuredInfo::GetPrevSame(TNodes::TConstIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToPrevSibling() ; Iter.IsValid() ; Iter.ToPrevSibling())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

TStructuredInfo::TNodes::TConstIterator TStructuredInfo::GetNextSame(TNodes::TConstIterator Iter)
{
	DEBUG_VERIFY(Iter.IsValid());

	const KString& Name = Iter->m_Name;

	for(Iter.ToNextSibling() ; Iter.IsValid() ; Iter.ToNextSibling())
	{
		if(Iter->m_Name == Name)
			return Iter;
	}

	return NULL;
}

// ----------------
// Global routines
// ----------------
KString CutSingleStringComments(const KString& SingleString)
{
	size_t i;

	if(SingleString.GetLength() < 2)
		return SingleString;

	bool bQuoted = false;

	for(i = 0 ; i < SingleString.GetLength() - 1 ; i++)
	{
		if(SingleString[i] == TEXT('\\'))
		{
			i++;
		}
		else if(SingleString[i] == TEXT('"'))
		{
			bQuoted = !bQuoted;
		}
		else
		{
			// checking for unquoted comment prefix
			if(!bQuoted && SingleString[i] == TEXT('/') && SingleString[i+1] == TEXT('/'))
				return SingleString.Left(i);
		}
	}

	return SingleString; // no comment detected
}

KString EncodeToSingleString(	const KString&	String,
								TCHAR			cTerminator)
{
	DEBUG_VERIFY(	cTerminator != TEXT(' ')	&&
					cTerminator != TEXT('\\')	&&
					cTerminator != TEXT('"'));

	size_t i;

	const bool bNeedQuotes =
		String.IsEmpty()							||	// empty
		_istspace(String[0])						||	// has leading  space(s)
		_istspace(String[String.GetLength() - 1])	||	// has trailing space(s)
		cTerminator && _tcschr(String, cTerminator)	||	// has terminator character
		String.Find(TEXT("//")) != UINT_MAX			||	// has single line comment start
		String.Find(TEXT("/*")) != UINT_MAX			||	// has long comment start
		String.Find(TEXT("*/")) != UINT_MAX;			// has long commend end

	KString SingleString;
	
	if(bNeedQuotes)
		SingleString += TEXT('"');

	for(i = 0 ; i < String.GetLength() ; i++)
	{
		if(!i && String[i] == TEXT('#'))
			SingleString += TEXT("\\#");
		else if(String[i] == TEXT('\t'))
			SingleString += TEXT("\\t");
		else if(String[i] == TEXT('\r'))
			SingleString += TEXT("\\r");
		else if(String[i] == TEXT('\n'))
			SingleString += TEXT("\\n");
		else if(String[i] == TEXT('\\'))
			SingleString += TEXT("\\\\");
		else if(String[i] == TEXT('"'))
			SingleString += TEXT("\\\"");
		else
			SingleString += String[i];
	}

	if(bNeedQuotes)
		SingleString += TEXT('"');

	return SingleString;
}

KString DecodeFromSingleString(	const KString&	SingleString,
								TCHAR			cTerminator,
								size_t*			pRLengthDecoded)
{
	DEBUG_VERIFY(	cTerminator != TEXT(' ')	&&
					cTerminator != TEXT('\\')	&&
					cTerminator != TEXT('"'));

	size_t i;

	KString String;

	bool bQuoted = false;

	size_t szTrailingSpacesStart = 0;
	
	for(i = 0 ; i < SingleString.GetLength() ; i++)
	{
		// Checking for unquoted terminator character
		if(!bQuoted && SingleString[i] == cTerminator)
			break;

		if(SingleString[i] == TEXT('"')) // quote state change
		{
			bQuoted = !bQuoted;
		}
		else
		{
			if(SingleString[i] == TEXT('\\')) // escape sequence
			{
				if(++i == SingleString.GetLength())
					INITIATE_DEFINED_FAILURE((KString)TEXT("Unfinished escape sequence inside encoded single string:\r\n\"") + SingleString + TEXT("\"."));

				szTrailingSpacesStart = String.GetLength() + 1;

				if(SingleString[i] == TEXT('t'))
					String += TEXT('\t');
				else if(SingleString[i] == TEXT('r'))
					String += TEXT('\r');
				else if(SingleString[i] == TEXT('n'))
					String += TEXT('\n');
				else
					String += SingleString[i];				
			}
			else // plain character
			{
				if(bQuoted || !_istspace(SingleString[i])) // non-trimmable character
				{
					szTrailingSpacesStart = String.GetLength() + 1;
				}
				else // trimmable space
				{
					if(szTrailingSpacesStart == 0) // leading space
						continue;
				}

				String += SingleString[i];
			}
		}
	}

	if(bQuoted)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Encoded single string has missing closing \":\r\n\"") + SingleString + TEXT("\"."));

	// Cutting trailing spaces
	String.SetLeft(szTrailingSpacesStart);

	if(pRLengthDecoded)
		*pRLengthDecoded = i;

	return String;
}

int& ReadInt(	const KString&	String,
				int&			iRValue,
				LPCTSTR			pValueName)
{
	if(!FromString(String, iRValue))
		REPORT_INCORRECT_VALUE_FORMAT(String, pValueName);

	return iRValue;
}

int& ReadNormalizedInt(	const KString&	String,
						int&			iRValue,
						LPCTSTR			pValueName,
						const ISEGMENT&	Limits)
{
	ReadInt(String, iRValue, pValueName);

	if(!HitsSegmentBounds(iRValue, Limits))
		REPORT_INVALID_VALUE(String, pValueName);

	return iRValue;
}

UINT& ReadUINT(	const KString&	String,
				UINT&			uiRValue,
				LPCTSTR			pValueName)
{
	if(!FromString(String, uiRValue))
		REPORT_INCORRECT_VALUE_FORMAT(String, pValueName);

	return uiRValue;
}

UINT& ReadNormalizedUINT(   const KString&		String,
						    UINT&				uiRValue,
						    LPCTSTR				pValueName,
						    const UISEGMENT&	Limits)
{
	ReadUINT(String, uiRValue, pValueName);

	if(!HitsSegmentBounds(uiRValue, Limits))
		REPORT_INVALID_VALUE(String, pValueName);

	return uiRValue;
}

float& ReadFloat(	const KString&	String,
					float&			fRValue,
					LPCTSTR			pValueName)
{
	if(!FromString(String, fRValue))
		REPORT_INVALID_VALUE(String, pValueName);

	return fRValue;
}

float& ReadNormalizedFloat(	const KString&	String,
							float&			fRValue,
							LPCTSTR			pValueName,
							const FSEGMENT&	Limits)
{
	ReadFloat(String, fRValue, pValueName);

	if(!HitsSegmentBounds(fRValue, Limits))
		REPORT_INVALID_VALUE(String, pValueName);

	return fRValue;
}

bool& ReadBool(	const KString&	String,
				bool&			bRValue,
				LPCTSTR			pValueName)
{
	if(!FromString(String, bRValue))
		REPORT_INCORRECT_VALUE_FORMAT(String, pValueName);

	return bRValue;
}

KString& ReadText(	TInfoNodeConstIterator	InfoNode,
					LPCTSTR					pParameterName,
					KString&				RText)
{
	TInfoParameterConstIterator PIter;

	bool bFirstRow = true;

	PIter.Invalidate();
	while((PIter = InfoNode->FindParameter(pParameterName, PIter)).IsValid())
	{
		if(bFirstRow)
			bFirstRow = false;
		else
			RText += TEXT('\n');

		RText += PIter->m_Value;
	}

	return RText;
}
