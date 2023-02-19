#include "InventoryChanger.hpp"
#include "ItemsManager.hpp"

std::add_pointer_t<IClientNetworkable* __cdecl( int32_t, int32_t )> get_wearable_create_fn( )
{
	for ( ClientClass* pClass = SDK::Interfaces::CHLClient->GetAllClasses( ); pClass != nullptr; pClass = pClass->m_pNext )
	{
		if ( !pClass || !pClass->m_pRecvTable || FNV1A( pClass->m_strNetworkName ) != FNV1A( "CEconWearable" ) )
			continue;

		return pClass->m_pCreateFn;
	}

	return nullptr;
}

void C_InventoryChanger::SequenceRemapping( CRecvProxyData* pData, C_BaseViewmodel* pEntity ) {

	if ( !g_Globals->m_LocalPlayer || !g_Globals->m_LocalPlayer->IsAlive( ) )
		return;

	if ( !pEntity || !pEntity->m_hWeapon( ) )
		return;

	C_BasePlayer* hOwner = static_cast< C_BasePlayer* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( pEntity->m_hWeapon( )->m_nOwnerEntity( ) ) );
	if ( hOwner != g_Globals->m_LocalPlayer )
		return;

	C_BaseCombatWeapon* pViewModelWeapon = static_cast< C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( pEntity->m_hWeapon( ) ) );
	if ( !pViewModelWeapon )
		return;

	if ( FNV1A( g_ItemManager.items[ pViewModelWeapon->m_nItemID( ) ]->m_szType ) != FNV1A( "#CSGO_Type_Knife" ) )
		return;

	auto& iSequence = pData->m_Value.m_Int;
	iSequence = GetNewAnimation( FNV1A( g_ItemManager.items[ pViewModelWeapon->m_nItemID( ) ]->m_szDisplayModelName ), iSequence );
}
static int32_t random_sequence( int32_t min, int32_t max ) {
	return rand( ) % ( max - min + 1 ) + min;
}
int32_t C_InventoryChanger::GetNewAnimation( const uint32_t model, const int32_t sequence )
{
	enum e_sequence_num {
		default_draw = 0,
		default_idle1 = 1,
		default_idle2 = 2,
		default_light_miss1 = 3,
		default_light_miss2 = 4,
		default_heavy_miss1 = 9,
		default_heavy_hit1 = 10,
		default_heavy_backstab = 11,
		default_lookat01 = 12,

		butterfly_draw = 0,
		butterfly_draw2 = 1,
		butterfly_lookat01 = 13,
		butterfly_lookat03 = 15,

		falchion_idle1 = 1,
		falchion_heavy_miss1 = 8,
		falchion_heavy_miss1_noflip = 9,
		falchion_lookat01 = 12,
		falchion_lookat02 = 13,

		daggers_idle1 = 1,
		daggers_light_miss1 = 2,
		daggers_light_miss5 = 6,
		daggers_heavy_miss2 = 11,
		daggers_heavy_miss1 = 12,

		bowie_idle1 = 1,
	};

	switch ( model ) {
		case FNV1A( "models/weapons/v_knife_butterfly.mdl" ): {
			switch ( sequence )
			{
				case e_sequence_num::default_draw:
				return random_sequence( e_sequence_num::butterfly_draw, e_sequence_num::butterfly_draw2 );
				case e_sequence_num::default_lookat01:
				return random_sequence( e_sequence_num::butterfly_lookat01, e_sequence_num::butterfly_lookat03 );
				default:
				return sequence + 1;
			}
		}
		case FNV1A( "models/weapons/v_knife_falchion_advanced.mdl" ): {
			switch ( sequence ) {
				case e_sequence_num::default_idle2:
				return e_sequence_num::falchion_idle1;
				case e_sequence_num::default_heavy_miss1:
				return random_sequence( e_sequence_num::falchion_heavy_miss1, e_sequence_num::falchion_heavy_miss1_noflip );
				case e_sequence_num::default_lookat01:
				return random_sequence( e_sequence_num::falchion_lookat01, e_sequence_num::falchion_lookat02 );
				case e_sequence_num::default_draw:
				case e_sequence_num::default_idle1:
				return sequence;
				default:
				return sequence - 1;
			}
		}
		case FNV1A( "models/weapons/v_knife_push.mdl" ): {
			switch ( sequence ) {
				case e_sequence_num::default_idle2:
				return e_sequence_num::daggers_idle1;
				case e_sequence_num::default_light_miss1:
				case e_sequence_num::default_light_miss2:
				return random_sequence( e_sequence_num::daggers_light_miss1, e_sequence_num::daggers_light_miss5 );
				case e_sequence_num::default_heavy_miss1:
				return random_sequence( e_sequence_num::daggers_heavy_miss2, e_sequence_num::daggers_heavy_miss1 );
				case e_sequence_num::default_heavy_hit1:
				case e_sequence_num::default_heavy_backstab:
				case e_sequence_num::default_lookat01:
				return sequence + 3;
				case e_sequence_num::default_draw:
				case e_sequence_num::default_idle1:
				return sequence;
				default:
				return sequence + 2;
			}
		}
		case FNV1A( "models/weapons/v_knife_survival_bowie.mdl" ): {
			switch ( sequence )
			{
				case e_sequence_num::default_draw:
				case e_sequence_num::default_idle1:
				return sequence;
				case e_sequence_num::default_idle2:
				return e_sequence_num::bowie_idle1;
				default:
				return sequence - 1;
			}
		}
		case FNV1A( "models/weapons/v_knife_ursus.mdl" ):
		case FNV1A( "models/weapons/v_knife_cord.mdl" ):
		case FNV1A( "models/weapons/v_knife_canis.mdl" ):
		case FNV1A( "models/weapons/v_knife_outdoor.mdl" ):
		case FNV1A( "models/weapons/v_knife_skeleton.mdl" ): {
			switch ( sequence ) {
				case e_sequence_num::default_draw:
				return random_sequence( e_sequence_num::butterfly_draw, e_sequence_num::butterfly_draw2 );
				case e_sequence_num::default_lookat01:
				return random_sequence( e_sequence_num::butterfly_lookat01, 14 );
				default:
				return sequence + 1;
			}
		}
		case FNV1A( "models/weapons/v_knife_stiletto.mdl" ): {
			switch ( sequence ) {
				case e_sequence_num::default_lookat01:
				return random_sequence( 12, 13 );
				default:
				return sequence;
			}
		}
		case FNV1A( "models/weapons/v_knife_widowmaker.mdl" ): {
			switch ( sequence ) {
				case e_sequence_num::default_lookat01:
				return random_sequence( 14, 15 );
				default:
				return sequence;
			}
		}
		default:
		return sequence;
	}
}
void C_InventoryChanger::ApplyGlove( IPlayerInventory* LocalInventory, PlayerInfo_t info )
{
	C_EconItemView* pEconItemView = LocalInventory->GetItemInLoadout( static_cast< int32_t >( g_Globals->m_LocalPlayer->m_iTeamNum( ) ), 41 );
	if ( !pEconItemView )
		return;

	C_EconItem* pSocData = pEconItemView->GetSocData( );
	if ( !pSocData )
		return;

	C_EconItemDefinition* pStaticData = pEconItemView->GetStaticData( );
	if ( !pStaticData )
		return;

	CBaseHandle* hMyWearables = g_Globals->m_LocalPlayer->m_hMyWearables( );
	if ( !hMyWearables )
		return;

	constexpr std::uint64_t iMask = ( std::numeric_limits<std::uint32_t>::max )( );
	const std::uint32_t iLowID = iMask & pSocData->m_nItemID;
	const std::uint32_t iHighID = pSocData->m_nItemID >> 32;

	C_BaseCombatWeapon* pWearable = static_cast< C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( hMyWearables[ 0 ] ) );
	if ( !pWearable || static_cast< std::uint32_t >( pWearable->m_iItemIDHigh( ) ) != iHighID || static_cast< std::uint32_t >( pWearable->m_iItemIDLow( ) ) != iLowID )
	{
		int Entry = SDK::Interfaces::EntityList->GetHighestEntityIndex( ) + 1;
		for ( int nEntityID = 0; nEntityID < Entry - 1; nEntityID++ )
		{
			auto pEntity = SDK::Interfaces::EntityList->GetClientEntity( nEntityID );
			if ( !pEntity || pEntity->GetClientClass( )->m_ClassID != 149 )
				continue;

			Entry = nEntityID;
			break;
		}

		const int nSerialID = rand( ) % 0x1000;
		const auto CreateWearableFn = get_wearable_create_fn( ); 
		CreateWearableFn( Entry, nSerialID );

		pWearable = reinterpret_cast< C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntity( Entry ) );
		if ( pWearable )
		{
			hMyWearables[ 0 ] = Entry | nSerialID << 16;
			
			pWearable->m_iItemIDLow( ) = iLowID;
			pWearable->m_iItemIDHigh( ) = iHighID;
			pWearable->m_nItemID( ) = pSocData->m_nWeaponID;
			pWearable->m_iAccountID( ) = static_cast< int32_t >( info.m_iSteamID64 );
			pWearable->m_bInitialized( ) = true;
			pWearable->SetModelIndex( SDK::Interfaces::ModelInfo->GetModelIndex( pStaticData->m_szWorldModelName ) );
			pWearable->EquipWearable( g_Globals->m_LocalPlayer );
			g_Globals->m_LocalPlayer->m_nBody( ) = 1;
			pWearable->InitializeAttibutes( );
		}
	}

}
void C_InventoryChanger::ApplyWeapons( IPlayerInventory* LocalInventory, PlayerInfo_t Info )
{
	CBaseHandle* hMyWeapons = g_Globals->m_LocalPlayer->m_hMyWeapons( );
	if ( !hMyWeapons )
		return;

	for ( auto iIndex = 0; hMyWeapons[ iIndex ] != INVALID_EHANDLE_INDEX; iIndex++ ) 
	{
		C_BaseCombatWeapon* pWeapon = static_cast < C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( hMyWeapons[ iIndex ] ) );
		if ( !pWeapon || FNV1A( g_ItemManager.items[ pWeapon->m_nItemID( ) ]->m_szType ) == FNV1A( "#CSGO_Type_Knife" ) )
			continue;

		if ( Info.m_iXuidHigh != pWeapon->m_OriginalOwnerXuidHigh( ) || Info.m_iXuidLow != pWeapon->m_OriginalOwnerXuidLow( ) )
			continue;

		C_EconItemView* pEconItemView = LocalInventory->GetItemInLoadout( static_cast< int32_t >( g_Globals->m_LocalPlayer->m_iTeamNum( ) ), g_ItemManager.items[ pWeapon->m_nItemID( ) ]->m_nLoadoutSlot );
		if ( !pEconItemView )
			continue;
		
		C_EconItem* pSocData = pEconItemView->GetSocData( );
		if ( !pSocData )
			continue;

		if ( pWeapon->m_nItemID( ) != pSocData->m_nWeaponID )
			continue;

		constexpr std::uint64_t iMask = ( std::numeric_limits<std::uint32_t>::max )( );
		const std::uint32_t iLowID = iMask & pSocData->m_nItemID;
		const std::uint32_t iHighID = pSocData->m_nItemID >> 32;

		pWeapon->m_iAccountID( ) = static_cast< int32_t >( Info.m_iSteamID64 );
		pWeapon->m_iItemIDLow( ) = iLowID;
		pWeapon->m_iItemIDHigh( ) = iHighID;
	}

}
void C_InventoryChanger::ApplyKnifes( IPlayerInventory* LocalInventory, PlayerInfo_t info )
{
	C_EconItemView* pEconItemView = LocalInventory->GetItemInLoadout( static_cast < int >( g_Globals->m_LocalPlayer->m_iTeamNum( ) ), 0 );
	if ( !pEconItemView )
		return;

	C_EconItem* pSocData = pEconItemView->GetSocData( );
	if ( !pSocData )
		return;

	CBaseHandle* m_hMyWeapons = g_Globals->m_LocalPlayer->m_hMyWeapons( );
	if ( !m_hMyWeapons )
		return;

	for ( auto iIndex = 0; m_hMyWeapons[ iIndex ] != INVALID_EHANDLE_INDEX; iIndex++ ) 
	{
		C_BaseCombatWeapon* pWeapon = static_cast< C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( m_hMyWeapons[ iIndex ] ) );
		if ( !pWeapon || FNV1A( g_ItemManager.items[ pWeapon->m_nItemID( ) ]->m_szType ) != FNV1A( "#CSGO_Type_Knife" ) )
			continue;

		if ( info.m_iXuidLow != pWeapon->m_OriginalOwnerXuidLow( ) || info.m_iXuidHigh != pWeapon->m_OriginalOwnerXuidHigh( ) )
			continue;

		constexpr std::uint64_t iMask = ( std::numeric_limits<std::uint32_t>::max )( );
		const std::uint32_t iLowID = iMask & pSocData->m_nItemID;
		const std::uint32_t iHighID = pSocData->m_nItemID >> 32;

		pWeapon->m_iItemIDLow( ) = iLowID;
		pWeapon->m_iItemIDHigh( ) = iHighID;
		pWeapon->m_iAccountID( ) = static_cast< int32_t >( info.m_iSteamID );
		pWeapon->m_nItemID( ) = pSocData->m_nWeaponID;
		pWeapon->SetModelIndex( SDK::Interfaces::ModelInfo->GetModelIndex( g_ItemManager.items[ pSocData->m_nWeaponID ]->m_szDisplayModelName ) );
	}

	/* Get viewmodel */
	C_BaseViewmodel* pViewModel = g_Globals->m_LocalPlayer->m_hViewModel( );
	if ( !pViewModel )
		return;

	/* Get viewmodel weapon */
	C_BaseCombatWeapon* pViewModelWeapon = pViewModel->m_hWeapon( );
	if ( !pViewModelWeapon )
		return;

	/* Set world model index */
	C_BaseCombatWeapon* pWorldModel = reinterpret_cast< C_BaseCombatWeapon* >( SDK::Interfaces::EntityList->GetClientEntityFromHandle( pViewModelWeapon->m_hWeaponWorldModel( ) ) );
	if ( pWorldModel )
		pWorldModel->m_nModelIndex( ) = SDK::Interfaces::ModelInfo->GetModelIndex( g_ItemManager.items[ pViewModelWeapon->m_nItemID( ) ]->m_szWorldModelName );

	/* Set viewmodel weapon index */
	pViewModel->m_nModelIndex( ) = SDK::Interfaces::ModelInfo->GetModelIndex( g_ItemManager.items[ pViewModelWeapon->m_nItemID( ) ]->m_szDisplayModelName );
}
void C_InventoryChanger::ApplyMedals( IPlayerInventory* LocalInventory )
{
	IPlayerResource* pPlayerResource = *SDK::Interfaces::PlayerResource;
	if ( !pPlayerResource )
		return;

	C_EconItemView* pEconItemView = LocalInventory->GetItemInLoadout( NULL, 55 );
	if ( !pEconItemView )
		return;

	pPlayerResource->m_nActiveCoinRank( g_Globals->m_LocalPlayer->EntIndex( ) ) = pEconItemView->GetSocData( ) ? pEconItemView->GetSocData( )->m_nWeaponID : 0;
}
void C_InventoryChanger::ApplyMusic( IPlayerInventory* LocalInventory )
{
	const auto pPlayerResource = *SDK::Interfaces::PlayerResource;

	if ( !pPlayerResource )
		return;

	const auto pEconItemView = LocalInventory->GetItemInLoadout( NULL, 54 );

	if ( !pEconItemView )
		return;

	pPlayerResource->m_nMusicID( g_Globals->m_LocalPlayer->EntIndex( ) ) = pEconItemView->GetSocData( ) ? pEconItemView->GetSocData( )->GetAttributes( )[ 0 ].m_szData[ 0 ] : 0;
}
void C_InventoryChanger::ApplyAgents( IPlayerInventory* LocalInventory, ClientFrameStage_t Stage )
{
	C_EconItemView* m_LoadOut = LocalInventory->GetItemInLoadout( static_cast< int32_t >( g_Globals->m_LocalPlayer->m_iTeamNum( ) ), 38 );
	if ( !m_LoadOut )
		return;

	C_EconItem* m_Item = m_LoadOut->GetSocData( );
	if ( !m_Item )
		return;

	C_EconItemDefinition* pDefIndex = g_ItemManager.items[ m_Item->m_nWeaponID ];
	if ( !pDefIndex || FNV1A( pDefIndex->m_szType ) != FNV1A( "#Type_CustomPlayer" ) )
		return;

	const char* m_szModel = pDefIndex->m_szDisplayModelName;
	if ( !m_szModel )
		return;

	int nModelIdx = SDK::Interfaces::ModelInfo->GetModelIndex( m_szModel );
	if ( !nModelIdx )
		return;

	g_Globals->m_LocalPlayer->SetModelIndex( nModelIdx );
	if ( C_BaseCombatWeapon* pRagdoll = static_cast < C_BaseCombatWeapon* > ( SDK::Interfaces::EntityList->GetClientEntityFromHandle( g_Globals->m_LocalPlayer->m_hRagdoll( ) ) ) )
		pRagdoll->SetModelIndex( nModelIdx );

	m_nModelIdx = nModelIdx;
}
void C_InventoryChanger::AddItemToInventory( IPlayerInventory* pLocalInventory, InventoryItem_t item, bool bAcKnowledge, std::string szParameter )
{
	auto pItem = C_EconItem::CreateEconItem( );

	pItem->m_nItemID = pLocalInventory->GetLastValueID( ) + iUniqueID++;
	pItem->m_nOriginalID = 0;
	pItem->m_nAccountID = static_cast< uint32_t >( pLocalInventory->GetOwnerID( ).m_nID );
	pItem->m_nInventory = bAcKnowledge ? 0 : 1;
	pItem->m_nQuality = 4;
	pItem->m_nWeaponID = item.m_nItemID;
	
	const char* szType = g_ItemManager.items[ item.m_nItemID ]->m_szType;
	if ( szType == _S( "#CSGO_Type_MusicKit" ) ) 
	{
		pItem->m_nRarity = 3;
		pItem->SetMusicID( item.m_iPaintKit );

		if ( item.m_StatTrack.enabled ) {
			pItem->SetStatTrack( item.m_StatTrack.counter );
			pItem->SetStatTrackType( 1 );
			pItem->m_nQuality = 9;
		}
	}
	else {
		pItem->SetPaintKit( static_cast< float_t >( item.m_iPaintKit ) );
		pItem->SetWear( item.m_flWear );

		for ( std::size_t j = 0; j < item.m_aStickers.size( ); ++j ) {
			const auto& sticker = item.m_aStickers[ j ];

			if ( sticker.m_nID == 0 )
				continue;

			pItem->SetStickerID( j, sticker.m_nID );
			pItem->SetStickerWear( j, sticker.m_flWear );
		}
		
		if ( item.m_StatTrack.enabled && szType != _S( "#Type_Hands" ) ) {
			pItem->SetStatTrack( item.m_StatTrack.counter );
			pItem->SetStatTrackType( 0 );
			pItem->m_nQuality = 9;
		}

		if ( FNV1A( szType ) == FNV1A( "#CSGO_Type_Knife" ) || FNV1A( szType ) == FNV1A( "#Type_Hands" ) )
			pItem->m_nQuality = 3;

		if ( item.m_iPaintKit )
			item.m_iRarity = std::clamp( g_ItemManager.items[ item.m_nItemID ]->GetRarityValue( ) + g_ItemManager.PaintKits[ item.m_iPaintKit ]->m_nRarity - 1, 0, ( g_ItemManager.PaintKits[ item.m_iPaintKit ]->m_nRarity == 7 ) ? 7 : 6 );
		else
			item.m_iRarity = g_ItemManager.items[ item.m_nItemID ]->GetRarityValue( );

		pItem->m_nRarity = item.m_iRarity;

	}

	pLocalInventory->GetItemBaseTypeCache( )->AddObject( pItem );
	pLocalInventory->AddEconItem( pItem, false, false, false );

	if ( const auto pView = pLocalInventory->GetInventoryItemByItemID( pItem->m_nItemID ) )
		pView->GetClearInventoryImageRGBA( );

	g_SettingsManager->m_aInventoryItemList.emplace( pItem->m_nItemID, item );

	if ( item.ct_team )
		SDK::Interfaces::InventoryManager->EquipItemInLoadout( 3, g_ItemManager.items[ item.m_nItemID ]->m_nLoadoutSlot, pItem->m_nItemID, false );

	if ( item.t_team )
		SDK::Interfaces::InventoryManager->EquipItemInLoadout( 2, g_ItemManager.items[ item.m_nItemID ]->m_nLoadoutSlot, pItem->m_nItemID, false );

	if ( item.none_team )
		SDK::Interfaces::InventoryManager->EquipItemInLoadout( 0, g_ItemManager.items[ item.m_nItemID ]->m_nLoadoutSlot, pItem->m_nItemID, false );

	g_Panorama.RunScript( std::string( "$.DispatchEvent( 'PanoramaComponent_Inventory_ItemCustomizationNotification', '0','" + szParameter + _S( "','" ) + std::to_string( pItem->m_nItemID ) + _S( "');" ) ).c_str( ) );
}
void C_InventoryChanger::OnPostDataUpdateStart( IPlayerInventory* LocalInventory )
{
	if ( !g_Globals->m_LocalPlayer->IsAlive( ) || SDK::Interfaces::ClientState->m_nSignonState < 6 )
		return;

	PlayerInfo_t Info;
	if ( !SDK::Interfaces::EngineClient->GetPlayerInfo( g_Globals->m_LocalPlayer->EntIndex( ), &Info ) )
		return;

	ApplyWeapons( LocalInventory, Info );
	ApplyKnifes( LocalInventory, Info );
	ApplyGlove( LocalInventory, Info );
}
void C_InventoryChanger::OnSoUpdated( C_SharedObject* object, int32_t event )
{
	if ( iLastEquipped <= 0 && object->GetTypeID( ) != 0x2B )
		return;

	*reinterpret_cast< int16_t* >( std::uintptr_t( object ) + 0x10 ) = 0;
	--iLastEquipped;
}
void C_InventoryChanger::OnEquipItemInLoadOut( int32_t iTeam, int32_t iSlot, uint64_t iItemID )
{
	const auto pLocalInventory = SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( );
	if ( !pLocalInventory )
		return;

	const auto& inventory_item_it = g_SettingsManager->m_aInventoryItemList.find( iItemID );
	if ( inventory_item_it == g_SettingsManager->m_aInventoryItemList.end( ) )
		return;

	if ( FNV1A( g_ItemManager.items[ inventory_item_it->second.m_nItemID ]->m_szType ) == FNV1A( "#CSGO_Type_Collectible" ) ) 
	{
		if ( const auto pView = pLocalInventory->GetInventoryItemByItemID( iItemID ) ) 
		{
			if ( const auto pEconitem = pView->GetSocData( ) )
				pLocalInventory->SoUpdated( pLocalInventory->GetOwnerID( ), ( C_SharedObject* ) pEconitem, 0x4 );
		}
	}
	else 
	{
		SDK::Interfaces::InventoryManager->EquipItemInLoadout( iTeam, iSlot, ( uint64_t( 0xF ) << 0x3C ) | static_cast< int16_t >( inventory_item_it->second.m_nItemID ), false );

		C_EconItemView* pView = pLocalInventory->GetItemInLoadout( iTeam, iSlot );
		if ( pView ) 
		{
			if ( C_EconItem* m_Item = pView->GetSocData( ) )
				pLocalInventory->SoUpdated( pLocalInventory->GetOwnerID( ), ( C_SharedObject* ) m_Item, 0x4 );
		}

		++iLastEquipped;
	}
}
void C_InventoryChanger::OnFrameStageNotify( ClientFrameStage_t Stage )
{
	IPlayerInventory* pLocalInventory = SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( );
	if ( !pLocalInventory )
		return;

	switch ( Stage )
	{
		case ClientFrameStage_t::FRAME_RENDER_START:
		{
			g_InventoryChanger->ApplyMedals( pLocalInventory );
			g_InventoryChanger->ApplyMusic( pLocalInventory );
			g_InventoryChanger->ApplyAgents( pLocalInventory, Stage );
		}
		break;

		case ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			g_InventoryChanger->OnPostDataUpdateStart( pLocalInventory );
		}
		break;
		case ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			g_InventoryChanger->ApplyAgents( pLocalInventory, Stage );
		}
		break;

		default: break;
	}

	if ( g_Globals->m_Inventory.m_bNeedFullUpdate )
	{
		SDK::Interfaces::ClientState->ForceFullUpdate( );
		g_Globals->m_Inventory.m_bNeedFullUpdate = false;
	}
}
void C_InventoryChanger::OnEvent( IGameEvent* m_GameEvent )
{
	IPlayerInventory* pLocalInventory = SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( );
	if ( !pLocalInventory )
		return;

	const uint32_t& uHash = FNV1A( m_GameEvent->GetName( ) );
	switch ( uHash )
	{
		case FNV1A( "player_death" ):
		{
			g_InventoryChanger->ChangeHudIcons( pLocalInventory, m_GameEvent );
			g_InventoryChanger->LiveStatTrack( pLocalInventory, m_GameEvent );
		}
		break;

		case FNV1A( "round_mvp" ):
		{
			/* Check for LocalPlayer */
			if ( SDK::Interfaces::EngineClient->GetLocalPlayer( ) != SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "userid" ) ) ) )
				return;

			C_EconItemView* pEconItemView = pLocalInventory->GetItemInLoadout( NULL, 54 );
			if ( !pEconItemView )
				return;

			C_EconItem* pSocData = pEconItemView->GetSocData( );
			if ( !pSocData )
				return;

			auto aInventoryItem = g_SettingsManager->m_aInventoryItemList.find(  pSocData->m_nItemID );
			if ( aInventoryItem == g_SettingsManager->m_aInventoryItemList.end( ) )
				return;

			auto& pInventoryItem = aInventoryItem->second;
			if ( !pInventoryItem.m_StatTrack.enabled )
				return;

			m_GameEvent->SetInt( _S( "musickitmvps" ), ++pInventoryItem.m_StatTrack.counter );
			pSocData->SetStatTrack( pInventoryItem.m_StatTrack.counter );
			pLocalInventory->SoUpdated( pLocalInventory->GetOwnerID( ), ( C_SharedObject* ) pSocData, 4 );
		}
		break;
	}
}
void C_InventoryChanger::ChangeHudIcons( IPlayerInventory* LocalInventory, IGameEvent* m_GameEvent )
{
	if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "attacker" ) ) ) != SDK::Interfaces::EngineClient->GetLocalPlayer( ) )
		return;

	std::string_view szWeapon = std::string_view { m_GameEvent->GetString( _S( "weapon" ) ) };
	if ( szWeapon != _S( "knife" ) && szWeapon != _S( "knife_t" ) )
		return;

	C_EconItemView* pEconItemView = LocalInventory->GetItemInLoadout( static_cast< int32_t >( g_Globals->m_LocalPlayer->m_iTeamNum( ) ), NULL );
	if ( !pEconItemView )
		return;

	C_EconItem* pSocData = pEconItemView->GetSocData( );
	if ( !pSocData )
		return;
	
	C_EconItemDefinition* pDefinition = g_ItemManager.items[ pSocData->m_nWeaponID ];
	if ( !pDefinition )
		return;
	
	const char* szWeaponName = pDefinition->m_szItemName;
	if ( std::string_view { ( std::string )( szWeaponName ) }._Starts_with( _S( "weapon_" ) ) )
		m_GameEvent->SetWString( _S( "weapon" ), ( const char* )( szWeaponName + 7 ) );
}
void C_InventoryChanger::LiveStatTrack( IPlayerInventory* LocalInventory, IGameEvent* m_GameEvent )
{
	if ( SDK::Interfaces::EngineClient->GetPlayerForUserID( m_GameEvent->GetInt( _S( "attacker" ) ) ) != SDK::Interfaces::EngineClient->GetLocalPlayer( ) )
		return;

	C_BaseCombatWeapon* pWeapon = static_cast< C_BaseCombatWeapon* > ( SDK::Interfaces::EntityList->GetClientEntityFromHandle( g_Globals->m_LocalPlayer->m_hActiveWeapon( ) ) );
	if ( !pWeapon )
		return;

	std::uint64_t iItemID = ( std::uint64_t( pWeapon->m_iItemIDHigh( ) ) << 32 ) | pWeapon->m_iItemIDLow( );
	if ( !iItemID )
		return;

	C_EconItemView* pEconItemView = LocalInventory->GetInventoryItemByItemID( iItemID );
	if ( !pEconItemView )
		return;

	C_EconItem* pSocData = pEconItemView->GetSocData( );
	if ( !pSocData )
		return;

	auto aInventoryItems = g_SettingsManager->m_aInventoryItemList.find( iItemID );
	if ( aInventoryItems == g_SettingsManager->m_aInventoryItemList.end( ) )
		return;

	auto& pInventoryItem = aInventoryItems->second;
	if ( !pInventoryItem.m_StatTrack.enabled )
		return;

	++pInventoryItem.m_StatTrack.counter;
	pSocData->SetStatTrack( pInventoryItem.m_StatTrack.counter );
	LocalInventory->SoUpdated( LocalInventory->GetOwnerID( ), ( C_SharedObject* ) pSocData, 4 );
}