#pragma once
#include <Windows.h>
#include <array>

class C_InputSystem
{
public:
	virtual void SetupData( HWND hWND );
	virtual void HandleMessage( UINT msg, WPARAM wParam );

	virtual bool IsToggled( DWORD dwKey )
	{
		return m_ToggledKeys[ dwKey ];
	}
	virtual bool IsKeyHeld( DWORD dwKey )
	{
		return m_HeldKeys[ dwKey ];
	}

	LONG_PTR m_hTargetWindow = NULL;
private:
	static LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
private:
private:
	std::array < bool, 256 > m_HeldKeys = { };
	std::array < bool, 256 > m_ToggledKeys = { };
};

inline C_InputSystem* g_InputSystem = new C_InputSystem( );