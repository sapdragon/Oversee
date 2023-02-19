#pragma once

class C_CSWeaponData
{
public:
	void* pVTable;                       // 0x0000
	char m_strConsoleName;                   // 0x0004
	char pad_0008[ 12 ];                   // 0x0008
	int m_iMaxClip1;                       // 0x0014
	int m_iMaxClip2;                       // 0x0018
	int m_iDefaultClip1;                   // 0x001C
	int m_iDefaultClip2;                   // 0x0020
	char pad_0024[ 8 ];                    // 0x0024
	char* m_szWorldModel;                  // 0x002C
	char* m_szViewModel;                   // 0x0030
	char* m_szDroppedMode;                 // 0x0034
	char pad_0038[ 4 ];                    // 0x0038
	char* m_szShotSound;                   // 0x003C
	char pad_0040[ 56 ];                   // 0x0040
	char* m_szEmptySound;                  // 0x0078
	char pad_007C[ 4 ];                    // 0x007C
	char* m_szBulletType;                  // 0x0080
	char pad_0084[ 4 ];                    // 0x0084
	char* m_szHudName;                     // 0x0088
	char* m_szWeaponName;                  // 0x008C
	char pad_0090[ 56 ];                   // 0x0090
	int m_iWeaponType;            // 0x00C8
	char pad_00CC[ 4 ];                    // 0x00CC
	int m_iWeaponPrice;                    // 0x00D0
	int m_iKillAward;                      // 0x00D4
	char* m_szAnimationPrefix;             // 0x00D8
	float m_flCycleTime;                   // 0x00DC
	float m_flCycleTimeAlt;                // 0x00E0
	float m_flTimeToIdle;                  // 0x00E4
	float m_flIdleInterval;                // 0x00E8
	bool m_bFullAuto;                      // 0x00EC
	char pad_00ED[ 3 ];                    // 0x00ED
	int m_iDamage;                         // 0x00F0
	int m_Pad;
	float m_flArmorRatio;                  // 0x00F4
	int m_iBullets;                        // 0x00F8
	float m_flPenetration;                 // 0x00FC
	float m_flFlinchVelocityModifierLarge; // 0x0100
	float m_flFlinchVelocityModifierSmall; // 0x0104
	float m_flRange;                       // 0x0108
	float m_flRangeModifier;               // 0x010C
	float m_flThrowVelocity;               // 0x0110
	char pad_0114[ 12 ];                   // 0x0114
	bool m_bHasSilencer;                   // 0x0120
	char pad_0121[ 3 ];                    // 0x0121
	char* m_pSilencerModel;                // 0x0124
	int m_iCrosshairMinDistance;           // 0x0128
	int m_iCrosshairDeltaDistance;         // 0x012C
	float m_flMaxPlayerSpeed;              // 0x0130
	float m_flMaxPlayerSpeedAlt;           // 0x0134
	float m_flMaxPlayerSpeedMod;           // 0x0138
	float m_flSpread;                      // 0x013C
	float m_flSpreadAlt;                   // 0x0140
	float m_flInaccuracyCrouch;            // 0x0144
	float m_flInaccuracyCrouchAlt;         // 0x0148
	float m_flInaccuracyStand;             // 0x014C
	float m_flInaccuracyStandAlt;          // 0x0150
	float m_flInaccuracyJumpInitial;       // 0x0154
	float m_flInaccuracyJump;              // 0x0158
	float m_flInaccuracyJumpAlt;           // 0x015C
	float m_flInaccuracyLand;              // 0x0160
	float m_flInaccuracyLandAlt;           // 0x0164
	float m_flInaccuracyLadder;            // 0x0168
	float m_flInaccuracyLadderAlt;         // 0x016C
	float m_flInaccuracyFire;              // 0x0170
	float m_flInaccuracyFireAlt;           // 0x0174
	float m_flInaccuracyMove;              // 0x0178
	float m_flInaccuracyMoveAlt;           // 0x017C
	float m_flInaccuracyReload;            // 0x0180
	int m_iRecoilSeed;                     // 0x0184
	float m_flRecoilAngle;                 // 0x0188
	float m_flRecoilAngleAlt;              // 0x018C
	float m_flRecoilAngleVariance;         // 0x0190
	float m_flRecoilAngleVarianceAlt;      // 0x0194
	float m_flRecoilMagnitude;             // 0x0198
	float m_flRecoilMagnitudeAlt;          // 0x019C
	float m_flRecoilMagnitudeVariance;     // 0x01A0
	float m_flRecoilMagnitudeVarianceAlt;  // 0x01A4
	float m_flRecoveryTimeCrouch;          // 0x01A8
	float m_flRecoveryTimeStand;           // 0x01AC
	float m_flRecoveryTimeCrouchFinal;     // 0x01B0
	float m_flRecoveryTimeStandFinal;      // 0x01B4
	char pad_01B8[ 40 ];                   // 0x01B8
	char* m_szWeaponClass;                 // 0x01E0
	char pad_01E4[ 8 ];                    // 0x01E4
	char* m_szEjectBrassEffect;            // 0x01EC
	char* m_szTracerEffect;                // 0x01F0
	int m_iTracerFrequency;                // 0x01F4
	int m_iTracerFrequencyAlt;             // 0x01F8
	char* m_szMuzzleFlashEffect_1stPerson; // 0x01FC
	char pad_0200[ 4 ];                    // 0x0200
	char* m_szMuzzleFlashEffect_3rdPerson; // 0x0204
	char pad_0208[ 4 ];                    // 0x0208
	char* m_szMuzzleSmokeEffect;           // 0x020C
	float m_flHeatPerShot;                 // 0x0210
	char* m_szZoomInSound;                 // 0x0214
	char* m_szZoomOutSound;                // 0x0218
	float m_flInaccuracyPitchShift;        // 0x021C
	float m_flInaccuracySoundThreshold;    // 0x0220
	float m_flBotAudibleRange;             // 0x0224
	char pad_0228[ 12 ];                   // 0x0228
	bool m_bHasBurstMode;                  // 0x0234
	bool m_bIsRevolver;                    // 0x0235
};