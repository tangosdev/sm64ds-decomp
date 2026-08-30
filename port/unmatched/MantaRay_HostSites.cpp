/* PORT_HOST_ABI. MANTA_RAY (226, 9daManta_c), ov090. Run rel0215 wave 2, lane
 * cast-ov090. Three sites, same three shapes as Skeeter's; see
 * unmatched/Skeeter_HostSites.cpp for the full derivation of each.
 *
 * (1) src/_ZN8MantaRay6RenderEv.cpp -- the ModelAnim slot-5 collision (T1).
 *     MantaRay draws unconditionally and passes a NULL scale, so its host copy
 *     is the bare dispatch; the 0x40000 flag guard Skeeter and CheepCheep have
 *     is not in MantaRay's source and is not invented here.
 *
 * (2) src/func_ov090_02132ac4.cpp -- the state setter, record 0 (ENTER),
 *     cell pointer at self+0x370. The PMF disease, incomplete-class form.
 *
 * (3) src/_ZN8MantaRay8BehaviorEv.cpp -- TWO refusals in one body, which is
 *     why the whole TU is hosted rather than just its dispatch:
 *       (a) the record-1 (TICK) pointer-to-member call, and
 *       (b) PathPtr AS A REAL C++ OBJECT. The source declares
 *             struct PathPtr { char pad[8]; PathPtr(); void FromID(unsigned);
 *                              void GetNode(Vector3&, unsigned) const; };
 *           and constructs one on the stack. Those are __thiscall member calls
 *           under MSVC and mangle to ?FromID@PathPtr@@..., which nothing in
 *           this link defines; hal/cxx_aliases.cpp bridges only the FLAT
 *           spellings (?_ZN7PathPtr6FromIDEj@@YAXPAXI@Z -> __ZN7PathPtr6FromIDEj),
 *           not the member ones. The host copy calls the flat arm9 symbols
 *           with an explicit self, the level_boot.cpp:4940 shape, over an
 *           8-byte local -- PathPtr's own size, from its pad[8].
 *       Likewise `ApproachLinear(short&, short, short)` is spelled as a plain
 *       C++ overload in the source and is really _Z14ApproachLinearRsss.
 *     Everything else is the matched source statement for statement.
 *
 * (4) src/_ZN8MantaRay13InitResourcesEv.cpp -- THE SAME PathPtr REFUSAL, found
 *     by the FIRST LINK rather than predicted (the slice_vs.txt "measured gap"
 *     discipline). It declares the same local `struct PathPtr` with a
 *     constructor and calls PathPtr::FromID, PathPtr::NumNodes and
 *     PathPtr::GetNode as __thiscall members; the link named all four
 *     decorations as unresolved. Nothing in this build defines a PathPtr
 *     member under those signatures -- include/PathPtr.h declares NumNodes
 *     returning `unsigned int` where this TU declares `int`, so even a face
 *     built on that header would decorate differently -- and hal/cxx_aliases
 *     bridges only the FLAT spellings. Host copy, same remedy as (3): the flat
 *     arm9 symbols with an explicit self over an 8-byte local.
 */
#include "ModelAnim.h"

extern "C" {

/* ---- (1) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch. */
int _ZN8MantaRay6RenderEv(void *selfv)
{
    ((ModelAnim *)((char *)selfv + 0x30c))->ModelAnim::Render(0);
    return 1;
}

/* ---- (2) and (3) ------------------------------------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int func_ov090_02132ac4(void *cv, void *pv)   /* record 0, the ENTER half */
{
    *(void **)((char *)cv + 0x370) = pv;
    PortOv090Pmf *q = (PortOv090Pmf *)pv;
    if (q[0].fn == 0)
        return 1;
    return ((PortOv090StateFn)(size_t)q[0].fn)(cv);
}

struct MrVec3 { int x, y, z; };

unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Sub(MrVec3 *out, MrVec3 *a, MrVec3 *b);
int  LenVec3(MrVec3 *v);
short Vec3_HorzAngle(MrVec3 *v0, MrVec3 *v1);
short Vec3_VertAngle(MrVec3 *v0, MrVec3 *v1);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angle);
void MulVec3Mat4x3(MrVec3 *v, void *m, MrVec3 *out);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *thiz, void *clsn);
void func_ov090_02132b14(void *c);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);
void _Z14ApproachLinearRsss(short *v, short target, short step);
void _ZN7PathPtrC1Ev(void *self);
void _ZN7PathPtr6FromIDEj(void *self, unsigned id);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void *self, MrVec3 *out, unsigned idx);
extern char data_020a0e68[];

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class, plus
 * three PathPtr __thiscall member calls MSVC cannot name. */
int _ZN8MantaRay8BehaviorEv(void *selfv)
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
        MrVec3 node;
        MrVec3 diff;
        MrVec3 v;
        int len;

        _ZN7PathPtrC1Ev(p);
        _ZN7PathPtr6FromIDEj(p, *(unsigned int *)(c + 0x37c));
        _ZNK7PathPtr7GetNodeER7Vector3j(p, &node, *(unsigned int *)(c + 0x384));
        Vec3_Sub(&diff, (MrVec3 *)(c + 0x5c), &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= 0x258000) {
            (*(int *)(((int)c + 0x384)))++;
            if (*(int *)(c + 0x384) >= *(int *)(c + 0x380))
                *(int *)(c + 0x384) = 0;
        }
        _Z14ApproachLinearRsss((short *)(c + 0x94),
                               Vec3_HorzAngle((MrVec3 *)(c + 0x5c), &node), 0x60);
        _Z14ApproachLinearRsss((short *)(c + 0x92),
                               Vec3_VertAngle((MrVec3 *)(c + 0x5c), &node), 0x40);
        *(short *)(c + 0x8c) = *(short *)(c + 0x92);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(short *)(c + 0x90) = *(short *)(c + 0x96);
        v.y = v.x = v.z = 0;
        v.z = 0xa000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x92));
        MulVec3Mat4x3(&v, data_020a0e68, (MrVec3 *)(c + 0xa4));
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
    func_ov090_02132b14(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}

/* ---- (4) InitResources, the same PathPtr refusal ----------------------- */
void *_ZN5Model8LoadFileER13SharedFilePtr(void *p);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
void _ZN9Animation8LoadFileER13SharedFilePtr(void *p);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        void *self, void *a, const MrVec3 *v, int b, int c, unsigned d, unsigned e);
unsigned _ZNK7PathPtr8NumNodesEv(const void *self);
extern char data_ov090_02134524[];
extern char data_ov002_0210da10[];
extern char data_ov002_0210d9a8[];
extern char data_ov090_0213452c[];
extern MrVec3 data_ov090_02134200;
extern int data_ov090_0213454c;
extern unsigned char data_0209f2d8;

/* PORT_HOST_ABI: two PathPtr locals built and walked as real C++ objects. */
int _ZN8MantaRay13InitResourcesEv(void *selfv)
{
    unsigned char *thiz = (unsigned char *)selfv;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(thiz + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_02134524), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da10);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_0213452c);

    *(int *)(thiz + 0x37c) = *(int *)(thiz + 8) & 0xff;
    *(int *)(thiz + 0x388) = (*(unsigned int *)(thiz + 8) >> 0xc) & 0xf;
    if (*(int *)(thiz + 0x37c) < 0) *(int *)(thiz + 0x37c) = 0;

    {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, *(unsigned int *)(thiz + 0x37c));
        *(int *)(thiz + 0x380) = (int)_ZNK7PathPtr8NumNodesEv(pp);
    }

    *(int *)(thiz + 0xa0) = -0x3c000;

    {
        MrVec3 v;
        v.x = data_ov090_02134200.x;
        v.y = data_ov090_02134200.y;
        v.z = data_ov090_02134200.z;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            thiz + 0x110, thiz, &v, 0x150000, 0xc8000, 0x200004, 0);
    }

    {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, *(unsigned int *)(thiz + 0x37c));
        *(int *)(thiz + 0x384) = 1;
        _ZNK7PathPtr7GetNodeER7Vector3j(pp, (MrVec3 *)(thiz + 0x5c),
                                        *(unsigned int *)(thiz + 0x384));
    }

    {
        int b = (int)(data_0209f2d8 == 2);
        if (b != 0) {
            *(int *)(thiz + 0x384) = 3;
            *(short *)(thiz + 0x92) = (short)0xf303;
            *(short *)(thiz + 0x94) = 0xb50;
            *(short *)(thiz + 0x96) = 0;
            *(int *)(thiz + 0x5c) = (int)0xfdfb8000;
            *(int *)(thiz + 0x60) = (int)0xff8f8000;
            *(int *)(thiz + 0x64) = 0x29a000;
            *(int *)(thiz + 0xb0) = 0;
        }
    }

    func_ov090_02132ac4(thiz, &data_ov090_0213454c);
    return 1;
}

}  /* extern "C" */
