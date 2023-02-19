#pragma once

class C_AnimationLayer
{
public:
	void*			m_pThis;			 //0x0000
	float			m_flAnimationTime;	 //0x0004
	float			m_flFadeoutTime;	 //0x0008
	int				m_nFlags;			 //0x001C
	int				m_iActivity;		 //0x0010
	int				m_nOrder;			 //0x0014
	int				m_nSequence;		 //0x0018
	float			m_flPrevCycle;		 //0x001C
	float			m_flWeight;			 //0x0020
	float			m_flWeightDeltaRate; //0x0024
	float			m_flPlaybackRate;	 //0x0028
	float			m_flCycle;			 //0x002C
	void*			m_pOwner;			 //0x0030
	int				m_nBitFlags;		 //0x0034
};