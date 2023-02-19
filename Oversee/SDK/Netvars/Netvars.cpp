#include "Netvars.hpp"
#include "../SDK.hpp"

void NetVars::Initialize( )
{
	for ( auto pClass = SDK::Interfaces::CHLClient->GetAllClasses( ); pClass != nullptr; pClass = pClass->m_pNext )
	{
		if ( !pClass || !pClass->m_pRecvTable )
			continue;

		m_NetVars.emplace_back( ScanTable( pClass->m_pRecvTable ) );
	}
}
NetVar_t NetVars::ScanTable( RecvTable* pTable )
{
	NetVar_t aNetVars;
	aNetVars.m_uOffset = 0;
	aNetVars.m_strName = pTable->m_pNetTableName;

	for ( int i = 0; i < pTable->m_nProps; i++ )
	{
		auto Prop = &pTable->m_pProps[ i ];
		if ( !Prop || isdigit( Prop->m_pVarName[ 0 ] ) )
			continue;

		if ( strstr( _S( "baseclass" ), Prop->m_pVarName ) )
			continue;

		if ( Prop->m_RecvType != DPT_DataTable 
			|| 
			!Prop->m_pDataTable ) 
		{
			aNetVars.m_aChildProps.emplace_back( Prop );
			continue;
		}

		aNetVars.m_aChildTables.emplace_back( ScanTable( Prop->m_pDataTable ) );
		aNetVars.m_aChildTables.back( ).m_uOffset = Prop->m_Offset;
		aNetVars.m_aChildTables.back( ).m_pProp = Prop;
	}

	return aNetVars;
}
RecvProp* NetVars::GetNetvarProp( std::string szTableName, std::string szPropName )
{
	for ( const auto& m_Table : m_NetVars )
		if ( m_Table.m_strName == szTableName )
			return GetNetvarProp( m_Table, szPropName );
	
	return nullptr;
}
RecvProp* NetVars::GetNetvarProp( const NetVar_t& pTable, std::string szPropName )
{
	for ( const auto& prop : pTable.m_aChildProps )
		if ( strncmp( prop->m_pVarName, szPropName.data( ), szPropName.size( ) ) == 0 )
			return prop;
	
	for ( const auto& child : pTable.m_aChildTables )
	{
		auto prop = GetNetvarProp( child, szPropName );
		if ( prop != 0 )
			return prop;
	}

	for ( const auto& child : pTable.m_aChildTables )
		if ( strncmp( child.m_pProp->m_pVarName, szPropName.data( ), szPropName.size( ) ) == 0 )
			return child.m_pProp;
	
	return nullptr;
}
uint32_t NetVars::GetPropOffset( std::string szTableName, std::string szPropName )
{
	for ( auto& Table : m_NetVars )
	{
		if ( Table.m_strName != szTableName )
			continue;

		return GetOffset( Table, szPropName );
	}

	return 0;
}
uint32_t NetVars::GetOffset( const NetVar_t& pTable, std::string szPropName )
{
	for ( auto& Prop : pTable.m_aChildProps )
	{
		if ( Prop->m_pVarName != szPropName )
			continue;

		return pTable.m_uOffset + Prop->m_Offset;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		uint32_t uOffset = GetOffset( Child, szPropName );
		if ( !uOffset )
			continue;

		return pTable.m_uOffset + uOffset;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		if ( Child.m_pProp->m_pVarName != szPropName )
			continue;

		return pTable.m_uOffset + Child.m_pProp->m_Offset;
	}

	return 0;
}