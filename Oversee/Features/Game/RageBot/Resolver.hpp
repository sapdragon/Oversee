#pragma once
#include "../../../SDK/SDK.hpp"
#include "Shots.hpp"

struct ResolverData_t
{
	int m_nMissedShots = 0;
	int m_nResolverSide = 0;
	float m_flLatestResolvedTime = 0.0f;
};
class C_Resolver
{
public:
	virtual void HandleMiss( C_BasePlayer* Player, LagRecord_t* m_Record );
	virtual void CorrectAnimations( C_BasePlayer* Player, LagRecord_t* m_Record, LagRecord_t* m_PrevRecord );
	virtual float BuildMoveYaw( C_BasePlayer* Player, float flFootYaw );
	//virtual void DebugDrawData( C_BasePlayer* Player );
private:
	std::unordered_map < int, ResolverData_t > m_ResData;
};

inline C_Resolver* g_Resolver = new C_Resolver( );