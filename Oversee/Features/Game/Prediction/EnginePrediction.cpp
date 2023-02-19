#include "EnginePrediction.hpp"
#include "../RageBot/RageBot.hpp"
#include "../RageBot/AutoStop.hpp"
#include "../RageBot/LocalAnimations.hpp"
#include "../../Misc/Movement/Movement.hpp"
#include "../Networking/Networking.hpp"
#include "../../../SDK/Utils/Utils.hpp"

#define VEC_VIEW			Vector( 0, 0, 64 )
#define VEC_HULL_MIN		Vector( -16, -16, 0 )
#define VEC_HULL_MAX		Vector( 16, 16, 72 )
#define VEC_DUCK_HULL_MIN	Vector( -16, -16, 0 )
#define VEC_DUCK_HULL_MAX	Vector( 16, 16, 36 )
#define VEC_DUCK_VIEW		Vector( 0, 0, 46 )

void C_EnginePrediction::UpdatePrediction( ) 
{
	Vector vecAbsOrigin = g_Globals->m_LocalPlayer->GetAbsOrigin( );
	if ( SDK::Interfaces::ClientState->m_nDeltaTick > 0 )
		SDK::Interfaces::Prediction->Update
		(
			SDK::Interfaces::ClientState->m_nDeltaTick,
			SDK::Interfaces::ClientState->m_nDeltaTick > 0,
			SDK::Interfaces::ClientState->m_nLastCommandAck,
			SDK::Interfaces::ClientState->m_nLastOutgoingCommand + SDK::Interfaces::ClientState->m_nChokedCommands
		);
	g_Globals->m_LocalPlayer->SetAbsOrigin( vecAbsOrigin );

	return g_EnginePrediction->SaveNetvars( );
}
void C_EnginePrediction::OnRunCommand( int nCommand )
{
	NetvarData_t* m_Data = &m_NetVars[ nCommand % MULTIPLAYER_BACKUP ];

	m_Data->m_nTickBase = g_Globals->m_LocalPlayer->m_nTickBase( );
	m_Data->m_angAimPunchAngle = g_Globals->m_LocalPlayer->m_aimPunchAngle( );
	m_Data->m_angViewPunchAngle = g_Globals->m_LocalPlayer->m_viewPunchAngle( );
	m_Data->m_vecAimPunchAngleVel = g_Globals->m_LocalPlayer->m_aimPunchAngleVel( );
	m_Data->m_vecViewOffset = g_Globals->m_LocalPlayer->m_vecViewOffset( );
	m_Data->m_vecNetworkOrigin = g_Globals->m_LocalPlayer->m_vecNetworkOrigin( );
	m_Data->m_vecAbsVelocity = g_Globals->m_LocalPlayer->m_vecAbsVelocity( );
	m_Data->m_vecVelocity = g_Globals->m_LocalPlayer->m_vecVelocity( );
	m_Data->m_vecBaseVelocity = g_Globals->m_LocalPlayer->m_vecBaseVelocity( );
	m_Data->m_flFallVelocity = g_Globals->m_LocalPlayer->m_flFallVelocity( );
	m_Data->m_flDuckAmount = g_Globals->m_LocalPlayer->m_flDuckAmount( );
	m_Data->m_flDuckSpeed = g_Globals->m_LocalPlayer->m_flDuckSpeed( );
	m_Data->m_flVelocityModifier = g_Globals->m_LocalPlayer->m_flVelocityModifier( );
	m_Data->m_bIsPredictedData = true;
}
void C_EnginePrediction::OnVelocityModifierProxy( float flValue )
{
	m_flVelocityModifier = flValue;
	m_nVelocityAcknowledged = SDK::Interfaces::ClientState->m_nCommandAck;
}
void C_EnginePrediction::OnFrameStageNotify( ClientFrameStage_t Stage )
{
	// local must be alive and we also must receive an update
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END || !g_Globals->m_LocalPlayer->IsAlive( ) )
		return;

	// define const
	const int nSimulationTick = g_Utils->TimeToTicks( g_Globals->m_LocalPlayer->m_flSimulationTime( ) );
	const int nOldSimulationTick = g_Utils->TimeToTicks( g_Globals->m_LocalPlayer->m_flOldSimulationTime( ) );
	const int nCorrectionTicks = g_Utils->TimeToTicks( 0.03f );
	const int nServerTick = SDK::Interfaces::ClientState->m_ClockDriftMgr.m_nServerTick;

	// check time
	if ( nSimulationTick <= nOldSimulationTick || abs( nSimulationTick - nServerTick ) > nCorrectionTicks )
		return;

	// save last simulation ticks amount
	m_TickBase.m_nSimulationTicks = nSimulationTick - nServerTick;
}
int C_EnginePrediction::AdjustPlayerTimeBase( int nSimulationTicks )
{
	// get tickbase
	int nTickBase = g_Globals->m_LocalPlayer->m_nTickBase( ) + 1;

	// define const
	const int nCorrectionTicks = g_Utils->TimeToTicks( 0.03f );
	const int nChokedCmds = SDK::Interfaces::ClientState->m_nChokedCommands;

	// if client gets ahead or behind of this, we'll need to correct.
	const int nTooFastLimit = nTickBase + nCorrectionTicks + nChokedCmds - m_TickBase.m_nSimulationTicks + 1;
	const int nTooSlowLimit = nTickBase - nCorrectionTicks + nChokedCmds - m_TickBase.m_nSimulationTicks + 1;

	// correct tick 
	if ( nTickBase + 1 > nTooFastLimit || nTickBase + 1 < nTooSlowLimit )
		nTickBase += nCorrectionTicks + nChokedCmds - m_TickBase.m_nSimulationTicks;

	// save predicted tickbase
	return nTickBase + nSimulationTicks;
}
void C_EnginePrediction::OnPostNetworkDataReceived( )
{
	// don't need without localplayer
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) )
		return;
	
	// specific functions to adjust 
	auto AdjustFieldFloat = [ & ] ( float& flNetworked, float flPredicted, float flTolerance, const std::string& szFieldName ) -> bool
	{
		if ( std::fabsf( flNetworked - flPredicted ) > flTolerance )
		{
#ifdef OVERSEE_DEV
			SDK::Interfaces::CVar->ConsolePrintf( _S( "[ oversee ] Prediction error detected. %s out of tolerance ( Tolerance: %s )( Diff ( Net - Pred ): %s )\n" ), szFieldName.c_str( ), std::to_string( flTolerance ).c_str( ), std::to_string( flNetworked - flPredicted ).c_str( ) );
#endif
			return false;
		}

		flNetworked = flPredicted;
		return true;
	};
	auto AdjustFieldVector = [ & ] ( Vector& vecNetworked, Vector vecPredicted, float flTolerance, const std::string& szFieldName ) -> bool
	{
		for ( int nAxis = 0; nAxis < 3; nAxis++ )
		{
			if ( fabsf( vecNetworked[ nAxis ] - vecPredicted[ nAxis ] ) <= flTolerance )
				continue;

#ifdef OVERSEE_DEV
			SDK::Interfaces::CVar->ConsolePrintf( _S( "[ oversee ] Prediction error detected. %s out of tolerance ( Tolerance: %s )( Diff ( Net - Pred ): %s )\n" ), szFieldName.c_str( ), std::to_string( flTolerance ).c_str( ), std::to_string( vecNetworked[ nAxis ] - vecPredicted[ nAxis ] ).c_str( ) );
#endif
			return false;
		}

		vecNetworked = vecPredicted;
		return true;
	};
	auto AdjustFieldAngle = [ & ] ( QAngle& vecNetworked, QAngle vecPredicted, float flTolerance, const std::string& szFieldName ) -> bool
	{
		for ( int nAxis = 0; nAxis < 3; nAxis++ )
		{
			if ( fabsf( vecNetworked[ nAxis ] - vecPredicted[ nAxis ] ) <= flTolerance )
				continue;

#ifdef OVERSEE_DEV
			SDK::Interfaces::CVar->ConsolePrintf( _S( "[ oversee ] Prediction error detected. %s out of tolerance ( Tolerance: %s )( Diff ( Net - Pred ): %s )\n" ), szFieldName.c_str( ), std::to_string( flTolerance ).c_str( ), std::to_string( vecNetworked[ nAxis ] - vecPredicted[ nAxis ] ).c_str( ) );
#endif
			return false;
		}

		vecNetworked = vecPredicted;
		return true;
	};

	/* get predicted data */
	NetvarData_t* aNetVars = &m_NetVars[ SDK::Interfaces::ClientState->m_nCommandAck % MULTIPLAYER_BACKUP ];
	if ( !aNetVars || !aNetVars->m_bIsPredictedData )
		return;

	/* check predicted data for errors */
	bool m_bHadPredictionErrors = false;
	if ( !AdjustFieldFloat( g_Globals->m_LocalPlayer->m_flVelocityModifier( ), aNetVars->m_flVelocityModifier, 0.4f * SDK::Interfaces::GlobalVars->m_flIntervalPerTick, "m_flVelocityModifier" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldFloat( g_Globals->m_LocalPlayer->m_flFallVelocity( ), aNetVars->m_flFallVelocity, 0.5f, "m_flFallVelocity" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldFloat( g_Globals->m_LocalPlayer->m_vecViewOffset( ).z, aNetVars->m_vecViewOffset.z, 0.25f, "m_vecViewOffset[2]" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldFloat( g_Globals->m_LocalPlayer->m_flDuckAmount( ), aNetVars->m_flDuckAmount, 0.0f, "m_flDuckAmount" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldFloat( g_Globals->m_LocalPlayer->m_flDuckSpeed( ), aNetVars->m_flDuckSpeed, 0.0f, "m_flDuckSpeed" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldVector( g_Globals->m_LocalPlayer->m_vecNetworkOrigin( ), aNetVars->m_vecNetworkOrigin, 0.0625f, "m_vecNetworkOrigin" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldVector( g_Globals->m_LocalPlayer->m_vecVelocity( ), aNetVars->m_vecVelocity, 0.5f, "m_vecVelocity" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldVector( g_Globals->m_LocalPlayer->m_vecBaseVelocity( ), aNetVars->m_vecBaseVelocity, 0.05f, "m_vecBaseVelocity" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldVector( g_Globals->m_LocalPlayer->m_aimPunchAngleVel( ), aNetVars->m_vecAimPunchAngleVel, 0.5f, "m_aimPunchAngleVel" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldAngle( g_Globals->m_LocalPlayer->m_viewPunchAngle( ), aNetVars->m_angViewPunchAngle, 0.5f, "m_viewPunchAngle" ) )
	{
		m_bHadPredictionErrors = true;
	}
	if ( !AdjustFieldAngle( g_Globals->m_LocalPlayer->m_aimPunchAngle( ), aNetVars->m_angAimPunchAngle, 0.5f, "m_aimPunchAngle" ) )
	{
		m_bHadPredictionErrors = true;
	}

	/* repredict */
	if ( m_bHadPredictionErrors )
		g_EnginePrediction->ForceEngineRepredict( );
	
	// data is used
	aNetVars->m_bIsPredictedData = false;
}
void C_EnginePrediction::OnPlayerMove( )
{
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || SDK::Interfaces::GameMovement->m_Player( ) != g_Globals->m_LocalPlayer )
		return;

	if ( !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_DUCKING ) && !g_Globals->m_LocalPlayer->m_bDucking( ) && !g_Globals->m_LocalPlayer->m_bDucked( ) )
		g_Globals->m_LocalPlayer->m_vecViewOffset( ) = VEC_VIEW;
	else if ( g_Globals->m_LocalPlayer->m_bDuckUntilOnGround( ) )
	{
		Vector hullSizeNormal = VEC_HULL_MAX - VEC_HULL_MIN;
		Vector hullSizeCrouch = VEC_DUCK_HULL_MAX - VEC_DUCK_HULL_MIN;
		Vector lowerClearance = hullSizeNormal - hullSizeCrouch;
		Vector duckEyeHeight = SDK::Interfaces::GameMovement->GetPlayerViewOffset( false ) - lowerClearance;

		g_Globals->m_LocalPlayer->m_vecViewOffset( ) = duckEyeHeight;
	}
	else if ( g_Globals->m_LocalPlayer->m_bDucked( ) && !g_Globals->m_LocalPlayer->m_bDucking( ) )
		g_Globals->m_LocalPlayer->m_vecViewOffset( ) = VEC_DUCK_VIEW;
}
bool C_EnginePrediction::CanAttack( bool bRevolverCheck, int nShiftAmount )
{
	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon || ( pCombatWeapon->m_nItemID( ) != WEAPON_TASER && !pCombatWeapon->IsGun( ) ) )
		return true;

	float flPlayerTime = g_Utils->TicksToTime( g_Globals->m_LocalPlayer->m_nTickBase( ) - nShiftAmount );
	if ( pCombatWeapon->m_iClip1( ) <= 0 || g_Globals->m_LocalPlayer->m_flNextAttack( ) >= flPlayerTime )
		return false;

	bool bIsDefusing = g_Globals->m_LocalPlayer->m_bIsDefusing( );
	if ( bIsDefusing )
		return false;

	bool bWaitForNoAttack = g_Globals->m_LocalPlayer->m_bWaitForNoAttack( );
	if ( bWaitForNoAttack )
		return false;

	int nPlayerState = g_Globals->m_LocalPlayer->m_nPlayerState( );
	if ( nPlayerState )
		return false;

	if ( g_Globals->m_LocalPlayer->IsFrozen( ) )
		return false;

	bool bIsRevolver = pCombatWeapon->m_nItemID( ) == WEAPON_REVOLVER;
	if ( bIsRevolver )
	{
		if ( bRevolverCheck )
			if ( pCombatWeapon->m_flPostponeFireReadyTime( ) >= flPlayerTime || pCombatWeapon->m_Activity( ) != 208 )
				return false;
	}

	return pCombatWeapon->m_flNextPrimaryAttack( ) <= flPlayerTime;
}
void C_EnginePrediction::StartPrediction( )
{
	// save globals data
	m_flCurTime = SDK::Interfaces::GlobalVars->m_flCurTime;
	m_flFrameTime = SDK::Interfaces::GlobalVars->m_flFrameTime;

	// save prediction data
	m_bInPrediction = SDK::Interfaces::Prediction->m_bInPrediction( );
	m_bIsFirstTimePredicted = SDK::Interfaces::Prediction->m_bIsFirstTimePredicted( );

	// save local data
	g_LocalAnimations->BeforePrediction( );

	// run prediction
	g_EnginePrediction->RunPrediction( );

	// run edge jump
	g_Movement->EdgeJump( );

	// update the weapon
	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		pCombatWeapon->UpdateAccuracyPenalty( );

		// get inaccuracy and spread
		m_flSpread = pCombatWeapon->GetSpread( );
		m_flInaccuracy = pCombatWeapon->GetInaccuracy( );
	}
}
void C_EnginePrediction::RunPrediction( )
{
	Vector vecAbsOrigin = g_Globals->m_LocalPlayer->GetAbsOrigin( );

	// push prediction data
	SDK::Interfaces::Prediction->m_bInPrediction( ) = true;
	SDK::Interfaces::Prediction->m_bIsFirstTimePredicted( ) = false;

	// save old buttons
	int nButtons = g_Globals->m_Packet.m_pCmd->m_nButtons;

	// update button state
	g_EnginePrediction->UpdateButtonState( );

	// push globals data
	SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TicksToTime( g_Globals->m_LocalPlayer->m_nTickBase( ) );
	SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

	// setup velocity
	g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = g_Globals->m_LocalPlayer->m_vecVelocity( );

	// start command
	g_EnginePrediction->StartCommand( );

	// set host
	SDK::Interfaces::MoveHelper->SetHost( g_Globals->m_LocalPlayer );

	SDK::Interfaces::Prediction->SetupMove( g_Globals->m_LocalPlayer, g_Globals->m_Packet.m_pCmd, SDK::Interfaces::MoveHelper, &m_MoveData );
	SDK::Interfaces::GameMovement->ProcessMovement( g_Globals->m_LocalPlayer, &m_MoveData );
	SDK::Interfaces::Prediction->FinishMove( g_Globals->m_LocalPlayer, g_Globals->m_Packet.m_pCmd, &m_MoveData );

	// finish command
	g_EnginePrediction->FinishCommand( );

	// save last predicted cmd
	m_LastPredictedCmd = g_Globals->m_Packet.m_pCmd;

	// restore old buttons
	g_Globals->m_Packet.m_pCmd->m_nButtons = nButtons;

	// reset host
	SDK::Interfaces::MoveHelper->SetHost( nullptr );

	g_Globals->m_LocalPlayer->SetAbsOrigin( vecAbsOrigin );

	// finish gamemovemesnt
	return SDK::Interfaces::GameMovement->Reset( );
}
void C_EnginePrediction::FinishPrediction( )
{
	// reset globals data
	SDK::Interfaces::GlobalVars->m_flCurTime = m_flCurTime;
	SDK::Interfaces::GlobalVars->m_flFrameTime = m_flFrameTime;

	// reset prediction data
	SDK::Interfaces::Prediction->m_bInPrediction( ) = m_bInPrediction;
	SDK::Interfaces::Prediction->m_bIsFirstTimePredicted( ) = m_bIsFirstTimePredicted;
} 
void C_EnginePrediction::StartCommand( )
{
	// setup player data
	g_Globals->m_LocalPlayer->GetUserCmd( ) = g_Globals->m_Packet.m_pCmd;
	if ( g_Globals->m_LocalPlayer )
		g_Globals->m_LocalPlayer->SetAsPredictionPlayer( );

	// set prediction seed
	( *( *( int** ) ( SDK::EngineData::m_AddressList[ CheatAddressList::PredictionSeed ] ) ) ) = g_Globals->m_Packet.m_pCmd->m_iRandomSeed;

	// start track prediction errors
	return SDK::Interfaces::GameMovement->StartTrackPredictionErrors( g_Globals->m_LocalPlayer );
}
void C_EnginePrediction::FinishCommand( )
{
	// reset player data
	g_Globals->m_LocalPlayer->GetUserCmd( ) = NULL;
	if ( g_Globals->m_LocalPlayer )
		g_Globals->m_LocalPlayer->UnsetAsPredictionPlayer( );

	// set prediction seed
	( *( *( int** ) ( SDK::EngineData::m_AddressList[ CheatAddressList::PredictionSeed ] ) ) ) = -1;

	// start track prediction errors
	return SDK::Interfaces::GameMovement->FinishTrackPredictionErrors( g_Globals->m_LocalPlayer );
}
void C_EnginePrediction::UpdateButtonState( )
{
	// force buttons
	g_Globals->m_Packet.m_pCmd->m_nButtons |= *( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3344 );
	g_Globals->m_Packet.m_pCmd->m_nButtons &= ~*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3340 );

	// set impulse
	*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x320C ) = g_Globals->m_Packet.m_pCmd->m_nButtons;

	// save old buttons
	int nOldButtons = g_Globals->m_Packet.m_pCmd->m_nButtons;
	int nButtonDifference = nOldButtons ^ *( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3208 );

	// update button state
	*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x31FC ) = *( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3208 );
	*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3208 ) = nOldButtons;
	*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3200 ) = nOldButtons & nButtonDifference;
	*( int* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x3204 ) = nButtonDifference & ~nOldButtons;
}
float C_EnginePrediction::GetSpread( )
{
	return m_flSpread;
}
float C_EnginePrediction::GetInaccuracy( )
{
	return m_flInaccuracy;
}
float C_EnginePrediction::GetMaxSpeed( C_BasePlayer* pPlayer )
{
	C_BaseCombatWeapon* pCombatWeapon = pPlayer->m_hActiveWeapon( );
	if ( !pCombatWeapon )
		return 260.0f;
	
	C_CSWeaponData* pCombatWeaponData = pCombatWeapon->GetWeaponData( );
	if ( !pCombatWeaponData )
		return 260.0f;

	float flMaxSpeed = 260.0f;
	switch ( pCombatWeapon->m_nWeaponMode( ) )
	{
		case CSWeaponMode::Primary_Mode:
		{
			flMaxSpeed = pCombatWeaponData->m_flMaxPlayerSpeed;
		}
		break;

		case CSWeaponMode::Secondary_Mode:
		{
			flMaxSpeed = pCombatWeaponData->m_flMaxPlayerSpeedAlt;
		}
		break;
	}

	return flMaxSpeed;
}
void C_EnginePrediction::SaveNetvars( )
{
	m_PredictionData.m_nFlags					=	g_Globals->m_LocalPlayer->m_fFlags( );
	m_PredictionData.m_angAimPunchAngle			=	g_Globals->m_LocalPlayer->m_aimPunchAngle( );
	m_PredictionData.m_angViewPunchAngle		=	g_Globals->m_LocalPlayer->m_viewPunchAngle( );
	m_PredictionData.m_vecAimPunchAngleVel		=	g_Globals->m_LocalPlayer->m_aimPunchAngleVel( );
	m_PredictionData.m_flDuckAmount				=	g_Globals->m_LocalPlayer->m_flDuckAmount( );
	m_PredictionData.m_flDuckSpeed				=	g_Globals->m_LocalPlayer->m_flDuckSpeed( );
	m_PredictionData.m_flVelocityModifier		=	g_Globals->m_LocalPlayer->m_flVelocityModifier( );
	m_PredictionData.m_flStamina				=	g_Globals->m_LocalPlayer->m_flStamina( );
	m_PredictionData.m_vecVelocity				=	g_Globals->m_LocalPlayer->m_vecVelocity( );
	m_PredictionData.m_vecAbsVelocity			=	g_Globals->m_LocalPlayer->m_vecAbsVelocity( );
	m_PredictionData.m_vecOrigin				=	g_Globals->m_LocalPlayer->m_vecOrigin( );
	m_PredictionData.m_vecViewOffset			=	g_Globals->m_LocalPlayer->m_vecViewOffset( );

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		m_PredictionData.m_flAccuracyPenalty	=	pCombatWeapon->m_fAccuracyPenalty( );
		m_PredictionData.m_flRecoilIndex		=	pCombatWeapon->m_flRecoilIndex( );
	}

	m_PredictionData.m_GroundEntity				=	g_Globals->m_LocalPlayer->m_hGroundEntity( );
}
void C_EnginePrediction::RestoreNetvars( )
{
	g_Globals->m_LocalPlayer->m_fFlags( )				=	m_PredictionData.m_nFlags;
	g_Globals->m_LocalPlayer->m_aimPunchAngle( )		=	m_PredictionData.m_angAimPunchAngle;
	g_Globals->m_LocalPlayer->m_viewPunchAngle( )		=	m_PredictionData.m_angViewPunchAngle;
	g_Globals->m_LocalPlayer->m_aimPunchAngleVel( )		=	m_PredictionData.m_vecAimPunchAngleVel;
	g_Globals->m_LocalPlayer->m_flDuckAmount( )			=	m_PredictionData.m_flDuckAmount;
	g_Globals->m_LocalPlayer->m_flDuckSpeed( )			=	m_PredictionData.m_flDuckSpeed;
	g_Globals->m_LocalPlayer->m_vecVelocity( )			=	m_PredictionData.m_vecVelocity;
	g_Globals->m_LocalPlayer->m_vecAbsVelocity( )		=	m_PredictionData.m_vecAbsVelocity;
	g_Globals->m_LocalPlayer->m_vecOrigin( )			=	m_PredictionData.m_vecOrigin;
	g_Globals->m_LocalPlayer->m_vecViewOffset( )		=	m_PredictionData.m_vecViewOffset;
	g_Globals->m_LocalPlayer->m_flVelocityModifier( )	=	m_PredictionData.m_flVelocityModifier;
	g_Globals->m_LocalPlayer->m_flStamina( )			=	m_PredictionData.m_flStamina;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		pCombatWeapon->m_fAccuracyPenalty( )	=	m_PredictionData.m_flAccuracyPenalty;
		pCombatWeapon->m_flRecoilIndex( )		=	m_PredictionData.m_flRecoilIndex;
	}

	g_Globals->m_LocalPlayer->m_hGroundEntity( )		=	m_PredictionData.m_GroundEntity;
}
void C_EnginePrediction::RePredict( )
{
	this->FinishPrediction( );
	this->RestoreNetvars( );
	this->RunPrediction( );
}
void C_EnginePrediction::ForceEngineRepredict( )
{
	SDK::Interfaces::Prediction->m_iPreviousStartFrame( ) = -1;
	SDK::Interfaces::Prediction->m_iPredictedCommands( ) = 0;
	SDK::Interfaces::Prediction->m_bPreviousAckHadErrors( ) = true;
}
void C_EnginePrediction::ModifyDatamap( )
{
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) )
		return;

	typedescription_t* pVphysicsCollisionState = g_Globals->m_LocalPlayer->GetDatamapEntry( g_Globals->m_LocalPlayer->GetPredDescMap( ), _S( "m_vphysicsCollisionState" ) );
	if ( !pVphysicsCollisionState || pVphysicsCollisionState->fieldOffset == 0x103EC )
		return;

	pVphysicsCollisionState->fieldType = FIELD_FLOAT;
	pVphysicsCollisionState->fieldTolerance = ( 1.0f / 2.5f ) * SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	pVphysicsCollisionState->fieldOffset = 0x103EC;
	pVphysicsCollisionState->fieldSizeInBytes = sizeof( float );
	pVphysicsCollisionState->flatOffset[ TD_OFFSET_NORMAL ] = 0x103EC;
	g_Globals->m_LocalPlayer->GetPredDescMap( )->m_Optimized = nullptr;
}