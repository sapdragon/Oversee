#pragma once
#include <cstdint>
#include "IAnimationLayer.hpp"

#define CSGO_ANIM_LOWER_CATCHUP_IDLE	100.0f
#define CSGO_ANIM_AIM_NARROW_WALK	0.8f
#define CSGO_ANIM_AIM_NARROW_RUN	0.5f
#define CSGO_ANIM_AIM_NARROW_CROUCHMOVING	0.5f
#define CSGO_ANIM_LOWER_CATCHUP_WITHIN	3.0f
#define CSGO_ANIM_LOWER_REALIGN_DELAY	1.1f
#define CSGO_ANIM_READJUST_THRESHOLD	120.0f
#define EIGHT_WAY_WIDTH 22.5f

#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f

#define CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED 2.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH 8.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH_CROUCH 16.0f
#define CSGO_ANIM_LADDER_CLIMB_COVERAGE 100.0f
#define CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER 0.85f
#define MAX_ANIMSTATE_ANIMNAME_CHARS 64

#define ANIM_TRANSITION_WALK_TO_RUN 0
#define ANIM_TRANSITION_RUN_TO_WALK 1

#define CS_PLAYER_SPEED_DUCK_MODIFIER 0.34f
#define CS_PLAYER_SPEED_WALK_MODIFIER 0.52f
#define CS_PLAYER_SPEED_CLIMB_MODIFIER 0.34f
#define CS_PLAYER_HEAVYARMOR_FLINCH_MODIFIER 0.5f
#define CS_PLAYER_SPEED_RUN 260.0f

#define MOVESTATE_IDLE	0
#define MOVESTATE_WALK	1
#define MOVESTATE_RUN	2

struct aimmatrix_transition_t
{
	void Init( void )
	{
		m_flDurationStateHasBeenValid = 0;
		m_flDurationStateHasBeenInValid = 0;
		m_flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
		m_flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
		m_flBlendValue = 0;
	}
	aimmatrix_transition_t( ) { Init( ); }

	float	m_flDurationStateHasBeenValid;
	float	m_flDurationStateHasBeenInValid;
	float	m_flHowLongToWaitUntilTransitionCanBlendIn;
	float	m_flHowLongToWaitUntilTransitionCanBlendOut;
	float	m_flBlendValue;
};
struct AnimstatePose_t
{
	bool		m_bInitialized;
	int			m_nIndex;
	const char* m_szName;

	AnimstatePose_t()
	{
		m_bInitialized = false;
		m_nIndex = -1;
		m_szName = "";
	}
};

struct procedural_foot_t
{
	Vector m_vecPosAnim;
	Vector m_vecPosAnimLast;
	Vector m_vecPosPlant;
	Vector m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};

#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX 58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN -58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX 90.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN -90.0f

class C_CSGOPlayerAnimationState
{
public:
	void*					m_pAnimlayerOrderPreset;									
	bool					m_bFirstRunSinceInit;									
	bool					m_bFirstFootPlantSinceInit;
	int						m_iLastUpdateFrame;
	int						m_flEyePositionSmoothLerp;
	int						m_flStrafeChangeWeightSmoothFalloff;
	aimmatrix_transition_t	m_tStandWalkAim;
	aimmatrix_transition_t	m_tStandRunAim;
	aimmatrix_transition_t	m_tCrouchWalkAim;
	int						m_iModelIndex;								
	int						m_iUnknownArray[ 2 ];		
	LPVOID					m_pWeaponLastBoneSetup;
	C_BasePlayer*			m_pBasePlayer;
	LPVOID					m_pWeapon;
	LPVOID					m_pWeaponLast;
	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;
	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;
	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;
	Vector					m_vecPositionCurrent;
	Vector					m_vecPositionLast;
	Vector					m_vecVelocity;
	Vector					m_vecVelocityNormalized;
	Vector					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;
	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;
	float					m_flDurationMoving;
	float					m_flDurationStill;
	bool					m_bOnGround;
	bool					m_bLanding;
	char					m_pad[2];
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;
	float					m_flWalkToRunTransition;
	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;
	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[ 20 ];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	Vector					m_vecVelocityLast;
	Vector					m_vecTargetAcceleration;
	Vector					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	AnimstatePose_t			m_tPoseParamMappings[ 20 ];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flFuckValve;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;

	void Reset( );
	int SelectSequenceFromActivityModifier( int iActivity );
	bool IsLayerSequenceFinished( C_AnimationLayer* pAnimationLayer, float flTime );
	void SetLayerSequence( C_AnimationLayer* pAnimationLayer, int iActivity );
	void SetLayerCycle( C_AnimationLayer* pAnimationLayer, float flCycle );
	void SetLayerRate( C_AnimationLayer* pAnimationLayer, float flRate );
	void SetLayerWeight( C_AnimationLayer* pAnimationLayer, float flWeight );
	void SetLayerWeightRate( C_AnimationLayer* pAnimationLayer, float flWeightRate );
	void IncrementLayerCycle( C_AnimationLayer* pAnimationLayer, bool bIsLoop );
}; // 840