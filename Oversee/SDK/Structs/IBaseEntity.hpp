#pragma once

class C_BaseEntity : public IClientEntity
{
public:
	VFunc( GetPredDescMap( ), 17, C_DataMap* ( __thiscall* )( void* ) );
	VFunc( IsPlayer( ), 158, bool( __thiscall* )( void* ) );
	VFunc( IsWeapon( ), 166, bool( __thiscall* )( void* ) );

	NETVAR( m_vecOrigin, Vector, "DT_BaseEntity", "m_vecOrigin" );
	NETVAR( m_nSurvivalTeam, int, "DT_CSPlayer", "m_nSurvivalTeam" );
	NETVAR( m_flSimulationTime, float, "DT_BaseEntity", "m_flSimulationTime" );
	NETVAR( m_nModelIndex, int, "DT_BaseEntity", "m_nModelIndex" );
	NETVAR( m_iTeamNum, int, "DT_BaseEntity", "m_iTeamNum" );
	NETVAR( m_nRenderMode, int, "DT_BaseEntity", "m_nRenderMode" );
	NETVAR( m_nExplodeEffectTickBegin, int, "DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin" );
	NETVAR( m_nOwnerEntity, CBaseHandle, "DT_BaseEntity", "m_hOwnerEntity" );

	DATAMAP( int, m_iEFlags );
	DATAMAP( float, m_flStamina );
	DATAMAP( float, m_flCycle );
	DATAMAP( int, m_fEffects );
	DATAMAP( Vector, m_vecAbsVelocity );
	DATAMAP( Vector, m_vecBaseVelocity );

	bool IsFriend( );
	__forceinline void OnSimulationTimeChanging( float flPreviousSimulationTime, float flNextSimulationTime )
	{
		( ( void( __thiscall* )( void*, float, float ) )( SDK::EngineData::m_AddressList[ CheatAddressList::OnSimulationTimeChanging ] ) )( this, flPreviousSimulationTime, flNextSimulationTime );
	}
	__forceinline void MakeEntitySpotted( )
	{
		/* m_bSpotted = true */
		*( bool* ) ( ( DWORD ) ( this ) + 0x93D ) = true;

		/* m_bSpottedBy[LocalPlayerIdx] = true */
		*( bool* ) ( ( DWORD ) ( this ) + 0x93E + SDK::Interfaces::EngineClient->GetLocalPlayer( ) ) = true;
	}
	__forceinline matrix3x4_t& m_rgflCoordinateFrame( )
	{
		return *( matrix3x4_t* ) ( ( DWORD ) ( this ) + 0x444 );
	}
	__forceinline bool IsBreakableEntity( )
	{
		if ( !this || !this->GetClientClass( ) )
			return false;

		const auto szObjectName = this->GetClientClass( )->m_strNetworkName;
		if ( szObjectName[ 0 ] == 'C' )
		{
			if ( szObjectName[ 7 ] == 't' || szObjectName[ 7 ] == 'b' )
				return true;
		}

		return ( ( bool( __thiscall* )( C_BaseEntity* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::IsBreakableEntity ] ) )( this );
	}
	__forceinline void InvalidateBoneCache( )
	{
		int nModelBoneCounter = **( int** ) ( ( ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ModelBoneCounter ] ) ) + 10 );

		*( float* ) ( ( DWORD ) this + 0x2928 ) = -FLT_MAX;
		*( int* ) ( ( DWORD ) this + 0x2690 ) = nModelBoneCounter - 1;
	}
	__forceinline void ForceBoneCache( )
	{
		int nModelBoneCounter = **( int** ) ( ( ( DWORD ) ( SDK::EngineData::m_AddressList[ CheatAddressList::ModelBoneCounter ] ) ) + 10 );

		*( float* ) ( ( DWORD ) this + 0x2928 ) = -FLT_MAX;
		*( int* ) ( ( DWORD ) this + 0x2690 ) = nModelBoneCounter;
	}
	__forceinline float& m_flOldSimulationTime( )
	{
		static DWORD dwSimulationTime = 0;
		if ( !dwSimulationTime )
			dwSimulationTime = NetVars::GetPropOffset( _S( "DT_BaseEntity" ), _S( "m_flSimulationTime" ) );

		return *( float* ) ( ( DWORD ) ( this ) + dwSimulationTime + 0x4 );
	}
	__forceinline void SetCollisionBounds( Vector vecMins, Vector vecMaxs )
	{
		( ( void( __thiscall* )( void*, Vector*, Vector* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetCollisionBounds ] ) )( ( void* ) ( ( DWORD ) ( this ) + 0x320 ), &vecMins, &vecMaxs );
	}
	__forceinline void SetAbsAngles( QAngle angViewAngles )
	{
		return ( ( void( __thiscall* )( LPVOID, QAngle& ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetAbsAngles ] ) )( this, angViewAngles );
	}
	__forceinline void SetAbsOrigin( Vector vetAbsOrigin )
	{
		return ( ( void( __thiscall* )( LPVOID, Vector& ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetAbsOrigin ] ) )( this, vetAbsOrigin );
	}
	__forceinline int GetSequenceActivity( int iSequence )
	{
		studiohdr_t* pStudioHDR = SDK::Interfaces::ModelInfo->GetStudiomodel( this->GetModel( ) );
		if ( !pStudioHDR )
			return -1;

		return ( ( int( FASTCALL* )( void*, void*, int ) )( SDK::EngineData::m_AddressList[ CheatAddressList::GetSequenceActivity ] ) )( this, pStudioHDR, iSequence );
	}
	inline float& m_flCreationTime( )
	{
		return *( float* ) ( ( uintptr_t ) ( this ) + 0x2D28 );
	}
};

class C_Precipitation : public C_BaseEntity
{
public:
	NETVAR( m_nPrecipitationType, int32_t, "DT_Precipitation", "m_nPrecipType" );
};