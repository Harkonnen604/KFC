#ifndef structured_info_h
#define structured_info_h

#include "file.h"

#define MAX_SET_LOADING_DIMENSIONS	(4)

// ----------------
// Structured info
// ----------------
class TStructuredInfo
{
public:
	// Node
	struct TNode
	{
		// Parameter
		struct TParameter
		{
		public:
			KString m_Name;
			KString m_Value;
			
		public:
			TParameter& Set(const KString& SName	= TEXT(""),
							const KString& SValue	= TEXT(""))
			{
				m_Name	= SName;
				m_Value	= SValue;

				return *this;
			}
		};

		// Parameters
		typedef TList<TParameter> TParameters;


		KString		m_Name;
		TParameters	m_Parameters;


		TNode& Set(const KString& SName = TEXT(""))
		{
			m_Name = SName;

			return *this;
		}

		TParameters::TConstIterator FindParameter(	const KString&				Name,
													TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		TParameters::TConstIterator GetParameter(	const KString&				Name,
													TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		const KString& GetParameterValue(	const KString&				Name,
											TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		const KString GetParameterValue(const KString&				Name,
										const KString&				DefaultValue,
										TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		bool HasParameter(	const KString&				Name,
							TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		bool HasTrueParameter(	const KString&				Name,
								TParameters::TConstIterator	After = TParameters::TConstIterator()) const;

		TParameters::TIterator AddParameter(const KString& Name		= TEXT(""),
											const KString& Value	= TEXT(""));

		static TParameters::TIterator GetPrevSame(TParameters::TIterator Iter);
		static TParameters::TIterator GetNextSame(TParameters::TIterator Iter);

		static TParameters::TConstIterator GetPrevSame(TParameters::TConstIterator Iter);
		static TParameters::TConstIterator GetNextSame(TParameters::TConstIterator Iter);

		static TParameters::TIterator& ToPrevSame(TParameters::TIterator& Iter)
			{ return Iter = GetPrevSame(Iter); }

		static TParameters::TIterator& ToNextSame(TParameters::TIterator& Iter)
			{ return Iter = GetNextSame(Iter); }

		static TParameters::TConstIterator& ToPrevSame(TParameters::TConstIterator& Iter)
			{ return Iter = GetPrevSame(Iter); }

		static TParameters::TConstIterator& ToNextSame(TParameters::TConstIterator& Iter)
			{ return Iter = GetNextSame(Iter); }
	};

	// Nodes
	typedef TTree<TNode> TNodes;

private:
	TNodes m_Nodes;

private:
	static void SkipNode(KStrings::TConstIterator& StringIter);

	static bool LoadNode(	TNodes::TIterator			Parent,
							LPCTSTR						pName,
							KStrings::TConstIterator&	StringIter,
							size_t						szDepth);

	static void LoadRec(LPCTSTR pFileName,
						KStrings& RStrings,
						KStrings::TIterator InsertAfter,
						const TTokens& ParentTokens,
						size_t szInclusionDepth);

	void SaveNode(TNodes::TConstIterator Iter, TFile& File) const;	

public:
	TStructuredInfo()
		{ Clear(); }

	TStructuredInfo(LPCTSTR pFileName)
		{ Load(pFileName); }

	void Clear();

	void Load(LPCTSTR pFileName);
	void Save(LPCTSTR pFileName) const;

	TNodes::TIterator		GetRootNode();
	TNodes::TConstIterator	GetRootNode() const;


	static TNodes::TConstIterator FindNode(	TNodes::TConstIterator	Parent,
											const KString&			Name,
											TNodes::TConstIterator	After = TNodes::TConstIterator());

	static TNodes::TConstIterator FindNodeSafe(	TNodes::TConstIterator	Parent,
												const KString&			Name,
												TNodes::TConstIterator	After = TNodes::TConstIterator());


	static TNodes::TConstIterator GetNode(	TNodes::TConstIterator	Parent,
											const KString&			Name,
											TNodes::TConstIterator	After = TNodes::TConstIterator());

	static const KString& GetSubNodeParameterValue(	TNodes::TConstIterator	Parent,
													const KString&			SubNodeName,
													const KString&			ParameterName,
													TNodes::TConstIterator	SubNodeAfter = TNodes::TConstIterator());

	static KString GetSubNodeParameterValue(TNodes::TConstIterator	Parent,
											const KString&			SubNodeName,
											const KString&			ParameterName,
											const KString&			DefaultValue,
											TNodes::TConstIterator	SubNodeAfter = TNodes::TConstIterator());

	static TNodes::TIterator AddNode(	TNodes::TIterator	Parent,
										const KString&		Name = TEXT(""));

	static void DelNode(TNodes::TIterator Node);

	static TNode::TParameters::TIterator AddSubNodeParameter(	TNodes::TIterator	Parent,
																const KString&		SubNodeName		= TEXT(""),
																const KString&		ParameterName	= TEXT(""),
																const KString&		ParameterValue	= TEXT(""));

	static TNodes::TIterator GetPrevSame(TNodes::TIterator Iter);
	static TNodes::TIterator GetNextSame(TNodes::TIterator Iter);

	static TNodes::TConstIterator GetPrevSame(TNodes::TConstIterator Iter);
	static TNodes::TConstIterator GetNextSame(TNodes::TConstIterator Iter);

	static TNodes::TIterator& ToPrevSame(TNodes::TIterator& Iter)
		{ return Iter = GetPrevSame(Iter); }

	static TNodes::TIterator& ToNextSame(TNodes::TIterator& Iter)
		{ return Iter = GetNextSame(Iter); }

	static TNodes::TConstIterator& ToPrevSame(TNodes::TConstIterator& Iter)
		{ return Iter = GetPrevSame(Iter); }

	static TNodes::TConstIterator& ToNextSame(TNodes::TConstIterator& Iter)
		{ return Iter = GetNextSame(Iter); }
};

// ----------------
// Global typedefs
// ----------------
typedef TStructuredInfo::TNode		TInfoNode;
typedef TStructuredInfo::TNodes		TInfoNodes;

typedef TInfoNode::TParameter	TInfoParameter;
typedef TInfoNode::TParameters	TInfoParameters;

typedef TInfoNodes::TIterator		TInfoNodeIterator;
typedef TInfoNodes::TConstIterator	TInfoNodeConstIterator;

typedef TInfoParameters::TIterator		TInfoParameterIterator;
typedef TInfoParameters::TConstIterator	TInfoParameterConstIterator;

// ----------------
// Global routines
// ----------------
KString CutSingleStringComments(const KString& SingleString);

KString EncodeToSingleString(	const KString&	String,
								TCHAR			cTerminator = 0);

KString DecodeFromSingleString(	const KString&	SingleString,
								TCHAR			cTerminator		= 0,
								size_t*			pRLengthDecoded	= NULL);

int& ReadInt(	const KString&	String,
				int&			iRValue,
				LPCTSTR			pValueName);

int& ReadNormalizedInt(	const KString&	String,
						int&			iRValue,
						LPCTSTR			pValueName,
						const ISEGMENT&	Limits);

UINT& ReadUINT(	const KString&	String,
				UINT&			uiRValue,
				LPCTSTR			pValueName);

UINT& ReadNormalizedUINT(	const KString&		String,
							UINT&				uiRValue,
							LPCTSTR				pValueName,
							const SZSEGMENT&	Limits);

float& ReadFloat(	const KString&	String,
					float&			fRValue,
					LPCTSTR			pValueName);

float& ReadNormalizedFloat(	const KString&	String,
							float&			fRValue,
							LPCTSTR			pValueName,
							const FSEGMENT&	Limits = FSEGMENT(0.0f, 1.0f));

bool& ReadBool(	const KString&	String,
				bool&			bRValue,
				LPCTSTR			pValueName);

template <class t>
t& ReadBiTypeValue(	const KString&	String,
					t&				RValue,
					LPCTSTR			pValueName)
{
	if(!FromString(String, RValue))
		REPORT_INCORRECT_VALUE_FORMAT(String, pValueName);

	if(!RValue.IsValid())
		REPORT_INVALID_VALUE(String, pValueName);

	return RValue;
}

KString& ReadText(	TInfoNodeConstIterator	InfoNode,
					LPCTSTR					pParameterName,
					KString&				RText);

#endif // structured_info_h
