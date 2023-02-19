#include "../Hooks.hpp"
#include "../../Features/Game/Prediction/EnginePrediction.hpp"
#include "../../SDK/Utils/Utils.hpp"

void FASTCALL C_Hooks::hkSetupAliveLoop( LPVOID pEcx, uint32_t )
{
	C_CSGOPlayerAnimationState* m_AnimationState = static_cast < C_CSGOPlayerAnimationState* > ( pEcx );
	if ( !m_AnimationState->m_pBasePlayer || m_AnimationState->m_pBasePlayer != g_Globals->m_LocalPlayer )
		return;
	
	C_AnimationLayer* m_AliveLoop = &m_AnimationState->m_pBasePlayer->GetAnimationLayers( )[ ANIMATION_LAYER_ALIVELOOP ];
	if ( !m_AliveLoop )
		return;

	if ( m_AliveLoop->m_nSequence != 8 && m_AliveLoop->m_nSequence != 9 )
	{
		m_AnimationState->SetLayerSequence( m_AliveLoop, ACT_CSGO_ALIVE_LOOP );
		m_AnimationState->SetLayerCycle( m_AliveLoop, g_Utils->RandomFloat( 0.0f, 1.0f ) );
		m_AnimationState->SetLayerRate( m_AliveLoop, m_AnimationState->m_pBasePlayer->GetLayerSequenceCycleRate( m_AliveLoop, m_AliveLoop->m_nSequence ) * g_Utils->RandomFloat( 0.8, 1.1f ) );
	}
	else
	{
		float_t flRetainCycle = m_AliveLoop->m_flCycle;
		if ( m_AnimationState->m_pWeapon != m_AnimationState->m_pWeaponLast )
		{
			m_AnimationState->SetLayerSequence( m_AliveLoop, ACT_CSGO_ALIVE_LOOP );
			m_AnimationState->SetLayerCycle( m_AliveLoop, flRetainCycle );
		}
		else if ( m_AnimationState->IsLayerSequenceFinished( m_AliveLoop, m_AnimationState->m_flLastUpdateIncrement ) )
			m_AnimationState->SetLayerRate( m_AliveLoop, m_AnimationState->m_pBasePlayer->GetLayerSequenceCycleRate( m_AliveLoop, m_AliveLoop->m_nSequence ) * g_Utils->RandomFloat( 0.8, 1.1f ) );
		else
			m_AnimationState->SetLayerWeight( m_AliveLoop, Math::RemapValClamped( m_AnimationState->m_flSpeedAsPortionOfRunTopSpeed, 0.55f, 0.9f, 1.0f, 0.0f ) );
	}

	return m_AnimationState->IncrementLayerCycle( m_AliveLoop, true );
}
void FASTCALL C_Hooks::hkModifyEyePosition( LPVOID pEcx, uint32_t, Vector& vecEyePosition )
{
	return;
}