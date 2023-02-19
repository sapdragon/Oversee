#pragma once
#include "../../../SDK/SDK.hpp"

enum EFixedVelocity
{
	Unresolved = 0,
	MovementLayer,
	AliveLoopLayer,
	LeanLayer
};
enum EBoneMatrix
{
	Aimbot,
	Left,
	Center,
	Right,
	Visual
};
enum EPlayerActivity
{
	NoActivity = 0,
	Jump = 1,
	Land = 2
};
enum EMatrixFlags
{
	Interpolated = ( 1 << 1 ),
	BoneUsedByHitbox = ( 1 << 2 ),
	VisualAdjustment = ( 1 << 3 )
};
struct LagRecord_t
{
	float m_flSimulationTime = 0.0f;
	float m_flOldSimulationTime = 0.0f;
	float m_flCollisionChangeTime = 0.0f;
	float m_flCollisionZ = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flLowerBodyYaw = 0.0f;
	float m_flThirdPersonRecoil = 0.0f;
	float m_flMaxSpeed = 0.0f;
	float m_flDesyncDelta = 58.0f;
	float m_flAnimationVelocity = 0.0f;
	float m_flEyeYaw = 0.0f;

	QAngle m_angEyeAngles = QAngle( 0, 0, 0 );
	QAngle m_angAbsAngles = QAngle( 0, 0, 0 );
	Vector m_vecOrigin = Vector( 0, 0, 0 );
	Vector m_vecAbsOrigin = Vector( 0, 0, 0 );
	Vector m_vecVelocity = Vector( 0, 0, 0 );
	Vector m_vecMins = Vector( 0, 0, 0 );
	Vector m_vecMaxs = Vector( 0, 0, 0 );

	int m_nFlags = 0;
	int m_nSimulationTicks = 0;

	int m_nActivityTick = 0;
	float m_flActivityPlayback = 0.0f;
	float m_flDurationInAir = 0.0f;
	EPlayerActivity m_nActivityType = EPlayerActivity::NoActivity;
	EFixedVelocity m_nVelocityMode = EFixedVelocity::Unresolved;

	bool m_bDidShot = false;
	int m_nShotTick = 0;

	bool m_bIsFakePlayer = false;
	bool m_bHasBrokenLC = false;
	bool m_bFirstAfterDormant = false;
	bool m_bIsInvalid = false;
	bool m_bIsResolved = false;
	bool m_bIsWalking = false;
	bool m_bUsedAsPreviousRecord = false;
	bool m_bRestoreData = true;

	std::array < C_AnimationLayer, ANIMATION_LAYER_COUNT > m_Layers = { };
	std::array < float, MAXSTUDIOPOSEPARAM > m_PoseParameters = { };
	std::array < std::array < matrix3x4_t, MAXSTUDIOBONES >, 5 > m_Matricies;
};
struct PlayerData_t
{
	float m_flExploitTime = 0.0f;
	bool m_bLeftDormancy = false;
	bool m_bJustSpawned = true;
	LagRecord_t m_LagRecord = LagRecord_t( );
	std::array < matrix3x4_t, MAXSTUDIOBONES > m_aCachedMatrix;
	Vector m_vecLastOrigin = Vector( 0, 0, 0 );
};
class C_LagCompensation
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
	virtual void InterpolatePlayers( );
	virtual void ForcePlayerRecord( C_BasePlayer* Player, LagRecord_t* m_LagRecord );
	virtual void InvalidatePlayer( C_BasePlayer* pPlayer );
	virtual int GetRecordPriority( LagRecord_t* m_LagRecord );
	virtual void CleanPlayer( C_BasePlayer* pPlayer );
	virtual void StoreRecordData( C_BasePlayer* Player, LagRecord_t* m_LagRecord, LagRecord_t* m_PreviousRecord );
	virtual bool WasPlayerDataChanged( C_BasePlayer* Player, LagRecord_t* m_PreviousRecord );
	virtual void HandleTickbaseShift( C_BasePlayer* Player, LagRecord_t* m_PreviousRecord );
	virtual bool GetBacktrackMatrix( C_BasePlayer* Player, matrix3x4_t* aMatrix );
	virtual bool IsRecordValid( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord );
	virtual bool IsTimeValid( float flTime, int nTick );
	virtual void ResetData( )
	{
		for ( int nPlayer = 0; nPlayer < m_LagCompensationRecords.size( ); nPlayer++ )
		{
			if ( m_LagCompensationRecords[ nPlayer ].empty( ) )
				continue;

			m_LagCompensationRecords[ nPlayer ].clear( );
		}
	}

	virtual void StartLagCompensation( );
	virtual void FinishLagCompensation( );

	virtual float GetLerpTime( );
	virtual LagRecord_t FindBestRecord( C_BasePlayer* pPlayer, std::deque < LagRecord_t > m_LagRecords, int& nPriority, const float& flSimTime );
	virtual LagRecord_t FindFirstRecord( C_BasePlayer* pPlayer, std::deque < LagRecord_t > m_LagRecords );
	virtual std::deque < LagRecord_t >& GetPlayerRecords( C_BasePlayer* pPlayer )
	{
		return m_LagCompensationRecords[ pPlayer->EntIndex( ) - 1 ];
	}
	virtual PlayerData_t& GetPlayerData( C_BasePlayer* pPlayer )
	{
		return m_LagCompensationPlayerData[ pPlayer->EntIndex( ) - 1 ];
	}
private:
	std::array < LagRecord_t, 64 > m_BackupData;
	std::array < std::deque < LagRecord_t >, 64 > m_LagCompensationRecords;
	std::array < PlayerData_t, 64 > m_LagCompensationPlayerData;
};

inline C_LagCompensation* g_LagCompensation = new C_LagCompensation( );