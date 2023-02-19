#pragma once

class C_MoveData
{
public:
    bool        m_bFirstRunOfFunctions : 1;
    bool        m_bGameCodeMovedPlayer : 1;
    int         m_nPlayerHandle;        
    int         m_nImpulseCommand;      
    Vector      m_vecViewAngles;        
    Vector      m_vecAbsViewAngles;    
    int         m_nButtons;            
    int         m_nOldButtons;         
    float       m_flForwardMove;
    float       m_flSideMove;
    float       m_flUpMove;
    float       m_flMaxSpeed;
    float       m_flClientMaxSpeed;
    Vector      m_vecVelocity;          
    Vector      m_vecAngles;         
    Vector      m_vecOldAngles;
    float       m_flOutStepHeight;       
    Vector      m_vecOutWishVel;          
    Vector      m_vecOutJumpVel;       
    Vector      m_vecConstraintCenter;
    float       m_flConstraintRadius;
    float       m_flConstraintWidth;
    float       m_flConstraintSpeedFactor;
    float       m_aUnknown[ 5 ];
    Vector      m_vecAbsOrigin;
};