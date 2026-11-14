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
 * breaks five functions and the section order. The three bracketed optimizer
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
// - dScMgSnowball_c::Vec2, the collision ring's hit pair, stays its own nested
//   type with a destructor and no constructor (see the header); it was not
//   swapped for Vector2 in the pass that typed mArray1/mArray2.
// - Measured residue, each with a note at the site: DrawCourseColumn's
//   volatile tile locals and SNOW_OFF row arithmetic, Render's y loads through
//   a stepped receiver, Behavior's volatile touch read, ResetScene's Pair copy,
//   and the opt_common_subs / opt_strength_reduction brackets.
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
   here, so the one copy that has to be a block move goes through the flat
   Mtx43 rather than member-wise through Vector3's destructor, which is
   0x20 bytes more than the ROM has. */
#include "dScMgSnowball_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "SpawnInfo.h"
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/gx/gx_load.h"
#include "cstd.h"
#include "decl_common.h"
#include "math/Quaternion.h"
#include "LevelRng.h"
#include "common.h"
#include "Sound.h"
#include "dScMgBase_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "private/mtx43.h"
#include "OamAttr.h"
#include "Vector2Math.h"
#include "nitro/gx/g3x.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"
#include "nitro/mi/memory.h"

/* Declarations the recovered sources need that no project header supplies.
 * Each one was checked against include/*.h first; these are the residue. */

typedef struct Pair {
    s32 x;
    s32 y;
} Pair;

/* Byte offset of a dScMgSnowball_c member, for the sites that must keep
   `pointer + constant` arithmetic because every member spelling moved bytes. */
#define SNOW_OFF(m) ((int)&((dScMgSnowball_c *)0)->m)

extern "C" {
extern void RenderOamBothScreens(void *a, int b, int c, int d, int e, void *f);
extern OamAttr *sSnowballBitSprites[];
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *p);
extern void func_0203d630(int *p, int m);
extern u32 d_s_mg_snowballNewHold(u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir,
                                  void *callback);
extern void func_02012718(int a, int b);
extern int atan2_hold_d_s_mg_snowball_1(int y, int x);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern struct Matrix4x3 data_020a0e68;
extern u16 sSnowballCourseTiles[][4];
extern unsigned char data_ov006_0212f3b0[];
extern unsigned char data_ov006_0212f3ac[];
extern int data_ov006_0212f3b4[];

/* A random integer in [0, n): the generator's top 15 bits, scaled. */
static inline u32 MgRandom(u32 n) {
    return (((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * n >> 15;
}
extern void DrawOamSprite(OamAttr *attr, int x, int y, struct Matrix2x2 *mtx);
extern void Mg_SetTopScreenYOffset(int);
extern void SetBg2Offset(int, int);
extern void SetBg3Offset(int, int);
extern void SetSubBg2Offset(int, int);
extern void SetSubBg3Offset(int, int);
extern void Quaternion_Normalize(s32 *);
extern s32 data_02092768[4];
void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void Mg_DrawNumberRotated(int a1, int a2, int a3, int a4, int a5, int a6, u16 a7);
s32 GetGameLanguage(void);
void FreeGfxSlotsById(int arg);
void Vec2_Sub(int *o, int *a, int *b);
extern void Ov004_Deallocate(void *p);
u32 LoadCompressedFileAt(int fileID, void *target);
void *Ov004_LoadFileByPath(void *p);
void DecompressLZ16(void *src, void *dst);
extern int data_0208ee44;
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
/* Where two recovered declarations disagreed, the more precise one is
  * kept. */
}

/* Empty: every call in the ROM's body is compiler-generated. The members go
   in reverse declaration order -- __cxa_vec_cleanup on mSprays (with
   SnowSpray's destructor), mArray2 and mArray1 (with Vector2's,
   NullDestructor_0203d47c), then ~dExtSimpleModel_c on mModel -- and the rest
   is the inlined dScMgSingle3DBase_c destructor. */
dScMgSnowball_c::~dScMgSnowball_c() {}

/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* SnowSpray's empty inline destructor (dScMgSnowball_c.h), emitted here. */

void dScMgSnowball_c::SnowSpray_DrawAll() {
    int i;
    for (i = 0; i < 0x20; i++) {
        if (mSprays[i].active != 0) {
            RenderOamBothScreens(sSnowballBitSprites[mSprays[i].sprite], mSprays[i].x >> 12,
                                 ((mSprays[i].y - mSprays[i].z - mScrollY) >> 12) - 0x110, -1, 0,
                                 0);
        }
    }
}

void dScMgSnowball_c::SnowSpray_UpdateAll() {
    int i;
    for (i = 0; i < 0x20; i++) {
        if (mSprays[i].active) {
            if (mSprays[i].life > 0) {
                mSprays[i].life--;
                if (mSprays[i].life <= 0) {
                    mSprays[i].active = 0;
                    return;
                }
            }
            mSprays[i].vz -= 0x200;
            VEC_Add((Vector3 *)&mSprays[i].x, (Vector3 *)&mSprays[i].vx, (Vector3 *)&mSprays[i].x);
            if (mSprays[i].vz < 0 && mSprays[i].z < 0) {
                mSprays[i].active = 0;
                return;
            }
        }
    }
}

void dScMgSnowball_c::SnowSpray_ClearAll() {
    int i;
    for (i = 0; i < 0x20; i++) {
        mSprays[i].x = 0;
        mSprays[i].y = 0;
        mSprays[i].z = 0;
        mSprays[i].vx = 0;
        mSprays[i].vy = 0;
        mSprays[i].vz = 0;
        mSprays[i].life = 0;
        mSprays[i].active = 0;
        mSprays[i].sprite = 0;
    }
}

int dScMgSnowball_c::IsInWater(int *point) {
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

int dScMgSnowball_c::HitsBreakable(int *point) {
    int i;
    for (i = 0; i < 0x80; i++) {
        if (mArray1Active[i] == 1) {
            int dx, dy;
            if (point[0] >= mArray1[i].x + 0x10000)
                continue;
            if (point[0] < mArray1[i].x - 0x10000)
                continue;
            if (point[1] >= mArray1[i].y + 0x8000)
                continue;
            if (point[1] < mArray1[i].y - 0x8000)
                continue;
            dy = mArray1[i].y - point[1];
            dx = mArray1[i].x - point[0];
            if (FX_MUL(dx, dx) + FX_MUL(dy, dy) / 4 > 0x100000)
                continue;
            mArray1Hit[i] = 1;
            if (mArray1Kind[i] == 1) {
                mSoundPending = 1;
                mHitSoundX = mArray1[i].x;
                mHitSoundY = mArray1[i].y;
            }
            return 1;
        }
    }
    return 0;
}

// kept: the ROM recomputes x >> 12 and courseY >> 12 after the tile lookup; with
// CSE on they are reused and the function is 0x288. A tile-lookup inline helper,
// u16/unsigned casts or % 16 on the locals, and reading point[0] directly do not
// break the reuse.
#pragma opt_common_subs off
int dScMgSnowball_c::HitsWall(int *point) {
    int courseY = point[1];
    int x;
    unsigned short tile;
    int diagonalSum, diagonalDifference, localX, localY;

    if (courseY < -0x20000)
        return 1;
    if (courseY >= mScrollLimit << 16)
        return 1;
    if (courseY >= mScrollY + 0x1d0000)
        return 1;
    x = point[0];
    if (x < 0)
        return 1;
    if (x >= 0x100000)
        return 1;
    if (courseY < 0)
        tile = mTileMap[(x >> 12) / 16][0];
    else
        tile = mTileMap[(x >> 12) / 16][(courseY >> 12) / 16];
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
        mProbeHit[i] = 0;
        mProbePush[i] = 0;
        mProbeWater[i] = 0;
        i = i + 1;
    } while (i < 0x20);
    fastWallHit = 0;
    mHitBreakable = 0;

    startPoint[0] = mPosX;
    startPoint[1] = mPosY;
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
            mProbeHit[j] = 0;
            sample[0] = 0;
            sample[1] = 0;
            sample[1] += mBallSize;
            func_0203d388(sample, angle);
            sample[0] += mPosX;
            sample[1] += mPosY;
            wallProbe[0] = sample[0];
            wallProbe[1] = sample[1];
            if (HitsWall(wallProbe) != 0) {
                mProbeHit[j] = 1;
                mProbePush[j] = 1;
                anyHit = 1;
                if (Vec2_Len((const Vector2 *)&mVelX) >= 0x800)
                    fastWallHit = 1;
                hit.x = sample[0];
                hit.y = sample[1];
            } else {
                breakableProbe[0] = sample[0];
                breakableProbe[1] = sample[1];
                if (HitsBreakable(breakableProbe) != 0) {
                    mProbeHit[j] = 1;
                    anyHit = 1;
                    mProbePush[j] = 1;
                    mHitBreakable = 1;
                    hit.x = sample[0];
                    hit.y = sample[1];
                }
            }
            waterProbe[0] = sample[0];
            waterProbe[1] = sample[1];
            if (IsInWater(waterProbe) != 0)
                mProbeWater[j] = 1;
            else
                mProbeWater[j] = zDry;
            angle = (s16)(angle + 0x800);
        }

        response[0] = zOrigin;
        response[1] = zOrigin;
        angle = zOrigin;
        if (anyHit != 0) {
            for (i = 0; i < 0x20; i++) {
                if (mProbeHit[i] == 1) {
                    hitNormal[0] = pushDirX;
                    hitNormal[1] = pushDirY;
                    func_0203d388(hitNormal, angle);
                    response[0] += hitNormal[0];
                    response[1] += hitNormal[1];
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(response);
            if (Vec2_Len((const Vector2 *)&mVelX) < 0x800)
                func_0203d630(response, Vec2_Len((const Vector2 *)&mVelX));
            else
                func_0203d630(response, pushDirY);
            mPosX -= response[0];
            mPosY -= response[1];
        } else if (startedInWater == 1) {
            allWater = 1;
            for (i = 0; i < 0x20; i++) {
                if (mProbeWater[i] == 1) {
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
            mVelX += response[0];
            mVelY += response[1];
            if (allWater == 1)
                mAllWater = 1;
        }
        resolvePass = resolvePass + 1;
    } while (anyHit == 1 && resolvePass < 0x21);

    if (mHitBreakable == 1) {
        len = Vec2_Len((const Vector2 *)&mVelX);
        mBallSize -= len;
        if (mBallSize < 0x4000)
            mBallSize = 0x4000;
        if (len >= 0x800) {
            int pz = mBallSize;
            int py = (mScrollY - hit.y) + 0x110000;
            int px = hit.x - 0x80000;
            part.x = px;
            part.y = py;
            part.z = pz;
            id1 = Particle::System::New(0, kPtclMgSnowballHitA, part.x * 8, part.y * 8, part.z * 8,
                                        0, 0);
            id2 = Particle::System::New(0, kPtclMgSnowballHitB, part.x * 8, part.y * 8, part.z * 8,
                                        0, 0);
            sys = Particle::System::FromUniqueID(id1);
            if (sys != 0) {
                /* 0x44 lies in Particle::System's unnamed pad_042. */
                ((int *)sys)[0x44 / 4] = (s16)((mBallSize - 0x4000) * 11 / 60 + 0x3000);
                sys->callbackParam = (s16)((mBallSize - 0x4000) * 36 / 10 / 60 + 0x1333);
                sys->callbackVelocity = (s16)((mBallSize - 0x4000) * 26 / 10 / 60 + 0x1666);
                sys->callbackScale = (s16)((mBallSize - 0x4000) * 8 / 10 / 60 + 0x666);
            }
            sys = Particle::System::FromUniqueID(id2);
            if (sys != 0) {
                sys->callbackParam = (s16)((mBallSize - 0x4000) * 62 / 10 / 60 + 0x1ccc);
                sys->callbackScale = (s16)((mBallSize - 0x4000) * 45 / 10 / 60 + 0x1800);
            }
            if (mState == 1) {
                func_02012718(0x169, hit.x);
                if (mSoundPending == 1) {
                    func_02012718(0x16a, mHitSoundX);
                    mSoundPending = 0;
                }
            }
        }
    } else if (fastWallHit == 1 && mState == 1) {
        func_02012718(0x168, hit.x);
    }

    k = 0;
    mSoundPending = 0;
    push[0] = 0;
    push[1] = 0;
    do {
        if (mProbePush[k] == 1) {
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
    normalVelocity = FX_MUL(mVelX, push[0]) + FX_MUL(mVelY, push[1]);
    if (mHitBreakable == 1) {
        normalVelocity = FX_MUL(normalVelocity, 0x1800);
        mVelX -= FX_MUL(normalVelocity, push[0]);
        mVelY -= FX_MUL(normalVelocity, push[1]);
        headingComponent = FX_SinIdx((unsigned short)cstd::atan2(mVelY, mVelX) >> 4);
        vx = mVelX;
        mVelX = vx / 8 + FX_MUL(vx, headingComponent) / 4;
        vy = mVelY;
        mVelY = vy / 8 + FX_MUL(vy, headingComponent) / 4;
    } else {
        normalVelocity = FX_MUL(normalVelocity, 0x1200);
        mVelX -= FX_MUL(normalVelocity, push[0]);
        mVelY -= FX_MUL(normalVelocity, push[1]);
    }
}

void dScMgSnowball_c::UpdateCamera() {
    int t = FX_Div(0xc0000, data_02082314);
    Vector3 v;
    int x, y;
    if (mScreensSwapped == 0) {
        mCamera.mLookAt.x = 0;
        mCamera.mLookAt.y = 0xd0000;
        mCamera.mLookAt.z = 0;
        mCamera.mPos.x = 0;
        mCamera.mPos.y = 0;
        mCamera.mPos.z = t;
        mCamera.mFovAngle = 0x200;
        Camera_UpdateMatrices(&mCamera);
    } else {
        mCamera.mLookAt.x = 0;
        mCamera.mLookAt.y = -0x60000;
        mCamera.mLookAt.z = 0;
        mCamera.mPos.x = 0;
        mCamera.mPos.y = 0;
        mCamera.mPos.z = t;
        mCamera.mFovAngle = 0x200;
        Camera_UpdateMatrices(&mCamera);
    }
    y = mScrollY - mPosY + 0x110000;
    x = mPosX - 0x80000;
    v.x = x;
    v.y = y;
    v.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mHeadingAngle);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mRollAngle);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(Mtx43 *)&mModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
}

void dScMgSnowball_c::RedrawCourse() {
    int row = (mScrollY >> 12) / 16 - 2;
    int i;
    if (row < 0)
        row = 0;
    for (i = 0; i < 0x10; i++) {
        DrawCourseColumn(row, 1);
        row++;
    }
    row = ((mScrollY >> 12) + 0x110) / 16 - 2;
    for (i = 0; i < 0x10; i++) {
        if (row >= mScrollLimit)
            return;
        DrawCourseColumn(row, 0);
        row++;
    }
}

// kept: natural indexing (sSnowballCourseTiles[mTileMap[x][y]][k], scr + x * 2)
// with strength reduction on reproduces the size, the prologue and the whole
// flag == 1 loop, but colours the second loop as a register permutation
// (47 words: x/j/row/line/tiles/2 land in r4/r6/r7/r5/sb/r8 against the ROM
// r4/r8/sb/r7/r6/r5). Declaration order, block-scoped or per-branch locals,
// else-vs-return, `long line` and dead initialisers did not move it.
#pragma opt_strength_reduction off
/* BG2 tile-column writer.
 *
 * Copies one 16-row column of 2x2 BG tiles into the BG2 screen of both the
 * main and the sub engine. Each cell index comes from the u16 grid at
 * self+0x4f38 (row stride 0x5c0, column `col`); its four tiles are the four
 * u16 words of sSnowballCourseTiles[idx], written with MI_CpuFill16 (two
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
    u16 *scr;

    if (flag == 1) {
        masked = col & 0xf;
        tile = sSnowballCourseTiles[0][3];
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
            tile = sSnowballCourseTiles[idx][0];
            scr = (u16 *)((char *)G2_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j, v0 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][1];
            scr = (u16 *)((char *)G2_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 1, v1 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][2];
            scr = (u16 *)((char *)G2_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 0x20, v2 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][3];
            scr = (u16 *)((char *)G2_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 0x21, v3 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][0];
            scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j, v4 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][1];
            scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 1, v5 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][2];
            scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 0x20, v6 = tile, n);

            idx = *(u16 *)(row + SNOW_OFF(mTileMap));
            tile = sSnowballCourseTiles[idx][3];
            scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + off);
            MI_CpuFill16(scr + j + 0x21, v7 = tile, n);

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
        tile = sSnowballCourseTiles[idx][0];
        scr = (u16 *)((char *)G2_GetBG2ScrPtr() + 0x800 + off);
        MI_CpuFill16(scr + j, w0 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][1];
        scr = (u16 *)((char *)G2_GetBG2ScrPtr() + 0x802 + off);
        MI_CpuFill16(scr + j, w1 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][2];
        scr = (u16 *)((char *)G2_GetBG2ScrPtr() + 0x840 + off);
        MI_CpuFill16(scr + j, w2 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][3];
        scr = (u16 *)((char *)G2_GetBG2ScrPtr() + k842 + off);
        MI_CpuFill16(scr + j, w3 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][0];
        scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + 0x800 + off);
        MI_CpuFill16(scr + j, w4 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][1];
        scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + 0x802 + off);
        MI_CpuFill16(scr + j, w5 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][2];
        scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + 0x840 + off);
        MI_CpuFill16(scr + j, w6 = tile, n);

        idx = *(u16 *)(row + SNOW_OFF(mTileMap));
        tile = sSnowballCourseTiles[idx][3];
        scr = (u16 *)((char *)G2S_GetBG2ScrPtr() + k842 + off);
        MI_CpuFill16(scr + j, w7 = tile, n);

        row += 0x5c0;
        j += 2;
        i += 1;
    } while (i < 0x10);
}
#pragma opt_strength_reduction on

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
        mArray1Active[i] = 0;
        mArray1Kind[i] = 0;
        mArray1Hit[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        mArray2Active[i] = 0;
    }
    mGoalCenterX = 0;

    if (IsAltVariant()) {
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

    for (row = mScrollLimit - 1; row >= 0; row--) {
        prevLeft = leftKind;
        prevRight = rightKind;
        leftKind = MgRandom(3);
        rightKind = MgRandom(3);

        if (row >= mScrollLimit - 8) {
            rightKind = leftKind = 0;
        } else if (row <= mGoalY / 16) {
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
            mTileMap[k][row] = 1;
        }
        switch (leftKind) {
        case 0:
            mTileMap[left][row] = 2;
            mTileMap[left + 1][row] = 0;
            break;
        case 1:
            mTileMap[left][row] = 3;
            mTileMap[left + 1][row] = 4;
            break;
        case 2:
            mTileMap[left][row] = 5;
            mTileMap[left + 1][row] = 6;
            break;
        }
        for (i = left + 2; i < right - 1; i++) {
            mTileMap[i][row] = 0;
        }
        switch (rightKind) {
        case 0:
            mTileMap[right - 1][row] = 0;
            mTileMap[right][row] = 7;
            break;
        case 1:
            mTileMap[right - 1][row] = 8;
            mTileMap[right][row] = 9;
            break;
        case 2:
            mTileMap[right - 1][row] = 10;
            mTileMap[right][row] = 11;
            break;
        }
        for (i = right + 1; i < 0x10; i++) {
            mTileMap[i][row] = 1;
        }

        if (leftKind == 1) {
            left++;
        }
        if (rightKind == 1) {
            right--;
        }
        if (row == mGoalY / 16 - 1) {
            mGoalCenterX = (right + left + 1) * 16 / 2 << 12;
        }

        if (row > mGoalY / 16 + 0x11) {
            if (cooldown > 0) {
                cooldown--;
            } else if (IsAltVariant() && row < mScrollLimit / 2) {
                if (right - left >= 0xd) {
                    if (MgRandom(8) == 0) {
                        if (MgRandom(2) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (mArray1Active[i] == 0) {
                                mArray1[i].x = (lane * 16 + 8) << 12;
                                mArray1[i].y = (row * 16 + 8) << 12;
                                mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                if (right - left >= 0xd) {
                    if (MgRandom(16) == 0) {
                        wideLane = left + MgRandom(right - left - 3) + 2;
                        mTileMap[wideLane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (mArray1Active[i] == 0) {
                                mArray1[i].x = (wideLane * 16 + 8) << 12;
                                mArray1[i].y = (row * 16 + 8) << 12;
                                mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xa) {
                    if (MgRandom(16) == 0) {
                        if (MgRandom(2) == 0) {
                            lane = left + 2;
                        } else {
                            lane = right - 2;
                        }
                        mTileMap[lane][row] = 12;
                        cooldown = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (mArray1Active[i] == 0) {
                                mArray1[i].x = (lane * 16 + 8) << 12;
                                mArray1[i].y = (row * 16 + 8) << 12;
                                mArray1Active[i] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (IsAltVariant()) {
        for (i = 0; i < 0x80; i++) {
            if (mArray1Active[i] == 1) {
                if (MgRandom(2) == 0) {
                    mArray1Kind[i] = 1;
                }
            }
        }
    }

    for (goalRow = mGoalY / 16 - 1; goalRow <= mGoalY / 16; goalRow++) {
        for (j = 0; j < 0x10; j++) {
            tile = mTileMap[j][goalRow];
            if (tile == 0) {
                mTileMap[j][goalRow] = 13;
            } else if (tile == 2) {
                mTileMap[j][goalRow] = 14;
            } else if (tile == 7) {
                mTileMap[j][goalRow] = 15;
            }
        }
    }

    slot = 0;
    for (propRow = mScrollLimit - 1; propRow >= 0; propRow--) {
        for (col = 0; col < 0x10; col++) {
            if (mTileMap[col][propRow] == 1) {
                roll = MgRandom(16);
                if (roll < 4) {
                    mTileMap[col][propRow] = (roll + 0x1d);
                } else if (roll == 4) {
                    if (slot < 0x80) {
                        mArray2[slot].x = (col * 16 + 8) << 12;
                        mArray2[slot].y = (propRow * 16 + 8) << 12;
                        mArray2Active[slot] = 1;
                        mArray2Kind[slot] = MgRandom(4);
                        slot++;
                    }
                }
            }
        }
    }
}

void dScMgSnowball_c::ResetScene() {
    s32 tmp[3];
    int i;
    int y;
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

    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
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

    G2_SetBG2Offset(0x100, mScrollY >> 12);
    G2_SetBG3Offset(0, mScrollY >> 12);
    G2S_SetBG2Offset(0, (mScrollY >> 12) + 0x110);
    G2S_SetBG3Offset(0, (mScrollY >> 12) + 0x110);

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

    UpdateCamera();
    for (i = 0; i < 0x20; i++) {
        mProbeHit[i] = 0;
        mProbePush[i] = 0;
        mProbeWater[i] = 0;
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

// kept: with strength reduction on, the two slot loops grow (0x6b0) and no indexed spelling recovers 0x694;
// natural mArray1Active[i] / mArray1[i].y comes out 0x68c, short exactly the two
// `ldr rN, =0xacdc` / `=0xb5dc` pool loads the ROM uses for the y reads
#pragma opt_strength_reduction off
/* Slot 9. `p` (this + i) is the stepping base for the mArray1Active/mArray1Hit
 * byte flags and colours r7; the indexed spelling colours it last. The y reads
 * go through an int cast of `&(this + i * 8)->mArray[0].y` because the ROM loads them
 * as `[this + i * 8, pool offset]`, which `mArray1[i].y` does not reproduce. */
s32 dScMgSnowball_c::Render() {
    int light[3];
    Vector3 scale;

    mAnimCounter++;
    if (mAnimCounter >= 0x20) {
        mAnimCounter = 0;
    }

    {
        int base = mGoalY;
        int t = (((mPosY >> 0xc) - base) * 0x7c) / (mStartY - base);
        Mg_RenderSpriteBoth(sSnowballCourseMeterSprites[0], 0xf0, t + 0x22, -1, 0);
    }
    Mg_RenderSpriteBoth(sSnowballCourseMeterSprites[1], 0xf0, 0x60, -1, 0);

    light[2] = -0xff8;
    light[1] = 0;
    light[0] = 0;
    func_0203cd80(light, -0x2000);
    G3_LightVector(GX_LIGHTID_0, light[0], light[1], light[2]);
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));

    {
        int t = mBallSize / 2 + mBallSize * 4;
        scale.x = t;
        scale.y = t;
        scale.z = t;
        if (mBallSize > 0) {
            mModel.Render(&scale);
        }
    }

    RedrawCourse();

    for (int i = 0; i < 0x80; i++) {
        char *p = (char *)this + i;
        if (((dScMgSnowball_c *)p)->mArray1Active[0] == 1) {
            int scrollY = mScrollY;
            int y = *(int *)(int)&((dScMgSnowball_c *)((char *)this + i * 8))->mArray1[0].y;
            if (y >= scrollY - 0x20000 &&
                y < scrollY + 0x1a0000 + (Mg_GetTopScreenYOffset() << 0xc)) {
                if (mArray1Kind[i] == 1) {
                    int idx = (mAnimCounter / 4) & 7;
                    if (((dScMgSnowball_c *)p)->mArray1Hit[0] == 1)
                        idx += 8;
                    Mg_RenderSpriteBoth(sSnowballBreakableAnimSprites[idx],
                                        (mArray1[i].x - mScrollX) >> 0xc,
                                        ((y - scrollY) >> 0xc) - 0x110, -1, -1);
                } else {
                    Mg_RenderSpriteBoth(sSnowballBreakableSprite, (mArray1[i].x - mScrollX) >> 0xc,
                                        ((y - scrollY) >> 0xc) - 0x110, -1, 2);
                }
            }
        }
    }

    for (int i = 0; i < 0x80; i++) {
        if (mArray2Active[i] == 1) {
            int scrollY = mScrollY;
            int y = *(int *)(int)&((dScMgSnowball_c *)((char *)this + i * 8))->mArray2[0].y;
            if (y >= scrollY - 0x40000 &&
                y < scrollY + 0x1c0000 + (Mg_GetTopScreenYOffset() << 0xc)) {
                switch (mArray2Kind[i]) {
                case 0:
                case 1:
                case 2:
                    Mg_RenderSpriteBoth(sSnowballObstacleSprite, (mArray2[i].x - mScrollX) >> 0xc,
                                        ((y - scrollY) >> 0xc) - 0x110, -1, 1);
                    break;
                case 3: {
                    int x = mArray2[i].x;
                    Mg_RenderSpriteBoth(sSnowballObstacleSideSprites[(x < 0x80000) ? 1 : 0],
                                        (x - mScrollX) >> 0xc, ((y - scrollY) >> 0xc) - 0x110, -1,
                                        1);
                    break;
                }
                }
            }
        }
    }

    Mg_RenderSpriteBoth(sSnowballBallSprite, (mDrawPosX - mScrollX) >> 0xc,
                        ((mDrawPosY - mScrollY) >> 0xc) - 0x110, -1, 2);

    {
        if (mCountdownTimer <= 0xf0 && mCountdownTimer > 0x3c) {
            int q = mCountdownTimer / 60;
            if (q >= 4)
                q = 3;
            Mg_DrawNumberRotated(0x80, 0x60, q, -1, -1, 0x800, 0);
        } else {
            int mode = mState;
            if (mode == 1 || mode == 2 || (mode == 3 && mEndDelayTimer > 0) ||
                (mode == 4 && mCrashTimer <= 0x3c)) {
                int v = mTimeLeft;
                int q60 = v / 60;
                int centis = v % 60 * 100 / 60;
                int timerY;
                int ones;
                if (v < 0x258) {
                    timerY = 0xc;
                } else if (v >= 0x276) {
                    timerY = -0x104;
                } else {
                    timerY = ((0x258 - v) * 0x110) / 30 + 0xc;
                }
                Mg_DrawBigNumber(0x84, timerY, q60, -1, -1, 1, 0);
                Mg_DrawBigNumber(0x9c, timerY, centis / 10, -1, -1, 1, 0);
                ones = centis % 10;
                if (ones == 5 || ones == 0) {
                    Mg_DrawBigNumber(0xac, timerY, ones, -1, -1, 1, 0);
                } else {
                    Mg_DrawBigNumber(0xac, timerY, ones + 1, -1, -1, 1, 0);
                }
                Mg_RenderSpriteBothMtx(sSnowballTextSpritesByLanguage[GetGameLanguage()][3], 0x5c,
                                       timerY, 0);
                Mg_RenderSpriteBothMtx(sSnowballTextSpritesByLanguage[GetGameLanguage()][7], 0x90,
                                       timerY, 0);
            }
        }
    }

    Bit_DrawAll();
    SnowSpray_DrawAll();
    return 1;
}
#pragma opt_strength_reduction on

/* Slot 6. The five-state snowball roll: 0 counts in, 1 steers with the
 * touch screen and grows the ball, 2 and 3 are the crash, 4 is the melt and
 * 5 is over.
 */
s32 dScMgSnowball_c::Behavior() {
    int scrollStart, scrollMax;
    int speed;
    int slot, held, touched;
    int heading;
    int steer[2];
    int moved[2];
    Vector3 part;
    int v, q, t;
    unsigned short m;
    int gate;
    int id2, id1;
    Particle::System *p;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }

    mPrevRollAngle = mRollAngle;
    mPrevAngleY = mAngleY;
    mPrevHeadingAngle = mHeadingAngle;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;

    scrollMax = (mScrollLimit << 16) - 0x1d0000;
    scrollStart = mScrollY;

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

        slot = gActivePlayerSlot;
        held = gTouchHeld[slot * 4];
        touched = 0;
        if (held != 0)
            touched = (gTouchEdge[slot * 4] != 0);
        if (touched != 0) {
            int vb = ((volatile unsigned char *)gTouchY)[(unsigned int)slot * 4];
            int va = ((volatile unsigned char *)gTouchX)[(unsigned int)slot * 4];
            mTouchX = va;
            mTouchY = vb;
        } else if (held != 0) {
            steer[0] = ((gTouchX[(unsigned int)slot * 4] - mTouchX) << 12) / 296;
            steer[1] = ((gTouchY[(unsigned int)slot * 4] - mTouchY) << 12) / 296;
            mTouchX = gTouchX[(unsigned int)slot * 4];
            mTouchY = gTouchY[(unsigned int)slot * 4];
            if (Vec2_Len((const Vector2 *)steer) >= 0x400) {
                steer[0] = 0;
                steer[1] = 0;
            }
            heading = cstd::atan2(mVelY, mVelX);
            func_0203d388(steer, (short)-heading);
            steer[1] = steer[1] << 2;
            func_0203d388(steer, heading);
            if (mVelY >= 0) {
                if (steer[1] > 0)
                    steer[1] = 0;
            }
            if (steer[1] > 0) {
                if (steer[1] >= -mVelY) {
                    mVelY = 0;
                    steer[1] = 0;
                }
            }
            mVelX += steer[0];
            mVelY += steer[1];
        }

        if (mVelX != 0 || mVelY != 0) {
            if (Vec2_Len((const Vector2 *)&mVelX) >= 0x8000 && func_0203d434(&mVelX) != 0)
                func_0203d630(&mVelX, 0x8000);
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
        Vec2_Sub(moved, &mPosX, &mSoundPosX);
        gate = (Vec2_Len((const Vector2 *)moved) >= 0x30000) ? 1 : 0;
        if (gate != 0) {
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
        if (mScrollY > scrollStart)
            mScrollY = scrollStart;
        if (mScrollY < 0)
            mScrollY = 0;
        if (mScrollY >= scrollMax)
            mScrollY = scrollMax;

        if (Vec2_Len((const Vector2 *)&mVelX) >= 0x10) {
            v = Vec2_Len((const Vector2 *)&mVelX);
            q = v * 0x2710 / mBallSize;
            mRollAngle += (u16)q;
            m = cstd::atan2(mVelX, mVelY);
            ApproachLinear(mHeadingAngle, m, (Vec2_Len((const Vector2 *)&mVelX) / 16));
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
        slot = gActivePlayerSlot;
        held = gTouchHeld[slot * 4];
        touched = 0;
        if (held != 0)
            touched = (gTouchEdge[slot * 4] != 0);
        if (touched != 0)
            mEndDelayTimer = 0;
        UpdateSnowball();

        if (mVelX != 0 || mVelY != 0) {
            speed = Vec2_Len((const Vector2 *)&mVelX);
            if (mPosY < 0xc0000)
                speed -= 0x80;
            if (speed < 0) {
                mVelX = 0;
                mVelY = 0;
            } else if (func_0203d434(&mVelX) != 0) {
                func_0203d630(&mVelX, speed);
            }
        }

        mPosX += mVelX;
        mPosY += mVelY;
        if (mPosY < 0xe8000)
            mScreensSwapped = 0;
        else
            mScreensSwapped = 1;

        if (Vec2_Len((const Vector2 *)&mVelX) >= 0x10) {
            v = Vec2_Len((const Vector2 *)&mVelX);
            q = v * 0x2710 / mBallSize;
            mRollAngle += (u16)q;
            mHeadingAngle = cstd::atan2(mVelX, mVelY);
        }

        mScrollY = mPosY - 0x190000;
        if (mScrollY > scrollStart)
            mScrollY = scrollStart;
        if (mScrollY < 0)
            mScrollY = 0;
        if (mScrollY >= scrollMax)
            mScrollY = scrollMax;
        break;

    case 4:
        if (mCrashTimer < 0x3c) {
            if (mVelX != 0 || mVelY != 0) {
                speed = Vec2_Len((const Vector2 *)&mVelX) + 0x100;
                if (speed >= 0x8000)
                    speed = 0x8000;
                if (func_0203d434(&mVelX) != 0)
                    func_0203d630(&mVelX, speed);
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
                    part.x = t0;
                    part.y = t1;
                    part.z = 0x20000;
                    id1 =
                        Particle::System::New(0, 0xf1, part.x << 3, part.y << 3, part.z << 3, 0, 0);
                    id2 =
                        Particle::System::New(0, 0xf2, part.x << 3, part.y << 3, part.z << 3, 0, 0);
                    p = Particle::System::FromUniqueID(id1);
                    if (p != 0) {
                        /* 0x44 lies in Particle::System's unnamed pad_042. */
                        ((int *)p)[0x44 / 4] = 0xffff8222;
                        p->callbackParam = 0x2e14;
                        p->callbackVelocity = 0x29cf;
                        p->callbackScale = 0xc5f;
                    }
                    p = Particle::System::FromUniqueID(id2);
                    if (p != 0) {
                        p->callbackParam = 0x4b17;
                        p->callbackScale = 0x3999;
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

        if (Vec2_Len((const Vector2 *)&mVelX) >= 0x10) {
            v = Vec2_Len((const Vector2 *)&mVelX);
            q = v * 0x2710 / mBallSize;
            mRollAngle += (u16)q;
            mHeadingAngle = cstd::atan2(mVelX, mVelY);
        }
        break;

    case 5:
        mPromptEnabled = 0;
        break;
    }

    Bit_UpdateAll();
    SnowSpray_UpdateAll();
    UpdateCamera();
    return 1;
}

/* dScMgSnowball_c::OnGraphCallback2, from its vtable slot. */
int dScMgSnowball_c::OnGraphCallback2() {
    if (mMenuOpen == 0) {
        if (mScreensSwapped == 0) {
            GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
            SetBg2Offset(0, mScrollY >> 12);
            SetBg3Offset(0, mScrollY >> 12);
            SetSubBg2Offset(0x100, (mScrollY >> 12) + 0x110);
            SetSubBg3Offset(0, (mScrollY >> 12) + 0x110);
            G2_SetBG2Offset(0, mScrollY >> 12);
            G2_SetBG3Offset(0, mScrollY >> 12);
            G2S_SetBG2Offset(0x100, (mScrollY >> 12) + 0x110);
            G2S_SetBG3Offset(0, (mScrollY >> 12) + 0x110);
        } else {
            GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
            SetBg2Offset(0x100, (mScrollY >> 12) + 0x110);
            SetBg3Offset(0, (mScrollY >> 12) + 0x110);
            SetSubBg2Offset(0, mScrollY >> 12);
            SetSubBg3Offset(0, mScrollY >> 12);
            G2_SetBG2Offset(0x100, (mScrollY >> 12) + 0x110);
            G2_SetBG3Offset(0, (mScrollY >> 12) + 0x110);
            G2S_SetBG2Offset(0, mScrollY >> 12);
            G2S_SetBG3Offset(0, mScrollY >> 12);
        }
    }
    return dScMgBase_c::OnGraphCallback2() != 0;
}

/* dScMgSnowball_c::OnGraphCallback3, from its vtable slot. */
int dScMgSnowball_c::OnGraphCallback3() {
    return dScMgBase_c::OnGraphCallback3() != 0;
}

/* dScMgSnowball_c::OnGraphCallback0, from its vtable slot. */
int dScMgSnowball_c::OnGraphCallback0() {
    return dScMgBase_c::OnGraphCallback0() != 0;
}

/* Slot 3. Frees the BMD that InitResources parked in mBallBmd. */
s32 dScMgSnowball_c::CleanupResources() {
    Ov004_Deallocate(mBallBmd);
    return 1;
}

/* Slot 18, an override of dScMgBase_c::StartRound(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgSnowball_c::StartRound(int i) {
    Mg_SubmitScore(0);
    ResetScene();
    if (i != 0x13)
        return;
    BuildCourse();
    RedrawCourse();
}

/* Slot 0. Sets up both screens' BG2 and BG3 layers, loads the palettes and
 * tiles, then loads the snowball BMD into mBallBmd and hands it to mModel.
 * Returning 0 when dExtModel_c::SetFile fails aborts the scene. */
s32 dScMgSnowball_c::InitResources() {
    void *buf;

    data_0208ee44 = 1;
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    mScreensSwapped = 1;

    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x14000);
    G2_BG2Mosaic(false);
    G2_SetBG2Offset(0, 0);
    G2_SetBG2Priority(2);

    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                          GX_BG_CHARBASE_0x14000);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2Priority(2);

    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                         GX_BG_CHARBASE_0x14000);
    G2_BG3Mosaic(false);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3Priority(3);

    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb000,
                          GX_BG_CHARBASE_0x14000);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3Priority(3);

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKIDAMA_NCG_BIN, (u8 *)G2_GetBG2CharPtr() + 0x4000);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKIDAMA_NCG_BIN, (u8 *)G2S_GetBG2CharPtr() + 0x4000);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKA_NSC_BIN, G2_GetBG3ScrPtr());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_YUKA_NSC_BIN, G2S_GetBG3ScrPtr());

    gMainScreenLayers |= DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3;
    gSubScreenLayers |= 0xc;

    buf = Ov004_LoadFileByPath(&sSnowballBgNclPath);
    DC_FlushRange(buf, 0x100);
    GX_LoadBGPltt(buf, 0x100, 0x100);
    GXS_LoadBGPltt(buf, 0x100, 0x100);
    Ov004_Deallocate(buf);

    buf = Ov004_LoadFileByPath(&sSnowballNcgPath);
    DecompressLZ16(buf, G2_GetOBJCharPtr());
    DecompressLZ16(buf, G2S_GetOBJCharPtr());
    Ov004_Deallocate(buf);

    buf = Ov004_LoadFileByPath(&sSnowballNclPath);
    DC_FlushRange(buf, 0x100);
    GX_LoadOBJPltt(buf, 0, 0x100);
    GXS_LoadOBJPltt(buf, 0, 0x100);
    Ov004_Deallocate(buf);

    G2_SetBG0Priority(1);
    G3X_SetFog(false, 0, 2, 0x1000);
    G3X_AlphaBlend(true);
    InitialiseVramGlobals();

    mBallBmd = (BMD_File *)Ov004_LoadFileByPath(&data_ov006_021400fc);
    dExtSimpleModel_c::UpdateFileOffsets(*mBallBmd);

    if (!mModel.SetFile(mBallBmd, 1, -1))
        return 0;

    G2_SetBG0Priority(1);
    gMainScreenLayers |= DISPLAY_LAYER_BG0;

    ResetScene();
    BuildCourse();
    RedrawCourse();
    return 1;
}

/* Everything but the `= 0` at 0xabf4 is compiler-generated: the base C2, the
   two vptr stores (dScMgSingle3DBase_c's, then this class's own, which lands
   on the cartridge's _ZTV15dScMgSnowball_c), the tracker at 0x471c, mModel,
   and the three element arrays through __cxa_vec_ctor (mArray1 and mArray2
   with Vector2's func_0203d738 / NullDestructor_0203d47c, mSprays with
   SnowSpray's pair), in the order ~dScMgSnowball_c unwinds them. */
dScMgSnowball_c::dScMgSnowball_c() : mBallBmd(0) {}

/* SnowSpray's empty inline constructor (dScMgSnowball_c.h), emitted here. */

void dScMgSnowball_c::Bit_DrawAll() {
    int i;
    for (i = 0; i < 50; i++) {
        if (mBits[i].visible != 0) {
            int bb = mBits[i].x >> 12;
            int cc = (mBits[i].y - mScrollY) >> 12;
            Mg_RenderSubSpriteMtx(sSnowballBitSprites[mBits[i].spriteA], bb, cc, 0);
            DrawOamSprite(sSnowballBitSprites[mBits[i].spriteB], bb, cc, 0);
        }
    }
}

void dScMgSnowball_c::Bit_StateFallSlowDown(int idx) {
    if (mBits[idx].fallTimer != 0) {
        mBits[idx].fallTimer--;
        if ((s16)mBits[idx].fallTimer < 0)
            mBits[idx].fallTimer = 0;
        return;
    }
    if (mBits[idx].vy > 0x100) {
        mBits[idx].vy -= 0x10;
        if ((s16)mBits[idx].vy < 0x100)
            mBits[idx].vy = 0x100;
        return;
    }
    mBits[idx].fallState = 0;
}

void dScMgSnowball_c::Bit_StateFallSpeedUp(int idx) {
    if (mBits[idx].fallSpeed > mBits[idx].vy) {
        mBits[idx].vy += 0x10;
        if (mBits[idx].fallSpeed > mBits[idx].vy)
            mBits[idx].vy = mBits[idx].fallSpeed;
    }
    if (mBits[idx].fallTimer != 0) {
        mBits[idx].fallTimer--;
        if ((s16)mBits[idx].fallTimer < 0)
            mBits[idx].fallTimer = 0;
        return;
    }
    mBits[idx].fallState = 2;
    mBits[idx].fallTimer = (u8)(MgRandom(32) + 0x20);
}

void dScMgSnowball_c::Bit_StateFallStart(int i) {
    mBits[i].vy = 0;
    mBits[i].fallSpeed = (MgRandom(16) << 4) + 0x300;
    mBits[i].fallState = 1;
    mBits[i].fallTimer = (u8)(MgRandom(32) + 0x20);
}

void dScMgSnowball_c::BitA_StateDriftStop(int idx) {
    mBits[idx].x += mBits[idx].vx;
    mBits[idx].y += mBits[idx].vy;
    if (mBits[idx].holdTimer != 0) {
        mBits[idx].driftTimer--;
        if ((s16)mBits[idx].driftTimer < 0)
            mBits[idx].driftTimer = 0;
        return;
    }
    if (mBits[idx].vx > 0) {
        mBits[idx].vx -= 8;
        if ((s16)mBits[idx].vx < 0)
            mBits[idx].vx = 0;
        return;
    }
    if (mBits[idx].vx < 0) {
        mBits[idx].vx += 8;
        if (mBits[idx].vx > 0)
            mBits[idx].vx = 0;
        return;
    }
    mBits[idx].driftState = 0;
}

void dScMgSnowball_c::BitA_StateDriftLeft(int i) {
    mBits[i].x += mBits[i].vx;
    mBits[i].y += mBits[i].vy;
    if (mBits[i].holdTimer != 0) {
        mBits[i].holdTimer--;
        if ((s16)mBits[i].holdTimer < 0)
            mBits[i].holdTimer = 0;
        return;
    }
    if (mBits[i].vx > -0x300) {
        mBits[i].vx -= 8;
        if (mBits[i].vx <= -0x300)
            mBits[i].vx = 0x300;
    }
    if (mBits[i].driftTimer != 0) {
        mBits[i].driftTimer--;
        if ((s16)mBits[i].driftTimer < 0)
            mBits[i].driftTimer = 0;
        return;
    }
    mBits[i].driftState = 3;
    mBits[i].driftTimer = (u8)(MgRandom(32) + 0x20);
}

void dScMgSnowball_c::BitA_StateDriftRight(int index) {
    mBits[index].x += mBits[index].vx;
    mBits[index].y += mBits[index].vy;
    if (mBits[index].holdTimer != 0) {
        mBits[index].holdTimer--;
        if ((s16)mBits[index].holdTimer < 0)
            mBits[index].holdTimer = 0;
        return;
    }
    if (mBits[index].vx < 0x300) {
        mBits[index].vx += 8;
        if (mBits[index].vx >= 0x300)
            mBits[index].vx = 0x300;
    }
    if (mBits[index].driftTimer != 0) {
        mBits[index].driftTimer--;
        if ((s16)mBits[index].driftTimer < 0)
            mBits[index].driftTimer = 0;
        return;
    }
    mBits[index].driftState = 3;
    mBits[index].driftTimer = (u8)(MgRandom(32) + 0x20);
}

void dScMgSnowball_c::BitA_StatePause(int i) {
    if (mBits[i].holdTimer != 0) {
        mBits[i].holdTimer--;
        if ((s16)mBits[i].holdTimer < 0)
            mBits[i].holdTimer = 0;
        return;
    }
    mBits[i].vx = 0;
    mBits[i].driftState = data_ov006_0212f3b0[MgRandom(2)];
    mBits[i].holdTimer = (u8)MgRandom(0x10);
    mBits[i].driftTimer = (u8)(MgRandom(0x40) + 0x60);
}

typedef void (dScMgSnowball_c::*PMF_02129d94)(int);
struct Entry_02129d94 {
    PMF_02129d94 pmf;
};
extern Entry_02129d94 sSnowballBitAStateTable[];
extern Entry_02129d94 sSnowballBitStateTable[];
/* Through a dScMgSnowball_c * copy of this: bare mBits[i] renumbers @NNN. */
void dScMgSnowball_c::Bit_RunKindA(int i) {
    (this->*sSnowballBitAStateTable[mBits[i].driftState].pmf)(i);
    (this->*sSnowballBitStateTable[mBits[i].fallState].pmf)(i);
}

void dScMgSnowball_c::BitB_StateDriftStop(int i) {
    mBits[i].x += mBits[i].vx;
    mBits[i].y += mBits[i].vy;
    if (mBits[i].vx > 0) {
        mBits[i].vx -= 0x20;
        if ((s16)mBits[i].vx < 0)
            mBits[i].vx = 0;
    } else if (mBits[i].vx < 0) {
        mBits[i].vx += 0x20;
        if (mBits[i].vx > 0)
            mBits[i].vx = 0;
    } else {
        mBits[i].driftState = 0;
    }
}

void dScMgSnowball_c::BitB_StateDriftLeft(int idx) {
    mBits[idx].x += mBits[idx].vx;
    mBits[idx].y += mBits[idx].vy;
    if (mBits[idx].holdTimer != 0) {
        mBits[idx].holdTimer--;
        if ((s16)mBits[idx].holdTimer < 0)
            mBits[idx].holdTimer = 0;
        return;
    }
    if (mBits[idx].vx > -0x400) {
        mBits[idx].vx -= 0x20;
        if (mBits[idx].vx <= -0x400)
            mBits[idx].vx = 0x400;
    }
    if (mBits[idx].driftTimer != 0) {
        mBits[idx].driftTimer--;
        if ((s16)mBits[idx].driftTimer < 0)
            mBits[idx].driftTimer = 0;
        return;
    }
    mBits[idx].driftState = 3;
}

void dScMgSnowball_c::BitB_StateDriftRight(int index) {
    mBits[index].x += mBits[index].vx;
    mBits[index].y += mBits[index].vy;
    if (mBits[index].holdTimer != 0) {
        mBits[index].holdTimer--;
        if ((s16)mBits[index].holdTimer < 0)
            mBits[index].holdTimer = 0;
        return;
    }
    if (mBits[index].vx < 0x400) {
        mBits[index].vx += 0x20;
        if (mBits[index].vx >= 0x400)
            mBits[index].vx = 0x400;
    }
    if (mBits[index].driftTimer != 0) {
        mBits[index].driftTimer--;
        if ((s16)mBits[index].driftTimer < 0)
            mBits[index].driftTimer = 0;
        return;
    }
    mBits[index].driftState = 3;
}

void dScMgSnowball_c::BitB_StatePause(int idx) {
    if (mBits[idx].holdTimer != 0) {
        mBits[idx].holdTimer--;
        if ((s16)mBits[idx].holdTimer < 0)
            mBits[idx].holdTimer = 0;
        return;
    }
    mBits[idx].vx = 0;
    mBits[idx].vy = (MgRandom(32) << 4) + 0x600;
    mBits[idx].driftState = data_ov006_0212f3ac[MgRandom(2)];
    mBits[idx].holdTimer = (u8)MgRandom(16);
    mBits[idx].driftTimer = (u8)(MgRandom(0x30) + 0x30);
}

typedef void (dScMgSnowball_c::*PMF_0212a224)(int);
extern "C" PMF_0212a224 sSnowballBitBStateTable[];

void dScMgSnowball_c::Bit_RunKindB(int idx) {
    (this->*sSnowballBitBStateTable[mBits[idx].driftState])(idx);
}

void dScMgSnowball_c::Bit_PickKind(int idx) {
    unsigned int v = MgRandom(8);
    int b = 0;
    if (v == 5)
        b = 1;
    mBits[idx].kind = data_ov006_0212f3b4[b];
    mBits[idx].driftState = 0;
}

typedef void (dScMgSnowball_c::*PMF_0212a2e0)(int);

extern "C" PMF_0212a2e0 sSnowballBitKindTable[];

void dScMgSnowball_c::Bit_UpdateAll() {
    int i;
    for (i = 0; i < 0x32; i++) {
        if (mBits[i].active != 0) {
            (this->*sSnowballBitKindTable[mBits[i].kind])(i);
            if ((mBits[i].y - mScrollY) >> 12 >= 0xc8) {
                mBits[i].x = MgRandom(32) << 15;
                mBits[i].y = mScrollY - 0x8000;
                mBits[i].driftState = 0;
                mBits[i].kind = 0;
                mBits[i].fallState = 0;
            }
        }
    }
}

/* The y and spriteB draws keep the raw value in `r`: through MgRandom the
 * 0x18 multiplier and this + 0xa000 trade registers (11 words). */
void dScMgSnowball_c::Bit_InitAll() {
    int i;
    int j;
    unsigned int r;
    unsigned int v;
    int q;

    for (i = 0; i < 0x32; i++) {
        mBits[i].x = 0;
        mBits[i].y = 0;
        mBits[i].vx = 0;
        mBits[i].vy = 0;
        mBits[i].holdTimer = 0;
        mBits[i].driftTimer = 0;
        mBits[i].fallTimer = 0;
        mBits[i].active = 0;
        mBits[i].kind = 0;
        mBits[i].driftState = 0;
        mBits[i].fallState = 0;
        mBits[i].visible = 0;
        mBits[i].spriteA = 0;
        mBits[i].spriteB = 1;
    }

    for (j = 0; j < 0x32; j++) {
        mBits[j].x = MgRandom(32) << 15;
        r = RandomIntInternal(&data_0209d4b8.value);
        q = (((r >> 16) & 0x7fff) * 0x18) >> 15;
        mBits[j].y = mScrollY - 0x8000 + (q << 15);
        mBits[j].active = 1;
        mBits[j].visible = 1;
        mBits[j].kind = 0;
        mBits[j].driftState = 0;
        mBits[j].spriteA = MgRandom(5);
        r = RandomIntInternal(&data_0209d4b8.value);
        v = mBits[j].spriteA + ((((r >> 16) & 0x7fff) * 4) >> 15) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        mBits[j].spriteB = v;
        mBits[j].holdTimer = 0;
    }
}

/* MG_SNOWBALL's base profile, between the PMF descriptors: classInit, then
 * the same priority (0x179, the registry id) in both halfwords. */
dScMgSnowball_c *dScMgSnowball_c_classInit(void);
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

/* Reconstructed source-style name: SM64DS proves dScMgSnowball_c through RTTI,
 * allocation size, vtable identity, and the MG_SNOWBALL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSnowballSlalom_Spawn.
 * `new` goes through fBase_c's operator new for the 0xc59c bytes and runs
 * the real constructor above on them. */
dScMgSnowball_c *dScMgSnowball_c_classInit(void) {
    return new dScMgSnowball_c;
}
