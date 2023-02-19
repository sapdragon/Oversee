#include "ShotChams.hpp"
#include "Chams.hpp"
#include "../../../Hooks/Hooks.hpp"

template < typename T >
static T GetVirtual( void* pClass, int nIndex )
{
	return reinterpret_cast< T >( ( *( int** ) pClass )[ nIndex ] );
}

void C_ShotChams::OnRageBotFire( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, Vector vecMatrixOrigin )
{
	IClientRenderable* pRenderable = pPlayer->GetClientRenderable( );
	if ( !pRenderable )
		return;

	const model_t* pModel = pPlayer->GetModel( );
	if ( !pModel )
		return;

	C_ShotChamsData& NewShotData = m_aShotChams.emplace_back( );

	NewShotData.m_RenderInfo.origin = pPlayer->GetAbsOrigin( );
	NewShotData.m_RenderInfo.angles = pPlayer->GetAbsAngles( );
	NewShotData.m_flAnimation = 0.0f;
	NewShotData.m_flCreationTime = SDK::Interfaces::GlobalVars->m_flRealTime;
	NewShotData.m_DrawState.m_pStudioHdr = SDK::Interfaces::ModelInfo->GetStudiomodel( pModel );
	NewShotData.m_DrawState.m_pStudioHWData = SDK::Interfaces::MDLCache->GetHardwareData( pModel->studio );
	NewShotData.m_DrawState.m_pRenderable = pRenderable;
	NewShotData.m_DrawState.m_drawFlags = 0;
	NewShotData.m_DrawState.m_pModelToWorld = &NewShotData.m_aWorldMatrix;

	NewShotData.m_RenderInfo.pRenderable = pRenderable;
	NewShotData.m_RenderInfo.pModel = pModel;
	NewShotData.m_RenderInfo.pLightingOffset = nullptr;
	NewShotData.m_RenderInfo.pLightingOrigin = nullptr;
	NewShotData.m_RenderInfo.hitboxset = pPlayer->m_nHitboxSet( );
	NewShotData.m_RenderInfo.skin = ( int ) ( ( uintptr_t ) ( pPlayer ) + 0xA1C );
	NewShotData.m_RenderInfo.body = ( int ) ( ( uintptr_t ) ( pPlayer ) + 0xA20 );
	NewShotData.m_RenderInfo.entity_index = pPlayer->EntIndex( );
	NewShotData.m_RenderInfo.instance = GetVirtual< ModelInstanceHandle_t( __thiscall* )( void* ) >( pRenderable, 30 )( pRenderable );
	NewShotData.m_RenderInfo.flags = 0x1;
	NewShotData.m_RenderInfo.pModelToWorld = &NewShotData.m_aWorldMatrix;

	std::memcpy( NewShotData.m_aMatrix.data( ), aMatrix, sizeof( matrix3x4_t ) * MAXSTUDIOBONES );

	Vector vecMatrixDelta = pPlayer->m_vecOrigin( ) - vecMatrixOrigin;
	for ( auto& Matrix : NewShotData.m_aMatrix )
	{
		Matrix[ 0 ][ 3 ] += vecMatrixDelta.x;
		Matrix[ 1 ][ 3 ] += vecMatrixDelta.y;
		Matrix[ 2 ][ 3 ] += vecMatrixDelta.z;
	}

	NewShotData.m_aWorldMatrix.AngleMatrix(Vector(NewShotData.m_RenderInfo.angles.pitch, NewShotData.m_RenderInfo.angles.yaw, NewShotData.m_RenderInfo.angles.roll), NewShotData.m_RenderInfo.origin );
}

void C_ShotChams::Instance( )
{
	if ( !g_Globals->m_LocalPlayer )
	{
		if ( !m_aShotChams.empty( ) )
			m_aShotChams.clear( );

		return;
	}

	ChamsSettings_t Settings = g_SettingsManager->GetChamsSettings( _S( "on_shot" ) );
	if ( !Settings.m_bDrawChams )
		return;

	for ( auto it = m_aShotChams.begin( ); it != m_aShotChams.end( ); it++ )
	{
		if ( it >= m_aShotChams.end( ) )
			break;

		if ( !it->m_DrawState.m_pModelToWorld || !it->m_DrawState.m_pRenderable || !it->m_DrawState.m_pStudioHdr || !it->m_DrawState.m_pStudioHWData ||
			!it->m_RenderInfo.pRenderable || !it->m_RenderInfo.pModelToWorld || !it->m_RenderInfo.pModel || !SDK::Interfaces::EntityList->GetClientEntity( it->m_RenderInfo.entity_index ) )
		{
			it = m_aShotChams.erase( it );
			continue;
		}

		if ( SDK::Interfaces::GlobalVars->m_flRealTime - it->m_flCreationTime > 4.0f )
		{
			it = m_aShotChams.erase( it );
			continue;
		}

		float flProgress = 1.0f;
		if ( SDK::Interfaces::GlobalVars->m_flRealTime - it->m_flCreationTime >= 3.8f )
		{
			it->m_flAnimation = Math::Interpolate( it->m_flAnimation, 1.0f, SDK::Interfaces::GlobalVars->m_flFrameTime * 5.0f );
			flProgress = 1.0f - it->m_flAnimation;
		}

		Color aMainColor = Settings.m_Color;

		float flFirstAlpha = aMainColor.a( ) * flProgress;
		float flSecondAlpha = Settings.m_aMaterialsColor[ 0 ][ 3 ] * flProgress;
		float flThirdAlpha = Settings.m_aMaterialsColor[ 1 ][ 3 ] * flProgress;
		float flFourthAlpha = Settings.m_aMaterialsColor[ 2 ][ 3 ] * flProgress;

		IMaterial* pMaterial = nullptr;
		if ( Settings.m_aMaterialParams[ 0 ] )
		{
			pMaterial = g_ChamsSystem->m_aMaterials.at( GOLD );
			g_ChamsSystem->PullMaterial( pMaterial, Settings.m_aMaterialsColor[ 0 ], true, false, true );

			SDK::Interfaces::RenderView->SetBlend( flSecondAlpha );
			Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
		}

		if ( Settings.m_aMaterialParams[ 1 ] )
		{
			pMaterial = g_ChamsSystem->m_aMaterials.at( GLOW );
			g_ChamsSystem->PullMaterial( pMaterial, Settings.m_aMaterialsColor[ 1 ], true, false, true );

			SDK::Interfaces::RenderView->SetBlend( flThirdAlpha );
			Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
		}

		if ( Settings.m_aMaterialParams[ 2 ] )
		{
			pMaterial = g_ChamsSystem->m_aMaterials.at( GHOST );
			g_ChamsSystem->PullMaterial( pMaterial, Settings.m_aMaterialsColor[ 2 ], true, false, true );

			SDK::Interfaces::RenderView->SetBlend( flFourthAlpha );
			Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
		}

		if ( Settings.m_aMaterialParams[ 3 ] )
		{
			pMaterial = g_ChamsSystem->m_aMaterials.at( GLASS );
			g_ChamsSystem->PullMaterial( pMaterial, Settings.m_aMaterialsColor[ 3 ], true, false, true );

			SDK::Interfaces::RenderView->SetBlend( flFourthAlpha );
			Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
		}

		if ( Settings.m_aMaterialParams[ 4 ] )
		{
			pMaterial = g_ChamsSystem->m_aMaterials.at( PULSE );
			g_ChamsSystem->PullMaterial( pMaterial, Settings.m_aMaterialsColor[ 4 ], true, false, true );

			SDK::Interfaces::RenderView->SetBlend( flFourthAlpha );
			Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
		}

		switch ( Settings.m_iFirstChamsLayerStyle )
		{
			case FLAT:
			pMaterial = g_ChamsSystem->m_aMaterials.at( FLAT );
			break;

			case REGULAR:
			pMaterial = g_ChamsSystem->m_aMaterials.at( REGULAR );
			break;

			case SKY:
			pMaterial = g_ChamsSystem->m_aMaterials.at( SKY );
			break;

			default:
			pMaterial = nullptr;
			break;
		}

		g_ChamsSystem->PullMaterial( pMaterial, Settings.m_Color, true, pMaterial == g_ChamsSystem->m_aMaterials.at( SKY ), true );

		SDK::Interfaces::RenderView->SetBlend( flFirstAlpha );
		Hooks::o_DrawModelExecute( SDK::Interfaces::ModelRender, GetVirtual < IMatRenderContext* ( __thiscall* )( void* ) >( SDK::Interfaces::MaterialSystem, 115 )( SDK::Interfaces::MaterialSystem ), it->m_DrawState, it->m_RenderInfo, it->m_aMatrix.data( ) );
	}
	SDK::Interfaces::ModelRender->ForcedMaterialOverride( nullptr );
}