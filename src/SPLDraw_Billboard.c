#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/fx/fx_mtx43.h"

extern void G3_MultMtx43(int *m);                /* multiply by the 4x3 matrix */
extern void SPLUtil_DrawXYPlane(int tileS, int tileT); /* the XY-plane quad */

/* Draw one particle as a camera-facing quad (draw type 0, func_0204a5c8):
 * alpha and the next polygon ID into POLYGON_ATTR (nothing more when the
 * alpha is 0), move basePosition + offset into view space, then load a matrix
 * that rotates by the particle's angle about the view axis, scales by
 * scale * animScale (x by aspectRatio too) and translates there, set its
 * color and emit the XY-plane quad. The name is pret's SPL's;
 * SPLDraw_Child_Billboard is the child-particle twin. */
void SPLDraw_Billboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 trans;
    MtxFx43 mat;
    u32 v;
    int sx;
    int sy;

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
    MTX_MultVec43(&trans, view, &trans);

    {
        int idx = ptcl->angle >> 4;
        int cos;
        int sin;

        sx = FX_MUL(sy, aspectRatio);
        cos = FX_CosIdx(idx);
        sin = FX_SinIdx(idx);

        /* the 4x3 rows: (cos, sin, 0) * sx, (-sin, cos, 0) * sy, (0, 0, 1),
         * then the view-space position */
        mat.m[0][0] = FX_MUL(cos, sx);
        mat.m[1][0] = FX_MUL(-sin, sy);
        mat.m[0][1] = FX_MUL(sin, sx);
        mat.m[3][0] = trans.x;
        mat.m[3][1] = trans.y;
        mat.m[2][2] = 0x1000;
        mat.m[3][2] = trans.z;
        mat.m[2][0] = 0;
        mat.m[1][1] = FX_MUL(cos, sy);
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
