#pragma once
#include "../../SDK/SDK.hpp"
#include "../../SDK/Misc/Panorama.hpp"

inline std::size_t iLastEquipped = 0;
inline int32_t iUniqueID = 1;

class C_InventoryChanger
{
public:
	virtual void SequenceRemapping( CRecvProxyData* pData, C_BaseViewmodel* entity );
	virtual int32_t GetNewAnimation( const uint32_t model, const int32_t sequence );
	virtual void ApplyGlove( IPlayerInventory* LocalInventory, PlayerInfo_t info );
	virtual void ApplyWeapons( IPlayerInventory* LocalInventory, PlayerInfo_t info );
	virtual void ApplyKnifes( IPlayerInventory* LocalInventory, PlayerInfo_t info );
	virtual void ApplyMedals( IPlayerInventory* LocalInventory );
	virtual void ApplyMusic( IPlayerInventory* LocalInventory );
	virtual void ApplyAgents( IPlayerInventory* LocalInventory, ClientFrameStage_t Stage );
	virtual void AddItemToInventory( IPlayerInventory* pLocalInventory, InventoryItem_t item, bool bAcKnowledge = true, std::string szParameter = "" );
	virtual void OnPostDataUpdateStart( IPlayerInventory* LocalInventory );
	virtual void OnSoUpdated( C_SharedObject* object, int32_t event );
	virtual void OnEquipItemInLoadOut( int32_t team, int32_t slot, uint64_t item_id );
	virtual void ChangeHudIcons( IPlayerInventory* LocalInventory, IGameEvent* event );
	virtual void LiveStatTrack( IPlayerInventory* LocalInventory, IGameEvent* event );
	virtual void OnFrameStageNotify( ClientFrameStage_t stage );
	virtual void OnEvent( IGameEvent* event );
private:
	int m_nModelIdx = 0;
	int m_nOriginalIdx = 0;
};
inline C_InventoryChanger* g_InventoryChanger = new C_InventoryChanger( );