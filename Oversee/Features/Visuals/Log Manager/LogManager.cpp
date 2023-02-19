#include "LogManager.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"
#include "../../Fonts/FontManager.hpp"

constexpr float LOG_SPACING = 19.0f;
constexpr float LOG_ANIM_TIME = 0.15f;
constexpr float LOG_EXIST_TIME = 4.0f;
constexpr float LOG_ICON_SIZE = 30.0f;

constexpr uint8_t g_aPrimaryColor[ 4 ] = { 0, 128, 255, 255 };
constexpr uint8_t g_aSecondaryColor[ 4 ] = { 255, 255, 255, 255 };

void C_LogManager::Instance( )
{
	// get the time
	float flRealTime = SDK::Interfaces::GlobalVars->m_flRealTime;

	// get anim speed
	float flAnimSpeed = SDK::Interfaces::GlobalVars->m_flFrameTime * ( 1.0f / LOG_ANIM_TIME );

	// log font
	ImFont* imLogFont = g_FontManager->GetFontFromHash( FNV1A( "Logs text" ) );
	if ( !imLogFont )
		return;

	ImFont* imIconFont = g_FontManager->GetFontFromHash( FNV1A( "Logs icons" ) );
	if ( !imIconFont )
		return;

	// iterate all logs
	for ( int nLog = 0; nLog < m_EventLogs.size( ); nLog++ )
	{
		// get event log
		EventLog_t* m_EventLog = &m_EventLogs[ nLog ];

		// erase too old logs
		if ( flRealTime - m_EventLog->m_flTime >= LOG_EXIST_TIME )
		{
			m_EventLogs.erase( m_EventLogs.begin( ) + nLog );
			continue;
		}

		// calculate anim progress
		if ( flRealTime - m_EventLog->m_flTime <= LOG_ANIM_TIME )
		{
			// interpolate item anim progress
			m_EventLog->m_flAnimProgress = std::clamp( Math::Interpolate( m_EventLog->m_flAnimProgress, 1.0f, flAnimSpeed ), 0.0f, 1.0f );

			// do not animate spacing on item destroy
			m_EventLog->m_flSpacing = ( LOG_SPACING + 2.0f ) * m_EventLog->m_flAnimProgress;
		}
		else if ( flRealTime - m_EventLog->m_flTime >= LOG_EXIST_TIME - LOG_ANIM_TIME )
			m_EventLog->m_flAnimProgress = std::clamp( Math::Interpolate( m_EventLog->m_flAnimProgress, 0.0f, flAnimSpeed ), 0.0f, 1.0f );

		/* draw */

		// animate items
		float flTextAlpha = 255.0f * m_EventLog->m_flAnimProgress;
		float flBackAlpha = 50.0f * m_EventLog->m_flAnimProgress;

		// calculate item spacing relative to previous items
		float flTotalSpacing = 0.0f;
		for ( int nPrevLog = 0; nPrevLog < nLog; nPrevLog++ )
			flTotalSpacing += m_EventLogs[ nPrevLog ].m_flSpacing + 1.0f;

		// calculate text size
		float flTextSize = imLogFont->CalcTextSizeA( imLogFont->FontSize, FLT_MAX, 0.0f, m_EventLog->m_szString.c_str( ) ).x;

		// get bg text
		ImColor imBackText =
			ImColor
			(
				45, 45, 45,
				static_cast < int > ( 10 * flBackAlpha )
			);

		ImColor imBackText2 =
			ImColor
			(
				45, 45, 45, 0
			);

		g_Renderer->AddRectFilledMultiColor( ImVec2( 0.0f + 10, flTotalSpacing + 10 ), ImVec2( 24.0f + flTextSize + 10.0f + 20, flTotalSpacing + LOG_SPACING + 10 ),
			imBackText, imBackText2, imBackText2, imBackText );

		// get icon color
		ImColor imIconColor =
			ImColor
			(
				m_EventLog->m_LogColor.r( ),
				m_EventLog->m_LogColor.g( ),
				m_EventLog->m_LogColor.b( ),
				static_cast < int > ( flTextAlpha )
			);

		// draw log icon
		g_Renderer->AddText( imIconFont, imIconFont->FontSize, ImVec2( 4.0f + 10, flTotalSpacing + 3.0f + 10 ), g_LogManager->GetIconFromID( m_EventLog->m_nLogIcon ), imIconColor, false );

		// get log text
		ImColor imTextColor =
			ImColor
			(
				255,
				255,
				255,
				static_cast < int > ( flTextAlpha )
			);

		// draw log text
		g_Renderer->AddText( imLogFont, imLogFont->FontSize, ImVec2( 20.0f + 10, flTotalSpacing + 1.0f + 10 ), m_EventLog->m_szString, imTextColor, false );
	}
}
std::string& C_LogManager::GetIconFromID( EventIcons nIconID )
{
	std::string szIcon = "";
	switch ( nIconID )
	{
		case EventIcons::CheckMark:
		{
			szIcon = _S( "1" );
		}
		break;

		case EventIcons::Cross:
		{
			szIcon = _S( "2" );
		}
		break;

		case EventIcons::Basket:
		{
			szIcon = _S( "3" );
		}
		break;

		case EventIcons::Bomb:
		{
			szIcon = _S( "4" );
		}
		break;
	}

	return szIcon;
}
void C_LogManager::PushLog( const std::string& m_szString, const EventIcons m_nIcon, const Color& aIconColor )
{
	EventLog_t* m_EventLog = &m_EventLogs.emplace_front( );

	m_EventLog->m_flTime = SDK::Interfaces::GlobalVars->m_flRealTime;
	m_EventLog->m_szString = m_szString;
	m_EventLog->m_nLogIcon = m_nIcon;
	m_EventLog->m_LogColor = aIconColor;

	/* print to the console */
	SDK::Interfaces::CVar->ConsoleColorPrintf( g_aSecondaryColor, _S( "[ " ) );
	SDK::Interfaces::CVar->ConsoleColorPrintf( g_aPrimaryColor, _S( "oversee" ) );
	SDK::Interfaces::CVar->ConsoleColorPrintf( g_aSecondaryColor, _S( ".one ] %s\n" ), m_EventLog->m_szString.c_str( ) );
}