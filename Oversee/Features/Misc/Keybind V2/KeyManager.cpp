#include "KeyManager.hpp"
#include "../../../SDK/SDK.hpp"
#include "../../../SDK/Utils/Utils.hpp"
#include "../../../SettingsManager.hpp"
#include "../../../Interface/Renderer/Renderer.hpp"

void C_KeyManager::HandleMessage( UINT msg, WPARAM wParam )
{
	const bool bIsInGame = SDK::Interfaces::EngineClient->IsInGame( );
	const bool bIsConnected = SDK::Interfaces::EngineClient->IsConnected( );
	const bool bIsConsoleVisible = SDK::Interfaces::EngineClient->IsConsoleVisible( );
	const bool bIsActiveApp = true;
	const bool bIsChatRaised = g_Utils->IsChatRaised( );

	bool bIsHandlePossible = true;
	if ( !bIsInGame || !bIsConnected || bIsConsoleVisible || !bIsActiveApp || bIsChatRaised )
	{
		bIsHandlePossible = false;
		if ( msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP )
			bIsHandlePossible = true;

		if ( !bIsActiveApp || !bIsConnected || !bIsInGame )
			bIsHandlePossible = false;
	}

	if ( bIsHandlePossible || wParam == VK_INSERT )
	{
		if ( msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN ||
			 msg == WM_XBUTTONDOWN || msg == WM_RBUTTONDOWN ||
			msg == WM_LBUTTONDOWN )
		{
			HandleHoldersDown( msg, wParam );
		}

		if ( msg == WM_KEYUP || msg == WM_SYSKEYUP ||
			 msg == WM_XBUTTONUP || msg == WM_RBUTTONUP ||
			msg == WM_LBUTTONUP )
		{
			HandleSwappers( msg, wParam );
			HandleHolders( msg, wParam );
		}
	}

	HandleM3( msg, wParam );
}

void C_KeyManager::RegisterKey( OnKeyAction_t Action )
{
	this->m_vKeyActions.push_back( Action );
}

void C_KeyManager::HandleSwappers( UINT msg, WPARAM wParam ) {
	for ( auto& a : m_vKeyActions )
	{
		if ( a.m_uiKeyID == 0x04 )
			continue;

		DWORD dwParam = wParam;
		if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 )
			dwParam = VK_XBUTTON1;
		else if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON2 )
			dwParam = VK_XBUTTON2;

		if ( a.m_uiKeyID == wParam || a.m_uiKeyID == dwParam )
		{
			if ( a.m_tActionType == KeyActionType::KEY_SWAP )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					if ( g_SettingsManager->B[ a.m_sVariableMapName ] == std::any_cast< bool >( a.defaultValue ) )
					{
						g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					if ( g_SettingsManager->I[ a.m_sVariableMapName ] == std::any_cast< int >( a.defaultValue ) )
					{
						g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					if ( g_SettingsManager->F[ a.m_sVariableMapName ] == std::any_cast< float >( a.defaultValue ) )
					{
						g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
			}
		}
	}
}

void C_KeyManager::HandleHolders( UINT msg, WPARAM wParam ) {
	DWORD dwParam = wParam;
	if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 )
		dwParam = VK_XBUTTON1;
	else if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON2 )
		dwParam = VK_XBUTTON2;

	for ( auto& a : m_vKeyActions )
	{
		if ( a.m_uiKeyID == 4 )
			continue;

		if ( a.m_uiKeyID == wParam || a.m_uiKeyID == dwParam )
		{
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_ON )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );

					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );

					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );

					a.m_iStatus = 0;
				}
			}
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_OFF )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );

					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );

					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );

					a.m_iStatus = 0;
				}
			}
		}
	}
}

void C_KeyManager::HandleHoldersDown( UINT msg, WPARAM wParam ) {
	DWORD dwParam = wParam;
	if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 )
		dwParam = VK_XBUTTON1;
	else if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON2 )
		dwParam = VK_XBUTTON2;

	for ( auto& a : m_vKeyActions )
	{
		if ( a.m_uiKeyID == 0x04 )
			continue;

		if ( a.m_uiKeyID == wParam || a.m_uiKeyID == dwParam )
		{
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_ON )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );
					a.m_iStatus = 1;
				}
			}
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_OFF )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );
					a.m_iStatus = 1;
				}
			}
		}
	}
}

void C_KeyManager::HandleM3( UINT msg, WPARAM wParam ) {
	for ( auto& a : m_vKeyActions )
	{
		if ( a.m_uiKeyID != 4 )
			continue;

		if ( msg == WM_MBUTTONUP )
		{
			if ( a.m_tActionType == KeyActionType::KEY_SWAP )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					if ( g_SettingsManager->B[ a.m_sVariableMapName ] == std::any_cast< bool >( a.defaultValue ) )
					{
						g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					if ( g_SettingsManager->I[ a.m_sVariableMapName ] == std::any_cast< int >( a.defaultValue ) )
					{
						g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					if ( g_SettingsManager->F[ a.m_sVariableMapName ] == std::any_cast< float >( a.defaultValue ) )
					{
						g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );
						a.m_iStatus = 1;
					}
					else
					{
						g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );
						a.m_iStatus = 0;
					}
				}
			}


			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_ON )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );
					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );
					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );
					a.m_iStatus = 0;
				}
			}
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_OFF )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );
					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );
					a.m_iStatus = 0;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );
					a.m_iStatus = 0;
				}
			}
		}

		if ( msg == WM_MBUTTONDOWN )
		{
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_ON )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.newValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.newValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.newValue );
					a.m_iStatus = 1;
				}
			}
			if ( a.m_tActionType == KeyActionType::KEY_HOLDING_OFF )
			{
				if ( a.m_tVariableType == KeyVariableType::BOOL_KEY_TYPE )
				{
					g_SettingsManager->B[ a.m_sVariableMapName ] = std::any_cast< bool >( a.defaultValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::INT_KEY_TYPE )
				{
					g_SettingsManager->I[ a.m_sVariableMapName ] = std::any_cast< int >( a.defaultValue );
					a.m_iStatus = 1;
				}
				if ( a.m_tVariableType == KeyVariableType::FLOAT_KEY_TYPE )
				{
					g_SettingsManager->F[ a.m_sVariableMapName ] = std::any_cast< float >( a.defaultValue );
					a.m_iStatus = 1;
				}
			}
		}
	}
}

std::vector<OnKeyAction_t> C_KeyManager::GetVariableAction( std::string var_name )
{
	std::vector<OnKeyAction_t> tempVector;
	for ( auto a : m_vKeyActions ) {
		if ( a.m_sVariableMapName == var_name )
			tempVector.push_back( a );
	}
	return tempVector;
}