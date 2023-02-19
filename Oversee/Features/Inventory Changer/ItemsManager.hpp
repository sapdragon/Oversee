#pragma once
#include "../../SDK/SDK.hpp"

struct WeaponItem_t {
	C_EconItemDefinition* m_tItem;
	C_EconPaintKitDefinition* m_tPaintKit;
	std::string m_szName;
	IDirect3DTexture9* m_tTexture;
};

struct DefaultItem_t {
	int16_t m_nItemID;
	std::string m_szName;
	IDirect3DTexture9* m_tTexture;
};

struct GloveItem_t {
	int16_t m_nItemID;
	std::string m_szName;
	int32_t m_nKitID;
	IDirect3DTexture9* m_tTexture;
};

struct AgentsItem_t {
	int16_t m_nItemID;
	std::string m_szName;
	int m_iTeamID;
	IDirect3DTexture9* m_tTexture;
};

struct MusicItem_t {
	uint32_t m_nItemID;
	C_EconMusicDefinition* m_tPaintKit;
	IDirect3DTexture9* m_tTexture;
};

struct StickerItem_t {
	C_EconStickerDefinition* m_tPaintKit;
	IDirect3DTexture9* m_tTexture;
};

class C_ItemManager
{
public:
	void Initialize( );
	void ImagePrecache( );
	IDirect3DTexture9* GetSkin( std::string path );
	std::string GetSkinPath( int16_t m_nItemID, int32_t m_iPaintKit );

	std::unordered_map<uint32_t, C_EconItemDefinition*> items;
	std::unordered_map<uint32_t, C_EconPaintKitDefinition*> PaintKits;
	std::unordered_map<uint32_t, C_EconStickerDefinition*> StickerKits;
	std::unordered_map<uint32_t, C_EconMusicDefinition*> MusicKits;
	std::unordered_map<uint32_t, std::vector<C_EconPaintKitDefinition*>> PaintKitsType;

	std::vector<DefaultItem_t*> m_vDefaults;
	std::vector<GloveItem_t*> m_vGloves;
	std::vector<AgentsItem_t*> m_vAgents;
	std::vector<WeaponItem_t*> m_vWeapons;
	std::vector<MusicItem_t*> m_vMusicKits;
	std::vector<StickerItem_t*> m_vStickers;

	bool Initialized = false;
};

inline C_ItemManager g_ItemManager;

static VOID WINAPI StartImagePrecaching( )
{
	return ExitThread( NULL );
}