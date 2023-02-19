#pragma once
#include "../../../SDK/SDK.hpp"

class C_DrawModel
{
public:
	virtual void Instance( );
	virtual void RenderPreview( );
	virtual ITexture* GetTexture( ) { return this->m_PreviewTexture; };
	virtual void SetChamsSettings( ChamsSettings_t Settings ) { this->m_ChamsSettings = Settings; };
	virtual void SetGlow( int glow ) { this->m_iGlow = glow; };
	virtual void SetGlowColor( Color glow ) { this->m_GlowColor = glow; };
private:
	ITexture* m_PreviewTexture = nullptr;
	ITexture* m_CubemapTexture = nullptr;
	C_MergedMDL* m_PreviewModel = nullptr;

	C_ViewSetup m_ViewSetup = { };
	bool isMouseInAction = false;
	float RecalculateAnimation = 0.f;
	bool RecalculateAnimationFlag = false;

	ChamsSettings_t m_ChamsSettings = { };

	int m_iGlow = 0;
	Color m_GlowColor = Color( 255, 255, 255, 255 );
};

enum DraggableItemCondiction : int {
	LEFT_COND = 0,
	RIGHT_COND = 1,
	TOP_COND = 2,
	BOT_COND = 3,
	CENTER_COND = 4,
	POOL_COND = 5,
	IN_MOVE_COND = 6,
};

struct MovableItems {
	std::string	ItemName;
	ImVec2 TemporaryPos = ImVec2( );
	ImVec2 BasicPositions = ImVec2( );
	ImVec2 WidgetSize = ImVec2( );

	int Draw;
	int VectorCond = 4;

	int Type = 0; // 0 - text, 1 - bar, 2 - box

	MovableItems(
		int drw,
		std::string name = "Default",
		int cond = 4,
		ImVec2 temp1 = ImVec2 { },
		ImVec2 temp2 = ImVec2 { }, int type = 0 )
	{
		Draw = drw;
		ItemName = name;
		VectorCond = cond;
		TemporaryPos = temp1;
		BasicPositions = temp2;
		Type = type;
	}
};


extern std::vector<std::vector<MovableItems>> Pool;


inline C_DrawModel* g_DrawModel = new C_DrawModel( );