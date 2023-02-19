#pragma once
#include <vector>
#include "LagCompensation.hpp"
#include "../../../SDK/SDK.hpp"

enum HitGroup : int
{
	HG_HEAD = 0,
	HG_CHEST,
	HG_ARMS,
	HG_STOMACH,
	HG_PELVIS,
	HG_LEGS
};
struct TempTarget_t
{
public:
	// temp target class constructor used to prevent from manually filling a variable in a few lines
	TempTarget_t( ) = default;
	TempTarget_t( C_BasePlayer* pPlayer, LagRecord_t LocalRecord, LagRecord_t TrackRecord, int nLocalPoints, int nHistoryPoints )
	{
		m_pPlayer = pPlayer;
		m_LocalRecord = LocalRecord;
		m_TrackRecord = TrackRecord;
		m_nLocalVulnerablePoints = nLocalPoints;
		m_nTrackVulnerablePoints = nHistoryPoints;
	}

	// vulnerablitity
	int m_nLocalVulnerablePoints = 0;
	int m_nTrackVulnerablePoints = 0;

	// lagrecord
	LagRecord_t m_LocalRecord = LagRecord_t( );

	// history record
	LagRecord_t m_TrackRecord = LagRecord_t( );

	// player
	C_BasePlayer* m_pPlayer = nullptr;
};
struct HitboxData_t
{
	HitboxData_t( ) = default;

	mstudiobbox_t* m_pHitbox = nullptr;
	Vector m_vecMins = Vector( 0, 0, 0 ), m_vecMaxs = Vector( 0, 0, 0 ), m_vecCenter = Vector( 0, 0, 0 );
	Vector m_vecBBMin, m_vecBBMax;
	int m_nHitbox = 0;
	float m_flRadius = 0.0f;
};
struct AimPointData_t
{
	AimPointData_t( ) = default;
	AimPointData_t( const Vector& Point, const float& Damage, const int& Priority, HitboxData_t HitboxData ) //-V818
	{
		m_flDamage = Damage;
		m_vecPoint = Point;
		m_nPriority = Priority;
		m_HitboxData = HitboxData;
		m_bIsInvalid = false;
	}

	bool m_bIsInvalid = true;
	float m_flDamage = 0.0f;
	int m_nPriority = 0;
	Vector m_vecPoint = Vector( 0, 0, 0 );
	HitboxData_t m_HitboxData = HitboxData_t( );
};
struct AimTarget_t
{
	AimTarget_t( ) = default;
	C_BasePlayer* m_Player = nullptr;
	LagRecord_t m_Record = LagRecord_t( );
	AimPointData_t m_Point = AimPointData_t( );
};
struct CollisionData_t
{
	Semaphore* Semaphore;
	SharedMutex* m_Mutex;

	Vector m_vecStart, m_vecForward;
	HitboxData_t m_Hitbox;
	matrix3x4_t* aMatrix = nullptr;
	bool m_bResult = false;
};
struct PointData_t
{
	PointData_t( Vector vecPoint, RageHitgroup nHitGroup, int nHitBox = 0, int nPriority = 0 )
	{
		m_nHitBox = nHitBox;
		m_nHitGroup = nHitGroup;
		m_vecPoint = vecPoint;
		m_nPriority = nPriority;
	}

	RageHitgroup m_nHitGroup = RageHitgroup::Head;
	Vector m_vecPoint;
	int m_nPriority = 0;
	int m_nHitBox = 0;
};

class C_RageBot
{
public:
	/* ragebot accuracy part */
	virtual bool HasMaximumAccuracy( C_BaseCombatWeapon* pWeapon, float flWeaponInAccuracy );
	virtual float GetHitchance( );
	virtual float GetAccuracy( );
	virtual void OnVelocityModifierProxy( float flValue );
	virtual bool ShouldRunAutoStop( C_BaseCombatWeapon* pWeapon );

	/* ragebot safepoints part */
	virtual bool IsSafePoint( C_BasePlayer* Player, LagRecord_t* m_LagRecord, const Vector& vecStart, const Vector& vecEnd, HitboxData_t HitboxData );
	virtual bool CollidePoint( const Vector& vecStart, const Vector& vecEnd, mstudiobbox_t* pHitbox, matrix3x4_t* aMatrix );
	static void ThreadedCollisionFunc( CollisionData_t* m_CollisionData );

	/* ragebot autostop part */
	virtual bool IsAutoStopEnabled( )
	{
		return m_Settings.m_bAutoStop;
	}

	/* ragebot config part */
	virtual void SetupConfiguration( );
	virtual RageSettings_t* GetSettings( ) { return &m_Settings; };

	/* ragebot exploits/lua part */
	virtual int GetLastTarget( ) { return m_ScanData.m_nLastPlayer; };

#ifdef OVERSEE_DEV
	/* debug */
	void DebugDrawData( C_BasePlayer* Player );
#endif

	/* ragebot main part */
	virtual void Instance( );
	virtual void FindTarget( const Vector& vecShootPosition );
	virtual HitboxData_t GetHitboxData( C_BasePlayer* Player, matrix3x4_t* aMatrix, int nHitbox );
	virtual AimPointData_t CalculateTargetPoint( C_BasePlayer* Player, const Vector& vecShootPosition, LagRecord_t* m_Record );
	virtual void ComputePointsPriority( C_BasePlayer* Player, LagRecord_t* m_Record, std::vector < PointData_t >& Data );
	virtual void ResetData( )
	{
		m_bIsRecursed = false;
		m_ScanData.m_AimTarget = AimTarget_t( );
		m_ScanData.m_nLastPlayer = 0;
		m_ScanData.m_BadPlayers.fill( false );
		m_VelMod.m_flVelocityModifier = 1.0f;
		m_VelMod.m_bForceAccuracy = false;
	}
private:
	/* recursion */
	bool m_bIsRecursed = false;

	/* ragebot scan data */
	struct
	{
		AimTarget_t m_AimTarget = AimTarget_t( );
		int m_nLastPlayer = 0;
		std::array < bool, 64 > m_BadPlayers = { };
	} m_ScanData;

	/* struct of velmod fixes */
	struct
	{
		float m_flVelocityModifier = 1.0f;
		bool m_bForceAccuracy = false;
	} m_VelMod;

	/* cached accuracy data */
	struct
	{
		/* cached state */
		bool m_bShouldBeCached = true;

		/* both */
		float m_flDefaultAccuracy = 0.0f;

		/* accuracy boost */
		struct
		{
			std::array < std::pair < float, float >, 8 > m_Iterations;
		} m_AccuracyBoost;

		/* hitchance */
		struct
		{
			float m_flInaccuracy = 0.0f;

			std::array < float, 256 > m_Spread;
			std::array < float, 256 > m_Theta0;
			std::array < float, 256 > m_Theta1;

		} m_Seeds;
	} m_CachedData;
	
	struct
	{
		Semaphore m_DispatchSemaphore;
		SharedMutex m_SempahoreMutex;
	} m_Threading;

	/* rage settings */
	RageSettings_t m_Settings;
};

inline C_RageBot* g_RageBot = new C_RageBot( );