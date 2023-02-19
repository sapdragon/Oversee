#include "GUI.hpp"
#include "../../Includes.hpp"
#include "../../Libraries/ImGui/imgui_tricks.hpp"

void C_UIElements::KeyBind( const char* szLabelName, int& m_KeyBind )
{
	ImGuiWindow* pWindow = ImGui::GetCurrentWindow( );
	ImGuiContext* m_Ctx = GImGui;

	ImGuiID nItemID = pWindow->GetID( szLabelName );
	ImGuiIO* pIO = &ImGui::GetIO( );
	ImGuiStyle* m_Style = &m_Ctx->Style;
	ImVec2 vecLabelSize = ImGui::CalcTextSize( szLabelName );

	char szLabelDisplay[ 64 ];
	ZeroMemory( szLabelDisplay, 64 );
	strcpy( szLabelDisplay, _S( "None" ) );

	ImVec2 vecPosition = ImGui::GetCursorScreenPos( );

	const float flWidth = 250;
	const float flHeight = 23.0f;
	const float flTotalHeight = 25.0f;

	ImVec2 vecFrameDrawPos = ImVec2( vecPosition.x, vecPosition.y );

	ImRect recTotal = ImRect( ImVec2( vecPosition.x, vecPosition.y ), ImVec2( vecFrameDrawPos.x + flWidth, vecFrameDrawPos.y + flTotalHeight ) );
	ImRect recFrame = ImRect( ImVec2( vecFrameDrawPos.x, recTotal.Min.y + ( ( recTotal.Max.y - recTotal.Min.y ) * 0.5f ) - ( flHeight * 0.5f ) ), ImVec2( vecFrameDrawPos.x + flWidth, recTotal.Min.y + ( ( recTotal.Max.y - recTotal.Min.y ) * 0.5f ) + ( flHeight * 0.5f ) ) );

	ImGui::ItemSize( recTotal, m_Style->FramePadding.y );
	if ( !ImGui::ItemAdd( recTotal, nItemID, &recFrame ) )
		return;

	bool bIsHovered = false;
	if ( ImGui::ItemHoverable( recFrame, nItemID ) )
		bIsHovered = true;

	bool bEditRequested = false;
	if ( bIsHovered && pIO->MouseClicked[ 0 ] )
		bEditRequested = true;

	bool bStyleRequested = false;
	if ( bIsHovered && pIO->MouseClicked[ 1 ] )
		bStyleRequested = true;

	bool bIsFocusRequested = false;
	if ( ImGui::FocusableItemRegister( pWindow, m_Ctx->ActiveId ) )
		bIsFocusRequested = true;

	if ( bEditRequested || bIsFocusRequested )
	{
		if ( m_Ctx->ActiveId != nItemID )
		{
			memset( pIO->MouseDown, 0, sizeof( pIO->MouseDown ) );
			memset( pIO->KeysDown, 0, sizeof( pIO->KeysDown ) );

			m_KeyBind = 0;
		}
		ImGui::SetActiveID( nItemID, pWindow );
		ImGui::FocusWindow( pWindow );
	}
	else if ( !bIsHovered && pIO->MouseClicked[ 0 ] && m_Ctx->ActiveId == nItemID )
		ImGui::ClearActiveID( );

	int nKeySelected = 0;
	if ( m_Ctx )
		nKeySelected = m_KeyBind;

	bool bIsValueChanged = false;
	if ( m_Ctx->ActiveId == nItemID )
	{
		for ( auto nMouseButton = 0; nMouseButton < 5; nMouseButton++ )
		{
			if ( pIO->MouseDown[ nMouseButton ] )
			{
				// handle the key
				switch ( nMouseButton )
				{
					case 0:
					nKeySelected = VK_LBUTTON;
					break;
					case 1:
					nKeySelected = VK_RBUTTON;
					break;
					case 2:
					nKeySelected = VK_MBUTTON;
					break;
					case 3:
					nKeySelected = VK_XBUTTON1;
					break;
					case 4:
					nKeySelected = VK_XBUTTON2;
					break;
				}

				// value was changed
				bIsValueChanged = true;

				// clear active ID
				ImGui::ClearActiveID( );
			}
		}

		if ( !bIsValueChanged )
		{
			for ( int nKey = VK_BACK; nKey <= VK_RMENU; nKey++ )
			{
				if ( !pIO->KeysDown[ nKey ] )
					continue;

				// set key selected
				nKeySelected = nKey;

				// value was changed
				bIsValueChanged = true;

				// clear active ID
				ImGui::ClearActiveID( );
			}
		}

		// reset on escape
		if ( ImGui::IsKeyPressedMap( ImGuiKey_Escape ) )
		{
			// reset the active key
			nKeySelected = 0;

			// clear active ID
			ImGui::ClearActiveID( );
		}

		// set the active key
		m_KeyBind = nKeySelected;
	}

	// define the buffer
	char aBufferToDisplay[ 64 ];
	ZeroMemory( aBufferToDisplay, 64 );

	// fill the buffer
	if ( m_KeyBind != 0 && m_Ctx->ActiveId != nItemID )
		strcpy_s( aBufferToDisplay, keys[ m_KeyBind ] );
	else if ( m_Ctx->ActiveId == nItemID )
		strcpy_s( aBufferToDisplay, _S( "..." ) );

	// render the main frame
	pWindow->DrawList->AddRectFilled( recFrame.Min, recFrame.Max, ImColor( 55, 55, 55, 255 ), 2.0f );
	pWindow->DrawList->AddRect( recFrame.Min, recFrame.Max, ImColor( 255, 255, 255, 25 ), 2.0f );

	// render the key text
	pWindow->DrawList->AddText
	(
		ImVec2
		(
			( ( recFrame.Min.x + recFrame.Max.x ) / 2 ) - ( ImGui::CalcTextSize( aBufferToDisplay ).x / 2 ),
			( ( recFrame.Min.y + recFrame.Max.y ) / 2 ) - ( ImGui::CalcTextSize( aBufferToDisplay ).y / 2 )
		),

		m_Ctx->ActiveId == nItemID ? ImColor( 220, 220, 220 ) : ImColor( 240, 240, 240 ),
		aBufferToDisplay
	);
}

void C_UIElements::Tab( const char* szLabelName, const char* szFullName, int& tab, int index, bool wide )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );
	const ImVec2 label_size = ImGui::CalcTextSize( szLabelName, NULL, true );
	const ImVec2 fulllabel_size = ImGui::CalcTextSize( szFullName, NULL, true );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { float( wide ? 160 : 40 ), 40 }, 40, 40 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed )
		tab = index, this->swapAnimation = 0;

	std::string label = std::string( szLabelName ) + "Hovered";

	float Active = ImTricks::Animations::FastFloatLerp( id, tab == index, 0.f, 1.f, 0.02f );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label.c_str( ) ), hovered, 0.f, 1.f, 0.02f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 190, 190, 190, 150 ), ImColor( 240, 240, 240 ), Active );

	window->DrawList->AddRectFilled( bb.Min + ImVec2( -8, 5 ), bb.Min + ImVec2( 3, 35 ), ImColor( 0, 153, 255, int( 255 * Active ) ), 3 );

	ImGui::PushFont( g_Menu->m_sFonts.MenuIcons );
	window->DrawList->AddText( bb.Min + ImVec2( 15 - label_size.x / 2, 17 - label_size.y / 2 ), Color, szLabelName );
	window->DrawList->AddText( bb.Min + ImVec2( 15 - label_size.x / 2, 17 - label_size.y / 2 ), ImColor( 240, 240, 240, int( 170 * Hovered ) ), szLabelName );
	ImGui::PopFont( );
}

ImColor RarityGetter( int rarity ) {
	if ( rarity == 1 )
		return ImColor( 0.69f, 0.76f, 0.85f, 1.f );
	else if ( rarity == 2 )
		return ImColor( 0.29f, 0.41f, 1.f, 1.f );
	else if ( rarity == 3 )
		return ImColor( 0.36f, 0.60f, 0.85f, 1.f );
	else if ( rarity == 4 )
		return ImColor( 0.53f, 0.27f, 1.f, 1.f );
	else if ( rarity == 5 )
		return ImColor( 0.82f, 0.17f, 0.90f, 1.f );
	else if ( rarity == 6 )
		return ImColor( 0.92f, 0.29f, 0.29f, 1.f );
	else if ( rarity == 7 )
		return ImColor( 0.89f, 0.68f, 0.22f, 1.f );
}

std::string ToUtf888( std::wstring_view wstr ) {

	if ( wstr.empty( ) )
		return std::string( );

	auto size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int32_t >( wstr.size( ) ), NULL, 0, NULL, NULL );

	std::string out( size_needed, 0 );
	WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], static_cast< int >( wstr.size( ) ), &out[ 0 ], size_needed, NULL, NULL );

	return out;
}

std::string LocalizeTex22( const char* in ) {

	const auto wide_name = SDK::Interfaces::Localize->FindSafe( in );

	if ( !wcslen( wide_name ) )
		return "";

	return ToUtf888( wide_name );
}


bool C_UIElements::InventoryItem( InventoryItem_t Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::to_string( Item.m_nItemID ).c_str( ) );
	const ImVec2 label_size = ImGui::CalcTextSize( Item.SkinName.c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( Item.m_iRarity ), 3, ImDrawCornerFlags_Bot );

	bool renderedImage = false;

	if ( renderedImage == false )
		for ( const auto& weapon : g_ItemManager.m_vWeapons ) {
			if ( weapon->m_tPaintKit->m_nID == Item.m_iPaintKit && weapon->m_tItem->m_nID == Item.m_nItemID ) {
				window->DrawList->AddImage( weapon->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) );
				renderedImage = true;
			}
		}

	if ( renderedImage == false )
		for ( const auto& weapon : g_ItemManager.m_vAgents ) {
			if ( weapon->m_nItemID == Item.m_nItemID ) {
				window->DrawList->AddImage( weapon->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) );
				renderedImage = true;
			}
		}


	if ( renderedImage == false ) {
		auto time = static_cast< float >( g.Time ) * 1.8f;
		window->DrawList->PathClear( );
		int start = static_cast< int >( abs( ImSin( time ) * ( 32 - 5 ) ) );
		const float a_min = IM_PI * 2.0f * ( ( float ) start ) / ( float ) 32;
		const float a_max = IM_PI * 2.0f * ( ( float ) 32 - 3 ) / ( float ) 32;
		const auto&& centre = ImVec2( pos.x + 70, pos.y + 60 );
		for ( auto i = 0; i < 32; i++ ) {
			const float a = a_min + ( ( float ) i / ( float ) 32 ) * ( a_max - a_min );
			window->DrawList->PathLineTo( { centre.x + ImCos( a + time * 8 ) * 20,
										  centre.y + ImSin( a + time * 8 ) * 20 } );
		}
		window->DrawList->PathStroke(ImColor(190, 190, 190), false, 4 );
	}


	window->DrawList->PushClipRect( bb.Min, bb.Max, true );
	window->DrawList->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), Item.SkinName.c_str( ) );
	window->DrawList->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}

bool C_UIElements::InventoryItemDefault( DefaultItem_t* Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::to_string( Item->m_nItemID ).c_str( ) );
	const ImVec2 label_size = ImGui::CalcTextSize( LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( 1 ), 3, ImDrawCornerFlags_Bot );
	window->DrawList->AddImage( Item->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) ); // 108  68

	ImGui::GetWindowDrawList( )->PushClipRect( bb.Min, bb.Max, true );
	ImGui::GetWindowDrawList( )->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ) );
	ImGui::GetWindowDrawList( )->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}

bool C_UIElements::InventoryItemAgent( AgentsItem_t* Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::to_string( Item->m_nItemID ).c_str( ) );
	const ImVec2 label_size = ImGui::CalcTextSize( LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( 1 ), 3, ImDrawCornerFlags_Bot );
	window->DrawList->AddImage( Item->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) ); // 108  68

	ImGui::GetWindowDrawList( )->PushClipRect( bb.Min, bb.Max, true );
	ImGui::GetWindowDrawList( )->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ) );
	ImGui::GetWindowDrawList( )->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}

bool C_UIElements::InventoryItemMusic( MusicItem_t* Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::to_string( Item->m_tPaintKit->m_nID ).c_str( ) );
	const ImVec2 label_size = ImGui::CalcTextSize( LocalizeTex22( Item->m_tPaintKit->m_szLocalizatedName ).c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( 1 ), 3, ImDrawCornerFlags_Bot );
	window->DrawList->AddImage( Item->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) ); // 108  68

	ImGui::GetWindowDrawList( )->PushClipRect( bb.Min, bb.Max, true );
	ImGui::GetWindowDrawList( )->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), LocalizeTex22( Item->m_tPaintKit->m_szLocalizatedName ).c_str( ) );
	ImGui::GetWindowDrawList( )->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}

bool C_UIElements::InventoryItemGlove( GloveItem_t* Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::to_string( Item->m_nKitID ).c_str( ) );
	const ImVec2 label_size = ImGui::CalcTextSize( LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( 1 ), 3, ImDrawCornerFlags_Bot );
	window->DrawList->AddImage( Item->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) ); // 108  68

	ImGui::GetWindowDrawList( )->PushClipRect( bb.Min, bb.Max, true );
	ImGui::GetWindowDrawList( )->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ) );
	ImGui::GetWindowDrawList( )->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}

bool C_UIElements::InventoryItem( WeaponItem_t* Item )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( Item->m_tPaintKit->m_szName );
	const ImVec2 label_size = ImGui::CalcTextSize( LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ), NULL, true );
	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 140, 142 }, 142, 142 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	ImGui::ItemAdd( bb, id );

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 58, 58, 58 ), 3 );
	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 140 ), bb.Max, RarityGetter( 1 ), 3, ImDrawCornerFlags_Bot );
	window->DrawList->AddImage( Item->m_tTexture, bb.Min + ImVec2( 17, 37 - 10 ), bb.Min + ImVec2( 125, 105 - 10 ) ); // 108  68

	ImGui::GetWindowDrawList( )->PushClipRect( bb.Min, bb.Max, true );
	ImGui::GetWindowDrawList( )->AddText( bb.Min + ImVec2( 10, 142 - label_size.y - 10 ), ImColor( 255, 255, 255 ), LocalizeTex22( Item->m_szName.c_str( ) ).c_str( ) );
	ImGui::GetWindowDrawList( )->PopClipRect( );

	window->DrawList->AddRectFilledMultiColor( bb.Min + ImVec2( 110, 142 - label_size.y - 10 ), bb.Max - ImVec2( 0, 6 ),
		ImColor( 58, 58, 58, 0 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 255 ), ImColor( 58, 58, 58, 0 ) );

	return pressed;
}


void C_UIElements::Subtab( const char* szLabelName, int& tab, int index )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );
	const ImVec2 label_size = ImGui::CalcTextSize( szLabelName, NULL, true );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 30 + label_size.x, 35 }, 40, 40 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed )
		tab = index;

	std::string hoverid = szLabelName + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), hovered, 0.f, 1.f, 0.05f );
	float Active = ImTricks::Animations::FastFloatLerp( id, tab == index, 0.f, 1.f, 0.03f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 120, 120, 120, 0 ), ImColor( 120, 120, 120, 150 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color );
	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 120, 120, 120, int( 110 * Hovered ) ) );
	window->DrawList->AddText( bb.Min + ImVec2( 15, 17 - label_size.y / 2 ), ImColor( 240, 240, 240 ), szLabelName );
}

void C_UIElements::WeaponSelect( const char* szLabelName, const char* icon, std::string& value )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );
	const ImVec2 label_size = ImGui::CalcTextSize( szLabelName, NULL, true );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 68.f, 35 }, 40, 40 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed ) {
		value = szLabelName;
	}

	std::string hoverid = szLabelName + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), hovered, 0.f, 1.f, 0.05f );
	float Active = ImTricks::Animations::FastFloatLerp( id, value == szLabelName, 0.f, 1.f, 0.03f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 120, 120, 120, 0 ), ImColor( 120, 120, 120, 150 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color );
	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 120, 120, 120, int( 110 * Hovered ) ) );

	ImGui::PushFont( g_Menu->m_sFonts.WeapIcons );
	window->DrawList->AddText( g_Menu->m_sFonts.WeapIcons, 16, bb.Min + ImVec2( 34, 18 ) - ( ( ImGui::CalcTextSize( icon, NULL, true ) / 2 ) * 0.8f ), ImColor( 240, 240, 240 ), icon );
	ImGui::PopFont( );
}

void C_UIElements::DashboardItem( const char* szLabelName, bool& v )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );
	const ImVec2 label_size = ImGui::CalcTextSize( szLabelName, NULL, true );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 120, 25 }, 40, 40 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed )
		v = !( v );

	std::string hoverid = szLabelName + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), hovered, 0.f, 1.f, 0.05f );
	float Active = ImTricks::Animations::FastFloatLerp( id, v, 0.f, 1.f, 0.03f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 60, 60, 60, 0 ), ImColor( 90, 90, 90, 150 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color, 3 );
	window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( 80, 80, 80, int( 110 * Hovered ) ), 3 );
	window->DrawList->AddText( bb.Min + ImVec2( 15, 12 - label_size.y / 2 ), ImColor( 240, 240, 240 ), szLabelName );
}

void C_UIElements::SameLineWithoutPadding( float offset_from_start_x, float spacing_w )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return;

	if ( offset_from_start_x != 0.0f )
	{
		window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
		window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
	}
	else
	{
		window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x;
		window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
	}

	window->DC.CurrLineSize = window->DC.PrevLineSize;
	window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

std::string timeStampToHReadble( long  timestamp )
{
	const time_t rawtime = ( const time_t ) timestamp;

	struct tm* dt;
	char timestr[ 30 ];
	char buffer[ 30 ];

	dt = localtime( &rawtime );
	// use any strftime format spec here
	strftime( timestr, sizeof( timestr ), "%m/%d/%y %H:%M:%S", dt );
	sprintf( buffer, "%s", timestr );
	std::string stdBuffer( buffer );
	return stdBuffer;
}

void C_UIElements::Configuration( CloudConfig_t config )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( config.SecureID.c_str( ) );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 580, 60 }, 295, 75 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	float Active = ImTricks::Animations::FastFloatLerp( id, hovered, 0.f, 1.f, 0.03f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 65, 65, 65 ), ImColor( 75, 75, 75 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color, 5 );
	window->DrawList->AddRectFilled( bb.Min, bb.Min + ImVec2( 60, 60 ), ImColor( 34, 34, 34 ), 5 );

	window->DrawList->AddRectFilled( bb.Max - ImVec2( 48, 48 ), bb.Max - ImVec2( 12, 12 ), ImColor( 0, 153, 255 ), 5 );
	window->DrawList->AddText( g_Menu->m_sFonts.MenuIcons, 16, bb.Max - ImVec2( 48 - 11, 48 - 10 ), ImColor( 240, 240, 240 ), "d" );
	if ( ImGui::IsItemClicked( ) && ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 48, 48 ), bb.Max - ImVec2( 12, 12 ) ) )
		strcpy( g_CloudConfigs->aConfigID, config.SecureID.c_str( ) ), g_CloudConfigs->CloudAction = CloudActions_t::GetConfigData;

	window->DrawList->AddRectFilled( bb.Max - ImVec2( 78, 48 - 6 ), bb.Max - ImVec2( 54, 12 + 6 ), ImColor( 99, 99, 99 ), 5 );
	window->DrawList->AddText( g_Menu->m_sFonts.MenuIcons, 14, bb.Max - ImVec2( 78 - 5, 48 - 10 ), ImColor( 240, 240, 240 ), "c" );
	if ( ImGui::IsItemClicked( ) && ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 78, 42 ), bb.Max - ImVec2( 54, 18 ) ) )
		strcpy( g_CloudConfigs->aConfigID, config.SecureID.c_str( ) ), g_CloudConfigs->CloudAction = CloudActions_t::SaveConfig;

	window->DrawList->AddText( bb.Min + ImVec2( 70, 7 ), ImColor( 240, 240, 240 ), config.Name.c_str( ) );
	window->DrawList->AddText( bb.Min + ImVec2( 70, 23 ), ImColor( 240, 240, 240 ), "by" );
	window->DrawList->AddText( bb.Min + ImVec2( 86, 23 ), ImColor( 0, 153, 255 ), config.Author.c_str( ) );
	window->DrawList->AddText( bb.Min + ImVec2( 70, 39 ), ImColor( 190, 190, 190 ), timeStampToHReadble( config.UpdatedAt ).c_str( ) );
}

void C_UIElements::Script( CloudScript_t config )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( config.SecureID.c_str( ) );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 580, 60 }, 295, 75 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	float Active = ImTricks::Animations::FastFloatLerp( id, hovered, 0.f, 1.f, 0.03f );
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 65, 65, 65 ), ImColor( 75, 75, 75 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color, 5 );
	window->DrawList->AddRectFilled( bb.Min, bb.Min + ImVec2( 60, 60 ), ImColor( 34, 34, 34 ), 5 );

	window->DrawList->AddRectFilled( bb.Max - ImVec2( 48, 48 ), bb.Max - ImVec2( 12, 12 ), g_LuaAPI->Loaded.at( config.ID ) ? ImColor( 0, 153, 255 ) : ImColor( 99, 99, 99 ), 5 );
	window->DrawList->AddText( g_Menu->m_sFonts.MenuIcons, 16, bb.Max - ImVec2( 48 - 11, 48 - 10 ), ImColor( 240, 240, 240 ), "d" );
	if ( ImGui::IsItemClicked( ) && ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 48, 48 ), bb.Max - ImVec2( 12, 12 ) ) ) {
		if ( !g_LuaAPI->Loaded.at( config.ID ) )
			g_LuaAPI->Load( config.ID );
		else
			g_LuaAPI->Unload( config.ID );
	}

	if ( config.Author == config.Owner )
	{
		if ( g_LuaAPI->InEdit.at( config.ID ) && g_LuaAPI->InEditMD5.at( config.ID ) != config.MD5 )
		{
			window->DrawList->AddRectFilled( bb.Max - ImVec2( 78, 48 - 6 ), bb.Max - ImVec2( 54, 12 + 6 ), ImColor( 99, 99, 99 ), 5 );
			window->DrawList->AddText( g_Menu->m_sFonts.MenuIcons, 14, bb.Max - ImVec2( 78 - 5, 48 - 10 ), ImColor( 240, 240, 240 ), "c" );
			if ( ImGui::IsItemClicked( ) && ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 78, 42 ), bb.Max - ImVec2( 54, 18 ) ) ) {
				strcpy( g_CloudScripts->aConfigID, config.SecureID.c_str( ) );

				g_CloudScripts->iScriptFileIndex = config.ID;
				g_CloudScripts->CloudAction = CloudScriptAction_t::SaveScript;

				g_LuaAPI->InEditManager( config.ID, false );
			}
		}

		if ( g_LuaAPI->InEdit.at( config.ID ) == false ) {
			window->DrawList->AddRectFilled( bb.Max - ImVec2( 78 + 29, 48 - 6 ), bb.Max - ImVec2( 54 + 29, 12 + 6 ), ImColor( 99, 99, 99 ), 5 );
			window->DrawList->AddText( bb.Max - ImVec2( 78 - 5 + 29, 48 - 10 ), ImColor( 240, 240, 240 ), "E" );
			if ( ImGui::IsItemClicked( ) && ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 78 + 29, 48 - 6 ), bb.Max - ImVec2( 54 + 29, 12 + 6 ) ) )
			{
				g_LuaAPI->InEditManager( config.ID, true );

				std::ofstream outfile( config.SecureID + ".lua" );
				outfile << config.Data << std::endl;
				outfile.close( );

				ShellExecute( 0, 0, std::string( config.SecureID + ".lua" ).c_str( ), 0, 0, SW_SHOW );
			}
		}
	}

	window->DrawList->AddText( bb.Min + ImVec2( 70, 7 ), ImColor( 240, 240, 240 ), config.Name.c_str( ) );
	window->DrawList->AddText( bb.Min + ImVec2( 70, 23 ), ImColor( 240, 240, 240 ), "by" );
	window->DrawList->AddText( bb.Min + ImVec2( 86, 23 ), ImColor( 0, 153, 255 ), config.Author.c_str( ) );
	window->DrawList->AddText( bb.Min + ImVec2( 70, 39 ), ImColor( 190, 190, 190 ), timeStampToHReadble( config.UpdatedAt ).c_str( ) );
}

bool BeginChildEx( const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags )
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* parent_window = g.CurrentWindow;

	flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
	flags |= ( parent_window->Flags & ImGuiWindowFlags_NoMove );  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = ImGui::GetContentRegionAvail( );
	ImVec2 size = ImFloor( size_arg );
	const int auto_fit_axises = ( ( size.x == 0.0f ) ? ( 1 << ImGuiAxis_X ) : 0x00 ) | ( ( size.y == 0.0f ) ? ( 1 << ImGuiAxis_Y ) : 0x00 );
	if ( size.x <= 0.0f )
		size.x = ImMax( content_avail.x + size.x, 4.0f ); // Arbitrary minimum child size (0.0f causing too much issues)
	if ( size.y <= 0.0f )
		size.y = ImMax( content_avail.y + size.y, 4.0f );
	ImGui::SetNextWindowSize( size );

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
	if ( name )
		ImFormatString( g.TempBuffer, IM_ARRAYSIZE( g.TempBuffer ), "%s/%s_%08X", parent_window->Name, name, id );
	else
		ImFormatString( g.TempBuffer, IM_ARRAYSIZE( g.TempBuffer ), "%s/%08X", parent_window->Name, id );

	const float backup_border_size = g.Style.ChildBorderSize;
	if ( !border )
		g.Style.ChildBorderSize = 0.0f;
	bool ret = ImGui::Begin( g.TempBuffer, NULL, flags );
	g.Style.ChildBorderSize = backup_border_size;

	ImGuiWindow* child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = ( ImS8 ) auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if ( child_window->BeginCount == 1 )
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if ( g.NavActivateId == id && !( flags & ImGuiWindowFlags_NavFlattened ) && ( child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll ) )
	{
		ImGui::FocusWindow( child_window );
		ImGui::NavInitWindow( child_window, false );
		ImGui::SetActiveID( id + 1, child_window ); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
		g.ActiveIdSource = ImGuiInputSource_Nav;
	}


	auto positon = ImGui::GetWindowPos( );

	float Hovered = ImTricks::Animations::FastFloatLerp( id, ImGui::IsMouseHoveringRect( positon, positon + size_arg ), 0.f, 1.f, 0.02f );

	parent_window->DrawList->AddRectFilled( positon, positon + size_arg, ImColor( 66 + int( Hovered * 3 ), 66 + int( Hovered * 3 ), 66 + int( Hovered * 3 ) ), 4 );
	parent_window->DrawList->AddRect( positon, positon + size_arg, ImColor( 25, 25, 25, 140 ), 4 );

	return ret;
}

bool BeginChildExEx( const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	return BeginChildEx( str_id, window->GetID( str_id ), size_arg, border, extra_flags );
}

void EndChildEx( )
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	IM_ASSERT( g.WithinEndChild == false );
	IM_ASSERT( window->Flags & ImGuiWindowFlags_ChildWindow );   // Mismatched BeginChild()/EndChild() calls

	g.WithinEndChild = true;
	if ( window->BeginCount > 1 )
	{
		ImGui::End( );
	}
	else
	{
		ImVec2 sz = window->Size;
		if ( window->AutoFitChildAxises & ( 1 << ImGuiAxis_X ) ) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax( 4.0f, sz.x );
		if ( window->AutoFitChildAxises & ( 1 << ImGuiAxis_Y ) )
			sz.y = ImMax( 4.0f, sz.y );
		ImGui::End( );

		ImGuiWindow* parent_window = g.CurrentWindow;
		ImRect bb( parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz );
		ImGui::ItemSize( sz );
		if ( ( window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll ) && !( window->Flags & ImGuiWindowFlags_NavFlattened ) )
		{
			ImGui::ItemAdd( bb, window->ChildId );
			ImGui::RenderNavHighlight( bb, window->ChildId );

			// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
			if ( window->DC.NavLayerActiveMask == 0 && window == g.NavWindow )
				ImGui::RenderNavHighlight( ImRect( bb.Min - ImVec2( 2, 2 ), bb.Max + ImVec2( 2, 2 ) ), g.NavId, ImGuiNavHighlightFlags_TypeThin );
		}
		else
		{
			// Not navigable into
			ImGui::ItemAdd( bb, 0 );
		}
	}
	g.WithinEndChild = false;
	g.LogLinePosY = -FLT_MAX;
}

void C_UIElements::BeginChild( const char* str_id, const ImVec2 size_arg, bool border, ImGuiWindowFlags extra_flags )
{
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 7 } );
	::BeginChildExEx( str_id, size_arg, border, extra_flags | ImGuiWindowFlags_NoScrollbar );
	ImGui::SetCursorPos( { 10, 10 } );
	ImGui::BeginGroup( );
}

void C_UIElements::EndChild( )
{
	ImGui::EndGroup( );
	::EndChildEx( );
	ImGui::PopStyleVar( );
}

std::string PenetrateByAnalMapKey( std::string in, std::string renderable ) {

	if ( in.find( "ragebot.auto" ) != std::string::npos ) {
		return "Auto | " + renderable;
	}
	if ( in.find( "ragebot.scout" ) != std::string::npos ) {
		return "Scout | " + renderable;
	}
	if ( in.find( "ragebot.awp" ) != std::string::npos ) {
		return "AWP | " + renderable;
	}
	if ( in.find( "ragebot.deagle" ) != std::string::npos ) {
		return "Deagle | " + renderable;
	}
	if ( in.find( "ragebot.revolver" ) != std::string::npos ) {
		return "Revolver | " + renderable;
	}
	if ( in.find( "ragebot.pistol" ) != std::string::npos ) {
		return "Pistol | " + renderable;
	}
	if ( in.find( "ragebot.rifle" ) != std::string::npos ) {
		return "Rifle | " + renderable;
	}
	if ( in.find( "ragebot.shotgun" ) != std::string::npos ) {
		return "Shotgun | " + renderable;
	}
	if ( in.find( "ragebot.smg" ) != std::string::npos ) {
		return "SMG | " + renderable;
	}

	return renderable;
}

std::string getTypeString( OnKeyAction_t key ) {
	if ( key.m_tActionType == 0 )
		return "Swap | " + std::string( keys[ key.m_uiKeyID ] );
	if ( key.m_tActionType == 1 )
		return "Hold On | " + std::string( keys[ key.m_uiKeyID ] );
	if ( key.m_tActionType == 2 )
		return "Hold Off | " + std::string( keys[ key.m_uiKeyID ] );
};

void C_UIElements::Checkbox( const char* szLabelName, std::string value, std::function<void( )> addictional_settings, float height, OverallElementsFlag flags )
{
	bool searched = std::string( szLabelName ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( szLabelName ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 270, 20 }, 290, 25 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed && ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) ) g_SettingsManager->B[ value ] = !( g_SettingsManager->B[ value ] );

	std::string label_hovered = std::string( szLabelName ) + "Hovered";

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );

	float Active = ImTricks::Animations::FastFloatLerp( id, g_SettingsManager->B[ value ], 0.f, 1.f, 0.02f );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label_hovered.c_str( ) ), hovered, 0.f, 1.f, 0.02f );

	Active = Active * swapAnimation;
	Hovered = Hovered * swapAnimation;

	window->DrawList->AddText( bb.Min + ImVec2( 27, 3 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), szLabelName );
	window->DrawList->AddText( bb.Min + ImVec2( 27, 3 ), ImColor( 240, 240, 240, int( 255 * Hovered ) ), szLabelName );

	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 0 ), bb.Min + ImVec2( 20, 20 ), ImColor( 42, 42, 42, int( 255 - ( 150 * Searched ) ) ), 4 );
	window->DrawList->AddRect( bb.Min + ImVec2( 0, 0 ), bb.Min + ImVec2( 20, 20 ), ImColor( 90, 90, 90, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImGui::RenderCheckMark( window->DrawList, bb.Min + ImVec2( 4, 4 ), ImColor( 190, 190, 190 ), 11 );

	window->DrawList->AddRectFilled( bb.Min + ImVec2( 2 + 16 * Active, 2 ), bb.Min + ImVec2( 18, 18 ), ImColor( 42, 42, 42, int( 255 - ( 150 * Searched ) ) ), 4 );

	std::string PopupAddictional = std::string( szLabelName ) + std::string( "PopupAddictional" );

	window->DrawList->AddText( g_Menu->m_sFonts.MenuIcons, 16, bb.Max - ImVec2( 17, 17 ), ImColor( 240, 240, 240, int( 255 - ( 150 * Searched ) ) ), "9" );

	if ( ImGui::BeginPopup( PopupAddictional.c_str( ), ImGuiWindowFlags_NoMove ) )
	{
		ImGui::SetWindowPos( bb.Min + ImVec2( 0, 25 ) );

		ImGui::SetCursorPos( { 10, 10 } );
		ImGui::BeginChild( "Addictional", { 250, height } );
		{
			addictional_settings( );
		}
		ImGui::EndChild( );
		ImGui::Dummy( { 270, 7 } );

		ImGui::EndPopup( );
	}

	if ( ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 20, 20 ), bb.Max ) && ImGui::GetIO( ).MouseDown[ 0 ] )
		ImGui::OpenPopup( PopupAddictional.c_str( ) );

	if ( !( flags & OverallFlags_NotBindable ) ) {

		auto mapVal = ImTricks::bindsMapInt.find( id );
		if ( mapVal == ImTricks::bindsMapInt.end( ) ) {
			ImTricks::bindsMapInt.insert( { id, -1 } );
			mapVal = ImTricks::bindsMapInt.find( id );
		}

		auto mapVal2 = ImTricks::bindsMapString.find( id );
		if ( mapVal2 == ImTricks::bindsMapString.end( ) ) {
			ImTricks::bindsMapString.insert( { id, "Create New" } );
			mapVal2 = ImTricks::bindsMapString.find( id );
		}

		std::string popup = std::string( szLabelName ) + std::string( "Popup" );

		if ( ImGui::BeginPopup( popup.c_str( ), ImGuiWindowFlags_NoMove ) )
		{
			ImGui::SetWindowPos( bb.Min + ImVec2( 0, 25 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( "KeyBind", { 250, 140 } );
			{
				ImGui::PushItemWidth( 250 );
				static OnKeyAction_t tempValue;

				if ( BeginCombo( "Binds", mapVal2->second.c_str( ), NULL, 250 ) ) {
					if ( ImGui::Selectable( "Create New", mapVal->second == -1 ) )
						mapVal->second = -1, mapVal2->second = "Create New";

					for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
						if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
							if ( ImGui::Selectable( getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), mapVal->second == a ) )
								mapVal->second = a, mapVal2->second = getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( );
					}

					ImGui::EndCombo( );
				}

				ImGui::Separator( );

				if ( mapVal->second == -1 )
				{
					ImGui::Combo( "##Mode", &tempValue.m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", tempValue.m_uiKeyID );

					if ( ImGui::Button( "Create Bind", { 250, 22 } ) ) {
						tempValue.m_tVariableType = KeyVariableType::BOOL_KEY_TYPE;
						tempValue.m_sRenderableName = PenetrateByAnalMapKey( value, szLabelName );
						tempValue.m_sVariableMapName = value;

						if ( tempValue.m_tActionType == 0 ) {
							tempValue.defaultValue = false;
							tempValue.newValue = true;
						}
						if ( tempValue.m_tActionType == 1 ) {
							tempValue.defaultValue = false;
							tempValue.newValue = true;
						}
						if ( tempValue.m_tActionType == 2 ) {
							tempValue.defaultValue = true;
							tempValue.newValue = false;
						}

						g_KeyManager->RegisterKey( tempValue );

						tempValue = OnKeyAction_t( );
					}
				}
				else
				{
					ImGui::Combo( "##Mode", &g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", g_KeyManager->m_vKeyActions[ mapVal->second ].m_uiKeyID );

					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 0 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = false;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = true;
					}
					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 1 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = false;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = true;
					}
					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 2 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = true;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = false;
					}

					if ( ImGui::Button( "Delete Bind", { 250, 22 } ) ) {
						g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + mapVal->second );
						mapVal->second = -1;
						mapVal2->second = "Create New";
					}
				}
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 270, 7 } );

			ImGui::EndPopup( );
		}

		if ( ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) && ImGui::GetIO( ).MouseDown[ 1 ] )
			ImGui::OpenPopup( popup.c_str( ) );
	}
}

void C_UIElements::Checkbox( const char* szLabelName, std::string value, OverallElementsFlag flags )
{
	bool searched = std::string( szLabelName ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( szLabelName ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 270, 20 }, 290, 25 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed && ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) ) g_SettingsManager->B[ value ] = !( g_SettingsManager->B[ value ] );

	std::string label_hovered = std::string( szLabelName ) + "Hovered";

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );

	float Active = ImTricks::Animations::FastFloatLerp( id, g_SettingsManager->B[ value ], 0.f, 1.f, 0.02f );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label_hovered.c_str( ) ), hovered, 0.f, 1.f, 0.02f );

	Active = Active * swapAnimation;
	Hovered = Hovered * swapAnimation;

	window->DrawList->AddText( bb.Min + ImVec2( 27, 3 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), szLabelName );
	window->DrawList->AddText( bb.Min + ImVec2( 27, 3 ), ImColor( 240, 240, 240, int( 255 * Hovered ) ), szLabelName );

	window->DrawList->AddRectFilled( bb.Min + ImVec2( 0, 0 ), bb.Min + ImVec2( 20, 20 ), ImColor( 42, 42, 42, int( 255 - ( 150 * Searched ) ) ), 4 );
	window->DrawList->AddRect( bb.Min + ImVec2( 0, 0 ), bb.Min + ImVec2( 20, 20 ), ImColor( 90, 90, 90, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImGui::RenderCheckMark( window->DrawList, bb.Min + ImVec2( 4, 4 ), ImColor( 190, 190, 190 ), 11 );

	window->DrawList->AddRectFilled( bb.Min + ImVec2( 2 + 16 * Active, 2 ), bb.Min + ImVec2( 18, 18 ), ImColor( 42, 42, 42, int( 255 - ( 150 * Searched ) ) ), 4 );

	if ( !( flags & OverallFlags_NotBindable ) ) {

		auto mapVal = ImTricks::bindsMapInt.find( id );
		if ( mapVal == ImTricks::bindsMapInt.end( ) ) {
			ImTricks::bindsMapInt.insert( { id, -1 } );
			mapVal = ImTricks::bindsMapInt.find( id );
		}

		auto mapVal2 = ImTricks::bindsMapString.find( id );
		if ( mapVal2 == ImTricks::bindsMapString.end( ) ) {
			ImTricks::bindsMapString.insert( { id, "Create New" } );
			mapVal2 = ImTricks::bindsMapString.find( id );
		}

		std::string popup = std::string( szLabelName ) + std::string( "Popup" );

		if ( ImGui::BeginPopup( popup.c_str( ), ImGuiWindowFlags_NoMove ) )
		{
			ImGui::SetWindowPos( bb.Min + ImVec2( 0, 25 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( "KeyBind", { 250, 140 } );
			{
				ImGui::PushItemWidth( 250 );
				static OnKeyAction_t tempValue;

				if ( BeginCombo( "Binds", mapVal2->second.c_str( ), NULL, 250 ) ) {
					if ( ImGui::Selectable( "Create New", mapVal->second == -1 ) )
						mapVal->second = -1, mapVal2->second = "Create New";

					for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
						if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
							if ( ImGui::Selectable( getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), mapVal->second == a ) )
								mapVal->second = a, mapVal2->second = getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( );
					}

					ImGui::EndCombo( );
				}

				ImGui::Separator( );

				if ( mapVal->second == -1 )
				{
					ImGui::Combo( "##Mode", &tempValue.m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", tempValue.m_uiKeyID );

					if ( ImGui::Button( "Create Bind", { 250, 22 } ) ) {
						tempValue.m_tVariableType = KeyVariableType::BOOL_KEY_TYPE;
						tempValue.m_sRenderableName = PenetrateByAnalMapKey( value, szLabelName );
						tempValue.m_sVariableMapName = value;

						if ( tempValue.m_tActionType == 0 ) {
							tempValue.defaultValue = false;
							tempValue.newValue = true;
						}
						if ( tempValue.m_tActionType == 1 ) {
							tempValue.defaultValue = false;
							tempValue.newValue = true;
						}
						if ( tempValue.m_tActionType == 2 ) {
							tempValue.defaultValue = true;
							tempValue.newValue = false;
						}

						g_KeyManager->RegisterKey( tempValue );

						tempValue = OnKeyAction_t( );
					}
				}
				else
				{
					ImGui::Combo( "##Mode", &g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", g_KeyManager->m_vKeyActions[ mapVal->second ].m_uiKeyID );

					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 0 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = false;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = true;
					}
					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 1 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = false;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = true;
					}
					if ( g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType == 2 ) {
						g_KeyManager->m_vKeyActions[ mapVal->second ].defaultValue = true;
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = false;
					}

					if ( ImGui::Button( "Delete Bind", { 250, 22 } ) ) {
						g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + mapVal->second );
						mapVal->second = -1;
						mapVal2->second = "Create New";
					}
				}
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 270, 7 } );

			ImGui::EndPopup( );
		}

		if ( ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) && ImGui::GetIO( ).MouseDown[ 1 ] )
			ImGui::OpenPopup( popup.c_str( ) );
	}
}

void C_UIElements::MasterSwitch( const char* szLabelName, std::string value )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 36, 18 }, 290, 25 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed && ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) ) g_SettingsManager->B[ value ] = !( g_SettingsManager->B[ value ] );

	std::string label_hovered = std::string( szLabelName ) + "Hovered";

	float Active = ImTricks::Animations::FastFloatLerp( id, g_SettingsManager->B[ value ], 0.f, 1.f, 0.02f );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label_hovered.c_str( ) ), hovered, 0.f, 1.f, 0.02f );

	Active = Active * swapAnimation;
	Hovered = Hovered * swapAnimation;

	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 78, 78, 78 ), ImColor( 51, 130, 1 ), Active );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, Color, 12 );
	window->DrawList->AddCircleFilled( bb.Min + ImVec2( 10 + ( 16 * Active ), 9 ), 10, ImColor( 255, 255, 255 ), 32 );
}

void C_UIElements::Hitbox( const char* szLabelName, std::string value )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( szLabelName );

	const ImVec2 pos = window->DC.CursorPos;
	const ImVec2 size = ImGui::CalcItemSize( { 18, 18 }, 290, 25 );
	ImRect bb( pos, pos + size );

	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	if ( pressed && ImGui::IsMouseHoveringRect( bb.Min, bb.Max ) ) g_SettingsManager->B[ value ] = !( g_SettingsManager->B[ value ] );

	std::string label_hovered = std::string( szLabelName ) + "Hovered";

	float Active = ImTricks::Animations::FastFloatLerp( id, g_SettingsManager->B[ value ], 0.f, 1.f, 0.02f );
	Active = Active * swapAnimation;
	ImColor Color = ImTricks::Animations::FastColorLerp( ImColor( 78, 78, 78 ), ImColor( 10, 164, 252, 150 ), Active );

	window->DrawList->AddCircleFilled( bb.Min + ImVec2( 9, 9 ), 7, Color, 32 );
	window->DrawList->AddCircleFilled( bb.Min + ImVec2( 9, 9 ), 3, ImColor( 255, 255, 255 ), 32 );
}

static float CalcMaxPopupHeightFromItemCount( int items_count )
{
	ImGuiContext& g = *GImGui;
	if ( items_count <= 0 )
		return FLT_MAX;
	return ( g.FontSize + g.Style.ItemSpacing.y ) * items_count - g.Style.ItemSpacing.y + ( g.Style.WindowPadding.y * 2 );
}

static bool Items_ArrayGetter( void* data, int idx, const char** out_text )
{
	const char* const* items = ( const char* const* ) data;
	if ( out_text )
		*out_text = items[ idx ];
	return true;
}

bool C_UIElements::Combo( const char* label, std::string value, std::vector<std::string> items, OverallElementsFlag flags )
{
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	bool value_changed = false;

	ImGuiContext& g = *GImGui;
	bool has_window_size_constraint = ( g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint ) != 0;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

	ImRect frame_bb;
	ImRect total_bb;

	if ( !( flags & OverallFlags_AutoScale ) )
	{
		frame_bb = ImRect( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( 270, 45 ) );
		total_bb = ImRect( window->DC.CursorPos, window->DC.CursorPos + ImVec2( 270, 45 ) );
	}
	else if ( flags & OverallFlags_AutoScale ) {
		frame_bb = ImRect( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 45 ) );
		total_bb = ImRect( window->DC.CursorPos, window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 45 ) );
	}

	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( frame_bb, id, &hovered, &held );
	bool popup_open = ImGui::IsPopupOpen( id, ImGuiPopupFlags_None );

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );

	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );
	window->DrawList->AddRect( frame_bb.Min, frame_bb.Max, ImColor( 90, 90, 90, int( 255 - ( 150 * Searched ) ) ), 4 );

	window->DrawList->AddText( total_bb.Min + ImVec2( 10, 25 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), items[ g_SettingsManager->I[ value ] ].c_str( ) );
	std::string label_hovered = std::string( label ) + "Hovered";
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label_hovered.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	if ( label_size.x > 0 )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	if ( ( pressed || g.NavActivateId == id ) && !popup_open )
	{
		if ( window->DC.NavLayerCurrent == 0 )
			window->NavLastIds[ 0 ] = id;
		ImGui::OpenPopupEx( id, ImGuiPopupFlags_None );
		popup_open = true;
	}

	auto selected_bind_map = ImTricks::comboMap.find( id );
	if ( selected_bind_map == ImTricks::comboMap.end( ) )
	{
		ImTricks::comboMap.insert( { id, { 0 } } );
		selected_bind_map = ImTricks::comboMap.find( id );
	}

	if ( !( flags & OverallFlags_NotBindable ) ) {
		std::string popup = std::string( label ) + std::string( "Popup" );

		auto mapVal = ImTricks::bindsMapInt.find( id );
		if ( mapVal == ImTricks::bindsMapInt.end( ) ) {
			ImTricks::bindsMapInt.insert( { id, -1 } );
			mapVal = ImTricks::bindsMapInt.find( id );
		}

		auto mapVal2 = ImTricks::bindsMapString.find( id );
		if ( mapVal2 == ImTricks::bindsMapString.end( ) ) {
			ImTricks::bindsMapString.insert( { id, "Create New" } );
			mapVal2 = ImTricks::bindsMapString.find( id );
		}

		if ( ImGui::BeginPopup( popup.c_str( ), ImGuiWindowFlags_NoMove ) )
		{
			ImGui::SetWindowPos( total_bb.Min + ImVec2( 0, 45 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( "KeyBind", { 250, 195 } );
			{
				ImGui::PushItemWidth( 250 );
				static OnKeyAction_t tempValue;

				if ( BeginCombo( "Binds", mapVal2->second.c_str( ), NULL, 250 ) ) {
					if ( ImGui::Selectable( "Create New", mapVal->second == -1 ) )
						mapVal->second = -1, mapVal2->second = "Create New";

					for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
						if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
							if ( ImGui::Selectable( getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), mapVal->second == a ) )
								mapVal->second = a, mapVal2->second = getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( );
					}

					ImGui::EndCombo( );
				}

				ImGui::Separator( );

				if ( mapVal->second == -1 )
				{
					ImGui::Combo( "##Mode", &tempValue.m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", tempValue.m_uiKeyID );

					if ( BeginCombo( "New Value", items[ std::any_cast< int >( tempValue.newValue ) ].c_str( ), NULL, 250 ) ) {
						for ( auto a = 0; a < items.size( ); a++ ) {
							if ( ImGui::Selectable( items[ a ].c_str( ) ) )
								tempValue.newValue = a;
						}
						ImGui::EndCombo( );
					}

					if ( ImGui::Button( "Create Bind", { 250, 22 } ) ) {
						tempValue.m_tVariableType = KeyVariableType::INT_KEY_TYPE;
						tempValue.m_sRenderableName = PenetrateByAnalMapKey( value, label );
						tempValue.m_sVariableMapName = value;
						tempValue.defaultValue = g_SettingsManager->I[ value ];

						g_KeyManager->RegisterKey( tempValue );

						tempValue = OnKeyAction_t( );
					}
				}
				else {
					ImGui::Combo( "##Mode", &g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", g_KeyManager->m_vKeyActions[ mapVal->second ].m_uiKeyID );

					if ( BeginCombo( "New Value", items[ std::any_cast< int >( g_KeyManager->m_vKeyActions[ mapVal->second ].newValue ) ].c_str( ), NULL, 250 ) ) {
						for ( auto a = 0; a < items.size( ); a++ ) {
							if ( ImGui::Selectable( items[ a ].c_str( ) ) )
								g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = a;
						}
						ImGui::EndCombo( );
					}

					if ( ImGui::Button( "Delete Bind", { 250, 22 } ) ) {
						g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + mapVal->second );
						mapVal->second = -1;
						mapVal2->second = "Create New";
					}
				}
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 270, 7 } );

			ImGui::EndPopup( );
		}

		if ( ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ) && ImGui::GetIO( ).MouseDown[ 1 ] )
			ImGui::OpenPopup( popup.c_str( ) );
	}

	if ( !popup_open )
		return false;

	char name[ 16 ];
	ImFormatString( name, IM_ARRAYSIZE( name ), "##Combo_%02d", g.BeginPopupStack.Size );

	if ( ImGuiWindow* popup_window = ImGui::FindWindowByName( name ) )
		if ( popup_window->WasActive )
		{
			ImVec2 size_expected = ImGui::CalcWindowNextAutoFitSize( popup_window );
			ImRect r_outer = ImGui::GetWindowAllowedExtentRect( popup_window );
			ImVec2 pos = ImGui::FindBestWindowPosForPopupEx( frame_bb.GetBL( ), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox );
			ImGui::SetNextWindowPos( pos );
		}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowSizeConstraints( ImVec2( 270, 3.0f ), ImVec2( FLT_MAX, CalcMaxPopupHeightFromItemCount( 5 ) ) );

	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( style.FramePadding.x, style.WindowPadding.y ) );
	ImGui::Begin( name, NULL, window_flags );
	ImGui::PopStyleVar( );

	for ( int i = 0; i < items.size( ); i++ )
	{
		ImGui::PushID( ( void* ) ( intptr_t ) i );

		if ( ImGui::Selectable( items[ i ].c_str( ), g_SettingsManager->I[ value ] == i ) )
		{
			value_changed = true;
			g_SettingsManager->I[ value ] = i;

			for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
				if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
					g_KeyManager->m_vKeyActions[ a ].defaultValue = i;
			}

			ImGui::SetItemDefaultFocus( );
		}

		ImGui::PopID( );
	}

	ImGui::EndPopup( );

	return true;
}

bool C_UIElements::BeginCombo( const char* label, const char* preview_value, ImGuiComboFlags flags, float width )
{
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	bool value_changed = false;

	ImGuiContext& g = *GImGui;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

	const ImRect frame_bb( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( width, 45 ) );
	const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( width, 45 ) );

	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( frame_bb, id, &hovered, &held );
	bool popup_open = ImGui::IsPopupOpen( id, ImGuiPopupFlags_None );

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );

	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );
	window->DrawList->AddRect( frame_bb.Min, frame_bb.Max, ImColor( 90, 90, 90, int( 255 - ( 150 * Searched ) ) ), 4 );

	window->DrawList->PushClipRect( frame_bb.Min, frame_bb.Max );
	window->DrawList->AddText( total_bb.Min + ImVec2( 10, 25 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), preview_value );
	window->DrawList->PopClipRect( );

	std::string label_hovered = std::string( label ) + "Hovered";
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( label_hovered.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	if ( label_size.x > 0 )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	if ( ( pressed || g.NavActivateId == id ) && !popup_open )
	{
		if ( window->DC.NavLayerCurrent == 0 )
			window->NavLastIds[ 0 ] = id;
		ImGui::OpenPopupEx( id, ImGuiPopupFlags_None );
		popup_open = true;
	}

	if ( !popup_open )
		return false;

	char name[ 16 ];
	ImFormatString( name, IM_ARRAYSIZE( name ), "##Combo_%02d", g.BeginPopupStack.Size ); // Recycle windows based on depth


	if ( ImGuiWindow* popup_window = ImGui::FindWindowByName( name ) )
		if ( popup_window->WasActive )
		{
			ImVec2 size_expected = ImGui::CalcWindowNextAutoFitSize( popup_window );
			ImRect r_outer = ImGui::GetWindowAllowedExtentRect( popup_window );
			ImVec2 pos = ImGui::FindBestWindowPosForPopupEx( frame_bb.GetBL( ), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox );
			ImGui::SetNextWindowPos( total_bb.Min + ImVec2( 0, 45 ) );
		}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowSizeConstraints( ImVec2( width, 3.0f ), ImVec2( FLT_MAX, CalcMaxPopupHeightFromItemCount( 5 ) ) );

	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( style.FramePadding.x, style.WindowPadding.y ) );
	bool ret = ImGui::Begin( name, NULL, window_flags );
	ImGui::PopStyleVar( );
	if ( !ret )
	{
		ImGui::EndPopup( );
		IM_ASSERT( 0 );   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}
	return true;
}

void C_UIElements::MultiCombo( const char* name, std::vector <std::string> variable, std::vector < std::string > labels )
{
	static std::string preview;

	for ( auto i = 0, j = 0; i < variable.size( ); i++ )
	{
		if ( g_SettingsManager->B[ variable[ i ] ] == true )
		{
			if ( j )
				preview += ", " + ( std::string ) labels[ i ];
			else
				preview = labels[ i ];

			j++;
		}
	}

	if ( BeginCombo( name, preview.c_str( ) ) )
	{
		ImGui::Spacing( );

		for ( auto i = 0; i < variable.size( ); i++ )
			if ( ImGui::Selectable( labels[ i ].c_str( ), g_SettingsManager->B[ variable[ i ] ], ImGuiSelectableFlags_DontClosePopups ) )
				g_SettingsManager->B[ variable[ i ] ] = !g_SettingsManager->B[ variable[ i ] ];

		ImGui::Spacing( );

		ImGui::EndCombo( );
	}

	preview = "None";
}

static const char* PatchFormatStringFloatToInt( const char* fmt )
{
	if ( fmt[ 0 ] == '%' && fmt[ 1 ] == '.' && fmt[ 2 ] == '0' && fmt[ 3 ] == 'f' && fmt[ 4 ] == 0 )
		return "%d";

	const char* fmt_start = ImParseFormatFindStart( fmt );
	const char* fmt_end = ImParseFormatFindEnd( fmt_start );

	if ( fmt_end > fmt_start && fmt_end[ -1 ] == 'f' )
	{
		if ( fmt_start == fmt && fmt_end[ 0 ] == 0 )
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString( g.TempBuffer, IM_ARRAYSIZE( g.TempBuffer ), "%.*s%%d%s", ( int ) ( fmt_start - fmt ), fmt, fmt_end );
		return g.TempBuffer;
	}
	return fmt;
}

bool SliderFloater( const char* label, float* value, float v_min, float v_max, const char* format, ImGuiSliderFlags flag )
{
	auto data_type = ImGuiDataType_Float;
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );
	const float w = ImGui::CalcItemWidth( );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );
	const ImRect frame_bb( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( 250, 27 ) );
	const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( 250, 30 ) );

	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	if ( format == NULL )
		format = ImGui::DataTypeGetInfo( ImGuiDataType_S32 )->PrintFmt;
	else if ( data_type == ImGuiDataType_S32 && strcmp( format, "%d" ) != 0 )
		format = PatchFormatStringFloatToInt( format );

	const bool hovered = ImGui::ItemHoverable( frame_bb, id );
	const bool temp_input_allowed = ( flag & ImGuiSliderFlags_NoInput ) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive( id );
	if ( !temp_input_is_active )
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister( window, id );
		const bool clicked = ( hovered && g.IO.MouseClicked[ 0 ] );
		if ( focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id )
		{
			ImGui::SetActiveID( id, window );
			ImGui::SetFocusID( id, window );
			ImGui::FocusWindow( window );
			g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
			if ( temp_input_allowed && ( focus_requested || ( clicked && g.IO.KeyCtrl ) || g.NavInputId == id ) )
			{
				temp_input_is_active = true;
				ImGui::FocusableItemUnregister( window );
			}
		}
	}

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );
	std::string hoverid = label + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior( frame_bb, id, data_type, value, &v_min, &v_max, format, flag, &grab_bb );

	int col_grb = int( 245 - ( 150 * Searched ) );

	window->DrawList->AddRectFilled( frame_bb.Min, grab_bb.Max + ImVec2( 2, 2 ), ImColor( 190, 190, 190, int( 255 - ( 150 * Searched ) ) ), 3 );
	window->DrawList->AddCircleFilled( grab_bb.Max - ImVec2( 0, 2 ), 6, ImColor( col_grb, col_grb, col_grb ), 32 );

	char value_buf[ 64 ];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString( value_buf, IM_ARRAYSIZE( value_buf ), data_type, value, format );
	window->DrawList->AddText( total_bb.Min + ImVec2( 250 - ImGui::CalcTextSize( value_buf ).x, 0 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), value_buf );

	if ( label_size.x > 0.0f )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	return value_changed;
}

bool C_UIElements::SliderFloat( const char* label, std::string value, float v_min, float v_max, const char* format, ImGuiSliderFlags flag, OverallElementsFlag flags )
{
	auto data_type = ImGuiDataType_Float;
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );
	const float w = ImGui::CalcItemWidth( );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

	ImRect frame_bb;
	ImRect total_bb;

	if ( !( flags & OverallFlags_AutoScale ) )
	{
		frame_bb = ImRect( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( 270, 27 ) );
		total_bb = ImRect( window->DC.CursorPos, window->DC.CursorPos + ImVec2( 270, 30 ) );
	}
	else if ( flags & OverallFlags_AutoScale ) {
		frame_bb = ImRect( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 27 ) );
		total_bb = ImRect( window->DC.CursorPos, window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 30 ) );
	}


	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	if ( format == NULL )
		format = ImGui::DataTypeGetInfo( ImGuiDataType_S32 )->PrintFmt;
	else if ( data_type == ImGuiDataType_S32 && strcmp( format, "%d" ) != 0 )
		format = PatchFormatStringFloatToInt( format );

	const bool hovered = ImGui::ItemHoverable( frame_bb, id );
	const bool temp_input_allowed = ( flags & ImGuiSliderFlags_NoInput ) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive( id );
	if ( !temp_input_is_active )
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister( window, id );
		const bool clicked = ( hovered && g.IO.MouseClicked[ 0 ] );
		if ( focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id )
		{
			ImGui::SetActiveID( id, window );
			ImGui::SetFocusID( id, window );
			ImGui::FocusWindow( window );
			g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
			if ( temp_input_allowed && ( focus_requested || ( clicked && g.IO.KeyCtrl ) || g.NavInputId == id ) )
			{
				temp_input_is_active = true;
				ImGui::FocusableItemUnregister( window );
			}
		}
	}

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );
	std::string hoverid = label + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior( frame_bb, id, data_type, &g_SettingsManager->F[ value ], &v_min, &v_max, format, flags, &grab_bb );

	if ( value_changed ) {
		for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
			if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
				g_KeyManager->m_vKeyActions[ a ].defaultValue = g_SettingsManager->F[ value ];
		}
	}

	int col_grb = int( 245 - ( 150 * Searched ) );
	ImVec2 grab_pos = ImVec2( ( grab_bb.Max.x - frame_bb.Min.x ) * swapAnimation, grab_bb.Max.y - frame_bb.Min.y );
	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Min + grab_pos + ImVec2( 2, 2 ), ImColor( 190, 190, 190, int( 255 - ( 150 * Searched ) ) ), 3 );
	window->DrawList->AddCircleFilled( frame_bb.Min + grab_pos - ImVec2( 0, 2 ), 6, ImColor( col_grb, col_grb, col_grb ), 32 );

	char value_buf[ 64 ];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString( value_buf, IM_ARRAYSIZE( value_buf ), data_type, &g_SettingsManager->F[ value ], format );
	window->DrawList->AddText( total_bb.Min + ImVec2( ImGui::GetWindowSize( ).x - 20 - ImGui::CalcTextSize( value_buf ).x, 0 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), value_buf );

	if ( label_size.x > 0.0f )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	if ( !( flags & OverallFlags_NotBindable ) ) {

		auto mapVal = ImTricks::bindsMapInt.find( id );
		if ( mapVal == ImTricks::bindsMapInt.end( ) ) {
			ImTricks::bindsMapInt.insert( { id, -1 } );
			mapVal = ImTricks::bindsMapInt.find( id );
		}

		auto mapVal2 = ImTricks::bindsMapString.find( id );
		if ( mapVal2 == ImTricks::bindsMapString.end( ) ) {
			ImTricks::bindsMapString.insert( { id, "Create New" } );
			mapVal2 = ImTricks::bindsMapString.find( id );
		}

		std::string popup = std::string( label ) + std::string( "Popup" );

		if ( ImGui::BeginPopup( popup.c_str( ), ImGuiWindowFlags_NoMove ) )
		{
			ImGui::SetWindowPos( total_bb.Min + ImVec2( 0, 25 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( "KeyBind", { 250, 180 } );
			{
				ImGui::PushItemWidth( 250 );
				static OnKeyAction_t tempValue;
				static float newValue = 0;

				if ( BeginCombo( "Binds", mapVal2->second.c_str( ), NULL, 250 ) ) {
					if ( ImGui::Selectable( "Create New", mapVal->second == -1 ) )
						mapVal->second = -1, mapVal2->second = "Create New";

					for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
						if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
							if ( ImGui::Selectable( getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), mapVal->second == a ) )
								mapVal->second = a, mapVal2->second = getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), newValue = std::any_cast< float >( g_KeyManager->m_vKeyActions[ a ].newValue );
					}

					ImGui::EndCombo( );
				}

				ImGui::Separator( );

				if ( mapVal->second == -1 )
				{
					ImGui::Combo( "##Mode", &tempValue.m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", tempValue.m_uiKeyID );
					if ( ::SliderFloater( "New Value", &newValue, v_min, v_max, format, NULL ) )
						tempValue.newValue = newValue;

					if ( ImGui::Button( "Create Bind", { 250, 22 } ) ) {
						tempValue.m_tVariableType = KeyVariableType::FLOAT_KEY_TYPE;
						tempValue.m_sRenderableName = PenetrateByAnalMapKey( value, label );
						tempValue.m_sVariableMapName = value;
						tempValue.defaultValue = g_SettingsManager->F[ value ];

						g_KeyManager->RegisterKey( tempValue );

						tempValue = OnKeyAction_t( );
						newValue = 0;
					}
				}
				else
				{
					ImGui::Combo( "##Mode", &g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", g_KeyManager->m_vKeyActions[ mapVal->second ].m_uiKeyID );
					if ( ::SliderFloater( "New Value", &newValue, v_min, v_max, format, NULL ) )
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = newValue;

					if ( ImGui::Button( "Delete Bind", { 250, 22 } ) ) {
						g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + mapVal->second );
						mapVal->second = -1;
						newValue = 0;
						mapVal2->second = "Create New";
					}
				}
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 270, 7 } );

			ImGui::EndPopup( );
		}

		if ( ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ) && ImGui::GetIO( ).MouseDown[ 1 ] )
			ImGui::OpenPopup( popup.c_str( ) );
	}

	return value_changed;
}

bool SliderInt( const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flag )
{
	auto data_type = ImGuiDataType_S32;
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );
	const float w = ImGui::CalcItemWidth( );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );
	const ImRect frame_bb( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( 250, 27 ) );
	const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( 250, 30 ) );

	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	if ( format == NULL )
		format = ImGui::DataTypeGetInfo( ImGuiDataType_S32 )->PrintFmt;
	else if ( data_type == ImGuiDataType_S32 && strcmp( format, "%d" ) != 0 )
		format = PatchFormatStringFloatToInt( format );

	const bool hovered = ImGui::ItemHoverable( frame_bb, id );
	const bool temp_input_allowed = ( flag & ImGuiSliderFlags_NoInput ) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive( id );
	if ( !temp_input_is_active )
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister( window, id );
		const bool clicked = ( hovered && g.IO.MouseClicked[ 0 ] );
		if ( focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id )
		{
			ImGui::SetActiveID( id, window );
			ImGui::SetFocusID( id, window );
			ImGui::FocusWindow( window );
			g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
			if ( temp_input_allowed && ( focus_requested || ( clicked && g.IO.KeyCtrl ) || g.NavInputId == id ) )
			{
				temp_input_is_active = true;
				ImGui::FocusableItemUnregister( window );
			}
		}
	}

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );
	std::string hoverid = label + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	const ImU32 frame_col = ImGui::GetColorU32( g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg );
	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior( frame_bb, id, data_type, v, &v_min, &v_max, format, flag, &grab_bb );

	int col_grb = int( 245 - ( 150 * Searched ) );

	window->DrawList->AddRectFilled( frame_bb.Min, grab_bb.Max + ImVec2( 2, 2 ), ImColor( 190, 190, 190, int( 255 - ( 150 * Searched ) ) ), 3 );
	window->DrawList->AddCircleFilled( grab_bb.Max - ImVec2( 0, 2 ), 6, ImColor( col_grb, col_grb, col_grb ), 32 );

	char value_buf[ 64 ];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString( value_buf, IM_ARRAYSIZE( value_buf ), data_type, v, format );
	window->DrawList->AddText( total_bb.Min + ImVec2( 250 - ImGui::CalcTextSize( value_buf ).x, 0 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), value_buf );

	if ( label_size.x > 0.0f )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	return value_changed;
}

bool  C_UIElements::SliderInt( const char* label, std::string value, int v_min, int v_max, const char* format, ImGuiSliderFlags flag, OverallElementsFlag flags )
{
	auto data_type = ImGuiDataType_S32;
	bool searched = std::string( label ).find( std::string( g_Menu->m_cSearch ) ) == std::string::npos;
	std::string label_searched = std::string( label ) + "Searched";

	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );
	const float w = ImGui::CalcItemWidth( );

	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );
	const ImRect frame_bb( window->DC.CursorPos + ImVec2( 0, 20 ), window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 27 ) );
	const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( ImGui::GetWindowSize( ).x - 20, 30 ) );

	ImGui::ItemSize( total_bb, style.FramePadding.y );
	if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
		return false;

	if ( format == NULL )
		format = ImGui::DataTypeGetInfo( ImGuiDataType_S32 )->PrintFmt;
	else if ( data_type == ImGuiDataType_S32 && strcmp( format, "%d" ) != 0 )
		format = PatchFormatStringFloatToInt( format );

	const bool hovered = ImGui::ItemHoverable( frame_bb, id );
	const bool temp_input_allowed = ( flag & ImGuiSliderFlags_NoInput ) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive( id );
	if ( !temp_input_is_active )
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister( window, id );
		const bool clicked = ( hovered && g.IO.MouseClicked[ 0 ] );
		if ( focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id )
		{
			ImGui::SetActiveID( id, window );
			ImGui::SetFocusID( id, window );
			ImGui::FocusWindow( window );
			g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
			if ( temp_input_allowed && ( focus_requested || ( clicked && g.IO.KeyCtrl ) || g.NavInputId == id ) )
			{
				temp_input_is_active = true;
				ImGui::FocusableItemUnregister( window );
			}
		}
	}

	float Searched = ImTricks::Animations::FastFloatLerp( window->GetID( label_searched.c_str( ) ), searched, 0.f, 1.f, 0.02f );
	std::string hoverid = label + std::string( "hover" );
	float Hovered = ImTricks::Animations::FastFloatLerp( window->GetID( hoverid.c_str( ) ), ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ), 0.f, 1.f, 0.02f );

	const ImU32 frame_col = ImGui::GetColorU32( g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg );
	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Max, ImColor( 55, 55, 55, int( 255 - ( 150 * Searched ) ) ), 4 );

	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior( frame_bb, id, data_type, &g_SettingsManager->I[ value ], &v_min, &v_max, format, flag, &grab_bb );

	if ( value_changed ) {
		for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
			if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
				g_KeyManager->m_vKeyActions[ a ].defaultValue = g_SettingsManager->I[ value ];
		}
	}

	int col_grb = int( 245 - ( 150 * Searched ) );

	ImVec2 grab_pos = ImVec2( ( grab_bb.Max.x - frame_bb.Min.x ) * swapAnimation, grab_bb.Max.y - frame_bb.Min.y );
	window->DrawList->AddRectFilled( frame_bb.Min, frame_bb.Min + grab_pos + ImVec2( 2, 2 ), ImColor( 190, 190, 190, int( 255 - ( 150 * Searched ) ) ), 3 );
	window->DrawList->AddCircleFilled( frame_bb.Min + grab_pos - ImVec2( 0, 2 ), 6, ImColor( col_grb, col_grb, col_grb ), 32 );

	char value_buf[ 64 ];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString( value_buf, IM_ARRAYSIZE( value_buf ), data_type, &g_SettingsManager->I[ value ], format );
	window->DrawList->AddText( total_bb.Min + ImVec2( ImGui::GetWindowSize( ).x - 20 - ImGui::CalcTextSize( value_buf ).x, 0 ), ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), value_buf );

	if ( label_size.x > 0.0f )
	{
		window->DrawList->AddText( total_bb.Min, ImColor( 200, 200, 200, int( 255 - ( 150 * Searched ) ) ), label );
		window->DrawList->AddText( total_bb.Min, ImColor( 240, 240, 240, int( 255 * Hovered ) ), label );
	}

	if ( !( flags & OverallFlags_NotBindable ) ) {

		auto mapVal = ImTricks::bindsMapInt.find( id );
		if ( mapVal == ImTricks::bindsMapInt.end( ) ) {
			ImTricks::bindsMapInt.insert( { id, -1 } );
			mapVal = ImTricks::bindsMapInt.find( id );
		}

		auto mapVal2 = ImTricks::bindsMapString.find( id );
		if ( mapVal2 == ImTricks::bindsMapString.end( ) ) {
			ImTricks::bindsMapString.insert( { id, "Create New" } );
			mapVal2 = ImTricks::bindsMapString.find( id );
		}

		std::string popup = std::string( label ) + std::string( "Popup" );

		if ( ImGui::BeginPopup( popup.c_str( ), ImGuiWindowFlags_NoMove ) )
		{
			ImGui::SetWindowPos( total_bb.Min + ImVec2( 0, 25 ) );

			ImGui::SetCursorPos( { 10, 10 } );
			ImGui::BeginChild( "KeyBind", { 250, 180 } );
			{
				ImGui::PushItemWidth( 250 );
				static OnKeyAction_t tempValue;
				static int newValue = 0;

				if ( BeginCombo( "Binds", mapVal2->second.c_str( ), NULL, 250 ) ) {
					if ( ImGui::Selectable( "Create New", mapVal->second == -1 ) )
						mapVal->second = -1, mapVal2->second = "Create New";

					for ( auto a = 0; a < g_KeyManager->m_vKeyActions.size( ); a++ ) {
						if ( g_KeyManager->m_vKeyActions[ a ].m_sVariableMapName == value )
							if ( ImGui::Selectable( getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), mapVal->second == a ) )
								mapVal->second = a, mapVal2->second = getTypeString( g_KeyManager->m_vKeyActions[ a ] ).c_str( ), newValue = std::any_cast< int >( g_KeyManager->m_vKeyActions[ a ].newValue );;
					}

					ImGui::EndCombo( );
				}

				ImGui::Separator( );

				if ( mapVal->second == -1 )
				{
					ImGui::Combo( "##Mode", &tempValue.m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", tempValue.m_uiKeyID );
					if ( ::SliderInt( "New Value", &newValue, v_min, v_max, format, NULL ) )
						tempValue.newValue = newValue;

					if ( ImGui::Button( "Create Bind", { 250, 22 } ) ) {
						tempValue.m_tVariableType = KeyVariableType::INT_KEY_TYPE;
						tempValue.m_sRenderableName = PenetrateByAnalMapKey( value, label );
						tempValue.m_sVariableMapName = value;
						tempValue.defaultValue = g_SettingsManager->I[ value ];

						g_KeyManager->RegisterKey( tempValue );

						tempValue = OnKeyAction_t( );
						newValue = 0;
					}
				}
				else
				{
					ImGui::Combo( "##Mode", &g_KeyManager->m_vKeyActions[ mapVal->second ].m_tActionType, "Toggle\0Hold On\0Hold Off\0" );
					KeyBind( "Key", g_KeyManager->m_vKeyActions[ mapVal->second ].m_uiKeyID );
					if ( ::SliderInt( "New Value", &newValue, v_min, v_max, format, NULL ) )
						g_KeyManager->m_vKeyActions[ mapVal->second ].newValue = newValue;

					if ( ImGui::Button( "Delete Bind", { 250, 22 } ) ) {
						g_KeyManager->m_vKeyActions.erase( g_KeyManager->m_vKeyActions.begin( ) + mapVal->second );
						mapVal->second = -1;
						newValue = 0;
						mapVal2->second = "Create New";
					}
				}
			}
			ImGui::EndChild( );
			ImGui::Dummy( { 270, 7 } );

			ImGui::EndPopup( );
		}

		if ( ImGui::IsMouseHoveringRect( total_bb.Min, total_bb.Max ) && ImGui::GetIO( ).MouseDown[ 1 ] )
			ImGui::OpenPopup( popup.c_str( ) );
	}

	return value_changed;
}

bool C_UIElements::Spinner( const char* label, float radius, int thickness, const ImU32& color, ImVec2 pos ) {
	auto window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	auto& g = *GImGui;
	const auto& style = g.Style;
	ImVec2 size { radius * 2, radius * 2 };
	const ImRect bb { pos, pos + size };
	ImGui::ItemSize( bb );
	if ( !ImGui::ItemAdd( bb, 0 ) )
		return false;

	auto time = static_cast< float >( g.Time ) * 1.8f;
	window->DrawList->PathClear( );
	int start = static_cast< int >( abs( ImSin( time ) * ( 32 - 5 ) ) );
	const float a_min = IM_PI * 2.0f * ( ( float ) start ) / ( float ) 32;
	const float a_max = IM_PI * 2.0f * ( ( float ) 32 - 3 ) / ( float ) 32;
	const auto&& centre = ImVec2( pos.x + radius, pos.y + radius );
	for ( auto i = 0; i < 32; i++ ) {
		const float a = a_min + ( ( float ) i / ( float ) 32 ) * ( a_max - a_min );
		window->DrawList->PathLineTo( { centre.x + ImCos( a + time * 8 ) * radius,
									  centre.y + ImSin( a + time * 8 ) * radius } );
	}
	window->DrawList->PathStroke( ImGui::GetColorU32( color ), false, thickness );

	return true;
}

void C_UIElements::AddCircleImageFilled( ImTextureID user_texture_id, const ImVec2& centre, float radius, ImU32 col, int num_segments )
{
	auto window = ImGui::GetCurrentWindow( );
	if ( window->SkipItems )
		return;
	ImGuiContext& g = *GImGui;
	window->DrawList->PathClear( );

	if ( ( col & IM_COL32_A_MASK ) == 0 || num_segments <= 2 )
		return;

	const bool push_texture_id = window->DrawList->_TextureIdStack.empty( ) || user_texture_id != window->DrawList->_TextureIdStack.back( );
	if ( push_texture_id )
		window->DrawList->PushTextureID( user_texture_id );
	int vert_start_idx = window->DrawList->VtxBuffer.Size;
	const float a_max = IM_PI * 2.0f * ( ( float ) num_segments - 1.0f ) / ( float ) num_segments;
	window->DrawList->PathArcTo( centre, radius, 0.0f, a_max, num_segments - 1 );
	window->DrawList->PathFillConvex( col );
	int vert_end_idx = window->DrawList->VtxBuffer.Size;
	ImGui::ShadeVertsLinearUV( window->DrawList, vert_start_idx, vert_end_idx, ImVec2( centre.x - radius, centre.y - radius ), ImVec2( centre.x + radius, centre.y + radius ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), true );
	if ( push_texture_id )
		window->DrawList->PopTextureID( );
}

void C_UIElements::PlayerList( const char* name, int index, const ImVec2& size_arg )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( name );
	const ImVec2 label_size = ImGui::CalcTextSize( name, NULL, true );

	ImVec2 pos = window->DC.CursorPos + ImVec2( 10, 10 );
	ImVec2 size = ImGui::CalcItemSize( size_arg, label_size.x, label_size.y );

	const ImRect bb( pos, pos + size );
	ImGui::ItemSize( size, style.FramePadding.y );

	ImGui::RenderText( ImVec2( ( bb.Min.x + size_arg.x ) - label_size.x, bb.Min.y + size_arg.y / 2 - label_size.y / 2 ), name );
	AddCircleImageFilled( g_Menu->m_tAvatars[ index ], ImVec2( bb.Min.x + 12, bb.Min.y + ( size_arg.y / 2 ) ), 12, ImColor( 255, 255, 255 ), 32 );
}

int C_UIElements::KeybindVisibleElement( OnKeyAction_t key, int index )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow( );
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( std::string( key.m_sVariableMapName + std::to_string( key.m_uiKeyID ) ).c_str( ) );

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize( { 600, 36 }, 610, 30 );

	const ImRect bb( pos, pos + size );
	ImGui::ItemSize( size, style.FramePadding.y );

	window->DrawList->AddRectFilled( bb.Min, bb.Max, index % 2 == 0 ? ImColor( 98, 98, 98 ) : ImColor( 67, 67, 67 ) );
	window->DrawList->AddText( bb.Min + ImVec2( 15, 9 ), ImColor( 255, 255, 255 ), key.m_sRenderableName.c_str( ) );

	auto getTypeString = [ key ] ( ) {
		if ( key.m_tActionType == 0 )
			return "Swap";
		if ( key.m_tActionType == 1 )
			return "Hold On";
		if ( key.m_tActionType == 2 )
			return "Hold Off";
	};

	window->DrawList->AddText( bb.Min + ImVec2( 200, 9 ), ImColor( 255, 255, 255 ), getTypeString( ) );
	window->DrawList->AddText( bb.Min + ImVec2( 300, 9 ), ImColor( 255, 255, 255 ), keys[ key.m_uiKeyID ] );


	window->DrawList->AddRectFilled( bb.Max - ImVec2( 80, 30 ), bb.Max - ImVec2( 18, 6 ), ImColor( 140, 140, 140, 120 ), 5 );
	window->DrawList->AddText( bb.Max - ImVec2( 70, 26 ), ImColor( 255, 255, 255 ), "Delete" );

	if ( ImGui::IsMouseHoveringRect( bb.Max - ImVec2( 60, 30 ), bb.Max - ImVec2( 18, 6 ) ) && ImGui::GetIO( ).MouseDown[ 0 ] )
		return 1;

	return 0;
}

void C_UIElements::Animate( )
{
	if ( this->swapAnimation < 1.f )
		this->swapAnimation += 0.02f * ( 1.f - ImGui::GetIO( ).DeltaTime );
}