#pragma once
#include <cstddef>
#include "../Math/Vector.hpp"

struct ColorRGBExp32
{
	std::byte r, g, b;
	signed char exponent;
};

struct DLight_t
{
	int		m_nFlags;
	Vector	m_vOrigin;
	float	m_flRadius;
	ColorRGBExp32 m_aColor;
	float	m_flDie;
	float	m_flDecay;
	float	m_flMinLight;
	int		m_nKey;
	int		m_nStyle;
	Vector	m_vDirection;
	float	m_flInnerAngle;
	float	m_flOuterAngle;
	float GetRadius() const
	{
		return m_flRadius;
	}
	float GetRadiusSquared() const
	{
		return m_flRadius * m_flRadius;
	}
	float IsRadiusGreaterThanZero() const
	{
		return m_flRadius > 0.0f;
	}
};

class IVEffects
{
public:
	// Retrieve decal texture index from decal by name
	virtual	int				Draw_DecalIndexFromName(char* name) = 0;

	// Apply decal. See decal_private.h for flags (FDECAL_*)
	// If normal is specified, don't decal surfaces antiparallel with normal
	virtual	void			DecalShoot(int textureIndex, int entity,
		const model_t* model, const Vector& model_origin, const QAngle& model_angles,
		const Vector& position, const Vector* saxis, int flags, const Vector* pNormal = NULL, int nAdditionalDecalFlags = 0) = 0;

	// Apply colored decal. See decal_private.h for flags (FDECAL_*)
	virtual	void			DecalColorShoot(int textureIndex, int entity,
		const model_t* model, const Vector& model_origin, const QAngle& model_angles,
		const Vector& position, const Vector* saxis, int flags, const Color& rgbaColor, const Vector* pNormal = NULL, int nAdditionalDecalFlags = 0) = 0;

	virtual void			PlayerDecalShoot(IMaterial* material, void* userdata, int entity, const model_t* model,
		const Vector& model_origin, const QAngle& model_angles,
		const Vector& position, const Vector* saxis, int flags, const Color& rgbaColor, int nAdditionalDecalFlags) = 0;

	// Allocate a dynamic world light ( key is the entity to whom it is associated )
	virtual	DLight_t* CL_AllocDlight(int key) = 0;

	// Allocate a dynamic entity light ( key is the entity to whom it is associated )
	virtual	DLight_t* CL_AllocElight(int key) = 0;

	// Get a list of the currently-active dynamic lights.
	virtual int CL_GetActiveDLights(DLight_t* pList[24]) = 0;

	// Retrieve decal texture name from decal by index
	virtual	const char* Draw_DecalNameFromIndex(int nIndex) = 0;

	// Given an elight key, find it. Does not search ordinary dlights. May return NULL.
	virtual DLight_t* GetElightByKey(int key) = 0;
};