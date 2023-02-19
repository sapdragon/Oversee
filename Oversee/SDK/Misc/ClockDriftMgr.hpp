#pragma once
#include <cstdint>

struct C_ClockDriftMgr
{
	float m_ClockOffsets[16];   //0x0000
	int m_iCurClockOffset; //0x0044
	int m_nServerTick;     //0x0048
	int m_iClientTick;     //0x004C
}; //Size: 0x0050