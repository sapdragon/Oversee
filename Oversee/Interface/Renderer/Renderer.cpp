#include "Renderer.hpp"
#include "../GUI/Menu.hpp"


void C_Renderer::SetupData()
{
	if (SDK::Interfaces::DirectDevice)
	{
		D3DDEVICE_CREATION_PARAMETERS lParams;
		SDK::Interfaces::DirectDevice->GetCreationParameters(&lParams);
		m_HWND = lParams.hFocusWindow;
	}

	g_InputSystem->SetupData(m_HWND);

	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGui_ImplWin32_Init(m_HWND);
	ImGui_ImplDX9_Init(SDK::Interfaces::DirectDevice);
}

void C_Renderer::SwapDrawData()
{
	std::unique_lock<std::shared_mutex> lock(m_DrawMutex);
	m_DrawListQueue.swap(m_DrawListQueueSafe);
}

void C_Renderer::ClearDrawData()
{
	if (!m_DrawListQueue.empty())
		m_DrawListQueue.clear();
}

void C_Renderer::Instance()
{
	std::unique_lock<std::shared_mutex> lock(m_DrawMutex);

	if (m_DrawListQueueSafe.empty())
		return;

	for (auto draw : m_DrawListQueueSafe)
	{
		if (!draw.m_Object.has_value())
			return;

		if (draw.m_Type == DrawObject_NONE) {}

		if (draw.m_Type == DrawObject_LINE)
		{
			const auto& pObject = std::any_cast<LineObject_t>(draw.m_Object);
			ImGui::GetBackgroundDrawList()->AddLine(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fThickness);
		}

		if (draw.m_Type == DrawObject_RECT)
		{
			const auto& pObject = std::any_cast<RectObject_t>(draw.m_Object);
			if (pObject.m_RectType == RectType_Clear)
				ImGui::GetBackgroundDrawList()->AddRect(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fRounding, pObject.m_tCorners, pObject.m_fThickness);
			if (pObject.m_RectType == RectType_Filled)
				ImGui::GetBackgroundDrawList()->AddRectFilled(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fRounding, pObject.m_tCorners);
		}

		if (draw.m_Type == DrawObject_RECTMULTI)
		{
			const auto& pObject = std::any_cast<RectMultiObject_t>(draw.m_Object);
			ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(pObject.m_Min, pObject.m_Max, pObject.m_Color1, pObject.m_Color2, pObject.m_Color3, pObject.m_Color4);
		}

		if (draw.m_Type == DrawObject_CIRCLE3D)
		{
			const auto& pObject = std::any_cast<Circle3dObject_t>(draw.m_Object);

			static Vector vecPreviousScreenPos = Vector( 0, 0, 0 );
			static float_t flStep = 3.14159265358979323846f * 2.0f / 72.0f;

			Vector vecScreenPosition = Vector( 0, 0, 0 );
			if ( SDK::Interfaces::DebugOverlay->ScreenPosition( pObject.Pos, vecScreenPosition ) )
				return;

			for ( float_t flRotation = 0.0f; flRotation <= 3.14159265358979323846f * 2.0f; flRotation += flStep ) //-V1034
			{
				Vector vecWorldPosition = Vector( pObject.Radius * cos( flRotation ) + pObject.Pos.x, pObject.Radius * sin( flRotation ) + pObject.Pos.y, pObject.Pos.z );
				if ( SDK::Interfaces::DebugOverlay->ScreenPosition(vecWorldPosition, vecScreenPosition ) )
					continue;

				ImGui::GetBackgroundDrawList( )->AddLine( { vecPreviousScreenPos.x, vecPreviousScreenPos.y }, { vecScreenPosition.x, vecScreenPosition.y }, pObject.Color, 1.0f );

				vecPreviousScreenPos = vecScreenPosition;
			}
		}

		if (draw.m_Type == DrawObject_CIRCLE2D)
		{
			const auto& pObject = std::any_cast< Circle3dObject_t >( draw.m_Object );

			Vector vecScreenPosition = Vector( 0, 0, 0 );
			if ( SDK::Interfaces::DebugOverlay->ScreenPosition( pObject.Pos, vecScreenPosition ) )
				return;

			ImGui::GetBackgroundDrawList( )->AddCircle( ImVec2( vecScreenPosition.x, vecScreenPosition.y ), pObject.Radius, pObject.Color, pObject.Points );
		}


		if ( draw.m_Type == DrawObject_CIRCLE2DFiled )
		{
			const auto& pObject = std::any_cast< Circle3dObject_t >( draw.m_Object );

			Vector vecScreenPosition = Vector( 0, 0, 0 );
			if ( SDK::Interfaces::DebugOverlay->ScreenPosition( pObject.Pos, vecScreenPosition ) )
				return;

			ImGui::GetBackgroundDrawList( )->AddCircleFilled( ImVec2( vecScreenPosition.x, vecScreenPosition.y ), pObject.Radius, pObject.Color, pObject.Points );
		}

		if (draw.m_Type == DrawObject_Arc)
		{
			const auto& pObject = std::any_cast<ArcObject_t>(draw.m_Object);

			ImGui::GetOverlayDrawList()->PathArcTo(pObject.Pos, pObject.Radius, DEG2RAD(pObject.MinAngle), DEG2RAD(pObject.MaxAngle), 32);
			ImGui::GetOverlayDrawList()->PathStroke(pObject.Color, false, pObject.Thikness);
		}

		if (draw.m_Type == DrawObject_TEXT)
		{
			const auto& pObject = std::any_cast<TextObject_t>(draw.m_Object);

			ImGui::GetBackgroundDrawList()->PushTextureID(pObject.m_Font->ContainerAtlas->TexID);

			if (pObject.Outline)
			{
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x + 1, pObject.m_Pos.y + 1), ImColor(0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x - 1, pObject.m_Pos.y - 1 ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x + 1, pObject.m_Pos.y - 1 ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x - 1, pObject.m_Pos.y + 1 ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x, pObject.m_Pos.y + 1 ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x, pObject.m_Pos.y - 1 ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x + 1, pObject.m_Pos.y ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
				ImGui::GetBackgroundDrawList( )->AddText( pObject.m_Font, pObject.m_fFontSize, ImVec2( pObject.m_Pos.x - 1, pObject.m_Pos.y ), ImColor( 0.0f, 0.0f, 0.0f ), pObject.m_Text.c_str( ) );
			}

			ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, pObject.m_Pos, pObject.m_Color, pObject.m_Text.c_str());
			ImGui::GetBackgroundDrawList()->PopTextureID();
		}
	}
}

void C_Renderer::AddLine(ImVec2 min, ImVec2 max, ImColor col, float thickness)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_LINE, LineObject_t{ min, max, col, thickness }));
}

void C_Renderer::AddRect(ImVec2 min, ImVec2 max, ImColor col, float thickness, float rounding, ImDrawCornerFlags corners)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECT, RectObject_t{ min, max, col, thickness, rounding, corners, RectType_Clear }));
}

void C_Renderer::AddRectFilled(ImVec2 min, ImVec2 max, ImColor col, float rounding, ImDrawCornerFlags corners)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECT, RectObject_t{ min, max, col, 0, rounding, corners, RectType_Filled }));
}

void C_Renderer::AddRectFilledMultiColor(ImVec2 min, ImVec2 max, ImColor col, ImColor col2, ImColor col3, ImColor col4)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECTMULTI, RectMultiObject_t{ min, max, col, col2, col3, col4 }));
}

void C_Renderer::AddText(ImFont* font, float font_size, ImVec2 pos, std::string text, ImColor color, bool outline)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_TEXT, TextObject_t{ font, font_size, pos, text, color, outline }));
}

void C_Renderer::AddCircle3D(Vector position, float points, float radius, ImColor color)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_CIRCLE3D, Circle3dObject_t{ position, points, color, radius }));
}

void C_Renderer::AddCircle2D( Vector position, float points, float radius, ImColor color )
{
	m_DrawListQueue.emplace_back( DrawQueueObject_t( DrawObject_CIRCLE2D, Circle3dObject_t { position, points, color, radius } ) );
}

void C_Renderer::AddCircle2DFilled( Vector position, float points, float radius, ImColor color )
{
	m_DrawListQueue.emplace_back( DrawQueueObject_t( DrawObject_CIRCLE2DFiled, Circle3dObject_t { position, points, color, radius } ) );
}

void C_Renderer::AddArc(ImVec2 position, float radius, float min_angle, float max_angle, ImColor col, float thickness)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_Arc, ArcObject_t{ position, radius, min_angle, max_angle, col, thickness }));
}
