#include "Animations.hpp"
#include "Resolver.hpp"
#include "LocalAnimations.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_PlayerAnimations::SimulatePlayerAnimations( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, PlayerData_t* m_PlayerData )
{
	/* Get animation state */
	C_CSGOPlayerAnimationState* m_AnimationState = pPlayer->GetAnimationState( );

	/* Setup data and get previous record */
	LagRecord_t* m_PrevRecord = g_PlayerAnimations->SetupData( pPlayer, m_LagRecord, m_PlayerData );

	/* Determine player's velocity */
	m_LagRecord->m_vecVelocity = g_PlayerAnimations->DeterminePlayerVelocity( pPlayer, m_LagRecord, m_PrevRecord, m_AnimationState );
	if ( m_LagRecord->m_bFirstAfterDormant )
		g_PlayerAnimations->HandleDormancyLeaving( pPlayer, m_LagRecord, m_AnimationState );

	/* Copy data from previous or current record depends on situation */
	g_PlayerAnimations->CopyRecordData( pPlayer, m_LagRecord, m_PrevRecord, m_AnimationState );

	/* fix simulation ticks for bots */
	if ( m_LagRecord->m_bIsFakePlayer )
		m_LagRecord->m_nSimulationTicks = 1;

	GameGlobals_t m_Globals;
	m_Globals.CaptureData( );

	/* Save player's data */
	std::tuple < Vector, Vector, Vector, int, int, float, float, float > m_Data = std::make_tuple
	(
		pPlayer->m_vecVelocity( ),
		pPlayer->m_vecAbsVelocity( ),
		pPlayer->GetAbsOrigin( ),
		pPlayer->m_fFlags( ),
		pPlayer->m_iEFlags( ),
		pPlayer->m_flDuckAmount( ),
		pPlayer->m_flLowerBodyYaw( ),
		pPlayer->m_flThirdpersonRecoil( )
	);
	
	/* Invalidate EFlags */
	pPlayer->m_iEFlags( ) &= ~( EFL_DIRTY_ABSVELOCITY | EFL_DIRTY_ABSTRANSFORM );

	/* Update collision bounds */
	g_PlayerAnimations->SetupCollision( pPlayer, m_LagRecord );

	/* Simulate legit player */
	if ( m_LagRecord->m_nSimulationTicks <= 1 || !m_PrevRecord )
	{
		/* Determine simulation tick */
		int nSimulationTick = g_Utils->TimeToTicks( m_LagRecord->m_flSimulationTime );
		
		/* Set global game's data */
		SDK::Interfaces::GlobalVars->m_flCurTime = m_LagRecord->m_flSimulationTime;
		SDK::Interfaces::GlobalVars->m_flRealTime = m_LagRecord->m_flSimulationTime;
		SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
		SDK::Interfaces::GlobalVars->m_iFrameCount = nSimulationTick;
		SDK::Interfaces::GlobalVars->m_iTickCount = nSimulationTick;
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;

		/* C_BaseEntity::SetAbsOrigin */
		pPlayer->SetAbsOrigin( m_LagRecord->m_vecOrigin );

		/* Set velociy */
		pPlayer->m_vecVelocity( ) = m_LagRecord->m_vecVelocity;
		pPlayer->m_vecAbsVelocity( ) = m_LagRecord->m_vecVelocity;

		/* Update animations */
		g_PlayerAnimations->UpdatePlayerAnimations( pPlayer, m_LagRecord, m_AnimationState );
	}
	else
	{
		/* Simulate player activity ( jump and land ) */
		g_PlayerAnimations->SimulatePlayerActivity( pPlayer, m_LagRecord, m_PrevRecord );

		/* Compute activity playback ( jump and land ) */
		m_LagRecord->m_flActivityPlayback = g_PlayerAnimations->ComputeActivityPlayback( pPlayer, m_LagRecord );

		/* Run player simulation */
		for ( int iSimulationTick = 1; iSimulationTick <= m_LagRecord->m_nSimulationTicks; iSimulationTick++ )
		{
			/* Determine simulation time and tick */
			float flSimulationTime = m_PrevRecord->m_flSimulationTime + g_Utils->TicksToTime( iSimulationTick );
			int iCurrentSimulationTick = g_Utils->TimeToTicks( flSimulationTime );

			/* Setup game's global data */
			SDK::Interfaces::GlobalVars->m_flCurTime = flSimulationTime;
			SDK::Interfaces::GlobalVars->m_flRealTime = flSimulationTime;
			SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
			SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
			SDK::Interfaces::GlobalVars->m_iFrameCount = iCurrentSimulationTick;
			SDK::Interfaces::GlobalVars->m_iTickCount = iCurrentSimulationTick;
			SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;

			/* Set player data */
			pPlayer->m_flDuckAmount( ) = Math::AnimLerp( m_PrevRecord->m_flDuckAmount, m_LagRecord->m_flDuckAmount, iSimulationTick, m_LagRecord->m_nSimulationTicks );
			pPlayer->m_flLowerBodyYaw( ) = m_PrevRecord->m_flLowerBodyYaw;
			pPlayer->m_angEyeAngles( ) = m_PrevRecord->m_angEyeAngles;

			/* Simulate origin */
			pPlayer->m_vecOrigin( ) = Math::AnimLerp( m_PrevRecord->m_vecOrigin, m_LagRecord->m_vecOrigin, iSimulationTick, m_LagRecord->m_nSimulationTicks );
			pPlayer->SetAbsOrigin( pPlayer->m_vecOrigin( ) );

			/* Activity simulation */
			if ( flSimulationTime < m_LagRecord->m_flSimulationTime )
			{
				/* Simulate shoot */
				if ( m_LagRecord->m_bDidShot )
				{
					if ( iCurrentSimulationTick < m_LagRecord->m_nShotTick )
						pPlayer->m_flThirdpersonRecoil( ) = m_PrevRecord->m_flThirdPersonRecoil;
					else
					{
						pPlayer->m_angEyeAngles( ) = m_LagRecord->m_angEyeAngles;
						pPlayer->m_flLowerBodyYaw( ) = m_LagRecord->m_flLowerBodyYaw;
						pPlayer->m_flThirdpersonRecoil( ) = m_LagRecord->m_flThirdPersonRecoil;
					}
				}

				if ( m_LagRecord->m_nActivityType != EPlayerActivity::NoActivity )
				{
					if ( iCurrentSimulationTick == m_LagRecord->m_nActivityTick )
					{
						/* Compute current layer */
						int nLayer = ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL;
						if ( m_LagRecord->m_nActivityType == EPlayerActivity::Land )
							nLayer = ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB;

						/* C_CSGOPlayerAnimationState::SetLayerSequence */
						pPlayer->GetAnimationLayers( )[ nLayer ].m_flCycle = 0.0f;
						pPlayer->GetAnimationLayers( )[ nLayer ].m_flWeight = 0.0f;
						pPlayer->GetAnimationLayers( )[ nLayer ].m_flPlaybackRate = m_LagRecord->m_flActivityPlayback;

						/* Force player's ground state */
						if ( m_LagRecord->m_nActivityType == EPlayerActivity::Jump )
							pPlayer->m_fFlags( ) &= ~FL_ONGROUND;
						else if ( m_LagRecord->m_nActivityType == EPlayerActivity::Land )
							pPlayer->m_fFlags( ) |= FL_ONGROUND;
					}
					else if ( iCurrentSimulationTick < m_LagRecord->m_nActivityTick )
					{
						/* Force player's ground state */
						if ( m_LagRecord->m_nActivityType == EPlayerActivity::Jump )
							pPlayer->m_fFlags( ) |= FL_ONGROUND;
						else if ( m_LagRecord->m_nActivityType == EPlayerActivity::Land )
							pPlayer->m_fFlags( ) &= ~FL_ONGROUND;
					}
				}
			}
			else /* Set the latest networked data for the latest simulation tick */
 			{
				pPlayer->m_fFlags( ) = m_LagRecord->m_nFlags;
				pPlayer->m_flDuckAmount( ) = m_LagRecord->m_flDuckAmount;
				pPlayer->m_flLowerBodyYaw( ) = m_LagRecord->m_flLowerBodyYaw;
				pPlayer->m_angEyeAngles( ) = m_LagRecord->m_angEyeAngles;
			}

			/* Set velocity */
			Vector& vecVelocity = pPlayer->m_vecVelocity( );
			vecVelocity.x = Math::AnimLerp( m_PrevRecord->m_vecVelocity.x, m_LagRecord->m_vecVelocity.x, iSimulationTick, m_LagRecord->m_nSimulationTicks );
			vecVelocity.y = Math::AnimLerp( m_PrevRecord->m_vecVelocity.y, m_LagRecord->m_vecVelocity.y, iSimulationTick, m_LagRecord->m_nSimulationTicks );
			pPlayer->m_vecAbsVelocity( ) = vecVelocity;

			/* Update animations */
			g_PlayerAnimations->UpdatePlayerAnimations( pPlayer, m_LagRecord, m_AnimationState );
		}
	}
	
	/* Reset animation layers */
	std::memcpy( pPlayer->GetAnimationLayers( ), m_LagRecord->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	/* Reset player's origin */
	pPlayer->SetAbsOrigin( std::get < 2 > ( m_Data ) );

	/* Save additional record data */
	m_LagRecord->m_angAbsAngles = QAngle( 0.0f, m_AnimationState->m_flFootYaw, 0.0f );
	{
		float flAimMatrixWidthRange = Math::Lerp( std::clamp( pPlayer->GetAnimationState( )->m_flSpeedAsPortionOfWalkTopSpeed, 0.0f, 1.0f ), 1.0f, Math::Lerp( pPlayer->GetAnimationState( )->m_flWalkToRunTransition, CSGO_ANIM_AIM_NARROW_WALK, CSGO_ANIM_AIM_NARROW_RUN ) ); //-V807
		if ( pPlayer->GetAnimationState( )->m_flAnimDuckAmount > 0 )
			flAimMatrixWidthRange = Math::Lerp( pPlayer->GetAnimationState( )->m_flAnimDuckAmount * std::clamp( pPlayer->GetAnimationState( )->m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f ), flAimMatrixWidthRange, CSGO_ANIM_AIM_NARROW_CROUCHMOVING );

		m_LagRecord->m_flDesyncDelta = flAimMatrixWidthRange * pPlayer->GetAnimationState( )->m_flAimYawMax;
	}

	/* Save record's poses */
	std::memcpy( m_LagRecord->m_PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * m_LagRecord->m_PoseParameters.size( ) );

	/* Setup bones on this record */
	{
		/* Setup visual matrix */
		g_PlayerAnimations->SetupPlayerMatrix( pPlayer, m_LagRecord, m_LagRecord->m_Matricies[ EBoneMatrix::Visual ].data( ), EMatrixFlags::Interpolated | EMatrixFlags::VisualAdjustment );
		std::memcpy( m_PlayerData->m_aCachedMatrix.data( ), m_LagRecord->m_Matricies[ EBoneMatrix::Visual ].data( ), sizeof( matrix3x4_t ) * MAXSTUDIOBONES );
		
		/* Setup rage matrix */
		if ( g_SettingsManager->B[ _S( "ragebot.enable" ) ] )
			g_PlayerAnimations->SetupPlayerMatrix( pPlayer, m_LagRecord, m_LagRecord->m_Matricies[ EBoneMatrix::Aimbot ].data( ), EMatrixFlags::BoneUsedByHitbox );

		/* Generate safe aimbot points */
		g_PlayerAnimations->GenerateSafePoints( pPlayer, m_LagRecord );
	}

	/* Restore player's data */
	pPlayer->m_vecVelocity( ) = std::get < 0 >( m_Data );
	pPlayer->m_vecAbsVelocity( ) = std::get < 1 >( m_Data );
	pPlayer->m_fFlags( ) = std::get < 3 >( m_Data );
	pPlayer->m_iEFlags( ) = std::get < 4 >( m_Data );
	pPlayer->m_flDuckAmount( ) = std::get < 5 >( m_Data );
	pPlayer->m_flLowerBodyYaw( ) = std::get < 6 >( m_Data );
	pPlayer->m_flThirdpersonRecoil( ) = std::get < 7 >( m_Data );

	m_Globals.AdjustData( );
}
float C_PlayerAnimations::ComputeActivityPlayback( C_BasePlayer* pPlayer, LagRecord_t* m_Record )
{
	/* Get animation layers */
	C_AnimationLayer* m_JumpingLayer = &m_Record->m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ];
	C_AnimationLayer* m_LandingLayer = &m_Record->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];

	/* Determine playback */
	float flActivityPlayback = 0.0f;
	switch ( m_Record->m_nActivityType )
	{
		case EPlayerActivity::Jump:
		{
			flActivityPlayback = pPlayer->GetLayerSequenceCycleRate( m_JumpingLayer, m_JumpingLayer->m_nSequence );
		}
		break;
		
		case EPlayerActivity::Land:
		{
			flActivityPlayback = pPlayer->GetLayerSequenceCycleRate( m_LandingLayer, m_LandingLayer->m_nSequence );
		}
		break;
	}

	return flActivityPlayback;
}
void C_PlayerAnimations::SimulatePlayerActivity( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord )
{
	/* Get animation layers */
	const C_AnimationLayer* m_JumpingLayer = &m_LagRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ];
	const C_AnimationLayer* m_LandingLayer = &m_LagRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];
	const C_AnimationLayer* m_PrevJumpingLayer = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ];
	const C_AnimationLayer* m_PrevLandingLayer = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];

	/* Detect jump/land, collect its data, rebuild time in air */
	const int nJumpingActivity = pPlayer->GetSequenceActivity( m_JumpingLayer->m_nSequence );
	const int nLandingActivity = pPlayer->GetSequenceActivity( m_LandingLayer->m_nSequence );
	
	/* Collect jump data */
	if ( nJumpingActivity == ACT_CSGO_JUMP )
	{
		/* check duration bounds */
		if ( m_JumpingLayer->m_flWeight > 0.0f && m_JumpingLayer->m_flPlaybackRate > 0.0f )
		{
			/* check cycle changed */
			if ( m_JumpingLayer->m_flCycle < m_PrevJumpingLayer->m_flCycle )
			{
				m_LagRecord->m_flDurationInAir = m_JumpingLayer->m_flCycle / m_JumpingLayer->m_flPlaybackRate;
				if ( m_LagRecord->m_flDurationInAir > 0.0f )
				{
					m_LagRecord->m_nActivityTick = g_Utils->TimeToTicks( m_LagRecord->m_flSimulationTime - m_LagRecord->m_flDurationInAir ) + 1;
					m_LagRecord->m_nActivityType = EPlayerActivity::Jump;
				}
			}
		}
	}

	/* Collect land data */
	if ( nLandingActivity == ACT_CSGO_LAND_LIGHT || nLandingActivity == ACT_CSGO_LAND_HEAVY )
	{
		/* weight changing everytime on activity switch in this layer */
		if ( m_LandingLayer->m_flWeight > 0.0f && m_PrevLandingLayer->m_flWeight <= 0.0f )
		{
			/* check cycle changed */
			if ( m_LandingLayer->m_flCycle > m_PrevLandingLayer->m_flCycle )
			{
				float flLandDuration = m_LandingLayer->m_flCycle / m_LandingLayer->m_flPlaybackRate;
				if ( flLandDuration > 0.0f )
				{
					m_LagRecord->m_nActivityTick = g_Utils->TimeToTicks( m_LagRecord->m_flSimulationTime - flLandDuration ) + 1;
					m_LagRecord->m_nActivityType = EPlayerActivity::Land;

					/* Determine duration in air */
					float flDurationInAir = ( m_JumpingLayer->m_flCycle - m_LandingLayer->m_flCycle );
					if ( flDurationInAir < 0.0f )
						flDurationInAir += 1.0f;

					/* Set time in air */
					m_LagRecord->m_flDurationInAir = flDurationInAir / m_JumpingLayer->m_flPlaybackRate;
				}
			}
		}
	}
}
void C_PlayerAnimations::HandleDormancyLeaving( C_BasePlayer* pPlayer, LagRecord_t* m_Record, C_CSGOPlayerAnimationState* m_AnimationState )
{
	/* Get animation layers */
	const C_AnimationLayer* m_JumpingLayer = &m_Record->m_Layers[ ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL ];
	const C_AnimationLayer* m_LandingLayer = &m_Record->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];

	/* Final m_flLastUpdateTime */
	float m_flLastUpdateTime = m_Record->m_flSimulationTime - SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

	/* Fix animation state timing */
	if ( m_Record->m_nFlags & FL_ONGROUND ) /* On ground */
	{
		/* Use information from landing */
		int nActivity = pPlayer->GetSequenceActivity( m_LandingLayer->m_nSequence );
		if ( nActivity == ACT_CSGO_LAND_HEAVY || nActivity == ACT_CSGO_LAND_LIGHT )
		{
			/* Compute land duration */
			float flLandDuration = m_LandingLayer->m_flCycle / m_LandingLayer->m_flPlaybackRate;

			/* Check landing time */
			float flLandingTime = m_Record->m_flSimulationTime - flLandDuration;
			if ( flLandingTime == m_flLastUpdateTime )
			{
				m_AnimationState->m_bOnGround = true;
				m_AnimationState->m_bLanding = true;
				m_AnimationState->m_flDuckAdditional = 0.0f;
			}
			else if ( flLandingTime - SDK::Interfaces::GlobalVars->m_flIntervalPerTick == m_flLastUpdateTime )
			{
				m_AnimationState->m_bOnGround = false;
				m_AnimationState->m_bLanding = false;
				m_AnimationState->m_flDuckAdditional = 0.0f;
			}

			/* Determine duration in air */
			float flDurationInAir = ( m_JumpingLayer->m_flCycle - m_LandingLayer->m_flCycle );
			if ( flDurationInAir < 0.0f )
				flDurationInAir += 1.0f;

			/* Set time in air */
			m_AnimationState->m_flDurationInAir = flDurationInAir / m_JumpingLayer->m_flPlaybackRate;

			/* Check bounds.*/
			/* There's two conditions to let this data be useful: */
			/* It's useful if player has landed after the latest client animation update */
			/* It's useful if player has landed before the previous tick */
			if ( flLandingTime < m_flLastUpdateTime && flLandingTime > m_AnimationState->m_flLastUpdateTime )
				m_flLastUpdateTime = flLandingTime;
		}
	}
	else /* In air */
	{
		/* Use information from jumping */
		int nActivity = pPlayer->GetSequenceActivity( m_JumpingLayer->m_nSequence );
		if ( nActivity == ACT_CSGO_JUMP )
		{
			/* Compute duration in air */
			float flDurationInAir = m_JumpingLayer->m_flCycle / m_JumpingLayer->m_flPlaybackRate;

			/* Check landing time */
			float flJumpingTime = m_Record->m_flSimulationTime - flDurationInAir;
			if ( flJumpingTime <= m_flLastUpdateTime )
				m_AnimationState->m_bOnGround = false;
			else if ( flJumpingTime - SDK::Interfaces::GlobalVars->m_flIntervalPerTick )
				m_AnimationState->m_bOnGround = true;

			/* Check bounds.*/
			/* There's two conditions to let this data be useful: */
			/* It's useful if player has jumped after the latest client animation update */
			/* It's useful if player has jumped before the previous tick */
			if ( flJumpingTime < m_flLastUpdateTime && flJumpingTime > m_AnimationState->m_flLastUpdateTime )
				m_flLastUpdateTime = flJumpingTime;

			/* Set time in air */
			m_AnimationState->m_flDurationInAir = flDurationInAir - SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

			/* Disable landing */
			m_AnimationState->m_bLanding = false;
		}
	}

	/* Set m_flLastUpdateTime */
	m_AnimationState->m_flLastUpdateTime = m_flLastUpdateTime;
}
void C_PlayerAnimations::CopyRecordData( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord, C_CSGOPlayerAnimationState* m_AnimationState )
{
	LagRecord_t* m_ForceRecord = m_LagRecord;
	if ( m_PrevRecord )
		m_ForceRecord = m_PrevRecord;

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	C_AnimationLayer* m_StrafeLayer = &m_ForceRecord->m_Layers[ 7 ];
	m_AnimationState->m_flStrafeChangeCycle = m_StrafeLayer->m_flCycle;
	m_AnimationState->m_flStrafeChangeWeight = m_StrafeLayer->m_flWeight;
	m_AnimationState->m_nStrafeSequence = m_StrafeLayer->m_nSequence;
	m_AnimationState->m_flPrimaryCycle = m_ForceRecord->m_Layers[ 6 ].m_flCycle;
	m_AnimationState->m_flMoveWeight = m_ForceRecord->m_Layers[ 6 ].m_flWeight;

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif

	m_AnimationState->m_flAccelerationWeight = m_ForceRecord->m_Layers[ 12 ].m_flWeight;
	std::memcpy( pPlayer->GetAnimationLayers( ), m_ForceRecord->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );
}
Vector C_PlayerAnimations::DeterminePlayerVelocity( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord, C_CSGOPlayerAnimationState* m_AnimationState )
{
	/* Prepare data once */
	if ( !m_PrevRecord )
	{
		const float flVelLength = m_LagRecord->m_vecVelocity.Length( );
		if ( flVelLength > m_LagRecord->m_flMaxSpeed )
			m_LagRecord->m_vecVelocity *= m_LagRecord->m_flMaxSpeed / flVelLength;
		
		return m_LagRecord->m_vecVelocity;
	}

	/* Define const */
	const float flMaxSpeed = SDK::EngineData::m_ConvarList[ CheatConvarList::MaxSpeed ]->GetFloat( );

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	/* Get animation layers */
	const C_AnimationLayer* m_AliveLoop = &m_LagRecord->m_Layers[ ANIMATION_LAYER_ALIVELOOP ];
	const C_AnimationLayer* m_PrevAliveLoop = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_ALIVELOOP ];
#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	const C_AnimationLayer* m_Movement = &m_LagRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_MOVE ];
	const C_AnimationLayer* m_PrevMovement = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_MOVE ];
	const C_AnimationLayer* m_Landing = &m_LagRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];
	const C_AnimationLayer* m_PrevLanding = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB ];

	/* Recalculate velocity using origin delta */
	m_LagRecord->m_vecVelocity = ( m_LagRecord->m_vecOrigin - m_PrevRecord->m_vecOrigin ) * ( 1.0f / g_Utils->TicksToTime( m_LagRecord->m_nSimulationTicks ) );

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	/* Check PlaybackRate */
	if ( m_Movement->m_flPlaybackRate < 0.00001f )
		m_LagRecord->m_vecVelocity.x = m_LagRecord->m_vecVelocity.y = 0.0f;
	else
	{
		/* Compute velocity using m_flSpeedAsPortionOfRunTopSpeed */
		float flWeight = m_AliveLoop->m_flWeight;
		if ( flWeight < 1.0f )
		{
#ifndef OVERSEE_DEV
			BreakIDA( );
#endif
			/* Check PlaybackRate */
			if ( m_AliveLoop->m_flPlaybackRate == m_PrevAliveLoop->m_flPlaybackRate )
			{
				/* Check Sequence */
				if ( m_AliveLoop->m_nSequence == m_PrevAliveLoop->m_nSequence )
				{
#ifndef OVERSEE_DEV
					BreakIDA( );
#endif
					/* Very important cycle check */
					if ( m_AliveLoop->m_flCycle > m_PrevAliveLoop->m_flCycle )
					{
						/* Check weapon */
						if ( m_AnimationState->m_pWeapon == pPlayer->m_hActiveWeapon( ) )
						{
							/* Get m_flSpeedAsPortionOfRunTopSpeed */
							float m_flSpeedAsPortionOfRunTopSpeed = ( ( 1.0f - flWeight ) / 2.8571432f ) + 0.55f;

#ifndef OVERSEE_DEV
							BreakIDA( );
#endif
							/* Check m_flSpeedAsPortionOfRunTopSpeed bounds ( from 55% to 90% from the speed ) */
							if ( m_flSpeedAsPortionOfRunTopSpeed > 0.55f && m_flSpeedAsPortionOfRunTopSpeed < 0.9f )
							{
#ifndef OVERSEE_DEV
								BreakIDA( );
#endif
								/* Compute velocity */
								m_LagRecord->m_flAnimationVelocity = m_flSpeedAsPortionOfRunTopSpeed * m_LagRecord->m_flMaxSpeed;
								m_LagRecord->m_nVelocityMode = EFixedVelocity::AliveLoopLayer;
							}
							else if ( m_flSpeedAsPortionOfRunTopSpeed > 0.9f )
							{
#ifndef OVERSEE_DEV
								BreakIDA( );
#endif
								/* Compute velocity */
								m_LagRecord->m_flAnimationVelocity = m_LagRecord->m_vecVelocity.Length2D( );
							}
						}
					}
				}
			}
		}

		/* Compute velocity using Movement ( 6 ) weight  */
		if ( m_LagRecord->m_flAnimationVelocity <= 0.0f )
		{
#ifndef OVERSEE_DEV
			BreakIDA( );
#endif
			/* Check Weight bounds from 10% to 90% from the speed */
			float flWeight = m_Movement->m_flWeight;
			if ( flWeight > 0.1f && flWeight < 0.9f )
			{
				/* Skip on land */
				if ( m_Landing->m_flWeight <= 0.0f )
				{
#ifndef OVERSEE_DEV
					BreakIDA( );
#endif
					/* Check Accelerate */
					if ( flWeight > m_PrevMovement->m_flWeight )
					{
						/* Skip on direction switch */
						if ( m_LagRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ].m_nSequence == m_PrevRecord->m_Layers[ ANIMATION_LAYER_MOVEMENT_STRAFECHANGE ].m_nSequence )
						{
#ifndef OVERSEE_DEV
							BreakIDA( );
#endif
							/* Check move sequence */
							if ( m_Movement->m_nSequence == m_PrevMovement->m_nSequence )
							{
								/* Check land sequence */
								if ( m_Landing->m_nSequence == m_PrevLanding->m_nSequence )
								{
#ifndef OVERSEE_DEV
									BreakIDA( );
#endif
									/* Check stand sequence */
									if ( m_LagRecord->m_Layers[ ANIMATION_LAYER_ADJUST ].m_nSequence == m_PrevRecord->m_Layers[ ANIMATION_LAYER_ADJUST ].m_nSequence )
									{
										/* Check Flags */
										if ( m_LagRecord->m_nFlags & FL_ONGROUND )
										{
#ifndef OVERSEE_DEV
											BreakIDA( );
#endif
											/* Compute MaxSpeed modifier */
											float flSpeedModifier = 1.0f;
											if ( m_LagRecord->m_nFlags & FL_DUCKING )
												flSpeedModifier = CS_PLAYER_SPEED_DUCK_MODIFIER;
											else if ( m_LagRecord->m_bIsWalking )
												flSpeedModifier = CS_PLAYER_SPEED_WALK_MODIFIER;

#ifndef OVERSEE_DEV
											BreakIDA( );
#endif
											/* Compute Velocity ( THIS CODE ONLY WORKS IN DUCK AND WALK ) */
											if ( flSpeedModifier < 1.0f )
											{
												m_LagRecord->m_flAnimationVelocity = ( flWeight * ( m_LagRecord->m_flMaxSpeed * flSpeedModifier ) );
												m_LagRecord->m_nVelocityMode = EFixedVelocity::MovementLayer;
											}
										}
									}
								}
							}
						}
					}
#ifndef OVERSEE_DEV
					BreakIDA( );
#endif
				}
			}
		}
	}

	/* Compute velocity from m_Record->m_flAnimationVelocity floating point */
	if ( m_LagRecord->m_flAnimationVelocity > 0.0f )
	{
		const float flModifier = m_LagRecord->m_flAnimationVelocity / m_LagRecord->m_vecVelocity.Length2D( );
		m_LagRecord->m_vecVelocity.x *= flModifier;
#ifndef OVERSEE_DEV
		BreakIDA( );
#endif
		m_LagRecord->m_vecVelocity.y *= flModifier;
	}

	/* Prepare data once */
	const float flVelLength = m_LagRecord->m_vecVelocity.Length( );

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	/* Clamp velocity if its out bounds */
	if ( flVelLength > m_LagRecord->m_flMaxSpeed )
		m_LagRecord->m_vecVelocity *= m_LagRecord->m_flMaxSpeed / flVelLength;

	return m_LagRecord->m_vecVelocity;
}
LagRecord_t* C_PlayerAnimations::SetupData( C_BasePlayer* pPlayer, LagRecord_t* m_Record, PlayerData_t* m_PlayerData )
{
	/* No previous record on spawn or left dormancy */
	if ( m_Record->m_bFirstAfterDormant )
		return nullptr;

	/* Get player's previous simulated record */
	LagRecord_t m_PrevRecord = m_PlayerData->m_LagRecord;
	if ( m_Record->m_bIsFakePlayer )
		return &m_PrevRecord;

	/* Determine simulation ticks with anim cycle */
	m_Record->m_nSimulationTicks = g_PlayerAnimations->DetermineAnimationCycle( pPlayer, m_Record, &m_PrevRecord );
	if ( g_Utils->TimeToTicks( m_Record->m_flSimulationTime - m_PrevRecord.m_flSimulationTime ) > MAX_SIMULATION_TICKS )
		return nullptr;

	/* return previous record */
	return &m_PrevRecord;
}
int C_PlayerAnimations::DetermineAnimationCycle( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord )
{
	/* Get animation layers */
	const C_AnimationLayer* m_AliveLoop = &m_LagRecord->m_Layers[ ANIMATION_LAYER_ALIVELOOP ];
	const C_AnimationLayer* m_PrevAliveLoop = &m_PrevRecord->m_Layers[ ANIMATION_LAYER_ALIVELOOP ];
	
	/* Get ticks animated on the server ( by default it's simtime delta ) */
	int nTicksAnimated = m_LagRecord->m_nSimulationTicks;
	if ( m_AliveLoop->m_flPlaybackRate == m_PrevAliveLoop->m_flPlaybackRate )
		nTicksAnimated = ( m_AliveLoop->m_flCycle - m_PrevAliveLoop->m_flCycle ) / ( m_AliveLoop->m_flPlaybackRate * SDK::Interfaces::GlobalVars->m_flIntervalPerTick );
	else
		nTicksAnimated = ( ( ( ( m_AliveLoop->m_flCycle / m_AliveLoop->m_flPlaybackRate ) + ( ( 1.0f - m_PrevAliveLoop->m_flCycle ) / m_PrevAliveLoop->m_flPlaybackRate ) ) / SDK::Interfaces::GlobalVars->m_flIntervalPerTick ) );

	return min( max( nTicksAnimated, m_LagRecord->m_nSimulationTicks ), MAX_SIMULATION_TICKS );
}
void C_PlayerAnimations::SetupCollision( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord )
{
	ICollideable* m_Collideable = pPlayer->GetCollideable( );
	if ( !m_Collideable )
		return;

	pPlayer->UpdateCollisionBounds( );
	m_LagRecord->m_vecMins = m_Collideable->OBBMins( );
	m_LagRecord->m_vecMaxs = m_Collideable->OBBMaxs( );
}
void C_PlayerAnimations::GenerateSafePoints( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord )
{
	auto GetYawRotation = [ & ] ( int nRotationSide ) -> float
	{
		float flOldEyeYaw = m_LagRecord->m_flEyeYaw;

#ifndef OVERSEE_DEV
		BreakIDA( );
#endif
		// set eye yaw
		float flEyeRotation = m_LagRecord->m_angEyeAngles.yaw;
		switch ( nRotationSide )
		{
			case -1: m_LagRecord->m_flEyeYaw = Math::NormalizeAngle( flEyeRotation - 60.0f ); break;
			case 0: m_LagRecord->m_flEyeYaw = Math::NormalizeAngle( flEyeRotation ); break;
			case 1: m_LagRecord->m_flEyeYaw = Math::NormalizeAngle( flEyeRotation + 60.0f ); break;
		}

		// generate foot yaw
		float flFootYaw = g_PlayerAnimations->BuildFootYaw( pPlayer, m_LagRecord );

		// restore eye yaw                                   
		m_LagRecord->m_flEyeYaw = flOldEyeYaw;

		// return result
		return flFootYaw;
	};

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	// point building func
	auto BuildSafePoint = [ & ] ( int nRotationSide )
	{
		// save animation data
		std::array < C_AnimationLayer, 13 > m_Layers;
		std::array < float, 24 > m_PoseParameters;
		C_CSGOPlayerAnimationState m_AnimationState;

#ifndef OVERSEE_DEV
		BreakIDA( );
#endif
		// copy data
		std::memcpy( m_Layers.data( ), pPlayer->GetAnimationLayers( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( m_PoseParameters.data( ), pPlayer->m_aPoseParameters( ).data( ), sizeof( float ) * 24 );
		std::memcpy( &m_AnimationState, pPlayer->GetAnimationState( ), sizeof( C_CSGOPlayerAnimationState ) );

		// set foot yaw
		pPlayer->GetAnimationState( )->m_flFootYaw = GetYawRotation( nRotationSide );

		// update player animations
		g_PlayerAnimations->UpdatePlayerAnimations( pPlayer, m_LagRecord, pPlayer->GetAnimationState( ) );

		// get matrix
		matrix3x4_t* aMatrix = nullptr;
		switch ( nRotationSide )
		{
			case -1: aMatrix = m_LagRecord->m_Matricies[ 1 ].data( ); break;
			case 0: aMatrix = m_LagRecord->m_Matricies[ 2 ].data( ); break;
			case 1: aMatrix = m_LagRecord->m_Matricies[ 3 ].data( ); break;
		}

		// setup bones
		g_PlayerAnimations->SetupPlayerMatrix( pPlayer, m_LagRecord, aMatrix, EMatrixFlags::BoneUsedByHitbox );

		// restore data
		std::memcpy( pPlayer->GetAnimationLayers( ), m_Layers.data( ), sizeof( C_AnimationLayer ) * 13 );
		std::memcpy( pPlayer->m_aPoseParameters( ).data( ), m_PoseParameters.data( ), sizeof( float ) * 24 );
		std::memcpy( pPlayer->GetAnimationState( ), &m_AnimationState, sizeof( C_CSGOPlayerAnimationState ) );
	};

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	// check conditions
	if ( !g_Globals->m_LocalPlayer->IsAlive( ) || !g_SettingsManager->B[ _S( "ragebot.enable" ) ] )
		return;

	// build safe points
	BuildSafePoint( -1 );
#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	BuildSafePoint( 0 );
#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	BuildSafePoint( 1 );
}
float C_PlayerAnimations::BuildFootYaw( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord )
{
	C_CSGOPlayerAnimationState* m_AnimationState = pPlayer->GetAnimationState( );
	if ( !m_AnimationState )
		return 0.0f;

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	float flAimMatrixWidthRange = Math::Lerp( std::clamp( m_AnimationState->m_flSpeedAsPortionOfWalkTopSpeed, 0.f, 1.f ), 1.0f, Math::Lerp( m_AnimationState->m_flWalkToRunTransition, CSGO_ANIM_AIM_NARROW_WALK, CSGO_ANIM_AIM_NARROW_RUN ) );
	if ( m_AnimationState->m_flAnimDuckAmount > 0 )
		flAimMatrixWidthRange = Math::Lerp( m_AnimationState->m_flAnimDuckAmount * std::clamp( m_AnimationState->m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f ), flAimMatrixWidthRange, CSGO_ANIM_AIM_NARROW_CROUCHMOVING );

	float flTempYawMax = m_AnimationState->m_flAimYawMax * flAimMatrixWidthRange;
	float flTempYawMin = m_AnimationState->m_flAimYawMin * flAimMatrixWidthRange;

	float flFootYaw = Math::NormalizeAngle( m_LagRecord->m_flEyeYaw );

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	float flEyeFootDelta = Math::AngleDiff( m_LagRecord->m_angEyeAngles.yaw, flFootYaw );
	if ( flEyeFootDelta > flTempYawMax )
		flFootYaw = m_LagRecord->m_angEyeAngles.yaw - fabs( flTempYawMax );
	else if ( flEyeFootDelta < flTempYawMin )
		flFootYaw = m_LagRecord->m_angEyeAngles.yaw + fabs( flTempYawMin );

	if ( m_AnimationState->m_bOnGround )
	{
#ifndef OVERSEE_DEV
		BreakIDA( );
#endif
		if ( m_AnimationState->m_flVelocityLengthXY > 0.1f || m_AnimationState->m_flVelocityLengthZ > 100.0f )
			flFootYaw = Math::ApproachAngle( m_LagRecord->m_flEyeYaw, flFootYaw, SDK::Interfaces::GlobalVars->m_flIntervalPerTick * ( 30.0f + ( 20.0f * m_AnimationState->m_flWalkToRunTransition ) ) );
		else
			flFootYaw = Math::ApproachAngle( m_LagRecord->m_flLowerBodyYaw, flFootYaw, SDK::Interfaces::GlobalVars->m_flIntervalPerTick * CSGO_ANIM_LOWER_CATCHUP_IDLE );
	}

	return Math::NormalizeAngle( flFootYaw );
}
void C_PlayerAnimations::UpdatePlayerAnimations( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, C_CSGOPlayerAnimationState* m_AnimationState )
{
#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	
	/* Bypass this check https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/cstrike15/csgo_playeranimstate.cpp#L266 */
	m_AnimationState->m_nLastUpdateFrame = 0;
	if ( m_AnimationState->m_flLastUpdateTime == SDK::Interfaces::GlobalVars->m_flCurTime )
		m_AnimationState->m_flLastUpdateTime = SDK::Interfaces::GlobalVars->m_flCurTime + SDK::Interfaces::GlobalVars->m_flIntervalPerTick;

	/* Force animation state player and previous weapon */
	m_AnimationState->m_pBasePlayer = pPlayer;
	m_AnimationState->m_pWeaponLast = pPlayer->m_hActiveWeapon( );

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	/* Force the owner of animation layers */
	for ( int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++ )
	{
		C_AnimationLayer* m_Layer = &pPlayer->GetAnimationLayers( )[ iLayer ];
		if ( !m_Layer )
			continue;

		m_Layer->m_pOwner = pPlayer;
	}

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif

	bool bClientSideAnimation = pPlayer->m_bClientSideAnimation( );
	pPlayer->m_bClientSideAnimation( ) = true;

	g_Globals->m_Animations.m_bUpdateAnims = true;
#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	pPlayer->UpdateClientSideAnimation( );
	g_Globals->m_Animations.m_bUpdateAnims = false;

#ifndef OVERSEE_DEV
	BreakIDA( );
#endif
	pPlayer->m_bClientSideAnimation( ) = bClientSideAnimation;
}
void C_PlayerAnimations::InterpolateMatricies( )
{
	g_LocalAnimations->InterpolateMatricies( );
	for ( int nPlayerID = 1; nPlayerID <= 64; nPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || pPlayer->IsFriend( ) || pPlayer == g_Globals->m_LocalPlayer || pPlayer->IsDormant( ) || !pPlayer->IsAlive( ) )
			continue;

		PlayerData_t* m_PlayerData = &g_LagCompensation->GetPlayerData( pPlayer );
		if ( !m_PlayerData )
			continue;

		// get bone count
		int nBoneCount = pPlayer->m_CachedBoneData( ).Count( );
		if ( nBoneCount > MAXSTUDIOBONES )
			nBoneCount = MAXSTUDIOBONES;

		// re-pos matrix
		g_PlayerAnimations->TransformateMatrix( pPlayer, m_PlayerData );

		// copy the entire matrix
		std::memcpy( pPlayer->m_CachedBoneData( ).Base( ), m_PlayerData->m_aCachedMatrix.data( ), sizeof( matrix3x4_t ) * nBoneCount );

		// build attachments
		std::memcpy( pPlayer->GetBoneAccessor( ).GetBoneArrayForWrite( ), m_PlayerData->m_aCachedMatrix.data( ), sizeof( matrix3x4_t ) * nBoneCount );
		pPlayer->SetupBones_AttachmentHelper( );
		std::memcpy( pPlayer->GetBoneAccessor( ).GetBoneArrayForWrite( ), m_PlayerData->m_aCachedMatrix.data( ), sizeof( matrix3x4_t ) * nBoneCount );
	}
}
void C_PlayerAnimations::TransformateMatrix( C_BasePlayer* pPlayer, PlayerData_t* m_PlayerData )
{
	Vector vecOriginDelta = pPlayer->GetAbsOrigin( ) - m_PlayerData->m_vecLastOrigin;
	for ( auto& Matrix : m_PlayerData->m_aCachedMatrix )
	{
		Matrix[ 0 ][ 3 ] += vecOriginDelta.x;
		Matrix[ 1 ][ 3 ] += vecOriginDelta.y;
		Matrix[ 2 ][ 3 ] += vecOriginDelta.z;
	}
	m_PlayerData->m_vecLastOrigin = pPlayer->GetAbsOrigin( );
}
bool C_PlayerAnimations::CopyCachedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, int nBoneCount )
{
	PlayerData_t* m_PlayerData = &g_LagCompensation->GetPlayerData( pPlayer );

	std::memcpy( aMatrix, m_PlayerData->m_aCachedMatrix.data( ), sizeof( matrix3x4_t ) * nBoneCount );
	return true;
}
void C_PlayerAnimations::SetupPlayerMatrix( C_BasePlayer* pPlayer, LagRecord_t* m_Record, matrix3x4_t* Matrix, int nFlags )
{
	/* Reset layers */
	std::memcpy( pPlayer->GetAnimationLayers( ), m_Record->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	/* Store game's globals */
	GameGlobals_t Globals;
	Globals.CaptureData( );

	/* Store player's data */
	std::tuple < int, int, int, int, int, bool, Vector > m_PlayerData = std::make_tuple
	(
		pPlayer->m_nLastSkipFramecount( ),
		pPlayer->m_fEffects( ),
		pPlayer->m_nClientEffects( ),
		pPlayer->m_nOcclusionFrame( ),
		pPlayer->m_nOcclusionMask( ),
		pPlayer->m_bJiggleBones( ),
		pPlayer->GetAbsOrigin( )
	);

	/* Force game's globals */
	int nSimulationTick = g_Utils->TimeToTicks( m_Record->m_flSimulationTime );
	SDK::Interfaces::GlobalVars->m_flCurTime = m_Record->m_flSimulationTime;
	SDK::Interfaces::GlobalVars->m_flRealTime = m_Record->m_flSimulationTime;
	SDK::Interfaces::GlobalVars->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	SDK::Interfaces::GlobalVars->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flIntervalPerTick;
	SDK::Interfaces::GlobalVars->m_iTickCount = nSimulationTick;
	SDK::Interfaces::GlobalVars->m_iFrameCount = INT_MAX; /* ShouldSkipAnimationFrame fix */
	SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;

	/* Force it https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/c_baseanimating.cpp#L3102 */
	pPlayer->InvalidateBoneCache( );

	/* Force the owner of animation layers */
	for ( int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++ )
	{
		C_AnimationLayer* m_Layer = &pPlayer->GetAnimationLayers( )[ iLayer ];
		if ( !m_Layer )
			continue;

		m_Layer->m_pOwner = pPlayer;
	}

	/* Disable ACT_CSGO_IDLE_TURN_BALANCEADJUST animation */
	if ( nFlags & EMatrixFlags::VisualAdjustment )
	{
		pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_LEAN ].m_flWeight = 0.0f;
		if ( pPlayer->GetSequenceActivity( pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_nSequence ) == ACT_CSGO_IDLE_TURN_BALANCEADJUST )
		{
			pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_flCycle = 0.0f;
			pPlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ADJUST ].m_flWeight = 0.0f;
		}
	}

	/* Remove interpolation if required */
	if ( !( nFlags & EMatrixFlags::Interpolated ) )
		pPlayer->SetAbsOrigin( m_Record->m_vecOrigin );

	/* Compute bone mask */
	int nBoneMask = BONE_USED_BY_ANYTHING;
	if ( nFlags & EMatrixFlags::BoneUsedByHitbox )
		nBoneMask = BONE_USED_BY_HITBOX;

	/* Fix player's data */
	pPlayer->m_bJiggleBones( ) = false;
	pPlayer->m_nClientEffects( ) |= 2;
	pPlayer->m_fEffects( ) |= EF_NOINTERP;
	pPlayer->m_nOcclusionFrame( ) = -1;
	pPlayer->m_nOcclusionMask( ) &= ~2;
	pPlayer->m_nLastSkipFramecount( ) = 0;

	/* Setup bones */
	g_PlayerAnimations->SetupBones( pPlayer, nBoneMask, Matrix );
	
	/* Restore player's data */
	pPlayer->m_nLastSkipFramecount( ) = std::get < 0 >( m_PlayerData );
	pPlayer->m_fEffects( ) = std::get < 1 >( m_PlayerData );
	pPlayer->m_nClientEffects( ) = std::get < 2 >( m_PlayerData );
	pPlayer->m_nOcclusionFrame( ) = std::get < 3 >( m_PlayerData );
	pPlayer->m_nOcclusionMask( ) = std::get < 4 >( m_PlayerData );
	pPlayer->m_bJiggleBones( ) = std::get < 5 >( m_PlayerData );
	pPlayer->SetAbsOrigin( std::get < 6 >( m_PlayerData ) );

	/* Reset layers */
	std::memcpy( pPlayer->GetAnimationLayers( ), m_Record->m_Layers.data( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	/* Restore game's globals */
	Globals.AdjustData( );
}
void C_PlayerAnimations::SetupBones( C_BasePlayer* pPlayer, int nBoneMask, matrix3x4_t* aMatrix )
{
	/* rebuild setup bones later */
	g_Globals->m_Animations.m_bSetupBones = true;
	pPlayer->SetupBones( aMatrix, MAXSTUDIOBONES, nBoneMask, 0.0f );
	g_Globals->m_Animations.m_bSetupBones = false;
}

void C_CSGOPlayerAnimationState::IncrementLayerCycle( C_AnimationLayer* Layer, bool bIsLoop )
{
	float_t flNewCycle = ( Layer->m_flPlaybackRate * this->m_flLastUpdateIncrement ) + Layer->m_flCycle;
	if ( !bIsLoop && flNewCycle >= 1.0f )
		flNewCycle = 0.999f;

	flNewCycle -= ( int32_t ) ( flNewCycle );
	if ( flNewCycle < 0.0f )
		flNewCycle += 1.0f;

	if ( flNewCycle > 1.0f )
		flNewCycle -= 1.0f;

	Layer->m_flCycle = flNewCycle;
}
bool C_CSGOPlayerAnimationState::IsLayerSequenceFinished( C_AnimationLayer* Layer, float_t flTime )
{
	return ( Layer->m_flPlaybackRate * flTime ) + Layer->m_flCycle >= 1.0f;
}
void C_CSGOPlayerAnimationState::SetLayerCycle( C_AnimationLayer* pAnimationLayer, float_t flCycle )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flCycle = flCycle;
}
void C_CSGOPlayerAnimationState::SetLayerRate( C_AnimationLayer* pAnimationLayer, float_t flRate )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flPlaybackRate = flRate;
}
void C_CSGOPlayerAnimationState::SetLayerWeight( C_AnimationLayer* pAnimationLayer, float_t flWeight )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flWeight = flWeight;
}
void C_CSGOPlayerAnimationState::SetLayerWeightRate( C_AnimationLayer* pAnimationLayer, float_t flPrevious )
{
	if ( pAnimationLayer )
		pAnimationLayer->m_flWeightDeltaRate = ( pAnimationLayer->m_flWeight - flPrevious ) / m_flLastUpdateIncrement;
}
void C_CSGOPlayerAnimationState::SetLayerSequence( C_AnimationLayer* pAnimationLayer, int iActivity )
{
	int iSequence = this->SelectSequenceFromActivityModifier( iActivity );
	if ( iSequence < 2 )
		return;

	pAnimationLayer->m_flCycle = 0.0f;
	pAnimationLayer->m_flWeight = 0.0f;
	pAnimationLayer->m_nSequence = iSequence;
	pAnimationLayer->m_flPlaybackRate = m_pBasePlayer->GetLayerSequenceCycleRate( pAnimationLayer, iSequence );
}
int C_CSGOPlayerAnimationState::SelectSequenceFromActivityModifier( int iActivity )
{
	bool bIsPlayerDucked = m_flAnimDuckAmount > 0.55f;
	bool bIsPlayerRunning = m_flSpeedAsPortionOfWalkTopSpeed > 0.25f;

	int iLayerSequence = -1;
	switch ( iActivity )
	{
		case ACT_CSGO_JUMP:
		{
			iLayerSequence = 15 + static_cast < int >( bIsPlayerRunning );
			if ( bIsPlayerDucked )
				iLayerSequence = 17 + static_cast < int >( bIsPlayerRunning );
		}
		break;

		case ACT_CSGO_ALIVE_LOOP:
		{
			iLayerSequence = 9;
			if ( m_pWeaponLast != m_pWeapon )
				iLayerSequence = 8;
		}
		break;

		case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		{
			iLayerSequence = 6;
		}
		break;

		case ACT_CSGO_FALL:
		{
			iLayerSequence = 14;
		}
		break;

		case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		{
			iLayerSequence = 4;
		}
		break;

		case ACT_CSGO_LAND_LIGHT:
		{
			iLayerSequence = 20;
			if ( bIsPlayerRunning )
				iLayerSequence = 22;

			if ( bIsPlayerDucked )
			{
				iLayerSequence = 21;
				if ( bIsPlayerRunning )
					iLayerSequence = 19;
			}
		}
		break;

		case ACT_CSGO_LAND_HEAVY:
		{
			iLayerSequence = 23;
			if ( bIsPlayerDucked )
				iLayerSequence = 24;
		}
		break;

		case ACT_CSGO_CLIMB_LADDER:
		{
			iLayerSequence = 13;
		}
		break;
		default: break;
	}

	return iLayerSequence;
}
void C_CSGOPlayerAnimationState::Reset( )
{
	m_pWeapon = this->m_pBasePlayer->m_hActiveWeapon( );
	m_pWeaponLast = m_pWeapon;
	m_pWeaponLastBoneSetup = m_pWeapon;
	m_flEyePositionSmoothLerp = 0;
	m_flStrafeChangeWeightSmoothFalloff = 0;
	m_bFirstFootPlantSinceInit = true;
	m_flLastUpdateTime = 0;
	m_flLastUpdateIncrement = 0;
	m_flEyeYaw = 0;
	m_flEyePitch = 0;
	m_flFootYaw = 0;
	m_flFootYawLast = 0;
	m_flMoveYaw = 0;
	m_flMoveYawIdeal = 0;
	m_flMoveYawCurrentToIdeal = 0;
	m_tStandWalkAim.Init( );
	m_tStandWalkAim.m_flHowLongToWaitUntilTransitionCanBlendIn = 0.4f;
	m_tStandWalkAim.m_flHowLongToWaitUntilTransitionCanBlendOut = 0.2f;
	m_tStandRunAim.Init( );
	m_tStandRunAim.m_flHowLongToWaitUntilTransitionCanBlendIn = 0.2f;
	m_tStandRunAim.m_flHowLongToWaitUntilTransitionCanBlendOut = 0.4f;
	m_tCrouchWalkAim.Init( );
	m_tCrouchWalkAim.m_flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
	m_tCrouchWalkAim.m_flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
	m_flPrimaryCycle = 0;
	m_flMoveWeight = 0;
	m_flMoveWeightSmoothed = 0;
	m_flAnimDuckAmount = 0;
	m_flDuckAdditional = 0; // for when we duck a bit after landing from a jump
	m_flRecrouchWeight = 0;
	m_vecPositionCurrent.Init( );
	m_vecPositionLast.Init( );
	m_vecVelocity.Init( );
	m_vecVelocityNormalized.Init( );
	m_vecVelocityNormalizedNonZero.Init( );
	m_flVelocityLengthXY = 0;
	m_flVelocityLengthZ = 0;
	m_flSpeedAsPortionOfRunTopSpeed = 0;
	m_flSpeedAsPortionOfWalkTopSpeed = 0;
	m_flSpeedAsPortionOfCrouchTopSpeed = 0;
	m_flDurationMoving = 0;
	m_flDurationStill = 0;
	m_bOnGround = true;
	m_flLandAnimMultiplier = 1.0f;
	m_flLeftGroundHeight = 0;
	m_bLanding = false;
	m_flJumpToFall = 0;
	m_flDurationInAir = 0;
	m_flWalkToRunTransition = 0;
	m_bLandedOnGroundThisFrame = false;
	m_bLeftTheGroundThisFrame = false;
	m_flInAirSmoothValue = 0;
	m_bOnLadder = false;
	m_flLadderWeight = 0;
	m_flLadderSpeed = 0;
	m_bWalkToRunTransitionState = ANIM_TRANSITION_WALK_TO_RUN;
	m_bDefuseStarted = false;
	m_bPlantAnimStarted = false;
	m_bTwitchAnimStarted = false;
	m_bAdjustStarted = false;
	m_flNextTwitchTime = 0;
	m_flTimeOfLastKnownInjury = 0;
	m_flLastVelocityTestTime = 0;
	m_vecVelocityLast.Init( );
	m_vecTargetAcceleration.Init( );
	m_vecAcceleration.Init( );
	m_flAccelerationWeight = 0;
	m_flAimMatrixTransition = 0;
	m_flAimMatrixTransitionDelay = 0;
	m_bFlashed = 0;
	m_flStrafeChangeWeight = 0;
	m_flStrafeChangeTargetWeight = 0;
	m_flStrafeChangeCycle = 0;
	m_nStrafeSequence = -1;
	m_bStrafeChanging = false;
	m_flDurationStrafing = 0;
	m_flFootLerp = 0;
	m_bFeetCrossed = false;
	m_bPlayerIsAccelerating = false;
	m_flDurationMoveWeightIsTooHigh = 0;
	m_flStaticApproachSpeed = 80;
	m_flStutterStep = 0;
	m_nPreviousMoveState = 0;
	m_flActionWeightBiasRemainder = 0;
	m_flAimYawMin = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN;
	m_flAimYawMax = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX;
	m_flAimPitchMin = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;
	m_flAimPitchMax = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;
	m_bFirstRunSinceInit = true;
}