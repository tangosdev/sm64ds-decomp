#include "common.h"
#include "nitro/fx/fx.h"
#include "math/Matrix.h"
#include "math/MtxStack.h"
#include "private/spl_particle_views.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/mi/memory.h"

/* No header declares these. */
extern void VEC_CrossProduct(const Vector3 *a, const Vector3 *b, Vector3 *out);
struct Vector3;
extern void VEC_Normalize(const struct Vector3 *v, struct Vector3 *out);
extern void MTX_MultVec33(const Vector3 *in, const Matrix3x3 *m, Vector3 *out);
extern void G3_MultMtx43(int *m);                /* multiply by the 4x3 matrix */
extern void func_0204c24c(int tileS, int tileT); /* the XY-plane quad */

/* Draw one particle as a quad stretched along its flight (draw type 1,
 * func_0204a5c8): alpha and the next polygon ID into POLYGON_ATTR (nothing
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
    int *view = mgr->mDrawMatrix;
    Vector3 velDir;
    Vector3 trans;
    Vector3 dir;
    Vector3 look;
    Matrix3x3 rot;
    int mat[12];
    u32 id;
    u32 v;
    int sx;
    s64 sy; /* s64, not int: int does not match */

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
    sx = FX_MUL((int)sy, aspectRatio);

    /* the quad's x axis: velocity x the view's z column, in view space */
    dir = ptcl->velocity;
    look.x = view[2];
    look.y = view[5];
    look.z = view[8];
    VEC_CrossProduct(&dir, &look, &dir);

    if (dir.x == 0 && dir.y == 0 && dir.z == 0)
        return;

    VEC_Normalize(&dir, &dir);
    MI_Copy36B(view, &rot);
    MTX_MultVec33(&dir, &rot, &dir);
    MTX_MultVec43(&trans, (Matrix4x3 *)view, &trans);
    velDir = ptcl->velocity;
    VEC_Normalize(&velDir, &velDir);

    {
        /* |velocity . -view z|: 1 when the particle flies straight at the
         * camera, 0 across it; y stretches by (1 - that) * dbbScale + 1 */
        int dot =
            FX_MUL(velDir.x, -view[2]) + FX_MUL(velDir.y, -view[5]) + FX_MUL(velDir.z, -view[8]);
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
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        /* Keep the retail u8 narrowing; passing these bitfields as int changes MWCC output. */
        func_0204c24c((u8)data->textureTileCountS, (u8)data->textureTileCountT);
    }
}
