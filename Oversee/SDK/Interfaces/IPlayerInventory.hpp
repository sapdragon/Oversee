#pragma once
#include <cmath>
#include "../structs/IEconItemView.hpp"
#include "../structs/ISharedObject.hpp"

struct WeaponDropInfo_t {
	int32_t m_ItemID = 0;
	int32_t m_nPaintKit = 0;
	int32_t m_nRarity;
	float_t m_flWear = 0.f;

	struct StatTrack_t {
		bool m_bEnabled = false;
		int32_t m_iCounter = 0;
	} StatTrack;

};

struct Soid_t {
	uint64_t m_nID;
	uint32_t m_nType;
	uint32_t padding;
};

class IPlayerInventory {
public:

	VFunc( SoUpdated( Soid_t owner, C_SharedObject* pObject, int32_t event ), 1, void( __thiscall* )( void*, Soid_t, C_SharedObject*, int32_t ), owner, pObject, event );
	VFunc( SoDestroyed( Soid_t owner, C_SharedObject* pObject, int32_t event ), 2, void( __thiscall* )( void*, Soid_t, C_SharedObject*, int32_t ), owner, pObject, event );


	VFunc( GetItemInLoadout( int32_t iTeam, int32_t iSlot ), 8, C_EconItemView* ( __thiscall* )( void*, int32_t, int32_t ), iTeam, iSlot );


	C_EconItemView* GetInventoryItemByItemID( const uint64_t m_nItemID );

	C_ClientSharedObjectCache* GetSOC( ) {
		return *reinterpret_cast< C_ClientSharedObjectCache** >( std::uintptr_t( this ) + 0x90 );
	}

	C_SharedObjectTypeCache* GetItemBaseTypeCache( ) {
		const auto pSoc = GetSOC( );

		if ( !pSoc )
			return nullptr;

		return pSoc->FindBaseTypeCache( 1 );
	}

	void RemoveItem( C_EconItem* pItem ) {
		SoDestroyed( GetOwnerID( ), ( C_SharedObject* ) pItem, 4 );
		GetItemBaseTypeCache( )->RemoveObject( pItem );
	}

	bool AddEconItem( C_EconItem* pItem, bool bUpdateAckFile, bool bWriteAckFile, bool bCheckForNewItems );

	mGetOffset( Soid_t, GetOwnerID, 0x8 );

	uint64_t GetLastValueID( ) {
		uint64_t iMaxID = 1;
		for ( auto&& i : GetItemBaseTypeCache( )->GetEconItems( ) ) {
			auto bIsDefaultItem = ( uint32_t ) ( i->m_nItemID >> 32 ) == 0xF0000000;
			if ( !bIsDefaultItem )
				if ( i->m_nItemID > iMaxID )
					iMaxID = i->m_nItemID;
		}
		return iMaxID;
	}


	CUtlVector<C_EconItemView*>* GetInventoryItems( ) {
		return reinterpret_cast< CUtlVector<C_EconItemView*>* >( std::uintptr_t( this ) + 0x2C );
	}

	C_EconItemView* FindKeyToOpen( C_EconItemView* pCreate ) {
		for ( int32_t i = 0; i < this->GetInventoryItems( )->Count( ); i++ ) {
			auto pPropKey = *( this->GetInventoryItems( )->m_Memory.OffsetBufferByIndex( i ) );
			if ( pPropKey ) {
				if ( pPropKey->ToolCanApplyTo( pCreate ) )
					return pPropKey;
			}
		}

		return nullptr;
	}
};