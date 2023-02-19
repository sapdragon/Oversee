#include "Viewmodel.hpp"
#include "../../../SDK/Utils/Utils.hpp"

void C_ViewmodelAnimation::ModifyOrigin( )
{
	/* Check Alive and SignonState */
	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) || SDK::Interfaces::ClientState->m_nSignonState < SIGNONSTATE_FULL )
		return;
	
	/* Get Viewmodel */
	C_BaseViewmodel* m_pViewModel = g_Globals->m_LocalPlayer->m_hViewModel( );
	if ( !m_pViewModel )
		return;

	/* AngleVectors */
	Vector vecForward, vecRight, vecUp;
	Math::AngleVectors( m_pViewModel->GetAbsAngles( ), vecForward, vecRight, vecUp );

	/* Custom axis */
	const float flAxisX = g_SettingsManager->F[ _S( "misc.viewmodel.x" ) ];
	const float flAxisY = g_SettingsManager->F[ _S( "misc.viewmodel.y" ) ];
	const float flAxisZ = g_SettingsManager->F[ _S( "misc.viewmodel.z" ) ];

	/* call C_BaseEntity::SetAbsOrigin */
	m_pViewModel->SetAbsOrigin( m_pViewModel->GetAbsOrigin( ) + ( vecRight * flAxisX ) + ( vecForward * flAxisY ) + ( vecUp * flAxisZ ) );
}
void C_ViewmodelAnimation::ResolveSequences( )
{
	/* Check Alive and SignonState */
	//if ( !g_Globals->m_LocalPlayer->IsAlive( ) || SDK::Interfaces::ClientState->m_nSignonState < SIGNONSTATE_FULL )
		return;

	/* Get Viewmodel */
	C_BaseViewmodel* m_pViewModel = g_Globals->m_LocalPlayer->m_hViewModel( );
	if ( !m_pViewModel )
		return;

	/* Get Original activity and sequence */
	int m_nSequence = m_pViewModel->m_iSequence( );
	int m_nActivity = m_pViewModel->GetSequenceActivity( m_nSequence );

	/* Fixed data */
	int m_iNewActivity = 0;
	int m_iOffset = 0;

	/* Iterate sequences */
	for ( int nSequence = 0; nSequence < 20; nSequence++ )
	{
		/* get temp activity */
		int nTempActivity = m_pViewModel->GetSequenceActivity( nSequence );
		if ( m_nActivity != -1 && m_nActivity == nTempActivity || m_nActivity == -1 && nTempActivity == 213 )
		{
			m_iNewActivity = nSequence;
			for ( int iNumber = 0; iNumber < 4; iNumber++ )
			{
				if ( m_pViewModel->GetSequenceActivity( nSequence + iNumber ) == nTempActivity )
					m_iOffset++;
			}

			break;
		}
		continue;
	}

	/* Fix activity  */
	if ( m_nActivity == -1 && m_iNewActivity == 1 )
		m_iNewActivity = m_nSequence;

	/* Fix sequence */
	if ( !m_iOffset )
		m_nSequence = m_iNewActivity;
	else
		m_nSequence = g_Utils->RandomInt( m_iNewActivity, m_iNewActivity + m_iOffset - 1 );

	/* Set Sequence */
	m_pViewModel->SendViewmodelMatchingSequence( m_nSequence );
}
void C_ViewmodelAnimation::SaveViewmodelData( )
{
	C_BaseViewmodel* pViewModel = g_Globals->m_LocalPlayer->m_hViewModel( );
	if ( !pViewModel )
		return;

	m_Data.m_flCycle = pViewModel->m_flCycle( );
	m_Data.m_flAnimTime = pViewModel->m_flAnimTime( );
	m_Data.m_nSequence = pViewModel->m_iSequence( );
	m_Data.m_nAnimationParity = pViewModel->m_iAnimationParity( );
	m_Data.m_dwWeaponLong = pViewModel->m_hWeapon( ).ToLong( );
}
void C_ViewmodelAnimation::ForceViewmodelData( )
{
	C_BaseViewmodel* pViewModel = g_Globals->m_LocalPlayer->m_hViewModel( );
	if ( !pViewModel )
		return;

	bool bIsDataValid = true;
	if ( pViewModel->m_iSequence( ) != m_Data.m_nSequence )
		bIsDataValid = false;

	if ( pViewModel->m_iAnimationParity( ) != m_Data.m_nAnimationParity )
		bIsDataValid = false;

	if ( pViewModel->m_hWeapon( ).ToLong( ) != m_Data.m_dwWeaponLong )
		bIsDataValid = false;

	if ( bIsDataValid )
	{
		pViewModel->m_flCycle( ) = m_Data.m_flCycle;
		pViewModel->m_flAnimTime( ) = m_Data.m_flAnimTime;
	}
}