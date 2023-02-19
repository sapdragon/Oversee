#include "Chams.hpp"
#include "../../../Hooks/Hooks.hpp"
#include "../../Game/RageBot/LocalAnimations.hpp"
#include "../../Game/RageBot/LagCompensation.hpp"

void C_ChamsSystem::Instance( IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	if ( strstr( pInfo.pModel->szName, _S( "player" ) ) )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( pInfo.entity_index );
		if ( !pPlayer || !pPlayer->IsPlayer( ) )
			return;

		if ( pPlayer == g_Globals->m_LocalPlayer )
			this->HandlingLocalPlayer( pCtx, pState, pInfo, aMatrix );
		else if ( pPlayer->m_iTeamNum( ) == g_Globals->m_LocalPlayer->m_iTeamNum( ) )
			this->HandlingTeamPlayer( pPlayer, pCtx, pState, pInfo, aMatrix );
		else
			this->HandlingEnemyPlayer( pPlayer, pCtx, pState, pInfo, aMatrix );
	}
	else if ( strstr( pInfo.pModel->szName, _S( "weapons/v_" ) ) && !strstr( pInfo.pModel->szName, _S( "arms" ) ) )
	{
		C_BaseCombatWeapon* pWeapon = ( C_BaseCombatWeapon* ) SDK::Interfaces::EntityList->GetClientEntity( pInfo.entity_index );
		if ( !pWeapon )
			return;

		this->HandlingLocalWeapon( pWeapon, pCtx, pState, pInfo, aMatrix );
	}

	else if ( strstr( pInfo.pModel->szName, _S( "weapons/w_" ) ) )
	{
		C_BaseCombatWeapon* pWeapon = ( C_BaseCombatWeapon* ) SDK::Interfaces::EntityList->GetClientEntity( pInfo.entity_index );
		if ( !pWeapon )
			return;

		this->HandlingDroppedWeapons( pWeapon, pCtx, pState, pInfo, aMatrix );
	}
}

void C_ChamsSystem::InitializeMaterials( )
{
	// Get Material from game
	this->m_aMaterials.at( REGULAR ) = SDK::Interfaces::MaterialSystem->FindMaterial( _S( "debug/debugambientcube" ) );
	this->m_aMaterials.at( GOLD ) = SDK::Interfaces::MaterialSystem->FindMaterial( _S( "models/inventory_items/wildfire_gold/wildfire_gold_detail" ) );
	this->m_aMaterials.at( GHOST ) = SDK::Interfaces::MaterialSystem->FindMaterial( _S( "dev/glow_armsrace" ), _S( TEXTURE_GROUP_OTHER ) );;
	this->m_aMaterials.at( GLASS ) = SDK::Interfaces::MaterialSystem->FindMaterial( _S( "models/inventory_items/trophy_majors/gloss" ), _S( TEXTURE_GROUP_MODEL ) );
	this->m_aMaterials.at( PULSE ) = SDK::Interfaces::MaterialSystem->FindMaterial( _S( "models/inventory_items/dogtags/dogtags_outline" ), _S( TEXTURE_GROUP_MODEL ) );;

	// Creating materials
	this->m_aMaterials.at( FLAT ) = this->CreateMaterial( _S( "Flat_Material_oversee" ), _S( "UnlitGeneric { }" ) );
	this->m_aMaterials.at( GLOW ) = this->CreateMaterial( _S( "Glow_Material_Oversee" ),
		_S(
		
			R"#( "VertexLitGeneric" 
			{
			  "$additive" "1"
			  "$envmap" "models/effects/cube_white"
			  "$envmaptint" "[1 1 1]"
			  "$envmapfresnel" "1"
			  "$envmapfresnelminmaxexp" "[0 1 2]"
			})#"
		
		)
	);
	this->m_aMaterials.at( SKY ) = this->CreateMaterial( _S( "Panorama_Material_Oversee" ), 
		_S(

		R"#(unlittwotexture
			{
				"$moondome"   "1"
				"$basetexture" "models\oversee\alpha"
				"$cubeparallax" "0.00005"
				"$texture2"    "models\oversee\sky"
			})#" 

		)
	);
}


void C_ChamsSystem::HandlingLocalPlayer( IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	bool bScoped = false;
	if ( g_Globals->m_LocalPlayer->m_bIsScoped( ) && SDK::Interfaces::Input->m_bCameraInThirdPerson )
		bScoped = true;

	// Set render local player alpha
	if ( bScoped )
		SDK::Interfaces::RenderView->SetBlend( g_SettingsManager->I[ _S( "misc.transparent_in_tperson" ) ] / 100.f );

	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "desync" ) ), pCtx, pState, pInfo, g_LocalAnimations->GetDesyncMatrix( ).data( ), true, false, bScoped );
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "real" ) ), pCtx, pState, pInfo, aMatrix, false, false, bScoped );
}

void C_ChamsSystem::HandlingTeamPlayer( C_BasePlayer* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	// Validate player
	if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) != g_Globals->m_LocalPlayer->m_iTeamNum( ) )
	{
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
		return SDK::Interfaces::ModelRender->ForcedMaterialOverride( nullptr );
	}

	// Getting shared data
	const auto SharedData = m_aSharedChamsData.at( pPlayer->EntIndex( ) );

	// Validate data
	if ( SharedData.m_iMainMaterial )
	{
		// Draw shared material to invisible chams type
		this->DrawSharedMateraisOnEntity( SharedData, pCtx, pState, pInfo, aMatrix, true, true );

		// Draw shared material for visible chams type
		this->DrawSharedMateraisOnEntity( SharedData, pCtx, pState, pInfo, aMatrix, false, false );

		return;
	}


	// Draw materials from settings to entity
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "team_invisible" ) ), pCtx, pState, pInfo, aMatrix, true, true );
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "team_visible" ) ), pCtx, pState, pInfo, aMatrix, false, false );
}

void C_ChamsSystem::HandlingEnemyPlayer( C_BasePlayer* pPlayer, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	// Validate player
	if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) )
	{
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
		return SDK::Interfaces::ModelRender->ForcedMaterialOverride( nullptr );
	}

	// Getting shared data
	const auto SharedData = m_aSharedChamsData.at( pPlayer->EntIndex( ) );

	// Validate data
	if ( SharedData.m_iMainMaterial )
	{
		// Draw shared material to invisible chams type
		this->DrawSharedMateraisOnEntity( SharedData, pCtx, pState, pInfo, aMatrix, true, true );

		// Draw shared material for visible chams type
		this->DrawSharedMateraisOnEntity( SharedData, pCtx, pState, pInfo, aMatrix, false, false );

		return;
	}

	std::array < matrix3x4_t, MAXSTUDIOBONES > aBacktrackMatrix;
	if ( g_LagCompensation->GetBacktrackMatrix( pPlayer, aBacktrackMatrix.data( ) ) )
		if ( pPlayer->m_vecVelocity( ).Length2D( ) > 5.0f )
			this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "enemy_backtrack" ) ), pCtx, pState, pInfo, aBacktrackMatrix.data( ), true, true );

	// Draw materials from settings to entity
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "enemy_invisible" ) ), pCtx, pState, pInfo, aMatrix, true, true );
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "enemy_visible" ) ), pCtx, pState, pInfo, aMatrix, false, false );
}

void C_ChamsSystem::HandlingLocalWeapon( C_BaseCombatWeapon* pWeapon, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	if (!pWeapon ) 
		return;

	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "local_weapon" ) ), pCtx, pState, pInfo, aMatrix, true, true );
}

void C_ChamsSystem::HandlingDroppedWeapons( C_BaseCombatWeapon* pWeapon, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	// validate weapon
	if ( !pWeapon )
		return;

	// draw materials from settings
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "weapon_invisible" ) ), pCtx, pState, pInfo, aMatrix, false, false );
	this->DrawMaterialsOnEntity( g_SettingsManager->GetChamsSettings( _S( "weapon_visible" ) ), pCtx, pState, pInfo, aMatrix, true, true );
}

void C_ChamsSystem::DrawMaterialsOnEntity( ChamsSettings_t Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ, bool bLocalScoped )
{
	if ( !Settings.m_bDrawChams )
		return;

	IMaterial* pMaterial = nullptr;

	switch ( Settings.m_iFirstChamsLayerStyle )
	{

		case FLAT:
		pMaterial = this->m_aMaterials.at( FLAT );
		break;
		case REGULAR:
		pMaterial = this->m_aMaterials.at( REGULAR );
		break;

		case SKY:
		pMaterial = this->m_aMaterials.at( SKY );
		break;
	}
	
	if ( !pMaterial )
		return;
	
	const float flScopeTransparency = g_SettingsManager->I[ _S( "misc.transparent_in_tperson" ) ] / 100.f;
	auto GetTransparency = [ & ]( float flOriginalAlpha ) -> float
	{
		if ( !bLocalScoped )
			return flOriginalAlpha;

		return fmin( flOriginalAlpha, flScopeTransparency * 255.0f );
	};

	Color cMainColor = Settings.m_Color;
	this->PullMaterial( pMaterial, Color( cMainColor.r( ), cMainColor.g( ), cMainColor.b( ), ( int ) ( GetTransparency( cMainColor.a( ) ) ) ), bXQZ, pMaterial == this->m_aMaterials.at( SKY ) );

	Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	if ( Settings.m_aMaterialParams[ 0 ] )
	{
		pMaterial = this->m_aMaterials.at( GOLD );
		this->PullMaterial( pMaterial, Color( Settings.m_aMaterialsColor[ 0 ].r( ), Settings.m_aMaterialsColor[ 0 ].g( ), Settings.m_aMaterialsColor[ 0 ].b( ), ( int ) ( GetTransparency( Settings.m_aMaterialsColor[ 0 ].a( ) ) ) ), bXQZ );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( Settings.m_aMaterialParams[ 1 ] )
	{
		pMaterial = this->m_aMaterials.at( GLOW );
		this->PullMaterial( pMaterial, Color( Settings.m_aMaterialsColor[ 1 ].r( ), Settings.m_aMaterialsColor[ 1 ].g( ), Settings.m_aMaterialsColor[ 1 ].b( ), ( int ) ( GetTransparency( Settings.m_aMaterialsColor[ 1 ].a( ) ) ) ), bXQZ );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( Settings.m_aMaterialParams[ 2 ] )
	{
		pMaterial = this->m_aMaterials.at( GHOST );
		this->PullMaterial( pMaterial, Color( Settings.m_aMaterialsColor[ 2 ].r( ), Settings.m_aMaterialsColor[ 2 ].g( ), Settings.m_aMaterialsColor[ 2 ].b( ), ( int ) ( GetTransparency( Settings.m_aMaterialsColor[ 2 ].a( ) ) ) ), bXQZ );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( Settings.m_aMaterialParams[ 3 ] )
	{
		pMaterial = this->m_aMaterials.at( GLASS );
		this->PullMaterial( pMaterial, Color( Settings.m_aMaterialsColor[ 3 ].r( ), Settings.m_aMaterialsColor[ 3 ].g( ), Settings.m_aMaterialsColor[ 3 ].b( ), ( int ) ( GetTransparency( Settings.m_aMaterialsColor[ 3 ].a( ) ) ) ), bXQZ );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( Settings.m_aMaterialParams[ 4 ] )
	{
		pMaterial = this->m_aMaterials.at( PULSE );
		this->PullMaterial( pMaterial, Color( Settings.m_aMaterialsColor[ 4 ].r( ), Settings.m_aMaterialsColor[ 4 ].g( ), Settings.m_aMaterialsColor[ 4 ].b( ), ( int ) ( GetTransparency( Settings.m_aMaterialsColor[ 4 ].a( ) ) ) ), bXQZ );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( !bForceNull )
		return;

	const float aWhite[ 3 ]
		=
	{
		1.0f,
		1.0f,
		1.0f
	};

	SDK::Interfaces::RenderView->SetBlend( GetTransparency( Settings.m_aMaterialsColor[ 0 ].a( ) ) / 255.0f );
	SDK::Interfaces::RenderView->SetColorModulation( aWhite );

	return SDK::Interfaces::ModelRender->ForcedMaterialOverride( NULL );

}

void C_ChamsSystem::DrawSharedMateraisOnEntity( C_SharedChamsData pData, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ )
{

	IMaterial* pMaterial;

	switch ( pData.m_iMainMaterial )
	{

		case FLAT:
		pMaterial = this->m_aMaterials.at( FLAT );
		break;
		case REGULAR:
		pMaterial = this->m_aMaterials.at( REGULAR );
		break;

		case SKY:
		pMaterial = this->m_aMaterials.at( SKY );
		break;
		default:
		pMaterial = nullptr;
		break;
	}

	if ( !pMaterial )
		return;

	int red = ( pData.m_iMainMaterialColor >> 16 ) & 0x0ff;
	int green = ( pData.m_iMainMaterialColor >> 8 ) & 0x0ff;
	int blue = ( pData.m_iMainMaterialColor ) & 0x0ff;
	// Setup main material
	this->PullMaterial( pMaterial, Color( ( ( pData.m_iMainMaterialColor >> 16 ) & 0x0ff ) * 255, ( ( pData.m_iMainMaterialColor >> 8 ) & 0x0ff ) * 255, ( ( pData.m_iMainMaterialColor ) & 0x0ff ) * 255 ), bXQZ, pMaterial == this->m_aMaterials.at( SKY ) );

	Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );


	if ( pData.m_bLayerEnabled[ 0 ] )
	{
		pMaterial = this->m_aMaterials.at( GLOW );
		this->PullMaterial( pMaterial, Color( ( pData.m_iLayerColor[ 0 ] >> 16 ) & 0x0ff, ( pData.m_iLayerColor[ 0 ] >> 8 ) & 0x0ff, ( pData.m_iLayerColor[ 0 ] ) & 0x0ff ), bXQZ, false );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}

	if ( pData.m_bLayerEnabled[ 1 ] )
	{
		pMaterial = this->m_aMaterials.at( GHOST );
		this->PullMaterial( pMaterial, Color( ( pData.m_iLayerColor[ 1 ] >> 16 ) & 0x0ff, ( pData.m_iLayerColor[ 1 ] >> 8 ) & 0x0ff, ( pData.m_iLayerColor[ 1 ] ) & 0x0ff ), bXQZ, false );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}


	if ( pData.m_bLayerEnabled[ 2 ] )
	{
		pMaterial = this->m_aMaterials.at( GLASS );
		this->PullMaterial( pMaterial, Color( ( pData.m_iLayerColor[ 2 ] >> 16 ) & 0x0ff, ( pData.m_iLayerColor[ 2 ] >> 8 ) & 0x0ff, ( pData.m_iLayerColor[ 2 ] ) & 0x0ff ), bXQZ, false );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, pCtx, pState, pInfo, aMatrix );
	}


	if ( !bForceNull )
		return;

	const float aWhite[ 3 ]
		=
	{
		1.0f,
		1.0f,
		1.0f
	};

	SDK::Interfaces::RenderView->SetBlend( 1.f );
	SDK::Interfaces::RenderView->SetColorModulation( aWhite );

	return SDK::Interfaces::ModelRender->ForcedMaterialOverride( NULL );
}

void C_ChamsSystem::PullMaterial( IMaterial* pMaterial, Color cColor, bool bIngoreZ, bool bUniqueMaterial, bool bOnShotChams )
{
	bool bHasBeenFound = false;

	if ( !pMaterial )
		return;

	if ( !bUniqueMaterial )
	{
		IMaterialVar* pMaterialVar = pMaterial->FindVar( _S( "$envmaptint" ), &bHasBeenFound );
		if ( !pMaterialVar )
			return;

		if ( bHasBeenFound )
			pMaterialVar->SetVecValue( cColor.r( ) / 255.0f, cColor.g( ) / 255.0f, cColor.b( ) / 255.0f );
	}

	const float aBlendColor[ 3 ]
		=
	{
		( float ) ( cColor.r( ) ) / 255.0f,
		( float ) ( cColor.g( ) ) / 255.0f,
		( float ) ( cColor.b( ) ) / 255.0f
	};


	pMaterial->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, bIngoreZ );


	pMaterial->ColorModulate( 1.0f, 1.0f, 1.0f );
	pMaterial->AlphaModulate( 1.0f );

	SDK::Interfaces::RenderView->SetColorModulation( aBlendColor );
	SDK::Interfaces::RenderView->SetBlend( cColor.a( ) / 255.0f );
	return SDK::Interfaces::ModelRender->ForcedMaterialOverride( pMaterial );

}

IMaterial* C_ChamsSystem::CreateMaterial( const char* szName, const char* szContent )
{
	// create key values
	C_KeyValues* m_KeyValues = new C_KeyValues( szName );
	m_KeyValues->LoadFromBuffer( m_KeyValues, szName, szContent );
	IMaterial* m_Material = SDK::Interfaces::MaterialSystem->CreateMaterial( szName, m_KeyValues );
	m_Material->IncrementReferenceCount( );
	return m_Material;
}
