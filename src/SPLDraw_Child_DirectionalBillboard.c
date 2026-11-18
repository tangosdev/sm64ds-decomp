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
extern void func_0204c24c(int tileS, int tileT); /* the XY-plane quad */

/* Draw one child particle as a quad stretched along its flight (child draw
 * type 1, func_0204a5c8): SPLDraw_DirectionalBillboard, still with the parent
 * header's aspect ratio and dbbScale, but the child record's tile counts.
 * The name is pret's SPL's. */
void SPLDraw_Child_DirectionalBillboard(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    const MtxFx43 *view = mgr->mDrawMatrix;
    VecFx32 velDir;
    VecFx32 trans;
    VecFx32 dir;
    VecFx32 look;
    MtxFx33 rot;
    int mat[12];
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

        mat[0] = FX_MUL(dir.x, sx);
        mat[8] = 0x1000;
        mat[9] = trans.x;
        mat[10] = trans.y;
        mat[11] = trans.z;
        mat[3] = FX_MUL(-dir.y, sy2);
        mat[1] = FX_MUL(dir.y, sx);
        mat[6] = 0;
        mat[4] = FX_MUL(dir.x, sy2);
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
