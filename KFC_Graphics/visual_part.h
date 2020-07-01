#ifndef visual_part_h
#define visual_part_h

#include "visual_model.h"
#include "d3d_material.h"
#include "stated_texture.h"

// ------------
// Visual part
// ------------
class TVisualPart
{
public:
	bool m_bAllocated;

	TObjectPointer<TVisualModelBase> m_Model;

	TObjectPointer<TD3DMaterial> m_Material;

	TArray< TObjectPointer<TStatedTexture> > m_Textures;


	TVisualPart();

	~TVisualPart()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate();

	void Render() const;
};

#endif // visual_part_h