#include "types.h"
// @symbol func_ov062_0211ba84
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* NOT daRFlag_c::Kill, and not a vtable slot at all. This word is entry 0 of the
 * six-record {function, 0} table at ov062 0x0211dcd0, which sits immediately
 * after daRFlag_c's 31-slot vtable ends at that same address -- the leaf scan
 * that produced the old name read one word past the table. The body settles it
 * independently: it reads c+0x444 and constructs dBgCh_Actr at c+0x178, and
 * +0x178 is Klepto's dBgCh_Actr (alloc 0x490), not daRFlag_c's, whose whole
 * object is 0x174 bytes. The owning class is left unnamed here rather than
 * guessed. */
extern char *_ZN8dActor_c13ClosestPlayerEv(char *self);
extern int func_ov062_0211c658(char *c, void *p);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void MulVec3Mat4x3(const Vector3 *v, void *m, Vector3 *out);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(char *self);
extern void *data_ov062_0211e17c;
extern signed char data_0209f2f8;
extern int data_020a0e68[];

int func_ov062_0211ba84(char *c)
{
    Vector3 v;
    Vector3 t;
    Vector3 hv;
    Vector3 vv;
    s16 r4;
    char *ip;
    int tx, ty, tz;

    r4 = 0;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    ip = _ZN8dActor_c13ClosestPlayerEv(c);
    if (ip != 0) {
        if (*(u8 *)(ip + 0x6fb) == 1) {
            *(s16 *)(c + 0x444) = 0x1e;
            func_ov062_0211c658(c, &data_ov062_0211e17c);
            return 1;
        }

        t = *(Vector3 *)(ip + 0x5c);
        tx = t.x;
        {
            int y = *(int *)(ip + 0x644);
            t.y = y;
            t.y = y + 0x28000;
        }
        tz = t.z;
        ty = t.y;
        hv.x = tx; hv.y = ty; hv.z = tz;
        *(s16 *)(c + 0x44a) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &hv);
        vv.x = tx; vv.y = ty; vv.z = tz;
        r4 = Vec3_VertAngle((Vector3 *)(c + 0x5c), &vv);

    }
    if (data_0209f2f8 == 0x10) {
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), (s16)*(int *)(c + 0x43c));
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), r4, (s16)*(int *)(c + 0x43c));
    } else {
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), (s16)(*(int *)(c + 0x43c) + 0x500));
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), r4, (s16)(*(int *)(c + 0x43c) + 0x500));
    }
    v.z = 0x1e000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16 *)(c + 0x8c));
    MulVec3Mat4x3(&v, data_020a0e68, (Vector3 *)(c + 0xa4));
    func_ov062_0211b51c(c);
    if (*(u16 *)(c + 0x100) == 0 || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x178) != 0) {
        *(s16 *)(c + 0x444) = 0x1e;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}
