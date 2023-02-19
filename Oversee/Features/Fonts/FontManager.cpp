#include "FontManager.hpp"

void C_FontManager::CreateFonts( )
{
	// create font config
	ImFontConfig cfg;
	cfg.PixelSnapH = 0;
	cfg.OversampleH = 5;
	cfg.OversampleV = 5;
	cfg.RasterizerMultiply = 1.2f;

	for ( auto& m_Font : m_FontQueue )
	{
		ImFont* imCreatedFont;
		if ( m_Font.m_nType ) // create font by name
			imCreatedFont
			=
			ImGui::GetIO( ).Fonts->AddFontFromFileTTF
			(
				( ( std::string ) ( _S( "C:\\Windows\\Fonts\\" ) ) + m_Font.m_szFileName + _S( ".ttf" ) ).c_str( ),
				m_Font.m_nPixelSize,
				&cfg,
				ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( )
			);
		else // create font from memory
			imCreatedFont
			=
			ImGui::GetIO( ).Fonts->AddFontFromMemoryTTF
			(
				m_Font.m_pFontData,
				m_Font.m_nFontDataSize,
				m_Font.m_nPixelSize,
				&cfg,
				ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( )
			);

#ifdef OVERSEE_DEV
		assert( imCreatedFont );
#endif

		m_FontList[ m_Font.m_uFontHash ] = imCreatedFont;
	}

	// clean out the queue 
	m_FontQueue.clear( );

	// build fonts
	ImGuiFreeType::BuildFontAtlas( ImGui::GetIO( ).Fonts, 0x00 );
}
void C_FontManager::PushFont( void* pFontData, int nFontDataSize, const uint32_t& uFontHash, int nFontPixelSize )
{
	FontTask_t FontQueue;
	FontQueue.m_nType = 0;
	FontQueue.m_uFontHash = uFontHash;
	FontQueue.m_nPixelSize = nFontPixelSize;
	FontQueue.m_nFontDataSize = nFontDataSize;
	FontQueue.m_pFontData = pFontData;

	m_FontQueue.emplace_back( FontQueue );
}
void C_FontManager::PushFont( const char* szFileName, const uint32_t& uFontHash, int nFontPixelSize )
{
	FontTask_t FontQueue;
	FontQueue.m_nType = 1;
	FontQueue.m_szFileName = szFileName;
	FontQueue.m_uFontHash = uFontHash;
	FontQueue.m_nPixelSize = nFontPixelSize;

	m_FontQueue.emplace_back( FontQueue );
}

ImFont* C_FontManager::GetFontFromHash( const uint32_t& uFontHash )
{
	ImFont* m_imFont = m_FontList[ uFontHash ];

#ifdef OVERSEE_DEV
	assert( m_imFont );
#endif

	return m_imFont;
}