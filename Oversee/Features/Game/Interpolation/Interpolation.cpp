#include "Interpolation.hpp"

void C_LocalInterpolation::SetupInterpolation( bool bPostFrame )
{
	if ( bPostFrame )
	{
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount = m_Data.m_flInterpolationAmount;
		return;
	}

	/* fix interpolation */
	m_Data.m_flInterpolationAmount = SDK::Interfaces::GlobalVars->m_flInterpolationAmount;
	if ( m_Data.m_bSkipInterpolation )
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount = 0.0f;

	/* fix predicted tick */
	g_Globals->m_LocalPlayer->m_nFinalPredictedTick( ) = g_Globals->m_LocalPlayer->m_nTickBase( ) + static_cast < int > ( m_Data.m_bSkipInterpolation );
}
void C_LocalInterpolation::SkipInterpolation( )
{
	m_Data.m_bSkipInterpolation = true;
}
void C_LocalInterpolation::ContinueInterpolation( )
{
	m_Data.m_bSkipInterpolation = false;
}