#include "ItemsManager.hpp"
#include "../../Interface/GUI/Notifies.h"

void C_ItemManager::Initialize( )
{
	const auto item_system = C_ItemSystem::GetItemSystem( );

	for ( uint32_t i = NULL; i < item_system->m_nCountItemID; i++ ) {
		C_EconItemDefinition* item = *reinterpret_cast< C_EconItemDefinition** >( item_system->m_nItemID + 0x4 * ( 0x3 * i ) + 0x4 );
		switch ( FNV1A( item->m_szType ) ) {
			case FNV1A( "#CSGO_Type_Pistol" ):
			case FNV1A( "#CSGO_Type_Rifle" ):
			case FNV1A( "#CSGO_Type_SniperRifle" ):
			case FNV1A( "#CSGO_Type_Machinegun" ):
			case FNV1A( "#CSGO_Type_SMG" ):
			case FNV1A( "#CSGO_Type_Shotgun" ):
			case FNV1A( "#CSGO_Type_Knife" ):
			case FNV1A( "#CSGO_Type_Grenade" ):
			case FNV1A( "#CSGO_Type_Equipment" ):
			case FNV1A( "#CSGO_Type_C4" ):
			case FNV1A( "#Type_Hands" ):
			case FNV1A( "#CSGO_Type_Collectible" ):
			case FNV1A( "#Type_CustomPlayer" ):
			case FNV1A( "#CSGO_Type_MusicKit" ):
			case FNV1A( "#CSGO_Type_WeaponCase" ):
			case FNV1A( "#CSGO_Tool_WeaponCase_KeyTag" ):
				items[ item->m_nID ] = item;
			break;
		}
	}

	for ( size_t i = NULL; i < item_system->m_nCountPaintKit; i++ ) {
		auto kit = *reinterpret_cast< C_EconPaintKitDefinition** >( item_system->m_nPaintKit + 0x18 * i + 0x14 );
		const auto kit_name = kit->m_szName;

		for ( size_t j = NULL; j < item_system->m_nCountAlternativeIcon; j++ ) {
			auto name = std::string( *reinterpret_cast< const char** >( 0x78 * j + item_system->m_nAlternativeIcon + 0x18 ) );

			if ( name[ name.length( ) - 0x7 ] != '_' )
				continue;

			name = name.substr( 0x17, name.length( ) - 0x1E );

			const auto pos = name.find( kit_name );
			if ( pos == std::string::npos )
				continue;

			const auto weapon_name = name.substr( NULL, pos - 0x1 );

			if ( name.length( ) - pos != strlen( kit_name ) )
				continue;

			auto weapon = item_system->GetItemSchemaInterface( )->GetItemDefentionByName( weapon_name.c_str( ) );
			if ( !weapon )
				continue;

			PaintKitsType[ weapon->m_nID ].emplace_back( kit );
			PaintKits[ kit->m_nID ] = kit;
		}
	}

	for ( const auto& it : PaintKitsType ) {
		const auto type = FNV1A( items[ it.first ]->m_szType );

		if ( type == FNV1A( "#CSGO_Type_Knife" ) || type == FNV1A( "#Type_Hands" ) )
			continue;

		std::sort( PaintKitsType[ it.first ].begin( ), PaintKitsType[ it.first ].end( ), [ ] ( auto a, auto b ) -> bool {
			return a->m_nRarity > b->m_nRarity;
		} );
	}

	for ( size_t i = 2; i < item_system->m_nCountMusicKit; i++ ) {
		auto kit = *reinterpret_cast< C_EconMusicDefinition** >( item_system->m_nMusicKit + 0x18 * i + 0x14 );
		MusicKits[ kit->m_nID ] = kit;
	}

	for ( size_t i = 1; i < item_system->m_nCountStickerKit; i++ ) {
		auto sticker_kit = *reinterpret_cast< C_EconStickerDefinition** >( item_system->m_nStickerKit + 0x18 * i + 0x14 );
		StickerKits[ sticker_kit->m_nID ] = sticker_kit;
	}
}

std::string ToUtf8( std::wstring_view wstr ) {

	if ( wstr.empty( ) )
		return std::string( );

	auto size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int32_t >( wstr.size( ) ), NULL, 0, NULL, NULL );

	std::string out( size_needed, 0 );
	WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int >( wstr.size( ) ), &out[ 0 ], size_needed, NULL, NULL );

	return out;
}

std::string LocalizeTex( const char* in ) {

	const auto wide_name = SDK::Interfaces::Localize->FindSafe( in );

	if ( !wcslen( wide_name ) )
		return "";

	return ToUtf8( wide_name );
}

void C_ItemManager::ImagePrecache( )
{
	const auto item_system = C_ItemSystem::GetItemSystem( );

	for ( uint32_t i = NULL; i < item_system->m_nCountItemID; i++ ) {
		auto item = *reinterpret_cast< C_EconItemDefinition** >( item_system->m_nItemID + 0x4 * ( 0x3 * i ) + 0x4 );

		if ( std::string( item->m_szName ).find( "#CSGO_CustomPlayer" ) != std::string::npos ) {
			if ( item->m_szInventoryImage != nullptr )
				g_ItemManager.m_vAgents.push_back( new AgentsItem_t {
				item->m_nID,
				item->m_szName,
				std::string( item->m_szInventoryImage ).find( "ctm" ) != std::string::npos ? 1 : 0,
				item->m_szInventoryImage != nullptr ? g_ItemManager.GetSkin( std::string( "resource/flash/" + std::string( item->m_szInventoryImage ) + ".png" ) ) : nullptr
				} );
		}

		switch ( item->m_nID ) {
			case WEAPON_DEAGLE:
			case WEAPON_ELITE:
			case WEAPON_FIVESEVEN:
			case WEAPON_GLOCK:
			case WEAPON_AK47:
			case WEAPON_AUG:
			case WEAPON_AWP:
			case WEAPON_FAMAS:
			case WEAPON_G3SG1:
			case WEAPON_GALILAR:
			case WEAPON_M249:
			case WEAPON_M4A1:
			case WEAPON_MAC10:
			case WEAPON_P90:
			case WEAPON_MP5SD:
			case WEAPON_UMP45:
			case WEAPON_XM1014:
			case WEAPON_BIZON:
			case WEAPON_MAG7:
			case WEAPON_NEGEV:
			case WEAPON_SAWEDOFF:
			case WEAPON_TEC9:
			case WEAPON_TASER:
			case WEAPON_HKP2000:
			case WEAPON_MP7:
			case WEAPON_MP9:
			case WEAPON_NOVA:
			case WEAPON_P250:
			case WEAPON_SCAR20:
			case WEAPON_SG553:
			case WEAPON_SSG08:
			case WEAPON_M4A1_SILENCER:
			case WEAPON_USP_SILENCER:
			case WEAPON_CZ75A:
			case WEAPON_REVOLVER:
			case WEAPON_KNIFE_BAYONET:
			case WEAPON_KNIFE_CSS:
			case WEAPON_KNIFE_FLIP:
			case WEAPON_KNIFE_GUT:
			case WEAPON_KNIFE_KARAMBIT:
			case WEAPON_KNIFE_M9_BAYONET:
			case WEAPON_KNIFE_TACTICAL:
			case WEAPON_KNIFE_FALCHION:
			case WEAPON_KNIFE_SURVIVAL_BOWIE:
			case WEAPON_KNIFE_BUTTERFLY:
			case WEAPON_KNIFE_PUSH:
			case WEAPON_KNIFE_CORD:
			case WEAPON_KNIFE_CANIS:
			case WEAPON_KNIFE_URSUS:
			case WEAPON_KNIFE_GYPSY_JACKKNIFE:
			case WEAPON_KNIFE_OUTDOOR:
			case WEAPON_KNIFE_STILETTO:
			case WEAPON_KNIFE_WIDOWMAKER:
			case WEAPON_KNIFE_SKELETON:
			g_ItemManager.m_vDefaults.push_back( new DefaultItem_t {
				item->m_nID,
				item->m_szName,
				g_ItemManager.GetSkin( g_ItemManager.GetSkinPath( item->m_nID, 0 ) )
			} );
			break;
		}
	}

	for ( size_t i = NULL; i < item_system->m_nCountPaintKit; i++ ) {
		auto kit = *reinterpret_cast< C_EconPaintKitDefinition** >( item_system->m_nPaintKit + 0x18 * i + 0x14 );
		const auto kit_name = kit->m_szName;

		for ( size_t j = NULL; j < item_system->m_nCountAlternativeIcon; j++ ) {
			auto name = std::string( *reinterpret_cast< const char** >( 0x78 * j + item_system->m_nAlternativeIcon + 0x18 ) );

			if ( name[ name.length( ) - 0x7 ] != '_' )
				continue;

			name = name.substr( 0x17, name.length( ) - 0x1E );

			const auto pos = name.find( kit_name );
			if ( pos == std::string::npos )
				continue;

			const auto weapon_name = name.substr( NULL, pos - 0x1 );

			if ( name.length( ) - pos != strlen( kit_name ) )
				continue;

			auto weapon = item_system->GetItemSchemaInterface( )->GetItemDefentionByName( weapon_name.c_str( ) );
			if ( !weapon )
				continue;

			g_ItemManager.m_vWeapons.push_back( new WeaponItem_t {
				weapon,
				kit,
				kit->m_nDescriptionTag,
				g_ItemManager.GetSkin( g_ItemManager.GetSkinPath( weapon->m_nID, kit->m_nID ) )
			} );

			if ( weapon->m_nID == 5030 || weapon->m_nID == 5034 || weapon->m_nID == 5031 || weapon->m_nID == 5027 || weapon->m_nID == 5033 || weapon->m_nID == 5035 || weapon->m_nID == 4725 ) {
				g_ItemManager.m_vGloves.push_back( new GloveItem_t {
					weapon->m_nID,
					kit->m_nDescriptionTag,
					kit->m_nID,
					g_ItemManager.GetSkin( GetSkinPath( weapon->m_nID, kit->m_nID ) )
				} );
			}
		}
	}

	g_ItemManager.Initialized = true;
}


IDirect3DTexture9* C_ItemManager::GetSkin( std::string path )
{
	IDirect3DTexture9* skin_image = nullptr;

	void* entry;
	entry = SDK::Interfaces::FileSystem->Open( path.c_str( ), _S( "r" ), _S( "GAME" ) );

	if ( entry ) {
		int file_len = SDK::Interfaces::FileSystem->Size( entry );
		char* image = new char[ file_len ];

		SDK::Interfaces::FileSystem->Read( image, file_len, entry );
		SDK::Interfaces::FileSystem->Close( entry );

		D3DXCreateTextureFromFileInMemory( SDK::Interfaces::DirectDevice, image, file_len, &skin_image );
		delete[ ] image;
	}

	return skin_image;
}

std::string C_ItemManager::GetSkinPath( int16_t m_nItemID, int32_t m_iPaintKit )
{
	if ( !g_ItemManager.items[ m_nItemID ] )
		return "";

	if ( m_nItemID )
	{
		if ( m_iPaintKit )
		{
			if ( m_nItemID == 58 ) {
				return std::string( "resource/flash/" + std::string( g_ItemManager.MusicKits[ m_iPaintKit ]->m_szInventoryImage ) + ".png" );
			}
			else {
				return std::string( "resource/flash/econ/default_generated/" + std::string( g_ItemManager.items[ m_nItemID ]->m_szItemName ) + "_" + std::string( m_iPaintKit == 542 ? "cu_bizon_curse" : g_ItemManager.PaintKits[ m_iPaintKit ]->m_szName ) + "_light_large.png" );
			}
		}
		else if ( g_ItemManager.items[ m_nItemID ]->m_szType == "#Type_Hands" && !m_iPaintKit || m_nItemID == 58 && !m_iPaintKit )
		{
			return "";
		}
		else {
			return std::string( "resource/flash/" + std::string( g_ItemManager.items[ m_nItemID ]->m_szInventoryImage ) + ".png" );
		}
	}
}