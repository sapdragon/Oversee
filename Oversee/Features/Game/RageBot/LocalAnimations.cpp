#include <tuple>
#include "LocalAnimations.hpp"
#include "../../Inventory Changer/InventoryChanger.hpp"
#include "../Networking/Networking.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_LocalAnimations::OnCreateMove( )
{
	g_LocalAnimations->StoreAnimationRecord( );
	if ( !*g_Globals->m_Packet.m_bSendPacket )
		return;

	std::tuple < float, float, float, float, float, int, int > m_Globals = std::make_tuple
	(
		SDK::Interfaces::GlobalVars->m_flCurTime,
		SDK::Interfaces::GlobalVars->m_flRealTime,
		SDK::Interfaces::GlobalVars->m_flFrameTime,
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime,
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount,

		SDK::Interfaces::GlobalVars->m_iFrameCount,
		SDK::Interfaces::GlobalVars->m_iTickCount
	);

	this->m_LocalData.m_nSimulationTicks = SDK::Interfaces::ClientState->m_nChokedCommands + 1;
	std::tuple < QAngle, QAngle, float, float, Vector, Vector, Vector, Vector, int, int, int, float, float > m_Data = std::make_tuple		
	(
		g_Globals->m_LocalPlayer->m_angVisualAngles( ),
		g_Globals->m_LocalPlayer->m_angEyeAngles( ),
		g_Globals->m_LocalPlayer->m_flDuckAmount( ),
		g_Globals->m_LocalPlayer->m_flDuckSpeed( ),
		g_Globals->m_LocalPlayer->GetAbsOrigin( ),
		g_Globals->m_LocalPlayer->m_vecOrigin( ),
		g_Globals->m_LocalPlayer->m_vecAbsVelocity( ),
		g_Globals->m_LocalPlayer->m_vecVelocity( ),
		g_Globals->m_LocalPlayer->m_iEFlags( ),
		g_Globals->m_LocalPlayer->m_fFlags( ),
		g_Globals->m_LocalPlayer->m_nMoveType( ),
		g_Globals->m_LocalPlayer->m_flThirdpersonRecoil( ),
		g_Globals->m_LocalPlayer->m_flLowerBodyYaw( )
	);

	/* set localplayer entity's flags */
	g_Globals->m_LocalPlayer->m_iEFlags( ) &= ~( EFL_DIRTY_ABSVELOCITY | EFL_DIRTY_ABSTRANSFORM );
	
	/* remove interpolation amount */
	SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;

	/* shot data */
	std::tuple < QAngle, bool > m_ShotData = std::make_tuple < QAngle, bool > ( QAngle( 0, 0, 0 ), false );

	/* copy data */
	g_LocalAnimations->CopyPlayerAnimationData( false );

	/* UpdatePlayerAnimations */
	for ( int nSimulationTick = 1; nSimulationTick <= m_LocalData.m_nSimulationTicks; nSimulationTick++ )
	{
		/* determine the tickbase and set globals to it */
		int nTickBase = g_Globals->m_LocalPlayer->m_nTickBase( ) - m_LocalData.m_nSimulationTicks + nSimulationTick;
		SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TicksToTime( nTickBase );
		SDK::Interfaces::GlobalVars->m_flRealTime = g_Utils->TicksToTime( nTickBase );
		SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_iFrameCount = nTickBase;
		SDK::Interfaces::GlobalVars->m_iTickCount = nTickBase;

		AnimationRecord_t* m_Record = &m_LocalData.m_AnimRecords[ ( g_Globals->m_Packet.m_pCmd->m_nCommand - m_LocalData.m_nSimulationTicks + nSimulationTick ) % MULTIPLAYER_BACKUP ];
		if ( m_Record )
		{
			/* set player data from the animation record */
			g_Globals->m_LocalPlayer->m_flThirdpersonRecoil( ) = m_Record->m_angAimPunch.pitch * SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponRecoilScale ]->GetFloat( );
			g_Globals->m_LocalPlayer->m_vecVelocity( ) = m_Record->m_vecVelocity;
			g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = m_Record->m_vecVelocity;
			g_Globals->m_LocalPlayer->m_flDuckAmount( ) = m_Record->m_flDuckAmount;
			g_Globals->m_LocalPlayer->m_flDuckSpeed( ) = m_Record->m_flDuckSpeed;
			g_Globals->m_LocalPlayer->m_angVisualAngles( ) = m_Record->m_angRealAngles;
			g_Globals->m_LocalPlayer->m_angEyeAngles( ) = m_Record->m_angRealAngles;
			g_Globals->m_LocalPlayer->m_fFlags( ) = m_Record->m_nFlags;
			g_Globals->m_LocalPlayer->m_nMoveType( ) = m_Record->m_nMoveType;

			/* fix localplayer strafe and sequences */
			g_LocalAnimations->SimulateStrafe( m_Record->m_nButtons );
			g_LocalAnimations->DoAnimationEvent( m_Record->m_nButtons );

			/* set shot angle */
			if ( nSimulationTick == m_LocalData.m_nSimulationTicks )
			{
				if ( std::get < 1 > ( m_ShotData ) )
				{
					g_Globals->m_LocalPlayer->m_angVisualAngles( ) = std::get < 0 > ( m_ShotData );
					g_Globals->m_LocalPlayer->m_angEyeAngles( ) = std::get < 0 > ( m_ShotData );
				}
			}
			else
			{
				if ( m_Record->m_bIsShooting )
				{
					std::get < 0 > ( m_ShotData ) = m_Record->m_angRealAngles;
					std::get < 1 > ( m_ShotData ) = true;
				}
			}
		}

		/* Fix framecount and time */
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_nLastUpdateFrame = 0;
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_flLastUpdateTime = SDK::Interfaces::GlobalVars->m_flCurTime - SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

		/* set player and weapon */
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_pBasePlayer = g_Globals->m_LocalPlayer;
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );

		/* force client-side animation */
		bool m_bClientSideAnimation = g_Globals->m_LocalPlayer->m_bClientSideAnimation( );
		g_Globals->m_LocalPlayer->m_bClientSideAnimation( ) = true;

		/* update localplayer animations */
		g_Globals->m_Animations.m_bUpdateAnims = true;
		g_Globals->m_LocalPlayer->UpdateClientSideAnimation( );
		g_Globals->m_Animations.m_bUpdateAnims = false;

		/* restore client-side animation */
		g_Globals->m_LocalPlayer->m_bClientSideAnimation( ) = m_bClientSideAnimation;
	}

	/* copy layers */
	std::memcpy( m_LocalData.m_Real.m_Layers.data( ), g_Globals->m_LocalPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	std::memcpy( m_LocalData.m_Real.m_PoseParameters.data( ), g_Globals->m_LocalPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );

	g_Globals->m_LocalPlayer->SetAbsOrigin( m_LocalData.m_vecAbsOrigin );
	//if ( !g_Globals->m_Packet.m_bSkipMatrix )
		g_LocalAnimations->SetupPlayerBones( m_LocalData.m_Real.m_Matrix.data( ), BONE_USED_BY_ANYTHING );
	g_LocalAnimations->UpdateDesyncAnimations(  );

	/* restore globals */
	SDK::Interfaces::GlobalVars->m_flCurTime = std::get < 0 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_flRealTime = std::get < 1 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_flFrameTime = std::get < 2 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_flAbsFrameTime = std::get < 3 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_flInterpolationAmount = std::get < 4 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_iFrameCount = std::get < 5 > ( m_Globals );
	SDK::Interfaces::GlobalVars->m_iTickCount = std::get < 6 > ( m_Globals );

	/* restore changed localplayer's data */
	g_Globals->m_LocalPlayer->m_angVisualAngles( ) = std::get < 0 > ( m_Data );
	g_Globals->m_LocalPlayer->m_angEyeAngles( ) = std::get < 1 > ( m_Data );
	g_Globals->m_LocalPlayer->m_flDuckAmount( ) = std::get < 2 > ( m_Data );
	g_Globals->m_LocalPlayer->m_flDuckSpeed( ) = std::get < 3 > ( m_Data );
	g_Globals->m_LocalPlayer->m_vecOrigin( ) = std::get < 5 > ( m_Data );
	g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = std::get < 6 > ( m_Data );
	g_Globals->m_LocalPlayer->m_vecVelocity( ) = std::get < 7 > ( m_Data );
	g_Globals->m_LocalPlayer->m_iEFlags( ) = std::get < 8 > ( m_Data );
	g_Globals->m_LocalPlayer->m_fFlags( ) = std::get < 9 > ( m_Data );
	g_Globals->m_LocalPlayer->m_nMoveType( ) = std::get < 10 > ( m_Data );
	g_Globals->m_LocalPlayer->m_flThirdpersonRecoil( ) = std::get < 11 > ( m_Data );
	g_Globals->m_LocalPlayer->m_flLowerBodyYaw( ) = std::get < 12 >( m_Data );
}
void C_LocalAnimations::CopyPlayerAnimationData( bool bFake )
{
	std::array < C_AnimationLayer, ANIMATION_LAYER_COUNT > m_Layers = m_LocalData.m_Real.m_Layers;
	if ( bFake )
		m_Layers = m_LocalData.m_Fake.m_Layers;

	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		&m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		sizeof( C_AnimationLayer )
	);
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		&m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		sizeof( C_AnimationLayer )
	);
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ALIVELOOP ],
		&m_Layers[ ANIMATION_LAYER_ALIVELOOP ],
		sizeof( C_AnimationLayer )
	);
}
void C_LocalAnimations::UpdateDesyncAnimations( )
{
	C_CSGOPlayerAnimationState m_AnimationState;
	std::memcpy( &m_AnimationState, g_Globals->m_LocalPlayer->GetAnimationState( ), sizeof( C_CSGOPlayerAnimationState ) );

	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		&m_LocalData.m_Fake.m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		sizeof( C_AnimationLayer )
	);
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		&m_LocalData.m_Fake.m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		sizeof( C_AnimationLayer )
	);
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ALIVELOOP ],
		&m_LocalData.m_Fake.m_Layers[ ANIMATION_LAYER_ALIVELOOP ],
		sizeof( C_AnimationLayer )
	);

	std::memcpy( g_Globals->m_LocalPlayer->GetAnimationState( ), &m_LocalData.m_Fake.m_AnimationState, sizeof( C_CSGOPlayerAnimationState ) );
	std::memcpy( g_Globals->m_LocalPlayer->m_aPoseParameters( ).data( ), m_LocalData.m_Fake.m_PoseParameters.data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );

	std::tuple < QAngle, bool > m_ShotData = std::make_tuple < QAngle, bool >( QAngle( 0, 0, 0 ), false );

	/* UpdatePlayerAnimations */
	for ( int nSimulationTick = 1; nSimulationTick <= m_LocalData.m_nSimulationTicks; nSimulationTick++ )
	{
		/* determine the tickbase and set globals to it */
		int nTickBase = g_Globals->m_LocalPlayer->m_nTickBase( ) - m_LocalData.m_nSimulationTicks + nSimulationTick;
		SDK::Interfaces::GlobalVars->m_flCurTime = g_Utils->TicksToTime( nTickBase );
		SDK::Interfaces::GlobalVars->m_flRealTime = g_Utils->TicksToTime( nTickBase );
		SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_iFrameCount = nTickBase;
		SDK::Interfaces::GlobalVars->m_iTickCount = nTickBase;

		AnimationRecord_t* m_Record = &m_LocalData.m_AnimRecords[ ( g_Globals->m_Packet.m_pCmd->m_nCommand - m_LocalData.m_nSimulationTicks + nSimulationTick ) % MULTIPLAYER_BACKUP ];
		if ( m_Record )
		{
			/* set player data from the animation record */
			g_Globals->m_LocalPlayer->m_flThirdpersonRecoil( ) = m_Record->m_angAimPunch.pitch * SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponRecoilScale ]->GetFloat( );
			g_Globals->m_LocalPlayer->m_vecVelocity( ) = m_Record->m_vecVelocity;
			g_Globals->m_LocalPlayer->m_vecAbsVelocity( ) = m_Record->m_vecVelocity;
			g_Globals->m_LocalPlayer->m_flDuckAmount( ) = m_Record->m_flDuckAmount;
			g_Globals->m_LocalPlayer->m_flDuckSpeed( ) = m_Record->m_flDuckSpeed;
			g_Globals->m_LocalPlayer->m_angVisualAngles( ) = m_Record->m_angFakeAngles;
			g_Globals->m_LocalPlayer->m_angEyeAngles( ) = m_Record->m_angFakeAngles;
			g_Globals->m_LocalPlayer->m_fFlags( ) = m_Record->m_nFlags;
			g_Globals->m_LocalPlayer->m_nMoveType( ) = m_Record->m_nMoveType;

			/* fix localplayer strafe and sequences */
			g_LocalAnimations->SimulateStrafe( m_Record->m_nButtons );
			g_LocalAnimations->DoAnimationEvent( m_Record->m_nButtons, true );

			/* set shot angle */
			if ( nSimulationTick == m_LocalData.m_nSimulationTicks )
			{
				if ( std::get < 1 > ( m_ShotData ) )
				{
					g_Globals->m_LocalPlayer->m_angVisualAngles( ) = std::get < 0 > ( m_ShotData );
					g_Globals->m_LocalPlayer->m_angEyeAngles( ) = std::get < 0 > ( m_ShotData );
				}
			}
			else
			{
				if ( m_Record->m_bIsShooting )
				{
					std::get < 0 > ( m_ShotData ) = m_Record->m_angRealAngles;
					std::get < 1 > ( m_ShotData ) = true;
				}
			}
		}

		/* Fix framecount */
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_nLastUpdateFrame = 0;

		/* set player and weapon */
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_pBasePlayer = g_Globals->m_LocalPlayer;
		g_Globals->m_LocalPlayer->GetAnimationState( )->m_pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );

		/* force client-side animation */
		bool m_bClientSideAnimation = g_Globals->m_LocalPlayer->m_bClientSideAnimation( );
		g_Globals->m_LocalPlayer->m_bClientSideAnimation( ) = true;

		/* update localplayer animations */
		g_Globals->m_Animations.m_bUpdateAnims = true;
		g_Globals->m_LocalPlayer->UpdateClientSideAnimation( );
		g_Globals->m_Animations.m_bUpdateAnims = false;

		/* restore client-side animation */
		g_Globals->m_LocalPlayer->m_bClientSideAnimation( ) = m_bClientSideAnimation;
	}

	std::memcpy( &m_LocalData.m_Fake.m_AnimationState, g_Globals->m_LocalPlayer->GetAnimationState( ), sizeof( C_CSGOPlayerAnimationState ) );
	std::memcpy( m_LocalData.m_Fake.m_Layers.data( ), g_Globals->m_LocalPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	std::memcpy( m_LocalData.m_Fake.m_PoseParameters.data( ), g_Globals->m_LocalPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );
	
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		&m_LocalData.m_Real.m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ],
		sizeof( C_AnimationLayer )
	);
	std::memcpy
	(
		&g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		&m_LocalData.m_Real.m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ],
		sizeof( C_AnimationLayer )
	);

	g_Globals->m_LocalPlayer->m_aPoseParameters( )[ 1 ] = m_LocalData.m_Real.m_PoseParameters[ 1 ];
	std::memcpy( &g_Globals->m_LocalPlayer->GetAnimationLayers( )[ 7 ], &m_LocalData.m_Real.m_Layers[ 7 ], sizeof( C_AnimationLayer ) );

	m_LocalData.m_flYawDelta = std::roundf( Math::AngleDiff( Math::NormalizeAngle( g_Globals->m_LocalPlayer->GetAnimationState( )->m_flFootYaw ), Math::NormalizeAngle( m_AnimationState.m_flFootYaw ) ) );

	g_LocalAnimations->SetupPlayerBones( m_LocalData.m_Fake.m_Matrix.data( ), BONE_USED_BY_ANYTHING );

	std::memcpy( g_Globals->m_LocalPlayer->GetAnimationState( ), &m_AnimationState, sizeof( C_CSGOPlayerAnimationState ) );
	std::memcpy( g_Globals->m_LocalPlayer->GetAnimationLayers( ), m_LocalData.m_Real.m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	std::memcpy( g_Globals->m_LocalPlayer->m_aPoseParameters( ).data( ), m_LocalData.m_Real.m_PoseParameters.data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );
}
void C_LocalAnimations::SimulateStrafe( int nButtons )
{
	Vector vecForward;
	Vector vecRight;
	Vector vecUp;

	Math::AngleVectors( QAngle( 0, g_Globals->m_LocalPlayer->GetAnimationState( )->m_flFootYaw, 0 ), vecForward, vecRight, vecUp );
	vecRight.NormalizeInPlace( );

	float flVelToRightDot = Math::DotProduct( g_Globals->m_LocalPlayer->GetAnimationState( )->m_vecVelocityNormalizedNonZero, vecRight );
	float flVelToForwardDot = Math::DotProduct( g_Globals->m_LocalPlayer->GetAnimationState( )->m_vecVelocityNormalizedNonZero, vecForward );

	bool bMoveRight = ( nButtons & ( IN_MOVERIGHT ) ) != 0;
	bool bMoveLeft = ( nButtons & ( IN_MOVELEFT ) ) != 0;
	bool bMoveForward = ( nButtons & ( IN_FORWARD ) ) != 0;
	bool bMoveBackward = ( nButtons & ( IN_BACK ) ) != 0;

	bool bStrafeRight = ( g_Globals->m_LocalPlayer->GetAnimationState( )->m_flSpeedAsPortionOfWalkTopSpeed >= 0.73f && bMoveRight && !bMoveLeft && flVelToRightDot < -0.63f );
	bool bStrafeLeft = ( g_Globals->m_LocalPlayer->GetAnimationState( )->m_flSpeedAsPortionOfWalkTopSpeed >= 0.73f && bMoveLeft && !bMoveRight && flVelToRightDot > 0.63f );
	bool bStrafeForward = ( g_Globals->m_LocalPlayer->GetAnimationState( )->m_flSpeedAsPortionOfWalkTopSpeed >= 0.65f && bMoveForward && !bMoveBackward && flVelToForwardDot < -0.55f );
	bool bStrafeBackward = ( g_Globals->m_LocalPlayer->GetAnimationState( )->m_flSpeedAsPortionOfWalkTopSpeed >= 0.65f && bMoveBackward && !bMoveForward && flVelToForwardDot > 0.55f );

	g_Globals->m_LocalPlayer->m_bStrafing( ) = ( bStrafeRight || bStrafeLeft || bStrafeForward || bStrafeBackward );
}
void C_LocalAnimations::DoAnimationEvent( int nButtons, bool bIsFakeAnimations )
{
	C_AnimationLayer* pLandOrClimbLayer = &g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];
	if ( !pLandOrClimbLayer )
		return;

	C_AnimationLayer* pJumpOrFallLayer = &g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ];
	if ( !pJumpOrFallLayer )
		return;

	int nCurrentFlags = m_LocalData.m_Real.m_nFlags;
	if ( bIsFakeAnimations )
		nCurrentFlags = m_LocalData.m_Fake.m_nFlags;

	int nCurrentMoveType = m_LocalData.m_Real.m_nMoveType;
	if ( bIsFakeAnimations )
		nCurrentMoveType = m_LocalData.m_Fake.m_nMoveType;

	if ( nCurrentMoveType != MOVETYPE_LADDER && g_Globals->m_LocalPlayer->m_nMoveType( ) == MOVETYPE_LADDER )
		g_Globals->m_LocalPlayer->GetAnimationState( )->SetLayerSequence( pLandOrClimbLayer, ACT_CSGO_CLIMB_LADDER );
	else if ( nCurrentMoveType == MOVETYPE_LADDER && g_Globals->m_LocalPlayer->m_nMoveType( ) != MOVETYPE_LADDER )
		g_Globals->m_LocalPlayer->GetAnimationState( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_FALL );
	else
	{
		if ( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		{
			if ( !( nCurrentFlags & FL_ONGROUND ) )
				g_Globals->m_LocalPlayer->GetAnimationState( )->SetLayerSequence
				( 
					pLandOrClimbLayer, 
					g_Globals->m_LocalPlayer->GetAnimationState( )->m_flDurationInAir > 1.0f ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT 
				);
		}
		else if ( nCurrentFlags & FL_ONGROUND )
		{
			if ( g_Globals->m_LocalPlayer->m_vecVelocity( ).z > 0.0f )
				g_Globals->m_LocalPlayer->GetAnimationState( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_JUMP );
			else
				g_Globals->m_LocalPlayer->GetAnimationState( )->SetLayerSequence( pJumpOrFallLayer, ACT_CSGO_FALL );
		}
	}

	if ( bIsFakeAnimations )
	{
		m_LocalData.m_Fake.m_nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
		m_LocalData.m_Fake.m_nFlags = g_Globals->m_LocalPlayer->m_fFlags( );
	}
	else
	{
		m_LocalData.m_Real.m_nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );
		m_LocalData.m_Real.m_nFlags = g_Globals->m_LocalPlayer->m_fFlags( );
	}
}
void C_LocalAnimations::StoreAnimationRecord( )
{
	AnimationRecord_t m_AnimRecord;
	
	// store record data
	m_AnimRecord.m_nFlags = m_LocalData.m_nFlags;
	m_AnimRecord.m_vecOrigin = g_Globals->m_LocalPlayer->m_vecOrigin( );
	m_AnimRecord.m_vecVelocity = g_Globals->m_LocalPlayer->m_vecVelocity( );
	m_AnimRecord.m_flDuckAmount = g_Globals->m_LocalPlayer->m_flDuckAmount( );
	m_AnimRecord.m_flDuckSpeed = g_Globals->m_LocalPlayer->m_flDuckSpeed( );
	m_AnimRecord.m_angRealAngles = g_Globals->m_Packet.m_pCmd->m_angViewAngles;
	m_AnimRecord.m_angFakeAngles = g_Globals->m_Packet.m_angFakeAngles;
	m_AnimRecord.m_angAimPunch = g_Globals->m_LocalPlayer->m_aimPunchAngle( );
	m_AnimRecord.m_nButtons = g_Globals->m_Packet.m_pCmd->m_nButtons;
	m_AnimRecord.m_nMoveType = g_Globals->m_LocalPlayer->m_nMoveType( );

	C_BaseCombatWeapon* pWeapon = g_Globals->m_LocalPlayer->m_hActiveWeapon( );
	if ( pWeapon )
	{
		if ( pWeapon->IsGrenade( ) )
		{
			if ( !pWeapon->m_bPinPulled( ) && pWeapon->m_fThrowTime( ) > 0.0f )
				m_AnimRecord.m_bIsShooting = true;
		}
		else if ( ( pWeapon->m_nItemID( ) == WEAPON_REVOLVER && g_Globals->m_Packet.m_bCanFireRev ) || ( pWeapon->m_nItemID( ) != WEAPON_REVOLVER && g_Globals->m_Packet.m_bCanFire ) )
		{
			if ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK )
				m_AnimRecord.m_bIsShooting = true;

			if ( pWeapon->IsKnife( ) )
				if ( ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK ) || ( g_Globals->m_Packet.m_pCmd->m_nButtons & IN_ATTACK2 ) )
					m_AnimRecord.m_bIsShooting = true;
		}
	}

	if ( m_AnimRecord.m_bIsShooting )
		m_AnimRecord.m_angFakeAngles = g_Globals->m_Packet.m_pCmd->m_angViewAngles;
	m_AnimRecord.m_angFakeAngles.roll = 0.0f;

	/* proper roll aa display */
	m_LocalData.m_AnimRecords[ g_Globals->m_Packet.m_pCmd->m_nCommand % MULTIPLAYER_BACKUP ] = m_AnimRecord;
}
void C_LocalAnimations::BeforePrediction( )
{
	m_LocalData.m_nFlags = g_Globals->m_LocalPlayer->m_fFlags( );
	m_LocalData.m_vecAbsOrigin = g_Globals->m_LocalPlayer->GetAbsOrigin( );

	if ( m_LocalData.m_flSpawnTime != g_Globals->m_LocalPlayer->m_flSpawnTime( ) )
	{
		std::memcpy( &m_LocalData.m_Fake.m_AnimationState, g_Globals->m_LocalPlayer->GetAnimationState( ), sizeof( C_CSGOPlayerAnimationState ) );
		std::memcpy( m_LocalData.m_Fake.m_Layers.data( ), g_Globals->m_LocalPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
		std::memcpy( m_LocalData.m_Fake.m_PoseParameters.data( ), g_Globals->m_LocalPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * MAXSTUDIOPOSEPARAM );
	}
	m_LocalData.m_flSpawnTime = g_Globals->m_LocalPlayer->m_flSpawnTime( );
}
void C_LocalAnimations::SetupShootPosition( )
{
	/* fix view offset */
	Vector vecViewOffset = g_Globals->m_LocalPlayer->m_vecViewOffset( );
	if ( vecViewOffset.z <= 46.05f )
		vecViewOffset.z = 46.0f;
	else if ( vecViewOffset.z > 64.0f )
		vecViewOffset.z = 64.0f;

	/* calculate default shoot position */
	m_LocalData.m_vecShootPosition = g_Globals->m_LocalPlayer->m_vecOrigin( ) + vecViewOffset;

	/* backup data */
	std::tuple < Vector, QAngle > m_Backup = std::make_tuple( g_Globals->m_LocalPlayer->GetAbsOrigin( ), g_Globals->m_LocalPlayer->m_angEyeAngles( ) );

	/* force LocalPlayer data */
	g_Globals->m_LocalPlayer->SetAbsOrigin( g_Globals->m_LocalPlayer->m_vecOrigin( ) );
	g_Globals->m_LocalPlayer->m_angEyeAngles( ) = g_Globals->m_Packet.m_pCmd->m_angViewAngles;

	/* normalize angles */
	Math::Normalize3( g_Globals->m_Packet.m_pCmd->m_angViewAngles );
	Math::ClampAngles( g_Globals->m_Packet.m_pCmd->m_angViewAngles );

	/* modify eye position rebuild */
	{
		/* should we modify eye pos */
		bool bModifyEyePosition = false;
		 
		/* modify eye pos on land */
		static int Flags = g_Globals->m_LocalPlayer->m_fFlags( );
		if ( Flags != g_Globals->m_LocalPlayer->m_fFlags( ) )
		{
			if ( !( Flags & FL_ONGROUND ) && ( g_Globals->m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
				bModifyEyePosition = true;
			
			Flags = g_Globals->m_LocalPlayer->m_fFlags( );
		}

		/* modify eye pos on duck */
		if ( g_Globals->m_LocalPlayer->m_flDuckAmount( ) != 0.0f )
			bModifyEyePosition = true;

		/* modify eye pos on FD */
		if ( g_Globals->m_Packet.m_bFakeDucking )
			bModifyEyePosition = true;
		
		/* modify LocalPlayer's EyePosition */
		if ( bModifyEyePosition )
		{
			/* store old body pitch */
			const float m_flOldBodyPitch = g_Globals->m_LocalPlayer->m_aPoseParameters( )[ 12 ];
			
			/* determine m_flThirdpersonRecoil */
			const float m_flThirdpersonRecoil = g_Globals->m_LocalPlayer->m_aimPunchAngle( ).pitch * SDK::EngineData::m_ConvarList[ CheatConvarList::WeaponRecoilScale ]->GetFloat( );

			/* set body pitch */
			g_Globals->m_LocalPlayer->m_aPoseParameters( )[ 12 ] = std::clamp( Math::AngleDiff( Math::AngleNormalize( m_flThirdpersonRecoil ), 0.0f ), 0.0f, 1.0f );

			/* build matrix */
			g_LocalAnimations->SetupPlayerBones( m_LocalData.m_Shoot.m_Matrix.data( ), BONE_USED_BY_HITBOX );

			/* reset body pitch */
			g_Globals->m_LocalPlayer->m_aPoseParameters( )[ 12 ] = m_flOldBodyPitch;

			/* C_CSGOPlayerAnimationState::ModifyEyePosition rebuild */
			g_LocalAnimations->ModifyEyePosition( m_LocalData.m_vecShootPosition, m_LocalData.m_Shoot.m_Matrix.data( ) );
		}
	}

	/* restore LocalPlayer data */
	g_Globals->m_LocalPlayer->SetAbsOrigin( std::get < 0 >( m_Backup ) );
	g_Globals->m_LocalPlayer->m_angEyeAngles( ) = std::get < 1 >( m_Backup );
}
void C_LocalAnimations::SetupPlayerBones( matrix3x4_t* aMatrix, int nMask )
{
	// save globals
	std::tuple < float, float, float, float, float, int, int > m_Globals = std::make_tuple
	(
		// backup globals
		SDK::Interfaces::GlobalVars->m_flCurTime,
		SDK::Interfaces::GlobalVars->m_flRealTime,
		SDK::Interfaces::GlobalVars->m_flFrameTime,
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime,
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount,

		// backup frame count and tick count
		SDK::Interfaces::GlobalVars->m_iFrameCount,
		SDK::Interfaces::GlobalVars->m_iTickCount
	);

	// save player data
	std::tuple < int, int, int, int, int, bool > m_PlayerData = std::make_tuple
	(
		g_Globals->m_LocalPlayer->m_nLastSkipFramecount( ),
		g_Globals->m_LocalPlayer->m_fEffects( ),
		g_Globals->m_LocalPlayer->m_nClientEffects( ),
		g_Globals->m_LocalPlayer->m_nOcclusionFrame( ),
		g_Globals->m_LocalPlayer->m_nOcclusionMask( ),
		g_Globals->m_LocalPlayer->m_bJiggleBones( )
	);

	// backup animation layers
	std::array < C_AnimationLayer, ANIMATION_LAYER_COUNT > m_Layers;
	std::memcpy( m_Layers.data( ), g_Globals->m_LocalPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
	
	/* set owners */
	for ( int nLayer = 0; nLayer < ANIMATION_LAYER_COUNT; nLayer++ )
	{
		C_AnimationLayer* m_Layer = &g_Globals->m_LocalPlayer->GetAnimationLayers( )[ nLayer ];
		if ( !m_Layer )
			continue;

		m_Layer->m_pOwner = g_Globals->m_LocalPlayer;
	}

	// get simulation time
	float flSimulationTime = g_Utils->TicksToTime( g_Networking->GetServerTick( ) );

	// setup globals
	SDK::Interfaces::GlobalVars->m_flCurTime = flSimulationTime;
	SDK::Interfaces::GlobalVars->m_flRealTime = flSimulationTime;
	SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;
	SDK::Interfaces::GlobalVars->m_iTickCount = g_Networking->GetServerTick( );

	// fix skipanimframe ( part 1 )
	SDK::Interfaces::GlobalVars->m_iFrameCount = INT_MAX;

	// invalidate bone cache
	g_Globals->m_LocalPlayer->InvalidateBoneCache( );

	// disable ugly lean animation
	g_Globals->m_LocalPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_LEAN ].m_flWeight = 0.0f;

	// disable bones jiggle
	g_Globals->m_LocalPlayer->m_bJiggleBones( ) = false;

	// force client effects
	g_Globals->m_LocalPlayer->m_nClientEffects( ) |= 2; // disable ik

	// force effects to disable interp
	g_Globals->m_LocalPlayer->m_fEffects( ) |= EF_NOINTERP;

	// fix PVS occlusion
	g_Globals->m_LocalPlayer->m_nOcclusionFrame( ) = -1;
	g_Globals->m_LocalPlayer->m_nOcclusionMask( ) &= ~2;
	
	// fix skipanimframe ( part 2 )
	g_Globals->m_LocalPlayer->m_nLastSkipFramecount( ) = 0;

	// setup bones
	g_Globals->m_Animations.m_bSetupBones = true;
	g_Globals->m_LocalPlayer->SetupBones( aMatrix, MAXSTUDIOBONES, nMask, 0.0f );
	g_Globals->m_Animations.m_bSetupBones = false;

	// restore animation layers
	std::memcpy( g_Globals->m_LocalPlayer->GetAnimationLayers( ), m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	// restore player data
	g_Globals->m_LocalPlayer->m_nLastSkipFramecount( ) = std::get < 0 > ( m_PlayerData );
	g_Globals->m_LocalPlayer->m_fEffects( ) = std::get < 1 > ( m_PlayerData );
	g_Globals->m_LocalPlayer->m_nClientEffects( ) = std::get < 2 > ( m_PlayerData );
	g_Globals->m_LocalPlayer->m_nOcclusionFrame( ) = std::get < 3 > ( m_PlayerData );
	g_Globals->m_LocalPlayer->m_nOcclusionMask( ) = std::get < 4 > ( m_PlayerData );
	g_Globals->m_LocalPlayer->m_bJiggleBones( ) = std::get < 5 > ( m_PlayerData );

	// restore globals
	SDK::Interfaces::GlobalVars->m_flCurTime = std::get < 0 >( m_Globals );
	SDK::Interfaces::GlobalVars->m_flRealTime = std::get < 1 >( m_Globals );
	SDK::Interfaces::GlobalVars->m_flFrameTime = std::get < 2 >( m_Globals );
	SDK::Interfaces::GlobalVars->m_flAbsFrameTime = std::get < 3 >( m_Globals );
	SDK::Interfaces::GlobalVars->m_flInterpolationAmount = std::get < 4 >( m_Globals );

	// restore frame count and tick count
	SDK::Interfaces::GlobalVars->m_iFrameCount = std::get < 5 >( m_Globals );
	SDK::Interfaces::GlobalVars->m_iTickCount = std::get < 6 >( m_Globals );
}
void C_LocalAnimations::ModifyEyePosition( Vector& vecInputEyePos, matrix3x4_t* aMatrix )
{
	Vector vecHeadPos = Vector
	(
		aMatrix[ 8 ][ 0 ][ 3 ],
		aMatrix[ 8 ][ 1 ][ 3 ],
		aMatrix[ 8 ][ 2 ][ 3 ] + 1.7f
	);

	if ( vecHeadPos.z > vecInputEyePos.z )
		return;

	float flLerp = Math::RemapValClamped( abs( vecInputEyePos.z - vecHeadPos.z ),
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN,
			FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX,
			0.0f, 1.0f );

	vecInputEyePos.z = Math::Lerp( flLerp, vecInputEyePos.z, vecHeadPos.z );
}
void C_LocalAnimations::InterpolateMatricies( )
{
	// correct matrix
	g_LocalAnimations->TransformateMatricies( );

	// copy bones
	std::memcpy( g_Globals->m_LocalPlayer->m_CachedBoneData( ).Base( ), m_LocalData.m_Real.m_Matrix.data( ), sizeof( matrix3x4_t ) * g_Globals->m_LocalPlayer->m_CachedBoneData( ).Count( ) );
	std::memcpy( g_Globals->m_LocalPlayer->GetBoneAccessor( ).GetBoneArrayForWrite( ), m_LocalData.m_Real.m_Matrix.data( ), sizeof( matrix3x4_t ) * g_Globals->m_LocalPlayer->m_CachedBoneData( ).Count( ) );

	// SetupBones_AttachmentHelper
	return g_Globals->m_LocalPlayer->SetupBones_AttachmentHelper( );
}
void C_LocalAnimations::TransformateMatricies( )
{
	Vector vecOriginDelta = g_Globals->m_LocalPlayer->GetAbsOrigin( ) - m_LocalData.m_Real.m_vecMatrixOrigin;
	for ( auto& Matrix : m_LocalData.m_Real.m_Matrix )
	{
		Matrix[ 0 ][ 3 ] += vecOriginDelta.x;
		Matrix[ 1 ][ 3 ] += vecOriginDelta.y;
		Matrix[ 2 ][ 3 ] += vecOriginDelta.z;
	}

	for ( auto& Matrix : m_LocalData.m_Fake.m_Matrix )
	{
		Matrix[ 0 ][ 3 ] += vecOriginDelta.x;
		Matrix[ 1 ][ 3 ] += vecOriginDelta.y;
		Matrix[ 2 ][ 3 ] += vecOriginDelta.z;
	}

	m_LocalData.m_Real.m_vecMatrixOrigin = g_Globals->m_LocalPlayer->GetAbsOrigin( );
}
bool C_LocalAnimations::CopyCachedMatrix( matrix3x4_t* aInMatrix, int nBoneCount )
{
	std::memcpy( aInMatrix, m_LocalData.m_Real.m_Matrix.data( ), sizeof( matrix3x4_t ) * nBoneCount );
	return true;
}
void C_LocalAnimations::CleanSnapshots( )
{
	*( float* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0x9B24 ) = 1.0f;
	*( float* ) ( ( DWORD ) ( g_Globals->m_LocalPlayer ) + 0xCF74 ) = 1.0f;
}
void C_LocalAnimations::ResetData( )
{
	m_LocalData.m_nFlags = 0;
	m_LocalData.m_nSimulationTicks = 0;
	m_LocalData.m_flSpawnTime = 0.0f;
	m_LocalData.m_flYawDelta = 0.0f;
	m_LocalData.m_AnimRecords = { };
	m_LocalData.m_vecShootPosition = Vector( 0, 0, 0 );

	m_LocalData.m_Real.m_nMoveType = 0;
	m_LocalData.m_Real.m_nFlags = 0;
	m_LocalData.m_Real.m_Layers = { };
	m_LocalData.m_Real.m_PoseParameters = { };
	m_LocalData.m_Real.m_vecMatrixOrigin = Vector( 0, 0, 0 );
	m_LocalData.m_Real.m_Matrix = { };

	m_LocalData.m_Fake.m_nMoveType = 0;
	m_LocalData.m_Fake.m_nFlags = 0;
	m_LocalData.m_Fake.m_Layers = { };
	m_LocalData.m_Fake.m_CleanLayers = { };
	m_LocalData.m_Fake.m_PoseParameters = { };
	m_LocalData.m_Fake.m_vecMatrixOrigin = Vector( 0, 0, 0 );
	m_LocalData.m_Fake.m_Matrix = { };

	m_LocalData.m_Shoot.m_Matrix = { };
	m_LocalData.m_Shoot.m_Layers = { };
	m_LocalData.m_Shoot.m_PoseParameters = { };
}