//cpp
/* dScMgSnowball_c -- the snowball-rolling minigame scene: 23 functions
 * (.text 0x0212568c..0x021295ac), the destructor, eight virtual overrides and
 * the thirteen helpers they call.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; removing it
 * breaks five functions and the section order. The six bracketed optimizer
 * pragmas are needed too, and so is the include order (see below).
 *
 * Blocked: the helpers are unnamed in symbols.txt. Some calls stay mangled:
 * cstd::atan2, ApproachLinear and Particle::System take Fix12 or reference
 * arguments, and decl_common.h declares a global named G2, so no
 * `namespace G2` can be opened here.
 */
#pragma defer_codegen off

/* Include order is load-bearing. dScMgSnowball_c.h reaches math/Matrix.h,
   which defines Matrix4x3 as {Matrix3x3 r; Vector3 t;}; common.h carries a
   second, flat `s32 m[12]` spelling of the same 0x30-byte type behind the
   same guard, and whichever a TU sees first stands. The nested spelling wins
   here, so the one copy that has to be a block move goes through M4x3Flat,
   below, rather than member-wise through Vector3's destructor. */
#include "dScMgSnowball_c.h"
#include "types.h"
#include "decl_common.h"
#include "common.h"
#include "Sound.h"
#include "dScMgBase_c.h"
#include "Particle__System.h"

/* Declarations the recovered sources need that no project header supplies.
 * Each one was checked against include/*.h first; these are the residue. */
typedef struct Vec3 {
    int x, y, z;
} Vec3;

struct V3 { int x, y; volatile int z; };

/* The FLAT 12-word view of Matrix4x3. include/common.h and include/math/Matrix.h
   carry two spellings of this 0x30-byte type and whichever a TU sees first
   stands; as one-function files these members saw common.h's flat `s32 m[12]`,
   but this TU reaches math/Matrix.h first through dScMgSnowball_c.h -> Model.h,
   and the nested {Matrix3x3 r; Vector3 t;} spelling copies member-wise because
   Vector3 has a user-declared destructor -- 0x20 bytes more than the ROM has.
   Copying through this view restores the cartridge's block copy. */
struct M4x3Flat { s32 m[12]; };

typedef struct Pair {
    s32 x;
    s32 y;
} Pair;

struct InitObject {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int query();
};

struct SPS {
    int a[2];
    int b[2];
    int cc[3];
};

#define MUL12(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define I(o) (*(s32 *)(c + (o)))
#define H(o) (*(u16 *)(c + (o)))
/* The SIGNED halfword read. The one-function files spelled this offset macro
   two ways -- `u16` in the helpers, `short` in Behavior -- and the merge can
   only keep one. The ROM settles it per site: the three angle carries at the
   top of Behavior are `ldrsh` at 0x021283d8/0x021283e0/0x021283e8, so those
   reads are signed and use HS; every other halfword access in this file is a
   store, where the two spellings emit the same `strh`. */
#define HS(o) (*(s16 *)(c + (o)))
#define B(o) (*(u8 *)(c + (o)))
#define AT(p, o) ((void *)(int)((char *)(p) + (o)))
#define LNDR(e) ((int)((long long)(e)))
#define C1 ((char*)(void*)(int)(c))
#define C2 ((char*)(void*)(int)(C1))
#define ATS(off) AT(C1,off)
#define ATI(off) AT(C2,off)
#define IA(o) (*(int*)AT(c,(o)))
#define HA(o) (*(short*)AT(c,(o)))
#define atan2 _ZN4cstd5atan2E5Fix12IiES1_
#define pnew _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
#define pfromid(id) ((int *)Particle::System::FromUniqueID(id))
#define ApproachLinear _Z14ApproachLinearRsss
/* H is `short` in Behavior and `unsigned short` in the helpers; HS is the
 * signed read where a site needs it. */

namespace cstd { int fdiv(int numerator, int denominator); }
namespace G2S  { char *GetBG2ScrPtr(); unsigned GetBG2CharPtr(); void *GetBG3ScrPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace GXS  { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace CP15 { void FlushAndInvalidateDataCache(u32 address, u32 length); }
namespace G3X  { void SetFog(bool enable, int blend, int slope, int offset); }

extern "C" {
extern void RenderOamBothScreens(int a, int b, int c, int d, int e, int f);
extern int data_ov006_02139c6c[];
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *dst);
extern int func_ov006_021259d8(char *o, int *p);
extern int func_ov006_02125cdc(int c, int *p);
extern int func_ov006_02125bbc(char *o, int *p);
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *p);
extern int Vec2_Len(int *p);
extern void func_0203d630(int *p, int m);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir, void *callback);
extern void func_02012718(int a, int b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
extern void Camera_UpdateMatrices(void *self);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern struct Matrix4x3 data_020a0e68;
extern void func_ov006_02126b4c(char *c, int a, int b);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(int val, char *dst, int n);
extern u16 data_ov006_0212f3bc[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void func_ov004_020b04d0(int);
extern void SetBg2Offset(int, int);
extern void SetBg3Offset(int, int);
extern void SetSubBg2Offset(int, int);
extern void SetSubBg3Offset(int, int);
extern void func_020731dc(void *, void *, void **);
extern void Quaternion_Normalize(s32 *);
extern void _ZN7Vector3D1Ev(void);
extern void func_ov006_02126948(char *);
extern void func_ov006_0212a3c0(char *);
extern void func_ov006_02125994(char *);
extern u32 data_ov006_02143004;
extern void *data_ov006_02143008;
extern s32 data_ov006_02143014[3];
extern s32 data_02092768[4];
void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void func_ov006_02126a98(char *c);
void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
s32  GetGameLanguage(void);
void func_02012790(int a);
void FreeGfxSlotsById(int arg);
void Vec2_Sub(int *o, int *a, int *b);
void _Z14ApproachLinearRsss(short *a, short b, short cc);
extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern "C" void Ov004_Deallocate(void *p);
void *_ZN2G213GetBG2CharPtrEv(void);
void *_ZN2G212GetBG3ScrPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
void *func_ov004_020adc74(void *p);
void DecompressLZ16(void *src, void *dst);
extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
 /* Where two recovered declarations disagreed, the more precise one is
  * kept. */
}

// @symbol _ZN15dScMgSnowball_cD1Ev
/* Destroys mArray3, mArray2 and mArray1, in reverse of construction. The
   ROM's fourth call, ~Model on mModel, is not written: mModel is declared
   before the arrays, so the compiler destroys it last, where the ROM has it.
   The rest is the inlined dScMgSingle3DBase_c destructor. */
dScMgSnowball_c::~dScMgSnowball_c()
{
    __cxa_vec_cleanup(mArray3, 0x20, 0x24, (void *)func_ov006_02125800);
    __cxa_vec_cleanup(mArray2, 0x80, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mArray1, 0x80, 8, (void *)NullDestructor_0203d47c);
}

// @symbol _ZN15dScMgSnowball_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

// @symbol func_ov006_02125800
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02125800(void)
{
}
}

// @symbol func_ov006_02125804
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02125804(char *scene)
{
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int i;
    char *p = scene;
    for (i = 0; i < 0x20; i++) {
        if (*(unsigned char *)(p + 0xba34) != 0) {
            RenderOamBothScreens(
                data_ov006_02139c6c[*(int *)(p + 0xba30)],
                *(int *)(p + 0xba14) >> 0xc,
                ((((*(int *)(p + 0xba18) - *(int *)(p + 0xba1c)) - self->mScrollY) >> 0xc) - 0x110),
                -1, 0, 0);
        }
        p += 0x24;
    }
}
}

// @symbol func_ov006_02125890
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02125890(char *scene)
{
    int i;
    char *rec = scene;
    char *pos = scene + 0xba14;
    char *vel = scene + 0xba20;
    for (i = 0; i < 0x20; i++, rec += 0x24, pos += 0x24, vel += 0x24) {
        if (*(u8 *)(rec + 0xba34)) {
            if (*(int *)(rec + 0xba2c) > 0) {
                *(int *)(((int)rec + 0xba2c)) -= 1;
                if (*(int *)(rec + 0xba2c) <= 0) {
                    *(u8 *)(scene + i * 0x24 + 0xba34) = 0;
                    return;
                }
            }
            *(int *)(((int)rec + 0xba28)) -= 0x200;
            AddVec3((Vec3 *)pos, (Vec3 *)vel, (Vec3 *)pos);
            if (*(int *)(rec + 0xba28) < 0 && *(int *)(rec + 0xba1c) < 0) {
                *(u8 *)(scene + i * 0x24 + 0xba34) = 0;
                return;
            }
        }
    }
}
}

// @symbol func_ov006_02125994
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02125994(char *rec) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(int*)(rec + 0xba14) = 0;
        *(int*)(rec + 0xba18) = 0;
        *(int*)(rec + 0xba1c) = 0;
        *(int*)(rec + 0xba20) = 0;
        *(int*)(rec + 0xba24) = 0;
        *(int*)(rec + 0xba28) = 0;
        *(int*)(rec + 0xba2c) = 0;
        *(unsigned char*)(rec + 0xba34) = 0;
        *(int*)(rec + 0xba30) = 0;
        rec += 0x24;
    }
}
}

// @symbol func_ov006_021259d8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_021259d8(char *scene, int *point) {
    int x = point[0];
    int y;
    int fa;
    int fb;
    int row;
    int col;
    unsigned int v;
    int sum;
    int diff;

    if (x < 0) {
        return 1;
    }
    if (x >= 0x100000) {
        return 1;
    }

    y = point[1];
    if (y < 0) {
        return 0;
    }
    if (y >= (*(int *)(scene + 0xba08) << 16)) {
        return 0;
    }
    if (y >= (*(int *)(scene + 0xab6c) + 0x1d0000)) {
        return 0;
    }

    fa = x >> 12;
    fb = y >> 12;
    row = fa / 16;
    col = fb / 16;
    v = *(unsigned short *)(scene + row * 0x5c0 + col * 2 + 0x4f38);
    sum = (fa & 0xf) + (fb & 0xf);
    diff = (fa & 0xf) - (fb & 0xf);

    switch (v) {
    case 0:
    case 0xd:
        return 0;
    case 0x10:
        return 1;
    case 0x11:
    case 0x1b:
        return (fa & 8) == 0;
    case 0x12:
        return sum < 0x18;
    case 0x13:
        return sum < 8;
    case 0x14:
        return diff < 8;
    case 0x15:
        return diff < -8;
    case 0x16:
    case 0x1c:
        return (fa & 8) != 0;
    case 0x17:
        return diff >= 8;
    case 0x18:
        return diff >= -8;
    case 0x19:
        return sum >= 0x18;
    case 0x1a:
        return sum >= 8;
    case 0xc:
        return 0;
    default:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 0xa:
    case 0xb:
    case 0xe:
    case 0xf:
        return 0;
    }
}
}

#pragma opt_strength_reduction off
// @symbol func_ov006_02125bbc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_02125bbc(char *scene, int *point)
{
    int i;
    for (i = 0; i < 0x80; i++) {
        if (*(u8 *)(scene + i + 0xac58) == 1) {
            char *prop = scene + i * 8;
            int dx, dy;
            if (point[0] >= *(int *)(prop + 0xacd8) + 0x10000)
                continue;
            if (point[0] < *(int *)(prop + 0xacd8) - 0x10000)
                continue;
            if (point[1] >= *(int *)(prop + 0xacdc) + 0x8000)
                continue;
            if (point[1] < *(int *)(prop + 0xacdc) - 0x8000)
                continue;
            dy = *(int *)(prop + 0xacdc) - point[1];
            dx = *(int *)(prop + 0xacd8) - point[0];
            if ((int)(((s64)dx * dx + 0x800) >> 12) + (int)(((s64)dy * dy + 0x800) >> 12) / 4 > 0x100000)
                continue;
            *(u8 *)(scene + i + 0xb2d8) = 1;
            if (*(int *)(scene + i * 4 + 0xb0d8) == 1) {
                *(u8 *)(scene + 0xb9e6) = 1;
                *(int *)(scene + 0xb9e8) = *(int *)(prop + 0xacd8);
                *(int *)(scene + 0xb9ec) = *(int *)(prop + 0xacdc);
            }
            return 1;
        }
    }
    return 0;
}
}
#pragma opt_strength_reduction on

#pragma opt_common_subs off
// @symbol func_ov006_02125cdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_02125cdc(int raw, int *point)
{
    char *scene = (char *)raw;
    int z = point[1];
    int x;
    unsigned short t;
    int sum, diff, xf, zf;

    if (z < -0x20000)
        return 1;
    if (z >= *(int *)(scene + 0xba08) << 16)
        return 1;
    if (z >= *(int *)(scene + 0xab6c) + 0x1d0000)
        return 1;
    x = point[0];
    if (x < 0)
        return 1;
    if (x >= 0x100000)
        return 1;
    if (z < 0)
        t = *(unsigned short *)(scene + ((x >> 12) / 16) * 0x5c0 + 0x4f38);
    else
    {
        int xq = (x >> 12) / 16;
        xq = xq ? xq : xq;
        t = *(unsigned short *)((char *)(raw + xq * 0x5c0 + ((z >> 12) / 16) * 2) + 0x4f38);
    }
    zf = (z >> 12) & 0xf;
    xf = (x >> 12) & 0xf;
    sum = xf + zf;
    diff = xf - zf;
    switch (t) {
    case 0: case 13: return 0;
    case 1: return 1;
    case 2: case 14: return ((x >> 12) & 8) == 0;
    case 3: return sum < 0x18;
    case 4: return sum < 8;
    case 5: return diff < 8;
    case 6: return diff < -8;
    case 7: case 15: return ((x >> 12) & 8) != 0;
    case 8: return diff >= 8;
    case 9: return diff >= -8;
    case 10: return sum >= 0x18;
    case 11: return sum >= 8;
    case 12: return 0;
    default: return 0;
    }
}
}
#pragma opt_common_subs on

// @symbol func_ov006_02125f68
/* The collision ring.
 * Once a tick the snowball fires 32 probes around itself, one every 0x800 of
 * angle at the current ball radius. Each probe records three flags -- solid,
 * push-out, in-water -- and the whole ring is re-fired (up to 0x21 times)
 * while anything is still hit, pushing the ball out along the averaged normal
 * each pass. If nothing is hit and the ball started inside water, the water
 * probes instead add buoyancy to the velocity. A breakable hit shrinks the
 * ball, spawns the two 0xf1/0xf2 particle systems scaled by the new radius,
 * and fires the break sound; a fast non-breakable hit fires the bump sound.
 * The tail re-walks the push flags to build the separation vector and
 * reflects the velocity off it.
 *
 * Four spellings are load-bearing, all measured against the ROM on 2004/b56:
 *   - `hit` is dScMgSnowball_c::Vec2, whose empty destructor keeps it an
 *     8-byte aggregate in the frame (sample at sp+0x38, hit at sp+0x40, acc at
 *     sp+0x48). As two ints or an int[2] it is scalarised into spill words at
 *     sp+0x18/0x1c and every later slot shifts.
 *   - `part` is Vector3 for the same reason: as three ints or an int[3] the
 *     two Particle::System::New calls keep the scaled coordinates in
 *     callee-saved registers instead of storing them at sp+0x90 and reloading
 *     for the second call (-6 instructions).
 *   - the four Particle rate fields re-read self->mBallSize instead of caching
 *     `mBallSize - 0x4000` in a local: the ROM re-loads 0xba0 for each of the
 *     six assignments.
 *   - `sample[1] += self->mBallSize` (not `0 + mBallSize`): the ROM adds
 *     through the register that just stored the zero, so the add survives.
 * The nine zero/constant scalars before the ring are the ROM's own frame
 * words sp+0x14..0x34 in source order; folding any pair together drops a
 * store the cartridge makes. The push loop divides its own `k * 0x10000`
 * rather than carrying an angle accumulator, which is what stops mwccarm
 * strength-reducing the /32 into a second induction variable. */
extern "C" void func_ov006_02125f68(char *p_)
{
    dScMgSnowball_c *self = (dScMgSnowball_c *)p_;
    int i;
    int k;
    int farHit;
    int anyHit;
    int initInside;
    int iter;
    int zNoHit;
    int zDry;
    int zOrigin;
    int pushDirX;
    int pushDirY;
    int floatDirX;
    int floatDirY;
    int zNotAllWater;
    int angle;
    int j;
    int sample[2];
    dScMgSnowball_c::Vec2 hit;
    int acc[2];
    int rotA[2];
    int rotB[2];
    int push[2];
    int rotPush[2];
    int initPt[2];
    int cdcCopy[2];
    int bbcCopy[2];
    int waterCopy[2];
    Vector3 part;
    int allWater;
    int len;
    int *sys;
    u32 id1;
    u32 id2;
    int cosv;
    int sc;
    int vx;
    int vy;

    i = 0;
    do {
        self->mProbeHit[i] = 0;
        self->mProbePush[i] = 0;
        self->mProbeWater[i] = 0;
        i = i + 1;
    } while (i < 0x20);
    farHit = 0;
    self->mHitBreakable = 0;

    initPt[0] = self->mPosX;
    initPt[1] = self->mPosY;
    initInside = func_ov006_021259d8((char *)self, initPt);

    iter = 0;
    zDry = 0;
    pushDirX = 0;
    pushDirY = 0x800;
    zNotAllWater = 0;
    floatDirX = 0;
    floatDirY = 0x1000;
    zOrigin = 0;
    zNoHit = 0;
    do {
        anyHit = zNoHit;
        angle = zNoHit;
        for (j = zNoHit; j < 0x20; j++) {
            self->mProbeHit[j] = 0;
            sample[0] = 0;
            sample[1] = 0;
            sample[1] += self->mBallSize;
            func_0203d388(sample, angle);
            sample[0] += self->mPosX;
            sample[1] += self->mPosY;
            cdcCopy[0] = sample[0];
            cdcCopy[1] = sample[1];
            if (func_ov006_02125cdc((int)self, cdcCopy) != 0) {
                self->mProbeHit[j] = 1;
                self->mProbePush[j] = 1;
                anyHit = 1;
                if (Vec2_Len(&self->mVelX) >= 0x800)
                    farHit = 1;
                hit.x = sample[0];
                hit.y = sample[1];
            } else {
                bbcCopy[0] = sample[0];
                bbcCopy[1] = sample[1];
                if (func_ov006_02125bbc((char *)self, bbcCopy) != 0) {
                    self->mProbeHit[j] = 1;
                    anyHit = 1;
                    self->mProbePush[j] = 1;
                    self->mHitBreakable = 1;
                    hit.x = sample[0];
                    hit.y = sample[1];
                }
            }
            waterCopy[0] = sample[0];
            waterCopy[1] = sample[1];
            if (func_ov006_021259d8((char *)self, waterCopy) != 0)
                self->mProbeWater[j] = 1;
            else
                self->mProbeWater[j] = zDry;
            angle = (s16)(angle + 0x800);
        }

        acc[0] = zOrigin;
        acc[1] = zOrigin;
        angle = zOrigin;
        if (anyHit != 0) {
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeHit[i] == 1) {
                    rotA[0] = pushDirX;
                    rotA[1] = pushDirY;
                    func_0203d388(rotA, angle);
                    acc[0] += rotA[0];
                    acc[1] += rotA[1];
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(acc);
            if (Vec2_Len(&self->mVelX) < 0x800)
                func_0203d630(acc, Vec2_Len(&self->mVelX));
            else
                func_0203d630(acc, pushDirY);
            self->mPosX -= acc[0];
            self->mPosY -= acc[1];
        } else if (initInside == 1) {
            allWater = 1;
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeWater[i] == 1) {
                    rotB[0] = floatDirX;
                    rotB[1] = floatDirY;
                    func_0203d388(rotB, angle);
                    acc[0] += rotB[0];
                    acc[1] += rotB[1];
                } else {
                    allWater = zNotAllWater;
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(acc);
            self->mVelX += acc[0];
            self->mVelY += acc[1];
            if (allWater == 1)
                self->mAllWater = 1;
        }
        iter = iter + 1;
    } while (anyHit == 1 && iter < 0x21);

    if (self->mHitBreakable == 1) {
        len = Vec2_Len(&self->mVelX);
        self->mBallSize -= len;
        if (self->mBallSize < 0x4000)
            self->mBallSize = 0x4000;
        if (len >= 0x800) {
            int pz = self->mBallSize;
            int py = (self->mScrollY - hit.y) + 0x110000;
            int px = hit.x - 0x80000;
            part.x = px;
            part.y = py;
            part.z = pz;
            id1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf1, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            id2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf2, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            sys = (int *)Particle::System::FromUniqueID(id1);
            if (sys != 0) {
                sys[0x44 / 4] = (s16)((self->mBallSize - 0x4000) * 11 / 60 + 0x3000);
                sys[0x48 / 4] = (s16)((self->mBallSize - 0x4000) * 36 / 10 / 60 + 0x1333);
                sys[0x4c / 4] = (s16)((self->mBallSize - 0x4000) * 26 / 10 / 60 + 0x1666);
                sys[0x50 / 4] = (s16)((self->mBallSize - 0x4000) * 8 / 10 / 60 + 0x666);
            }
            sys = (int *)Particle::System::FromUniqueID(id2);
            if (sys != 0) {
                sys[0x48 / 4] = (s16)((self->mBallSize - 0x4000) * 62 / 10 / 60 + 0x1ccc);
                sys[0x50 / 4] = (s16)((self->mBallSize - 0x4000) * 45 / 10 / 60 + 0x1800);
            }
            if (self->mState == 1) {
                func_02012718(0x169, hit.x);
                if (self->mSoundPending == 1) {
                    func_02012718(0x16a, self->unk_b9e8);
                    self->mSoundPending = 0;
                }
            }
        }
    } else if (farHit == 1 && self->mState == 1) {
        func_02012718(0x168, hit.x);
    }

    k = 0;
    self->mSoundPending = 0;
    push[0] = 0;
    push[1] = 0;
    do {
        if (self->mProbePush[k] == 1) {
            rotPush[0] = 0;
            rotPush[1] = 0x1000;
            func_0203d388(rotPush, (s16)(k * 0x10000 / 32));
            push[0] -= rotPush[0];
            push[1] -= rotPush[1];
        }
        k = k + 1;
    } while (k < 0x20);

    if (push[0] == 0 && push[1] == 0)
        return;

    func_0203d434(push);
    sc = MUL12(self->mVelX, push[0]) + MUL12(self->mVelY, push[1]);
    if (self->mHitBreakable == 1) {
        sc = MUL12(sc, 0x1800);
        self->mVelX -= MUL12(sc, push[0]);
        self->mVelY -= MUL12(sc, push[1]);
        cosv = data_02082214[((unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(
            self->mVelY, self->mVelX) >> 4) * 2];
        vx = self->mVelX;
        self->mVelX = vx / 8 + MUL12(vx, cosv) / 4;
        vy = self->mVelY;
        self->mVelY = vy / 8 + MUL12(vy, cosv) / 4;
    } else {
        sc = MUL12(sc, 0x1200);
        self->mVelX -= MUL12(sc, push[0]);
        self->mVelY -= MUL12(sc, push[1]);
    }
}

#pragma opt_lifetimes off
// @symbol func_ov006_02126948
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02126948(char *scene) {
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int t = cstd::fdiv(0xc0000, data_02082314);
    struct V3 v;
    int x, y;
    if (self->mScreensSwapped == 0) {
        *(int *)(scene + 0x4700) = 0;
        *(int *)(scene + 0x4704) = 0xd0000;
        *(int *)(scene + 0x4708) = 0;
        *(int *)(scene + 0x470c) = 0;
        *(int *)(scene + 0x4710) = 0;
        *(int *)(scene + 0x4714) = t;
        *(short *)(scene + 0x4718) = 0x200;
        Camera_UpdateMatrices(scene + 0x4660);
    } else {
        *(int *)(scene + 0x4700) = 0;
        *(int *)(scene + 0x4704) = -0x60000;
        *(int *)(scene + 0x4708) = 0;
        *(int *)(scene + 0x470c) = 0;
        *(int *)(scene + 0x4710) = 0;
        *(int *)(scene + 0x4714) = t;
        *(short *)(scene + 0x4718) = 0x200;
        Camera_UpdateMatrices(scene + 0x4660);
    }
    y = self->mScrollY - self->mPosY + 0x110000;
    x = self->mPosX - 0x80000;
    v.x = x; v.y = y; v.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, x, y, 0);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, self->mHeadingAngle);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mRollAngle);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->unk_ab7a);
    *(struct M4x3Flat *)(scene + 0xabc0) = *(struct M4x3Flat *)&data_020a0e68;
}
}
#pragma opt_lifetimes on

// @symbol func_ov006_02126a98
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02126a98(char *scene)
{
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int flag = 1;
    int val = (self->mScrollY >> 12) / 16 - 2;
    int i;
    if (val < 0) val = 0;
    for (i = 0; i < 0x10; i++) {
        func_ov006_02126b4c(scene, val, flag);
        val++;
    }
    {
        int flag2 = 0;
        int k = flag2;
        int r8 = ((self->mScrollY >> 12) + 0x110) / 16 - 2;
        for (; flag2 < 0x10; flag2++) {
            if (r8 >= self->mScrollLimit) return;
            func_ov006_02126b4c(scene, r8, k);
            r8++;
        }
    }
}
}

#pragma opt_strength_reduction off
// @symbol func_ov006_02126b4c
/* BG2 tile-column writer.
 *
 * Copies one 16-row column of 2x2 BG tiles into the BG2 screen of both the
 * main and the sub engine. Each cell index comes from the u16 grid at
 * self+0x4f38 (row stride 0x5c0, column `col`); its four tiles are the four
 * u16 words of data_ov006_0212f3bc[idx], written with MultiStore16 (two
 * halfwords each) at screen offset ((col & 0xf) << 7) + 2*j, +2, +0x40,
 * +0x42. flag == 1 targets the first half of the screen block, anything else
 * the half at +0x800.
 *
 * Three spellings are load-bearing for the second loop's register colouring
 * (notes/mwccarm-codegen.md 6cd and the 6cu addendum): the two dead
 * assignments at the head of the first loop (`tile`, `row`), the dead
 * initialiser on `i`, and `off` declared as long rather than int. The sixteen
 * volatile u16 locals are the ROM's real stack traffic (each tile is stored
 * and reloaded around the screen-pointer call).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02126b4c(char *scene, int col, int flag)
{
    volatile u16 v0, v1, v2, v3, v4, v5, v6, v7;
    volatile u16 w0, w1, w2, w3, w4, w5, w6, w7;
    int n;
    int i = 0;
    u16 tile;
    char *row;
    int j;
    long off;
    int k842;
    int masked;
    u32 idx;
    volatile u16 *scr;

    if (flag == 1) {
        masked = col & 0xf;
        tile = *(u16 *)((char *)data_ov006_0212f3bc + 6);
        row = scene;
        i = 0;
        j = i;
        row = scene + (col << 1);
        off = masked << 7;
        n = 2;
        do {
            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v0 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v1 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v2 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
            scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + off);
            MultiStore16(v3 = tile, (char *)(scr + j + 0x21), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
            scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + off);
            MultiStore16(v4 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
            scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + off);
            MultiStore16(v5 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
            scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + off);
            MultiStore16(v6 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + 0x4f38);
            tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
            scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + off);
            MultiStore16(v7 = tile, (char *)(scr + j + 0x21), n);

            row += 0x5c0;
            j += 2;
            i += 1;
        } while (i < 0x10);
        return;
    }

    masked = col & 0xf;
    i = 0;
    j = i;
    row = scene + (col << 1);
    off = masked << 7;
    n = 2;
    k842 = 0x842;
    do {
        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x800 + off);
        MultiStore16(w0 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x802 + off);
        MultiStore16(w1 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + 0x840 + off);
        MultiStore16(w2 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
        scr = (volatile u16 *)(_ZN2G212GetBG2ScrPtrEv() + k842 + off);
        MultiStore16(w3 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3));
        scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + 0x800 + off);
        MultiStore16(w4 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 2);
        scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + 0x802 + off);
        MultiStore16(w5 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 4);
        scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + 0x840 + off);
        MultiStore16(w6 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + 0x4f38);
        tile = *(u16 *)((char *)data_ov006_0212f3bc + (idx << 3) + 6);
        scr = (volatile u16 *)(G2S::GetBG2ScrPtr() + k842 + off);
        MultiStore16(w7 = tile, (char *)(scr + j), n);

        row += 0x5c0;
        j += 2;
        i += 1;
    } while (i < 0x10);
}
}
#pragma opt_strength_reduction on

// @symbol func_ov006_02126ee4
/* The course generator.
 * Paints mTileMap row by row from the bottom of the course upwards, carving a
 * corridor between a left and a right edge cursor that random-walk one lane at
 * a time. Each row draws two kinds (0 hold, 1 pull in, 2 push out), clamps
 * them against the corridor width and against the dead zones at both ends of
 * the course, then paints solid 1 outside the corridor, a two-tile cap at each
 * wall and 0 in between. Obstacle tile 12 drops in on a cooldown and takes an
 * mArray1 slot with it. A second pass recolours the two goal rows, and a third
 * walks every solid tile to seed decoration tiles 0x1d..0x20 and the mArray2
 * props.
 *
 * FIVE SPELLINGS ARE LOAD-BEARING, all measured against the ROM on 2004/b56;
 * every one of them was worth whole blocks of registers, never one or two
 * instructions:
 *   - mArray1 and mArray2 carry their 8-byte stride in the TYPE (the header
 *     now spells them `s32 [0x80][2]`). Written as byte arrays with the
 *     stride in the expression, mwccarm strength-reduces `slot * 8` into a
 *     second induction variable, which costs the third pass the register the
 *     ROM keeps 0x7fff in and spills the constant 1 to the frame: +9 words.
 *   - `row` is declared AFTER rightKind. Declaration order picks the
 *     callee-saved register here: declared earlier it takes sb and rotates
 *     leftKind/rightKind/left/right one register each, 145 wrong words.
 *   - the goal-row pass, the prop pass and the first `left` fill each own
 *     their loop counter (goalRow/j, propRow/col, k). Sharing one `i` with
 *     the rest of the function swaps the counter and the derived row pointer
 *     between r1 and r2 in each of them.
 *   - the wide-corridor obstacle owns `wideLane` rather than sharing `lane`
 *     with the other two obstacle sites, which is what puts the lane in r2
 *     and the mArray1 scan counter in r1 the way the ROM has them.
 *   - `right - left` in the else arm stays an EXPRESSION. As a named local it
 *     is allocated a frame slot ahead of the three induction temporaries and
 *     every later slot shifts; the ROM spills it last, at sp+0x28.
 * The run of literal stores at sp+0x2c..0xc4 is not a table: it is mwccarm
 * hoisting one value per textual constant out of the row loop and spilling
 * all of them, which falls out of writing the constants inline. */
extern "C" void func_ov006_02126ee4(void *p_)
{
    dScMgSnowball_c *self = (dScMgSnowball_c *)p_;
    int k;
    int i;
    int left;
    int right;
    int leftKind;
    int rightKind;
    int row;
    int margin;
    int narrow;
    int wide;
    int prevLeft;
    int prevRight;
    int cooldown;
    int propRow;
    int lane;
    int slot;
    int roll;
    u16 tile;
    int goalRow;
    int col;
    int j;
    int wideLane;

    for (i = 0; i < 0x80; i++) {
        self->mArray1Active[i] = 0;
        self->mArray1Kind[i] = 0;
        self->mArray1Hit[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        self->mArray2Active[i] = 0;
    }
    self->unk_ab5c = 0;

    if (self->Virtual8C()) {
        narrow = 0xb;
        wide = 0xe;
        margin = 6;
    } else {
        narrow = 0xa;
        wide = 0xd;
        margin = 7;
    }

    rightKind = leftKind = 0;
    left = 2;
    right = 0xd;
    cooldown = 0x12;

    for (row = self->mScrollLimit - 1; row >= 0; row--) {
        prevLeft = leftKind;
        prevRight = rightKind;
        leftKind = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        rightKind = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;

        if (row >= self->mScrollLimit - 8) {
            rightKind = leftKind = 0;
        } else if (row <= self->mGoalY / 16) {
            rightKind = leftKind = 0;
        } else if (right - left <= narrow) {
            if (leftKind == 1) {
                leftKind = 0;
            }
            if (rightKind == 1) {
                rightKind = 0;
            }
        } else if (right - left == narrow + 1 && leftKind == 1 && rightKind == 1) {
            rightKind = leftKind = 0;
        } else if (right - left >= wide) {
            if (leftKind == 2) {
                leftKind = 0;
            }
            if (rightKind == 2) {
                rightKind = 0;
            }
        } else if (right - left == wide - 1 && leftKind == 2 && rightKind == 2) {
            rightKind = leftKind = 0;
        }

        switch (leftKind) {
        case 0:
            break;
        case 1:
            if (prevLeft == 2) {
                leftKind = 0;
            } else if (left >= margin) {
                if (prevLeft == 0) {
                    leftKind = 2;
                    left--;
                } else {
                    leftKind = 0;
                }
            }
            break;
        case 2:
            if (prevLeft == 1) {
                leftKind = 0;
            } else if (left < 1) {
                if (prevLeft == 0) {
                    leftKind = 1;
                } else {
                    leftKind = 0;
                }
            } else {
                left--;
            }
            break;
        default:
            leftKind = 0;
            break;
        }

        switch (rightKind) {
        case 0:
            break;
        case 1:
            if (prevRight == 2) {
                rightKind = 0;
            } else if (right <= 0x10 - margin) {
                if (prevRight == 0) {
                    rightKind = 2;
                    right++;
                } else {
                    rightKind = 0;
                }
            }
            break;
        case 2:
            if (prevRight == 1) {
                rightKind = 0;
            } else if (right >= 0xf) {
                if (prevRight == 0) {
                    rightKind = 1;
                } else {
                    rightKind = 0;
                }
            } else {
                right++;
            }
            break;
        default:
            rightKind = 0;
            break;
        }

        for (k = 0; k < left; k++) {
            self->mTileMap[k][row] = 1;
        }
        switch (leftKind) {
        case 0:
            self->mTileMap[left][row] = 2;
            self->mTileMap[left + 1][row] = 0;
            break;
        case 1:
            self->mTileMap[left][row] = 3;
            self->mTileMap[left + 1][row] = 4;
            break;
        case 2:
            self->mTileMap[left][row] = 5;
            self->mTileMap[left + 1][row] = 6;
            break;
        }
        for (i = left + 2; i < right - 1; i++) {
            self->mTileMap[i][row] = 0;
        }
        switch (rightKind) {
        case 0:
            self->mTileMap[right - 1][row] = 0;
            self->mTileMap[right][row] = 7;
            break;
        case 1:
            self->mTileMap[right - 1][row] = 8;
            self->mTileMap[right][row] = 9;
            break;
        case 2:
            self->mTileMap[right - 1][row] = 10;
            self->mTileMap[right][row] = 11;
            break;
        }
        for (i = right + 1; i < 0x10; i++) {
            self->mTileMap[i][row] = 1;
        }

        if (leftKind == 1) {
            left++;
        }
        if (rightKind == 1) {
            right--;
        }
        if (row == self->mGoalY / 16 - 1) {
            self->unk_ab5c = (right + left + 1) * 16 / 2 << 12;
        }

        if (row > self->mGoalY / 16 + 0x11) {
            if (cooldown > 0) {
                cooldown--;
            } else if (self->Virtual8C() && row < self->mScrollLimit / 2) {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15) == 0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (lane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) == 0) {
                        wideLane = left + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * (right - left - 3) >> 15) + 2;
                        self->mTileMap[wideLane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (wideLane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xa) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) == 0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i][0] = (lane * 16 + 8) << 12;
                                self->mArray1[i][1] = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (self->Virtual8C()) {
        for (i = 0; i < 0x80; i++) {
            if (self->mArray1Active[i] == 1) {
                if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) == 0) {
                    self->mArray1Kind[i] = 1;
                }
            }
        }
    }

    for (goalRow = self->mGoalY / 16 - 1; goalRow <= self->mGoalY / 16; goalRow++) {
        for (j = 0; j < 0x10; j++) {
            tile = self->mTileMap[j][goalRow];
            if (tile == 0) {
                self->mTileMap[j][goalRow] = 13;
            } else if (tile == 2) {
                self->mTileMap[j][goalRow] = 14;
            } else if (tile == 7) {
                self->mTileMap[j][goalRow] = 15;
            }
        }
    }

    slot = 0;
    for (propRow = self->mScrollLimit - 1; propRow >= 0; propRow--) {
        for (col = 0; col < 0x10; col++) {
            if (self->mTileMap[col][propRow] == 1) {
                roll = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
                if (roll < 4) {
                    self->mTileMap[col][propRow] = (u16)(roll + 0x1d);
                } else if (roll == 4) {
                    if (slot < 0x80) {
                        self->mArray2[slot][0] = (col * 16 + 8) << 12;
                        self->mArray2[slot][1] = (propRow * 16 + 8) << 12;
                        self->mArray2Active[slot] = 1;
                        self->mArray2Kind[slot] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15;
                        slot++;
                    }
                }
            }
        }
    }
}

// @symbol func_ov006_021279b0
extern "C" void func_ov006_021279b0(void *p_)
{
    char *c = (char *)p_;
    s32 tmp[3];
    int i;
    int y;
    u16 display;
    Pair *saved;

    if (((InitObject *)c)->query()) {
        I(0xba00) = 0x2dc0;
        I(0xba04) = 0x190;
        I(0xba08) = 0x2e0;
        I(0xb9dc) = 0x960;
    } else {
        I(0xba00) = 0x1740;
        I(0xba04) = 0x190;
        I(0xba08) = 0x178;
        I(0xb9dc) = 0x4b0;
    }

    display = *(volatile u16 *)0x04000304;
    *(volatile u16 *)0x04000304 = display & ~0x8000;
    B(0xb9f8) = 1;
    func_ov004_020b04d0(0x50);

    y = I(0xba00);
    I(0xab38) = 0x80000;
    I(0xab3c) = y << 12;
    I(0xab50) = I(0xab38);
    I(0xab54) = I(0xab3c);
    y = I(0xab3c);
    I(0xab68) = 0;
    I(0xab6c) = y - 0x190000;

    y = I(0xab6c) >> 12;
    SetBg2Offset(0x100, y);
    SetBg3Offset(0, I(0xab6c) >> 12);
    SetSubBg2Offset(0, (I(0xab6c) >> 12) + 0x110);
    SetSubBg3Offset(0, (I(0xab6c) >> 12) + 0x110);

    *(volatile u32 *)0x04000018 = 0x100 | (((I(0xab6c) >> 12) << 16) & 0x01ff0000);
    *(volatile u32 *)0x0400001c = ((I(0xab6c) >> 12) << 16) & 0x01ff0000;
    *(volatile u32 *)0x04001018 = (((I(0xab6c) >> 12) + 0x110) << 16) & 0x01ff0000;
    *(volatile u32 *)0x0400101c = (((I(0xab6c) >> 12) + 0x110) << 16) & 0x01ff0000;

    I(0xab40) = I(0xab38);
    I(0xab44) = I(0xab3c);
    saved = (Pair *)AT(c, 0xab40);
    I(0xab48) = saved->x;
    I(0xab4c) = saved->y;
    I(0xab60) = 0;
    I(0xab64) = 0;
    I(0xaba0) = 0x4000;
    H(0xab78) = 0;
    H(0xab7a) = 0;
    H(0xab7c) = 0;
    H(0xab7e) = 0;
    H(0xab80) = 0;
    H(0xab82) = 0;

    if (!(data_ov006_02143004 & 1)) {
        data_ov006_02143014[0] = 0;
        data_ov006_02143014[1] = 0x1000;
        data_ov006_02143014[2] = 0;
        func_020731dc(data_ov006_02143014, (void *)&_ZN7Vector3D1Ev,
                      &data_ov006_02143008);
        data_ov006_02143004 |= 1;
    }

    I(0xab84) = data_ov006_02143014[0];
    I(0xab88) = data_ov006_02143014[1];
    I(0xab8c) = data_ov006_02143014[2];
    func_0203ce80((struct Vector3 *)tmp, (struct Vector3 *)(c + 0xab84));

    I(0xab90) = data_02092768[0];
    I(0xab94) = data_02092768[1];
    I(0xab98) = data_02092768[2];
    I(0xab9c) = data_02092768[3];
    Quaternion_FromVector3((int *)(c + 0xab90), (struct Vector3 *)data_ov006_02143014,
                           (struct Vector3 *)(c + 0xab84));
    Quaternion_Normalize((s32 *)(c + 0xab90));

    func_ov006_02126948(c);
    for (i = 0; i < 0x20; i++) {
        char *p = c + i + 0xa000;
        *(u8 *)(p + 0xbf8) = 0;
        *(u8 *)(p + 0xc18) = 0;
        *(u8 *)(p + 0xc38) = 0;
    }

    I(0xb9e0) = 0;
    B(0xb9e4) = 0;
    B(0xb9e5) = 0;
    I(0xb9f4) = 0;
    I(0xab58) = 0;
    I(0xb9f0) = 0;

    func_ov006_0212a3c0(c);
    func_ov006_02125994(c);

    I(0xb9fc) = 0xf1;
    I(0xba0c) = 0;
    I(0xb9d8) = 0;
    B(0xb9e6) = 0;
    I(0xba10) = 0;
}

#pragma opt_strength_reduction off
#pragma opt_common_subs off
// @symbol _ZN15dScMgSnowball_c6RenderEv
/* Slot 9. `c` stays next to `self` because the two 0x80-entry arrays at
 * 0xacd8 and 0xb5d8 are raw bytes in the header (mArray1 and mArray2), so
 * those reads stay byte offsets. Both pragmas are needed. */
s32 dScMgSnowball_c::Render()
{
    char *c = (char *)this;
    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    int m[3];
    int vecArr[3];

    *(int*)((long long)(int)(c + 0xb9d8)) = *(int*)((long long)(int)(c + 0xb9d8)) + 1;
    if (self->mAnimCounter >= 0x20) {
        self->mAnimCounter = 0;
    }

    {
        int base = self->mGoalY;
        int t = (((self->mPosY >> 0xc) - base) * 0x7c) / (self->mStartY - base);
        func_ov004_020afdd0(data_ov006_02139d24[0], 0xf0, t + 0x22, -1, 0);
    }
    func_ov004_020afdd0(data_ov006_02139d24[1], 0xf0, 0x60, -1, 0);

    m[2] = 0xfffff008;
    m[1] = 0;
    m[0] = 0;
    func_0203cd80(m, -0x2000);
    *(volatile int*)0x040004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(int*)0x040004cc = 0x7fff;

    {
        int v = self->mBallSize;
        int t = v / 2 + v * 4;
        vecArr[0] = t;
        vecArr[1] = t;
        vecArr[2] = t;
        if (self->mBallSize > 0) {
            void *self = (void*)(c + 0xaba4);
            void (*fn)(void*, int*) = *(void(**)(void*, int*))((char*)(*(void**)self) + 0x14);
            fn(self, vecArr);
        }
    }

    func_ov006_02126a98(c);

    for (int i1 = 0; i1 < 0x80; i1++) {
        char *p = c + i1;
        if (*(u8*)(p + 0xac58) == 1) {
            int t6 = self->mScrollY;
            int t5 = *(int*)((long long)(int)(c + i1 * 8 + 0xacdc));
            if (t5 >= t6 - 0x20000 && t5 < t6 + 0x1a0000 + (func_ov004_020b04c0() << 0xc)) {
                if (*(int*)(c + i1 * 4 + 0xb0d8) == 1) {
                    int cnt = self->mAnimCounter;
                    int idx = (cnt / 4) & 7;
                    if (*(u8*)(p + 0xb2d8) == 1) idx += 8;
                    int a1 = (*(int*)(c + i1 * 8 + 0xacd8) - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02138d64[idx], a1, a2, -1, -1);
                } else {
                    int a1 = (*(int*)(c + i1 * 8 + 0xacd8) - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c24, a1, a2, -1, 2);
                }
            }
        }
    }

    for (int i2 = 0; i2 < 0x80; i2++) {
        if (*(u8*)(c + i2 + 0xb358) == 1) {
            int t7 = self->mScrollY;
            int t6 = *(int*)((long long)(int)(c + i2 * 8 + 0xb5dc));
            if (t6 >= t7 - 0x40000 && t6 < t7 + 0x1c0000 + (func_ov004_020b04c0() << 0xc)) {
                switch (*(int*)(c + i2 * 4 + 0xb3d8)) {
                case 0:
                case 1:
                case 2: {
                    int a1 = (*(int*)(c + i2 * 8 + 0xb5d8) - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139d18, a1, a2, -1, 1);
                    break;
                }
                case 3: {
                    int v = *(int*)(c + i2 * 8 + 0xb5d8);
                    int sel = (v < 0x80000) ? 1 : 0;
                    int a1 = (v - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c80[sel], a1, a2, -1, 1);
                    break;
                }
                }
            }
        }
    }

    {
        int a1 = (self->mDrawPosX - self->mScrollX) >> 0xc;
        int a2 = ((self->mDrawPosY - self->mScrollY) >> 0xc) - 0x110;
        func_ov004_020afdd0(data_ov006_02139c38, a1, a2, -1, 2);
    }

    {
        int t4 = self->mCountdownTimer;
        if (t4 <= 0xf0 && t4 > 0x3c) {
            int q = t4 / 60;
            if (q >= 4) q = 3;
            func_ov004_020b2220(0x80, 0x60, q, -1, -1, 0x800, 0);
        } else {
            int mode = self->mState;
            if (mode == 1 || mode == 2
                || (mode == 3 && self->mEndDelayTimer > 0)
                || (mode == 4 && self->mCrashTimer <= 0x3c)) {
                int v = self->mTimeLeft;
                int q60 = v / 60;
                int rem60 = v % 60;
                int r7 = rem60 * 100;
                int centis = r7 / 60;
                int r4;
                int tens;
                int ones;
                if (v < 0x258) {
                    r4 = 0xc;
                } else if (v >= 0x276) {
                    r4 = -0x104;
                } else {
                    r4 = ((0x258 - v) * 0x110) / 30 + 0xc;
                }
                func_ov004_020b2444(0x84, r4, q60, -1, -1, 1, 0);
                tens = centis / 10;
                func_ov004_020b2444(0x9c, r4, tens, -1, -1, 1, 0);
                ones = centis % 10;
                if (ones == 5 || ones == 0) {
                    func_ov004_020b2444(0xac, r4, ones, -1, -1, 1, 0);
                } else {
                    func_ov004_020b2444(0xac, r4, ones + 1, -1, -1, 1, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void**)((char*)data_ov006_0213fff0[idx] + 0xc), 0x5c, r4, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void**)((char*)data_ov006_0213fff0[idx] + 0x1c), 0x90, r4, 0);
                }
            }
        }
    }

    func_ov006_02129690(c);
    func_ov006_02125804(c);
    return 1;
}
#pragma opt_common_subs on
#pragma opt_strength_reduction on

// @symbol _ZN15dScMgSnowball_c8BehaviorEv
/* Slot 6. The five-state snowball roll: 0 counts in, 1 steers with the
 * touch screen and grows the ball, 2 and 3 are the crash, 4 is the melt and
 * 5 is over.
 *
 * The address macros and the `AT` and `LNDR` launders stop mwcc from sharing
 * field addresses across calls, which the ROM does not do; keep them. */
s32 dScMgSnowball_c::Behavior()
{
    char *c = (char *)this;
    int r5, r4;
    int idx, r3rec, r7, j;
    struct SPS s;
    int v, q, t;
    unsigned short m;
    int gate;
    int id2, id1;
    int *p;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    H(0xab7e) = HS(0xab78);
    H(0xab80) = HS(0xab7a);
    H(0xab82) = HS(0xab7c);
    I(0xab40) = I(0xab38);
    I(0xab44) = I(0xab3c);

    r4 = (I(0xba08) << 16) - 0x1d0000;
    r5 = I(0xab6c);

    switch (I(0xb9f4)) {
    case 0:
        IA(0xb9fc) -= 1;
        v = I(0xb9fc);
        if (v == 0x3c) {
            I(0xb9f4) = 1;
            func_ov004_020b0cac(0xc, 0x80, 0x60, 0, -1, 0xd);
            func_02012790(0x2a);
        } else if (v == 0x78 || v == 0xb4 || v == 0xf0) {
            func_02012790(0x2b);
        }
        break;

    case 1:
        if (I(0xb9fc) > 0) {
            IA(0xb9fc) -= 1;
            if (I(0xb9fc) == 0)
                FreeGfxSlotsById(0xc);
        }

        idx = data_020a0e40[0];
        r3rec = data_020a0de8[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (data_020a0de9[j] != 0);
        if (r7 != 0) {
            int vb = ((volatile unsigned char*)data_020a0deb)[(unsigned int)idx * 4];
            int va = ((volatile unsigned char*)data_020a0dea)[(unsigned int)idx * 4];
            I(0xab70) = va;
            I(0xab74) = vb;
        } else if (r3rec != 0) {
            s.a[0] = ((data_020a0dea[(unsigned int)idx * 4] - I(0xab70)) << 12) / 296;
            s.a[1] = ((data_020a0deb[(unsigned int)idx * 4] - I(0xab74)) << 12) / 296;
            I(0xab70) = data_020a0dea[(unsigned int)idx * 4];
            I(0xab74) = data_020a0deb[(unsigned int)idx * 4];
            if (Vec2_Len(s.a) >= 0x400) {
                s.a[0] = 0;
                s.a[1] = 0;
            }
            r7 = atan2(I(0xab64), I(0xab60));
            func_0203d388(s.a, (short)-r7);
            s.a[1] = s.a[1] << 2;
            func_0203d388(s.a, r7);
            if (I(0xab64) >= 0) {
                if (s.a[1] > 0)
                    s.a[1] = 0;
            }
            if (s.a[1] > 0) {
                if (s.a[1] >= -I(0xab64)) {
                    I(0xab64) = 0;
                    s.a[1] = 0;
                }
            }
            IA(0xab60) += s.a[0];
            IA(0xab64) += s.a[1];
        }

        if (I(0xab60) != 0 || I(0xab64) != 0) {
            if (Vec2_Len((int*)AT(c,0xab60)) >= 0x8000 && func_0203d434((int*)ATS(0xab60)) != 0)
                func_0203d630((int*)ATI(0xab60), 0x8000);
        }
        func_ov006_02125f68(c);

        IA(0xab38) += I(0xab60);
        IA(0xab3c) += I(0xab64);

        if (I(0xab3c) >= I(0xab44)) {
            IA(0xab58) += I(0xab3c) - I(0xab44);
        } else if (I(0xab58) > 0) {
            IA(0xab58) += I(0xab3c) - I(0xab44);
            if (I(0xab58) < 0) {
                IA(0xaba0) += (-I(0xab58)) / 128;
                I(0xab58) = 0;
            }
        } else {
            IA(0xaba0) += (I(0xab44) - I(0xab3c)) / 128;
        }

        if (I(0xaba0) >= 0x37000)
            I(0xaba0) = 0x37000;
        Vec2_Sub(s.b, (int*)AT(C1,0xab38), (int*)AT(c,0xab50));
        gate = (Vec2_Len(s.b) >= 0x30000) ? 1 : 0;
        if ((int)((long long)gate) != 0) {
            I(0xab50) = I(0xab38);
            I(0xab54) = I(0xab3c);
            if (I(0xaba0) < 0x10000)
                func_02012718(0x165, I(0xab38));
            else if (I(0xaba0) < 0x20000)
                func_02012718(0x166, I(0xab38));
            else
                func_02012718(0x167, I(0xab38));
        }

        I(0xab6c) = I(0xab3c) - 0x190000;
        if (I(0xab6c) > r5)
            I(0xab6c) = r5;
        if (I(0xab6c) < 0)
            I(0xab6c) = 0;
        if (I(0xab6c) >= r4)
            I(0xab6c) = r4;

        if (Vec2_Len((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = Vec2_Len((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            m = atan2(I(0xab60), I(0xab64));
            ApproachLinear((short*)AT(c,0xab7c), (short)m, (short)(Vec2_Len((int*)ATI(0xab60)) / 16));
        }

        if (B(0xb9e5) == 1 || I(0xb9dc) <= 0) {
            I(0xb9f0) = 1;
            I(0xb9f4) = 4;
            break;
        }
        IA(0xb9dc) -= 1;
        IA(0xb9e0) += 1;
        t = I(0xb9dc);
        if (t % 60 == 0) {
            func_02012790(0x2b);
        } else {
            if (t < 0x258 && t % 30 == 0)
                func_02012790(0x2b);
            else if (t < 0xb4 && t % 15 == 0)
                func_02012790(0x2b);
        }
        if (I(0xab3c) - I(0xaba0) < (I(0xba04) << 12) + 0x10000) {
            func_ov004_020adb1c(I(0xb9e0));
            I(0xb9f4) = 2;
            Sound::PlayBank2_2D(0x10e);
        }
        break;

    case 2:
        IA(0xba0c) += 1;
        if (I(0xba0c) >= 0x3c) {
            func_ov004_020b67e8(4);
            func_ov004_020b0a54(0x13);
            I(0xba10) = 0xb4;
            I(0xb9f4) = 3;
        }
        /* fall through */

    case 3:
        B(0xc3) = 0;
        if (I(0xba10) > 0)
            IA(0xba10) -= 1;
        idx = data_020a0e40[0];
        r3rec = data_020a0de8[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (data_020a0de9[j] != 0);
        if (r7 != 0)
            I(0xba10) = 0;
        func_ov006_02125f68(c);

        if (I(0xab60) != 0 || I(0xab64) != 0) {
            r7 = Vec2_Len((int*)AT(c,0xab60));
            if (I(0xab3c) < 0xc0000)
                r7 -= 0x80;
            if (r7 < 0) {
                I(0xab60) = 0;
                I(0xab64) = 0;
            } else if (func_0203d434((int*)ATS(0xab60)) != 0) {
                func_0203d630((int*)ATI(0xab60), r7);
            }
        }

        IA(0xab38) += I(0xab60);
        IA(0xab3c) += I(0xab64);
        if (I(0xab3c) < 0xe8000)
            B(0xb9f8) = 0;
        else
            B(0xb9f8) = 1;

        if (Vec2_Len((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = Vec2_Len((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            H(0xab7c) = atan2(I(0xab60), I(0xab64));
        }

        I(0xab6c) = I(0xab3c) - 0x190000;
        if (I(0xab6c) > r5)
            I(0xab6c) = r5;
        if (I(0xab6c) < 0)
            I(0xab6c) = 0;
        if (I(0xab6c) >= r4)
            I(0xab6c) = r4;
        break;

    case 4:
        if (I(0xba0c) < 0x3c) {
            if (I(0xab60) != 0 || I(0xab64) != 0) {
                r4 = Vec2_Len((int*)AT(c,0xab60)) + 0x100;
                if (r4 >= 0x8000)
                    r4 = 0x8000;
                if (func_0203d434((int*)ATS(0xab60)) != 0)
                    func_0203d630((int*)ATI(0xab60), r4);
            }
            IA(0xba0c) += 1;
        } else if (I(0xba0c) == 0x3c) {
            if (I(0xaba0) > 0) {
                IA(0xaba0) -= 0x1000;
                if (I(0xaba0) <= 0) {
                    int t1, t0;
                    I(0xaba0) = 0;
                    t1 = I(0xab6c) - I(0xab3c) + 0x110000;
                    t0 = I(0xab38) - 0x80000;
                    s.cc[0] = t0;
                    s.cc[1] = t1;
                    s.cc[2] = 0x20000;
                    id1 = pnew(0, 0xf1, s.cc[0] << 3, s.cc[1] << 3, s.cc[2] << 3, 0, 0);
                    id2 = pnew(0, 0xf2, s.cc[0] << 3, s.cc[1] << 3, s.cc[2] << 3, 0, 0);
                    p = pfromid(id1);
                    if (p != 0) {
                        p[0x11] = 0xffff8222;
                        p[0x12] = 0x2e14;
                        p[0x13] = 0x29cf;
                        p[0x14] = 0xc5f;
                    }
                    p = pfromid(id2);
                    if (p != 0) {
                        p[0x12] = 0x4b17;
                        p[0x14] = 0x3999;
                    }
                    func_02012718(0x16b, I(0xab38));
                    IA(0xba0c) += 1;
                }
            }
        } else {
            IA(0xba0c) += 1;
            if (I(0xba0c) >= 0x5a) {
                func_ov004_020b67e8(0xe);
                func_ov004_020b0a54(3);
                I(0xb9f4) = 5;
            }
        }

        if (Vec2_Len((int*)AT(c,0xab60)) >= 0x10) {
            short *hp;
            v = Vec2_Len((int*)ATS(0xab60));
            hp = (short*)AT(c,0xab78);
            q = __aeabi_idiv(v * 0x2710, I(0xaba0));
            *hp = *hp + (unsigned short)q;
            H(0xab7c) = atan2(I(0xab60), I(0xab64));
        }
        break;

    case 5:
        B(0xc3) = 0;
        break;
    }

    func_ov006_0212a2e0(c);
    func_ov006_02125890(c);
    func_ov006_02126948(c);
    return 1;
}

// @symbol _ZN15dScMgSnowball_c8OnKickedEv
/* OnKicked keeps its own `extern "C"` block and the one-letter `V` scroll
   macro, which is #undef'd at the end of the block. */

extern "C" {
/* dScMgSnowball_c::OnKicked, from its vtable slot. */

#define V (self->mScrollY >> 12)

int dScMgSnowball_c::OnKicked()
{
    char *c = (char *)this;

    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    if (self->mMenuOpen == 0) {
        if (self->mScreensSwapped == 0) {
            *(volatile unsigned short *)0x4000304 |= 0x8000;
            SetBg2Offset(0, V);
            SetBg3Offset(0, V);
            SetSubBg2Offset(0x100, V + 0x110);
            SetSubBg3Offset(0, V + 0x110);
            *(volatile int *)0x4000018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400001c = (V << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400101c = ((V + 0x110) << 16) & 0x1ff0000;
        } else {
            *(volatile unsigned short *)0x4000304 &= ~0x8000;
            SetBg2Offset(0x100, V + 0x110);
            SetBg3Offset(0, V + 0x110);
            SetSubBg2Offset(0, V);
            SetSubBg3Offset(0, V);
            *(volatile int *)0x4000018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400001c = ((V + 0x110) << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400101c = (V << 16) & 0x1ff0000;
        }
    }
    return ((dScMgBase_c *)c)->dScMgBase_c::OnKicked() != 0;
}
}
#undef V

// @symbol _ZN15dScMgSnowball_c8OnPushedEv
/* dScMgSnowball_c::OnPushed, from its vtable slot. */
int dScMgSnowball_c::OnPushed()
{
    void *t = (void *)this;
 return ((dScMgBase_c *)t)->dScMgBase_c::OnPushed() != 0; }

// @symbol _ZN15dScMgSnowball_c11OnAttacked2Ev
/* dScMgSnowball_c::OnAttacked2, from its vtable slot. */
int dScMgSnowball_c::OnAttacked2()
{
    void *t = (void *)this;
 return ((dScMgBase_c *)t)->dScMgBase_c::OnAttacked2() != 0; }

// @symbol _ZN15dScMgSnowball_c16CleanupResourcesEv
/* Slot 3. Frees the BMD that InitResources parked in unk_abf4. The field is
 * a file pointer but untyped in the header, so it is read through a cast. */
s32 dScMgSnowball_c::CleanupResources()
{
    Ov004_Deallocate(*(void **)&unk_abf4);
    return 1;
}

// @symbol _ZN15dScMgSnowball_c13OnYoshiTryEatEi
/* Slot 18, an override of dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgSnowball_c::OnYoshiTryEat(int i)
{
    void *c = (void *)this;

  func_ov004_020adb1c(0);
  func_ov006_021279b0(c);
  if(i != 0x13) return;
  func_ov006_02126ee4(c);
  func_ov006_02126a98((char *)c);
}

// @symbol _ZN15dScMgSnowball_c13InitResourcesEv
/* Slot 0. Sets up both screens' BG2 and BG3 layers, loads the palettes and
 * tiles, then loads the snowball BMD into unk_abf4 and hands it to mModel.
 * Returning 0 when ModelBase::SetFile fails aborts the scene. The helpers at
 * the end take the scene as an opaque pointer, so the receiver stays raw. */
s32 dScMgSnowball_c::InitResources()
{
    void *arg0 = this;
    u8 *r4 = (u8 *)arg0;
    void *buf;

    data_0208ee44 = 1;
    *(vu16 *)0x4000304 &= ~0x8000;
    *(r4 + 0xb9f8) = 1;

    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & 0x43) | 0x5214;
    *(vu16 *)0x400000c &= ~0x40;
    *(vu32 *)0x4000018 = 0;
    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & ~3) | 2;

    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & 0x43) | 0x5214;
    *(vu16 *)0x400100c &= ~0x40;
    *(vu32 *)0x4001018 = 0;
    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & ~3) | 2;

    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & 0x43) | 0x1614;
    *(vu16 *)0x400000e &= ~0x40;
    *(vu32 *)0x400001c = 0;
    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & ~3) | 3;

    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & 0x43) | 0x1614;
    *(vu16 *)0x400100e &= ~0x40;
    *(vu32 *)0x400101c = 0;
    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & ~3) | 3;

    LoadCompressedFileAt(0xad, (u8 *)_ZN2G213GetBG2CharPtrEv() + 0x4000);
    LoadCompressedFileAt(0xad, (void *)(G2S::GetBG2CharPtr() + 0x4000));
    LoadCompressedFileAt(0xac, _ZN2G212GetBG3ScrPtrEv());
    LoadCompressedFileAt(0xac, G2S::GetBG3ScrPtr());

    data_0209d45c |= 0xc;
    data_0209d454 |= 0xc;

    buf = func_ov004_020adc74(&data_ov006_0214009c);
    CP15::FlushAndInvalidateDataCache((u32)buf, 0x100);
    GX::LoadBGPltt(buf, 0x100, 0x100);
    GXS::LoadBGPltt(buf, 0x100, 0x100);
    Ov004_Deallocate(buf);

    buf = func_ov004_020adc74(&data_ov006_021400bc);
    DecompressLZ16(buf, (void *)0x6400000);
    DecompressLZ16(buf, (void *)0x6600000);
    Ov004_Deallocate(buf);

    buf = func_ov004_020adc74(&data_ov006_021400dc);
    CP15::FlushAndInvalidateDataCache((u32)buf, 0x100);
    GX::LoadOBJPltt(buf, 0, 0x100);
    GXS::LoadOBJPltt(buf, 0, 0x100);
    Ov004_Deallocate(buf);

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    G3X::SetFog(false, 0, 2, 0x1000);
    *(vu16 *)0x4000060 = (*(vu16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();

    *(void **)(r4 + 0xabf4) = func_ov004_020adc74(&data_ov006_021400fc);
    Model::UpdateFileOffsets(**(BMD_File **)(r4 + 0xabf4));

    if (((ModelBase *)(r4 + 0xaba4))->SetFile(*(BMD_File **)(r4 + 0xabf4), 1, -1) == 0)
        return 0;

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    data_0209d45c |= 1;

    func_ov006_021279b0(arg0);
    func_ov006_02126ee4(arg0);
    func_ov006_02126a98((char *)arg0);
    return 1;
}
