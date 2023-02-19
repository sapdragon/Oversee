#pragma once
#include "../../../SDK/SDK.hpp"

struct C_ShotChamsData
{
    int m_iEntIndex;
    ModelRenderInfo_t m_RenderInfo;
    DrawModelState_t m_DrawState;
    
    float m_flAnimation = 0.0f;
    float m_flCreationTime = 0.0f;

    std::array < matrix3x4_t, MAXSTUDIOBONES > m_aMatrix;
    matrix3x4_t m_aWorldMatrix;
};

class C_ShotChams
{
public:
    virtual void Instance( );
    virtual void OnRageBotFire( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, Vector vecMatrixOrigin );
    virtual void ClearData( ) { m_aShotChams.clear( ); };
private:
    std::vector < C_ShotChamsData > m_aShotChams;
};

inline C_ShotChams* g_ShotChams = new C_ShotChams( );