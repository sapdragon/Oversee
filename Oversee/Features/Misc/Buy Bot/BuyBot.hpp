#pragma once
#pragma once
#include "../../../SDK/SDK.hpp"

class C_BuyBot
{
public:
	virtual void OnCreateMove( );
	virtual void OnRoundStart( );
private:
	bool m_bDidBuy = false;
	bool m_bShouldBuy = false;
};

inline C_BuyBot* g_BuyBot = new C_BuyBot( );