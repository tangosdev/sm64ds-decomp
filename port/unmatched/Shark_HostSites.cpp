/* PORT_HOST_ABI. SHARK (225, 9daShark_c), ov090. Run rel0215 wave 2, lane
 * cast-ov090. Three sites; see unmatched/Skeeter_HostSites.cpp for the full
 * derivation of the ModelAnim and PMF shapes and
 * unmatched/MantaRay_HostSites.cpp for the PathPtr one.
 *
 * (1) src/_ZN5Shark6RenderEv.cpp -- the ModelAnim slot-5 collision (T1).
 *     Unconditional, null scale, the same body as MantaRay's.
 * (2) src/func_ov090_021338b4.cpp -- the state setter, record 0 (ENTER).
 * (3) src/_ZN5Shark8BehaviorEv.cpp -- the record-1 (TICK) pointer-to-member
 *     call plus the three PathPtr __thiscall member calls and the
 *     ApproachLinear overload. Shark's path fields are at 0x388 / 0x390 /
 *     0x38c where MantaRay's are 0x37c / 0x384 / 0x380, its horizontal
 *     approach step is 0x180 where MantaRay's is 0x60 and its forward speed is
 *     0x14000 where MantaRay's is 0xa000 -- read off Shark's own source, not
 *     copied across from the sibling.
 */
#include "ModelAnim.h"

extern "C" {

/* ---- (1) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch. */
int _ZN5Shark6RenderEv(void *selfv)
{
    ((ModelAnim *)((char *)selfv + 0x30c))->ModelAnim::Render(0);
    return 1;
}

/* ---- (2) and (3) ------------------------------------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int func_ov090_021338b4(void *cv, void *pv)   /* record 0, the ENTER half */
{
    *(void **)((char *)cv + 0x370) = pv;
    PortOv090Pmf *q = (PortOv090Pmf *)pv;
    if (q[0].fn == 0)
        return 1;
    return ((PortOv090StateFn)(size_t)q[0].fn)(cv);
}

struct ShVec3 { int x, y, z; };

unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Sub(ShVec3 *out, ShVec3 *a, ShVec3 *b);
int  LenVec3(ShVec3 *v);
short Vec3_HorzAngle(ShVec3 *v0, ShVec3 *v1);
short Vec3_VertAngle(ShVec3 *v0, ShVec3 *v1);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angle);
void MulVec3Mat4x3(ShVec3 *v, void *m, ShVec3 *out);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *thiz, void *clsn);
void func_ov090_02133904(void *c);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);
char *_ZN5Actor13ClosestPlayerEv(void *c);
void _Z14ApproachLinearRsss(short *v, short target, short step);
void _ZN7PathPtrC1Ev(void *self);
void _ZN7PathPtr6FromIDEj(void *self, unsigned id);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void *self, ShVec3 *out, unsigned idx);
extern char data_020a0e68[];

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class, plus
 * three PathPtr __thiscall member calls MSVC cannot name. */
int _ZN5Shark8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        PortOv090Pmf *o = *(PortOv090Pmf **)(c + 0x370);
        if (o[1].fn != 0)
            ((PortOv090StateFn)(size_t)o[1].fn)(c);
    }
    {
        char p[8];
        ShVec3 node;
        ShVec3 diff;
        ShVec3 v;
        int len;

        _ZN7PathPtrC1Ev(p);
        _ZN7PathPtr6FromIDEj(p, *(unsigned int *)(c + 0x388));
        _ZNK7PathPtr7GetNodeER7Vector3j(p, &node, *(unsigned int *)(c + 0x390));
        Vec3_Sub(&diff, (ShVec3 *)(c + 0x5c), &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= 0x258000) {
            (*(int *)(((int)c + 0x390)))++;
            if (*(int *)(c + 0x390) >= *(int *)(c + 0x38c))
                *(int *)(c + 0x390) = 0;
        }
        _Z14ApproachLinearRsss((short *)(c + 0x94),
                               Vec3_HorzAngle((ShVec3 *)(c + 0x5c), &node), 0x180);
        _Z14ApproachLinearRsss((short *)(c + 0x92),
                               Vec3_VertAngle((ShVec3 *)(c + 0x5c), &node), 0x40);
        *(short *)(c + 0x8c) = *(short *)(c + 0x92);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(short *)(c + 0x90) = *(short *)(c + 0x96);
        v.y = v.x = v.z = 0;
        v.z = 0x14000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x92));
        MulVec3Mat4x3(&v, data_020a0e68, (ShVec3 *)(c + 0xa4));
    }
    {
        int s = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int m2 = *(int *)(c + 0xa0);
        int ac = *(int *)(c + 0xac);
        if (s >= m2) m2 = s;
        *(int *)(c + 0xa8) = m2;
        *(int *)(c + 0xac) = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x110);
    func_ov090_02133904(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    return 1;
}

}  /* extern "C" */
