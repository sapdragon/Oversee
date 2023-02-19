#include "../SDK.hpp"

C_EconItemView* IPlayerInventory::GetInventoryItemByItemID( const uint64_t iItemID ) 
{
	auto IntentoryItemByItemIDFn = reinterpret_cast< C_EconItemView * ( __thiscall* )( void*, uint64_t ) >( SDK::EngineData::m_AddressList[ CheatAddressList::InventoryItemByItemID ] );
	return IntentoryItemByItemIDFn( this, iItemID );
}

bool IPlayerInventory::AddEconItem( C_EconItem* pItem, bool bUpdateAckFile, bool bWriteAckFile, bool bCheckForNewItems )
{
	auto AddEconItemFn = reinterpret_cast< bool( __thiscall* )( void*, C_EconItem*, bool, bool, bool ) >( SDK::EngineData::m_AddressList[ CheatAddressList::AddEconItem ] );
	return AddEconItemFn( this, pItem, bUpdateAckFile, bWriteAckFile, bCheckForNewItems );
}
