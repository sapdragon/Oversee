#pragma once
#include "IWeaponData.hpp"

__forceinline std::string WideToMultiByte( const std::wstring& str )
{
	int nStringLength = WideCharToMultiByte( CP_UTF8, 0, str.data( ), ( int ) ( str.size( ) ), 0, 0, 0, 0 );
	if ( nStringLength <= 0 )
		return "";

	std::string szString = std::string( nStringLength, 0 );
	WideCharToMultiByte( CP_UTF8, 0, str.data( ), ( int ) ( str.size( ) ), &szString[ 0 ], nStringLength, 0, 0 );
	return szString;
}

class C_BaseCombatWeapon : public C_BaseEntity
{
public:
	NETVAR( m_iClip1, int, "DT_BaseCombatWeapon", "m_iClip1" );
	NETVAR( m_iClip2, int, "DT_BaseCombatWeapon", "m_iClip2" );
	NETVAR( m_iPrimaryReserveAmmoCount, int, "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount" );
	NETVAR( m_flRecoilIndex, float, "DT_WeaponCSBase", "m_flRecoilIndex" );
	NETVAR( m_fAccuracyPenalty, float, "DT_WeaponCSBase", "m_fAccuracyPenalty" );
	NETVAR( m_flLastShotTime, float, "DT_WeaponCSBase", "m_fLastShotTime" );
	NETVAR( m_iViewModelIndex, int, "DT_BaseCombatWeapon", "m_iViewModelIndex" );
	NETVAR( m_iWorldModelIndex, int, "DT_BaseCombatWeapon", "m_iWorldModelIndex" );
	NETVAR( m_iWorldDroppedModelIndex, int, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex" );
	NETVAR( m_bPinPulled, bool, "DT_BaseCSGrenade", "m_bPinPulled" );
	NETVAR( m_fThrowTime, float, "DT_BaseCSGrenade", "m_fThrowTime" );
	NETVAR( m_flThrowStrength, float, "DT_BaseCSGrenade", "m_flThrowStrength" );
	NETVAR( m_iIronSightMode, int, "DT_WeaponCSBase", "m_iIronSightMode" );
	NETVAR( m_nZoomLevel, int, "DT_WeaponCSBase", "m_zoomLevel" );
	NETVAR( m_hWeaponWorldModel, CBaseHandle, "DT_BaseCombatWeapon", "m_hWeaponWorldModel" );

	VFunc( UpdateAccuracyPenalty( ), 484, void( __thiscall* )( void* ) );
	VFunc( GetWeaponData( ), 461, C_CSWeaponData* ( __thiscall* )( void* ) );
	VFunc( GetInaccuracy( ), 483, float( __thiscall* )( void* ) );
	VFunc( GetSpread( ), 453, float( __thiscall* )( void* ) );

	NETVAR( m_OriginalOwnerXuidLow, int, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow" );
	NETVAR( m_OriginalOwnerXuidHigh, int, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh" );
	NETVAR( m_iItemIDHigh, int, "DT_BaseAttributableItem", "m_iItemIDHigh" );
	NETVAR( m_iItemIDLow, int, "DT_BaseAttributableItem", "m_iItemIDLow" );
	NETVAR( m_iAccountID, int, "DT_BaseAttributableItem", "m_iAccountID" );
	NETVAR( m_nFallbackStatTrak, int, "DT_BaseAttributableItem", "m_nFallbackStatTrak" );
	NETVAR( m_bInitialized, int, "DT_BaseAttributableItem", "m_bInitialized" );

	DATAMAP( int, m_Activity );

	VFunc( SetModelIndex( int32_t index ), 75, void* ( __thiscall* )( void*, int32_t ), index );


	void EquipWearable( C_BaseEntity* entity ) {
		auto fn_equip_wearable = reinterpret_cast< int32_t( __thiscall* )( void*, void* ) >( SDK::EngineData::m_AddressList[ CheatAddressList::EquipWearable ] );
		fn_equip_wearable( this, entity );
	}

	void InitializeAttibutes( ) {
		auto fn_initialize_attibutes = reinterpret_cast< int32_t( __thiscall* )( void* ) >( SDK::EngineData::m_AddressList[ CheatAddressList::InitializeAttibutes ] );
		fn_initialize_attibutes( this );
	}

	inline int16_t& m_nItemID( )
	{
		return *( int16_t* ) ( ( DWORD ) ( this ) + 0x2FBA );
	}
	inline int& m_nWeaponMode( )
	{
		return *( int* ) ( ( DWORD ) ( this ) + 0x3328 );
	}

	inline bool IsRevolver( )
	{
		return m_nItemID( ) == WEAPON_REVOLVER;
	}
	inline bool IsSMG( )
	{
		int ID = m_nItemID( );
		if ( ID == WEAPON_M249 || ID == WEAPON_NEGEV || ID == WEAPON_MP5SD || ID == WEAPON_MP7 || ID == WEAPON_MP9 || ID == WEAPON_P90 || ID == WEAPON_UMP45 || ID == WEAPON_BIZON )
			return true;

		return false;
	}
	inline bool IsGrenade( )
	{
		return GetWeaponData( )->m_iWeaponType == WEAPONTYPE_GRENADE;
	}
	inline bool IsGun( )
	{
		switch ( GetWeaponData( )->m_iWeaponType )
		{
			case WEAPONTYPE_C4:
			return false;
			case WEAPONTYPE_GRENADE:
			return false;
			case WEAPONTYPE_KNIFE:
			return false;
			case WEAPONTYPE_UNKNOWN:
			return false;
			default:
			return true;
		}
	}
	inline bool CanShift( )
	{
		switch ( ( int ) ( this->m_nItemID( ) ) )
		{
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 84:
				return false;
			break;
		}

		return this->IsGun( );
	}
	inline bool IsKnife( )
	{
		int idx = this->m_nItemID( );
		return idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
			|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET
			|| idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL
			|| idx == WEAPON_KNIFEGG || idx == WEAPON_KNIFE_GHOST || idx == WEAPON_KNIFE_GYPSY_JACKKNIFE || idx == WEAPON_KNIFE_STILETTO
			|| idx == WEAPON_KNIFE_URSUS || idx == WEAPON_KNIFE_WIDOWMAKER || idx == WEAPON_KNIFE_CSS || idx == WEAPON_KNIFE_CANIS
			|| idx == WEAPON_KNIFE_CORD || idx == WEAPON_KNIFE_OUTDOOR || idx == WEAPON_KNIFE_SKELETON;
	}
	inline bool IsRifle( )
	{
		switch ( GetWeaponData( )->m_iWeaponType )
		{
			case WEAPONTYPE_RIFLE:
			return true;
			case WEAPONTYPE_SUBMACHINEGUN:
			return true;
			case WEAPONTYPE_SHOTGUN:
			return true;
			case WEAPONTYPE_MACHINEGUN:
			return true;
			default:
			return false;
		}
	}
	inline bool IsSniper( )
	{
		int idx = this->m_nItemID( );
		return idx == WEAPON_SCAR20 || idx == WEAPON_G3SG1 || idx == WEAPON_AWP || idx == WEAPON_SG553 || idx == WEAPON_SSG08 || idx == WEAPON_AUG;
	}

	inline float& m_flNextPrimaryAttack( )
	{
		return *( float* ) ( ( uintptr_t ) ( this ) + 0x3248 );
	}
	inline float& m_flNextSecondaryAttack( )
	{
		return *( float* ) ( ( uintptr_t ) ( this ) + 0x324C );
	}
	inline float& m_flPostponeFireReadyTime( )
	{
		return *( float* ) ( ( uintptr_t ) ( this ) + 0x335C );
	}

	inline std::string GetLocalizedName( )
	{
		LPVOID pEconItem = ( ( LPVOID( __thiscall* )( LPVOID ) )( SDK::EngineData::m_AddressList[ CheatAddressList::GetEconItemView ] ) )( this );
		if ( !pEconItem )
			return _S( "Invalid" );

		C_EconItemDefinition* pEconItemDefinition = ( ( C_EconItemDefinition * ( __thiscall* )( LPVOID ) )( SDK::EngineData::m_AddressList[ CheatAddressList::GetStaticData ] ) )( pEconItem );
		if ( !pEconItemDefinition )
			return _S( "Invalid" );

		return WideToMultiByte( SDK::Interfaces::Localize->Find( pEconItemDefinition->GetItemBaseName( ) ) );
	}

	inline CHandle < C_BasePlayer >& m_hThrower()
	{
		return *(CHandle < C_BasePlayer >*)((uintptr_t)(this) + 10656);
	}


};