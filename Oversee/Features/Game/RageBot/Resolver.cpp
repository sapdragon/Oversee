#include "Resolver.hpp"
#include "Animations.hpp"

void C_Resolver::HandleMiss( C_BasePlayer* Player, LagRecord_t* m_Record )
{

}
void C_Resolver::CorrectAnimations( C_BasePlayer* Player, LagRecord_t* m_Record, LagRecord_t* m_PrevRecord )
{
	/* CANCELED IN FIRST RELEASE VERSION */
	return;

}
float C_Resolver::BuildMoveYaw( C_BasePlayer* Player, float flFootYaw )
{
	/* m_PlayerAnimationStateCSGO( )->m_flMoveYawIdeal */
	float m_flMoveYawIdeal = 0.0f;

	/* Rebuild m_flMoveYawIdeal */
	float flVelocityLengthXY = Player->m_vecVelocity( ).Length2D( );
	if ( flVelocityLengthXY > 0 && Player->m_fFlags( ) & FL_ONGROUND )
	{
		// convert horizontal velocity vec to angular yaw
		float flRawYawIdeal = ( atan2( -Player->m_vecVelocity( )[ 1 ], -Player->m_vecVelocity( )[ 0 ] ) * M_RADPI );
		if ( flRawYawIdeal < 0.0f )
			flRawYawIdeal += 360.0f;

		m_flMoveYawIdeal = Math::AngleNormalize( Math::AngleDiff( flRawYawIdeal, flFootYaw ) );
	}

	/* Return m_flMoveYaw */
	return m_flMoveYawIdeal;
}
//float C_Resolver::BuildPlaybackRate( C_BasePlayer* Player, int nSide )
//{
//	/* Move sequence */
//	const int nMoveSequence = Player->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_MOVE ].m_nSequence;
//
//	/* flMoveCycleRate Value */
//	float flMoveCycleRate = 0.0f;
//
//	/* Rebuild flMoveCycleRate */
//	float flVelocityLengthXY = Player->m_vecVelocity( ).Length2D( );
//	if ( flVelocityLengthXY > 0 )
//	{
//		flMoveCycleRate = Player->GetLayerSequenceCycleRate( &Player->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_MOVE ], nMoveSequence );
//		flMoveCycleRate *= flVelocityLengthXY / fmax( Player->GetSequenceMoveDist( nMoveSequence ) / ( 1.0f / flMoveCycleRate ), 0.001f );
//		flMoveCycleRate *= Math::Lerp( Player->GetAnimationState( )->m_flWalkToRunTransition, 1.0f, CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER );
//	}
//
//	/* Calculate the final m_flPlaybackRate */
//	return flMoveCycleRate * SDK::Interfaces::GlobalVars->m_flIntervalPerTick;	
//}
//#include "../../../Interface/Renderer/Renderer.hpp"
//#include "../../Fonts/FontManager.hpp"
//void C_Resolver::DebugDrawData( C_BasePlayer* Player )
//{
//	int nCount = 0;
//	for ( float flRate : m_ResData[ Player->EntIndex( ) ].m_DebugDist )
//	{
//		nCount++;
//
//		Vector vec2DPoint;
//		if ( SDK::Interfaces::DebugOverlay->ScreenPosition( Player->GetAbsOrigin( ), vec2DPoint ) )
//			continue;
//
//		g_Renderer->AddText( g_FontManager->GetFontFromHash( FNV1A( "Player ESP" ) ), 14.0f, ImVec2( vec2DPoint.x, vec2DPoint.y + ( float ) ( nCount ) * 10.0f ), "Dist " + std::to_string( nCount ) + ": " + std::to_string( flRate ), ImColor( 255, 255, 255, 255 ), false );
//	}
//
//	nCount = 0;
//	for ( float flMoveYaw : m_ResData[ Player->EntIndex( ) ].m_MoveYaw )
//	{
//		nCount++;
//
//		Vector vec2DPoint;
//		if ( SDK::Interfaces::DebugOverlay->ScreenPosition( Player->GetAbsOrigin( ), vec2DPoint ) )
//			continue;
//
//		g_Renderer->AddText( g_FontManager->GetFontFromHash( FNV1A( "Player ESP" ) ), 14.0f, ImVec2( vec2DPoint.x + 120, vec2DPoint.y + ( float ) ( nCount ) * 10.0f ), "m_flMoveYaw " + std::to_string( nCount ) + ": " + std::to_string( flMoveYaw ), ImColor( 255, 255, 255, 255 ), false );
//	}
//}