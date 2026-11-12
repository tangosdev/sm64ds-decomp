#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/MtxStack.h"


extern void (*data_02099fbc[])(int, int, void *); /* func_0204c0a8, func_0204c0e8 */
extern void (*data_02099fb4[])(u8, u8);           /* func_0204c24c, func_0204c194 */

extern void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz); /* scale matrix */
extern void G3_LoadMtx43(void *m);                        /* load the 4x3 matrix */
extern void G3_MultMtx43(void *m);                        /* multiply by the 4x3 matrix */

/* Draw one child particle as a textured quad (child draw type 2,
 * func_0204a5c8): func_0204b244 with the child record's rotation axis, plane
 * and tile counts, and the rotation applied before the scale. pret's SPL calls
 * this SPLDraw_Child_Polygon. */
void func_0204b028(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    Matrix4x3 out;
    Matrix4x3 rot;
    Matrix4x3 scl;
    u32 id;
    u32 v;

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

    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        int idx = ptcl->angle >> 4;
        data_02099fbc[child->flags.polygonRotAxis](FX_SinIdx(idx), FX_CosIdx(idx), &rot);
    }

    {
        PtclDef *def = mgr->mCurrentSystem->mDefinition;
        int sy = FX_MUL(ptcl->scale, ptcl->animScale);
        s16 aspectRatio = def->data->aspectRatio;
        int sx = FX_MUL(sy, aspectRatio);
        MTX_Scale43_(&scl, sx, sy, sy);
    }

    MTX_Concat43(&rot, &scl, &out);

    out.m[9] = ptcl->offset.x + ptcl->basePosition.x;
    out.m[10] = ptcl->offset.y + ptcl->basePosition.y;
    out.m[11] = ptcl->offset.z + ptcl->basePosition.z;

    G3_LoadMtx43(mgr->mDrawMatrix);
    G3_MultMtx43(&out);

    G3_Color(ptcl->color);
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        data_02099fb4[child->flags.polygonReferencePlane](child->textureTileCountS,
                                                          child->textureTileCountT);
    }
}
