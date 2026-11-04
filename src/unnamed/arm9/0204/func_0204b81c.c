#include "common.h"
#include "nitro/fx/fx.h"
#include "private/spl_particle_views.h"

#define reg_G3_COLOR        (*(volatile u32 *)0x04000480)
#define reg_G3_IDENTITY     (*(volatile u32 *)0x04000454)
#define reg_G3_POLYGON_ATTR (*(volatile u32 *)0x040004a4)

/* No header declares these. CrossVec3 uses its Vec3_Fix12 view,
   NormalizeVec3 uses Vector3, and MulVec3Mat3x3 uses raw component words. */
typedef struct {
    Fix12i x, y, z;
} Vec3_Fix12;
extern void CrossVec3(const Vec3_Fix12 *a, const Vec3_Fix12 *b, Vec3_Fix12 *out);
struct Vector3;
extern void NormalizeVec3(const struct Vector3 *v, struct Vector3 *out);
extern void Copy36Bytes(int *src, int *dst);
extern void MulVec3Mat3x3(const int *in, const int *m, int *out);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void func_02055388(int *m);             /* multiply by the 4x3 matrix */
extern void func_0204c24c(u8 tileS, u8 tileT); /* the XY-plane quad */

/* Draw one particle as a quad stretched along its flight (draw type 1,
 * func_0204a5c8): alpha and the next polygon ID into POLYGON_ATTR (nothing
 * more when the alpha is 0); the quad's x axis is velocity x the view's z
 * axis turned into view space (nothing is drawn when they are parallel), y
 * at right angles to it, scaled by scale * animScale (x by aspectRatio too,
 * y stretched by dbbScale the more the particle flies across the view),
 * translated to basePosition + offset in view space; then its color and the
 * XY-plane quad. pret's SPL calls this SPLDraw_DirectionalBillboard (that
 * revision adds the scale-animation axis and view-space cases);
 * func_0204b460 is the child-particle twin. */
void func_0204b81c(ManagerView *mgr, ParticleElement *ptcl) {
    ParticleSystem *sys = mgr->mCurrentSystem;
    s16 aspectRatio = sys->mDefinition->data->aspectRatio;
    int alpha = (ptcl->visibility.baseAlpha * (ptcl->visibility.animAlpha + 1)) >> 5;
    int *view = mgr->mDrawMatrix;
    Vector3 velDir;
    Vector3 trans;
    Vector3 dir;
    Vector3 look;
    int rot[9];
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
    sx = FX_MUL((int)sy, aspectRatio);

    /* the quad's x axis: velocity x the view's z column, in view space */
    dir = ptcl->velocity;
    look.x = view[2];
    look.y = view[5];
    look.z = view[8];
    CrossVec3((Vec3_Fix12 *)&dir, (Vec3_Fix12 *)&look, (Vec3_Fix12 *)&dir);

    if (dir.x == 0 && dir.y == 0 && dir.z == 0)
        return;

    NormalizeVec3((const struct Vector3 *)&dir, (struct Vector3 *)&dir);
    Copy36Bytes(view, rot);
    MulVec3Mat3x3((int *)&dir, rot, (int *)&dir);
    MulVec3Mat4x3(&trans, (Matrix4x3 *)view, &trans);
    velDir = ptcl->velocity;
    NormalizeVec3((const struct Vector3 *)&velDir, (struct Vector3 *)&velDir);

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
        reg_G3_IDENTITY = 0;
        func_02055388(mat);
    }

    reg_G3_COLOR = ptcl->color;
    {
        PtclData *data = mgr->mCurrentSystem->mDefinition->data;
        func_0204c24c(data->textureTileCountS, data->textureTileCountT);
    }
}
