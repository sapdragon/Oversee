#pragma once
#include "../../../SDK/SDK.hpp"

struct WepData_t
{
	float m_flMaxRange = 0.0f;
	float m_flRangeModifier = 0.0f;
	float m_flWeaponDamage = 0.0f;
	float m_flPenetrationPower = 0.0f;
	float m_flPenetrationDistance = 0.0f;
	float m_flArmorRatio = 0.0f;
};
struct PenetrationData_t
{
	Semaphore* Semaphore;
	SharedMutex* m_Mutex;

	Vector m_vecShootPosition = Vector( 0, 0, 0 );
	Vector m_vecTargetPosition = Vector( 0, 0, 0 );
	Vector m_vecDirection = Vector( 0, 0, 0 );

	bool m_bSuccess = false;
	bool m_bFinished = false;

	int m_PenetrationCount = 4;
	int m_Hitbox = 0;
	int m_nPriority = 0;

	float m_flMinDamage = 0.0f;
	float m_flDamageModifier = 0.5f;
	float m_flPenetrationModifier = 1.0f;
	float m_flCurrentDamage = 0.0f;
	float m_flCurrentDistance = 0.0f;

	CGameTrace m_EnterTrace;
	CGameTrace m_ExitTrace;
};

class C_AutoWall
{
public:
	virtual bool TraceToExit( PenetrationData_t* m_PenetrationData, const Vector& vecStart, CGameTrace* pEnterTrace, CGameTrace* pExitTrace );
	virtual bool HandleBulletPenetration( PenetrationData_t* m_PenetrationData );
	virtual bool IsArmored( C_BasePlayer* pPlayer, const int& nHitGroup );
	virtual void ScaleDamage( CGameTrace Trace, float& flDamage );
	virtual void ClipTraceToPlayers( const Vector& vecStart, const Vector& vecEnd, CGameTrace* Trace, CTraceFilter* Filter, uint32_t nMask );
	virtual Vector GetPointDirection( const Vector& vecShootPosition, const Vector& vecTargetPosition );
	static void ScanPoint( PenetrationData_t* m_PenetrationData );
	virtual bool SimulateFireBullet( PenetrationData_t* m_PenetrationData );
	virtual bool IsPenetrablePoint( const Vector& vecShootPosition, const Vector& vecTargetPosition );
	virtual void CacheWeaponData( );
	virtual WepData_t& GetWeaponData( )
	{
		return m_Data;
	}
private:
	WepData_t m_Data;
};

inline C_AutoWall* g_AutoWall = new C_AutoWall( );