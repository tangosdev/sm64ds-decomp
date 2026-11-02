#include "types.h"
#include "nitro/fx/fx.h"
#include "private/spl_particle_views.h"

#define reg_G3_COLOR        (*(volatile u32 *)0x04000480)
#define reg_G3_POLYGON_ATTR (*(volatile u32 *)0x040004a4)

extern s16 data_02082214[];                       /* the sin/cos table */
extern void (*data_02099fbc[])(int, int, void *); /* func_0204c0a8, func_0204c0e8 */
extern void (*data_02099fb4[])(u8, u8);           /* func_0204c24c, func_0204c194 */

extern void func_020527e8(int *m, int sx, int sy, int sz); /* scale matrix */
extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
extern void func_020553a4(void *m); /* load the 4x3 matrix */
extern void func_02055388(void *m); /* multiply by the 4x3 matrix */

/* Draw one child particle as a textured quad (child draw type 2,
 * func_0204a5c8): func_0204b244 with the child record's rotation axis, plane
 * and tile counts, and the rotation applied before the scale. pret's SPL calls
 * this SPLDraw_Child_Polygon. */
void func_0204b028(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    int out[12];
    int rot[12];
    int scl[12];
    u32 id;
    u32 v;

    /* polygonIDs.current, read through the whole word: the field read does not match */
    v = *(u32 *)&sys->polygonIDs << 14;
    id = v >> 26;
    v = mgr->mPolygonAttr | 0xc0;
    v |= id << 24;
    v |= alpha << 16;
    reg_G3_POLYGON_ATTR = v;
    reg_G3_POLYGON_ATTR;
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
        data_02099fbc[child->flags.polygonRotAxis](data_02082214[idx * 2],
                                                   data_02082214[idx * 2 + 1], rot);
    }

    {
        PtclDef *def = mgr->mCurrentSystem->mDefinition;
        int sy = FX_MUL(ptcl->scale, ptcl->animScale);
        s16 aspectRatio = def->data->aspectRatio;
        int sx = FX_MUL(sy, aspectRatio);
        func_020527e8(scl, sx, sy, sy);
    }

    MulMat4x3Mat4x3(rot, scl, out);

    out[9] = ptcl->offset.x + ptcl->basePosition.x;
    out[10] = ptcl->offset.y + ptcl->basePosition.y;
    out[11] = ptcl->offset.z + ptcl->basePosition.z;

    func_020553a4(mgr->mDrawMatrix);
    func_02055388(out);

    reg_G3_COLOR = ptcl->color;
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        data_02099fb4[child->flags.polygonReferencePlane](child->textureTileCountS,
                                                          child->textureTileCountT);
    }
}
