#ifndef d3d_state_block_h
#define d3d_state_block_h

#include "graphics_device_globals.h"

// ----------------
// D3D state block
// ----------------
class TD3DStateBlock
{
private:
	IDirect3DStateBlock9* m_pStateBlock;

public:
	TD3DStateBlock();

	~TD3DStateBlock()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pStateBlock; }

	void Release();

	TD3DStateBlock& Create(D3DSTATEBLOCKTYPE Type = D3DSBT_ALL);

	TD3DStateBlock& Allocate(IDirect3DStateBlock9* pSStateBlock);

	void Capture();

	void Apply() const
		{ DEBUG_VERIFY_ALLOCATION; DEBUG_EVALUATE_VERIFY(!m_pStateBlock->Apply()); }

	IDirect3DStateBlock9* GetStateBlock() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pStateBlock; }

	operator IDirect3DStateBlock9* () const
		{ return GetStateBlock(); }
};

// -------------------------
// D3D state block recorder
// -------------------------
class TD3DStateBlockRecorder
{
private:
	TD3DStateBlock& m_RStateBlock;

public:
	TD3DStateBlockRecorder(TD3DStateBlock& SRStateBlock);

	~TD3DStateBlockRecorder();
};

// ---------------------
// D3D state block node
// ---------------------
class TD3DStateBlockNode
{
private:
	const TD3DStateBlockNode* m_pParent;

	size_t m_szDepth; // required for run-pointer alignment before chains comparison

public:
	TD3DStateBlock m_StateBlock;


	TD3DStateBlockNode()
		{ m_szDepth = UINT_MAX; }

	~TD3DStateBlockNode()
		{ Release(); }
	
	bool IsAllocated() const
		{ return m_szDepth != UINT_MAX && m_StateBlock.IsAllocated(); }

	void Release();

	void Allocate(const TD3DStateBlockNode* pSParent);

	const TD3DStateBlockNode* GetParent() const
		{ return m_pParent; }

	size_t GetDepth() const
		{ return m_szDepth; }

	// Never attempts to shrink history arrays allocation
	static const TD3DStateBlockNode*	GetComonAncestor
		(	const TD3DStateBlockNode*	p1,
			const TD3DStateBlockNode*	p2,
			const TD3DStateBlockNode**	ppRP2History,
			size_t&						szRP2HistorySize)
	{
		szRP2HistorySize = 0;

		if(p2 == NULL)
			return NULL;

		if(p1 == NULL)
		{
			while(p2)
			{
				ppRP2History[szRP2HistorySize++] = p2;
				p2 = p2->m_pParent;
			}

			return NULL;
		}

		// Performing depth alignment
		{
			int dif = p1->m_szDepth - p2->m_szDepth;

			for( ; dif > 0 ; --dif)
				p1 = p1->m_pParent;

			for( ; dif < 0 ; ++dif)
			{
				ppRP2History[szRP2HistorySize++] = p2;
				p2 = p2->m_pParent;
			}
		}

		// Serching for common ancestor
		while(p1 != p2)
		{
			p1 = p1->m_pParent;

			ppRP2History[szRP2HistorySize++] = p2;
			p2 = p2->m_pParent;
		}

		return p1;
	}	
};

#endif // d3d_state_block_h