#pragma once
#include "LagCompensation.hpp"
#include "../../../SDK/SDK.hpp"

/* Specific obfuscation for animations */
__forceinline void BreakIDA( )
{
	volatile DWORD64 dwMythicPtr = 0;
	volatile DWORD64 dwRDX = 0;
	__asm
	{
		/* sub rsp, 40 */
		sub esp, 0x40

		/* push rax, push rbx */
		mov dword ptr[ esp + 0x2C ], eax
		mov dword ptr[ esp + 0x34 ], ebx

		/* add rsp, 0x30 */
		mov ebx, esp
		ror ebx, 200
		add ebx, 0x54F12F43
		lea eax, [ ebx - 0x54F12F43 ]
		rol eax, 200
		add eax, 0x30
		mov esp, eax

		/* push junk at volatiles */
		mov dwMythicPtr, ecx
		mov dwRDX, edx

		/* pop rax, pop rbx */
		pop eax
		pop ebx
	}

	if ( _rotl( ( ( ( dwMythicPtr - dwRDX ) ) | 0xDDDD ) | 0xEEEE, 270 ) == _rotr64( dwRDX, 270 ) )
	{
		__asm
		{
			vmovdqu32 zmm0, zmmword ptr[ esp ]
			lea esp, dword ptr[ esp - 0xFFFFFFF + edx ]
			vmovntdq zmmword ptr[ esp ], zmm0
			or ax, ax
			and al, al
			vzeroupper
		}
	}
}

struct GameGlobals_t
{
	float m_flCurTime;
	float m_flRealTime;
	float m_flFrameTime;
	float m_flAbsFrameTime;
	float m_flInterpolationAmount;
	int m_nTickCount, m_nFrameCount;
	
	void AdjustData( )
	{
		SDK::Interfaces::GlobalVars->m_flCurTime = this->m_flCurTime;
		SDK::Interfaces::GlobalVars->m_flRealTime = this->m_flRealTime;
		SDK::Interfaces::GlobalVars->m_flFrameTime = this->m_flFrameTime;
		SDK::Interfaces::GlobalVars->m_flAbsFrameTime = this->m_flAbsFrameTime;
		SDK::Interfaces::GlobalVars->m_flInterpolationAmount = this->m_flInterpolationAmount;
		SDK::Interfaces::GlobalVars->m_iTickCount = this->m_nTickCount;
		SDK::Interfaces::GlobalVars->m_iFrameCount = this->m_nFrameCount;
	}
	void CaptureData( )
	{
		this->m_flCurTime = SDK::Interfaces::GlobalVars->m_flCurTime;
		this->m_flRealTime = SDK::Interfaces::GlobalVars->m_flRealTime;
		this->m_flFrameTime = SDK::Interfaces::GlobalVars->m_flFrameTime;
		this->m_flAbsFrameTime = SDK::Interfaces::GlobalVars->m_flAbsFrameTime;
		this->m_flInterpolationAmount = SDK::Interfaces::GlobalVars->m_flInterpolationAmount;
		this->m_nTickCount = SDK::Interfaces::GlobalVars->m_iTickCount;
		this->m_nFrameCount = SDK::Interfaces::GlobalVars->m_iFrameCount;
	}
};
class C_PlayerAnimations
{
public:
	virtual void SetupPlayerMatrix( C_BasePlayer* pPlayer, LagRecord_t* m_Record, matrix3x4_t* Matrix, int nFlags );
	virtual void SimulatePlayerAnimations( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, PlayerData_t* m_PlayerData );
	virtual void SimulatePlayerActivity( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord );
	virtual float ComputeActivityPlayback( C_BasePlayer* pPlayer, LagRecord_t* m_Record );
	virtual void HandleDormancyLeaving( C_BasePlayer* pPlayer, LagRecord_t* m_Record, C_CSGOPlayerAnimationState* m_AnimationState );
	virtual void SetupCollision( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord );
	virtual void CopyRecordData( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord, C_CSGOPlayerAnimationState* m_AnimationState );
	virtual void GenerateSafePoints( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord );
	virtual void SetupBones( C_BasePlayer* pPlayer, int nBoneMask, matrix3x4_t* aMatrix );
	virtual void UpdatePlayerAnimations( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, C_CSGOPlayerAnimationState* m_AnimationState );
	virtual void InterpolateMatricies( );
	virtual void TransformateMatrix( C_BasePlayer* pPlayer, PlayerData_t* m_PlayerData );
	virtual bool CopyCachedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix, int nBoneCount );
	virtual float BuildFootYaw( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord );
	virtual int DetermineAnimationCycle( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord );
	virtual LagRecord_t* SetupData( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, PlayerData_t* m_PlayerData );
	virtual Vector DeterminePlayerVelocity( C_BasePlayer* pPlayer, LagRecord_t* m_LagRecord, LagRecord_t* m_PrevRecord, C_CSGOPlayerAnimationState* m_AnimationState );
};

inline C_PlayerAnimations* g_PlayerAnimations = new C_PlayerAnimations( );