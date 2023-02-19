#pragma once
#include <list>
#include "../../../SDK/SDK.hpp"

class C_AutoStop
{
public:
	virtual void Initialize( bool bIsForced = false );
	virtual void RunAutoStop( );
	virtual void RunEarlyAutoStop( bool bForced = false );
	virtual void Friction( );
	virtual void RunMovementPrediction( );
	virtual void ProcessMovement( );
	virtual void ResetData( )
	{
		m_Movement.m_nTicksToStop = 0;
		m_Movement.m_bIsPeeking = false;
		m_Movement.m_bCanPredictMovement = false;
		memset( m_Scan.m_BadPlayers.data( ), 0, sizeof( m_Scan.m_BadPlayers ) );
	}
	virtual int GetTicksToStop( ) { return m_Movement.m_nTicksToStop; };
	virtual void CleanUp( )
	{
		memset( m_Scan.m_BadPlayers.data( ), 0, sizeof( m_Scan.m_BadPlayers ) );
	}
	virtual float ComputeConstraintSpeedFactor( );
	virtual float CalcRoll( const QAngle& angAngles, const Vector& vecVelocity, float flRollAngle, float flRollSpeed );
	virtual bool IsPeeking( ) { return m_Movement.m_bIsPeeking; };
	virtual void SetAsPeeking( ) { m_Movement.m_bIsPeeking = true; };
private:
	int m_LastPlayer = 0;
	struct
	{
		C_MoveData* MoveData = new C_MoveData( );
		Vector m_vecForward, m_vecRight, m_vecUp;
	} m_PredData;	

	struct
	{
		int m_nTicksToStop = 0;
		bool m_bIsPeeking = false;
		bool m_bCanPredictMovement = false;
	} m_Movement;

	struct
	{
		std::array < bool, 64 > m_BadPlayers = { };
	} m_Scan;
};

inline C_AutoStop* g_AutoStop = new C_AutoStop( );