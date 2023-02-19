#pragma once
#include <winsmcrd.h>
#include "../Math/Vector.hpp"
class CParticleCollection;
class C_InitRandomColor {
	BYTE pad_0[92];
public:
	Vector	m_flNormColorMin;
	Vector	m_flNormColorMax;
};
