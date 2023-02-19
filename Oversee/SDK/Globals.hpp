#pragma once
#include <string>
#include <string.h>
#include "../SDK/Math/QAngle.hpp"

class C_UserCmd;
class C_BasePlayer;
class C_Globals
{
public:
	struct
	{
		unsigned long long m_bOpenedCase = false;
		bool m_bNeedFullUpdate = false;
	} m_Inventory;

	struct
	{
		std::string m_szName;
		std::string m_szPass;
		std::string m_szHWID;
	};

	struct
	{
		// user cmd
		C_UserCmd* m_pCmd = nullptr;
	
		// global sendpacket
		bool* m_bSendPacket = nullptr;
		bool m_bCanFire = false;
		bool m_bCanFireRev = false;
		bool m_bFakeDucking = false;
		bool m_bVisualFakeDucking = false;
		bool m_bInCreateMove = false;
		bool m_bIsValidPacket = false;
		bool m_bDoubleTap = false;
		bool m_bSkipMatrix = false;
		bool m_bSkipStrafe = false;
		bool m_bForcePacket = false;
		bool m_bAntiExploit = false;

		// ticksallowed from server
		int m_nTicksAllowed = 0;
		int m_nSequence = 0;

		// AA angles
		QAngle m_angFakeAngles = QAngle( 0, 0, 0 );
		QAngle m_angLastAngles = QAngle( 0, 0, 0 );
	} m_Packet;

	struct 
	{
		bool m_bUpdateAnims = false;
		bool m_bSetupBones = false;
	} m_Animations;

	struct
	{
		bool m_bScriptLoaded = false;
	} m_Lua;

	struct
	{
		int m_nScreenSizeX;
		int m_nScreenSizeY;
	} m_ScreenData;

	struct
	{
		bool m_bShouldClearDeathNotices = false;
	} m_RoundInfo;

	struct
	{
		bool m_bDrawModel = false;
		float m_flModelRotation = -180.f;
	} m_Model;
	
	C_BasePlayer* m_LocalPlayer = nullptr;
};

inline C_Globals* g_Globals = new C_Globals( );