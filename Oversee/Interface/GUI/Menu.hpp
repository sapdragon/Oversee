#pragma once
#include "../../Includes.hpp"
#include "../../SDK/SDK.hpp"
#include "../../Libraries/ImGui/imgui.h"
#include "../../Libraries/ImGui/imgui_internal.h"
#include "../../Libraries/ImGui/imgui_impl_dx9.h"
#include "../../Libraries/ImGui/imgui_impl_win32.h"
#include "../Input/Input.hpp"

#include <d3dx9.h>

struct InventoryItem {
	IDirect3DTexture9* m_skin_texture;
};

class C_Menu
{
public:
	virtual void RenderWatermark( );
	virtual void RenderKeybinds( );
	virtual void RenderItemsControllers( );
	virtual void RenderChat( );
	virtual void Instance( );
	virtual void SetupData( );
public:
	virtual bool IsVisible( ) { return m_bIsVisible; };
	virtual void SetVisible( bool bIsVisible ) { m_bIsVisible = bIsVisible; };

	struct
	{
		ImFont* Default14;
		ImFont* Default24;
		ImFont* ESPBold;
		ImFont* MenuIcons;
		ImFont* WeapIcons;
	} m_sFonts;

	ImTextureID m_tAvatars[ 64 ];
	ImTextureID m_tUserAvatar;

	char m_cSearch[ 64 ];
	char m_cConfig[ 64 ];

	int m_iSelectedTab = 1;
	int m_iSelectedSubTab[ 10 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };

private:
	bool m_bIsVisible = true;
	bool m_bToggled = false;
	std::vector<InventoryItem> inv;
};

inline C_Menu* g_Menu = new C_Menu( );