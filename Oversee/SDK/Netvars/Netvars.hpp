#pragma once
#include <vector>
#include "../Misc/Recv.hpp"

struct NetVar_t
{   
    std::string                 m_strName;
    RecvProp*                   m_pProp;
    uint32_t                    m_uOffset;
    std::vector< RecvProp* >    m_aChildProps;
    std::vector< NetVar_t >     m_aChildTables;
};

namespace NetVars
{
	// function that will parse everything
	void Initialize( );

	// scaner
	NetVar_t ScanTable( RecvTable* pClientClass );

	// helpers
	RecvProp* GetNetvarProp( std::string szTableName, std::string szPropName );
	uint32_t GetPropOffset( std::string szTableName, std::string szPropName );

	uint32_t GetOffset( const NetVar_t& pTable, std::string szPropName );
	RecvProp* GetNetvarProp( const NetVar_t& pTable, std::string szPropName );
	
	// all netvars list
	inline std::vector < NetVar_t > m_NetVars;
};