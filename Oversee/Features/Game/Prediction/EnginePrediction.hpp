#pragma once
#include "../../../SDK/SDK.hpp"

struct TickbaseRecord_t
{
	TickbaseRecord_t( )
	{
		m_nTickBase = 0;
		m_bIsValidRecord = false;
	}
	TickbaseRecord_t( int nTick )
	{
		m_nTickBase = nTick;
		m_bIsValidRecord = true;
	}

	int m_nTickBase = 0;
	bool m_bIsValidRecord = false;
};
struct PredictionData_t
{
	int m_nTickBase = 0;
	int m_nFlags = 0;

	Vector m_vecVelocity = Vector( 0, 0, 0 );
	Vector m_vecBaseVelocity = Vector( 0, 0, 0 );
	Vector m_vecAbsVelocity = Vector( 0, 0, 0 );
	Vector m_vecViewOffset = Vector( 0, 0, 0 );
	Vector m_vecOrigin = Vector( 0, 0, 0 );
	Vector m_vecAimPunchAngleVel = Vector( 0, 0, 0 );

	QAngle m_angAimPunchAngle = QAngle( 0, 0, 0 );
	QAngle m_angViewPunchAngle = QAngle( 0, 0, 0 );

	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_flFallVelocity = 0.0f;
	float m_flAccuracyPenalty = 0.0f;
	float m_flRecoilIndex = 0.0f;
	float m_flVelocityModifier = 0.0f;
	float m_flStamina = 0.0f;

	CHandle < C_BaseEntity > m_GroundEntity;
};
struct NetvarData_t
{
	bool m_bIsPredictedData = false;

	float m_flDuckAmount = 0.f;
	float m_flDuckSpeed = 0.f;
	float m_flFallVelocity = 0.f;
	float m_flVelocityModifier = 0.f;

	int m_nTickBase = 0;
	int m_nFlags = 0;
	int m_nTeamNum = 0;

	Vector m_vecVelocity = Vector( 0.0f, 0.0f, 0.0f );
	Vector m_vecAbsVelocity = Vector( 0.0f, 0.0f, 0.0f );
	Vector m_vecBaseVelocity = Vector( 0.0f, 0.0f, 0.0f );

	Vector m_vecNetworkOrigin = Vector( 0.f, 0.f, 0.f );
	Vector m_vecViewOffset = Vector( 0.f, 0.f, 0.f );
	Vector m_vecAimPunchAngleVel = Vector( 0.f, 0.f, 0.f );

	QAngle m_angAimPunchAngle = QAngle( 0.f, 0.f, 0.f );
	QAngle m_angViewPunchAngle = QAngle( 0.f, 0.f, 0.f );
};

class C_EnginePrediction
{
public:
	virtual void ModifyDatamap( );
	virtual void UpdatePrediction( );
	virtual void OnPlayerMove( );

	virtual void OnRunCommand( int nCommand );
	virtual void OnPostNetworkDataReceived( );

	virtual void RePredict( );
	virtual void ForceEngineRepredict( );

	virtual void StartPrediction( );
	virtual void RunPrediction( );
	virtual void FinishPrediction( );
	virtual void UpdateButtonState( );
	virtual void ResetData( )
	{
		m_TickBase.m_nSimulationTicks = 0;
		m_TickBase.m_aTickBase = { };
		m_TickBase.m_aGameTickBase = { };
	}

	virtual int GetEngineTickbase( int nCommand ) { return m_TickBase.m_aGameTickBase[ nCommand % MULTIPLAYER_BACKUP ]; };

	virtual void SetTickbase( int nCommand, int nTickBase ) 
	{ 
		m_TickBase.m_aTickBase[ nCommand % MULTIPLAYER_BACKUP ] = TickbaseRecord_t( nTickBase );
	};
	virtual TickbaseRecord_t* GetTickbaseRecord( int nCommand )
	{
		return &m_TickBase.m_aTickBase[ nCommand % MULTIPLAYER_BACKUP ];
	}

	virtual int GetPacketLoss( ) { return m_TickBase.m_nSimulationTicks; };
	virtual int AdjustPlayerTimeBase( int nSimulationTicks );
	virtual void OnFrameStageNotify( ClientFrameStage_t Stage );
	virtual void OnVelocityModifierProxy( float flValue );
		
	virtual void SaveNetvars( );
	virtual void RestoreNetvars( );
	virtual bool CanAttack( bool bRevolverCheck = false, int nShiftAmount = 0 );

	virtual float GetMaxSpeed( C_BasePlayer* pPlayer = g_Globals->m_LocalPlayer );
	virtual PredictionData_t* GetUnpredictedData( )
	{
		return &m_PredictionData;
	}
public:
	virtual float GetSpread( );
	virtual float GetInaccuracy( );
private:
	virtual void StartCommand( );
	virtual void FinishCommand( );
protected:
	std::array < NetvarData_t, MULTIPLAYER_BACKUP > m_NetVars;

	struct
	{
		int m_nSimulationTicks = 0;

		std::array < TickbaseRecord_t, MULTIPLAYER_BACKUP > m_aTickBase;
		std::array < int, MULTIPLAYER_BACKUP > m_aGameTickBase;
	} m_TickBase;

	// old pred data
	bool m_bInPrediction = false;
	bool m_bIsFirstTimePredicted = false;
	
	// old globals data
	float m_flCurTime = 0.0f;
	float m_flFrameTime = 0.0f;

	float m_flVelocityModifier = 1.0f;
	int m_nVelocityAcknowledged = 0;

	// weapon data
	float m_flSpread = 0.0f;
	float m_flInaccuracy = 0.0f;

	// netvars
	PredictionData_t m_PredictionData;

	// move data
	C_MoveData m_MoveData = C_MoveData( );

	// save last cmd
	C_UserCmd* m_LastPredictedCmd = NULL;
};

inline C_EnginePrediction* g_EnginePrediction = new C_EnginePrediction( );