#pragma once
#include <cstdint>

struct PlayerInfo_t
{
	int64_t iSomething;
	union
	{
		int64_t       m_iSteamID64;          //0x0008 - SteamID64
		struct
		{
			int     m_iXuidLow;
			int     m_iXuidHigh;
		};
	};
	char            m_strName[128];
	int             m_iUserId;
	char            m_strSteamID[20];
	char            pad_0x00A8[0x10];
	unsigned long   m_iSteamID;
	char            m_strFriendsName[128];
	bool            m_bIsFakePlayer;
	bool            m_bIsHLTV;
	unsigned int    m_aCustomFiles[4];
	unsigned char   m_bFilesDownloaded;
};