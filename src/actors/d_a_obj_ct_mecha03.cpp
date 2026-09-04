//cpp
/* ov065 / daObjCtMecha03_c -- the swinging pendulum of the clock-tower set
 * (registry profile CT_MECHA03, RTTI name at ov065:0x0211d0b4).
 *
 * One translation unit for the whole ROM run 0x02119f3c..0x0211a494: the four
 * class methods, the two file-local helpers that sit between the destructor
 * pair and CleanupResources, the destructor pair itself and the factory, plus
 * the .data run 0x0211d0a8..0x0211d16c that holds the class's RTTI pair, its
 * profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daObjCtMecha03_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */
#include "decl_common.h"

#include "daObjCtMecha03_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"

/* Reconciled from the nine per-function copies; where two of them spelled the
 * same symbol differently, the C++ (extern "C") spelling wins, since the whole
 * TU is now one C++ file. Every one of these is a ROM symbol spelled by its
 * final mangled name, so it needs C linkage: seen as C++ the compiler would
 * mangle the already-mangled spelling a second time and emit a reference
 * nothing defines. */
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void  _ZN10dBgActor_cC2Ev(void *self);
extern void  _ZN11ShadowModelC1Ev(void *self);

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, void *bmd, int a, int b);
extern void  _ZN11ShadowModel10InitCuboidEv(void *thisp);
extern void  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *thisp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thisp, void *kcl, void *mtx, int fix, short s, void *clps);
extern int   _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void  _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern void  _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int f, int a, int b, unsigned int c);
extern void  _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);

/* dBgW's three setters. None is in a decl_*.h; the per-function files
 * declared them locally. */
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
extern void func_020393d4(void *p, void *v);

extern u16  DecIfAbove0_Short(u16 *p);
extern int  RandomIntInternal(int *seed);
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *in, void *m, Vector3 *out);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);

extern u8  data_0209f2c0;
extern int data_0209e650;
extern void *data_020a0e68;

/* Defined below, in the reverse order the ROM holds them. */
int  func_ov065_0211a114(char *self);
void func_ov065_02119fe8(char *self);
}

extern int data_ov065_0211d88c[];
extern int data_ov065_0211d894[];
extern short data_ov065_0211c0b0[];

/* Same address as the "ambiguous" data_ovNNN_02112198 in every other overlay
   that reaches this InitResources shape (PyramidTop's ov024, etc.) -- a shared
   CLPS_Block. It is NOT ov065's own: config/arm9/overlays/ov065's lowest
   symbol is ~0x02115ee0, and tools/modules.py derives an overlay's base from
   its lowest symbol, so adding an ov065 entry here at 0x02112198 would drag
   every other ov065 address out of range. SETTLED to ov035 by
   tools/overlay_residency.py. */
extern int data_ov035_02112198;

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjCtMecha03_c_classInit, 0x0211a45c, size 0x38         */
/* ------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjCtMecha03_c through its
 * ROM RTTI string, allocation size, vtable identity, and the CT_MECHA03
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. Historical placeholder: func_ov065_0211a45c. */
// @symbol daObjCtMecha03_c_classInit
extern "C" int *daObjCtMecha03_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(904);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x0211d0e4 and +8 is what reaches the
         * 0x0211d0ec address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV16daObjCtMecha03_c[2];
        _ZN11ShadowModelC1Ev((char *)p + 0x330);
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* g_profile_CT_MECHA03, 0x0211d0c8, size 0x1c                                */
/* ------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTS (0x0211d0b4) and the vtable object (0x0211d0e4). */
struct CtMecha03SpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x006e == actor 110 */
    s16 drawOrder;                  /* 0x00e9 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char CtMecha03SpawnInfo_size_must_be_0x1c[
    sizeof(CtMecha03SpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_CT_MECHA03
extern "C" CtMecha03SpawnInfo g_profile_CT_MECHA03 = {
    daObjCtMecha03_c_classInit, 0x006e, 0x00e9, 0x00000002,
    -0x00200000, 0x00600000, 0x01000000, 0x01000000
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjCtMecha03_c::InitResources, 0x0211a358, size 0x104   */
/* ------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c13InitResourcesEv
s32 daObjCtMecha03_c::InitResources()
{
    char *self = (char *)this;
    void *bmd;
    void *kcl;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d88c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void *)(self + 0xd4), bmd, 1, -1);
    _ZN11ShadowModel10InitCuboidEv((void *)(self + 0x330));

    mSwingDir = 1;
    mSwingAccel = data_ov065_0211c0b0[data_0209f2c0];
    mSwingAngle = 0x1964;
    mAngleZ = mSwingAngle;

    func_ov065_0211a114(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);

    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov065_0211d894);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (void *)(self + 0x124), kcl, (void *)(self + 0x2ec), 0x1000,
        mAngleY, (void *)&data_ov035_02112198);

    func_020393a4((void *)(self + 0x124), 0x300000);
    func_02039394((void *)(self + 0x124), -0x200000);

    if (data_0209f2c0 != 3) {
        func_020393d4((void *)(self + 0x124),
            (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjCtMecha03_c::Behavior, 0x0211a1c8, size 0x190        */
/* ------------------------------------------------------------------------- */
/* I16 reads/writes 0x322 through a RAW combined offset -- not the (c+0x300)+0x22
   decomposition used everywhere else in this function. The mask is a no-op on a
   32-bit int; its only purpose is to stop the compiler from recognizing 0x322 as
   0x300+0x22 and reusing the r4+0x300 base already live in a register nearby.
   The ROM computes this one address via a literal-pool add instead -- verified
   by build_pin against the un-migrated func_ov065_0211a1c8. */
#define I16(off) (*(short *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFFLL))

// @symbol _ZN16daObjCtMecha03_c8BehaviorEv
s32 daObjCtMecha03_c::Behavior()
{
    char *c = (char *)this;

    if (data_0209f2c0 != 3) {
        if (*(unsigned short *)(c + 0x300 + 0x26) != 0) {
            if (DecIfAbove0_Short((u16 *)(c + 0x326)) == 0) {
                _ZN5Sound9PlayBank3EjRK7Vector3(0x38, c + 0x74);
            }
        }
        if (DecIfAbove0_Short((u16 *)(c + 0x328)) == 0) {
            {
                short vx = *(short *)(c + 0x300 + 0x1e);
                short vy = *(short *)(c + 0x300 + 0x22);
                short *accelP = (short *)(((int)c + 0x324) & 0xFFFFFFFF);
                if (vy * vx > 0) {
                    vx = -vx;
                    *(short *)(c + 0x300 + 0x1e) = vx;
                }
                {
                    short spd = *(short *)(c + 0x300 + 0x20);
                    short pos = *(short *)(c + 0x300 + 0x1e);
                    short accel = *accelP;
                    *accelP = (short)(spd * pos + accel);
                }
            }
            if (data_0209f2c0 == 2 && *(short *)(c + 0x300 + 0x24) == 0) {
                int r0 = RandomIntInternal(&data_0209e650);
                if ((unsigned)r0 % 3 != 0)
                    *(short *)(c + 0x300 + 0x20) = 0xd;
                else
                    *(short *)(c + 0x300 + 0x20) = 0x2a;
                if ((r0 & 1) == 0) {
                    *(short *)(c + 0x300 + 0x28) = ((unsigned)r0 >> 0x1b) + 3;
                }
            }
            if (*(short *)(c + 0x300 + 0x24) == 0) {
                *(short *)(c + 0x300 + 0x26) = *(unsigned short *)(c + 0x300 + 0x28) + 0xf;
            }
            I16(0x322) = I16(0x322) + *(short *)(c + 0x300 + 0x24);
        }
        *(short *)(c + 0x90) = *(short *)(c + 0x300 + 0x22);
    }

    func_ov065_0211a114(c);
    func_ov065_02119fe8(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0x300000, -0x200000) != 0)
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjCtMecha03_c::Render, 0x0211a1a0, size 0x28           */
/* ------------------------------------------------------------------------- */
/* The per-function file spelled this with two local shadow types -- a `struct
 * Base` of six virtuals and a `struct Derived { char pad[0xd4]; Base base; }`
 * -- which are dBgActor_c's Model at +0xd4 and its slot-5
 * Render(const Vector3 *). Reconciled onto the real member. */
// @symbol _ZN16daObjCtMecha03_c6RenderEv
s32 daObjCtMecha03_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjCtMecha03_c::CleanupResources, 0x0211a15c, size 0x44 */
/* ------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
s32 daObjCtMecha03_c::CleanupResources()
{
    if (((dBgW *)((char *)this + 0x124))->IsEnabled())
        ((dBgW *)((char *)this + 0x124))->Disable();
    ((SharedFilePtr *)data_ov065_0211d88c)->Release();
    ((SharedFilePtr *)data_ov065_0211d894)->Release();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov065_0211a114, 0x0211a114, size 0x48               */
/* ------------------------------------------------------------------------- */
/* Rebuilds the platform's collision matrix from its facing and roll, and parks
 * the position, scaled down by 8, in the matrix's translation row. It returns
 * that last Z as a side effect nobody reads -- but the ROM leaves it in r0, so
 * the return type is not void. Both callers are in this file. */
// @symbol func_ov065_0211a114
extern "C" int func_ov065_0211a114(char *c)
{
    Matrix4x3_FromRotationZXYExt(c + 0xf0, 0, *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    {
        int z = *(int *)(c + 0x64) >> 3;
        *(int *)(c + 0x11c) = z;
        return z;
    }
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov065_02119fe8, 0x02119fe8, size 0x12c              */
/* ------------------------------------------------------------------------- */
/* The per-frame drop shadow. It rotates a point 0x320000 below the platform's
 * origin by the platform's own three angles, adds the platform position, casts
 * a dBgCh_Gnd ray from 0xc8000 below that, parks the hit height in
 * mShadowGroundY, and projects the shadow through mShadowMat. */
// @symbol func_ov065_02119fe8
extern "C" void func_ov065_02119fe8(char *self)
{
    Vector3 v1;
    Vector3 v2;
    Vector3 pos;
    v1.y = 0;
    v1.y = -0x320000;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0;
    v1.x = 0;
    v1.z = 0;
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, *(s16 *)(self + 0x8c),
                                 *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    MulVec3Mat4x3(&v1, &data_020a0e68, &v2);
    AddVec3(&v2, (Vector3 *)(self + 0x5c), &v2);
    {
        int vy = v2.y;
        pos.x = v2.x;
        pos.z = v2.z;
        pos.y = vy;
        pos.y = vy - 0xc8000;
    }
    dBgCh_Gnd rc;
    rc.SetObjAndPos(pos, 0);
    *(int *)(self + 0x32c) = pos.y;
    if (rc.DetectClsn())
        *(int *)(self + 0x32c) = rc.clsnY;
    Matrix4x3_FromRotationY(self + 0x358, *(s16 *)(self + 0x8e));
    *(int *)(self + 0x37c) = v2.x >> 3;
    *(int *)(self + 0x380) = *(int *)(self + 0x32c) >> 3;
    *(int *)(self + 0x384) = v2.z >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, self + 0x330, self + 0x358, 0x12c000, 0x12c000, 0x78000, 0xf);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN16daObjCtMecha03_cD1Ev  0x02119f3c  size 0x4c  (complete-object dtor) */
/*   _ZN16daObjCtMecha03_cD0Ev  0x02119f88  size 0x60  (deleting destructor)  */
/* ------------------------------------------------------------------------- */
/* Defined in include/daObjCtMecha03_c.h's class body, and deliberately NOT
 * repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all. The body is the two vptr stores,
 * the ShadowModel member's own destructor -- this class's one member with a
 * destructor, which is what makes D1 0x8 bytes longer than a bare dBgActor_c
 * child's -- and the dBgActor_c teardown. D0 additionally returns the object
 * to its heap through the inline operator delete it inherits. */

// @symbol _ZN16daObjCtMecha03_cD0Ev
// @symbol _ZN16daObjCtMecha03_cD1Ev
