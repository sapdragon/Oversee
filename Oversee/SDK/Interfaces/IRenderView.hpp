#pragma once

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEngineSprite;
class IClientEntity;
class C_Material;
struct model_t;
class IClientRenderable;
class ITexture;

enum RenderGroup_t
{
    RENDER_GROUP_OPAQUE_ENTITY = 0,
    RENDER_GROUP_TRANSLUCENT_ENTITY,
    RENDER_GROUP_TWOPASS, // Implied opaque and translucent in two passes
    RENDER_GROUP_VIEW_MODEL_OPAQUE, // solid weapon view models
    RENDER_GROUP_VIEW_MODEL_TRANSLUCENT, // transparent overlays etc
    RENDER_GROUP_OTHER,	// Unclassfied. Won't get drawn.

    RENDER_GROUP_COUNT_VERSION_1,

    RENDER_GROUP_OPAQUE_STATIC,
    RENDER_GROUP_OPAQUE_BRUSH,

    // This one's always gotta be last
    RENDER_GROUP_COUNT
};

class C_ViewSetup
{
public:
    int			iX;
    int			iUnscaledX;
    int			iY;
    int			iUnscaledY;
    int			iWidth;
    int			iUnscaledWidth;
    int			iHeight;
    int			iUnscaledHeight;
    bool		bOrtho;
    BYTE    	pad0[ 0x8F ];
    float		flFOV;
    float		flViewModelFOV;
    Vector		vecOrigin;
    QAngle		angView;
    float		flNearZ;
    float		flFarZ;
    float		flNearViewmodelZ;
    float		flFarViewmodelZ;
    float		flAspectRatio;
    float		flNearBlurDepth;
    float		flNearFocusDepth;
    float		flFarFocusDepth;
    float		flFarBlurDepth;
    float		flNearBlurRadius;
    float		flFarBlurRadius;
    float		flDoFQuality;
    int			nMotionBlurMode;
    float		flShutterTime;
    Vector		vecShutterOpenPosition;
    QAngle		vecShutterOpenAngles;
    Vector		vecShutterClosePosition;
    QAngle		vecShutterCloseAngles;
    float		flOffCenterTop;
    float		flOffCenterBottom;
    float		flOffCenterLeft;
    float		flOffCenterRight;
    bool		bOffCenter;
    bool		bRenderToSubrectOfLargerScreen;
    bool		bDoBloomAndToneMapping;
    bool		bDoDepthOfField;
    bool		bHDRTarget;
    bool		bDrawWorldNormal;
    bool		bCullFontFaces;
    bool		bCacheFullSceneState;
    bool		bCSMView;
};

//-----------------------------------------------------------------------------
// Flags used by DrawWorldLists
//-----------------------------------------------------------------------------
enum
{
    DRAWWORLDLISTS_DRAW_STRICTLYABOVEWATER = 0x001,
    DRAWWORLDLISTS_DRAW_STRICTLYUNDERWATER = 0x002,
    DRAWWORLDLISTS_DRAW_INTERSECTSWATER = 0x004,
    DRAWWORLDLISTS_DRAW_WATERSURFACE = 0x008,
    DRAWWORLDLISTS_DRAW_SKYBOX = 0x010,
    DRAWWORLDLISTS_DRAW_CLIPSKYBOX = 0x020,
    DRAWWORLDLISTS_DRAW_SHADOWDEPTH = 0x040,
    DRAWWORLDLISTS_DRAW_REFRACTION = 0x080,
    DRAWWORLDLISTS_DRAW_REFLECTION = 0x100,
    DRAWWORLDLISTS_DRAW_WORLD_GEOMETRY = 0x200,
    DRAWWORLDLISTS_DRAW_DECALS_AND_OVERLAYS = 0x400,
};

enum
{
    MAT_SORT_GROUP_STRICTLY_ABOVEWATER = 0,
    MAT_SORT_GROUP_STRICTLY_UNDERWATER,
    MAT_SORT_GROUP_INTERSECTS_WATER_SURFACE,
    MAT_SORT_GROUP_WATERSURFACE,

    MAX_MAT_SORT_GROUPS
};

//-----------------------------------------------------------------------------
// Leaf index
//-----------------------------------------------------------------------------
typedef unsigned short LeafIndex_t;
enum
{
    INVALID_LEAF_INDEX = (LeafIndex_t)~0
};

struct WorldListLeafData_t
{
    LeafIndex_t     leafIndex;    // 16 bits
    int16_t         waterData;
    uint16_t        firstTranslucentSurface;    // engine-internal list index
    uint16_t        translucentSurfaceCount;    // count of translucent surfaces+disps
};

struct WorldListInfo_t
{
    int                     m_ViewFogVolume;
    int                     m_LeafCount;
    bool                    m_bHasWater;
    WorldListLeafData_t*    m_pLeafDataList;
};

class IWorldRenderList /*: public IRefCounted*/
{
};

//-----------------------------------------------------------------------------
// Describes the fog volume for a particular point
//-----------------------------------------------------------------------------
struct VisibleFogVolumeInfo_t
{
    int            m_nVisibleFogVolume;
    int            m_nVisibleFogVolumeLeaf;
    bool        m_bEyeInFogVolume;
    float       m_flDistanceToWater;
    float       m_flWaterHeight;
    C_Material*  m_pFogVolumeMaterial;
};

struct VPlane
{
    Vector        m_Normal;
    vec_t        m_Dist;
};
#define FRUSTUM_NUMPLANES    6
typedef VPlane Frustum[FRUSTUM_NUMPLANES];
//-----------------------------------------------------------------------------
// Vertex format for brush models
//-----------------------------------------------------------------------------
struct BrushVertex_t //-V690
{
    Vector        m_Pos;
    Vector        m_Normal;
    Vector        m_TangentS;
    Vector        m_TangentT;
    Vector2D    m_TexCoord;
    Vector2D    m_LightmapCoord;

private:
    BrushVertex_t(const BrushVertex_t& src);
};

//-----------------------------------------------------------------------------
// Visibility data for area portal culling
//-----------------------------------------------------------------------------
struct VisOverrideData_t
{
    Vector        m_vecVisOrigin;                    // The point to to use as the viewpoint for area portal backface cull checks.
    float        m_fDistToAreaPortalTolerance;    // The distance from an area portal before using the full screen as the viewable portion.
};


//-----------------------------------------------------------------------------
// interface for asking about the Brush surfaces from the client DLL
//-----------------------------------------------------------------------------

class C_BrushSurface
{
public:
    // Computes texture coordinates + lightmap coordinates given a world position
    virtual void ComputeTextureCoordinate(Vector const& worldPos, Vector2D& texCoord) = 0;
    virtual void ComputeLightmapCoordinate(Vector const& worldPos, Vector2D& lightmapCoord) = 0;

    // Gets the vertex data for this surface
    virtual int  GetVertexCount() const = 0;
    virtual void GetVertexData(BrushVertex_t* pVerts) = 0;

    // Gets at the material properties for this surface
    virtual C_Material* GetMaterial() = 0;
};


//-----------------------------------------------------------------------------
// interface for installing a new renderer for brush surfaces
//-----------------------------------------------------------------------------

class C_BrushRenderer
{
public:
    // Draws the surface; returns true if decals should be rendered on this surface
    virtual bool RenderBrushModelSurface(IClientEntity* pBaseEntity, C_BrushSurface* pBrushSurface) = 0;
};

#define MAX_VIS_LEAVES    32
#define MAX_AREA_STATE_BYTES        32
#define MAX_AREA_PORTAL_STATE_BYTES 24

class ColorVec_t;
class ViewMatrix_t;
class IMatRenderContext;
class IVRenderView
{
public:
    virtual void      DrawBrushModel(IClientEntity* pBaseEntity, model_t* pModel, const Vector& vecOrigin, const QAngle& angView, bool bSort) = 0;
    virtual void      DrawIdentityBrushModel(IWorldRenderList* pList, model_t* pModel) = 0;
    virtual void      TouchLight(struct dlight_t* dLight) = 0;
    virtual void      Draw3DDebugOverlays() = 0;
    virtual void      SetBlend(float flBlend) = 0;
    virtual float      GetBlend() = 0;
    virtual void      SetColorModulation(float const* flBlend) = 0;
    virtual void      GetColorModulation(float* flBlend) = 0;
    virtual void      SceneBegin() = 0;
    virtual void      SceneEnd() = 0;
    virtual void      GetVisibleFogVolume(const Vector& vecEyePoint, VisibleFogVolumeInfo_t* pInfo) = 0;
    virtual IWorldRenderList* CreateWorldList() = 0;
    virtual void      BuildWorldLists_Epilogue(IWorldRenderList* pList, WorldListInfo_t* pInfo, bool bShadowDepth) = 0;
    virtual void      BuildWorldLists(IWorldRenderList* pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = nullptr, bool bShadowDepth = false, float* pReflectionWaterHeight = nullptr) = 0;
    virtual void      DrawWorldLists(IWorldRenderList* pList, unsigned long flags, float flWaterZAdjust) = 0;
    virtual void      GetWorldListIndicesInfo(void* pIndicesInfoOut, IWorldRenderList* pList, unsigned long nFlags) = 0;
    virtual void      DrawTopView(bool bEnable) = 0;
    virtual void      TopViewNoBackfaceCulling(bool bDisable) = 0;
    virtual void      TopViewNoVisCheck(bool bDisable) = 0;
    virtual void      TopViewBounds(Vector2D const& vecMins, Vector2D const& vecMaxs) = 0;
    virtual void      SetTopViewVolumeCuller(const void* pVolumeCuller) = 0;
    virtual void      DrawLights() = 0;
    virtual void      DrawMaskEntities() = 0;
    virtual void      DrawTranslucentSurfaces(IWorldRenderList* pList, int* pSortList, int iSortCount, unsigned long fFlags) = 0;
    virtual void      DrawLineFile() = 0;
    virtual void      DrawLightmaps(IWorldRenderList* pList, int iPageID) = 0;
    virtual void      ViewSetupVis(bool bNoVis, int nOrigins, const Vector vecOrigin[]) = 0;
    virtual bool      AreAnyLeavesVisible(int* pLeafList, int nLeaves) = 0;
    virtual  void      VguiPaint() = 0;
    virtual void      ViewDrawFade(uint8_t* pColor, C_Material* pMaterial) = 0;
    virtual void      OLD_SetProjectionMatrix(float flFov, float zNear, float zFar) = 0;
    virtual ColorVec_t    GetLightAtPoint(Vector& vecPosition) = 0;
    virtual int        GetViewEntity() = 0;
    virtual bool      IsViewEntity(int nEntityIndex) = 0;
    virtual float      GetFieldOfView() = 0;
    virtual unsigned char** GetAreaBits() = 0;
    virtual void      SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;
    virtual void      InstallBrushSurfaceRenderer(C_BrushRenderer* pBrushRenderer) = 0;
    virtual void      DrawBrushModelShadow(IClientRenderable* pRenderable) = 0;
    virtual  bool      LeafContainsTranslucentSurfaces(IWorldRenderList* pList, int nSortIndex, unsigned long fFlags) = 0;
    virtual bool      DoesBoxIntersectWaterVolume(const Vector& vecMins, const Vector& vecMaxs, int nLeafWaterDataID) = 0;
    virtual void      SetAreaState(unsigned char uAreaBits[ MAX_AREA_STATE_BYTES ], unsigned char uAreaPortalBits[ MAX_AREA_PORTAL_STATE_BYTES ]) = 0;
    virtual void      VGui_Paint(int nMode) = 0;
    virtual void      Push3DView(IMatRenderContext* pRenderContext, const C_ViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
    virtual void      Push2DView(IMatRenderContext* pRenderContext, const C_ViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
    virtual void      PopView(IMatRenderContext* pRenderContext, Frustum frustumPlanes) = 0;
    virtual void      SetMainView(const Vector& vecOrigin, const QAngle& angView) = 0;
    virtual void      ViewSetupVisEx(bool bNoVis, int nOrigins, const Vector arrOrigin[], unsigned int& uReturnFlags) = 0;
    virtual void      OverrideViewFrustum(Frustum custom) = 0;
    virtual void      DrawBrushModelShadowDepth(IClientEntity* pEntity, model_t* pModel, const Vector& vecOrigin, const QAngle& angView, int nDepthMode) = 0;
    virtual void      UpdateBrushModelLightmap(model_t* pModel, IClientRenderable* pRenderable) = 0;
    virtual void      BeginUpdateLightmaps() = 0;
    virtual void      EndUpdateLightmaps() = 0;
    virtual void      OLD_SetOffCenterProjectionMatrix(float flFOV, float flNearZ, float flFarZ, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
    virtual void      OLD_SetProjectionMatrixOrtho(float flLeft, float flTop, float flRight, float flBottom, float flNearZ, float flFarZ) = 0;
    virtual void      Push3DView(IMatRenderContext* pRenderContext, const C_ViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture) = 0;
    virtual void      GetMatricesForView(const C_ViewSetup& view, ViewMatrix_t* pWorldToView, ViewMatrix_t* pViewToProjection, ViewMatrix_t* pWorldToProjection, ViewMatrix_t* pWorldToPixels) = 0;
    virtual void      DrawBrushModelEx(IClientEntity* pEntity, model_t* pModel, const Vector& vecOrigin, const QAngle& angView, int nMode) = 0;
    virtual bool      DoesBrushModelNeedPowerOf2Framebuffer(const model_t* pModel) = 0;
    virtual void      DrawBrushModelArray(IMatRenderContext* pContext, int nCount, const void* pInstanceData, int nModelTypeFlags) = 0;
};