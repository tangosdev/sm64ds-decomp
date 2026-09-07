//cpp
/* Production translation unit for ov065/daObjCtMecha03_c.
 *
 * mwccarm emits ordinary functions in reverse source order, so the eight
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group.
 *
 * func_ov065_02119fe8 and func_ov065_0211a114 keep their address-derived
 * names: ov065's symbols.txt spells them that way, so they are C-linkage
 * free functions here rather than members.
 *
 * Superseded one-function sources (ROM address order):
 *   [0] 0x02119f3c  src/_ZN16daObjCtMecha03_cD1Ev.cpp
 *   [1] 0x02119f88  src/_ZN16daObjCtMecha03_cD0Ev.cpp
 *   [2] 0x02119fe8  src/func_ov065_02119fe8.cpp
 *   [3] 0x0211a114  src/func_ov065_0211a114.c
 *   [4] 0x0211a15c  src/_ZN16daObjCtMecha03_c16CleanupResourcesEv.cpp
 *   [5] 0x0211a1a0  src/_ZN16daObjCtMecha03_c6RenderEv.cpp
 *   [6] 0x0211a1c8  src/_ZN16daObjCtMecha03_c8BehaviorEv.cpp
 *   [7] 0x0211a358  src/_ZN16daObjCtMecha03_c13InitResourcesEv.cpp
 */

#include "daObjCtMecha03_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Behavior reads and writes 0x322 through a RAW combined offset -- not the
   (c+0x300)+0x22 decomposition used everywhere else in that function. The mask
   is a no-op on a 32-bit int; its only purpose is to stop the compiler from
   recognizing 0x322 as 0x300+0x22 and reusing the r4+0x300 base already live in
   a register nearby. The ROM computes this one address via a literal-pool add
   instead. */
#define I16(off) (*(short *)(((int)c + (off)) & 0xFFFFFFFF))

/* Every ROM symbol below is spelled by its already-mangled name: an ordinary
   (non-"C") declaration would mangle it a SECOND time and eligible.py's
   reference check flags the doubly-mangled name as unresolvable. `extern`
   inside the block on purpose -- the bare block form would DEFINE the data
   objects and collide at link time. */
extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *in, void *m, Vector3 *out);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int f, int a, int b, unsigned int c);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, void *bmd, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(void *thisp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thisp, void *kcl, void *mtx, int fix, short s, void *clps);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
extern void func_02039394(void *p, int v);
extern void func_020393a4(void *p, int v);
extern void func_020393d4(void *p, void *v);
extern int func_ov065_0211a114(char *self);
extern void func_ov065_02119fe8(char *self);
extern void *data_020a0e68;
extern u8 data_0209f2c0;
extern int data_0209e650;
extern short data_ov065_0211c0b0[];
extern int data_ov065_0211d88c[];
extern int data_ov065_0211d894[];
/* Same address as the "ambiguous" data_ovNNN_02112198 in every other overlay
   that reaches this InitResources shape (PyramidTop's ov024, etc.) -- a shared
   CLPS_Block. It is NOT ov065's own: config/arm9/overlays/ov065's lowest symbol
   is ~0x02115ee0, and tools/modules.py derives an overlay's base from its
   lowest symbol, so adding an ov065 entry here at 0x02112198 would drag every
   other ov065 address out of range.

   SETTLED to ov035 by tools/overlay_residency.py: of the eleven overlays whose
   own symbols.txt declares an "ambiguous" entry at this address
   (ov012/013/024/033/035/043/045/047/052/053/056), residency's loader/level
   constraints rule out every one but ov035 as able to coexist with ov065 --
   `settle()` returns exactly one survivor. */
extern int data_ov035_02112198;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN16daObjCtMecha03_c13InitResourcesEv, 0x0211a358, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c13InitResourcesEv
int daObjCtMecha03_c::InitResources()
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjCtMecha03_c8BehaviorEv, 0x0211a1c8, size 0x190 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c8BehaviorEv
int daObjCtMecha03_c::Behavior()
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjCtMecha03_c6RenderEv, 0x0211a1a0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c6RenderEv
int daObjCtMecha03_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjCtMecha03_c16CleanupResourcesEv, 0x0211a15c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
int daObjCtMecha03_c::CleanupResources()
{
    if (((dBgW *)((char *)this + 0x124))->IsEnabled())
        ((dBgW *)((char *)this + 0x124))->Disable();
    ((SharedFilePtr *)data_ov065_0211d88c)->Release();
    ((SharedFilePtr *)data_ov065_0211d894)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov065_0211a114, 0x0211a114, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211a114
/* Rebuilds the collider matrix from the actor's X/Y angles and copies the
   actor position into its translation row. */
extern "C" int func_ov065_0211a114(char *c)
{
    Matrix4x3_FromRotationZXYExt(c + 0xf0, 0, *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    int z = *(int *)(c + 0x64) >> 3;
    *(int *)(c + 0x11c) = z;
    return z;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov065_02119fe8, 0x02119fe8, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02119fe8
/* Drops the pendulum's shadow: swings a fixed offset through the actor's
   orientation, raycasts the ground under the result, then hands the shadow
   matrix to dActor_c::DropShadowScaleXYZ. */
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
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjCtMecha03_cD0Ev, 0x02119f88, size 0x60        */
/* ROM ordinal 0 -- _ZN16daObjCtMecha03_cD1Ev, 0x02119f3c, size 0x4c        */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */
