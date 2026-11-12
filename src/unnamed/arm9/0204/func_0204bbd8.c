#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/MtxStack.h"

extern void G3_MultMtx43(int *m);                /* multiply by the 4x3 matrix */
extern void func_0204c24c(int tileS, int tileT); /* the XY-plane quad */

/* Draw one child particle as a camera-facing quad (child draw type 0,
 * func_0204a5c8): func_0204be40 with the child record's tile counts. pret's
 * SPL calls this SPLDraw_Child_Billboard. */
void func_0204bbd8(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    int *view = mgr->mDrawMatrix;
    Vector3 trans;
    int mat[12];
    u32 id;
    u32 v;
    int sx;
    int sy;

    /* polygonIDs.current, read through the whole word: the field read does not match */
    v = *(u32 *)&sys->polygonIDs << 14;
    id = v >> 26;
    v = mgr->mPolygonAttr | 0xc0;
    v |= id << 24;
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
    MTX_MultVec43(&trans, (Matrix4x3 *)view, &trans);

    {
        int idx = ptcl->angle >> 4;
        int cos;
        int sin;

        sx = FX_MUL(sy, aspectRatio);
        cos = FX_CosIdx(idx);
        sin = FX_SinIdx(idx);

        /* the 4x3 rows: (cos, sin, 0) * sx, (-sin, cos, 0) * sy, (0, 0, 1),
         * then the view-space position */
        mat[0] = FX_MUL(cos, sx);
        mat[3] = FX_MUL(-sin, sy);
        mat[1] = FX_MUL(sin, sx);
        mat[9] = trans.x;
        mat[10] = trans.y;
        mat[8] = 0x1000;
        mat[11] = trans.z;
        mat[6] = 0;
        mat[4] = FX_MUL(cos, sy);
        mat[7] = 0;
        mat[2] = 0;
        mat[5] = 0;
        G3_Identity();
        G3_MultMtx43(mat);
    }

    G3_Color(ptcl->color);
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        func_0204c24c((u8)child->textureTileCountS, (u8)child->textureTileCountT);
    }
}
