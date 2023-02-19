#include "Model.hpp"
#include "../../../Interface/GUI/Menu.hpp"
#include "../Chams/Chams.hpp"
#include "../../../SDK/Math/Math.hpp"
#include "../../../Interface/GUI/GUI.hpp"
#include "../../Fonts/FontManager.hpp"

std::array < Vector4D, 6 > aWhiteArray =
{
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D( 0.4f, 0.4f, 0.4f, 1.0f ),
};

std::array < float, 3 > aColorModulation =
{
	1.0f,
	1.0f,
	1.0f
};

void C_DrawModel::Instance( )
{
	if ( !g_Menu->IsVisible( ) )
		return;

	if ( !m_PreviewTexture )
	{
		SDK::Interfaces::MaterialSystem->BeginRenderTargetAllocation( );

		m_PreviewTexture = SDK::Interfaces::MaterialSystem->CreateNamedRenderTargetTextureEx(
			_S( "Preview" ),
			350, 575,
			RT_SIZE_NO_CHANGE,
			SDK::Interfaces::MaterialSystem->GetBackBufferFormat( ),
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
			CREATERENDERTARGETFLAGS_HDR
		);

		SDK::Interfaces::MaterialSystem->FinishRenderTargetAllocation( );
	}

	if ( !m_CubemapTexture )
		m_CubemapTexture = SDK::Interfaces::MaterialSystem->FindTexture( _S( "editor/cubemap.hdr" ), _S( TEXTURE_GROUP_CUBE_MAP ) );

	auto CreateModel = reinterpret_cast< void( __thiscall* )( void* ) >( SDK::EngineData::m_AddressList[ CheatAddressList::CreateModel ] );
	if ( !m_PreviewModel )
	{
		m_PreviewModel = static_cast< C_MergedMDL* >( SDK::Interfaces::MemAlloc->Alloc( 0x75C ) );
		CreateModel( m_PreviewModel );

		m_PreviewModel->SetMDL( _S( "models/player/custom_player/uiplayer/animset_uiplayer.mdl" ) );
		m_PreviewModel->SetMergedMDL( _S( "models/player/custom_player/legacy/ctm_fbi_variantb.mdl" ) );
		m_PreviewModel->SetMergedMDL( _S( "models/weapons/w_pist_elite.mdl" ) );

		m_PreviewModel->SetSequence( 32, false );
		m_PreviewModel->SetupBonesForAttachmentQueries( );
	}

	m_PreviewModel->RootMDL.flTime += SDK::Interfaces::GlobalVars->m_flFrameTime / 2.0f;

	m_ViewSetup.iX = 0;
	m_ViewSetup.iY = 0;
	m_ViewSetup.iWidth = 350;
	m_ViewSetup.iHeight = 575;
	m_ViewSetup.bOrtho = false;
	m_ViewSetup.flFOV = 70.f;
	m_ViewSetup.vecOrigin = Vector( ( g_Menu->m_iSelectedTab == 1 && g_Menu->m_iSelectedSubTab[ 1 ] == 0 ) ? -50.f : -65.0f, 0.0f, 50 );
	m_ViewSetup.angView = QAngle( 0, 0, 0 );
	m_ViewSetup.flNearZ = 7.0f;
	m_ViewSetup.flFarZ = 1000.f;
	m_ViewSetup.bDoBloomAndToneMapping = true;

	CMatRenderContextPtr pRenderContext( SDK::Interfaces::MaterialSystem );

	pRenderContext->PushRenderTargetAndViewport( );
	pRenderContext->SetRenderTarget( m_PreviewTexture );

	pRenderContext->BindLocalCubemap( m_CubemapTexture );
	pRenderContext->SetLightingOrigin( -65.0f, 0.0f, 50.0f );
	pRenderContext->SetIntRenderingParameter( 10, 0 );

	Frustum dummyFrustum;
	SDK::Interfaces::RenderView->Push3DView( pRenderContext, m_ViewSetup, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, m_PreviewTexture, dummyFrustum );

	pRenderContext->ClearColor4ub( false, false, false, false );
	pRenderContext->ClearBuffers( true, true, true );
	pRenderContext->SetAmbientLightCube( aWhiteArray.data( ) );

	SDK::Interfaces::StudioRender->SetAmbientLightColors( aWhiteArray.data( ) );
	SDK::Interfaces::StudioRender->SetLocalLights( 0, nullptr );

	matrix3x4_t matPlayerView = { };
	Math::AngleMatrix( QAngle( 0, ( g_Menu->m_iSelectedTab == 1 && g_Menu->m_iSelectedSubTab[ 1 ] == 0 ) ? -180.f : g_Globals->m_Model.m_flModelRotation, 0 ), matPlayerView, Vector( 0, 0, 0 ) );

	SDK::Interfaces::ModelRender->SuppressEngineLighting( true );
	g_Globals->m_Model.m_bDrawModel = true;
	m_PreviewModel->Draw( matPlayerView );
	g_Globals->m_Model.m_bDrawModel = false;
	SDK::Interfaces::ModelRender->SuppressEngineLighting( false );

	float white[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
	SDK::Interfaces::RenderView->SetBlend( 1.0f );
	SDK::Interfaces::RenderView->SetColorModulation( white );
	SDK::Interfaces::RenderView->PopView( pRenderContext, dummyFrustum );
	pRenderContext->BindLocalCubemap( nullptr );

	pRenderContext->PopRenderTargetAndViewport( );
	pRenderContext->Release( );
}

void Username( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.1" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "Nickname" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, FontSize + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, size.y / 2 - ImTextSize.y / 2 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.name" ) ] ), g_Globals->m_szName.c_str( ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, FontSize + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, size.y / 2 - ImTextSize.y / 2 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.name" ) ] ), g_Globals->m_szName.c_str( ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, FontSize + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, size.y / 2 - ImTextSize.y / 2 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.name" ) ] ), g_Globals->m_szName.c_str( ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 70, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Nickname" ) );
	}

	if ( ImGui::BeginPopup( _S( "UsernamePopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.name" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.1" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "UsernamePopup" ) );
}

void Weapon( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.0" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, g_SettingsManager->B[ _S( "esp.enemy.ammo_amount" ) ] ? _S( "Dual Elites [12]" ) : _S( "Dual Elites" ) );

	std::string text = g_SettingsManager->B[ _S( "esp.enemy.ammo_amount" ) ] ? _S( "Dual Elites [12]" ) : _S( "Dual Elites" );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.weapon" ) ] ), text.c_str( ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.weapon" ) ] ), text.c_str( ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImGui::CalcTextSize( text.c_str( ) ).x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.weapon" ) ] ), text.c_str( ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 60, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Weapon" ) );
	}

	if ( ImGui::BeginPopup( _S( "WeaponPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.weapon" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->Checkbox( _S( "Ammo Amount" ), _S( "esp.enemy.ammo_amount" ) );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.0" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "WeaponPopup" ) );
}

void HealthBar( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	if ( cond == LEFT_COND || cond == RIGHT_COND ) {
		size = ImVec2( 6, 320 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 1 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_outline_bottom" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 60 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_bttom" ) ] );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, 10 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 5, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_outline_bottom" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 56, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.health_bttom" ) ] );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 55, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Health" ) );
	}

	if ( ImGui::BeginPopup( _S( "HealthBarPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.health_bttom" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Outer" ), &g_SettingsManager->IC[ _S( "esp.enemy.health_outline_upper" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Inner" ), &g_SettingsManager->IC[ _S( "esp.enemy.health_outline_bottom" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "HealthBarPopup" ) );
}

void ArmorBar( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	if ( cond == LEFT_COND || cond == RIGHT_COND ) {
		size = ImVec2( 6, 320 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 1 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_bottom" ) ] );

		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 98 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor" ) ] );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, 10 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 5, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_bottom" ) ] );

		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 56, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.armor" ) ] );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 45, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Armor" ) );
	}

	if ( ImGui::BeginPopup( _S( "ArmorBarPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.armor" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Outer" ), &g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_upper" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Inner" ), &g_SettingsManager->IC[ _S( "esp.enemy.armor_outline_bottom" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "ArmorBarPopup" ) );
}

void Box( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	if ( cond == CENTER_COND ) {
		size = ImVec2( 180, 320 );

		draw->AddRect( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.box" ) ] ) );
		draw->AddRect( pos + ImVec2( 3, -1 ), pos + size - ImVec2( 3, -1 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.box_outline_upper" ) ] );
		draw->AddRect( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 1 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.box_outline_bottom" ) ] );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 35, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Box" ) );
	}

	if ( ImGui::BeginPopup( _S( "BoxPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.box" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Outer" ), &g_SettingsManager->IC[ _S( "esp.enemy.box_outline_upper" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::ColorEdit4( _S( "Color Inner" ), &g_SettingsManager->IC[ _S( "esp.enemy.box_outline_bottom" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "BoxPopup" ) );
}

void Ammo( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	if ( cond == LEFT_COND || cond == RIGHT_COND ) {
		size = ImVec2( 6, 320 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 1 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo_outline_bottom" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 1 ), pos + size - ImVec2( 5, 60 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo" ) ] );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, 10 );
		draw->AddRectFilled( pos + ImVec2( 4, 0 ), pos + size - ImVec2( 4, 0 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo_outline_upper" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 5, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo_outline_bottom" ) ] );
		draw->AddRectFilled( pos + ImVec2( 5, 3 ), pos + size - ImVec2( 56, 3 ), ( ImColor ) g_SettingsManager->IC[ _S( "esp.enemy.ammo" ) ] );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 45, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Ammo" ) );
	}

	if ( ImGui::BeginPopup( _S( "PingPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{

			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "PingPopup" ) );
}

void Scope( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.3" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "Scoped" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.scope" ) ] ), _S( "Scoped" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.scope" ) ] ), _S( "Scoped" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.scope" ) ] ), _S( "Scoped" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 45, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Scope" ) );
	}

	if ( ImGui::BeginPopup( _S( "ScopePopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.scope" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.3" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "ScopePopup" ) );
}

void Defusing( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.4" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "Defusing" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.defusing" ) ] ), _S( "Defusing" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.defusing" ) ] ), _S( "Defusing" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.defusing" ) ] ), _S( "Defusing" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 70, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Defusing" ) );
	}

	if ( ImGui::BeginPopup( _S( "DefusingPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.defusing" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.4" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "DefusingPopup" ) );
}

void Flashed( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.5" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "Flashed" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.flashed" ) ] ), _S( "Flashed" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.flashed" ) ] ), _S( "Flashed" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.flashed" ) ] ), _S( "Flashed" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 55, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Flashed" ) );
	}

	if ( ImGui::BeginPopup( _S( "FlashedPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.flashed" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.5" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "FlashedPopup" ) );
}

void Taser( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.6" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "Taser" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.taser" ) ] ), _S( "Taser" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.taser" ) ] ), _S( "Taser" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.taser" ) ] ), _S( "Taser" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 47, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "Taser" ) );
	}

	if ( ImGui::BeginPopup( _S( "TaserPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.taser" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.6" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "TaserPopup" ) );
}

void LC( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.7" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "LC" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.lc" ) ] ), _S( "LC" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.lc" ) ] ), _S( "LC" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImTextSize.x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.lc" ) ] ), _S( "LC" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 25, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), "LC" );
	}

	if ( ImGui::BeginPopup( _S( "LCPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.lc" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.7" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "LCPopup" ) );
}

void Duck( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond )
{
	ImFont* m_Font = g_FontManager->GetFontFromHash( FnvHash( "Player ESP" ) );
	if ( !m_Font )
		return;

	int FontSize = g_SettingsManager->I[ _S( "esp.enemy.font_sizing.8" ) ];
	ImVec2 ImTextSize = m_Font->CalcTextSizeA( FontSize, FLT_MAX, 0.0f, _S( "FD" ) );

	if ( cond == LEFT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 1, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.ducking" ) ] ), _S( "FD" ) );
	}

	if ( cond == RIGHT_COND ) {
		size = ImVec2( ImTextSize.x + 10, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 9, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.ducking" ) ] ), _S( "FD" ) );
	}

	if ( cond == TOP_COND || cond == BOT_COND ) {
		size = ImVec2( 180, ImTextSize.y + 6 );
		draw->AddText( m_Font, FontSize, pos + ImVec2( 90 - ImGui::CalcTextSize( _S( "FD" ) ).x / 2, 3 ), ImColor( g_SettingsManager->IC[ _S( "esp.enemy.ducking" ) ] ), _S( "FD" ) );
	}

	if ( cond == IN_MOVE_COND || cond == POOL_COND ) {
		size = ImVec2( 55, 20 );

		draw->AddRectFilled( pos, pos + size, ImColor( 117, 117, 117 ), 3 );
		draw->AddText( pos + ImVec2( 5, 3 ), ImColor( 255, 255, 255 ), _S( "FD" ) );
	}

	if ( ImGui::BeginPopup( _S( "duckingPopup" ) ) )
	{
		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginGroup( );
		{
			ImGui::ColorEdit4( _S( "Color" ), &g_SettingsManager->IC[ _S( "esp.enemy.ducking" ) ].Value.x, ImGuiColorEditFlags_NoInputs );
			g_UIElements->SliderInt( _S( "Font Size" ), _S( "esp.enemy.font_sizing.8" ), 9, 22 );
			ImGui::Dummy( { 150, 10 } );
		}
		ImGui::EndGroup( );

		ImGui::EndPopup( );
	}

	if ( pressed )
		ImGui::OpenPopup( _S( "duckingPopup" ) );
}

void GetDrawableFunc( bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond, int var ) {
	switch ( var ) {
		case 0: return Weapon( pressed, draw, pos, size, cond ); break;
		case 1: return Username( pressed, draw, pos, size, cond ); break;
		case 2: return HealthBar( pressed, draw, pos, size, cond ); break;
		case 3: return ArmorBar( pressed, draw, pos, size, cond ); break;
		case 4: return Box( pressed, draw, pos, size, cond ); break;
		case 5: return Ammo( pressed, draw, pos, size, cond ); break;
		case 6: return Scope( pressed, draw, pos, size, cond ); break;
		case 7: return Defusing( pressed, draw, pos, size, cond ); break;
		case 8: return Flashed( pressed, draw, pos, size, cond ); break;
		case 9: return Taser( pressed, draw, pos, size, cond ); break;
		case 10: return LC( pressed, draw, pos, size, cond ); break;
		case 11: return Duck( pressed, draw, pos, size, cond ); break;
	}
}

std::vector<std::vector<MovableItems>> Pool = {
	std::vector<MovableItems>{}, // Left 0 
	std::vector<MovableItems>{}, // Right 1
	std::vector<MovableItems>{}, // Top 2
	std::vector<MovableItems>{}, // Bot 3
	std::vector<MovableItems>{}, // Center 4
	std::vector<MovableItems>{
	MovableItems( 0, _S( "Weapon" ), POOL_COND, { 10, 540 }, { 10, 540 }, 0 ),
		MovableItems( 1, _S( "Username" ), POOL_COND, { 75, 540 }, { 75, 540 }, 0 ),
		MovableItems( 2, _S( "Health" ), POOL_COND, { 150, 540 }, { 150, 540 }, 1 ),
		MovableItems( 3, _S( "Armor" ), POOL_COND, { 210, 540 }, { 210, 540 }, 1 ),
		MovableItems( 4, _S( "Box" ), POOL_COND, { 260, 540 }, { 260, 540 }, 2 ),
		MovableItems( 5, _S( "Ammo" ), POOL_COND, { 300, 540 }, { 300, 540 }, 1 ),
		MovableItems( 6, _S( "Scope" ), POOL_COND, { 10, 570 }, { 10, 570 }, 0 ),
		MovableItems( 7, _S( "Defusing" ), POOL_COND, { 60, 570 }, { 60, 570 }, 0 ),
		MovableItems( 8, _S( "Flashed" ), POOL_COND, { 135, 570 }, { 135, 570 }, 0 ),
		MovableItems( 9, _S( "Taser" ), POOL_COND, { 195, 570 }, { 195, 570 }, 0 ),
		MovableItems( 10, _S( "LC" ), POOL_COND, { 245, 570 }, { 245, 570 }, 0 ),
		MovableItems( 11, _S( "FD" ), POOL_COND, { 275, 570 }, { 275, 570 }, 0 ),
}, // Pool 5
std::vector<MovableItems>{}, // InMove 6
};

bool ItemInMove( MovableItems Item ) {
	for ( auto a = 0; a < Pool[ IN_MOVE_COND ].size( ); a++ ) {
		if ( Pool[ IN_MOVE_COND ][ a ].ItemName == Item.ItemName )
			return true;
	}
	return false;
}

std::tuple<int, int> GetMovableItemPosition( MovableItems Item )
{
	for ( auto a = 0; a < 7; a++ ) {
		for ( auto b = 0; b < Pool[ a ].size( ); b++ ) {
			if ( Pool[ a ][ b ].ItemName == Item.ItemName )
				return std::make_tuple( a, b );
		}
	}
}

std::tuple<int, int> GetMovableItemPosition( std::string name )
{
	for ( auto a = 0; a < 7; a++ ) {
		for ( auto b = 0; b < Pool[ a ].size( ); b++ ) {
			if ( Pool[ a ][ b ].ItemName == name )
				return std::make_tuple( a, b );
		}
	}
}

void Vector2Vector( MovableItems Item, int Destination )
{
	auto Position = GetMovableItemPosition( Item );

	Pool[ std::get<0>( Position ) ].erase( Pool[ std::get<0>( Position ) ].begin( ) + std::get<1>( Position ) ); // Erase From Position
	Pool[ Destination ].emplace_back( MovableItems( Item.Draw, Item.ItemName, Destination, Item.TemporaryPos, Item.BasicPositions, Item.Type ) ); // Append to Destination
}

void Vector2VectorIndexed( MovableItems Item, int Destination, int Index )
{
	auto Position = GetMovableItemPosition( Item );

	Pool[ std::get<0>( Position ) ].erase( Pool[ std::get<0>( Position ) ].begin( ) + std::get<1>( Position ) ); // Erase From Position
	Pool[ Destination ].emplace( Pool[ Destination ].cbegin( ) + Index, MovableItems( Item.Draw, Item.ItemName, Destination, Item.TemporaryPos, Item.BasicPositions, Item.Type ) ); // Append to Destination
}

static bool Handle( MovableItems& Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiID id = window->GetID( Item.ItemName.c_str( ) ); // use the pointer address as identifier
	ImGuiButtonFlags flags = 0;

	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;

	ImRect rect = ImRect( ImGui::GetWindowPos( ).x + Item.TemporaryPos.x,
		ImGui::GetWindowPos( ).y + Item.TemporaryPos.y,
		ImGui::GetWindowPos( ).x + Item.TemporaryPos.x + Item.WidgetSize.x,
		ImGui::GetWindowPos( ).y + Item.TemporaryPos.y + Item.WidgetSize.y );

	ImGui::ItemSize( rect, style.FramePadding.y );
	if ( !ImGui::ItemAdd( rect, id ) )
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( rect, id, &hovered, &held, flags );
	bool is_active = ImGui::IsItemActive( );

	ImGuiCol handle_color = ImColor( ImGui::GetStyle( ).Colors[ ImGuiCol_ButtonHovered ] );
	ImDrawList* draw_list = ImGui::GetWindowDrawList( );

	GetDrawableFunc( ImGui::IsMouseClicked( ImGuiMouseButton_Right ) && ImGui::IsMouseHoveringRect( rect.Min, rect.Max ), draw_list, ImGui::GetWindowPos( ) + Item.TemporaryPos, Item.WidgetSize, Item.VectorCond, Item.Draw );

	if ( is_active && ImGui::IsMouseDragging( 0 ) ) {
		Item.TemporaryPos[ 0 ] += ImGui::GetIO( ).MouseDelta.x;
		Item.TemporaryPos[ 1 ] += ImGui::GetIO( ).MouseDelta.y;
	}

	return held;
}

bool MouseIntersectRect( ImVec2 pos1, ImVec2 pos2 )
{
	if ( ImGui::GetIO( ).MousePos.x >= ImGui::GetWindowPos( ).x + pos1.x &&
		ImGui::GetIO( ).MousePos.y >= ImGui::GetWindowPos( ).y + pos1.y &&
		ImGui::GetIO( ).MousePos.x <= ImGui::GetWindowPos( ).x + pos2.x &&
		ImGui::GetIO( ).MousePos.y <= ImGui::GetWindowPos( ).y + pos2.y )
		return true;

	return false;
}

void RecalculatePool( float animation )
{
	for ( auto a = 0; a <= IN_MOVE_COND; a++ )
	{
		for ( auto b = 0; b < Pool[ a ].size( ); b++ )
		{
			if ( a == IN_MOVE_COND || a == POOL_COND ) {
				Pool[ a ][ b ].TemporaryPos = ImLerp( Pool[ a ][ b ].TemporaryPos, Pool[ a ][ b ].BasicPositions, animation );

				if ( a == IN_MOVE_COND )
					Vector2Vector( Pool[ a ][ b ], POOL_COND );
			}
		}
	}
}

void RecalculateLeftRight( float animation )
{
	float PositionLeft = 0;
	float xModifier = 0;
	for ( auto b = 0; b < Pool[ LEFT_COND ].size( ); b++ )
	{
		Pool[ LEFT_COND ][ b ].TemporaryPos = ImLerp( Pool[ LEFT_COND ][ b ].TemporaryPos, ImVec2( 100 - Pool[ LEFT_COND ][ b ].WidgetSize.x - xModifier
			, 120 + PositionLeft ), animation );

		if ( b < 1 )
		{
			if ( Pool[ LEFT_COND ][ 0 ].ItemName == _S( "Health" ) || Pool[ LEFT_COND ][ 0 ].ItemName == _S( "Armor") ||
				Pool[ LEFT_COND ][ 1 ].ItemName == _S( "Health" ) || Pool[ LEFT_COND ][ 1 ].ItemName == _S( "Armor") )
				xModifier += 10;
		}

		if ( Pool[ LEFT_COND ][ b ].ItemName != _S( "Health" ) && Pool[ LEFT_COND ][ b ].ItemName != _S( "Armor") )
			PositionLeft += Pool[ LEFT_COND ][ b ].WidgetSize.y;
	}


	float PositionRight = 0;
	float yModifier = 0;
	for ( auto b = 0; b < Pool[ RIGHT_COND ].size( ); b++ )
	{
		Pool[ RIGHT_COND ][ b ].TemporaryPos = ImLerp( Pool[ RIGHT_COND ][ b ].TemporaryPos, ImVec2( 280 + yModifier
			, 120 + PositionRight ), animation );

		if ( b < 1 )
		{
			if ( Pool[ RIGHT_COND ][ 0 ].ItemName == _S( "Health" ) || Pool[ RIGHT_COND ][ 0 ].ItemName == _S( "Armor" ) ||
				Pool[ RIGHT_COND ][ 1 ].ItemName == _S( "Health" ) || Pool[ RIGHT_COND ][ 1 ].ItemName == _S( "Armor" ) )
				yModifier += 10;
		}

		if ( Pool[ RIGHT_COND ][ b ].ItemName != _S( "Health" ) && Pool[ RIGHT_COND ][ b ].ItemName != _S( "Armor" ) )
			PositionRight += Pool[ RIGHT_COND ][ b ].WidgetSize.y;
	}
}

void RecalculateTopBot( float animation )
{
	float PositionTop = 0;
	for ( auto b = 0; b < Pool[ TOP_COND ].size( ); b++ )
	{
		PositionTop += Pool[ TOP_COND ][ b ].WidgetSize.y;
		Pool[ TOP_COND ][ b ].TemporaryPos = ImLerp( Pool[ TOP_COND ][ b ].TemporaryPos, ImVec2( 100, 120 - PositionTop ), animation );
	}

	float PositionBot = 0;
	for ( auto b = 0; b < Pool[ BOT_COND ].size( ); b++ )
	{
		Pool[ BOT_COND ][ b ].TemporaryPos = ImLerp( Pool[ BOT_COND ][ b ].TemporaryPos, ImVec2( 100, 440 + PositionBot ), animation );
		PositionBot += Pool[ BOT_COND ][ b ].WidgetSize.y;
	}

	for ( auto b = 0; b < Pool[ CENTER_COND ].size( ); b++ )
	{
		Pool[ CENTER_COND ][ b ].TemporaryPos = ImLerp( Pool[ CENTER_COND ][ b ].TemporaryPos, ImVec2( 100, 120 ), animation );
	}
}

int emptyPos = 0;
int emptyPosPrediction = 0;
bool dragginAnyItem = false;

void IsHoveringStandingItem( ) {
	for ( auto a = 0; a < CENTER_COND; a++ )
	{
		for ( auto b = 0; b < Pool[ a ].size( ); b++ )
		{
			auto pos = ImGui::GetWindowPos( );
			if ( ImGui::IsMouseHoveringRect( pos + Pool[ a ][ b ].TemporaryPos, pos + Pool[ a ][ b ].TemporaryPos + ImVec2( Pool[ a ][ b ].WidgetSize.x, Pool[ a ][ b ].WidgetSize.y / 2 ) ) )
			{
				if ( b == 0 ) {
					emptyPosPrediction = 0;
				}
				else {
					emptyPosPrediction = b - 1;
				}
			}
			else if ( ImGui::IsMouseHoveringRect( pos + Pool[ a ][ b ].TemporaryPos + ImVec2( 0, Pool[ a ][ b ].WidgetSize.y / 2 ), pos + Pool[ a ][ b ].TemporaryPos + Pool[ a ][ b ].WidgetSize ) ) {
				emptyPosPrediction = b + 1;
			}
		}
	}
}

void IsDragginAnyItem( ) {
	for ( auto a = 0; a < CENTER_COND; a++ )
	{
		for ( auto b = 0; b < Pool[ a ].size( ); b++ )
		{
			auto pos = ImGui::GetWindowPos( );
			if ( ImGui::IsMouseHoveringRect( pos + Pool[ a ][ b ].TemporaryPos, pos + Pool[ a ][ b ].TemporaryPos + Pool[ a ][ b ].WidgetSize )
				&& ImGui::IsMouseDragging( 0 ) )
			{
				dragginAnyItem = true;
			}
			else {
				dragginAnyItem = false;
			}
		}
	}
}

void C_DrawModel::RenderPreview( )
{
	auto draw = ImGui::GetWindowDrawList( );
	auto pos = ImGui::GetWindowPos( );

	draw->AddRectFilled( pos + ImVec2( 0, 530 ), pos + ImVec2( 380, 600 ), ImColor( 100, 100, 100, 50 ), 8, 15 );

	for ( auto a = 0; a < 7; a++ )
	{
		for ( auto b = 0; b < Pool[ a ].size( ); b++ )
		{
			if ( Handle( Pool[ a ][ b ] ) )
			{
				isMouseInAction = true;
				RecalculateAnimationFlag = true;

				if ( MouseIntersectRect( ImVec2( 0, 530 ), ImVec2( 380, 600 ) ) ) {
					if ( a == IN_MOVE_COND ) {
						Pool[ a ][ b ].VectorCond = POOL_COND;
						Vector2Vector( Pool[ a ][ b ], POOL_COND );
					}
				}
				else if ( MouseIntersectRect( ImVec2( 0, 120 ), ImVec2( 100, 440 ) ) && Pool[ a ][ b ].ItemName != _S( "Box" ) ) {
					if ( a == IN_MOVE_COND ) {
						if ( Pool[ a ][ b ].ItemName != _S( "Health" ) && Pool[ a ][ b ].ItemName != _S( "Armor" ) ) {
							Pool[ a ][ b ].VectorCond = LEFT_COND;
							Vector2Vector( Pool[ a ][ b ], LEFT_COND );
						}
						else if ( Pool[ a ][ b ].ItemName == _S( "Health" ) ) {
							Pool[ a ][ b ].VectorCond = LEFT_COND;
							Vector2VectorIndexed( Pool[ a ][ b ], LEFT_COND, 0 );
						}
						else if ( Pool[ a ][ b ].ItemName == _S( "Armor" ) ) {
							Pool[ a ][ b ].VectorCond = LEFT_COND;
							Vector2VectorIndexed( Pool[ a ][ b ], LEFT_COND, 0 );
						}
					}
				}
				else if ( MouseIntersectRect( ImVec2( 280, 120 ), ImVec2( 380, 440 ) ) && Pool[ a ][ b ].ItemName != _S( "Box" ) ) {
					if ( a == IN_MOVE_COND ) {
						if ( Pool[ a ][ b ].ItemName != _S( "Health" ) && Pool[ a ][ b ].ItemName != _S( "Armor" ) ) {
							Pool[ a ][ b ].VectorCond = RIGHT_COND;
							Vector2Vector( Pool[ a ][ b ], RIGHT_COND );
						}
						else if ( Pool[ a ][ b ].ItemName == _S( "Health" ) ) {
							Pool[ a ][ b ].VectorCond = RIGHT_COND;
							Vector2VectorIndexed( Pool[ a ][ b ], RIGHT_COND, 0 );
						}
						else if ( Pool[ a ][ b ].ItemName == _S( "Armor" ) ) {
							Pool[ a ][ b ].VectorCond = RIGHT_COND;
							Vector2VectorIndexed( Pool[ a ][ b ], RIGHT_COND, 0 );
						}
					}
				}
				else if ( MouseIntersectRect( ImVec2( 100, 40 ), ImVec2( 280, 120 ) ) && Pool[ a ][ b ].ItemName != _S( "Box" ) ) {
					if ( a == IN_MOVE_COND ) {
						Pool[ a ][ b ].VectorCond = TOP_COND;
						Vector2Vector( Pool[ a ][ b ], TOP_COND );
					}
				}
				else if ( MouseIntersectRect( ImVec2( 100, 440 ), ImVec2( 280, 520 ) ) && Pool[ a ][ b ].ItemName != _S( "Box" ) ) {
					if ( a == IN_MOVE_COND ) {
						Pool[ a ][ b ].VectorCond = BOT_COND;
						Vector2Vector( Pool[ a ][ b ], BOT_COND );
					}
				}
				else if ( MouseIntersectRect( ImVec2( 100, 120 ), ImVec2( 280, 440 ) ) && Pool[ a ][ b ].ItemName == _S( "Box" ) ) {
					if ( a == IN_MOVE_COND ) {
						Pool[ a ][ b ].VectorCond = CENTER_COND;
						Vector2Vector( Pool[ a ][ b ], CENTER_COND );
					}
				}
				else {
					if ( ImGui::GetIO( ).MouseDownDuration[ 0 ] > 0.0f ) {
						Vector2Vector( Pool[ a ][ b ], IN_MOVE_COND );
					}
				}
			}

			if ( !ImGui::GetIO( ).MouseDown[ 0 ] )
				isMouseInAction = false;
		}
	}

	if ( isMouseInAction == false )
	{
		if ( RecalculateAnimationFlag == true ) {
			RecalculateAnimation = 0.f;
			RecalculateAnimationFlag = false;
		}

		if ( RecalculateAnimation < 1.f )
			RecalculateAnimation += 0.005f * ( 60 / ImGui::GetIO( ).Framerate );

		RecalculateAnimation = std::clamp( RecalculateAnimation, 0.f, 1.f );

		RecalculatePool( RecalculateAnimation );
		RecalculateLeftRight( RecalculateAnimation );
		RecalculateTopBot( RecalculateAnimation );

		if ( ImGui::IsMouseDragging( 0 ) && ImGui::IsMouseHoveringRect( ImGui::GetWindowPos( ), ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ) ) )
			g_Globals->m_Model.m_flModelRotation += ( ImGui::GetIO( ).MouseDelta.x + ImGui::GetIO( ).MouseDelta.y ) / 3;


	}

	IsHoveringStandingItem( );
	IsDragginAnyItem( );
}

void C_MergedMDL::SetupBonesForAttachmentQueries( )
{
	return ( ( void( __thiscall* )( void* ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetupBonesForAttachmentQueries ] ) )( this );
}

bool& IMaterialSystem::DisableRenderTargetAllocationForever( )
{
	return *reinterpret_cast< bool* >( reinterpret_cast< std::uintptr_t >( this ) + ( uint8_t* ) SDK::EngineData::m_AddressList[ CheatAddressList::DisableRenderTargetAllocationForever ] );
}

void C_MergedMDL::SetMergedMDL( const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner, void* pProxyData )
{
	return ( ( void( __thiscall* )( void*, const char*, CCustomMaterialOwner*, void*, bool ) )( SDK::EngineData::m_AddressList[ CheatAddressList::SetMergedMDL ] ) )( this, szModelPath, pCustomMaterialOwner, pProxyData, false );
}