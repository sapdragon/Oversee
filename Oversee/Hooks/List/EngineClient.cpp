#include "../Hooks.hpp"

float C_Hooks::hkGetScreenSizeAspectRatio( LPVOID pEcx, uint32_t, int X, int Y )
{
	return g_SettingsManager->F[ _S( "misc.aspect_ratio" ) ];
}
bool C_Hooks::hkIsConnected( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD >( _ReturnAddress( ) ) == reinterpret_cast < DWORD >( SDK::EngineData::m_AddressList[ CheatAddressList::IsConnected_Return ] ) )
		if ( g_SettingsManager->B[ _S( "misc.unlock_inventory_access" ) ] )
			return false;

	return Hooks::o_IsConnected( pEcx );
}
bool C_Hooks::hkIsLowViolence( LPVOID pEcx, uint32_t )
{
	return Hooks::o_IsLowViolence( pEcx );
}
void C_Hooks::hkInitNewParticlesScalar( C_InitRandomColor* thisPtr, void* edx, CParticleCollection* pParticles, int start_p, int nParticleCount, int nAttributeWriteMask, void* pContext )
{
	Vector o_min = thisPtr->m_flNormColorMin;
	Vector o_max = thisPtr->m_flNormColorMax;

	const char* mat_name = *( char** ) ( *( uintptr_t* ) ( ( uintptr_t ) pParticles + 0x48 ) + 0x40 );
	assert( mat_name );

	switch ( FNV1A( mat_name ) )
	{
		case FNV1A( "particle\\fire_burning_character\\fire_env_fire.vmt" ):
		case FNV1A( "particle\\fire_burning_character\\fire_env_fire_depthblend.vmt" ):
		case FNV1A( "particle\\fire_burning_character\\fire_burning_character_depthblend.vmt" ):
		case FNV1A( "particle\\fire_burning_character\\fire_burning_character.vmt" ):
		case FNV1A( "particle\\fire_burning_character\\fire_burning_character_nodepth.vmt" ):
		case FNV1A( "particle\\particle_flares\\particle_flare_001.vmt" ):
		case FNV1A( "particle\\particle_flares\\particle_flare_004.vmt" ):
		case FNV1A( "particle\\particle_flares\\particle_flare_004b_mod_ob.vmt" ):
		case FNV1A( "particle\\particle_flares\\particle_flare_004b_mod_z.vmt" ):
		case FNV1A( "particle\\fire_explosion_1\\fire_explosion_1_bright.vmt" ):
		case FNV1A( "particle\\fire_explosion_1\\fire_explosion_1b.vmt" ):
		case FNV1A( "particle\\fire_particle_4\\fire_particle_4.vmt" ):
		case FNV1A( "particle\\fire_explosion_1\\fire_explosion_1_oriented.vmt" ):
		thisPtr->m_flNormColorMin = thisPtr->m_flNormColorMax = Vector( 0, 1, 0 );
		break;
	}

	Hooks::o_InitNewParticlesScalar( thisPtr, edx, pParticles, start_p, nParticleCount, nAttributeWriteMask, pContext );

	thisPtr->m_flNormColorMin = o_min;
	thisPtr->m_flNormColorMax = o_max;
}
