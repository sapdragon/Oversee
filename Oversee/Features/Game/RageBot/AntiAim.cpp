#include "AntiAim.hpp"
#include "LocalAnimations.hpp"
#include "../Dormant/DormantSystem.hpp"
#include "../Networking/Networking.hpp"
#include "../Prediction/EnginePrediction.hpp"

void C_AntiAim::OnCreateMove( )
{
	if ( !g_SettingsManager->B[ _S( "ragebot.enable" ) ] || !g_SettingsManager->B[ _S( "antiaims.enable" ) ] )
	{
		Math::Normalize3( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
		Math::ClampAngles( g_Globals->m_Packet.m_pCmd->m_angViewAngles );

		return;
	}

	if ( !g_AntiAim->CheckConditions( ) )
	{
		Math::Normalize3( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
		Math::ClampAngles( g_Globals->m_Packet.m_pCmd->m_angViewAngles );

		return;
	}

	// set angles
	g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch = g_AntiAim->GetPitch( );
	g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw = g_AntiAim->GetYaw( );

	/* Compute should we set roll or not */
	bool bSetRollAntiAim = false;

	/* Set roll on slowwalk */
	if ( g_SettingsManager->B[ _S( "antiaims.slow_walk" ) ] )
		bSetRollAntiAim = true;

	/* Set roll in stand */
	const float flVelLength2D = g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( );
	if ( flVelLength2D <= 5.0f )
		bSetRollAntiAim = true;

	/* Acceleration status */
	const float flMaxSpeed = g_EnginePrediction->GetMaxSpeed( );
	if ( flVelLength2D > flMaxSpeed * 0.8f )
		m_bAccelerated = true;
	else if ( flVelLength2D < flMaxSpeed * 0.2f )
		m_bAccelerated = false;

	/* Set roll after acceleration */
	if ( m_bAccelerated )
		bSetRollAntiAim = true;

	/* Set Roll */
	if ( bSetRollAntiAim )
		g_Globals->m_Packet.m_pCmd->m_angViewAngles.roll = g_SettingsManager->I[ _S( "antiaims.roll_antiaim" ) ];
}
float C_AntiAim::GetAtTargetsYaw( )
{
	std::tuple < C_BasePlayer*, Vector, float > m_PlayerData = std::make_tuple < C_BasePlayer*, Vector, float >( NULL, Vector( 0, 0, 0 ), FLT_MAX );
	for ( int nPlayerID = 1; nPlayerID <= 64; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->IsFriend( ) || pPlayer == g_Globals->m_LocalPlayer )
			continue;

		float flDistanceToPlayer = g_Globals->m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
		if ( flDistanceToPlayer > std::get < 2 >( m_PlayerData ) )
			continue;

		// save player
		m_PlayerData = std::make_tuple( pPlayer, pPlayer->m_vecOrigin( ), flDistanceToPlayer );
	}

	// skip if we didn't find a player
	float flEyeYaw = Math::NormalizeAngle( g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw + 180.0f );
	if ( std::get < 0 >( m_PlayerData ) )
		flEyeYaw = Math::NormalizeAngle( Math::CalcAngle( g_Globals->m_LocalPlayer->m_vecOrigin( ), std::get < 1 >( m_PlayerData ) ).yaw + 180.0f );

	return flEyeYaw;
}
float C_AntiAim::GetPitch( )
{
	/* get animstate */
	C_CSGOPlayerAnimationState* m_AnimState = g_Globals->m_LocalPlayer->GetAnimationState( );
	if ( !m_AnimState )
		return 0.0f;

	/* return minimum pitch */
	return fmax( fmin( m_AnimState->m_flAimPitchMax, 89.0f ), -89.0f );
}
float C_AntiAim::GetYaw( )
{
	float flBaseRotation = g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw + 180.0f;
	if ( g_SettingsManager->B[ _S( "antiaims.at_targets" ) ] )
		flBaseRotation = g_AntiAim->GetAtTargetsYaw( );
	else
		flBaseRotation += g_SettingsManager->I[ _S( "antiaims.yaw_offset" ) ];

	if ( *g_Globals->m_Packet.m_bSendPacket )
	{
		float flJitterAmount = ( float ) ( g_SettingsManager->I[ _S( "antiaims.jitter_offset" ) ] );
		if ( m_bJitterState )
			flJitterAmount = -flJitterAmount;
		flBaseRotation += flJitterAmount;
		m_bJitterState = !m_bJitterState;

		bool bInverter = g_SettingsManager->B[ _S( "antiaims.invert_side" ) ];
		if ( m_bInverterState != bInverter )
		{
			m_bAlternativeSide = false;
			m_nDesyncSide = -m_nDesyncSide;
			m_nAlternativeDesyncSide = m_nDesyncSide;
		}
		else if ( g_SettingsManager->B[ _S( "antiaims.avoid_overlap" ) ] )
			g_AntiAim->AvoidOverlap( );

		if ( g_SettingsManager->B[ _S( "antiaims.jitter_sides" ) ] )
		{
			m_bAlternativeSide = true;
			m_nAlternativeDesyncSide = -m_nAlternativeDesyncSide;
		}
		else
			m_bAlternativeSide = false;

		m_bInverterState = bInverter;
	}
	else
	{
		if ( g_SettingsManager->B[ _S( "antiaims.avoid_overlap" ) ] )
			g_AntiAim->AvoidOverlap( );

		int nDesyncSide = m_nDesyncSide;
		if ( m_bAlternativeSide )
			nDesyncSide = m_nAlternativeDesyncSide;

		float flDesyncRange = g_SettingsManager->I[ _S( "antiaims.right_limit" ) ];
		if ( nDesyncSide < 0 )
			flDesyncRange = g_SettingsManager->I[ _S( "antiaims.left_limit" ) ];

		flBaseRotation += flDesyncRange * nDesyncSide / 0.5f;
	}

	return Math::NormalizeAngle( flBaseRotation );
}
void C_AntiAim::AvoidOverlap( )
{
	float flYawDelta = g_LocalAnimations->GetYawDelta( );
	if ( !m_bOverlap )
	{
		if ( flYawDelta > 0.0f && flYawDelta < 10.0f )
		{
			m_nDesyncSide = 1;
			m_bOverlap = true;
		}
		else if ( flYawDelta < 0.0f && flYawDelta > -10.0f )
		{
			m_nDesyncSide = -1;
			m_bOverlap = true;
		}
	}
	else if ( fabs( flYawDelta ) > 10.0f )
		m_bOverlap = false;
}
void C_AntiAim::JitterMove( )
{
	const float flSpeedPerTick = 12.0f / g_Networking->GetTickRate( );
	if ( !g_SettingsManager->B[ _S( "antiaims.jitter_move" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) )
		return;

	if ( !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) || g_Globals->m_Packet.m_pCmd->m_nButtons & IN_JUMP )
		return;

	float m_flAccelerationSpeed = 90.0f;
	if ( g_SettingsManager->B[ _S( "antiaims.slow_walk" ) ] )
		return;

	const float flTargetVelocity = ( ( g_Globals->m_Packet.m_pCmd->m_nCommand % g_Networking->GetTickRate( ) ) * flSpeedPerTick ) + 90.0f;
	if ( flTargetVelocity <= 100.0 )
		if ( flTargetVelocity >= 90.0 )
			m_flAccelerationSpeed = flTargetVelocity;
		else
			m_flAccelerationSpeed = 100.0;

	const float flWishVelocity = ( m_flAccelerationSpeed / 100.0 ) * g_EnginePrediction->GetMaxSpeed( );
	if ( flWishVelocity <= 0.0f )
		return;

	const float flMoveLength = Vector( g_Globals->m_Packet.m_pCmd->m_flForwardMove, g_Globals->m_Packet.m_pCmd->m_flSideMove, 0.0f ).Length2D( );
	if ( flMoveLength < 10.0 || flMoveLength < flWishVelocity )
		return;

	g_Globals->m_Packet.m_pCmd->m_flForwardMove = ( g_Globals->m_Packet.m_pCmd->m_flForwardMove / flMoveLength ) * flWishVelocity;
	g_Globals->m_Packet.m_pCmd->m_flSideMove = ( g_Globals->m_Packet.m_pCmd->m_flSideMove / flMoveLength ) * flWishVelocity;
}
void C_AntiAim::SlowWalk( )
{
	bool bCanSlowwalk = true;
	if ( !g_SettingsManager->B[ _S( "antiaims.slow_walk" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) || !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		bCanSlowwalk = false;

	float flMinimalSpeed = g_EnginePrediction->GetMaxSpeed( ) * 0.03f;
	if ( bCanSlowwalk )
	{
		float flSpeed2D = g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( );
		if ( flSpeed2D > flMinimalSpeed - 1.0f )
		{
			/* set movement */
			g_Globals->m_Packet.m_pCmd->m_flSideMove /= flSpeed2D / flMinimalSpeed;
			g_Globals->m_Packet.m_pCmd->m_flForwardMove /= flSpeed2D / flMinimalSpeed;

			/* clamp movement */
			g_Globals->m_Packet.m_pCmd->m_flSideMove = std::clamp( g_Globals->m_Packet.m_pCmd->m_flSideMove, -450.0f, 450.0f );
			g_Globals->m_Packet.m_pCmd->m_flForwardMove = std::clamp( g_Globals->m_Packet.m_pCmd->m_flForwardMove, -450.0f, 450.0f );
		}

		/* do not slowdown us more */
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_SPEED;
	}
}
void C_AntiAim::LegMovement( )
{
	if ( !g_SettingsManager->B[ _S( "antiaims.enable" ) ] )
		return;

}
void C_AntiAim::MicroMovement( )
{
	if ( !g_SettingsManager->B[ _S( "antiaims.enable" ) ] || g_Globals->m_Packet.m_pCmd->m_nButtons & IN_JUMP || !g_AntiAim->CheckConditions( ) )
		return;

	if ( g_Globals->m_LocalPlayer->m_nMoveType( ) == MOVETYPE_LADDER || g_Globals->m_LocalPlayer->m_nMoveType( ) == MOVETYPE_NOCLIP )
		return;

	float flVelocityLength = g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( );
	if ( flVelocityLength > 5.0f )
		return;

	float flMovementSpeed = 1.1f;
	if ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_DUCK || g_Globals->m_LocalPlayer->m_bDucking( ) || g_Globals->m_LocalPlayer->m_fFlags( ) & FL_DUCKING )
		flMovementSpeed *= 3.333334f;

	if ( g_Globals->m_Packet.m_pCmd->m_nCommand & 1 )
		flMovementSpeed = -flMovementSpeed;

	if ( fabs( g_Globals->m_Packet.m_pCmd->m_flSideMove ) < fabs( flMovementSpeed + 1.0f ) )
		g_Globals->m_Packet.m_pCmd->m_flSideMove = flMovementSpeed;
}
bool C_AntiAim::CheckConditions( )
{
	if ( !g_SettingsManager->B[ _S( "ragebot.enable" ) ] || !g_SettingsManager->B[ _S( "antiaims.enable" ) ] )
		return false;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( !pCombatWeapon || g_Globals->m_LocalPlayer->IsFrozen( ) )
		return false;

	if ( pCombatWeapon->IsKnife( ) && !g_SettingsManager->B[ _S( "antiaims.at_knife" ) ] )
		return false;

	if ( g_Globals->m_LocalPlayer->m_nMoveType( ) == MOVETYPE_NOCLIP || g_Globals->m_LocalPlayer->m_nMoveType( ) == MOVETYPE_LADDER )
		return false;

	if ( pCombatWeapon->IsGrenade( ) )
	{
		if ( !pCombatWeapon->m_bPinPulled( ) && pCombatWeapon->m_fThrowTime( ) > 0.0f )
			return false;
	}
	else
	{
		if ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK )
			if ( g_EnginePrediction->CanAttack( true ) )
				return false;

		if ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK2 )
			if ( pCombatWeapon->IsKnife( ) )
				return false;
	}

	if ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_USE )
		return false;

	return true;
}