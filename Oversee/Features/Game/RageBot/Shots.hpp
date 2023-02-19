#pragma once
#include "../../../SDK/SDK.hpp"
#include "RageBot.hpp"
#include "LocalAnimations.hpp"

enum class ShotStatus : int
{
	NoStatus,
	Occlusion,
	Spread,
	Resolver,
	LagCompensation
};
struct RageShot_t
{
	bool m_bRegisteredShot = false;
	bool m_bDidHurtPlayer = false;

	float m_flWeaponDamage = 0.0f;
	float m_flServerDamage = 0.0f;

	int m_nEstimate = 0;
	int m_nTickCount = 0;
	int m_nTickBase = 0;
	int m_nTrackedTicks = 0;

	std::string m_szServerHitbox = "";

	AimTarget_t m_Target;
	Vector m_vecShootPosition;
	std::vector < Vector > m_ImpactList;
};
class C_ShotSystem
{
public:
	virtual void OnBulletImpact( IGameEvent* m_GameEvent );
	virtual void OnPlayerHurt( IGameEvent* m_GameEvent );
	virtual void OnFrameStageNotify( ClientFrameStage_t Stage );

	/* shots */
	std::deque < RageShot_t > m_RageShots;
};

inline C_ShotSystem* g_ShotSystem = new C_ShotSystem( );