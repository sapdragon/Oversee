#pragma once
#include "../Includes.hpp"

#include "Misc/VFunc.hpp"
#include "Misc/Studio.hpp"
#include "Misc/DataMap.hpp"
#include "Misc/Convar.hpp"

#include "Interfaces/ICHLClient.hpp"
#include "Interfaces/IMaterialSystem.hpp"
#include "Interfaces/IVModelInfoClient.hpp"
#include "Interfaces/IVModelRender.hpp"
#include "Interfaces/IVEngineClient.hpp"
#include "Interfaces/IClientEntityList.hpp"
#include "Interfaces/IRenderView.hpp"
#include "Interfaces/ISurface.hpp"
#include "Interfaces/IEngineTrace.hpp"
#include "Interfaces/IViewRenderBeams.hpp"
#include "Interfaces/IVDebugOverlay.hpp"
#include "Interfaces/IPrediction.hpp"
#include "Interfaces/IGameMovement.hpp"
#include "Interfaces/IGameEventmanager.hpp"
#include "Interfaces/IGameRules.hpp"
#include "Interfaces/IGlobalVars.hpp"
#include "Interfaces/IGlowObjectManager.hpp"
#include "Interfaces/ILocalize.hpp"
#include "Interfaces/IFileSystem.hpp"
#include "Interfaces/IGlowObjectManager.hpp"
#include "Interfaces/IVDebugOverlay.hpp"
#include "Interfaces/IEngineSound.hpp"
#include "Interfaces/ICvar.hpp"
#include "Interfaces/Input.hpp"
#include "Interfaces/IViewRenderBeams.hpp"
#include "Interfaces/IPanel.hpp"
#include "Interfaces/IPhysics.hpp"
#include "Interfaces/IClientState.hpp"
#include "Interfaces/IMemAllocate.hpp"
#include "Interfaces/IPlayerInventory.hpp"
#include "Interfaces/IPlayerResource.hpp"
#include "Interfaces/IInventoryManager.hpp"
#include "Interfaces/Panorama/IPanorama.hpp"
#include "Netvars/Netvars.hpp"
#include "Math/Math.hpp"
#include "Interfaces/IMDLCache.hpp"
#include "Steam/isteamclient.h"
#include "Steam/isteamuser.h"
#include "Steam/isteamutils.h"
#include "Steam/isteamgamecoordinator.h"
#include "Steam/isteammatchmaking.h"
#include "Interfaces/IEffects.hpp"

struct DormantPlayer_t
{
	float m_DormantAlpha = 0.0f;
	float m_LastDormantTime = 0.0f;
	float m_LastNonDormantTime = 0.0f;

	Vector m_DormantOrigin = Vector( 0, 0, 0 );
	Vector m_NetworkOrigin = Vector( 0, 0, 0 );

	int m_LastDormantHealth = 0;
	int m_LastEventWeaponID = 0;
	int m_LastEventWeaponType = 0;
};
struct RadarDataUpdate_t
{
	char PAD[ 0x8 ];
	int m_nEntityID;
	int m_nClassID;
	Vector m_vecOrigin;
	int m_nAngleYaw;
	bool m_bIsDefuser;
	bool m_bPlayerHasDefuser;
	bool m_bPlayerHasC4;
};
struct SoundDataUpdate_t
{
	char PAD[ 0x8 ];
	Vector m_vecOrigin;
	uint32_t m_nVolume;
	float m_flDelayValue;
	int m_nSequenceNumber;
	int m_nEntityIndex;
	int m_nChannel;
	int m_nPitch;
	int m_nFlags;
	uint32_t m_nSoundNum;
	uint32_t m_nSoundNumHandle;
	int m_nSpeakerEntity;
	int m_nRandomSeed;
	int m_nSoundLevel;
	bool m_bIsSentence;
	bool m_bIsAmbient;
};

class C_VoiceCommunicationData
{
public:
	uint32_t m_nXuidLow { };
	uint32_t m_nXuidHigh { };
	int32_t m_nSequenceBytes { };
	uint32_t m_nSectionNumber { };
	uint32_t m_nUnCompressedSampleOffset;

	__forceinline uint8_t* GetRawData( )
	{
		return  ( uint8_t* ) this;
	}

};
class C_CLCMsg_VoiceData
{
public:
	uint32_t INetMessageVTable; //0x0000
	char PAD[ 4 ]; //0x0004
	uint32_t CCLCMsg_VoiceData_Vtable; //0x0008
	char PAD0C[ 8 ]; //0x000C
	void* pData; //0x0014
	uint32_t m_nXuidLow { };
	uint32_t m_nXuidHigh { };
	int32_t m_nFormat; //0x0020
	int32_t m_nSequenceBytes; //0x0024
	uint32_t m_nSectionNumber; //0x0028
	uint32_t m_nUnCompressedSampleOffset; //0x002C
	int32_t m_nCachedSize; //0x0030

	uint32_t m_nFlags; //0x0034

	uint8_t NoStackOverFlow[ 0xFF ]; // fix clown crashers

	__forceinline void SetData( C_VoiceCommunicationData* pData )
	{
		m_nXuidLow = pData->m_nXuidLow;
		m_nXuidHigh = pData->m_nXuidHigh;
		m_nSequenceBytes = pData->m_nSequenceBytes;
		m_nSectionNumber = pData->m_nSectionNumber;
		m_nUnCompressedSampleOffset = pData->m_nUnCompressedSampleOffset;
	}
};
class C_SVCMsg_GameEvent
{
public:
	char PAD[ 0x8 ];
};
class C_SVCMsg_VoiceData
{
public:
	char PAD[ 0x8 ];
	int m_iClient;
	int m_nAudibleMask;
	uint32_t m_nXuidLow { };
	uint32_t m_nXuidHigh { };
	std::string* m_szVoiceData;
	bool m_bProximity;
	bool m_bCaster;
	int m_nFormat;
	int m_nSequenceBytes;
	uint32_t m_nSectionNumber;
	uint32_t m_nUnCompressedSampleOffset;

	__forceinline C_VoiceCommunicationData GetData( )
	{
		C_VoiceCommunicationData cData;
		cData.m_nXuidLow = m_nXuidLow;
		cData.m_nXuidHigh = m_nXuidHigh;
		cData.m_nSequenceBytes = m_nSequenceBytes;
		cData.m_nSectionNumber = m_nSectionNumber;
		cData.m_nUnCompressedSampleOffset = m_nUnCompressedSampleOffset;
		return cData;
	}
};
class C_SVCMsg_SoundData
{
public:
	char PAD[ 0x8 ];
	void* m_pAllocation;
	int m_nSize;
};
class CCSUsrMsg_ProcessSpottedEntityUpdate_SpottedEntityUpdate
{
public:
	char PAD[ 0xC ];
	int m_nSize;
};
class CCSUsrMsg_ReportHit
{
public:
	char PAD[ 0x8 ];
	float m_flPosX;
	float m_flPosY;
	float m_flTime;
	float m_flPosZ;
};

class CPanoramaMarshallHelper;

namespace SDK
{
	void Initialize( HINSTANCE hInstance );

	template< typename T >
	inline T* GetInterface( HMODULE hModule, const char* InterfaceName )
	{
		typedef void* ( *CreateInterfaceFn )( const char*, int* );
		return reinterpret_cast < T* >( reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hModule, _S( "CreateInterface" ) ) )( InterfaceName, NULL ) );
	}

	namespace Interfaces
	{
		inline ICHLClient* CHLClient = nullptr;
		inline IMaterialSystem* MaterialSystem = nullptr;
		inline IVModelRender* ModelRender = nullptr;
		inline IEngineSound* EngineSound = nullptr;
		inline IVModelInfoClient* ModelInfo = nullptr;
		inline IVEngineClient* EngineClient = nullptr;
		inline IClientEntityList* EntityList = nullptr;
		inline IDirect3DDevice9* DirectDevice = nullptr;
		inline IVRenderView* RenderView = nullptr;
		inline ISurface* Surface = nullptr;
		inline IPlayerResource** PlayerResource = nullptr;

		inline IVDebugOverlay* DebugOverlay = nullptr;
		inline IGameRules** GameRules = nullptr;
		inline ILocalize* Localize = nullptr;
		inline ICVar* CVar = nullptr;
		inline IPanel* Panel = nullptr;
		inline IGlowObjectManager* GlowObjManager = nullptr;
		inline IViewRenderBeams* RenderBeams = nullptr;
		inline IPrediction* Prediction = nullptr;
		inline IGameMovement* GameMovement = nullptr;
		inline IGlobalVarsBase* GlobalVars = nullptr;
		inline IInput* Input = nullptr;
		inline IMoveHelper* MoveHelper = nullptr;
		inline IGameEventManager* EventManager = nullptr;
		inline IEngineTrace* EngineTrace = nullptr;
		inline IPhysicsSurfaceProps* PhysicsSurface = nullptr;
		inline IClientState* ClientState = nullptr;
		inline void* ClientMode = nullptr;
		inline IMemAlloc* MemAlloc = nullptr;
		inline IStudioRender* StudioRender = nullptr;
		inline IInventoryManager* InventoryManager = nullptr;
		inline IFileSystem* FileSystem = nullptr;
		inline IPanoramaUIEngine* PanoramaEngine = nullptr;
		inline CPanoramaMarshallHelper* PanoramaHelper = nullptr;
		inline C_MDLCache* MDLCache = nullptr;
		inline IVEffects* Effects = nullptr;

	}
	namespace Steam
	{
		inline ISteamClient* SteamClient = nullptr;
		inline ISteamHTTP* SteamHTTP = nullptr;
		inline ISteamUser* SteamUser = nullptr;
		inline ISteamFriends* SteamFriends = nullptr;
		inline ISteamInventory* SteamInventory = nullptr;
		inline ISteamGameCoordinator* GameCoordinator = nullptr;
		inline ISteamMatchmaking* SteamMatchMaking = nullptr;
		inline ISteamUtils* SteamUtils = nullptr;
	}
	namespace EngineData
	{
		inline std::unordered_map < CheatAddressList, void* > m_AddressList;
		inline std::unordered_map < CheatConvarList, ConVar* > m_ConvarList;
	}

	inline DWORD FindInDatamap( C_DataMap* pDataMap, const std::string& szPropName )
	{
		while ( pDataMap )
		{
			for ( int i = 0; i < pDataMap->m_iDataNumFields; i++ )
			{
				if ( !pDataMap->m_pDataDescription[ i ].fieldName )
					continue;

				if ( pDataMap->m_pDataDescription[ i ].fieldName == szPropName )
					return pDataMap->m_pDataDescription[ i ].fieldOffset;

				if ( pDataMap->m_pDataDescription[ i ].fieldType != FIELD_EMBEDDED )
					continue;

				if ( !pDataMap->m_pDataDescription[ i ].td )
					continue;

				DWORD dwOffset = FindInDatamap( pDataMap->m_pDataDescription[ i ].td, szPropName );
				if ( !dwOffset )
					continue;

				return dwOffset;
			}

			pDataMap = pDataMap->m_pBaseMap;
		}

		return 0;
	}
}

#define NETVAR( name, type, table, prop ) \
__forceinline type& name( ) \
{\
    static uint32_t g_##name = 0; \
    if ( !g_##name )\
        g_##name = NetVars::GetPropOffset( _S( table ), _S( prop ) );\
    return *( type* )( ( DWORD )( this ) + g_##name ); \
}\

#define DATAMAP( type, name ) \
__forceinline type& name( ) \
{\
    static DWORD dwDatamapOffset_##name = 0;\
    if ( !dwDatamapOffset_##name )\
        dwDatamapOffset_##name = SDK::FindInDatamap( this->GetPredDescMap( ), _S( #name ) );\
    return *( type* ) ( ( DWORD )( this ) + dwDatamapOffset_##name );\
}\

#define CUSTOM_OFFSET( name, type, offset ) \
__forceinline type& name( ) \
{\
    return *( type* )( ( DWORD )( this ) + offset ); \
}\

#include "Misc/EHandle.hpp"
#include "Structs/IInitRandomColor.hpp"
#include "Structs/IEconItemView.hpp"
#include "Structs/ISharedObject.hpp"
#include "Structs/ICollisionProperty.hpp"
#include "Structs/IBaseEntity.hpp"
#include "Structs/IBaseCombatWeapon.hpp"
#include "Structs/IBaseViewmodel.hpp"
#include "Structs/IBasePlayer.hpp"
#include "Structs/IEconItem.hpp"
#include "Structs/IFogController.hpp"
#include "Globals.hpp"