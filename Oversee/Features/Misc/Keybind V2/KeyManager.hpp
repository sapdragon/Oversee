#pragma once

#include <any>
#include <mutex>
#include <string>
#include <vector>
#include <functional>

#include "../../../Interface/Input/Input.hpp"

enum KeyActionType : int {
	KEY_SWAP,
	KEY_HOLDING_ON,
	KEY_HOLDING_OFF,
};

enum KeyVariableType : int {
	INT_KEY_TYPE,
	FLOAT_KEY_TYPE,
	BOOL_KEY_TYPE,
};

struct OnKeyAction_t {
	int m_tActionType;
	int m_tVariableType;

	std::string m_sRenderableName;
	std::string m_sVariableMapName;

	std::any defaultValue = 0;
	std::any newValue = 0;

	int m_uiKeyID;

	int m_iStatus = 0;
};

class C_KeyManager {
public:
	virtual void HandleMessage( UINT msg, WPARAM wParam );

	virtual void HandleSwappers( UINT msg, WPARAM wParam );
	virtual void HandleHolders( UINT msg, WPARAM wParam );
	virtual void HandleHoldersDown(UINT msg, WPARAM wParam);

	virtual void HandleM3(UINT msg, WPARAM wParam);

	virtual void RegisterKey( OnKeyAction_t Action );
	virtual std::vector<OnKeyAction_t> GetVariableAction( std::string var_name );

	std::vector<OnKeyAction_t> m_vKeyActions;
};

inline C_KeyManager* g_KeyManager = new C_KeyManager( );
