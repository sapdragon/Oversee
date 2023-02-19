#pragma once
#include <deque>
#include "../../../SDK/SDK.hpp"

enum class EventIcons : int
{
	CheckMark = 0,
	Cross,
	Basket,
	Bomb
};

struct EventLog_t
{
	float m_flTime = 0.0f;
	float m_flAnimProgress = 0.0f;
	float m_flSpacing = 0.0f;

	bool m_bDidPrint = false;

	std::string m_szString = "";

	Color m_LogColor = Color( 255, 255, 255, 255 );
	EventIcons m_nLogIcon = EventIcons::CheckMark;
};
class C_LogManager
{
public:
	virtual void Instance( );
	virtual std::string& GetIconFromID( EventIcons m_nIconID );
	virtual void PushLog( const std::string& m_szString, const EventIcons m_nIcon, const Color& aIconColor = Color( 0, 220, 255, 255 ) );
private:
	std::deque < EventLog_t > m_EventLogs;
};

inline C_LogManager* g_LogManager = new C_LogManager( );