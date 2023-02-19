#pragma once
#include "../../Includes.hpp"
#include "../../Libraries/ImGui/imgui_freetype.h"

struct FontTask_t
{
	int m_nType = 0;
	uint32_t m_uFontHash = 0;
	std::string m_szFileName = "";
	int m_nPixelSize = 0;
	void* m_pFontData = nullptr;
	int m_nFontDataSize = 0;
};
class C_FontManager
{
public:
	virtual void CreateFonts( );
	virtual void PushFont( void* pFontData, int nFontSize, const uint32_t& uFontHash, int nFontPixelSize );
	virtual void PushFont( const char* szFileName, const uint32_t& uFontHash, int nFontPixelSize );
	virtual ImFont* GetFontFromHash( const uint32_t& uFontHash );
private:
	std::unordered_map < uint32_t, ImFont* > m_FontList;
protected:
	std::vector < FontTask_t > m_FontQueue;
};

inline std::shared_ptr < C_FontManager > g_FontManager = std::make_shared < C_FontManager > ( C_FontManager( ) );