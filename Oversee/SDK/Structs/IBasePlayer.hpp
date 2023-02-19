#pragma once
#include "IAnimationState.hpp"
#include "ISurfaceData.hpp"
#include "../Misc/BoneAccessor.hpp"

class C_BasePlayer : public C_BaseEntity
{
public:
	static C_BasePlayer* GetPlayerByIndex( int index )
	{
		return static_cast < C_BasePlayer* > ( SDK::Interfaces::EntityList->GetClientEntity( index ) );
	}
public:
	VFunc( WorldSpaceCenter( ), 79, Vector( __thiscall* )( void* ) );
	VFunc( UpdateClientSideAnimation( ), 224, void( __thiscall* )( void* ) );
	VFunc( GetLayerSequenceCycleRate( C_AnimationLayer* AnimationLayer, int iLayerSequence ), 223, float( __thiscall* )( void*, C_AnimationLayer*, int ), AnimationLayer, iLayerSequence );
	VFunc( SetModelIndex(int32_t m_nIndex), 75, void(__thiscall*) (void*, int32_t), m_nIndex);

	CUSTOM_OFFSET( m_nMoveType, int, 0x25C );
	CUSTOM_OFFSET( m_nClientEffects, int, 0x68 );
	CUSTOM_OFFSET( m_nLastSkipFramecount, int, 0xA68 );
	CUSTOM_OFFSET( m_nOcclusionFrame, int, 0xA24 );
	CUSTOM_OFFSET( m_nOcclusionMask, int, 0xA28 );
	CUSTOM_OFFSET( m_bJiggleBones, bool, 0x2930 );

	NETVAR( m_vecVelocity,			Vector		, "DT_BasePlayer", "m_vecVelocity[0]" );
	NETVAR( m_vecViewOffset,		Vector		, "DT_BasePlayer", "m_vecViewOffset[0]" );
	NETVAR( m_aimPunchAngleVel,		Vector		, "DT_BasePlayer", "m_aimPunchAngleVel" );
	NETVAR( m_viewPunchAngle,		QAngle		, "DT_BasePlayer", "m_viewPunchAngle" );
	NETVAR( m_aimPunchAngle,		QAngle		, "DT_BasePlayer", "m_aimPunchAngle" );
	NETVAR( m_angEyeAngles,			QAngle		, "DT_CSPlayer", "m_angEyeAngles[0]" );
	NETVAR( m_nTickBase,			int			, "DT_BasePlayer", "m_nTickBase" );
	NETVAR( m_fLifeState,			int			, "DT_BasePlayer", "m_lifeState" );
	NETVAR( m_iHealth,				int			, "DT_BasePlayer", "m_iHealth" );
	NETVAR(	m_iHideHUD,				int			, "DT_BasePlayer", "m_iHideHUD" );
	NETVAR( m_fFlags,				int			, "DT_BasePlayer", "m_fFlags" );
	NETVAR( m_hObserverTarget,      CHandle<C_BasePlayer>, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR( m_iObserverMode,		int			, "DT_BasePlayer", "m_iObserverMode" );
	NETVAR( m_ArmourValue,			int			, "DT_CSPlayer", "m_ArmorValue" );
	NETVAR( m_iAccount,				int			, "DT_CSPlayer", "m_iAccount" );
	NETVAR( m_iMoveState,			int			, "DT_CSPlayer", "m_iMoveState" );
	NETVAR( m_iShotsFired,			int			, "DT_CSPlayer", "m_iShotsFired" );
	NETVAR( m_nHitboxSet,			int			, "DT_BaseAnimating", "m_nHitboxSet" );
	NETVAR(	m_flFallVelocity,		float		, "DT_BasePlayer", "m_flFallVelocity" );
	NETVAR( m_flLowerBodyYaw,		float		, "DT_CSPlayer", "m_flLowerBodyYawTarget" );
	NETVAR( m_flVelocityModifier,	float		, "DT_CSPlayer", "m_flVelocityModifier" );
	NETVAR( m_flThirdpersonRecoil,	float		, "DT_CSPlayer", "m_flThirdpersonRecoil" );
	NETVAR( m_flFlashDuration,		float		, "DT_CSPlayer", "m_flFlashDuration" );
	NETVAR(	m_flNextAttack,			float		, "DT_BaseCombatCharacter", "m_flNextAttack" );
	NETVAR( m_flDuckSpeed,			float		, "DT_BasePlayer", "m_flDuckSpeed" );
	NETVAR( m_flDuckAmount,			float		, "DT_BasePlayer", "m_flDuckAmount" );
	NETVAR( m_bHasHeavyArmor,		bool		, "DT_CSPlayer", "m_bHasHeavyArmor" );
	NETVAR( m_bHasHelmet,			bool		, "DT_CSPlayer", "m_bHasHelmet" );
	NETVAR( m_bIsScoped,			bool		, "DT_CSPlayer", "m_bIsScoped" );
	NETVAR( m_bGunGameImmunity,		bool		, "DT_CSPlayer", "m_bGunGameImmunity" );
	NETVAR( m_bIsWalking,			bool		, "DT_CSPlayer", "m_bIsWalking" );
	NETVAR( m_bStrafing,			bool		, "DT_CSPlayer", "m_bStrafing" );
	NETVAR( m_bClientSideAnimation,	bool		, "DT_BaseAnimating", "m_bClientSideAnimation" );
	NETVAR( m_bDucking,				bool		, "DT_BasePlayer", "m_bDucking" );
	NETVAR( m_bDucked,				bool		, "DT_BasePlayer", "m_bDucked" );
	NETVAR( m_szLastPlaceName,		const char*	, "DT_BasePlayer", "m_szLastPlaceName" );
	NETVAR( m_nBody,				int32_t		, "DT_BaseAnimating", "m_nBody");
	NETVAR( m_hRagdoll,				CBaseHandle , "DT_CSPlayer", "m_hRagdoll");

	CUSTOM_OFFSET( m_flOldSimulationTime, float, 620 );
	DATAMAP( float, m_surfaceFriction );
	DATAMAP( Vector, m_vecNetworkOrigin );

	NETVAR( m_hGroundEntity,		CHandle < C_BaseEntity >, "DT_BasePlayer", "m_hGroundEntity" );
	NETVAR( m_hActiveWeapon,		CHandle < C_BaseCombatWeapon >, "DT_BaseCombatCharacter", "m_hActiveWeapon" );
	NETVAR( m_hViewModel,			CHandle < C_BaseViewmodel >, "DT_BasePlayer", "m_hViewModel[0]" );

	float __declspec( noinline ) GetSequenceMoveDist( int nSequence )
	{
		Vector vecOutput;

		typedef void( __fastcall* GetSequenceLinearMotion_t )( C_StudioHDR*, int, float*, Vector* );
		( ( GetSequenceLinearMotion_t )( SDK::EngineData::m_AddressList[ CheatAddressList::GetSequenceLinearMotion ] ) )( this->GetStudioHdr( ), nSequence, ( float* )( this->m_aPoseParameters( ).data( ) ), &vecOutput );

		return vecOutput.Length( );
	}

	__forceinline void InvalidatePhysicsRecursive( int Flags )
	{
		return ( ( void( __thiscall* )( void*, int ) )( SDK::EngineData::m_AddressList[ CheatAddressList::InvalidatePhysicsRecursive ] ) )( this, Flags );
	}
	__forceinline void UpdateCollisionBounds( )
	{
		return ( ( void( __thiscall* )( void* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::UpdateCollisionBounds ] ) )( this );
	}
	__forceinline C_CSGOPlayerAnimationState* GetAnimationState( )
	{
		return *( C_CSGOPlayerAnimationState** ) ( ( DWORD ) ( this ) + 0x9960 );
	}
	__forceinline C_AnimationLayer* GetAnimationLayers( )
	{
		return *( C_AnimationLayer** ) ( ( DWORD ) ( this ) + 0x2990 );
	}
	__forceinline std::array < float, MAXSTUDIOPOSEPARAM >& m_aPoseParameters( )
	{
		return *( std::array < float, MAXSTUDIOPOSEPARAM >* )( ( DWORD ) ( this ) + 0x2778 );
	}
	__forceinline int GetAnimationLayerCount( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x299C );
	}
	__forceinline int GetBoneCounter( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x2920 );
	}
	__forceinline int& m_nFinalPredictedTick( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x3444 );
	}
	__forceinline int m_nPlayerState( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x9978 );
	}
	__forceinline int GetWaterLevel( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x25E );
	}
	__forceinline QAngle& m_angVisualAngles( )
	{
		return *( QAngle* ) ( ( DWORD ) ( this ) + 0x31E8 );
	}
	__forceinline bool m_bDuckUntilOnGround( )
	{
		return *( bool* ) ( ( DWORD ) ( this ) + 0x10478 );
	}
	__forceinline bool IsAlive( )
	{
		return m_fLifeState( ) == LIFE_ALIVE;
	}
	__forceinline bool IsFrozen( )
	{
		return m_fFlags( ) & FL_FROZEN || ( ( *SDK::Interfaces::GameRules ) && ( *SDK::Interfaces::GameRules )->IsFreezePeriod( ) );
	}
	__forceinline bool m_bWaitForNoAttack( )
	{
		return *( bool* ) ( ( DWORD ) ( this ) + 0x99BC );
	}
	__forceinline bool m_bIsDefusing( )
	{
		return *( bool* ) ( ( DWORD ) ( this ) + 0x997C );
	}
	__forceinline float& m_flCollisionChangeTime( )
	{
		return *( float* ) ( ( DWORD ) ( this ) + 0x9924 );
	}
	__forceinline float& m_flCollisionChangeOrigin( )
	{
		return *( float* ) ( ( DWORD ) ( this ) + 0x9920 );
	}
	__forceinline float& m_flSpawnTime( )
	{
		return *( float* ) ( ( DWORD ) ( this ) + 0x103C0 );
	}
	__forceinline C_StudioHDR* GetStudioHdr( )
	{
		return *( C_StudioHDR** ) ( ( DWORD ) ( this ) + 0x2950 );
	}
	__forceinline C_SurfaceData* GetSurfaceData( )
	{
		return *( C_SurfaceData** ) ( ( DWORD ) ( this ) + 0x35DC );
	}
	__forceinline C_UserCmd*& GetUserCmd( )
	{
		return *( C_UserCmd** ) ( ( DWORD ) ( this ) + 0x3348 );
	}
	__forceinline CBaseHandle* m_hMyWearables()
	{
		return (CBaseHandle*)((DWORD)(this) + 0x2F14);
	}


	__forceinline CBaseHandle* m_hMyWeapons()
	{
		return (CBaseHandle*)((DWORD)(this) + 0x2E08);
	}

	__forceinline Vector GetShootPosition( )
	{
		Vector vecShootPosition = Vector( 0, 0, 0 );

		typedef void( __thiscall* GetShootPosition_t )( LPVOID, Vector* );
		reinterpret_cast < GetShootPosition_t >( GetVFunction_NoObf( this, 285 ) )( this, &vecShootPosition );
		return vecShootPosition;
	}
	__forceinline CUtlVector < matrix3x4_t >& m_CachedBoneData( )
	{
		return *( CUtlVector < matrix3x4_t >* )( ( DWORD ) ( this ) + 0x2914 );
	}
	__forceinline C_BoneAccessor& GetBoneAccessor( )
	{
		return *( C_BoneAccessor* ) ( ( DWORD ) ( this ) + 0x26A8 );
	}
	__forceinline void SetAsPredictionPlayer( )
	{
		( **( ( C_BasePlayer*** ) ( SDK::EngineData::m_AddressList[ CheatAddressList::PredictionPlayer ] ) ) ) = this;
	}
	__forceinline void UnsetAsPredictionPlayer( )
	{
		( **( ( C_BasePlayer*** ) ( SDK::EngineData::m_AddressList[ CheatAddressList::PredictionPlayer ] ) ) ) = NULL;
	}
	__forceinline void SetupBones_AttachmentHelper( )
	{
		return ( ( void( __thiscall* )( void*, void* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetupBonesAttachmentHelper ] ) )( this, this->GetStudioHdr( ) );
	}
	__forceinline typedescription_t* GetDatamapEntry( C_DataMap* pDatamap, const char* szName )
	{
		while ( pDatamap )
		{
			for ( int i = 0; i < pDatamap->m_iDataNumFields; i++ )
			{
				if ( strcmp( szName, pDatamap->m_pDataDescription[ i ].fieldName ) ) //-V526
					continue;

				return &pDatamap->m_pDataDescription[ i ];
			}

			pDatamap = pDatamap->m_pBaseMap;
		}

		return NULL;
	}
};