#pragma once
#include "../Misc/IHandleEntity.hpp"
#include "../Math/Vector.hpp"

class IHandleEntity;
struct Ray_t;
struct model_t;
class CGameTrace;
typedef CGameTrace trace_t;
class IClientUnknown;
class matrix3x4_t;

class ICollideable
{
public:
    virtual IHandleEntity*      GetEntityHandle() = 0;
    virtual Vector&             OBBMins() const = 0;
    virtual Vector&             OBBMaxs() const = 0;
    virtual void                WorldSpaceTriggerBounds(Vector* pVecWorldMins, Vector* pVecWorldMaxs) const = 0;
    virtual bool                TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) = 0;
    virtual bool                TestHitboxes(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) = 0;
    virtual int                 GetCollisionModelIndex() = 0;
    virtual const model_t*      GetCollisionModel() = 0;
    virtual const Vector&       GetCollisionOrigin() const = 0;
    virtual const QAngle&       GetCollisionAngles() const = 0;
    virtual const matrix3x4_t&  CollisionToWorldTransform() const = 0;
    virtual int             GetSolid() const = 0;
    virtual int                 GetSolidFlags() const = 0;
    virtual IClientUnknown*     GetIClientUnknown() = 0;
    virtual int                 GetCollisionGroup() const = 0;
    virtual void                WorldSpaceSurroundingBounds(Vector* pVecMins, Vector* pVecMaxs) = 0;
    virtual bool                ShouldTouchTrigger(int triggerSolidFlags) const = 0;
    virtual const matrix3x4_t*  GetRootParentToWorldTransform() const = 0;
};

class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class C_BaseEntity;
class IClientThinkable;
class IClientAlphaProperty;

class IClientUnknown : public IHandleEntity
{
public:
    virtual ICollideable*              GetCollideable() = 0;
    virtual IClientNetworkable*        GetClientNetworkable() = 0;
    virtual IClientRenderable*         GetClientRenderable() = 0;
    virtual IClientEntity*             GetClientEntity() = 0;
    virtual C_BaseEntity*              GetBaseEntity() = 0;
    virtual IClientThinkable*          GetClientThinkable() = 0;
    virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
};