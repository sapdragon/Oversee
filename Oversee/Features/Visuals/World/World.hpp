#pragma once
#include "../../../SDK/SDK.hpp"

struct ClientImpact_t
{
	Vector m_vecPosition;
	float m_flTime;
	float m_flExpirationTime;
};
class C_World
{
public:
	virtual void OnFrameStageNotify( ClientFrameStage_t Stage );
	virtual void DestroySmokeParticles( );
	virtual void DestroySmokeRenderer( );
	virtual void DestroyHandShaking( );

	virtual void ResetData( )
	{
		/* reset statements */
		m_Statements.m_bLastSmokeState = false;
		m_Statements.m_bLastShadowState = false;
		m_Statements.m_bLastShakeState = false;
		m_Statements.m_bLastBloodState = false;
		m_Statements.m_bUnlockedConVars = false;
		m_Statements.m_bFilteredConsole = false;
	
		/* reset clantag */
		m_ClanTag.m_flTime = 0.0f;
		m_ClanTag.m_nState = 0;

		/* reset impacts */
		m_Impacts.m_nLastDrawn = 0;
		m_Impacts.m_flTurnTime = 0.0f;
	}
	virtual void RunFilters( );
	virtual void RunVisualFeatures( );
	virtual void PreserveKillfeed( );
	virtual void PenetrationCrosshair( );
	virtual void PerformSkyBoxTexture( );
	virtual void RunClanTagChanger( );
	virtual void FogChanger( );
	virtual void GrenadeWarning( );
private:
	struct
	{
		bool m_bLastSmokeState = false;
		bool m_bLastShadowState = false;
		bool m_bLastShakeState = false;
		bool m_bLastBloodState = false;
		bool m_bUnlockedConVars = false;
		bool m_bFilteredConsole = false;
	} m_Statements;

	struct
	{
		int m_nState = 0;
		float m_flTime = 0.0f;
	} m_ClanTag;

	struct
	{
		int m_nLastDrawn = 0;
		float m_flTurnTime = 0.0f;
	} m_Impacts;

	struct
	{
		int m_nServerTick = 0;
	} m_Warning;
};

inline C_World* g_World = new C_World( );