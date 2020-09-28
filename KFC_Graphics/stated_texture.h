#ifndef stated_texture_h
#define stated_texture_h

#include "texture.h"
#include "d3d_state_block.h"

// ---------------
// Stated texture
// ---------------
class TStatedTexture
{
private:
    size_t m_szStage;

public:
    TTexture m_Texture;

    TObjectPointer<TD3DStateBlockNode> m_StateBlockNode;


    TStatedTexture();

    ~TStatedTexture()
        { Release(); }

    bool IsAllocated() const
        { return m_szStage != UINT_MAX; }

    void Release();

    void Allocate(size_t szSStage);

    void Install() const;

    size_t GetStage() const
        { return m_szStage; }
};

#endif // stated_texture_h
