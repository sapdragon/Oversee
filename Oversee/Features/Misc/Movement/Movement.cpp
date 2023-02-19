#include "Movement.hpp"
#include "../../Game/Networking/Networking.hpp"
#include "../../Game/Prediction/EnginePrediction.hpp"
#include "../../Game/RageBot/RageBot.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"

void C_Movement::BunnyHop( )
{
	bool bRunBunnyHop = true;
	if ( !g_SettingsManager->B[ _S( "misc.auto_jump" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) )
		bRunBunnyHop = false;

	int nRenderMode = g_Globals->m_LocalPlayer->m_nRenderMode( );
	if ( nRenderMode == 9 || nRenderMode == 8 )
		bRunBunnyHop = false;

	int nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
	if ( nMoveType == 9 || nMoveType == 8 )
		bRunBunnyHop = false;

	int nFlags = g_Globals->m_LocalPlayer->m_fFlags( );
	if ( nFlags & FL_ONGROUND )
		bRunBunnyHop = false;

	if ( bRunBunnyHop )
		g_Globals->m_Packet.m_pCmd->m_nButtons &= ~IN_JUMP;
}
void C_Movement::AutoStrafe( )
{
	bool bRunAutoStrafe = true;
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || !g_SettingsManager->B[ _S( "misc.auto_strafe" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) )
		bRunAutoStrafe = false;

	if ( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		bRunAutoStrafe = false;

	int nRenderMode = g_Globals->m_LocalPlayer->m_nRenderMode( );
	if ( nRenderMode == 9 || nRenderMode == 8 )
		bRunAutoStrafe = false;

	int nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
	if ( nMoveType == 9 || nMoveType == 8 )
		bRunAutoStrafe = false;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pCombatWeapon )
	{
		if ( pCombatWeapon->m_nItemID( ) == WEAPON_SSG08 )
		{
			if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) < 5.0f )
				bRunAutoStrafe = false;
		}
	}

	static float flOldYaw = g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw;
	static bool bFlip = false;

	if ( bRunAutoStrafe )
	{
		Vector vecVelocity = g_Globals->m_LocalPlayer->m_vecVelocity( );
		vecVelocity.z = 0.0f;

		float flForwardMove = g_Globals->m_Packet.m_pCmd->m_flForwardMove;
		float flSideMove = g_Globals->m_Packet.m_pCmd->m_flSideMove;

		float flTurnVelocityModifier = bFlip ? 1.5f : -1.5f;
		QAngle angViewAngles = g_Globals->m_Packet.m_pCmd->m_angViewAngles;

		if ( flForwardMove || flSideMove )
		{
			g_Globals->m_Packet.m_pCmd->m_flForwardMove = 0.0f;
			g_Globals->m_Packet.m_pCmd->m_flSideMove = 0.0f;

			float m_flTurnAngle = atan2( -flSideMove, flForwardMove );
			angViewAngles.yaw += m_flTurnAngle * M_RADPI;
		}
		else if ( flForwardMove )
			g_Globals->m_Packet.m_pCmd->m_flForwardMove = 0.0f;

		float flStrafeAngle = RAD2DEG( atan( 15.0f / vecVelocity.Length2D( ) ) );
		if ( flStrafeAngle > 90.0f )
			flStrafeAngle = 90.0f;
		else if ( flStrafeAngle < 0.0f )
			flStrafeAngle = 0.0f;

		Vector vecTemp = Vector( 0.0f, angViewAngles.yaw - flOldYaw, 0.0f );
		vecTemp.y = Math::NormalizeAngle( vecTemp.y );
		flOldYaw = angViewAngles.yaw;

		float flYawDelta = vecTemp.y;
		float flAbsYawDelta = fabs( flYawDelta );
		if ( flAbsYawDelta <= flStrafeAngle || flAbsYawDelta >= 30.0f )
		{
			QAngle angVelocityAngle;
			Math::VectorAngles( vecVelocity, angVelocityAngle );

			vecTemp = Vector( 0.0f, angViewAngles.yaw - angVelocityAngle.yaw, 0.0f );
			vecTemp.y = Math::NormalizeAngle( vecTemp.y );

			float flVelocityAngleYawDelta = vecTemp.y;
			float flVelocityDegree = std::clamp( RAD2DEG( atan( 30.0f / vecVelocity.Length2D( ) ) ), 0.0f, 90.0f ) * 0.005f;

			if ( flVelocityAngleYawDelta <= flVelocityDegree || vecVelocity.Length2D( ) <= 15.0f )
			{
				if ( -flVelocityDegree <= flVelocityAngleYawDelta || vecVelocity.Length2D( ) <= 15.0f )
				{
					angViewAngles.yaw += flStrafeAngle * flTurnVelocityModifier;
					g_Globals->m_Packet.m_pCmd->m_flSideMove = 450.0f * flTurnVelocityModifier;
				}
				else
				{
					angViewAngles.yaw = angVelocityAngle.yaw - flVelocityDegree;
					g_Globals->m_Packet.m_pCmd->m_flSideMove = 450.0f;
				}
			}
			else
			{
				angViewAngles.yaw = angVelocityAngle.yaw + flVelocityDegree;
				g_Globals->m_Packet.m_pCmd->m_flSideMove = -450.0f;
			}
		}
		else if ( flYawDelta > 0.0f )
			g_Globals->m_Packet.m_pCmd->m_flSideMove = 450.0f;
		else if ( flYawDelta < 0.0f )
			g_Globals->m_Packet.m_pCmd->m_flSideMove = 450.0f;

		Vector vecMove = Vector( g_Globals->m_Packet.m_pCmd->m_flForwardMove, g_Globals->m_Packet.m_pCmd->m_flSideMove, 0.0f );
		float flSpeed = vecMove.Length( );

		QAngle angMoveAngle;
		Math::VectorAngles( vecMove, angMoveAngle );

		float flNormalizedX = fmod( g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch + 180.0f, 360.0f ) - 180.0f;
		float flNormalizedY = fmod( g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw + 180.0f, 360.0f ) - 180.0f;
		float flYaw = DEG2RAD( ( flNormalizedY - angViewAngles.yaw ) + angMoveAngle.yaw );

		if ( g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch <= 200.0f && ( flNormalizedX >= 90.0f || flNormalizedX <= -90.0f || ( g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch >= 90.0f && g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch <= 200.0f ) || g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch <= -90.0f ) )
			g_Globals->m_Packet.m_pCmd->m_flForwardMove = -cos( flYaw ) * flSpeed;
		else
			g_Globals->m_Packet.m_pCmd->m_flForwardMove = cos( flYaw ) * flSpeed;

		g_Globals->m_Packet.m_pCmd->m_flSideMove = sin( flYaw ) * flSpeed;
	}

	bFlip = !bFlip;
}
void C_Movement::MouseCorrections( )
{
	QAngle angOldViewAngles;
	SDK::Interfaces::EngineClient->GetViewAngles( &angOldViewAngles );

	float delta_x = std::remainderf( g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch - angOldViewAngles.pitch, 360.0f );
	float delta_y = std::remainderf( g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw - angOldViewAngles.yaw, 360.0f );

	if ( delta_x != 0.0f )
	{
		float mouse_y = -( ( delta_x / SDK::EngineData::m_ConvarList[ CheatConvarList::Pitch ]->GetFloat( ) ) / SDK::EngineData::m_ConvarList[ CheatConvarList::Sensitivity ]->GetFloat( ) );
		short mousedy;
		if ( mouse_y <= 32767.0f ) {
			if ( mouse_y >= -32768.0f ) {
				if ( mouse_y >= 1.0f || mouse_y < 0.0f ) {
					if ( mouse_y <= -1.0f || mouse_y > 0.0f )
						mousedy = static_cast< short >( mouse_y );
					else
						mousedy = -1;
				}
				else {
					mousedy = 1;
				}
			}
			else {
				mousedy = 0x8000u;
			}
		}
		else {
			mousedy = 0x7FFF;
		}

		g_Globals->m_Packet.m_pCmd->m_nMouseDirectionY = mousedy;
	}

	if ( delta_y != 0.0f )
	{
		float mouse_x = -( ( delta_y / SDK::EngineData::m_ConvarList[ CheatConvarList::Yaw ]->GetFloat( ) ) / SDK::EngineData::m_ConvarList[ CheatConvarList::Sensitivity ]->GetFloat( ) );
		short mousedx;
		if ( mouse_x <= 32767.0f ) {
			if ( mouse_x >= -32768.0f ) {
				if ( mouse_x >= 1.0f || mouse_x < 0.0f ) {
					if ( mouse_x <= -1.0f || mouse_x > 0.0f )
						mousedx = static_cast< short >( mouse_x );
					else
						mousedx = -1;
				}
				else {
					mousedx = 1;
				}
			}
			else {
				mousedx = 0x8000u;
			}
		}
		else {
			mousedx = 0x7FFF;
		}

		g_Globals->m_Packet.m_pCmd->m_nMouseDirectionX = mousedx;
	}
}
void C_Movement::EdgeJump( )
{
	bool bRunEdgeJump = true;
	if ( !g_SettingsManager->B[ _S( "misc.edge_jump" ) ] || g_SettingsManager->B[ _S( "misc.fake_duck" ) ] || g_Globals->m_LocalPlayer->IsFrozen( ) )
		bRunEdgeJump = false;

	int nRenderMode = g_Globals->m_LocalPlayer->m_nRenderMode( );
	if ( nRenderMode == 9 || nRenderMode == 8 )
		bRunEdgeJump = false;

	int nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
	if ( nMoveType == 9 || nMoveType == 8 )
		bRunEdgeJump = false;

	int nUnpredictedFlags = g_EnginePrediction->GetUnpredictedData( )->m_nFlags;
	if ( bRunEdgeJump )
	{
		/* we must stay at prev tick */
		if ( nUnpredictedFlags & FL_ONGROUND )
		{
			/* we must be in fall in this tick */
			if ( !( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
			{
				/* JUMP */
				g_Globals->m_Packet.m_pCmd->m_nButtons |= IN_JUMP;

				/* repredict */
				g_EnginePrediction->FinishPrediction( );
				g_EnginePrediction->RestoreNetvars( );
				g_EnginePrediction->RunPrediction( );
			}
		}
	}
}
void C_Movement::FastAutoStop( )
{
	bool bRunQuickStop = true;
	if ( !g_SettingsManager->B[ _S( "misc.quick_stop" ) ] )
		return;

	int nRenderMode = g_Globals->m_LocalPlayer->m_nRenderMode( );
	if ( nRenderMode == 9 || nRenderMode == 8 )
		bRunQuickStop = false;

	int nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
	if ( nMoveType == 9 || nMoveType == 8 )
		bRunQuickStop = false;

	int nFlags = g_Globals->m_LocalPlayer->m_fFlags( );
	if ( !( nFlags & FL_ONGROUND ) )
		bRunQuickStop = false;

	int nButtons = g_Globals->m_Packet.m_pCmd->m_nButtons;
	if ( ( nButtons & IN_MOVELEFT ) || ( nButtons & IN_MOVERIGHT ) || ( nButtons & IN_MOVELEFT ) || ( nButtons & IN_FORWARD ) || ( nButtons & IN_BACK ) || ( nButtons & IN_JUMP ) )
		bRunQuickStop = false;

	float flLowVelocity = g_EnginePrediction->GetMaxSpeed( ) * 0.34f;
	if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).Length2D( ) <= flLowVelocity )
		bRunQuickStop = false;

	// run the quick stop function
	if ( bRunQuickStop )
		this->QuickStop( );
}
void C_Movement::QuickStop( )
{
	QAngle angResistance = QAngle( 0, 0, 0 );
	Math::VectorAngles( ( g_Globals->m_LocalPlayer->m_vecVelocity( ) * -1.f ), angResistance );

	angResistance.yaw = g_Globals->m_Packet.m_pCmd->m_angViewAngles.yaw - angResistance.yaw;
	angResistance.pitch = g_Globals->m_Packet.m_pCmd->m_angViewAngles.pitch - angResistance.pitch;

	Vector vecResistance = Vector( 0, 0, 0 );
	Math::AngleVectors( angResistance, vecResistance );

	g_Globals->m_Packet.m_pCmd->m_flForwardMove = std::clamp( vecResistance.x, -450.f, 450.0f );
	g_Globals->m_Packet.m_pCmd->m_flSideMove = std::clamp( vecResistance.y, -450.f, 450.0f );
}