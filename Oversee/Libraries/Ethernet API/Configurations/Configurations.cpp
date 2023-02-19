#include "Configurations.hpp"
#include "../../../Features/Inventory Changer/ItemsManager.hpp"
#include "../../../Features/Misc/Keybind V2/KeyManager.hpp"

void C_ConfigManager::ESPAction( nlohmann::json& j, bool action )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
	{
		for ( auto a = 0; a <= IN_MOVE_COND; a++ )
		{
			for ( auto b = 0; b < Pool[ a ].size( ); b++ )
			{
				nlohmann::json tempJ;

				tempJ[ _S( "A" ) ] = a;
				tempJ[ _S( "B" ) ] = b;
				tempJ[ _S( "ItemName" ) ] = Pool[ a ][ b ].ItemName;
				tempJ[ _S( "VectorCond" ) ] = Pool[ a ][ b ].VectorCond;
				tempJ[ _S( "TemporaryPos" ) ][ _S( "X" ) ] = Pool[ a ][ b ].TemporaryPos.x;
				tempJ[ _S( "TemporaryPos" ) ][ _S( "Y" ) ] = Pool[ a ][ b ].TemporaryPos.y;
				tempJ[ _S( "BasicPositions" ) ][ _S( "X" ) ] = Pool[ a ][ b ].BasicPositions.x;
				tempJ[ _S( "BasicPositions" ) ][ _S( "Y" ) ] = Pool[ a ][ b ].BasicPositions.y;
				tempJ[ _S( "Var" ) ] = Pool[ a ][ b ].Draw;

				j[ _S( "DraggableESP" ) ].push_back( tempJ );
			}
		}
	}
	else
	{
		Pool.clear( );

		std::vector<MovableItems> LEFT;
		std::vector<MovableItems> RIGHT;
		std::vector<MovableItems> TOP;
		std::vector<MovableItems> BOT;
		std::vector<MovableItems> CENTER;
		std::vector<MovableItems> POOL;
		std::vector<MovableItems> IN_MOVE;

		for ( auto a = 0; a < j[ _S( "DraggableESP" ) ].size( ); a++ )
		{
			auto DragA = j[ _S( "DraggableESP" ) ][ a ][ _S( "A" ) ].get<int>( );
			auto DragB = j[ _S( "DraggableESP" ) ][ a ][ _S( "B" ) ].get<int>( );

			if ( DragA == LEFT_COND ) {
				LEFT.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == RIGHT_COND ) {
				RIGHT.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == TOP_COND ) {
				TOP.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == BOT_COND ) {
				BOT.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == CENTER_COND ) {
				CENTER.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == POOL_COND ) {
				POOL.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
			if ( DragA == IN_MOVE_COND ) {
				IN_MOVE.push_back( MovableItems(
					j[ _S( "DraggableESP" ) ][ a ][ _S( "Var" ) ].get<int>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "ItemName" ) ].get<std::string>( ), j[ _S( "DraggableESP" ) ][ a ][ _S( "VectorCond" ) ].get<int>( ),
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "TemporaryPos" ) ][ _S( "Y" ) ].get<float>( )
					},
					{
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "X" ) ].get<float>( ),
						j[ _S( "DraggableESP" ) ][ a ][ _S( "BasicPositions" ) ][ _S( "Y" ) ].get<float>( )
					} ) );
			}
		}

		Pool.push_back( LEFT );
		Pool.push_back( RIGHT );
		Pool.push_back( TOP );
		Pool.push_back( BOT );
		Pool.push_back( CENTER );
		Pool.push_back( POOL );
		Pool.push_back( IN_MOVE );
	}
}

void C_ConfigManager::MakeAction( nlohmann::json& j, float v_default, bool action, std::string key )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
		j = v_default;
	else
		g_SettingsManager->F[ key ] = j.get<float>( );
}

void C_ConfigManager::MakeAction( nlohmann::json& j, int v_default, bool action, std::string key )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
		j = v_default;
	else
		g_SettingsManager->I[ key ] = j.get<int>( );
}

void C_ConfigManager::MakeAction( nlohmann::json& j, bool v_default, bool action, std::string key )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
		j = v_default;
	else
		g_SettingsManager->B[ key ] = j.get<bool>( );
}

void C_ConfigManager::MakeAction( nlohmann::json& j, Color v_default, bool action, std::string key )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
		j[ _S( "R" ) ] = v_default.r( ),
		j[ _S( "G" ) ] = v_default.g( ),
		j[ _S( "B" ) ] = v_default.b( ),
		j[ _S( "A" ) ] = v_default.a( );
	else
		g_SettingsManager->C[ key ].SetColor( j[ _S( "R" ) ].get<int>( ), j[ _S( "G" ) ].get<int>( ), j[ _S( "B" ) ].get<int>( ), j[ _S( "A" ) ].get<int>( ) );
}

void C_ConfigManager::MakeAction( nlohmann::json& j, ImColor v_default, bool action, std::string key )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
		j[ _S( "R" ) ] = v_default.Value.x,
		j[ _S( "G" ) ] = v_default.Value.y,
		j[ _S( "B" ) ] = v_default.Value.z,
		j[ _S( "A" ) ] = v_default.Value.w;
	else
		g_SettingsManager->IC[ key ] = ImColor( j[ _S( "R" ) ].get<float>( ), j[ _S( "G" ) ].get<float>( ), j[ _S( "B" ) ].get<float>( ), j[ _S( "A" ) ].get<float>( ) );
}

void C_ConfigManager::InventoryChanger( nlohmann::json& j, bool action )
{
	if ( j[ _S( "InventoryChanger" ) ].is_null( ) && !action )
		return;

	if ( action )
	{
		int forIterator = 0;
		for ( auto const& [key, value] : g_SettingsManager->m_aInventoryItemList )
		{
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "TTeam" ) ] = value.t_team;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "CTTeam" ) ] = value.ct_team;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "Skin Name" ) ] = value.SkinName;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_flPearlescent" ) ] = value.m_flPearlescent;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_flWear" ) ] = value.m_flWear;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_iPaintKit" ) ] = value.m_iPaintKit;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_iRarity" ) ] = value.m_iRarity;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_nItemID" ) ] = value.m_nItemID;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_StatTrack_Enabled" ) ] = value.m_StatTrack.enabled;
			j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "m_StatTrack_Counter" ) ] = value.m_StatTrack.counter;
			for ( auto a = 0; a < 4; a++ ) {
				j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "Color" ) ][ a ][ 0 ] = value.color[ a ][ 0 ];
				j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "Color" ) ][ a ][ 1 ] = value.color[ a ][ 1 ];
				j[ _S( "InventoryChanger" ) ][ forIterator ][ _S( "Color" ) ][ a ][ 2 ] = value.color[ a ][ 2 ];
			}

			forIterator += 1;
		}
	}
	else {
		for ( auto a = 0; a < j[ _S( "InventoryChanger" ) ].size( ); a++ ) {

			InventoryItem_t Temporary;
			Temporary.t_team = j[ _S( "InventoryChanger" ) ][ a ][ _S( "TTeam" ) ].get<int>( );
			Temporary.ct_team = j[ _S( "InventoryChanger" ) ][ a ][ _S( "CTTeam" ) ].get<int>( );
			Temporary.m_iRarity = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_iRarity" ) ].get<int>( );
			Temporary.SkinName = j[ _S( "InventoryChanger" ) ][ a ][ _S( "Skin Name" ) ].get<std::string>( );
			Temporary.m_nItemID = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_nItemID" ) ].get<int16_t>( );
			Temporary.m_iPaintKit = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_iPaintKit" ) ].get<int32_t>( );
			Temporary.m_flPearlescent = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_flPearlescent" ) ].get<int32_t>( );
			Temporary.m_flWear = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_flWear" ) ].get<float_t>( );
			Temporary.m_StatTrack.enabled = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_StatTrack_Enabled" ) ].get<bool>( );
			Temporary.m_StatTrack.counter = j[ _S( "InventoryChanger" ) ][ a ][ _S( "m_StatTrack_Counter" ) ].get<int32_t>( );

			for ( auto b = 0; b < 4; b++ ) {
				Temporary.color[ b ][ 0 ] = j[ _S( "InventoryChanger" ) ][ a ][ _S( "Color" ) ][ b ][ 0 ].get<float>( );
				Temporary.color[ b ][ 1 ] = j[ _S( "InventoryChanger" ) ][ a ][ _S( "Color" ) ][ b ][ 1 ].get<float>( );
				Temporary.color[ b ][ 2 ] = j[ _S( "InventoryChanger" ) ][ a ][ _S( "Color" ) ][ b ][ 2 ].get<float>( );
			}

			g_InventoryChanger->AddItemToInventory( SDK::Interfaces::InventoryManager->GetLocalPlayerInventory( ), Temporary );
		}
	}
}

void C_ConfigManager::BindsSaver( nlohmann::json& j, bool action )
{
	if ( j.is_null( ) && !action )
		return;

	if ( action )
	{
		for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
			j[ a ][ _S( "m_tActionType" ) ] = g_KeyManager->m_vKeyActions[ a ].m_tActionType;
			j[ a ][ _S( "m_tVariableType" ) ] = g_KeyManager->m_vKeyActions[ a ].m_tVariableType;
			j[ a ][ _S( "m_sVariableMapName" ) ] = g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName;
			j[ a ][ _S( "m_sRenderableName" ) ] = g_KeyManager->m_vKeyActions[ a ].m_sRenderableName;
			j[ a ][ _S( "m_uiKeyID" ) ] = g_KeyManager->m_vKeyActions[ a ].m_uiKeyID;

			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == KeyVariableType::BOOL_KEY_TYPE ) {
				j[ a ][ _S( "newValue" ) ] = std::any_cast< bool >( g_KeyManager->m_vKeyActions[ a ].newValue );
				j[ a ][ _S( "defaultValue" ) ] = std::any_cast< bool >( g_KeyManager->m_vKeyActions[ a ].defaultValue );
			}
			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == KeyVariableType::INT_KEY_TYPE ) {
				j[ a ][ _S( "newValue" ) ] = std::any_cast< int >( g_KeyManager->m_vKeyActions[ a ].newValue );
				j[ a ][ _S( "defaultValue" ) ] = std::any_cast< int >( g_KeyManager->m_vKeyActions[ a ].defaultValue );
			}
			if ( g_KeyManager->m_vKeyActions[ a ].m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE ) {
				j[ a ][ _S( "newValue" ) ] = std::any_cast< float >( g_KeyManager->m_vKeyActions[ a ].newValue );
				j[ a ][ _S( "defaultValue" ) ] = std::any_cast< float >( g_KeyManager->m_vKeyActions[ a ].defaultValue );
			}
		}
	}
	else
	{
		for ( auto a = 0; a < j.size( ); a++ ) {
			OnKeyAction_t tempValue;

			tempValue.m_tActionType = j[ a ][ _S( "m_tActionType" ) ].get<int>( );
			tempValue.m_tVariableType = j[ a ][ _S( "m_tVariableType" ) ].get<int>( );
			tempValue.m_sVariableMapName = j[ a ][ _S( "m_sVariableMapName" ) ].get<std::string>( );
			tempValue.m_sRenderableName = j[ a ][ _S( "m_sRenderableName" ) ].get<std::string>( );
			tempValue.m_uiKeyID = j[ a ][ _S( "m_uiKeyID" ) ].get<int>( );

			if ( tempValue.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE ) {
				tempValue.newValue = j[ a ][ _S( "newValue" ) ].get<bool>( );
				tempValue.defaultValue = j[ a ][ _S( "defaultValue" ) ].get<bool>( );
			}
			if ( tempValue.m_tVariableType == KeyVariableType::INT_KEY_TYPE ) {
				tempValue.newValue = j[ a ][ _S( "newValue" ) ].get<int>( );
				tempValue.defaultValue = j[ a ][ _S( "defaultValue" ) ].get<int>( );
			}
			if ( tempValue.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE ) {
				tempValue.newValue = j[ a ][ _S( "newValue" ) ].get<float>( );
				tempValue.defaultValue = j[ a ][ _S( "defaultValue" ) ].get<float>( );
			}

			g_KeyManager->RegisterKey( tempValue );
		}
	}
}


void C_ConfigManager::Action( bool save, nlohmann::json& m_jsonConfiguration )
{
	if ( g_KeyManager->m_vKeyActions.size( ) > 0 && !save )
		g_KeyManager->m_vKeyActions.clear( );

	ESPAction( m_jsonConfiguration[ _S( "ESP" ) ], save );
	BindsSaver( m_jsonConfiguration[ _S( "Binds" ) ], save );
	InventoryChanger( m_jsonConfiguration, save );

	for ( auto const& [key, value] : g_SettingsManager->B ) {
		MakeAction( m_jsonConfiguration[ _S( "Booleans" ) ][ key ], value, save, key );
	}
	for ( auto const& [key, value] : g_SettingsManager->I ) {
		MakeAction( m_jsonConfiguration[ _S( "Ints" ) ][ key ], value, save, key );
	}
	for ( auto const& [key, value] : g_SettingsManager->F ) {
		MakeAction( m_jsonConfiguration[ _S( "Floats" ) ][ key ], value, save, key );
	}
	for ( auto const& [key, value] : g_SettingsManager->C ) {
		MakeAction( m_jsonConfiguration[ _S( "Colors" ) ][ key ], value, save, key );
	}
	for ( auto const& [key, value] : g_SettingsManager->IC ) {
		MakeAction( m_jsonConfiguration[ _S( "Imgui Colors" ) ][ key ], value, save, key );
	}
}