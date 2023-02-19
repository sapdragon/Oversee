#pragma once

class C_BaseViewmodel : public C_BaseEntity
{
public:
	VFunc( SendViewmodelMatchingSequence( int nSequence ), 246, void( __thiscall* )( void*, int ), nSequence );
	NETVAR( m_iSequence, int, "DT_BaseViewModel", "m_nSequence" );
	NETVAR( m_iAnimationParity, int, "DT_BaseViewModel", "m_nAnimationParity" );

	DATAMAP( float, m_flCycle );
	DATAMAP( float, m_flAnimTime );

	NETVAR( m_hWeapon, CHandle< C_BaseCombatWeapon >, "DT_BaseViewModel", "m_hWeapon" );
};