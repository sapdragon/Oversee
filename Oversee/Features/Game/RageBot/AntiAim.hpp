#pragma once
#include "../../../SDK/SDK.hpp"

class C_AntiAim
{
public:
	virtual void FinishCreateMove( )
	{
		if ( *g_Globals->m_Packet.m_bSendPacket )
			g_Globals->m_Packet.m_angFakeAngles = g_Globals->m_Packet.m_pCmd->m_angViewAngles;

		Math::Normalize3( g_Globals->m_Packet.m_angFakeAngles );
		Math::ClampAngles( g_Globals->m_Packet.m_angFakeAngles );
	}
	virtual void OnCreateMove( );
	virtual void AvoidOverlap( );
	virtual void LegMovement( );
	virtual void JitterMove( );
	virtual void MicroMovement( );
	virtual void ResetData( )
	{
		*this = C_AntiAim( );
	}
	virtual float GetAtTargetsYaw( );
	virtual float GetPitch( );
	virtual float GetYaw( );
	virtual void SlowWalk( );
	virtual bool CheckConditions( );
private:
	int m_nDesyncSide = -1;
	int m_nAlternativeDesyncSide = -1;
	int m_nShotCmd = 0;

	bool m_bWorkedTickBefore = false;
	bool m_bAlternativeSide = false;
	bool m_bOverlap = false;
	bool m_bInverterState = false;
	bool m_bJitterState = false;
	bool m_bAccelerated = false;

	struct
	{
		float m_flYawDelta = 0.0f;
	} m_AnimData;
};

inline C_AntiAim* g_AntiAim = new C_AntiAim( );