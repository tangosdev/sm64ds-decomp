//cpp
/* dScMgSnowball_c -- the snowball-rolling minigame scene: 43 functions
 * (.text 0x0212568c..0x0212a554), the destructor, eight virtual overrides, the
 * constructor and the helpers they call, including the per-element helpers for
 * the 0x32-element array at this+0xbe94, and last the factory
 * dScMgSnowball_c_classInit (0x0212a520).
 * The constructor (0x021295ac) and the 18 functions after it were separate
 * one-function files (the constructor through Bit_InitAll).
 *
 * _ZTS15dScMgSnowball_c 0x0213ffdc. _ZTI15dScMgSnowball_c 0x0213ffd0.
 * _ZTV15dScMgSnowball_c 0x0214000c. The destructor is the key function:
 * D1 at 0x0212568c, D0 at 0x0212573c.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; removing it
 * breaks five functions and the section order. The six bracketed optimizer
 * pragmas are needed too, and so is the include order (see below).
 *
 * The helpers whose first parameter was the scene are now members of
 * dScMgSnowball_c, including the 17 per-element helpers dispatched through
 * the PMF tables sSnowballBitStateTable/38/50/70 (8-byte {function, 0}
 * records). __sinit_d_s_mg_snowball.cpp copies them. Some calls stay mangled:
 * cstd::atan2 and Particle::System take Fix12 or reference
 * arguments. GetBG2CharPtr and GetBG3ScrPtr stay spelled.
 */

// leftovers:
// - Snowball_UpdateCamera stays a free function. Inside its `opt_lifetimes
//   off` window the member spelling reserves six extra stack slots (frame
//   0x10 -> 0x28, v leaves sp+0) under every alias shape tried, so it keeps
//   its char* parameter and C linkage (caller: Behavior's per-element
//   dispatch in this TU).
// - dScMgSnowball_c::Vec2, the collision ring's hit pair, stays its own nested
//   type with a destructor and no constructor (see the header); it was not
//   swapped for Vector2 in the pass that typed mArray1/mArray2.
// - The raw-pointer puns (the AT()/ATS()/ATI() launders) are load-bearing: the
//   0xa000..0xc000 field offsets exceed the +/-0xfff store immediate, so the
//   compiler's own pointer arithmetic has to stand in for them. Sites
//   whose member spelling was tried and moved bytes keep `pointer + offset`
//   arithmetic, with the offset named through SNOW_OFF and a note saying so.
// - The r4/r5/r7 locals in the render-setup helpers are named for the
//   registers they color; renaming them needs the coloring comments updated
//   in the same pass.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgSnowball_c_AssetId {
    ASSET_D_2D_MG_BG_YUKA_NSC_BIN = 0x00ac,
    ASSET_D_2D_MG_BG_YUKIDAMA_NCG_BIN = 0x00ad
};

#pragma defer_codegen off

/* Include order is load-bearing. dScMgSnowball_c.h reaches math/Matrix.h,
   which defines Matrix4x3 as {Matrix3x3 r; Vector3 t;}; common.h carries a
   second, flat `s32 m[12]` spelling of the same 0x30-byte type behind the
   same guard, and whichever a TU sees first stands. The nested spelling wins
   here, so the one copy that has to be a block move goes through M4x3Flat,
   below, rather than member-wise through Vector3's destructor. */
#include "dScMgSnowball_c.h"
#include "G2.h"
#include "SpawnInfo.h"
#include "types.h"
#include "CP15.h"
#include "GX.h"
#include "cstd.h"
#include "decl_common.h"
#include "common.h"
#include "Sound.h"
#include "dScMgBase_c.h"
#include "Particle__System.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"

/* Declarations the recovered sources need that no project header supplies.
 * Each one was checked against include/*.h first; these are the residue. */
typedef struct Vec3 {
    int x, y, z;
} Vec3;

struct V3 {
    int x, y;
    volatile int z;
};

/* The FLAT 12-word view of Matrix4x3. include/common.h and include/math/Matrix.h
   carry two spellings of this 0x30-byte type and whichever a TU sees first
   stands; as one-function files these members saw common.h's flat `s32 m[12]`,
   but this TU reaches math/Matrix.h first through dScMgSnowball_c.h -> dExtSimpleModel_c.h,
   and the nested {Matrix3x3 r; Vector3 t;} spelling copies member-wise because
   Vector3 has a user-declared destructor -- 0x20 bytes more than the ROM has.
   Copying through this view restores the cartridge's block copy. */
struct M4x3Flat {
    s32 m[12];
};

typedef struct Pair {
    s32 x;
    s32 y;
} Pair;

struct SPS {
    int a[2];
    int b[2];
    int cc[3];
};

#define AT(p, o) ((void *)(int)((char *)(p) + (o)))
#define LNDR(e) ((int)((long long)(e)))
#define C1 ((char*)(void*)(int)(c))
#define C2 ((char*)(void*)(int)(C1))
#define ATS(off) AT(C1,off)
#define ATI(off) AT(C2,off)
/* Byte offset of a dScMgSnowball_c member, for the sites that must keep
   `pointer + constant` arithmetic because every member spelling moved bytes. */
#define SNOW_OFF(m) ((int)&((dScMgSnowball_c *)0)->m)
#define pnew Particle::System::New
#define pfromid(id) ((int *)Particle::System::FromUniqueID(id))

namespace G3X {
void SetFog(int enable, int blend, int slope, int offset);
}

extern "C" {
extern void RenderOamBothScreens(int a, int b, int c, int d, int e, int f);
extern int sSnowballBitSprites[];
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *dst);
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *p);
extern int Vec2_Len(int *p);
extern void func_0203d630(int *p, int m);
extern u32 d_s_mg_snowballNewHold(u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir,
                                  void *callback);
extern void func_02012718(int a, int b);
extern int atan2_hold_d_s_mg_snowball_1(int y, int x);
extern s16 data_02082214[];
extern void Camera_UpdateMatrices(void *self);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern struct Matrix4x3 data_020a0e68;
extern void MultiStore16(int val, char *dst, int n);
extern u16 sSnowballCourseTiles[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void Mg_SetTopScreenYOffset(int);
extern void SetBg2Offset(int, int);
extern void SetBg3Offset(int, int);
extern void SetSubBg2Offset(int, int);
extern void SetSubBg3Offset(int, int);
extern void Quaternion_Normalize(s32 *);
extern s32 data_02092768[4];
void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void Mg_DrawNumberRotated(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
s32 GetGameLanguage(void);
void FreeGfxSlotsById(int arg);
void Vec2_Sub(int *o, int *a, int *b);
extern "C" void Ov004_Deallocate(void *p);
u32 LoadCompressedFileAt(u16 fileID, void *target);
void *Ov004_LoadFileByPath(void *p);
void DecompressLZ16(void *src, void *dst);
extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
/* Where two recovered declarations disagreed, the more precise one is
  * kept. */
}

// @symbol _ZN15dScMgSnowball_cD1Ev
/* Empty: every call in the ROM's body is compiler-generated. The members go
   in reverse declaration order -- __cxa_vec_cleanup on mSprays (with
   SnowSpray's destructor), mArray2 and mArray1 (with Vector2's,
   NullDestructor_0203d47c), then ~dExtSimpleModel_c on mModel -- and the rest
   is the inlined dScMgSingle3DBase_c destructor. */
dScMgSnowball_c::~dScMgSnowball_c() {}

// @symbol _ZN15dScMgSnowball_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

// @symbol _ZN9SnowSprayD1Ev
/* SnowSpray's empty inline destructor (dScMgSnowball_c.h), emitted here. */

// @symbol _ZN15dScMgSnowball_c17SnowSpray_DrawAllEv
void dScMgSnowball_c::SnowSpray_DrawAll() {
    char *scene = (char *)this;
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int i;
    char *p = scene;
    for (i = 0; i < 0x20; i++) {
        if (((dScMgSnowball_c *)p)->mSprays[0].active != 0) {
            RenderOamBothScreens(
                sSnowballBitSprites[((dScMgSnowball_c *)p)->mSprays[0].sprite],
                ((dScMgSnowball_c *)p)->mSprays[0].x >> 0xc,
                ((((((dScMgSnowball_c *)p)->mSprays[0].y - ((dScMgSnowball_c *)p)->mSprays[0].z) -
                   self->mScrollY) >>
                  0xc) -
                 0x110),
                -1, 0, 0);
        }
        p += 0x24;
    }
}

// @symbol _ZN15dScMgSnowball_c19SnowSpray_UpdateAllEv
void dScMgSnowball_c::SnowSpray_UpdateAll() {
    char *scene = (char *)this;
    int i;
    char *rec = scene;
    char *pos = (char *)&mSprays[0].x;
    char *vel = (char *)&mSprays[0].vx;
    for (i = 0; i < 0x20; i++, rec += 0x24, pos += 0x24, vel += 0x24) {
        if (((dScMgSnowball_c *)rec)->mSprays[0].active) {
            if (((dScMgSnowball_c *)rec)->mSprays[0].life > 0) {
                /* mSprays[0].life and .vz keep `rec + offset` arithmetic (the
                   (int) launder included): their member spellings move bytes. */
                *(int *)(((int)rec + SNOW_OFF(mSprays[0].life))) -= 1;
                if (*(int *)(rec + SNOW_OFF(mSprays[0].life)) <= 0) {
                    ((dScMgSnowball_c *)scene)->mSprays[i].active = 0;
                    return;
                }
            }
            *(int *)(((int)rec + SNOW_OFF(mSprays[0].vz))) -= 0x200;
            AddVec3((Vector3 *)pos, (Vector3 *)vel, (Vector3 *)pos);
            if (((dScMgSnowball_c *)rec)->mSprays[0].vz < 0 &&
                ((dScMgSnowball_c *)rec)->mSprays[0].z < 0) {
                ((dScMgSnowball_c *)scene)->mSprays[i].active = 0;
                return;
            }
        }
    }
}

// @symbol _ZN15dScMgSnowball_c18SnowSpray_ClearAllEv
void dScMgSnowball_c::SnowSpray_ClearAll() {
    char *rec = (char *)this;
    int i;
    for (i = 0; i < 0x20; i++) {
        ((dScMgSnowball_c *)rec)->mSprays[0].x = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].y = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].z = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].vx = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].vy = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].vz = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].life = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].active = 0;
        ((dScMgSnowball_c *)rec)->mSprays[0].sprite = 0;
        rec += 0x24;
    }
}

// @symbol _ZN15dScMgSnowball_c9IsInWaterEPi
int dScMgSnowball_c::IsInWater(int *point) {
    char *scene = (char *)this;
    int x = point[0];
    int y;
    int pixelX;
    int pixelY;
    int lane;
    int courseRow;
    unsigned int tile;
    int diagonalSum;
    int diagonalDifference;

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
    if (y >= (mScrollLimit << 16)) {
        return 0;
    }
    if (y >= (mScrollY + 0x1d0000)) {
        return 0;
    }

    pixelX = x >> 12;
    pixelY = y >> 12;
    lane = pixelX / 16;
    courseRow = pixelY / 16;
    tile = mTileMap[lane][courseRow];
    diagonalSum = (pixelX & 0xf) + (pixelY & 0xf);
    diagonalDifference = (pixelX & 0xf) - (pixelY & 0xf);

    switch (tile) {
    case 0:
    case 0xd:
        return 0;
    case 0x10:
        return 1;
    case 0x11:
    case 0x1b:
        return (pixelX & 8) == 0;
    case 0x12:
        return diagonalSum < 0x18;
    case 0x13:
        return diagonalSum < 8;
    case 0x14:
        return diagonalDifference < 8;
    case 0x15:
        return diagonalDifference < -8;
    case 0x16:
    case 0x1c:
        return (pixelX & 8) != 0;
    case 0x17:
        return diagonalDifference >= 8;
    case 0x18:
        return diagonalDifference >= -8;
    case 0x19:
        return diagonalSum >= 0x18;
    case 0x1a:
        return diagonalSum >= 8;
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

#pragma opt_strength_reduction off
// @symbol _ZN15dScMgSnowball_c13HitsBreakableEPi
int dScMgSnowball_c::HitsBreakable(int *point) {
    char *scene = (char *)this;
    int i;
    for (i = 0; i < 0x80; i++) {
        if (((dScMgSnowball_c *)(scene + i))->mArray1Active[0] == 1) {
            char *prop = scene + i * 8;
            int dx, dy;
            /* mArray1 member spellings here grow the function by 8 bytes, so
               these reads keep `prop + offset` */
            if (point[0] >= *(int *)(prop + SNOW_OFF(mArray1[0].x)) + 0x10000)
                continue;
            if (point[0] < *(int *)(prop + SNOW_OFF(mArray1[0].x)) - 0x10000)
                continue;
            if (point[1] >= *(int *)(prop + SNOW_OFF(mArray1[0].y)) + 0x8000)
                continue;
            if (point[1] < *(int *)(prop + SNOW_OFF(mArray1[0].y)) - 0x8000)
                continue;
            dy = *(int *)(prop + SNOW_OFF(mArray1[0].y)) - point[1];
            dx = *(int *)(prop + SNOW_OFF(mArray1[0].x)) - point[0];
            if (FX_MUL(dx, dx) + FX_MUL(dy, dy) / 4 > 0x100000)
                continue;
            ((dScMgSnowball_c *)(scene + i))->mArray1Hit[0] = 1;
            if (mArray1Kind[i] == 1) {
                mSoundPending = 1;
                mHitSoundX = ((dScMgSnowball_c *)prop)->mArray1[0].x;
                mHitSoundY = ((dScMgSnowball_c *)prop)->mArray1[0].y;
            }
            return 1;
        }
    }
    return 0;
}
#pragma opt_strength_reduction on

#pragma opt_common_subs off
// @symbol _ZN15dScMgSnowball_c8HitsWallEPi
int dScMgSnowball_c::HitsWall(int *point) {
    int raw = (int)this;
    char *scene = (char *)raw;
    int courseY = point[1];
    int x;
    unsigned short tile;
    int diagonalSum, diagonalDifference, localX, localY;

    if (courseY < -0x20000)
        return 1;
    if (courseY >= ((dScMgSnowball_c *)scene)->mScrollLimit << 16)
        return 1;
    if (courseY >= ((dScMgSnowball_c *)scene)->mScrollY + 0x1d0000)
        return 1;
    x = point[0];
    if (x < 0)
        return 1;
    if (x >= 0x100000)
        return 1;
    if (courseY < 0)
        tile = ((dScMgSnowball_c *)scene)->mTileMap[(x >> 12) / 16][0];
    else {
        int lane = (x >> 12) / 16;
        lane = lane ? lane : lane;
        tile =
            ((dScMgSnowball_c *)(raw + lane * 0x5c0 + ((courseY >> 12) / 16) * 2))->mTileMap[0][0];
    }
    localY = (courseY >> 12) & 0xf;
    localX = (x >> 12) & 0xf;
    diagonalSum = localX + localY;
    diagonalDifference = localX - localY;
    switch (tile) {
    case 0:
    case 13:
        return 0;
    case 1:
        return 1;
    case 2:
    case 14:
        return ((x >> 12) & 8) == 0;
    case 3:
        return diagonalSum < 0x18;
    case 4:
        return diagonalSum < 8;
    case 5:
        return diagonalDifference < 8;
    case 6:
        return diagonalDifference < -8;
    case 7:
    case 15:
        return ((x >> 12) & 8) != 0;
    case 8:
        return diagonalDifference >= 8;
    case 9:
        return diagonalDifference >= -8;
    case 10:
        return diagonalSum >= 0x18;
    case 11:
        return diagonalSum >= 8;
    case 12:
        return 0;
    default:
        return 0;
    }
}
#pragma opt_common_subs on

// @symbol _ZN15dScMgSnowball_c14UpdateSnowballEv
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
void dScMgSnowball_c::UpdateSnowball() {
    char *p_ = (char *)this;
    dScMgSnowball_c *self = (dScMgSnowball_c *)p_;
    int i;
    int k;
    int fastWallHit;
    int anyHit;
    int startedInWater;
    int resolvePass;
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
    int response[2];
    int hitNormal[2];
    int waterNormal[2];
    int push[2];
    int rotPush[2];
    int startPoint[2];
    int wallProbe[2];
    int breakableProbe[2];
    int waterProbe[2];
    Vector3 part;
    int allWater;
    int len;
    Particle::System *sys;
    u32 id1;
    u32 id2;
    int headingComponent;
    int normalVelocity;
    int vx;
    int vy;

    i = 0;
    do {
        self->mProbeHit[i] = 0;
        self->mProbePush[i] = 0;
        self->mProbeWater[i] = 0;
        i = i + 1;
    } while (i < 0x20);
    fastWallHit = 0;
    self->mHitBreakable = 0;

    startPoint[0] = self->mPosX;
    startPoint[1] = self->mPosY;
    startedInWater = IsInWater(startPoint);

    resolvePass = 0;
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
            wallProbe[0] = sample[0];
            wallProbe[1] = sample[1];
            if (HitsWall(wallProbe) != 0) {
                self->mProbeHit[j] = 1;
                self->mProbePush[j] = 1;
                anyHit = 1;
                if (Vec2_Len(&self->mVelX) >= 0x800)
                    fastWallHit = 1;
                hit.x = sample[0];
                hit.y = sample[1];
            } else {
                breakableProbe[0] = sample[0];
                breakableProbe[1] = sample[1];
                if (HitsBreakable(breakableProbe) != 0) {
                    self->mProbeHit[j] = 1;
                    anyHit = 1;
                    self->mProbePush[j] = 1;
                    self->mHitBreakable = 1;
                    hit.x = sample[0];
                    hit.y = sample[1];
                }
            }
            waterProbe[0] = sample[0];
            waterProbe[1] = sample[1];
            if (IsInWater(waterProbe) != 0)
                self->mProbeWater[j] = 1;
            else
                self->mProbeWater[j] = zDry;
            angle = (s16)(angle + 0x800);
        }

        response[0] = zOrigin;
        response[1] = zOrigin;
        angle = zOrigin;
        if (anyHit != 0) {
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeHit[i] == 1) {
                    hitNormal[0] = pushDirX;
                    hitNormal[1] = pushDirY;
                    func_0203d388(hitNormal, angle);
                    response[0] += hitNormal[0];
                    response[1] += hitNormal[1];
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(response);
            if (Vec2_Len(&self->mVelX) < 0x800)
                func_0203d630(response, Vec2_Len(&self->mVelX));
            else
                func_0203d630(response, pushDirY);
            self->mPosX -= response[0];
            self->mPosY -= response[1];
        } else if (startedInWater == 1) {
            allWater = 1;
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeWater[i] == 1) {
                    waterNormal[0] = floatDirX;
                    waterNormal[1] = floatDirY;
                    func_0203d388(waterNormal, angle);
                    response[0] += waterNormal[0];
                    response[1] += waterNormal[1];
                } else {
                    allWater = zNotAllWater;
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(response);
            self->mVelX += response[0];
            self->mVelY += response[1];
            if (allWater == 1)
                self->mAllWater = 1;
        }
        resolvePass = resolvePass + 1;
    } while (anyHit == 1 && resolvePass < 0x21);

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
            id1 = Particle::System::New(0, 0xf1, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            id2 = Particle::System::New(0, 0xf2, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            sys = Particle::System::FromUniqueID(id1);
            if (sys != 0) {
                /* 0x44 lies in Particle::System's unnamed pad_042. */
                ((int *)sys)[0x44 / 4] = (s16)((self->mBallSize - 0x4000) * 11 / 60 + 0x3000);
                sys->callbackParam = (s16)((self->mBallSize - 0x4000) * 36 / 10 / 60 + 0x1333);
                sys->callbackVelocity = (s16)((self->mBallSize - 0x4000) * 26 / 10 / 60 + 0x1666);
                sys->callbackScale = (s16)((self->mBallSize - 0x4000) * 8 / 10 / 60 + 0x666);
            }
            sys = Particle::System::FromUniqueID(id2);
            if (sys != 0) {
                sys->callbackParam = (s16)((self->mBallSize - 0x4000) * 62 / 10 / 60 + 0x1ccc);
                sys->callbackScale = (s16)((self->mBallSize - 0x4000) * 45 / 10 / 60 + 0x1800);
            }
            if (self->mState == 1) {
                func_02012718(0x169, hit.x);
                if (self->mSoundPending == 1) {
                    func_02012718(0x16a, self->mHitSoundX);
                    self->mSoundPending = 0;
                }
            }
        }
    } else if (fastWallHit == 1 && self->mState == 1) {
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
    normalVelocity = FX_MUL(self->mVelX, push[0]) + FX_MUL(self->mVelY, push[1]);
    if (self->mHitBreakable == 1) {
        normalVelocity = FX_MUL(normalVelocity, 0x1800);
        self->mVelX -= FX_MUL(normalVelocity, push[0]);
        self->mVelY -= FX_MUL(normalVelocity, push[1]);
        headingComponent =
            data_02082214[((unsigned short)cstd::atan2(self->mVelY, self->mVelX) >> 4) * 2];
        vx = self->mVelX;
        self->mVelX = vx / 8 + FX_MUL(vx, headingComponent) / 4;
        vy = self->mVelY;
        self->mVelY = vy / 8 + FX_MUL(vy, headingComponent) / 4;
    } else {
        normalVelocity = FX_MUL(normalVelocity, 0x1200);
        self->mVelX -= FX_MUL(normalVelocity, push[0]);
        self->mVelY -= FX_MUL(normalVelocity, push[1]);
    }
}

#pragma opt_lifetimes off
// @symbol Snowball_UpdateCamera
extern "C" {
void Snowball_UpdateCamera(char *scene) {
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int t = cstd::fdiv(0xc0000, data_02082314);
    struct V3 v;
    int x, y;
    /* pad_4660 is the head of the base's 0xbc camera (dScMgSingle3DBase_c.h). */
    if (self->mScreensSwapped == 0) {
        self->mCameraEyeX = 0;
        self->mCameraEyeY = 0xd0000;
        self->mCameraEyeZ = 0;
        self->mCameraTargetX = 0;
        self->mCameraTargetY = 0;
        self->mCameraTargetZ = t;
        self->mCameraAngle = 0x200;
        Camera_UpdateMatrices((char *)self->pad_4660);
    } else {
        self->mCameraEyeX = 0;
        self->mCameraEyeY = -0x60000;
        self->mCameraEyeZ = 0;
        self->mCameraTargetX = 0;
        self->mCameraTargetY = 0;
        self->mCameraTargetZ = t;
        self->mCameraAngle = 0x200;
        Camera_UpdateMatrices((char *)self->pad_4660);
    }
    y = self->mScrollY - self->mPosY + 0x110000;
    x = self->mPosX - 0x80000;
    v.x = x;
    v.y = y;
    v.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, x, y, 0);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, self->mHeadingAngle);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mRollAngle);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    *(struct M4x3Flat *)&self->mModel.mat4x3 = *(struct M4x3Flat *)&data_020a0e68;
}
}
#pragma opt_lifetimes on

// @symbol _ZN15dScMgSnowball_c12RedrawCourseEv
void dScMgSnowball_c::RedrawCourse() {
    char *scene = (char *)this;
    dScMgSnowball_c *self = (dScMgSnowball_c *)scene;
    int flag = 1;
    int val = (self->mScrollY >> 12) / 16 - 2;
    int i;
    if (val < 0)
        val = 0;
    for (i = 0; i < 0x10; i++) {
        DrawCourseColumn(val, flag);
        val++;
    }
    {
        int flag2 = 0;
        int k = flag2;
        int r8 = ((self->mScrollY >> 12) + 0x110) / 16 - 2;
        for (; flag2 < 0x10; flag2++) {
            if (r8 >= self->mScrollLimit)
                return;
            DrawCourseColumn(r8, k);
            r8++;
        }
    }
}

#pragma opt_strength_reduction off
// @symbol _ZN15dScMgSnowball_c16DrawCourseColumnEii
/* BG2 tile-column writer.
 *
 * Copies one 16-row column of 2x2 BG tiles into the BG2 screen of both the
 * main and the sub engine. Each cell index comes from the u16 grid at
 * self+0x4f38 (row stride 0x5c0, column `col`); its four tiles are the four
 * u16 words of sSnowballCourseTiles[idx], written with MultiStore16 (two
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
void dScMgSnowball_c::DrawCourseColumn(int col, int flag) {
    char *scene = (char *)this;
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
        tile = *(u16 *)((char *)sSnowballCourseTiles + 6);
        row = scene;
        i = 0;
        j = i;
        row = scene + (col << 1);
        off = masked << 7;
        n = 2;
        /* every member spelling of these mTileMap reads shrinks the function,
           so they keep the `row + offset` arithmetic, named through SNOW_OFF */
        do {
            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3));
            scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + off);
            MultiStore16(v0 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 2);
            scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + off);
            MultiStore16(v1 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 4);
            scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + off);
            MultiStore16(v2 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 6);
            scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + off);
            MultiStore16(v3 = tile, (char *)(scr + j + 0x21), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3));
            scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + off);
            MultiStore16(v4 = tile, (char *)(scr + j), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 2);
            scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + off);
            MultiStore16(v5 = tile, (char *)(scr + j + 1), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 4);
            scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + off);
            MultiStore16(v6 = tile, (char *)(scr + j + 0x20), n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 6);
            scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + off);
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
        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3));
        scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + 0x800 + off);
        MultiStore16(w0 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 2);
        scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + 0x802 + off);
        MultiStore16(w1 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 4);
        scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + 0x840 + off);
        MultiStore16(w2 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 6);
        scr = (volatile u16 *)((char *)G2::GetBG2ScrPtr() + k842 + off);
        MultiStore16(w3 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3));
        scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + 0x800 + off);
        MultiStore16(w4 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 2);
        scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + 0x802 + off);
        MultiStore16(w5 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 4);
        scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + 0x840 + off);
        MultiStore16(w6 = tile, (char *)(scr + j), n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = *(u16 *)((char *)sSnowballCourseTiles + (idx << 3) + 6);
        scr = (volatile u16 *)((char *)G2S::GetBG2ScrPtr() + k842 + off);
        MultiStore16(w7 = tile, (char *)(scr + j), n);

        row += 0x5c0;
        j += 2;
        i += 1;
    } while (i < 0x10);
}
#pragma opt_strength_reduction on

// @symbol _ZN15dScMgSnowball_c11BuildCourseEv
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
void dScMgSnowball_c::BuildCourse() {
    void *p_ = (void *)this;
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
    self->mGoalCenterX = 0;

    if (self->IsAltVariant()) {
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
            self->mGoalCenterX = (right + left + 1) * 16 / 2 << 12;
        }

        if (row > self->mGoalY / 16 + 0x11) {
            if (cooldown > 0) {
                cooldown--;
            } else if (self->IsAltVariant() && row < self->mScrollLimit / 2) {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15) ==
                        0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) ==
                            0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i].x = (lane * 16 + 8) << 12;
                                self->mArray1[i].y = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                if (right - left >= 0xd) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) ==
                        0) {
                        wideLane = left +
                                   ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) *
                                        (right - left - 3) >>
                                    15) +
                                   2;
                        self->mTileMap[wideLane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i].x = (wideLane * 16 + 8) << 12;
                                self->mArray1[i].y = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xa) {
                    if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) ==
                        0) {
                        if (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) ==
                            0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        self->mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (self->mArray1Active[i] == 0) {
                                self->mArray1[i].x = (lane * 16 + 8) << 12;
                                self->mArray1[i].y = (row * 16 + 8) << 12;
                                self->mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (self->IsAltVariant()) {
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
                        self->mArray2[slot].x = (col * 16 + 8) << 12;
                        self->mArray2[slot].y = (propRow * 16 + 8) << 12;
                        self->mArray2Active[slot] = 1;
                        self->mArray2Kind[slot] =
                            (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15;
                        slot++;
                    }
                }
            }
        }
    }
}

// @symbol _ZN15dScMgSnowball_c10ResetSceneEv
void dScMgSnowball_c::ResetScene() {
    void *p_ = (void *)this;
    char *c = (char *)p_;
    s32 tmp[3];
    int i;
    int y;
    u16 display;
    Pair *saved;

    if (IsAltVariant()) {
        mStartY = 0x2dc0;
        mGoalY = 0x190;
        mScrollLimit = 0x2e0;
        mTimeLeft = 0x960;
    } else {
        mStartY = 0x1740;
        mGoalY = 0x190;
        mScrollLimit = 0x178;
        mTimeLeft = 0x4b0;
    }

    display = *(volatile u16 *)0x04000304;
    *(volatile u16 *)0x04000304 = display & ~0x8000;
    mScreensSwapped = 1;
    Mg_SetTopScreenYOffset(0x50);

    y = mStartY;
    mPosX = 0x80000;
    mPosY = y << 12;
    mSoundPosX = mPosX;
    mSoundPosY = mPosY;
    y = mPosY;
    mScrollX = 0;
    mScrollY = y - 0x190000;

    y = mScrollY >> 12;
    SetBg2Offset(0x100, y);
    SetBg3Offset(0, mScrollY >> 12);
    SetSubBg2Offset(0, (mScrollY >> 12) + 0x110);
    SetSubBg3Offset(0, (mScrollY >> 12) + 0x110);

    *(volatile u32 *)0x04000018 = 0x100 | (((mScrollY >> 12) << 16) & 0x01ff0000);
    *(volatile u32 *)0x0400001c = ((mScrollY >> 12) << 16) & 0x01ff0000;
    *(volatile u32 *)0x04001018 = (((mScrollY >> 12) + 0x110) << 16) & 0x01ff0000;
    *(volatile u32 *)0x0400101c = (((mScrollY >> 12) + 0x110) << 16) & 0x01ff0000;

    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    saved = (Pair *)&mPrevPosX;
    mDrawPosX = saved->x;
    mDrawPosY = saved->y;
    mVelX = 0;
    mVelY = 0;
    mBallSize = 0x4000;
    mRollAngle = 0;
    mAngleY = 0;
    mHeadingAngle = 0;
    mPrevRollAngle = 0;
    mPrevAngleY = 0;
    mPrevHeadingAngle = 0;

    /* Constructed on the first call: mwcc emits the guard test, the three
       stores and the ~Vector3() registration inline here. */
    static Vector3 sUnitY(0, 0x1000, 0);

    mSpinAxis[0] = sUnitY.x;
    mSpinAxis[1] = sUnitY.y;
    mSpinAxis[2] = sUnitY.z;
    func_0203ce80((struct Vector3 *)tmp, (struct Vector3 *)mSpinAxis);

    mSpinQuat[0] = data_02092768[0];
    mSpinQuat[1] = data_02092768[1];
    mSpinQuat[2] = data_02092768[2];
    mSpinQuat[3] = data_02092768[3];
    Quaternion_FromVector3(mSpinQuat, &sUnitY, (struct Vector3 *)mSpinAxis);
    Quaternion_Normalize(mSpinQuat);

    Snowball_UpdateCamera(c);
    for (i = 0; i < 0x20; i++) {
        char *p = c + i;
        ((dScMgSnowball_c *)p)->mProbeHit[0] = 0;
        ((dScMgSnowball_c *)p)->mProbePush[0] = 0;
        ((dScMgSnowball_c *)p)->mProbeWater[0] = 0;
    }

    mScore = 0;
    mHitBreakable = 0;
    mAllWater = 0;
    mState = 0;
    mUphillDebt = 0;
    mMeltStarted = 0;

    Bit_InitAll();
    SnowSpray_ClearAll();

    mCountdownTimer = 0xf1;
    mCrashTimer = 0;
    mAnimCounter = 0;
    mSoundPending = 0;
    mEndDelayTimer = 0;
}

#pragma opt_strength_reduction off
#pragma opt_common_subs off
// @symbol _ZN15dScMgSnowball_c6RenderEv
/* Slot 9. `c` stays next to `self` as the stepping base for the
 * mArray1Active/mArray1Hit byte flags, read through `(dScMgSnowball_c *)p`.
 * Both pragmas are needed. */
s32 dScMgSnowball_c::Render() {
    char *c = (char *)this;
    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    int m[3];
    int vecArr[3];

    self->mAnimCounter++;
    if (self->mAnimCounter >= 0x20) {
        self->mAnimCounter = 0;
    }

    {
        int base = self->mGoalY;
        int t = (((self->mPosY >> 0xc) - base) * 0x7c) / (self->mStartY - base);
        Mg_RenderSpriteBoth(sSnowballCourseMeterSprites[0], 0xf0, t + 0x22, -1, 0);
    }
    Mg_RenderSpriteBoth(sSnowballCourseMeterSprites[1], 0xf0, 0x60, -1, 0);

    m[2] = 0xfffff008;
    m[1] = 0;
    m[0] = 0;
    func_0203cd80(m, -0x2000);
    *(volatile int *)0x040004c8 = (((short)m[0] >> 3) & 0x3ff) |
                                  ((((short)m[1] >> 3) & 0x3ff) << 10) |
                                  ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(int *)0x040004cc = 0x7fff;

    {
        int v = self->mBallSize;
        int t = v / 2 + v * 4;
        vecArr[0] = t;
        vecArr[1] = t;
        vecArr[2] = t;
        if (self->mBallSize > 0) {
            dExtSimpleModel_c *model = &((dScMgSnowball_c *)c)->mModel;
            const Vector3 *scale = (const Vector3 *)vecArr;
            model->Render(scale);
        }
    }

    RedrawCourse();

    for (int i1 = 0; i1 < 0x80; i1++) {
        char *p = c + i1;
        if (((dScMgSnowball_c *)p)->mArray1Active[0] == 1) {
            int t6 = self->mScrollY;
            int t5 = *(int *)((long long)(int)&((dScMgSnowball_c *)(c + i1 * 8))->mArray1[0].y);
            if (t5 >= t6 - 0x20000 && t5 < t6 + 0x1a0000 + (Mg_GetTopScreenYOffset() << 0xc)) {
                if (mArray1Kind[i1] == 1) {
                    int cnt = self->mAnimCounter;
                    int idx = (cnt / 4) & 7;
                    if (((dScMgSnowball_c *)p)->mArray1Hit[0] == 1)
                        idx += 8;
                    int a1 = (self->mArray1[i1].x - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    Mg_RenderSpriteBoth(sSnowballBreakableAnimSprites[idx], a1, a2, -1, -1);
                } else {
                    int a1 = (self->mArray1[i1].x - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    Mg_RenderSpriteBoth(sSnowballBreakableSprite, a1, a2, -1, 2);
                }
            }
        }
    }

    for (int i2 = 0; i2 < 0x80; i2++) {
        if (self->mArray2Active[i2] == 1) {
            int t7 = self->mScrollY;
            int t6 = *(int *)((long long)(int)&((dScMgSnowball_c *)(c + i2 * 8))->mArray2[0].y);
            if (t6 >= t7 - 0x40000 && t6 < t7 + 0x1c0000 + (Mg_GetTopScreenYOffset() << 0xc)) {
                switch (mArray2Kind[i2]) {
                case 0:
                case 1:
                case 2: {
                    int a1 = (self->mArray2[i2].x - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    Mg_RenderSpriteBoth(sSnowballObstacleSprite, a1, a2, -1, 1);
                    break;
                }
                case 3: {
                    int v = self->mArray2[i2].x;
                    int sel = (v < 0x80000) ? 1 : 0;
                    int a1 = (v - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    Mg_RenderSpriteBoth(sSnowballObstacleSideSprites[sel], a1, a2, -1, 1);
                    break;
                }
                }
            }
        }
    }

    {
        int a1 = (self->mDrawPosX - self->mScrollX) >> 0xc;
        int a2 = ((self->mDrawPosY - self->mScrollY) >> 0xc) - 0x110;
        Mg_RenderSpriteBoth(sSnowballBallSprite, a1, a2, -1, 2);
    }

    {
        int t4 = self->mCountdownTimer;
        if (t4 <= 0xf0 && t4 > 0x3c) {
            int q = t4 / 60;
            if (q >= 4)
                q = 3;
            Mg_DrawNumberRotated(0x80, 0x60, q, -1, -1, 0x800, 0);
        } else {
            int mode = self->mState;
            if (mode == 1 || mode == 2 || (mode == 3 && self->mEndDelayTimer > 0) ||
                (mode == 4 && self->mCrashTimer <= 0x3c)) {
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
                Mg_DrawBigNumber(0x84, r4, q60, -1, -1, 1, 0);
                tens = centis / 10;
                Mg_DrawBigNumber(0x9c, r4, tens, -1, -1, 1, 0);
                ones = centis % 10;
                if (ones == 5 || ones == 0) {
                    Mg_DrawBigNumber(0xac, r4, ones, -1, -1, 1, 0);
                } else {
                    Mg_DrawBigNumber(0xac, r4, ones + 1, -1, -1, 1, 0);
                }
                {
                    int idx = GetGameLanguage();
                    Mg_RenderSpriteBothMtx(
                        *(void **)((char *)sSnowballTextSpritesByLanguage[idx] + 0xc), 0x5c, r4, 0);
                }
                {
                    int idx = GetGameLanguage();
                    Mg_RenderSpriteBothMtx(
                        *(void **)((char *)sSnowballTextSpritesByLanguage[idx] + 0x1c), 0x90, r4,
                        0);
                }
            }
        }
    }

    Bit_DrawAll();
    SnowSpray_DrawAll();
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
s32 dScMgSnowball_c::Behavior() {
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

    mPrevRollAngle = *(s16 *)&mRollAngle;
    mPrevAngleY = *(s16 *)&mAngleY;
    mPrevHeadingAngle = *(s16 *)&mHeadingAngle;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;

    r4 = (mScrollLimit << 16) - 0x1d0000;
    r5 = mScrollY;

    switch (mState) {
    case 0:
        mCountdownTimer -= 1;
        v = mCountdownTimer;
        if (v == 0x3c) {
            mState = 1;
            Mg_ShowBanner(0xc, 0x80, 0x60, 0, -1, 0xd);
            func_02012790(0x2a);
        } else if (v == 0x78 || v == 0xb4 || v == 0xf0) {
            func_02012790(0x2b);
        }
        break;

    case 1:
        if (mCountdownTimer > 0) {
            mCountdownTimer -= 1;
            if (mCountdownTimer == 0)
                FreeGfxSlotsById(0xc);
        }

        idx = gActivePlayerSlot;
        r3rec = gTouchHeld[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (gTouchEdge[j] != 0);
        if (r7 != 0) {
            int vb = ((volatile unsigned char *)gTouchY)[(unsigned int)idx * 4];
            int va = ((volatile unsigned char *)gTouchX)[(unsigned int)idx * 4];
            mTouchX = va;
            mTouchY = vb;
        } else if (r3rec != 0) {
            s.a[0] = ((gTouchX[(unsigned int)idx * 4] - mTouchX) << 12) / 296;
            s.a[1] = ((gTouchY[(unsigned int)idx * 4] - mTouchY) << 12) / 296;
            mTouchX = gTouchX[(unsigned int)idx * 4];
            mTouchY = gTouchY[(unsigned int)idx * 4];
            if (Vec2_Len(s.a) >= 0x400) {
                s.a[0] = 0;
                s.a[1] = 0;
            }
            r7 = cstd::atan2(mVelY, mVelX);
            func_0203d388(s.a, (short)-r7);
            s.a[1] = s.a[1] << 2;
            func_0203d388(s.a, r7);
            if (mVelY >= 0) {
                if (s.a[1] > 0)
                    s.a[1] = 0;
            }
            if (s.a[1] > 0) {
                if (s.a[1] >= -mVelY) {
                    mVelY = 0;
                    s.a[1] = 0;
                }
            }
            mVelX += s.a[0];
            mVelY += s.a[1];
        }

        if (mVelX != 0 || mVelY != 0) {
            if (Vec2_Len(&mVelX) >= 0x8000 && func_0203d434((int *)ATS(SNOW_OFF(mVelX))) != 0)
                func_0203d630((int *)ATI(SNOW_OFF(mVelX)), 0x8000);
        }
        UpdateSnowball();

        mPosX += mVelX;
        mPosY += mVelY;

        if (mPosY >= mPrevPosY) {
            mUphillDebt += mPosY - mPrevPosY;
        } else if (mUphillDebt > 0) {
            mUphillDebt += mPosY - mPrevPosY;
            if (mUphillDebt < 0) {
                mBallSize += (-mUphillDebt) / 128;
                mUphillDebt = 0;
            }
        } else {
            mBallSize += (mPrevPosY - mPosY) / 128;
        }

        if (mBallSize >= 0x37000)
            mBallSize = 0x37000;
        Vec2_Sub(s.b, (int *)AT(C1, SNOW_OFF(mPosX)), &mSoundPosX);
        gate = (Vec2_Len(s.b) >= 0x30000) ? 1 : 0;
        if ((int)((long long)gate) != 0) {
            mSoundPosX = mPosX;
            mSoundPosY = mPosY;
            if (mBallSize < 0x10000)
                func_02012718(0x165, mPosX);
            else if (mBallSize < 0x20000)
                func_02012718(0x166, mPosX);
            else
                func_02012718(0x167, mPosX);
        }

        mScrollY = mPosY - 0x190000;
        if (mScrollY > r5)
            mScrollY = r5;
        if (mScrollY < 0)
            mScrollY = 0;
        if (mScrollY >= r4)
            mScrollY = r4;

        if (Vec2_Len(&mVelX) >= 0x10) {
            short *hp;
            v = Vec2_Len((int *)ATS(SNOW_OFF(mVelX)));
            hp = (short *)&mRollAngle;
            q = __aeabi_idiv(v * 0x2710, mBallSize);
            *hp = *hp + (unsigned short)q;
            m = cstd::atan2(mVelX, mVelY);
            ApproachLinear(*(short *)&mHeadingAngle, (short)m,
                           (short)(Vec2_Len((int *)ATI(SNOW_OFF(mVelX))) / 16));
        }

        if (mAllWater == 1 || mTimeLeft <= 0) {
            mMeltStarted = 1;
            mState = 4;
            break;
        }
        mTimeLeft -= 1;
        mScore += 1;
        t = mTimeLeft;
        if (t % 60 == 0) {
            func_02012790(0x2b);
        } else {
            if (t < 0x258 && t % 30 == 0)
                func_02012790(0x2b);
            else if (t < 0xb4 && t % 15 == 0)
                func_02012790(0x2b);
        }
        if (mPosY - mBallSize < (mGoalY << 12) + 0x10000) {
            Mg_SubmitScore(mScore);
            mState = 2;
            Sound::PlayBank2_2D(0x10e);
        }
        break;

    case 2:
        mCrashTimer += 1;
        if (mCrashTimer >= 0x3c) {
            Mg_SetNextBanner(4);
            Mg_SetSharedState(MG_STATE_FINISHED);
            mEndDelayTimer = 0xb4;
            mState = 3;
        }
        /* fall through */

    case 3:
        mPromptEnabled = 0;
        if (mEndDelayTimer > 0)
            mEndDelayTimer -= 1;
        idx = gActivePlayerSlot;
        r3rec = gTouchHeld[idx * 4];
        j = idx * 4;
        r7 = 0;
        if (r3rec != 0)
            r7 = (gTouchEdge[j] != 0);
        if (r7 != 0)
            mEndDelayTimer = 0;
        UpdateSnowball();

        if (mVelX != 0 || mVelY != 0) {
            r7 = Vec2_Len(&mVelX);
            if (mPosY < 0xc0000)
                r7 -= 0x80;
            if (r7 < 0) {
                mVelX = 0;
                mVelY = 0;
            } else if (func_0203d434((int *)ATS(SNOW_OFF(mVelX))) != 0) {
                func_0203d630((int *)ATI(SNOW_OFF(mVelX)), r7);
            }
        }

        mPosX += mVelX;
        mPosY += mVelY;
        if (mPosY < 0xe8000)
            mScreensSwapped = 0;
        else
            mScreensSwapped = 1;

        if (Vec2_Len(&mVelX) >= 0x10) {
            short *hp;
            v = Vec2_Len((int *)ATS(SNOW_OFF(mVelX)));
            hp = (short *)&mRollAngle;
            q = __aeabi_idiv(v * 0x2710, mBallSize);
            *hp = *hp + (unsigned short)q;
            mHeadingAngle = cstd::atan2(mVelX, mVelY);
        }

        mScrollY = mPosY - 0x190000;
        if (mScrollY > r5)
            mScrollY = r5;
        if (mScrollY < 0)
            mScrollY = 0;
        if (mScrollY >= r4)
            mScrollY = r4;
        break;

    case 4:
        if (mCrashTimer < 0x3c) {
            if (mVelX != 0 || mVelY != 0) {
                r4 = Vec2_Len(&mVelX) + 0x100;
                if (r4 >= 0x8000)
                    r4 = 0x8000;
                if (func_0203d434((int *)ATS(SNOW_OFF(mVelX))) != 0)
                    func_0203d630((int *)ATI(SNOW_OFF(mVelX)), r4);
            }
            mCrashTimer += 1;
        } else if (mCrashTimer == 0x3c) {
            if (mBallSize > 0) {
                mBallSize -= 0x1000;
                if (mBallSize <= 0) {
                    int t1, t0;
                    mBallSize = 0;
                    t1 = mScrollY - mPosY + 0x110000;
                    t0 = mPosX - 0x80000;
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
                    func_02012718(0x16b, mPosX);
                    mCrashTimer += 1;
                }
            }
        } else {
            mCrashTimer += 1;
            if (mCrashTimer >= 0x5a) {
                Mg_SetNextBanner(0xe);
                Mg_SetSharedState(MG_STATE_RETRY_PROMPT);
                mState = 5;
            }
        }

        if (Vec2_Len(&mVelX) >= 0x10) {
            short *hp;
            v = Vec2_Len((int *)ATS(SNOW_OFF(mVelX)));
            hp = (short *)&mRollAngle;
            q = __aeabi_idiv(v * 0x2710, mBallSize);
            *hp = *hp + (unsigned short)q;
            mHeadingAngle = cstd::atan2(mVelX, mVelY);
        }
        break;

    case 5:
        mPromptEnabled = 0;
        break;
    }

    Bit_UpdateAll();
    SnowSpray_UpdateAll();
    Snowball_UpdateCamera(c);
    return 1;
}

// @symbol _ZN15dScMgSnowball_c16OnGraphCallback2Ev
/* dScMgSnowball_c::OnGraphCallback2, from its vtable slot. The one-letter `V`
   scroll macro is #undef'd at the end of the function. */

#define V (self->mScrollY >> 12)

int dScMgSnowball_c::OnGraphCallback2() {
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
    return ((dScMgBase_c *)c)->dScMgBase_c::OnGraphCallback2() != 0;
}
#undef V

// @symbol _ZN15dScMgSnowball_c16OnGraphCallback3Ev
/* dScMgSnowball_c::OnGraphCallback3, from its vtable slot. */
int dScMgSnowball_c::OnGraphCallback3() {
    void *t = (void *)this;
    return ((dScMgBase_c *)t)->dScMgBase_c::OnGraphCallback3() != 0;
}

// @symbol _ZN15dScMgSnowball_c16OnGraphCallback0Ev
/* dScMgSnowball_c::OnGraphCallback0, from its vtable slot. */
int dScMgSnowball_c::OnGraphCallback0() {
    void *t = (void *)this;
    return ((dScMgBase_c *)t)->dScMgBase_c::OnGraphCallback0() != 0;
}

// @symbol _ZN15dScMgSnowball_c16CleanupResourcesEv
/* Slot 3. Frees the BMD that InitResources parked in mBallBmd. */
s32 dScMgSnowball_c::CleanupResources() {
    Ov004_Deallocate(mBallBmd);
    return 1;
}

// @symbol _ZN15dScMgSnowball_c10StartRoundEi
/* Slot 18, an override of dScMgBase_c::StartRound(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgSnowball_c::StartRound(int i) {
    void *c = (void *)this;

    Mg_SubmitScore(0);
    ResetScene();
    if (i != 0x13)
        return;
    BuildCourse();
    RedrawCourse();
}

// @symbol _ZN15dScMgSnowball_c13InitResourcesEv
/* Slot 0. Sets up both screens' BG2 and BG3 layers, loads the palettes and
 * tiles, then loads the snowball BMD into mBallBmd and hands it to mModel.
 * Returning 0 when dExtModel_c::SetFile fails aborts the scene. The helpers at
 * the end take the scene as an opaque pointer, so the receiver stays raw. */
s32 dScMgSnowball_c::InitResources() {
    void *arg0 = this;
    u8 *r4 = (u8 *)arg0;
    void *buf;

    data_0208ee44 = 1;
    *(vu16 *)0x4000304 &= ~0x8000;
    ((dScMgSnowball_c *)r4)->mScreensSwapped = 1;

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

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKIDAMA_NCG_BIN, (u8 *)G2::GetBG2CharPtr() + 0x4000);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKIDAMA_NCG_BIN,
                         (void *)((unsigned)G2S::GetBG2CharPtr() + 0x4000));
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKA_NSC_BIN, G2::GetBG3ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKA_NSC_BIN, G2S::GetBG3ScrPtr());

    data_0209d45c |= 0xc;
    data_0209d454 |= 0xc;

    buf = Ov004_LoadFileByPath(&sSnowballBgNclPath);
    CP15::FlushAndInvalidateDataCache((u32)buf, 0x100);
    GX::LoadBGPltt(buf, 0x100, 0x100);
    GXS::LoadBGPltt(buf, 0x100, 0x100);
    Ov004_Deallocate(buf);

    buf = Ov004_LoadFileByPath(&sSnowballNcgPath);
    DecompressLZ16(buf, (void *)0x6400000);
    DecompressLZ16(buf, (void *)0x6600000);
    Ov004_Deallocate(buf);

    buf = Ov004_LoadFileByPath(&sSnowballNclPath);
    CP15::FlushAndInvalidateDataCache((u32)buf, 0x100);
    GX::LoadOBJPltt(buf, 0, 0x100);
    GXS::LoadOBJPltt(buf, 0, 0x100);
    Ov004_Deallocate(buf);

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    G3X::SetFog(false, 0, 2, 0x1000);
    *(vu16 *)0x4000060 = (*(vu16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();

    ((dScMgSnowball_c *)r4)->mBallBmd = (BMD_File *)Ov004_LoadFileByPath(&data_ov006_021400fc);
    /* The two mBallBmd reloads keep `r4 + offset`: as members they cost 8
       bytes of address setup the ROM does not have. */
    dExtSimpleModel_c::UpdateFileOffsets(**(BMD_File **)(r4 + SNOW_OFF(mBallBmd)));

    if (!((dScMgSnowball_c *)r4)->mModel.SetFile(*(BMD_File **)(r4 + SNOW_OFF(mBallBmd)), 1, -1))
        return 0;

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    data_0209d45c |= 1;

    ResetScene();
    BuildCourse();
    RedrawCourse();
    return 1;
}

// @symbol _ZN15dScMgSnowball_cC1Ev
/* Everything but the `= 0` at 0xabf4 is compiler-generated: the base C2, the
   two vptr stores (dScMgSingle3DBase_c's, then this class's own, which lands
   on the cartridge's _ZTV15dScMgSnowball_c), the tracker at 0x471c, mModel,
   and the three element arrays through __cxa_vec_ctor (mArray1 and mArray2
   with Vector2's func_0203d738 / NullDestructor_0203d47c, mSprays with
   SnowSpray's pair), in the order ~dScMgSnowball_c unwinds them. */
dScMgSnowball_c::dScMgSnowball_c() : mBallBmd(0) {}

// @symbol _ZN9SnowSprayC1Ev
/* SnowSpray's empty inline constructor (dScMgSnowball_c.h), emitted here. */

// @symbol _ZN15dScMgSnowball_c11Bit_DrawAllEv
extern "C" {
extern void Mg_RenderSubSpriteMtx(void *a, int b, int c, void *m);
extern void DrawOamSprite(void *a, int b, int c, void *m);

/* Unused since Bit_DrawAll reads mBits directly. Kept: deleting these
   anonymous structs renumbers the TU's @NNN constants. */
typedef struct {  /* dScMgSnowball_bit, with the header's names */
    s32 x;        /* 0x00 */
    s32 y;        /* 0x04 */
    u8 pad[0x16]; /* 0x08 */
    u8 visible;   /* 0x1e */
    u8 spriteA;   /* 0x1f */
    u8 spriteB;   /* 0x20 */
    u8 pad2[3];   /* 0x21 */
} Elem_02129690;

typedef struct {
    u8 head[0xab6c];
    s32 fixed; /* 0xab6c */
    u8 mid[0xbe94 - 0xab70];
    Elem_02129690 arr[50]; /* 0xbe94 */
} Obj_02129690;
}
void dScMgSnowball_c::Bit_DrawAll() {
    void *self = (void *)this;
    dScMgSnowball_c *a = (dScMgSnowball_c *)self;
    int i;
    for (i = 0; i < 50; i++) {
        if (a->mBits[i].visible != 0) {
            int bb = a->mBits[i].x >> 12;
            int cc = (a->mBits[i].y - a->mScrollY) >> 12;
            Mg_RenderSubSpriteMtx((void *)sSnowballBitSprites[a->mBits[i].spriteA], bb, cc, 0);
            DrawOamSprite((void *)sSnowballBitSprites[a->mBits[i].spriteB], bb, cc, 0);
        }
    }
}

/* Most Bit_State* handlers address mBits[idx] as `(char *)&mBits[0].field + off`
 * with off = idx * 0x24, the ROM's `this + 0xbeXX + off` order. Bare
 * `mBits[idx].field` matches only at some sites: elsewhere it renumbers the
 * TU's @NNN constants (+28) or changes the code, measured per site. The
 * dead `char *` copies of this at the top of each handler stay: deleting any
 * one of them renumbers @NNN too. */
// @symbol _ZN15dScMgSnowball_c21Bit_StateFallSlowDownEi
void dScMgSnowball_c::Bit_StateFallSlowDown(int idx) {
    char *c = (char *)this;
    int off = idx * 0x24;
    short *h = (short *)((char *)&mBits[0].fallTimer + off);
    if (*(unsigned short *)h != 0) {
        *h = (short)(*(unsigned short *)h - 1);
        if (*h < 0)
            *h = 0;
        return;
    }
    {
        int *w = (int *)((char *)&mBits[0].vy + off);
        if (*w > 0x100) {
            *w = *w - 0x10;
            if ((short)*w < 0x100)
                *w = 0x100;
            return;
        }
    }
    *((unsigned char *)&mBits[0].fallState + off) = 0;
}

// @symbol _ZN15dScMgSnowball_c20Bit_StateFallSpeedUpEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}
void dScMgSnowball_c::Bit_StateFallSpeedUp(int idx) {
    unsigned char *base = (unsigned char *)this;
    int off = idx * 0x24;
    int *pa = (int *)((char *)&mBits[0].vy + off);
    int *pb = (int *)((char *)&mBits[0].fallSpeed + off);
    if (*pb > *pa) {
        *pa = *pa + 0x10;
        if (*pb > *pa)
            *pa = *pb;
    }
    {
        unsigned short *cnt = (unsigned short *)((char *)&mBits[0].fallTimer + off);
        if (*cnt != 0) {
            *cnt = *cnt - 1;
            if (*(short *)cnt < 0)
                *cnt = 0;
            return;
        }
        *(unsigned char *)((char *)&mBits[0].fallState + off) = 2;
        *cnt = (unsigned char)(((32 *
                                 (((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >>
                                15) +
                               0x20);
    }
}

// @symbol _ZN15dScMgSnowball_c18Bit_StateFallStartEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}
void dScMgSnowball_c::Bit_StateFallStart(int i) {
    unsigned int r;
    mBits[i].vy = 0;
    r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mBits[i].fallSpeed = (((r << 4) >> 15) << 4) + 0x300;
    mBits[i].fallState = 1;
    r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    r = ((r << 5) >> 15) + 0x20;
    mBits[i].fallTimer = (unsigned char)r;
}

// @symbol _ZN15dScMgSnowball_c19BitA_StateDriftStopEi
void dScMgSnowball_c::BitA_StateDriftStop(int idx) {
    char *self = (char *)this;
    int off = idx * 0x24;
    int *px = (int *)((char *)&mBits[0].x + off);
    int *pvx = (int *)((char *)&mBits[0].vx + off);
    int *py = (int *)((char *)&mBits[0].y + off);
    *px = *px + *pvx;
    *py = *py + *(int *)((char *)&mBits[0].vy + off);
    if (*(unsigned short *)((char *)&mBits[0].holdTimer + off) != 0) {
        unsigned short *drift = (unsigned short *)((char *)&mBits[0].driftTimer + off);
        *drift = *drift - 1;
        if ((short)*drift < 0)
            *drift = 0;
        return;
    }
    if (*pvx > 0) {
        *pvx -= 8;
        if ((short)*pvx < 0)
            *pvx = 0;
        return;
    }
    if (*pvx < 0) {
        *pvx += 8;
        if (*pvx > 0)
            *pvx = 0;
        return;
    }
    *(unsigned char *)((char *)&mBits[0].driftState + off) = 0;
}

// @symbol _ZN15dScMgSnowball_c19BitA_StateDriftLeftEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}
void dScMgSnowball_c::BitA_StateDriftLeft(int i) {
    char *base = (char *)this;
    int o = i * 0x24;
    unsigned short *hold;

    *(int *)((char *)&mBits[0].x + o) += *(int *)((char *)&mBits[0].vx + o);
    *(int *)((char *)&mBits[0].y + o) =
        *(int *)((char *)&mBits[0].y + o) + *(int *)((char *)&mBits[0].vy + o);

    hold = (unsigned short *)((char *)&mBits[0].holdTimer + o);
    if (*hold != 0) {
        *hold = *hold - 1;
        if (*(short *)hold < 0)
            *hold = 0;
        return;
    }

    {
        int *pvx = (int *)((char *)&mBits[0].vx + o);
        if (*pvx > -0x300) {
            *pvx -= 8;
            if (*pvx <= -0x300)
                *pvx = 0x300;
        }
    }

    {
        unsigned short *drift = (unsigned short *)((char *)&mBits[0].driftTimer + o);
        if (*drift != 0) {
            *drift = *drift - 1;
            if (*(short *)drift < 0)
                *drift = 0;
            return;
        }
        *(unsigned char *)((char *)&mBits[0].driftState + o) = 3;
        *drift =
            (short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >>
                      0xf) +
                     0x20) &
                    0xff);
    }
}

// @symbol _ZN15dScMgSnowball_c20BitA_StateDriftRightEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}
void dScMgSnowball_c::BitA_StateDriftRight(int index) {
    char *base = (char *)this;
    int i = index * 0x24;
    unsigned short v;

    *(int *)((char *)&mBits[0].x + i) += *(int *)((char *)&mBits[0].vx + i);
    *(int *)((char *)&mBits[0].y + i) += *(int *)((char *)&mBits[0].vy + i);

    v = *(unsigned short *)((char *)&mBits[0].holdTimer + i);
    if (v != 0) {
        *(short *)((char *)&mBits[0].holdTimer + i) = v - 1;
        if (*(short *)((char *)&mBits[0].holdTimer + i) < 0)
            *(short *)((char *)&mBits[0].holdTimer + i) = 0;
        return;
    }

    if (*(int *)((char *)&mBits[0].vx + i) < 0x300) {
        *(int *)((char *)&mBits[0].vx + i) += 8;
        if (*(int *)((char *)&mBits[0].vx + i) >= 0x300)
            *(int *)((char *)&mBits[0].vx + i) = 0x300;
    }

    v = *(unsigned short *)((char *)&mBits[0].driftTimer + i);
    if (v != 0) {
        *(short *)((char *)&mBits[0].driftTimer + i) = v - 1;
        if (*(short *)((char *)&mBits[0].driftTimer + i) < 0)
            *(short *)((char *)&mBits[0].driftTimer + i) = 0;
        return;
    }

    *(char *)((char *)&mBits[0].driftState + i) = 3;
    *(short *)((char *)&mBits[0].driftTimer + i) =
        (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 &
        0xff;
}

// @symbol _ZN15dScMgSnowball_c15BitA_StatePauseEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212f3b0[];
}
void dScMgSnowball_c::BitA_StatePause(int i) {
    char *o = (char *)this;
    int n = i * 0x24;
    if (*(unsigned short *)((char *)&mBits[0].holdTimer + n) != 0) {
        *(unsigned short *)((char *)&mBits[0].holdTimer + n) =
            *(unsigned short *)((char *)&mBits[0].holdTimer + n) - 1;
        if (*(short *)((char *)&mBits[0].holdTimer + n) < 0)
            *(short *)((char *)&mBits[0].holdTimer + n) = 0;
        return;
    }
    *(int *)((char *)&mBits[0].vx + n) = 0;
    *(unsigned char *)((char *)&mBits[0].driftState + n) =
        data_ov006_0212f3b0[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                << 1 >>
                            15];
    *(unsigned short *)((char *)&mBits[0].holdTimer + n) =
        (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                        0x7fff)) >>
                               0xf);
    *(unsigned short *)((char *)&mBits[0].driftTimer + n) =
        (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                         0x7fff)) >>
                                0xf) +
                               0x60);
}

// @symbol _ZN15dScMgSnowball_c12Bit_RunKindAEi
typedef void (dScMgSnowball_c::*PMF_02129d94)(int);
struct Entry_02129d94 {
    PMF_02129d94 pmf;
};
extern Entry_02129d94 sSnowballBitAStateTable[];
extern Entry_02129d94 sSnowballBitStateTable[];
/* Through a dScMgSnowball_c * copy of this: bare mBits[i] renumbers @NNN. */
void dScMgSnowball_c::Bit_RunKindA(int i) {
    dScMgSnowball_c *c = (dScMgSnowball_c *)this;
    int idx0 = c->mBits[i].driftState;
    (this->*sSnowballBitAStateTable[idx0].pmf)(i);
    int idx1 = c->mBits[i].fallState;
    (this->*sSnowballBitStateTable[idx1].pmf)(i);
}

// @symbol _ZN15dScMgSnowball_c19BitB_StateDriftStopEi
void dScMgSnowball_c::BitB_StateDriftStop(int i) {
    char *c = (char *)this;
    int o = i * 0x24;
    char *vxBase = (char *)&mBits[0].vx;
    char *xBase = (char *)&mBits[0].x;
    char *yBase = (char *)&mBits[0].y;
    int v;
    *(int *)(xBase + o) = *(int *)(xBase + o) + *(int *)(vxBase + o);
    *(int *)(yBase + o) = *(int *)(yBase + o) + ((dScMgSnowball_c *)(c + o))->mBits[0].vy;
    v = *(int *)(vxBase + o);
    if (v > 0) {
        v -= 0x20;
        *(int *)(vxBase + o) = v;
        v = *(int *)(vxBase + o);
        v = (short)v;
        if (v < 0) {
            v = 0;
            *(int *)(vxBase + o) = v;
        }
    } else if (v < 0) {
        v += 0x20;
        *(int *)(vxBase + o) = v;
        v = *(int *)(vxBase + o);
        if (v > 0) {
            v = 0;
            *(int *)(vxBase + o) = v;
        }
    } else {
        ((dScMgSnowball_c *)(c + o))->mBits[0].driftState = 0;
    }
}

// @symbol _ZN15dScMgSnowball_c19BitB_StateDriftLeftEi
void dScMgSnowball_c::BitB_StateDriftLeft(int idx) {
    char *c = (char *)this;
    int n = idx * 0x24;
    unsigned short d;
    short r;
    *(int *)((char *)&mBits[0].x + n) =
        *(int *)((char *)&mBits[0].x + n) + *(int *)((char *)&mBits[0].vx + n);
    *(int *)((char *)&mBits[0].y + n) =
        *(int *)((char *)&mBits[0].y + n) + *(int *)((char *)&mBits[0].vy + n);
    d = *(unsigned short *)((char *)&mBits[0].holdTimer + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)((char *)&mBits[0].holdTimer + n) = r;
        if (*(short *)((char *)&mBits[0].holdTimer + n) < 0)
            *(short *)((char *)&mBits[0].holdTimer + n) = 0;
        return;
    }
    if (*(int *)((char *)&mBits[0].vx + n) > -0x400) {
        *(int *)((char *)&mBits[0].vx + n) -= 0x20;
        if (*(int *)((char *)&mBits[0].vx + n) <= -0x400)
            *(int *)((char *)&mBits[0].vx + n) = 0x400;
    }
    d = *(unsigned short *)((char *)&mBits[0].driftTimer + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)((char *)&mBits[0].driftTimer + n) = r;
        if (*(short *)((char *)&mBits[0].driftTimer + n) < 0)
            *(short *)((char *)&mBits[0].driftTimer + n) = 0;
        return;
    }
    *(unsigned char *)((char *)&mBits[0].driftState + n) = 3;
}

// @symbol _ZN15dScMgSnowball_c20BitB_StateDriftRightEi
void dScMgSnowball_c::BitB_StateDriftRight(int index) {
    char *base = (char *)this;
    int n;

    mBits[index].x += mBits[index].vx;
    mBits[index].y += mBits[index].vy;

    n = mBits[index].holdTimer;
    if (n != 0) {
        mBits[index].holdTimer = (unsigned short)(n - 1);
        if (*(short *)&mBits[index].holdTimer < 0)
            *(short *)&mBits[index].holdTimer = 0;
        return;
    }

    if (mBits[index].vx < 0x400) {
        mBits[index].vx += 0x20;
        if (mBits[index].vx >= 0x400)
            mBits[index].vx = 0x400;
    }

    n = mBits[index].driftTimer;
    if (n != 0) {
        mBits[index].driftTimer = (unsigned short)(n - 1);
        if (*(short *)&mBits[index].driftTimer < 0)
            *(short *)&mBits[index].driftTimer = 0;
        return;
    }

    mBits[index].driftState = 3;
}

// @symbol _ZN15dScMgSnowball_c15BitB_StatePauseEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212f3ac[];
}
void dScMgSnowball_c::BitB_StatePause(int idx) {
    char *c = (char *)this;
    int n = idx * 0x24;
    unsigned short d;
    short r;
    unsigned int v;
    d = *(unsigned short *)((char *)&mBits[0].holdTimer + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)((char *)&mBits[0].holdTimer + n) = r;
        if (*(short *)((char *)&mBits[0].holdTimer + n) < 0)
            *(short *)((char *)&mBits[0].holdTimer + n) = 0;
        return;
    }
    *(int *)((char *)&mBits[0].vx + n) = 0;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)((char *)&mBits[0].vy + n) = (int)(((v << 5) >> 15) << 4) + 0x600;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)((char *)&mBits[0].driftState + n) = data_ov006_0212f3ac[(v << 1) >> 15];
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)((char *)&mBits[0].holdTimer + n) = (unsigned char)((v << 4) >> 15);
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)((char *)&mBits[0].driftTimer + n) = (unsigned char)(((v * 0x30) >> 15) + 0x30);
}

// @symbol _ZN15dScMgSnowball_c12Bit_RunKindBEi

typedef void (dScMgSnowball_c::*PMF_0212a224)(int);
extern "C" PMF_0212a224 sSnowballBitBStateTable[];

void dScMgSnowball_c::Bit_RunKindB(int idx) {
    char *base = (char *)this; /* bare mBits[idx] renumbers @NNN */
    unsigned char state = ((dScMgSnowball_c *)base)->mBits[idx].driftState;
    (this->*sSnowballBitBStateTable[state])(idx);
}

// @symbol _ZN15dScMgSnowball_c12Bit_PickKindEi
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212f3b4[];
}
void dScMgSnowball_c::Bit_PickKind(int idx) {
    char *self = (char *)this;
    unsigned int v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) << 3 >> 0xf;
    int b = 0;
    if (v == 5)
        b = 1;
    /* through self: bare mBits[idx] renumbers @NNN */
    ((dScMgSnowball_c *)self)->mBits[idx].kind = data_ov006_0212f3b4[b];
    ((dScMgSnowball_c *)self)->mBits[idx].driftState = 0;
}

// @symbol _ZN15dScMgSnowball_c13Bit_UpdateAllEv

typedef void (dScMgSnowball_c::*PMF_0212a2e0)(int);

extern "C" PMF_0212a2e0 sSnowballBitKindTable[];
extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209d4b8;

void dScMgSnowball_c::Bit_UpdateAll() {
    char *o = (char *)this;
    int i;
    char *q;
    int mask;

    q = o;
    i = 0;
    mask = 0x7fff;
    do {
        /* mBits[0] fields at q. The y and kind stores keep `q + offset`:
           their member spellings grow the function by 8 and 12 bytes. */
        if (((dScMgSnowball_c *)q)->mBits[0].active != 0) {
            u8 fidx = ((dScMgSnowball_c *)q)->mBits[0].kind;
            (this->*sSnowballBitKindTable[fidx])(i);
            if ((((dScMgSnowball_c *)q)->mBits[0].y - mScrollY) >> 12 >= 0xc8) {
                u32 rnd = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & mask;
                rnd = (rnd << 5) >> 0xf;
                ((dScMgSnowball_c *)q)->mBits[0].x = rnd << 0xf;
                *(int *)(q + SNOW_OFF(mBits[0].y)) = mScrollY - 0x8000;
                ((dScMgSnowball_c *)q)->mBits[0].driftState = 0;
                *(u8 *)(q + SNOW_OFF(mBits[0].kind)) = 0;
                ((dScMgSnowball_c *)q)->mBits[0].fallState = 0;
            }
        }
        i++;
        q += 0x24;
    } while (i < 0x32);
}

// @symbol _ZN15dScMgSnowball_c11Bit_InitAllEv
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
}
void dScMgSnowball_c::Bit_InitAll() {
    char *c = (char *)this;
    int i;
    char *p;
    unsigned int r;
    unsigned int v;
    int q;
    unsigned int m;

    /* mBits fields through the stepping pointer p. */
    i = 0;
    p = c;
    for (; i < 0x32; i++) {
        ((dScMgSnowball_c *)p)->mBits[0].x = 0;
        ((dScMgSnowball_c *)p)->mBits[0].y = 0;
        ((dScMgSnowball_c *)p)->mBits[0].vx = 0;
        ((dScMgSnowball_c *)p)->mBits[0].vy = 0;
        ((dScMgSnowball_c *)p)->mBits[0].holdTimer = 0;
        ((dScMgSnowball_c *)p)->mBits[0].driftTimer = 0;
        ((dScMgSnowball_c *)p)->mBits[0].fallTimer = 0;
        ((dScMgSnowball_c *)p)->mBits[0].active = 0;
        ((dScMgSnowball_c *)p)->mBits[0].kind = 0;
        ((dScMgSnowball_c *)p)->mBits[0].driftState = 0;
        ((dScMgSnowball_c *)p)->mBits[0].fallState = 0;
        ((dScMgSnowball_c *)p)->mBits[0].visible = 0;
        ((dScMgSnowball_c *)p)->mBits[0].spriteA = 0;
        ((dScMgSnowball_c *)p)->mBits[0].spriteB = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++) {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        ((dScMgSnowball_c *)p)->mBits[0].x = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x18) >> 0xf;
        ((dScMgSnowball_c *)p)->mBits[0].y = mScrollY - 0x8000 + (q << 0xf);
        ((dScMgSnowball_c *)p)->mBits[0].active = 1;
        ((dScMgSnowball_c *)p)->mBits[0].visible = 1;
        ((dScMgSnowball_c *)p)->mBits[0].kind = 0;
        ((dScMgSnowball_c *)p)->mBits[0].driftState = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        ((dScMgSnowball_c *)p)->mBits[0].spriteA = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        /* this re-read of mBits[0].spriteA keeps `p + offset`: every member
           spelling grows the function by 12 bytes */
        v = *(unsigned char *)(p + SNOW_OFF(mBits[0].spriteA)) +
            ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        ((dScMgSnowball_c *)p)->mBits[0].spriteB = (char)v;
        ((dScMgSnowball_c *)p)->mBits[0].holdTimer = 0;
        p += 0x24;
    }
}

/* MG_SNOWBALL's base profile, between the PMF descriptors: classInit, then
 * the same priority (0x179, the registry id) in both halfwords. */
extern "C" dScMgSnowball_c *dScMgSnowball_c_classInit(void);
BaseSpawnInfo<dScMgSnowball_c> g_profile_MG_SNOWBALL = {
    dScMgSnowball_c_classInit,
    0x179,
    0x179,
};

/* Source order is copy order. Each entry is a {function, 0} descriptor;
 * __sinit_d_s_mg_snowball.cpp copies them into the tables. */
extern "C" PMF_0212a2e0 sSnowballBitKindTable[3] = {
    &dScMgSnowball_c::Bit_PickKind,
    &dScMgSnowball_c::Bit_RunKindB,
    &dScMgSnowball_c::Bit_RunKindA,
};
extern "C" PMF_0212a224 sSnowballBitBStateTable[4] = {
    &dScMgSnowball_c::BitB_StatePause,
    &dScMgSnowball_c::BitB_StateDriftRight,
    &dScMgSnowball_c::BitB_StateDriftLeft,
    &dScMgSnowball_c::BitB_StateDriftStop,
};
/* Six slots so the symbol runs to the overlay bss end at 0x021430a0.
 * Only the first four are copied from descriptors; the tail stays zero. */
Entry_02129d94 sSnowballBitAStateTable[6] = {
    {&dScMgSnowball_c::BitA_StatePause},
    {&dScMgSnowball_c::BitA_StateDriftRight},
    {&dScMgSnowball_c::BitA_StateDriftLeft},
    {&dScMgSnowball_c::BitA_StateDriftStop},
};
Entry_02129d94 sSnowballBitStateTable[3] = {
    {&dScMgSnowball_c::Bit_StateFallStart},
    {&dScMgSnowball_c::Bit_StateFallSpeedUp},
    {&dScMgSnowball_c::Bit_StateFallSlowDown},
};

// @symbol dScMgSnowball_c_classInit
/* Reconstructed source-style name: SM64DS proves dScMgSnowball_c through RTTI,
 * allocation size, vtable identity, and the MG_SNOWBALL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSnowballSlalom_Spawn.
 * `new` goes through fBase_c's operator new for the 0xc59c bytes and runs
 * the real constructor above on them. */
extern "C" dScMgSnowball_c *dScMgSnowball_c_classInit(void) {
    return new dScMgSnowball_c;
}
