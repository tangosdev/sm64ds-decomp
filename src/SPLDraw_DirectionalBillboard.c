#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_vec.h"
#include "nitro/fx/fx_mtx33.h"
#include "nitro/fx/fx_mtx43.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/mi/memory.h"

/* No header declares these. */
extern void G3_MultMtx43(int *m);                /* multiply by the 4x3 matrix */
extern void SPLUtil_DrawXYPlane(int tileS, int tileT); /* the XY-plane quad */

/* Draw one particle as a quad stretched along its flight (draw type 1,
 * SPLManager_DoDraw): alpha and the next polygon ID into POLYGON_ATTR (nothing
 * more when the alpha is 0); the quad's x axis is velocity x the view's z
 * axis turned into view space (nothing is drawn when they are parallel), y
 * at right angles to it, scaled by scale * animScale (x by aspectRatio too,
 * y stretched by dbbScale the more the particle flies across the view),
 * translated to basePosition + offset in view space; then its color and the
 * XY-plane quad. The name is pret's SPL's (pret's revision adds the
 * scale-animation axis and view-space cases);
 * SPLDraw_Child_DirectionalBillboard is the child-particle twin. */
void SPLDraw_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 velDir;
    VecFx32 trans;
    VecFx32 dir;
    VecFx32 look;
    MtxFx33 rot;
    MtxFx43 mat;
    u32 v;
    int sx;
    s64 sy; /* s64, not int: int does not match */

    v = sys->polygonIDs.current << 24;
    v |= mgr->mPolygonAttr | 0xc0;
    v |= alpha << 16;
    REG_POLYGON_ATTR = v;
    REG_POLYGON_ATTR;
    {
        PolygonIDs *ids = &sys->polygonIDs;
        ids->current++;
        if (sys->polygonIDs.current > sys->polygonIDs.max)
            ids->current = sys->polygonIDs.min;
    }

    if (alpha == 0)
        return;

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    sx = FX_MUL((int)sy, aspectRatio);

    /* the quad's x axis: velocity x the view's z column, in view space */
    dir = ptcl->velocity;
    look.x = view->m[0][2];
    look.y = view->m[1][2];
    look.z = view->m[2][2];
    VEC_CrossProduct(&dir, &look, &dir);

    if (dir.x == 0 && dir.y == 0 && dir.z == 0)
        return;

    VEC_Normalize(&dir, &dir);
    MI_Copy36B(view, &rot);
    MTX_MultVec33(&dir, &rot, &dir);
    MTX_MultVec43(&trans, view, &trans);
    velDir = ptcl->velocity;
    VEC_Normalize(&velDir, &velDir);

    {
        /* |velocity . -view z|: 1 when the particle flies straight at the
         * camera, 0 across it; y stretches by (1 - that) * dbbScale + 1 */
        int dot = FX_MUL(velDir.x, -view->m[0][2]) + FX_MUL(velDir.y, -view->m[1][2]) +
                  FX_MUL(velDir.z, -view->m[2][2]);
        int side;
        int stretch;
        int sy2;
        if (dot < 0)
            dot = -dot;
        side = 0x1000 - dot;
        stretch = FX_MUL(side, (int)mgr->mCurrentSystem->mDefinition->data->dbbScale);
        sy2 = FX_MUL(sy, (stretch + 0x1000));

        mat.m[0][0] = FX_MUL(dir.x, sx);
        mat.m[2][2] = 0x1000;
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[3][2] = trans.z;
        mat.m[1][0] = FX_MUL(-dir.y, sy2);
        mat.m[0][1] = FX_MUL(dir.y, sx);
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(dir.x, sy2);
        mat.m[2][1] = 0;
        mat.m[0][2] = 0;
        mat.m[1][2] = 0;
        G3_Identity();
        G3_MultMtx43(&mat.m[0][0]);
    }

    G3_Color(ptcl->color);
    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        SPLUtil_DrawXYPlane((u8)data->textureTileCountS, (u8)data->textureTileCountT);
    }
}
