#include "kfc_cgi_pch.h"
#include "cgi_tokenizer.h"

#include "cgi_device_globals.h"

// ----------------
// Global routines
// ----------------
static void PrepareHTML_SubstTokens(TTokens& Tokens)
{
	for(TTokens::TWalker Walker = Tokens.GetWalker() ; Walker ; ++Walker)
	{
		KString  SrcString =  Walker.GetPath();
		KString& DstString = *Walker;

		if(	SrcString.GetFirstChar	() == TEXT('[') &&
			SrcString.GetLastChar	() == TEXT(']'))
		{
			if(	SrcString.GetPostFirstChar	() == TEXT('[') &&
				SrcString.GetPreLastChar	() == TEXT(']'))
			{
				DstString = EncodeForHTMLWithoutSpaces(DstString);
			}
			else if(SrcString.GetPostFirstChar	() == TEXT('{') &&
					SrcString.GetPreLastChar	() == TEXT('}'))
			{
				DstString = EncodeForHTMLWithEOL_AndWithoutSpaces(DstString);
			}
		}
		else if(SrcString.GetFirstChar() == TEXT('{') &&
				SrcString.GetLastChar () == TEXT('}'))
		{
			if(	SrcString.GetPostFirstChar	() == TEXT('{') &&
				SrcString.GetPreLastChar	() == TEXT('}'))
			{
				DstString = EncodeForHTMLWithEOL(DstString);
			}
			else
			{
				DstString = EncodeForHTML(DstString);
			}
		}
	}
}

KString ProcessCGI_TokensPreloaded(const KString& Text, const TTokens* pTokens, LPCTSTR pBasePath)
{
	KString TempText;

	TTokens SubstTokens;

	if(pTokens)
		PrepareHTML_SubstTokens(SubstTokens = *pTokens);

	KString Result;

	size_t i, j;

	LPCTSTR s = Text;

	for(i = 0 ; ; )
	{
		j = Text.Find(TEXT("[FILE:"), i);

		if(j == UINT_MAX)
		{
			Result += SubstTokens.Process(Text.Mid(i));
			break;
		}

		Result += SubstTokens.Process(Text.Mid(i, j - i));

		j += 6;

		KString	FileName;
		TTokens	SubTokens;
		size_t	cd = 0;
		bool	bFileName	= true;
		bool	bSrcString	= false;

		KString SrcString;
		KString DstString;

		for( ; ; j++)
		{
			TCHAR c = s[j];

			if(!c)
				INITIATE_DEFINED_FAILURE(TEXT("Unexpected end of string in CGI token source."));

			if(c == TEXT('\\'))
			{
				if(!(c = s[++j]))
					INITIATE_DEFINED_FAILURE(TEXT("Unterminated escape sequence within CGI token source."));

				switch(c)
				{
				case TEXT('t'):
					c = TEXT('\t');
					break;

				case TEXT('r'):
					c = TEXT('\r');
					break;

				case TEXT('n'):
					c = TEXT('\n');
					break;
				}
			}

			if(c == TEXT('['))
			{
				cd++;
			}
			else if(c == TEXT(']'))
			{
				if(!cd--)
				{
					if(!bFileName && !SrcString.IsEmpty())
						SubTokens.Add(SrcString, pTokens ? ProcessCGI_TokensPreloaded(DstString, pTokens) : DstString);

					break;
				}
			}
			else if(!cd)
			{
				if(c == TEXT(':'))
				{
					if(bFileName)
					{
						bFileName = false;

						bSrcString = true;
					}
					else
					{
						if(!SrcString.IsEmpty())
							SubTokens.Add(SrcString, pTokens ? ProcessCGI_TokensPreloaded(DstString, pTokens) : DstString);

						SrcString.Empty(), DstString.Empty();

						bSrcString = true;
					}
					
					if(!c)
						break;

					continue;
				}
				else if(!bFileName && c == TEXT('='))
				{
					if(bSrcString)
					{
						bSrcString = false;
						continue;
					}
				}
			}

			(bFileName ? FileName : bSrcString ? SrcString : DstString) += c;
		}

		if(Text[j] != TEXT(']'))
			INITIATE_DEFINED_FAILURE((KString)TEXT("Unterminated \"[FILE:\" within CGI template."));

		Result += ProcessCGI_Tokens(IsAbsolutePath(FileName) ? FileName : (KString)pBasePath + FileName, &SubTokens);

		i = j + 1;
	}

	return Result;
}
