#include "common.h"
#include "nitro/fx/fx.h"
#include "private/spl_particle_views.h"

#define reg_G3_COLOR        (*(volatile u32 *)0x04000480)
#define reg_G3_IDENTITY     (*(volatile u32 *)0x04000454)
#define reg_G3_POLYGON_ATTR (*(volatile u32 *)0x040004a4)

extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void func_02055388(int *m);             /* multiply by the 4x3 matrix */
extern void func_0204c24c(u8 tileS, u8 tileT); /* the XY-plane quad */
extern s16 data_02082214[];                    /* the sin/cos table */

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

    sy = FX_MUL(ptcl->scale, ptcl->animScale);
    trans.x = ptcl->offset.x + ptcl->basePosition.x;
    trans.y = ptcl->offset.y + ptcl->basePosition.y;
    trans.z = ptcl->offset.z + ptcl->basePosition.z;
    MulVec3Mat4x3(&trans, (Matrix4x3 *)view, &trans);

    {
        int idx = ptcl->angle >> 4;
        int cos;
        int sin;

        sx = FX_MUL(sy, aspectRatio);
        cos = data_02082214[idx * 2 + 1];
        sin = data_02082214[idx * 2];

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
        reg_G3_IDENTITY = 0;
        func_02055388(mat);
    }

    reg_G3_COLOR = ptcl->color;
    {
        PtclChild *child = mgr->mCurrentSystem->mDefinition->child;
        func_0204c24c(child->textureTileCountS, child->textureTileCountT);
    }
}
