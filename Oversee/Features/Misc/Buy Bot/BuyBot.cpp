#include "BuyBot.hpp"

void C_BuyBot::OnRoundStart( )
{
	if ( m_bShouldBuy )
		return;

	m_bDidBuy = false;
	m_bShouldBuy = true;
}

void C_BuyBot::OnCreateMove( )
{
	if ( m_bDidBuy || !m_bShouldBuy )
		return;

	std::string strBuyCommand = "";
	if ( !g_SettingsManager->B[ _S( "misc.buybot.enable" ) ] )
		return;

	if ( !g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( ) || g_Globals->m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_nItemID( ) != WEAPON_AWP || !g_SettingsManager->B[ _S( "misc.buybot.keep_awp" ) ] )
	{
		switch ( g_SettingsManager->I[ _S( "misc.buybot.primary" ) ] )
		{
			case 1: strBuyCommand += _S( "buy scar20; buy g3sg1; " ); break;
			case 2: strBuyCommand += _S( "buy ssg08; " ); break;
			case 3: strBuyCommand += _S( "buy awp; " ); break;
			case 4: strBuyCommand += _S( "buy ak47; buy m4a1; " ); break;
		}
	}

	switch ( g_SettingsManager->I[ _S( "misc.buybot.secondary" ) ] )
	{
		case 1: strBuyCommand += _S( "buy fn57; buy tec9; " ); break;
		case 2: strBuyCommand += _S( "buy elite; " ); break;
		case 3: strBuyCommand += _S( "buy deagle; " ); break;
		case 4: strBuyCommand += _S( "buy usp_silencer; buy glock; " ); break;
		case 5: strBuyCommand += _S( "buy p250; " ); break;
	}

	if ( g_SettingsManager->B[ _S( "misc.buybot.molotov" ) ] )
		strBuyCommand += _S( "buy incgrenade; buy molotov; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.smokegrenade" ) ] )
		strBuyCommand += _S( "buy smokegrenade; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.hegrenade" ) ] )
		strBuyCommand += _S( "buy hegrenade; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.taser" ) ] )
		strBuyCommand += _S( "buy taser; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.heavyarmor" ) ] )
		strBuyCommand += _S( "buy heavyarmor; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.vesthelm" ) ] )
		strBuyCommand += _S( "buy vesthelm; " );

	if ( g_SettingsManager->B[ _S( "misc.buybot.defuser" ) ] )
		strBuyCommand += _S( "buy defuser" );

	if ( strBuyCommand.empty( ) )
		return;

	m_bDidBuy = true;
	m_bShouldBuy = false;

	return SDK::Interfaces::EngineClient->ExecuteClientCmd( strBuyCommand.c_str( ) );
}