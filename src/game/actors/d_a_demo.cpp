//cpp
/* daDemo_c -- the KuppaScript cutscene actor, registry profile 352 (0x160),
 * debug name DEMO.
 *
 * How it is driven. ProcessKuppaScript walks the current script's records
 * (s[0] record length, s[1] command, s[2..3] start frame, s[4..5] end frame,
 * compared against gKuppaScriptFrame). Commands 0x12 and up go to
 * RunDemoCommand: the command byte is used as a param1. FindDemoActor looks
 * for a live actor with that param1 among the 16 IDs at gDemoActorIDs; if
 * none, SpawnDemoActor spawns actor 0x160 with that param1 into a free slot.
 * daDemo_c::RunCommand then calls entry s[6] of the 24-entry daDemo_c::*
 * table data_ov002_0211104c with (s + 7, start, end). Handlers that compare
 * a frame against gKuppaScriptFrame are acting on the record's first or last
 * frame.
 *
 * Handler table, index -> member (func_ov002_020fXXXX):
 *    0 7bb8   1 79c0   2 7780   3 7538   4 7410   5 7384   6 72bc   7 71c4
 *    8 7038   9 7020  10 6e48  11 6c60  12 6c34  13 6c24  14 6bc0  15 6b4c
 *   16 6b28  17 6ae4  18 6ab8  19 6a9c  20 6448  21 63f8  22 26d4  23 26c4
 *
 * param1 picks the variant (InitResources, Behavior, Render, Cleanup):
 *   0x12        anmModel_c, model data_ov085_0213074c
 *   0x13        anmModel_c; file set chosen by CURRENT_GAMEMODE == 2
 *   0x14..0x16  simpleModel_c, data_ov002_02110b70
 *   0x17, 0x18  simpleModel_c, data_ov002_02110b50 / data_ov002_0211094c
 *   0x19        simpleModel_c, data_ov002_02110b78; Render draws it three
 *               times at the positions in data_ov002_0210bb7c
 *   0x1a..0x2d  anmModel_c from data_ov009_02113c20; moved with
 *               UpdatePosWithOnlySpeed and rotated ZXY instead of XYZ
 *   0x2e        the 0x518-byte block at mStaffRoll (handler 20 allocates it;
 *               Behavior 63d4, Cleanup 63a0)
 *   0x2f        the 0x36-byte ScrollState at mScroll (handler 22 allocates
 *               it through the 26d4 veneer to 2630; Behavior 23f0, Cleanup
 *               23d0): two scrolling sub-screen BG layers, a sub-screen
 *               blend fade, two centre sprites and three sprites that cross
 *               the screen.
 * The model variants all start at scale 1.0 (0x1000).
 *
 * ROM: _ZTS8daDemo_c at ov002:0x0210b940, _ZTI8daDemo_c at 0x0210b94c
 * over _ZTI8dActor_c, _ZTV8daDemo_c at 0x0210bd60. ~daDemo_c is the key
 * function (D1 0x020f1f70, D0 0x020f1f94).
 *
 * Leftover:
 *   2aec, 2bf4, 335c, 340c and 39ec keep the per-member brackets.
 *   27e8 and the OAM tail return the callee's r0, so those callees stay int.
 *   26d4 tail-calls 2630 through void(*)(void), so both stay free functions.
 *   Descriptor handlers are static members. A pointer to member does not
 *   fit the int word a .data descriptor holds.
 *   common.h comes before daDemo_c.h, or Behavior's matrix copy scalarizes.
 *   simpleModel_c's destructor is compiled with defer_codegen on.
 *   anmModel_c's class operator delete is what calls Memory::operator_delete2.
 *   The file-handle wrappers stay below func_ov002_020f6c60.
 *   Render copies the view matrix through the three-pointer struct.
 *   Cleanup's null tests stay around the two deletes.
 *   43cc's speed clamp, 55b4's step and 5894's kira fields add the index
 *   in bytes to the field address.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDemo_c_AssetId {
    ASSET_D_2D_ED_STAR_NCG_BIN = 0x022c,
    ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCG_BIN = 0x024f,
    ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCL_BIN = 0x0250,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCG_BIN = 0x0251,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCL_BIN = 0x0252,
    ASSET_D_2D_STAFF_ROLL_CAKE_BRIGHT_NSC_BIN = 0x0253,
    ASSET_D_2D_STAFF_ROLL_CAKE_DARK1_NSC_BIN = 0x0254,
    ASSET_D_2D_STAFF_ROLL_CAKE_DARK2_NSC_BIN = 0x0255,
    ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCG_BIN = 0x0256,
    ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCL_BIN = 0x0257,
    ASSET_D_2D_STAFF_ROLL_END_EUR_NCG_BIN = 0x0267,
    ASSET_D_2D_STAFF_ROLL_END_EUR_NCL_BIN = 0x0268,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_F_NCG_BIN = 0x026b,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_G_NCG_BIN = 0x026d,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_I_NCG_BIN = 0x026f,
    ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_S_NCG_BIN = 0x0271
};

#include "KuppaScript.h"
#include "types.h"
#include "CP15.h"
#include "G2.h"
#include "G2x.h"
#include "GX.h"
#include "Memory.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/gx_bgcnt.h"
#include "common.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "daDemo_c.h"
#include "CutsceneSpline.h"
#include "SharedFilePtr.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"
#include "dFdBrightness_c.h"
#include "OAM.h"
#include "SaveData.h"
#include "dExtAnmTexPat_c.h"
#include "dCamera_c.h"
#include "cstd.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "ParticleIds.h"

struct OamAttr;

#pragma defer_codegen off

/* The staff-roll block's helpers are StaffRoll members; the typedef lets
   the file spell them without the daDemo_c:: qualifier. */
typedef daDemo_c::StaffRoll StaffRoll;

typedef signed short s16;

struct OamEnt {
    s16 x;       /* 0 */
    s16 y;       /* 2 */
    s16 ang;     /* 4 */
    s16 pad;     /* 6 */
    int enabled; /* 8 */
};

struct ParticleCallback;

typedef void (StaffRoll::*PMF)(int);

/* pointer-to-member taking no args (dispatch tables at 02110f34/02110f9c) */
typedef void (StaffRoll::*PMF0)();

/* pointer-to-member taking (cmd, a2, a3) — RunCommand's table */
typedef void (daDemo_c::*PMF3)(unsigned char *, int, int);

typedef void (StaffRoll::*PMF37)(int);
struct Entry37 {
    PMF37 pmf;
};

typedef void (StaffRoll::*PMF2dd4)(int);
struct Entry2dd4 {
    PMF2dd4 pmf;
};

union W {
    int w[2];
    PMF pmf;
    PMF0 pmf0;
    PMF3 pmf3;
    PMF37 pmf37;
    PMF2dd4 pmf2dd4;
};

struct Ent {
    int out0;
    int out4;
    char pad8[8];
    int vx;
    char pad14[0x1a];
    u16 angle;
    char pad30[0x1c];
};

struct BMD_File;

typedef int s32;

typedef unsigned char u8;

typedef unsigned short u16;

typedef int Fix12i;

/* Lakitu's model-file handle, owned and constructed by d_a_c_jugem.cpp's
 * static init. Same TU-local wrapper spelling as the owning TU. */
struct JugemModelFile : SharedFilePtr {
    JugemModelFile(u32 fileID);
    ~JugemModelFile();
};

extern "C" {
extern unsigned char gSubScreenLayers;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern OamAttr data_ov002_0210bddc;
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
/* The scene fader: dScene_c::SetFaders(dFdBrightness_c *) assigns it. Kept
   as the void * this file has always declared (the tree's declarations
   disagree); 23f0 casts at the one use. */
extern void *gSceneFader;
extern u8 data_ov002_02111144;
extern u32 data_020a0db0;
extern int func_ov002_020f2630(daDemo_c *self);
extern struct OamAttr *data_ov002_0210b6a8[];

extern u32 d_a_demoNewHold(u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void *rot,
                           struct ParticleCallback *callback);
extern int data_ov002_02100140[];
extern int GetOwnerLanguage(void);
extern void OamRender10_d_a_demo(int a, struct OamAttr *attr, int x, int y, int e, int f, int g,
                                 int h, int i, int j);
extern struct OamAttr *data_ov002_0210b988[];
extern void func_0201ef50(unsigned char v);
extern void func_0201f108(void);
extern struct OamAttr *data_ov002_0210bba0[];
extern int *data_ov002_0210b97c[];
extern void SetSubBg2Offset(int a, int b);
extern PMF data_ov002_02110e24[];
extern PMF data_ov002_02110e9c[];
extern void MultiCopyHalf(void *src, void *dst, int count);
extern int data_ov002_021002a0[];
extern s16 data_02082214[];
extern short atan2_hold_d_a_demo_1(int y, int x);
extern short data_02082214[];
extern PMF data_ov002_02110e34[];
extern unsigned short data_ov002_021000c0[];
extern int data_ov002_02100120[];
extern int data_ov002_02100130[];
extern unsigned short data_ov002_021000b8[];
extern int data_ov002_02100100[];
extern int data_ov002_02100110[];
extern unsigned short data_ov002_021000c8[];
extern int data_ov002_021000e0[];
extern int data_ov002_021000f0[];
extern unsigned short data_ov002_021000d0[];
extern int data_ov002_02100150[];
extern int data_ov002_02100160[];
extern unsigned short data_ov002_021000d8[];
extern PMF data_ov002_02110eec[];
extern int data_ov002_02100190[];
extern int data_ov002_02100170[];
extern PMF data_ov002_02110e64[];
extern PMF data_ov002_02110e7c[];
extern PMF0 data_ov002_02110f9c[];
extern void SetSubBg3Offset(int a, int b);
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *p);
extern void func_02056434(const void *src, int offset, int count);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void func_02056374(const void *src, u32 offset, u32 count);
extern void func_02012694(unsigned int, void *);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern int *Vec3_LslInPlace(int *v, int sh);
extern u16 gKuppaScriptFrame;
extern char *gCamera;
extern int data_ov002_0210b614;
extern int data_ov002_0210b958;
extern s16 Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);
extern int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern void func_0201f138(void);
extern void func_0201ef38(void);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern void Vec3_MulScalar(VecFx32 *out, const VecFx32 *in, int scale);
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

extern int data_0208ee44;
extern int data_ov002_0210b970[3];
extern unsigned char gLocalPlayerId;
extern void *gPlayers[];
/* void *: every caller passes &mPosX and POD int[3] scratch; retyping that
   scratch as Vector3 renumbers this TU's @NNN literals. */
extern void Vec3_RotateYAndTranslate(void *out, void *in, short angle, void *src);
extern int data_ov002_0210b964[3];
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzLen(const Vector3 *v);
extern PMF3 data_ov002_0210b618;
extern PMF3 data_ov002_0210b620;
extern PMF3 data_ov002_0210b640;
extern PMF3 data_ov002_0210b628;
extern PMF3 data_ov002_0210b648;
extern W data_ov002_0210b678;
extern W data_ov002_0210b708;
extern W data_ov002_0210b750;
extern W data_ov002_0210b8c0;
extern W data_ov002_0210b8c8;
extern W data_ov002_0210b8d0;
extern W data_ov002_0210b8d8;
extern W data_ov002_0210b8e0;
extern W data_ov002_0210b8e8;
extern W data_ov002_0210b8f0;
extern W data_ov002_0210b8f8;
extern W data_ov002_0210b900;
extern W data_ov002_0210b908;
extern W data_ov002_0210b910;
extern W data_ov002_0210b918;
extern W data_ov002_0210b920;
extern W data_ov002_0210b928;
extern W data_ov002_0210b930;
extern W data_ov002_0210b938;
extern PMF3 data_ov002_0211104c[24];
/* local extern: the Vector3 spelling math/CalcLib.h declares. The locals
   are the POD VecFx32, so the call casts. */
extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s32 x, s32 y, s32 z);
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 data_0209b41c;
extern unsigned char gGameMode;
extern char data_ov002_0211094c;
extern JugemModelFile data_ov085_0213074c;
/* The file handles and their registration nodes are defined at the
   bottom of this file; only the guard word and the 12-byte gap between
   the two out-of-order nodes stay here so the function-local Vector3
   statics keep their retail addresses. */
int data_ov002_02110b00;
int data_ov002_02110ddc[3];
}

daDemo_c::~daDemo_c() {}

/* Sub-screen fade step (param1 0x2f, states 2 and 3). While fadeLevel is
 * non-zero it counts fadeTimer to 0x18, then drops fadeLevel by one and calls
 * G2x::SetBlendAlpha on 0x4001050 with (4, 0x28, fadeLevel, 0x10 - fadeLevel).
 * When fadeLevel reaches zero it clears bit 2 of gSubScreenLayers, writes that
 * mask into bits 8..12 of 0x4001000, sets the blend to (0, 0x28, 0xc, 4),
 * advances the state and gives the three sprite slots delays 0x10/0x20/0x30. */

void daDemo_c::func_ov002_020f1fcc() {
    int i, v;
    if (mScroll->fadeLevel == 0)
        return;
    mScroll->fadeTimer++;
    if (mScroll->fadeTimer != 0x18)
        return;
    mScroll->fadeTimer = 0;
    mScroll->fadeLevel--;
    v = mScroll->fadeLevel;
    if (v != 0) {
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 0x28, v, 0x10 - v);
        return;
    }
    gSubScreenLayers &= ~4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (gSubScreenLayers << 8);
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 0, 0x28, 0xc, 4);
    mScroll->state++;
    for (i = 0; i < 3; i++) {
        mScroll->spriteDelay[i] = (i + 1) << 4;
    }
}

/* Moving-sprite step. For each active slot, on frames where FRAME_TIMER bit 0
 * is clear, spriteX += spriteVelX and the RNG is stepped (result unused); a
 * slot that reaches x 0x140 moving right or x -0x40 moving left goes idle with
 * spriteDelay (slot + 1) << 5. Every active slot is drawn each frame. */
extern "C" struct OamAttr *data_ov002_0210be1c[];

void daDemo_c::func_ov002_020f20f4() {
    for (int i = 0; i < 3; i++) {
        if (mScroll->spriteActive[i] != 0) {
            if ((data_020a0db0 & 1) == 0) {
                mScroll->spriteX[i] += mScroll->spriteVelX[i];
                RandomIntInternal(&data_0209e650);
                if (mScroll->spriteVelX[i] >= 0) {
                    if (mScroll->spriteX[i] >= 0x140) {
                        mScroll->spriteActive[i] = 0;
                        mScroll->spriteDelay[i] = (i + 1) << 5;
                    }
                } else if (mScroll->spriteX[i] <= -0x40) {
                    mScroll->spriteActive[i] = 0;
                    mScroll->spriteDelay[i] = (i + 1) << 5;
                }
            }
            OAM::RenderSub(data_ov002_0210be1c[i], mScroll->spriteX[i], mScroll->spriteY[i], -1, 2);
        }
    }
}

/* Sprite respawn. Counts each slot's non-zero spriteDelay down; when it hits
 * zero on an idle slot, RNG bit 0 picks x -0x40 moving right or x 0x140 moving
 * left, and y is (rng >> 16) % 48 + 0x30 * slot. Does nothing in state 6. */

void daDemo_c::func_ov002_020f2210() {
    int i;
    int base;
    u8 state;
    state = mScroll->state;
    /* The redundant `state >= 3` is compiled into the ROM: `state > 5` alone
       builds 8 bytes short. */
    if (state >= 3 && state > 5)
        return;
    i = 0;
    base = 0;
    for (; i < 3; i++) {
        if (mScroll->spriteDelay[i] != 0) {
            mScroll->spriteDelay[i] -= 1;
            if (mScroll->spriteDelay[i] == 0) {
                if (mScroll->spriteActive[i] == 0) {
                    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650);
                    if (r & 1) {
                        mScroll->spriteX[i] = -0x40;
                        mScroll->spriteVelX[i] = 1;
                    } else {
                        mScroll->spriteX[i] = 0x140;
                        mScroll->spriteVelX[i] = -1;
                    }
                    mScroll->spriteY[i] = (u16)((r >> 16) % 48 + base);
                    mScroll->spriteActive[i] = 1;
                }
            }
        }
        base += 0x30;
    }
}

/* Draw the two centre sprites at x 0x80. */

void daDemo_c::func_ov002_020f2340() {
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[0]);
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[1]);
}

/* Scroll the two sub-screen BG layers in opposite directions (bg1 x down
 * one, bg0 x up one) and write both offsets to the hardware. */

void daDemo_c::func_ov002_020f237c() {
    mScroll->bg1X -= 1;
    mScroll->bg0X += 1;
    SetSubBg0Offset(mScroll->bg0X, mScroll->bg0Y);
    SetSubBg1Offset(mScroll->bg1X, mScroll->bg1Y);
}

/* CleanupResources for param1 0x2f: free the scroll block. */

int daDemo_c::func_ov002_020f23d0() {
    Memory::operator_delete2(this->mScroll);
    return 1;
}

/* Behavior for param1 0x2f: the scroll block's seven states.
 *   0  each frame the scene fader (gSceneFader) reports IsAtStart,
 *      centerSpriteY[0]++ and centerSpriteY[1]--; next state when
 *      centerSpriteY[0] reaches 8 (it starts at -8)
 *   1  wait for mCue == 3 (written by script handler 26c4), then set
 *      data_ov002_02111144 to 1
 *   2  on frames with FRAME_TIMER bit 0 clear, bg1Y++ and bg0Y--; at
 *      bg1Y 0x80, next state and sprite delays 1/0x41/0x81. Also runs the
 *      horizontal scroll (237c) and the fade step (1fcc)
 *   3  fade step and horizontal scroll; the fade step advances the state
 *      when fadeLevel reaches zero
 *   4  wait for mCue == 4, horizontal scroll
 *   5  on frames with FRAME_TIMER bit 0 clear, bg1Y++ and bg0Y--; at
 *      bg1Y 0xe0, next state and bits 0..1 of gSubScreenLayers cleared into
 *      bits 8..12 of 0x4001000. Also runs the horizontal scroll (237c)
 *   6  nothing
 * Every state then runs 2340, 2210 and 20f4. */

int daDemo_c::func_ov002_020f23f0() {
    switch (mScroll->state) {
    case 0:
        if (((dFdBrightness_c *)gSceneFader)->IsAtStart() != 0) {
            mScroll->centerSpriteY[0]++;
            mScroll->centerSpriteY[1]--;
            if (mScroll->centerSpriteY[0] == 8) {
                mScroll->state++;
            }
        }
        break;
    case 1:
        if (this->mCue == 3) {
            data_ov002_02111144 = 1;
            mScroll->state++;
        }
        break;
    case 2:
        if ((data_020a0db0 & 1) == 0) {
            mScroll->bg1Y++;
            mScroll->bg0Y--;
            if (mScroll->bg1Y >= 0x80) {
                int i;
                int v;
                mScroll->state++;
                i = 0;
                v = 1;
                for (; i < 3; i++) {
                    mScroll->spriteDelay[i] = v;
                    v += 0x40;
                }
            }
        }
        func_ov002_020f237c();
        func_ov002_020f1fcc();
        break;
    case 3:
        func_ov002_020f1fcc();
        func_ov002_020f237c();
        break;
    case 4:
        if (this->mCue == 4) {
            mScroll->state++;
        }
        func_ov002_020f237c();
        break;
    case 5:
        if ((data_020a0db0 & 1) == 0) {
            mScroll->bg1Y++;
            mScroll->bg0Y--;
            if (mScroll->bg1Y >= 0xe0) {
                mScroll->state++;
                gSubScreenLayers = gSubScreenLayers & ~3;
                *(volatile u32 *)0x4001000 =
                    (*(volatile u32 *)0x4001000 & ~0x1f00) | (gSubScreenLayers << 8);
            }
        }
        func_ov002_020f237c();
        break;
    case 6:
        break;
    }

    func_ov002_020f2340();
    func_ov002_020f2210();
    func_ov002_020f20f4();
    return 1;
}

extern "C" { /* stays free: func_ov002_020f26d4 tail-calls it through a raw void(*)(void) */
int func_ov002_020f2630(daDemo_c *self) {
    self->mScroll = (daDemo_c::ScrollState *)::operator new(0x36);
    self->mScroll->bg0X = 0x30;
    self->mScroll->bg0Y = 0x20;
    self->mScroll->bg1X = 0;
    self->mScroll->bg1Y = 0;
    self->mScroll->centerSpriteY[0] = 0xfff8;
    self->mScroll->centerSpriteY[1] = 0xc8;
    self->mScroll->fadeLevel = 0x10;
    SetSubBg0Offset(self->mScroll->bg0X, self->mScroll->bg0Y);
    SetSubBg1Offset(self->mScroll->bg1X, self->mScroll->bg1Y);
    return 1;
}
}

/* Script handler (table entry 23): mCue = p[0], the value 23f0 waits on. */

int daDemo_c::func_ov002_020f26c4(daDemo_c *self, unsigned char *src) {
    self->mCue = src[0];
    return 1;
}

extern "C" {
void func_ov002_020f26d4(void) {
    ((void (*)(void))func_ov002_020f2630)();
}
}

void StaffRoll::func_ov002_020f26e0() {
    /* The kira callback's tracking records: dPa_c.h keeps them as opaque
       words, so OamEnt views each one as the sparkle sprite it draws. */
    struct OamEnt *e = (struct OamEnt *)this->kira.trackingRecords;
    int i;
    for (i = 0; i < 0x40; i++, e++) {
        int idx;
        int s;
        if (e->enabled == 0)
            continue;
        idx = 0;
        if (e->ang & 0x80)
            idx = 1;
        s = e->ang / 512;
        if (s >= 7)
            s = 7;
        if (s == 0)
            continue;
        OAM::Render(1, data_ov002_0210b6a8[idx], e->x, e->y, -1, 1, ((7 - s) << 10) + 0x1000, 0);
    }
}

void StaffRoll::func_ov002_020f2790(int p1, int p2, int p3, s16 p4) {
    this->kira.unk308 = p3;
    this->kira.unk30c = p4;
    this->kiraParticle = Particle::System::New(
        this->kiraParticle, kPtclEndingStarSparkle, p1 << 12, p2 << 12, 0, 0,
        (dPa_c::level_c::callback_c *)((struct ParticleCallback *)&this->kira));
}

int StaffRoll::func_ov002_020f27e8() {
    if (this->burstActive == 0)
        return this->burstActive;

    this->burstTimer += 1;

    if (this->burstDefParam != 0x1000) {
        if (this->burstTimer == data_ov002_02100140[this->burstPeriods & 3]) {
            this->burstDefParam += 0x199;
            if (this->burstDefParam >= 0x1000)
                this->burstDefParam = 0x1000;
        }
    }

    if (this->burstSysParam != 0x800) {
        if (this->burstPeriods >= 5) {
            if ((this->burstTimer & 3) == 3) {
                this->burstSysParam += 0x199;
                if (this->burstSysParam >= 0x800)
                    this->burstSysParam = 0x800;
            }
        }
    }

    func_ov002_020f2790(0x80, 0x60, this->burstDefParam, this->burstSysParam);
    func_ov002_020f2790(0x80, 0x60, this->burstDefParam, this->burstSysParam);

    if (this->burstTimer >= 0x1e) {
        this->burstTimer = 0;
        this->burstPeriods += 1;
    }
    if (this->burstPeriods < 0xa)
        return this->burstPeriods;
    return func_ov002_020f2984();
}

void StaffRoll::func_ov002_020f2958() {
    this->burstActive = 1;
    this->burstDefParam = 0x199;
    this->burstSysParam = 0x199;
    this->burstTimer = 0;
    this->burstPeriods = 0;
}

int StaffRoll::func_ov002_020f2984() {
    this->burstActive = 0;
    return (int)this;
}

int StaffRoll::func_ov002_020f2990() {
    int x, y;
    int idx;

    if (this->endVisible == 0)
        return (int)this;

    x = this->endX >> 0xc;
    y = this->endY >> 0xc;
    idx = 0;
    if (GetOwnerLanguage() == 3)
        idx = 1;
    else if (GetOwnerLanguage() == 2 || GetOwnerLanguage() == 5)
        idx = 2;
    else if (GetOwnerLanguage() == 4)
        idx = 3;

    OAM::Render(0, data_ov002_0210b988[idx], x, y, -1, 1, 0x1000, 0x1000, 0, -1);
}

void StaffRoll::func_ov002_020f2a48() {
    /* 0x1d8/0x1d9 and the position: see func_ov002_020f2990. */
    this->endActive = 1;
    this->endVisible = 1;
    this->endX = 524288;
    this->endY = 393216;
}

void StaffRoll::func_ov002_020f2a68() {
    /* 0x1d8/0x1d9: see func_ov002_020f2990. */
    this->endActive = 0;
    this->endVisible = 0;
}

void StaffRoll::func_ov002_020f2a78(StaffRoll *self, int i) {
    self->credit[i].active = 0;
}

void StaffRoll::func_ov002_020f2a88(StaffRoll *self, int i) {
    self->credit[i].timer++;
    if (self->credit[i].timer < 0x39)
        return;
    self->credit[i].timer = 0;
    self->credit[i].step++;
}

void StaffRoll::func_ov002_020f2aec(StaffRoll *self, int idx) {
    StaffRoll *s = self;

    if (s->credit[idx].timer == 0) {
        gSubScreenLayers |= 1;
    }
    s->credit[idx].timer++;
    s->credit[idx].blendTimer++;

    if (s->credit[idx].blendTimer >= 2) {
        s->credit[idx].blendTimer = 0;
        s->credit[idx].blend++;
        unsigned char cnt = s->credit[idx].blend;
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x36, cnt, 0x10 - cnt);
    }

    if (s->credit[idx].timer < 30) {
        return;
    }
    s->credit[idx].timer = 0;
    s->credit[idx].step++;
    *(volatile unsigned short *)0x4001050 = 0;
}

void StaffRoll::func_ov002_020f2bf4(StaffRoll *self, int idx) {
    StaffRoll *s = self;

    s->credit[idx].timer++;
    s->credit[idx].blendTimer++;

    if (s->credit[idx].blendTimer >= 2) {
        s->credit[idx].blendTimer = 0;
        s->credit[idx].blend++;
        unsigned char cnt = s->credit[idx].blend;
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x36, 0x10 - cnt, cnt);
    }

    if (s->credit[idx].timer < 30) {
        return;
    }
    s->credit[idx].timer = 0;
    s->credit[idx].step++;
    s->credit[idx].message++;
    s->credit[idx].blend = 0;
    s->credit[idx].blendTimer = 0;
    gSubScreenLayers &= ~1;
    func_0201ef50(s->credit[idx].message);
}

void StaffRoll::func_ov002_020f2d0c(StaffRoll *self, int i) {
    self->credit[i].timer++;
    if (self->credit[i].timer < 0x39)
        return;
    self->credit[i].timer = 0;
    self->credit[i].step++;
}

void StaffRoll::func_ov002_020f2d70(StaffRoll *self, int i) {
    self->credit[i].timer++;
    if (self->credit[i].timer < 0x1e)
        return;
    self->credit[i].timer = 0;
    self->credit[i].step++;
}

extern Entry2dd4 data_ov002_02110ebc[];
void StaffRoll::func_ov002_020f2dd4() {
    if (this->credit[0].active == 0)
        return;
    (this->*data_ov002_02110ebc[this->credit[0].step].pmf)(0);
}

void StaffRoll::StartCredit(int n) {
    if (n == 0)
        return;
    if (n == 0x15)
        return;
    credit[0].active = 1;
    credit[0].step = 0;
    credit[0].timer = 0;
    credit[0].blendTimer = 0;
    credit[0].blend = 0;
    n = (n - 1) << 1;
    credit[0].message = (unsigned char)n;
    func_0201f108();
    func_0201ef50(n & 0xff);
    gSubScreenLayers |= 1;
}

void StaffRoll::func_ov002_020f2ea0() {
    this->credit[0].active = 0;
}

void StaffRoll::func_ov002_020f2eac() {
    int i;
    StaffRoll *p = this;
    for (i = 0; i < 5; i++) {
        if (p->dolls[0].visible != 0) {
            OAM::RenderSub(data_ov002_0210bba0[p->dolls[0].frame], p->dolls[0].x >> 12,
                           p->dolls[0].y >> 12, -1, p->dolls[0].priority);
        }
        p = (StaffRoll *)((Doll *)p + 1);
    }
}

void StaffRoll::func_ov002_020f2f18(int a, int b) {
    StaffRoll *c = this;
    int i = 0;
    do {
        if (c->dolls[0].active != 0) {
            c->dolls[0].x -= a;
            c->dolls[0].y -= b;
        }
        i++;
        c = (StaffRoll *)((Doll *)c + 1);
    } while (i < 5);
}

void StaffRoll::func_ov002_020f2f64() {
    StaffRoll *self = this;
    int i;
    for (i = 0; i < 5; i++) {
        if (self->dolls[0].active != 0) {
            self->dolls[0].frame++;
        }
        self = (StaffRoll *)((Doll *)self + 1);
    }
}

void StaffRoll::func_ov002_020f2f9c(int count) {
    StaffRoll *sl = this;
    int i;
    int idx = func_ov002_020f5a94() - 2;
    if (idx < 0)
        idx = 0;
    for (i = 0; i < count + 1; i++) {
        int ok = 0;
        if (i == 0 || i == 1 || i == 4) {
            ok++;
        } else if (i == 2) {
            if (SaveData::IsCharacterUnlocked(2))
                ok++;
        } else {
            if (SaveData::IsCharacterUnlocked(1))
                ok++;
        }
        if (ok) {
            int *p;
            sl->dolls[0].active = 1;
            sl->dolls[0].visible = 1;
            p = data_ov002_0210b97c[idx];
            sl->dolls[0].x = (p[i * 4 + 0] + 0xe0) << 12;
            sl->dolls[0].y = (p[i * 4 + 1] + 0x66) << 12;
            sl->dolls[0].priority = (u8)p[i * 4 + 2];
            sl->dolls[0].frame = (u8)p[i * 4 + 3];
        }
        sl = (StaffRoll *)((Doll *)sl + 1);
    }
}

void StaffRoll::func_ov002_020f30d4() {
    StaffRoll *p = this;
    int i;
    for (i = 0; i < 5; i++) {
        p->dolls[0].active = 0;
        p->dolls[0].visible = 0;
        p = (StaffRoll *)((Doll *)p + 1);
    }
}

void StaffRoll::func_ov002_020f30f8(StaffRoll *self, int i) {
    unsigned short t;
    int oldA, oldB;
    int cc, dd, ee, ff;

    t = self->window[i].delay;
    if (t != 0) {
        self->window[i].delay = t - 1;
        return;
    }

    self->window[i].left -=
        self->window[i].stepX;
    self->window[i].top -=
        self->window[i].stepY;
    self->window[i].right +=
        self->window[i].stepX;
    self->window[i].bottom +=
        self->window[i].stepY;

    oldA = self->window[i].bgX;
    oldB = self->window[i].bgY;
    self->window[i].bgX =
        oldA + self->window[i].stepX;
    self->window[i].bgY +=
        self->window[i].stepY;

    if (self->window[i].bgX >= 0)
        self->window[i].bgX = 0;
    if (self->window[i].bgY >= 0)
        self->window[i].bgY = 0;
    if (self->window[i].left < 0)
        self->window[i].left = 0;
    if (self->window[i].top < 0)
        self->window[i].top = 0;
    if (self->window[i].right >= 0xff000)
        self->window[i].right = 0xff000;
    if (self->window[i].bottom >= 0xc0000)
        self->window[i].bottom = 0xc0000;
    if (self->window[i].bgX > 0)
        self->window[i].bgX = 0;
    if (self->window[i].bgY > 0)
        self->window[i].bgY = 0;

    cc = self->window[i].bgX - oldA;
    self->func_ov002_020f2f18(cc, self->window[i].bgY - oldB);

    ee = self->window[i].right >> 12;
    ff = self->window[i].bottom >> 12;
    cc = self->window[i].left >> 12;
    dd = self->window[i].top >> 12;
    *(unsigned short *)0x4001042 = ((cc << 8) & 0xff00) | (ee & 0xff);
    *(unsigned short *)0x4001046 = ((dd << 8) & 0xff00) | (ff & 0xff);
    SetSubBg1Offset(self->window[i].bgX >> 12,
                    self->window[i].bgY >> 12);
    SetSubBg2Offset(self->window[i].bgX >> 12,
                    self->window[i].bgY >> 12);
}

void StaffRoll::func_ov002_020f32e4(StaffRoll *self, int i) {
    self->window[i].delay = 0x10;
    self->window[i].stepX = 0x1800;
    self->window[i].stepY = 0x1800;
    self->window[i].step = 1;
}

void StaffRoll::func_ov002_020f3310(StaffRoll *self, int idx) {
    unsigned char sel = self->window[idx].step;
    (self->*data_ov002_02110e24[sel])(idx);
}

void StaffRoll::func_ov002_020f335c(StaffRoll *self, int i) {
    StaffRoll *s = self;
    s->window[i].top += 0x1800;
    if (s->window[i].top >= 0x66000) {
        s->window[i].top = 0x66000;
        s->window[i].active = 0;
        s->window[i].step = 0;
    }
    int left = s->window[i].left >> 0xc;
    int right = s->window[i].right >> 0xc;
    int bottom = s->window[i].bottom >> 0xc;
    int top = s->window[i].top >> 0xc;
    *(volatile unsigned short *)0x4001042 =
        (unsigned short)(((left << 8) & 0xff00) | (right & 0xff));
    *(volatile unsigned short *)0x4001046 =
        (unsigned short)(((top << 8) & 0xff00) | (bottom & 0xff));
}

void StaffRoll::func_ov002_020f340c(StaffRoll *self, int i) {
    StaffRoll *s = self;
    s->window[i].top += 0x1800;
    if (s->window[i].top >= 0x66000) {
        s->window[i].top = 0x66000;
        s->window[i].active = 0;
        s->window[i].step = 0;
    }
    int left = s->window[i].left >> 0xc;
    int right = s->window[i].right >> 0xc;
    int bottom = s->window[i].bottom >> 0xc;
    int top = s->window[i].top >> 0xc;
    *(volatile unsigned short *)0x4001042 =
        (unsigned short)(((left << 8) & 0xff00) | (right & 0xff));
    *(volatile unsigned short *)0x4001046 =
        (unsigned short)(((top << 8) & 0xff00) | (bottom & 0xff));
}

void StaffRoll::func_ov002_020f34bc(StaffRoll *self, int idx) {
    int flag;
    int a, b, c, d;
    int bs, cs, ds, as_;
    self->window[idx].left -= 0x1800;
    self->window[idx].top -= 0x1800;
    self->window[idx].right += 0x1800;
    self->window[idx].bottom += 0x1800;
    flag = 0;
    if (self->window[idx].left <= 0x80000)
        self->window[idx].left = 0x80000;
    if (self->window[idx].top <= 0x56000)
        self->window[idx].top = 0x56000;
    if (self->window[idx].right >= 0xfc000)
        self->window[idx].right = 0xfc000;
    if (self->window[idx].bottom >= 0xbc000) {
        self->window[idx].bottom = 0xbc000;
        flag++;
    }
    if (flag != 0)
        self->window[idx].step++;
    a = self->window[idx].top;
    b = self->window[idx].left;
    c = self->window[idx].right;
    d = self->window[idx].bottom;
    bs = b >> 12;
    cs = c >> 12;
    ds = d >> 12;
    as_ = a >> 12;
    *(unsigned short *)0x4001042 = ((bs << 8) & 0xff00) | (cs & 0xff);
    *(unsigned short *)0x4001046 = ((as_ << 8) & 0xff00) | (ds & 0xff);
}

void StaffRoll::func_ov002_020f35e4(StaffRoll *self, int i) {
    self->window[i].left = 0xe0000;
    self->window[i].top = 0x66000;
    self->window[i].right = 0xe0000;
    self->window[i].bottom = 0x66000;
    *((volatile unsigned int *)0x4001000) =
        ((*((volatile unsigned int *)0x4001000)) & (~0xe000)) | 0x4000;
    *((volatile unsigned short *)0x4001048) =
        (unsigned short)(((((*((volatile unsigned short *)0x4001048)) & (~0x3f00)) | 0x1700) &
                          0xFFFFFFFFFFFFFFFFull) |
                         0x2000);
    *((volatile unsigned short *)0x400104a) =
        (unsigned short)(((((*((volatile unsigned short *)0x400104a)) & (~0x3f)) | 0x11) &
                          0xFFFFFFFFFFFFFFFFull) |
                         0x20);
    {
        int a = self->window[i].left >> 12;
        int d = self->window[i].right >> 12;
        int e = self->window[i].bottom >> 12;
        int b = self->window[i].top >> 12;
        *((volatile unsigned short *)0x4001042) =
            (unsigned short)(((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *)0x4001046) =
            (unsigned short)(((b << 8) & 0xff00) | (e & 0xff));
    }
    self->window[i].bgX = -0x60000;
    self->window[i].bgY = -0x38000;
    SetSubBg1Offset(self->window[i].bgX >> 12,
                    self->window[i].bgY >> 12);
    SetSubBg2Offset(self->window[i].bgX >> 12,
                    self->window[i].bgY >> 12);
    gSubScreenLayers |= 6;
    self->window[i].step =
        self->window[i].step + 1;
}

void StaffRoll::func_ov002_020f3740(StaffRoll *self, int idx) {
    unsigned char sel = self->window[idx].step;
    (self->*data_ov002_02110e9c[sel])(idx);
}

void StaffRoll::func_ov002_020f378c(StaffRoll *self, int idx) {
    self->window[idx].step = 0;
}

extern Entry37 data_ov002_02110e4c[];
void StaffRoll::func_ov002_020f37a0() {
    if (this->window[0].active == 0)
        return;
    (this->*data_ov002_02110e4c[this->window[0].kind].pmf)(0);
}

void StaffRoll::func_ov002_020f37fc() {
    func_ov002_020f3828();
    this->window[0].active = 1;
    this->window[0].kind = 2;
    this->window[0].step = 0;
}

void StaffRoll::func_ov002_020f3828() {
    this->window[0].left = 0x80000;
    this->window[0].top = 0x66000;
    this->window[0].right = 0xfc000;
    this->window[0].bottom = 0xbc000;
    *((volatile unsigned int *)0x4001000) =
        ((*((volatile unsigned int *)0x4001000)) & (~0xe000)) | 0x4000;
    *((volatile unsigned short *)0x4001048) =
        (unsigned short)(((((*((volatile unsigned short *)0x4001048)) & (~0x3f00)) | 0x1700) &
                          0xFFFFFFFFFFFFFFFFull) |
                         0x2000);
    *((volatile unsigned short *)0x400104a) =
        (unsigned short)(((((*((volatile unsigned short *)0x400104a)) & (~0x3f)) | 1) &
                          0xFFFFFFFFFFFFFFFFull) |
                         0x20);
    {
        int a = this->window[0].left >> 12;
        int d = this->window[0].right >> 12;
        int e = this->window[0].bottom >> 12;
        int b = this->window[0].top >> 12;
        *((volatile unsigned short *)0x4001042) =
            (unsigned short)(((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *)0x4001046) =
            (unsigned short)(((b << 8) & 0xff00) | (e & 0xff));
    }
    this->window[0].bgX = -0x60000;
    this->window[0].bgY = -0x38000;
    SetSubBg1Offset(this->window[0].bgX >> 12, this->window[0].bgY >> 12);
    SetSubBg2Offset(this->window[0].bgX >> 12, this->window[0].bgY >> 12);
    gSubScreenLayers |= 6;
}

void StaffRoll::func_ov002_020f3954() {
    this->window[0].active = 1;
    this->window[0].kind = 1;
    this->window[0].step = 0;
}

void StaffRoll::func_ov002_020f396c() {
    this->window[0].active = 0;
}

extern void *data_ov002_0210b998[];
void StaffRoll::func_ov002_020f3978() {
    Star *c = this->stars;
    int i;
    for (i = 0; i < 4; i++) {
        if (c->visible) {
            OAM::Render(true, (OamAttr *)data_ov002_0210b998[i], c->x >> 0xc, c->y >> 0xc, -1, 1,
                        0);
        }
        c++;
    }
}

void StaffRoll::func_ov002_020f39ec(int idx) {
    StaffRoll *s = this;
    s->stars[idx].frameTimer++;
    if (s->stars[idx].frameTimer < 4)
        return;
    s->stars[idx].frameTimer = 0;
    s->stars[idx].frame++;
    if (s->stars[idx].holdFrame != 0) {
        if (s->stars[idx].frame >= 0x10)
            s->stars[idx].frame = 0x10;
    } else if (s->stars[idx].frame >= 0x20) {
        s->stars[idx].frame = 0;
    }

    for (int i = 0; i < 4; i++) {
        int v = data_ov002_021002a0[s->stars[idx].frame];
        int src = i * 0x800 + ((v & 7) << 8);
        if (v & 8)
            src += 0x2000;
        MultiCopyHalf(s->starChars + src,
                      (u8 *)G2S_GetOBJCharPtr() + 0x2000 + idx * 0x100 + i * 0x400, 0x100);
    }
}

void StaffRoll::func_ov002_020f3ae8(StaffRoll *self, int idx) {
    Star *arr = self->stars;
    int a;
    int b;
    a = data_02082214[((arr[idx].heading >> 4) * 2) + 1];
    b = arr[idx].radius;
    arr[idx].x = FX_MUL(a, b) + 0xe0000;
    a = data_02082214[(arr[idx].heading >> 4) * 2];
    b = arr[idx].radius;
    arr[idx].y = FX_MUL(a, b) + 0x66000;
    if (arr[idx].radius > 0x18000) {
        arr[idx].radius = arr[idx].radius - 0x10;
    }
    arr[idx].heading += 0x200;
}

void StaffRoll::func_ov002_020f3ba0(StaffRoll *self, int i) {
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = self->stars[i].speed;
        self->stars[i].x += FX_MUL(tv, spd);
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = self->stars[i].speed;
        self->stars[i].y += FX_MUL(tv, spd);
    }

    self->stars[i].speed += 0x30;

    dx = 0xe0 - (self->stars[i].x >> 12);
    dy = 0x46 - (self->stars[i].y >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        self->stars[i].step += 1;
        self->stars[i].delay = 0;
        self->stars[i].heading = 0xc000;
        self->stars[i].radius = 0x20000;
        self->stars[i].x = 0xe0000;
        self->stars[i].y = 0x46000;
        return;
    }

    target = cstd::atan2(dy, dx);
    ang = self->stars[i].heading;
    if (target > ang) {
        self->stars[i].heading += 0x80;
        if (target <= self->stars[i].heading)
            self->stars[i].heading = target;
    } else {
        if (ang <= target)
            return;
        self->stars[i].heading -= 0x80;
        if (self->stars[i].heading <= target)
            self->stars[i].heading = target;
    }
}

void StaffRoll::func_ov002_020f3d38(StaffRoll *self, int i) {
    if (self->stars[i].delay != 0) {
        self->stars[i].delay =
            self->stars[i].delay - 1;
        return;
    }
    self->stars[i].x = -0x10000;
    self->stars[i].y = -0x10000;
    self->stars[i].speed = 0x2000;
    self->stars[i].heading = 0;
    self->stars[i].delay = 0;
    self->stars[i].step = self->stars[i].step + 1;
}

void StaffRoll::func_ov002_020f3d98(StaffRoll *self, int idx) {
    unsigned char sel = self->stars[idx].step;
    (self->*data_ov002_02110e34[sel])(idx);
}

void StaffRoll::func_ov002_020f3de4(StaffRoll *self, int i) {
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;
    int j;

    v = self->stars[i].delay;
    if (v != 0) {
        self->stars[i].delay = v - 1;
        return;
    }

    if (self->stars[i].phase == 0) {
        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].speed;
            self->stars[i].x += FX_MUL(tv, spd);
        }

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].speed;
            self->stars[i].y += FX_MUL(tv, spd);
        }

        if (self->stars[i].speed >= 0x2000)
            self->stars[i].speed -= 0x120;

        dx = (self->stars[i].targetX - self->stars[i].x) >> 12;
        dy = (self->stars[i].targetY - self->stars[i].y) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            self->stars[i].phase += 1;
            self->stars[i].orbitTravel = 0;
            self->stars[i].heading = 0xc000;
            return;
        }

        target = cstd::atan2(dy, dx);
        ang = self->stars[i].heading;
        if (target > ang) {
            self->stars[i].heading += 0x100;
            if (target <= self->stars[i].heading)
                self->stars[i].heading = target;
        } else {
            if (ang <= target)
                return;
            self->stars[i].heading -= 0x100;
            if (self->stars[i].heading <= target)
                self->stars[i].heading = target;
        }
        return;
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = self->stars[i].radius;
        self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }
    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = self->stars[i].radius;
        self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }
    self->stars[i].heading +=
        self->stars[i].angSpeed;
    self->stars[i].orbitTravel += self->stars[i].angSpeed;

    if (i != self->func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)self->stars[0].orbitTravel < 0x10000)
        return;

    Star *p = self->stars;
    for (j = 0; j < 4; j++) {
        if (p->active != 0) {
            p->step = 1;
            p->loops = 0;
            p->phase = 0;
            p->radius = 0x38000;
            p->heading = data_ov002_021000c0[j];
            p->orbitTravel = 0;
            p->angSpeed = 0x200;
        }
        p++;
    }
}

void StaffRoll::func_ov002_020f40fc(StaffRoll *self, int i) {

    if (self->stars[i].frame == 0) {
        if (self->stars[i].frameTimer == 0) {
            self->stars[i].loops += 1;
        }
    }

    if (self->stars[i].phase == 0) {
        int idx;
        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].radius;
            self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }
        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].radius;
            self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        self->stars[i].heading -=
            self->stars[i].angSpeed;
        self->stars[i].orbitTravel +=
            self->stars[i].angSpeed;

        if (self->stars[0].phase != 0) {
            self->stars[i].angSpeed += 8;
        } else {
            if (self->stars[i].angSpeed < 0x200) {
                self->stars[i].angSpeed += 6;
                if (self->stars[i].angSpeed > 0x200)
                    self->stars[i].angSpeed = 0x200;
            }
        }

        if ((unsigned int)self->stars[i].orbitTravel >=
                (unsigned int)data_ov002_02100120[i] &&
            self->holdOrbit == 0) {
            self->stars[i].phase += 1;
            self->stars[i].flyTimer = 0;
            self->stars[i].speed = 0x2000;
        }

        if (self->func_ov002_020f5a94() != 3)
            return;
        if ((unsigned int)self->stars[i].orbitTravel < 0xc000)
            return;
        if (i != 2)
            return;
        if (self->holdOrbit != 0)
            return;
        self->stars[i].phase += 1;
        self->stars[i].flyTimer = 0;
        self->stars[i].speed = 0x2000;
        return;
    }

    self->stars[i].flyTimer += 1;
    self->stars[i].x += self->stars[i].speed;
    self->stars[i].speed += 0x300;
    if (self->stars[0].flyTimer < 0x70)
        return;
    {
        self->stars[i].step += 1;
        self->stars[i].loops = 0;
        self->stars[i].phase = 0;
        self->stars[i].radius = 0x38000;
        self->stars[i].heading = 0;
        self->stars[i].delay = data_ov002_02100130[i] << 6;
        self->stars[i].x = -0x10000;
        self->stars[i].y = 0;
        self->stars[i].angSpeed = 0x100;
        self->stars[i].speed = 0x5800;
        self->stars[i].targetX = 0x80000;
        self->stars[i].targetY = 0x28000;
    }
}

void StaffRoll::func_ov002_020f43cc(StaffRoll *self, int i) {
    int idx = i;
    char *px;
    char *s;
    int off = idx * 0x4c;
    {
        unsigned short v = self->stars[idx].delay;
        if (v != 0) {
            self->stars[idx].delay = v - 1;
            return;
        }
    }
    if (self->stars[idx].phase == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = self->stars[idx].heading >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = self->stars[idx].speed;
            self->stars[idx].x += FX_MUL(tv, spd);
        }
        idxa = self->stars[idx].heading >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = self->stars[idx].speed;
            self->stars[idx].y += FX_MUL(tv, spd);
        }
        px = (char *)self + off;
        if (*(int *)((char *)&self->stars[0].speed + off) <= -0x2000)
            *(int *)((char *)&self->stars[0].speed + off) =
                *(int *)((char *)&self->stars[0].speed + off) + 0x120;
        {
            s = (char *)self + idx * 0x4c;
            int dx = (((Star *)s)->targetX - ((Star *)px)->x) >> 12;
            int dy = (((Star *)s)->targetY - self->stars[idx].y) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                self->stars[idx].phase++;
                ((Star *)s)->orbitTravel = 0;
                self->stars[idx].heading = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)cstd::atan2(dy, dx);
                unsigned short cur = self->stars[idx].heading;
                if (a > cur) {
                    unsigned short nv;
                    self->stars[idx].heading += 0x100;
                    nv = self->stars[idx].heading;
                    if (a <= nv)
                        self->stars[idx].heading = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    self->stars[idx].heading -= 0x100;
                    nv2 = self->stars[idx].heading;
                    if (nv2 <= a)
                        self->stars[idx].heading = a;
                    return;
                }
                return;
            }
        }
    }
    {
        int idxa;
        short tv;
        int spd;
        idxa = self->stars[idx].heading >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = self->stars[idx].radius;
            self->stars[idx].x = FX_MUL(tv, spd) + 0x80000;
        }
        idxa = self->stars[idx].heading >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = self->stars[idx].radius;
            self->stars[idx].y = FX_MUL(tv, spd) + 0x60000;
        }
        self->stars[idx].heading -=
            self->stars[idx].angSpeed;
        self->stars[idx].orbitTravel +=
            self->stars[idx].angSpeed;
    }
    if (idx != self->func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)self->stars[0].orbitTravel < 0x10000U)
        return;
    {
        int j;
        Star *p = self->stars;
        for (j = 0; j < 4; j++) {
            if (p->active != 0) {
                (p->step)++;
                p->loops = 0;
                p->phase = 0;
                p->radius = 0x38000;
                p->heading = data_ov002_021000b8[j];
                if (self->func_ov002_020f5a94() == 3) {
                    if (j == 2)
                        p->heading = 0;
                }
                p->orbitTravel = 0;
            }
            p++;
        }
    }
}

void StaffRoll::func_ov002_020f4710(StaffRoll *self, int i) {

    if (self->stars[i].phase == 0) {
        int idx;

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].radius;
            self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].radius;
            self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        self->stars[i].heading -=
            self->stars[i].angSpeed;
        self->stars[i].orbitTravel +=
            self->stars[i].angSpeed;

        if (self->stars[0].phase != 0) {
            self->stars[i].angSpeed += 8;
        } else if (self->stars[i].angSpeed < 0x200) {
            self->stars[i].angSpeed += 6;
            if (self->stars[i].angSpeed > 0x200)
                self->stars[i].angSpeed = 0x200;
        }

        if ((u32) self->stars[i].orbitTravel <
            (u32)data_ov002_02100100[i])
            return;
        if (self->holdOrbit != 0)
            return;

        self->stars[i].phase++;
        self->stars[i].flyTimer = 0;

        if (i == 0 || i == 3) {
            self->stars[i].speed = 0x2000;
        } else {
            self->stars[i].speed = -0x2000;
        }

        if (self->func_ov002_020f5a94() != 3)
            return;
        if (i != 2)
            return;
        if (self->holdOrbit == 0)
            self->stars[i].speed = 0x2000;
        return;
    }

    self->stars[i].flyTimer++;
    self->stars[i].x += self->stars[i].speed;

    if (self->func_ov002_020f5a94() == 3 && i == 2) {
        self->stars[i].speed += 0x300;
    } else if (i == 0 || i == 3) {
        self->stars[i].speed += 0x300;
    } else {
        self->stars[i].speed -= 0x300;
    }

    if (self->stars[0].flyTimer < 0x60)
        return;

    {
        self->stars[i].step++;
        self->stars[i].loops = 0;
        self->stars[i].phase = 0;
        self->stars[i].radius = 0x38000;
        self->stars[i].heading = 0x8000;
        self->stars[i].delay = (u16)(data_ov002_02100110[i] << 6);

        if (self->func_ov002_020f5a94() == 3) {
            if (i == 2)
                self->stars[i].delay = 0x40;
        }
    }

    self->stars[i].x = 0x110000;
    self->stars[i].y = 0;
    self->stars[i].angSpeed = 0x100;
    self->stars[i].speed = 0x5800;
    self->stars[i].targetX = 0x80000;
    self->stars[i].targetY = 0x28000;
}

void StaffRoll::func_ov002_020f4a2c(StaffRoll *self, int i) {
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = self->stars[i].delay;
    if (v != 0) {
        self->stars[i].delay = v - 1;
        return;
    }

    if (self->stars[i].phase == 0) {
        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].speed;
            self->stars[i].x += FX_MUL(tv, spd);
        }

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].speed;
            self->stars[i].y += FX_MUL(tv, spd);
        }

        if (self->stars[i].speed <= -0x2000)
            self->stars[i].speed =
                self->stars[i].speed + 0x120;

        dx = (self->stars[i].targetX -
              self->stars[i].x) >>
             12;
        dy = (self->stars[i].targetY -
              self->stars[i].y) >>
             12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            self->stars[i].phase += 1;
            self->stars[i].orbitTravel = 0;
            self->stars[i].heading = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = self->stars[i].heading;
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            self->stars[i].heading += 0x100;
            nv = self->stars[i].heading;
            if (target <= nv)
                self->stars[i].heading = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            self->stars[i].heading -= 0x100;
            nv2 = self->stars[i].heading;
            if (nv2 <= target)
                self->stars[i].heading = target;
            return;
        }
        return;
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = self->stars[i].radius;
        self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = self->stars[i].radius;
        self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }

    self->stars[i].heading -=
        self->stars[i].angSpeed;
    self->stars[i].orbitTravel +=
        self->stars[i].angSpeed;

    if (i != self->func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)self->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        Star *p = self->stars;
        for (j = 0; j < 4; j++) {
            if (p->active != 0) {
                p->step += 1;
                p->loops = 0;
                p->phase = 0;
                p->radius = 0x38000;
                p->heading = data_ov002_021000c8[j];
                if (self->func_ov002_020f5a94() == 3) {
                    if (j == 2)
                        p->heading = 0;
                }
                p->orbitTravel = 0;
            }
            p++;
        }
    }
}

void StaffRoll::func_ov002_020f4d70(StaffRoll *self, int i) {

    if (self->stars[i].frame == 0) {
        if (self->stars[i].frameTimer == 0)
            self->stars[i].loops++;
    }

    if (self->stars[i].phase == 0) {
        int idx;

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].radius;
            self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].radius;
            self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        self->stars[i].heading +=
            self->stars[i].angSpeed;
        self->stars[i].orbitTravel +=
            self->stars[i].angSpeed;

        if (self->stars[0].phase != 0) {
            self->stars[i].angSpeed += 8;
        } else {
            if (self->stars[i].angSpeed < 0x200) {
                self->stars[i].angSpeed += 6;
                if (self->stars[i].angSpeed > 0x200)
                    self->stars[i].angSpeed = 0x200;
            }
        }

        if ((u32) self->stars[i].orbitTravel <
            (u32)data_ov002_021000e0[i])
            return;

        if (self->holdOrbit != 0)
            return;

        self->stars[i].phase++;
        self->stars[i].flyTimer = 0;
        self->stars[i].speed = -0x2000;
        return;
    }

    self->stars[i].flyTimer++;
    self->stars[i].x += self->stars[i].speed;
    self->stars[i].speed -= 0x300;

    if (self->stars[0].flyTimer < 0x70)
        return;

    {
        self->stars[i].step++;
        self->stars[i].radius = 0x38000;
        self->stars[i].heading = 0x8000;
        self->stars[i].phase = 0;
        self->stars[i].loops = 0;
        self->stars[i].delay = data_ov002_021000f0[i] << 6;

        if (self->func_ov002_020f5a94() == 3) {
            if (i == 2)
                self->stars[i].delay = 0x40;
        }
    }

    self->stars[i].x = 0x110000;
    self->stars[i].y = 0;
    self->stars[i].angSpeed = 0x100;
    self->stars[i].speed = 0x5800;
    self->stars[i].targetX = 0x80000;
    self->stars[i].targetY = 0x28000;
}

void StaffRoll::func_ov002_020f5010(StaffRoll *self, int i) {
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = self->stars[i].delay;
    if (v != 0) {
        self->stars[i].delay = v - 1;
        return;
    }

    if (self->stars[i].phase == 0) {
        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].speed;
            self->stars[i].x += FX_MUL(tv, spd);
        }

        idx = self->stars[i].heading >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].speed;
            self->stars[i].y += FX_MUL(tv, spd);
        }

        if (self->stars[i].speed >= 0x2000)
            self->stars[i].speed =
                self->stars[i].speed - 0x120;

        dx = (self->stars[i].targetX -
              self->stars[i].x) >>
             12;
        dy = (self->stars[i].targetY -
              self->stars[i].y) >>
             12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            self->stars[i].phase += 1;
            self->stars[i].orbitTravel = 0;
            self->stars[i].heading = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = self->stars[i].heading;
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            self->stars[i].heading += 0x100;
            nv = self->stars[i].heading;
            if (target <= nv)
                self->stars[i].heading = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            self->stars[i].heading -= 0x100;
            nv2 = self->stars[i].heading;
            if (nv2 <= target)
                self->stars[i].heading = target;
            return;
        }
        return;
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = self->stars[i].radius;
        self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }

    idx = self->stars[i].heading >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = self->stars[i].radius;
        self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }

    self->stars[i].heading +=
        self->stars[i].angSpeed;
    self->stars[i].orbitTravel +=
        self->stars[i].angSpeed;

    if (i != self->func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)self->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        Star *p = self->stars;
        for (j = 0; j < 4; j++) {
            if (p->active != 0) {
                p->step += 1;
                p->loops = 0;
                p->phase = 0;
                p->radius = 0x38000;
                p->heading = data_ov002_021000d0[j];
                p->orbitTravel = 0;
            }
            p++;
        }
    }
}

void StaffRoll::func_ov002_020f5328(StaffRoll *self, int i) {

    if (self->stars[i].phase == 0) {
        int idx;
        {
            idx = self->stars[i].heading >> 4;
            short tv = data_02082214[idx * 2 + 1];
            int spd = self->stars[i].radius;
            self->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }
        {
            idx = self->stars[i].heading >> 4;
            short tv = data_02082214[idx * 2];
            int spd = self->stars[i].radius;
            self->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        self->stars[i].heading +=
            self->stars[i].angSpeed;
        self->stars[i].orbitTravel +=
            self->stars[i].angSpeed;

        if (self->stars[0].phase != 0) {
            self->stars[i].angSpeed += 8;
        } else if (self->stars[i].angSpeed < 0x200) {
            self->stars[i].angSpeed += 6;
            if (self->stars[i].angSpeed > 0x200)
                self->stars[i].angSpeed = 0x200;
        }

        if ((unsigned int)self->stars[i].orbitTravel <
            (unsigned int)data_ov002_02100150[i])
            return;
        if (self->holdOrbit != 0)
            return;

        self->stars[i].phase += 1;
        self->stars[i].flyTimer = 0;

        if (i == 1 || i == 3) {
            self->stars[i].speed = 0x2000;
        } else {
            self->stars[i].speed = -0x2000;
        }
        return;
    }

    self->stars[i].flyTimer += 1;
    self->stars[i].x += self->stars[i].speed;
    if (i == 1 || i == 3) {
        self->stars[i].speed += 0x300;
    } else {
        self->stars[i].speed -= 0x300;
    }

    if (self->stars[0].flyTimer < 0x60)
        return;

    {
        self->stars[i].step += 1;
        self->stars[i].loops = 0;
        self->stars[i].phase = 0;
        self->stars[i].radius = 0x38000;
        self->stars[i].heading = 0;
        self->stars[i].delay =
            (unsigned short)(data_ov002_02100160[i] << 6);
        self->stars[i].x = -0x10000;
        self->stars[i].y = 0;
        self->stars[i].angSpeed = 0x100;
        self->stars[i].speed = 0x5800;
        self->stars[i].targetX = 0x80000;
        self->stars[i].targetY = 0x28000;
    }
}

void StaffRoll::func_ov002_020f55b4(StaffRoll *self, int i) {
    Star *e = (Star *)((char *)self + i * 0x4c);
    if (e->frame != 0x10)
        return;
    (*((unsigned char *)&self->stars[0].step + i * 0x4c)) += 1;
    e->radius = 0x38000;
    e->heading = data_ov002_021000d8[i];
    e->phase = 0;
    e->loops = 0;
    e->orbitTravel = 0;
    e->angSpeed = 0x200;
    e->kiraDefParam = 0x1000;
    e->kiraSysParam = 0x200;
}

void StaffRoll::func_ov002_020f562c(StaffRoll *self, int idx) {
    unsigned char sel = self->stars[idx].step;
    (self->*data_ov002_02110eec[sel])(idx);
}

void StaffRoll::func_ov002_020f5678(StaffRoll *self, int i) {
    Star *e = &self->stars[i];
    e->kiraDefParam = 0xccc;
    e->kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f569c(StaffRoll *self, int i) {
    int ax;
    int by;
    int tx;
    int ty;

    if (self->stars[i].delay != 0) {
        self->stars[i].delay =
            self->stars[i].delay - 1;
        return;
    }

    self->stars[i].kiraDefParam = 0x1000;
    self->stars[i].kiraSysParam = 0x200;

    self->stars[i].x =
        self->stars[i].x + self->stars[i].speed;
    self->stars[i].y =
        self->stars[i].y + self->stars[i].speedY;

    tx = data_ov002_02100190[i * 2];
    ty = data_ov002_02100190[i * 2 + 1];
    ax = self->stars[i].x >> 12;
    by = self->stars[i].y >> 12;

    if (self->stars[i].speed > 0) {
        if (ax > tx)
            self->stars[i].x = tx << 12;
    } else {
        if (ax < tx)
            self->stars[i].x = tx << 12;
    }

    if (self->stars[i].speedY > 0) {
        if (by > ty)
            self->stars[i].y = ty << 12;
    } else {
        if (by < ty)
            self->stars[i].y = ty << 12;
    }

    ax = self->stars[i].x >> 12;
    by = self->stars[i].y >> 12;

    if (ax == tx && by == ty) {
        self->stars[i].step =
            self->stars[i].step + 1;
    }
}

void StaffRoll::func_ov002_020f57c0(StaffRoll *self, int idx) {
    if (self->stars[idx].frame != 0)
        return;
    if (self->stars[idx].frameTimer != 0)
        return;
    self->stars[idx].sparkling = 1;
    self->stars[idx].visible = 1;
    self->stars[idx].step++;
    self->stars[idx].speed = data_ov002_02100170[idx * 2];
    self->stars[idx].speedY = data_ov002_02100170[idx * 2 + 1];
    if (idx == 0)
        self->stars[idx].delay = 0x78;
    else
        self->stars[idx].delay = 0x20;
}

void StaffRoll::func_ov002_020f5848(StaffRoll *self, int idx) {
    unsigned char sel = self->stars[idx].step;
    (self->*data_ov002_02110e64[sel])(idx);
}

void StaffRoll::func_ov002_020f5894(StaffRoll *self, int i) {
    int off;
    unsigned short *p34;
    short s2c;
    if (i != 0)
        return;
    off = i * 0x4c;
    p34 = (unsigned short *)((char *)&self->stars[0].kiraTimer + off);
    *p34 = *p34 + 1;
    if (*(int *)((char *)&self->stars[0].kiraDefParam + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int *)((char *)&self->stars[0].kiraDefParam + off) =
                *(int *)((char *)&self->stars[0].kiraDefParam + off) + 0x199;
            if (*(int *)((char *)&self->stars[0].kiraDefParam + off) >= 0x1000)
                *(int *)((char *)&self->stars[0].kiraDefParam + off) = 0x1000;
        }
    }
    s2c = *(short *)((char *)&self->stars[0].kiraSysParam + off);
    if (s2c != 0x800) {
        if (((StaffRoll *)((char *)self + off))->stars[0].kiraPeriods >= 5) {
            if ((*p34 & 3) == 3) {
                *(short *)((char *)&self->stars[0].kiraSysParam + off) = s2c + 0x199;
                if (*(short *)((char *)&self->stars[0].kiraSysParam + off) >= 0x800)
                    *(short *)((char *)&self->stars[0].kiraSysParam + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e)
        return;
    *p34 = 0;
    {
        unsigned short *p3e =
            (unsigned short *)((char *)&self->stars[0].kiraPeriods + off);
        *p3e = *p3e + 1;
    }
}

void StaffRoll::func_ov002_020f5990() {
    int i;
    StaffRoll::Star *s = this->stars;
    {
        int *cnt = &this->kiraStar;
        *cnt = *cnt + 1;
        *cnt = *cnt & 3;
    }
    for (i = 0; i < 4; i++, s++) {
        if (s->active == 0)
            continue;
        ((this)->*data_ov002_02110e7c[s->state])(i);
        func_ov002_020f39ec(i);
        if (s->sparkling == 0)
            continue;
        if (i != this->kiraStar)
            continue;
        func_ov002_020f2790((s16)(s->x >> 12), (s16)(s->y >> 12), s->kiraDefParam, s->kiraSysParam);
    }
}

void StaffRoll::func_ov002_020f5a6c() {
    Star *p = this->stars;
    int i;
    for (i = 0; i < 4; i++) {
        p->active = 0;
        p->visible = 0;
        p->holdFrame = 0;
        p++;
    }
}

int StaffRoll::func_ov002_020f5a94() {
    unsigned char count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (SaveData::IsCharacterUnlocked(i))
            count = count + 1;
    }
    return count;
}

void StaffRoll::func_ov002_020f5ad4() {
    /* StaffRoll::pad_1f0: two words cleared here and read nowhere. */
    this->pad_1f0[0] = 0;
    this->pad_1f0[1] = 0;
    this->kiraStar = 0;
    this->holdOrbit = 0;
    func_ov002_020f5a6c();
    func_ov002_020f396c();
    func_ov002_020f30d4();
    func_ov002_020f2ea0();
    func_ov002_020f2a68();
    func_ov002_020f2984();
}

void StaffRoll::func_ov002_020f5b24(StaffRoll *self) {
    gSubScreenLayers &= ~6;
    gSubScreenLayers |= 8;
    self->func_ov002_020f2f64();
    *(volatile unsigned int *)0x4001000 &= ~0xe000;
    self->window[0].active = 0;
    self->func_ov002_020f2a48();
}

void StaffRoll::func_ov002_020f5b7c(StaffRoll *self) {}

void StaffRoll::func_ov002_020f5b80(StaffRoll *self) {}

void StaffRoll::func_ov002_020f5b84(StaffRoll *self) {}

void StaffRoll::func_ov002_020f5b88(StaffRoll *self) {}

void StaffRoll::func_ov002_020f5b8c(StaffRoll *self) {
    self->holdOrbit = 1;
}

void StaffRoll::func_ov002_020f5b98(StaffRoll *self) {
    StaffRoll::Star *e = self->stars;
    int i;
    for (i = 0; i < 4; i++) {
        if (e->active != 0) {
            e->state = 2;
            e->step = 0;
            e->holdFrame = 0;
        }
        e++;
    }
}

void StaffRoll::func_ov002_020f5bcc(StaffRoll *self) {
    Star *p = self->stars;
    int i;
    for (i = 0; i < 4; i++) {
        if (p->active)
            p->holdFrame = 1;
        p++;
    }
}

void StaffRoll::func_ov002_020f5bf4(StaffRoll *self) {
    if (self->stars[3].active == 0)
        return;
    self->stars[3].x = 0x80000;
    self->stars[3].y = 0x60000;
    self->stars[3].delay = 0;
    self->stars[3].state = 1;
    self->stars[3].kiraDefParam = 0xccc;
    self->stars[3].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5c40(StaffRoll *self) {
    if (!self->stars[2].active)
        return;
    self->stars[2].x = 0x80000;
    self->stars[2].y = 0x60000;
    self->stars[2].delay = 0;
    self->stars[2].state = 1;
    self->stars[2].kiraDefParam = 0xccc;
    self->stars[2].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5c88(StaffRoll *self) {
    if (self->stars[1].active == 0)
        return;
    self->stars[1].x = 0x80000;
    self->stars[1].y = 0x60000;
    self->stars[1].delay = 0;
    self->stars[1].state = 1;
    self->stars[1].kiraDefParam = 0xccc;
    self->stars[1].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5cd0(StaffRoll *self) {
    StaffRoll::Star *c = self->stars;
    int i;
    if (c->active == 0)
        return;
    c->x = 0x80000;
    c->y = 0x60000;
    c->delay = 0;
    c->state = 1;
    c->kiraDefParam = 0xccc;
    c->kiraSysParam = 0x155;
    for (i = 0; i < 4; i++) {
        c->frame = 0;
        c->frameTimer = 0;
        c++;
    }
}

void StaffRoll::func_ov002_020f5d34(StaffRoll *self) {
    int n = self->func_ov002_020f5a94();
    int i = 0;
    Star *e;
    int v;
    if (n <= 0)
        goto done;
    e = self->stars;
    v = 0;
    do {
        /* +0x38 is Star::pad_38: written twice here, read nowhere. */
        e->active = 1;
        e->visible = 0;
        e->heading = 0;
        e->pad_38 = 0;
        e->x = 0x80000;
        e->y = 0x60000;
        e->delay = v;
        e->frameTimer = 0;
        e->kiraTimer = 0;
        e->kiraPeriods = 0;
        e->kiraDefParam = 0x199;
        e->kiraSysParam = 0x199;
        e->pad_38 = 0;
        e->state = 0;
        e->step = 0;
        e->sparkling = 0;
        e++;
        v += 0x40;
        i++;
    } while (i < n);
done:
    self->func_ov002_020f2958();
}

extern PMF0 data_ov002_02110f34[];
void StaffRoll::func_ov002_020f5dd8(int idx) {
    (this->*data_ov002_02110f34[idx])();
}

void StaffRoll::func_ov002_020f5e18(StaffRoll *self) {
    self->func_ov002_020f37fc();
    self->func_ov002_020f2f9c(4);
}

void StaffRoll::func_ov002_020f5e38(StaffRoll *self) {
    self->func_ov002_020f3828();
    self->func_ov002_020f2f9c(4);
}

void StaffRoll::func_ov002_020f5e58(StaffRoll *self) {
    self->func_ov002_020f3828();
    self->func_ov002_020f2f9c(3);
}

void StaffRoll::func_ov002_020f5e78(StaffRoll *self) {
    self->func_ov002_020f3828();
    self->func_ov002_020f2f9c(2);
}

void StaffRoll::func_ov002_020f5e98(StaffRoll *self) {
    self->func_ov002_020f3828();
    self->func_ov002_020f2f9c(1);
}

void StaffRoll::func_ov002_020f5eb8(StaffRoll *self) {
    self->func_ov002_020f3828();
    self->func_ov002_020f2f9c(0);
}

void StaffRoll::func_ov002_020f5ed8(StaffRoll *self) {
    self->func_ov002_020f3828();
}

void StaffRoll::func_ov002_020f5ee4(StaffRoll *self) {
    self->func_ov002_020f3828();
}

void StaffRoll::func_ov002_020f5ef0(StaffRoll *self) {
    self->func_ov002_020f3954();
}

void StaffRoll::func_ov002_020f5efc(StaffRoll *self) {
    LoadFont(0);
}

void StaffRoll::func_ov002_020f5f0c(int idx) {
    (this->*data_ov002_02110f9c[idx])();
    this->StartCredit(idx);
}

void StaffRoll::func_ov002_020f5f60() {
    void *p = this->starChars;
    if (p == 0)
        return;
    Memory::operator_delete2(p);
    this->starChars = 0;
}

int StaffRoll::func_ov002_020f5f8c() {
    this->func_ov002_020f26e0();
    func_ov002_020f3978();
    func_ov002_020f2eac();
    return func_ov002_020f2990();
}

int StaffRoll::func_ov002_020f5fb8() {
    func_ov002_020f5990();
    func_ov002_020f37a0();
    func_ov002_020f2dd4();
    return func_ov002_020f27e8();
}

void StaffRoll::func_ov002_020f5fe4() {
    void *f;
    void *r5;

    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    GXS::SetGraphicsMode(0);

    gSubScreenLayers &= ~0x1f;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCG_BIN);
    DecompressLZ16(f, (void *)0x6400000);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCL_BIN);
    GX::LoadOBJPltt(f, 0, 0x200);
    Deallocate(f);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3);
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1100;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3);
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0xe08;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & ~3) | 2;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x800;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCG_BIN);
    DecompressLZ16(f, (void *)G2S::GetBG1CharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCL_BIN);
    GX::LoadBGPltt(f, 0, 2);
    GXS::LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK1_NSC_BIN);
    func_02056434(f, 0, 0x800);
    Deallocate(f);

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0xc90;

    if (GetOwnerLanguage() == 5) {
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_S_NCG_BIN);
    } else if (GetOwnerLanguage() == 4) {
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_I_NCG_BIN);
    } else if (GetOwnerLanguage() == 3) {
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_G_NCG_BIN);
    } else if (GetOwnerLanguage() == 2) {
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_F_NCG_BIN);
    } else {
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCG_BIN);
    }

    DecompressLZ16(r5, (void *)G2S::GetBG3CharPtr());
    Deallocate(r5);

    r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCL_BIN);
    CP15::FlushAndInvalidateDataCache((u32)r5, 0x200);
    GX::SetBankForSubBGExtPltt(0x80);
    GXS::BeginLoadBGExtPltt();
    GXS::LoadBGExtPltt(r5, 0x4000, 0x200);
    GXS::LoadBGExtPltt(r5, 0x6000, 0x200);
    GXS::EndLoadBGExtPltt();
    Deallocate(r5);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_BRIGHT_NSC_BIN);
    func_020562b4(f, 0, 0x800);
    Deallocate(f);

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0xa90;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK2_NSC_BIN);
    func_02056374(f, 0, 0x800);
    Deallocate(f);

    gSubScreenLayers |= 0x10;

    {
        void *f6 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCG_BIN);
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCL_BIN);
        DecompressLZ16(f6, (void *)0x6600000);
        GXS::LoadOBJPltt(r5, 0, 0x200);
        Deallocate(f6);
        Deallocate(r5);
    }

    this->starChars = 0;
    if (this->starChars == 0) {
        this->starChars = (u8 *)Memory::operator_new2(0x4000);
    }
    f = LoadFile(ASSET_D_2D_ED_STAR_NCG_BIN);
    DecompressLZ16(f, this->starChars);
    Deallocate(f);

    func_ov002_020f5ad4();
}

int daDemo_c::func_ov002_020f63a0() {
    this->mStaffRoll->func_ov002_020f5f60();
    if (this->mStaffRoll != 0) {
        Memory::operator_delete2(this->mStaffRoll);
        this->mStaffRoll = 0;
    }
    return 1;
}

int daDemo_c::func_ov002_020f63d4() {
    this->mStaffRoll->func_ov002_020f5fb8();
    this->mStaffRoll->func_ov002_020f5f8c();
    return 1;
}

int daDemo_c::func_ov002_020f63f8(daDemo_c *self, unsigned char *src) {
    self->mCue = *src;
    self->mStaffRoll->func_ov002_020f5dd8((int)self->mCue);
    return 1;
}

int daDemo_c::func_ov002_020f6424() {
    this->mStaffRoll->func_ov002_020f5f0c((int)this->mCue);
    return 1;
}

int daDemo_c::func_ov002_020f6448(daDemo_c *self, unsigned char *arg1) {
    StaffRoll *p;
    self->mCue = *arg1;
    p = new StaffRoll;
    self->mStaffRoll = p;
    if (self->mStaffRoll == 0)
        return 0;
    self->mStaffRoll->func_ov002_020f5fe4();
    return self->func_ov002_020f6424();
}

void daDemo_c::anmModel_c::func_ov002_020f64ac(char *r4) {
    Advance();
    if (!Finished())
        return;
    signed char r2 = *(signed char *)(r4 + (mAnimEntry << 2) + 3);
    if (r2 < 0)
        return;
    func_ov002_020f6514((u8 *)r4, (u8)(r2 & 0xff));
}

void daDemo_c::anmModel_c::func_ov002_020f6514(void *tbl, unsigned char arg) {
    u8 *e;
    int v;
    mAnimEntry = arg;
    e = (u8 *)tbl + mAnimEntry * 4;
    v = (e[2] == 0) ? 0x40000000 : 0;
    ((dExtAnmModel_c *)this)->SetAnim((BCA_File *)(mAnimFiles[e[0]]->filePtr), v, 0x1000, 0);
    {
        dExtAnmTexPat_c *ts = mTexSeq;
        s8 idx;
        if (ts == 0)
            return;
        idx = (s8)e[1];
        if (idx < 0)
            return;
        ts->SetFile(*(BTP_File *)mTexSeqFiles[idx]->filePtr, v, 0x1000, 0);
    }
}

void daDemo_c::anmModel_c::func_ov002_020f65b8() {
    if (mTexSeq == 0)
        return;
    mTexSeq->Update(data);
}

void daDemo_c::anmModel_c::func_ov002_020f65ec() {
    if (mTexSeq != 0)
        mTexSeq->Advance();
}

int daDemo_c::anmModel_c::func_ov002_020f6618(SharedFilePtr *mdl, int nAnims, SharedFilePtr **anims,
                                              int arg5, unsigned char texByte, SharedFilePtr **texs,
                                              int tsData) {
    char *self = (char *)this;
    int i;
    mModelFile = mdl;
    if (((dExtModel_c *)self)
            ->SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*mModelFile), 1, tsData) == 0)
        return 0;
    mNumAnimFiles = (u8)nAnims;
    mAnimFiles = anims;
    for (i = 0; i < mNumAnimFiles; i++) {
        dExtFrameCtrl_c::LoadFile(*mAnimFiles[i]);
    }
    ((dExtAnmModel_c *)self)->SetAnim((BCA_File *)(mAnimFiles[0]->filePtr), 0, 0x1000, 0);
    if (texByte) {
        mTexSeq = new dExtAnmTexPat_c;
        if (mTexSeq == 0)
            return 0;
        mNumTexSeqFiles = texByte;
        mTexSeqFiles = texs;
        for (i = 0; i < mNumTexSeqFiles; i++) {
            void *bmd = mModelFile->filePtr;
            void *btp = dExtAnmTexPat_c::LoadFile(*mTexSeqFiles[i]);
            dExtAnmTexPat_c::Prepare(*(BMD_File *)bmd, *(BTP_File *)btp);
        }
        mTexSeq->SetFile(*(BTP_File *)mTexSeqFiles[0]->filePtr, 0, 0x1000, 0);
    }
    mSnapToFloor = (u8)arg5;
    return 1;
}

daDemo_c::anmModel_c::~anmModel_c() {
    SharedFilePtr *file;
    int i;

    file = mModelFile;
    if (file != 0)
        file->Release();
    for (i = 0; i < mNumAnimFiles; i++) {
        file = mAnimFiles[i];
        if (file != 0)
            file->Release();
    }
    if (mTexSeq != 0) {
        for (i = 0; i < mNumTexSeqFiles; i++) {
            file = mTexSeqFiles[i];
            if (file != 0)
                file->Release();
        }
        delete mTexSeq;
    }
}

int daDemo_c::simpleModel_c::func_ov002_020f6960(SharedFilePtr *fp, int n) {
    mFile = fp;
    BMD_File *f = (BMD_File *)dExtSimpleModel_c::LoadFile(*mFile);
    return ((dExtModel_c *)this)->SetFile(f, 1, n) != 0 ? 1 : 0;
}

#pragma defer_codegen on
daDemo_c::simpleModel_c::~simpleModel_c() {
    SharedFilePtr *file = mFile;
    if (file != 0) {
        file->Release();
    }
}
#pragma defer_codegen off

daDemo_c::param_c::param_c() {
    mScale[0].x = 0;
    mScale[0].y = 0;
    mScale[0].z = 0;
}

int daDemo_c::func_ov002_020f6a9c(daDemo_c *self) {
    self->MarkForDestruction();
    return 1;
}

int daDemo_c::func_ov002_020f6ab8(daDemo_c *self, unsigned char *p) {
    unsigned int id = ReadUnalignedInt(p);
    Sound::Play(1, id, self->mCamSpacePos);
    return 1;
}

int daDemo_c::func_ov002_020f6ae4(daDemo_c *self, unsigned char *p) {
    unsigned int id = ReadUnalignedInt(p);
    self->mSoundHandle = Sound::PlayLong(self->mSoundHandle, 3, id, self->mCamSpacePos, 0);
    return 1;
}

int daDemo_c::func_ov002_020f6b28(daDemo_c *self, unsigned char *p) {
    unsigned int v = ReadUnalignedInt(p);
    func_02012694(v, &self->mCamSpacePosX);
    return 1;
}

/* Script handler 15: turn to face the point whose whole-unit coordinates
 * are the unaligned shorts at p+0/+2/+4, and take the short at p+6 as the
 * horizontal speed. The legacy per-function file recorded its match under
 * mwccarm 1.2/sp2p3; this TU builds under the 2004/b56 pin. */

int daDemo_c::func_ov002_020f6b4c(daDemo_c *self, unsigned char *p) {
    Vector3 target;
    int y, z;
    z = ReadUnalignedShort(p + 4) << 12;
    y = ReadUnalignedShort(p + 2) << 12;
    target.x = ReadUnalignedShort(p) << 12;
    target.y = y;
    target.z = z;
    self->mAngleY = Vec3_HorzAngle(&self->mPos, &target);
    self->mPrevAngleY = self->mAngleY;
    self->mHorzSpeed = ReadUnalignedShort(p + 6);
    return 1;
}

s32 daDemo_c::func_ov002_020f6bc0(daDemo_c *self, unsigned char *data) {
    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    self->mPosX = x;
    self->mPosY = y;
    self->mPosZ = z;
    self->mAngleY = ReadUnalignedShort(data + 6);
    self->mPrevAngleY = self->mAngleY;
    return 1;
}

int daDemo_c::func_ov002_020f6c24(daDemo_c *self, unsigned char *src) {
    self->mOpacity = src[0];
    return 1;
}

int daDemo_c::func_ov002_020f6c34(daDemo_c *self, unsigned char *src) {
    unsigned char v = *src;
    ((anmModel_c *)self->mAnmModel)->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, v);
    return 1;
}

inline unsigned int inline_fn(unsigned int arg0) {
    return arg0;
}

int daDemo_c::func_ov002_020f6c60(daDemo_c *self, void *arg1, int arg2, int arg3) {
    u8 *flag = (u8 *)arg1;
    VecFx32 v;
    if (arg2 == gKuppaScriptFrame) {
        if ((*flag) != 0) {
            int r = (inline_fn(RandomIntInternal(&data_0209e650)) >> 20) & 0xfff;
            int m = FX_MUL(r, 0xa000);
            self->mMagnitude = m + 0x19000;
        }
        AddVec3((Vector3 *)&self->mPosX, &((dCamera_c *)gCamera)->pos, (Vector3 *)&self->mPosX);
        if (arg2 > 0x2bc) {
            self->mPosY -= 0x96000;
        }
        if (arg2 != data_ov002_0210b614) {
            func_0201267c(0x6a, &self->mCamSpacePos);
            data_ov002_0210b614 = arg2;
        }
    }
    if (arg3 == gKuppaScriptFrame) {
        self->mOpacity = 0;
        return 1;
    }
    self->mOpacity = 0x1f;
    if ((*flag) == 0) {
        /* First call stores the look-at point and registers ~Vector3(). */
        static __attribute__((aligned(8))) Vector3 sLookTarget(0xffdd6000, 0xbe4000, 0x2ae000);
        self->func_ov002_020f6f48(&sLookTarget, 0x20);
    } else {
        MulVec3Mat4x3((Vector3 *)&data_ov002_0210b958, (Matrix4x3 *)&data_0209b41c, (Vector3 *)&v);
        Vec3_LslInPlace((int *)&v, 3);
        self->func_ov002_020f6f48((Vector3 *)&v, 8);
    }
    self->mPrevAngleX = self->mAngleX;
    self->mPrevAngleY = self->mAngleY;
    self->mPrevAngleZ = self->mAngleZ;
    return 1;
}

/* Script handler 10: position (p[0..5], whole units shifted into 20.12) and
 * angles (p[6..11]) from the record, mMagnitude = p[12] << 12, scale 0xb33, and
 * the animation's currFrame set from a random draw scaled by
 * numFramesAndFlags. */

int daDemo_c::func_ov002_020f6e48(daDemo_c *self, unsigned char *p) {
    int b;
    int c0;
    int d;
    int e;
    int a;
    int f;
    unsigned char new_var;
    int rv;
    int new_var2;
    a = ReadUnalignedShort(p + 4) << 12;
    b = ReadUnalignedShort(p + 2) << 12;
    c0 = ReadUnalignedShort(p) << 12;
    self->mPosX = c0;
    self->mPosY = b;
    self->mPosZ = a;
    d = ReadUnalignedShort(p + 0xa);
    e = ReadUnalignedShort(p + 8);
    f = ReadUnalignedShort(p + 6);
    self->mAngleX = (short)f;
    self->mAngleY = (short)e;
    self->mAngleZ = (short)d;
    self->mPrevAngleX = self->mAngleX;
    self->mPrevAngleY = self->mAngleY;
    self->mPrevAngleZ = self->mAngleZ;
    self->mMagnitude = (new_var = p[0xc]) << 12;
    self->mScaleX = 0xb33;
    self->mScaleY = 0xb33;
    self->mScaleZ = 0xb33;
    rv = self->mAnmModel->numFramesAndFlags;
    new_var2 = RandomIntInternal(&data_0209e650);
    self->mAnmModel->currFrame =
        ((unsigned short)(FX_MUL((int)((((unsigned int)new_var2) >> 20) & 0xfff), rv) >> 12)) << 12;
    return 1;
}

/* Turn mAngleX/mAngleY toward v by 1/amt of the remaining difference per
 * call (step capped at 0x4000, 90 degrees), then set mSpeedX, mVertSpeed and
 * mSpeedZ to mMagnitude times rotation elements 6..8 of the animated model's
 * matrix. */

void daDemo_c::func_ov002_020f6f48(struct Vector3 *v, int amt) {
    Matrix4x3 *m;
    ApproachAngle(&this->mAngleX, Vec3_VertAngle((&this->mPos), v), amt, 0x4000, 0);
    ApproachAngle(&this->mAngleY, Vec3_HorzAngle((&this->mPos), v), amt, 0x4000, 0);
    m = &this->mAnmModel->mat4x3;
    this->mSpeedX = FX_MUL(this->mMagnitude, m->m[6]);
    this->mVertSpeed = FX_MUL(this->mMagnitude, m->m[7]);
    this->mSpeedZ = FX_MUL(this->mMagnitude, m->m[8]);
}

/* Script handler 9: clear mFlags bit 0x2. */

int daDemo_c::func_ov002_020f7020(daDemo_c *self) {
    self->mFlags &= ~0x2;
    return 1;
}

int daDemo_c::func_ov002_020f7038(daDemo_c *self, int a, int arg) {
    if (arg == gKuppaScriptFrame) {
        self->mParticleID[0] = 0;
        self->mParticleID[1] = 0;
        self->mParticleID[2] = 0;
    }
    if (self->mPosY >= 0x514000) {
        if (self->mRate < 0x3c000) {
            self->mRate += 0x5000;
        }
    } else {
        if (self->mRate > 0x1a000) {
            self->mRate -= 0x2000;
        }
    }
    {
        anmModel_c *obj = (anmModel_c *)self->mAnmModel;
        if (obj->mAnimEntry == 0 && (self->mGroundFlags & 1)) {
            obj->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, 1);
        }
    }
    self->mPosY -= FX_MUL(self->mRate, 0x199);
    {
        struct Vector3 pos;
        int z = self->mPosZ;
        int x = self->mPosX;
        int y = self->mPosY + 0x50000;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        self->mParticleID[0] = Particle::System::New(self->mParticleID[0], kPtclDemoDescendA, pos.x,
                                                     pos.y, pos.z, 0, 0);
        self->mParticleID[1] = Particle::System::New(self->mParticleID[1], kPtclDemoDescendB, pos.x,
                                                     pos.y, pos.z, 0, 0);
        self->mParticleID[2] = Particle::System::New(self->mParticleID[2], kPtclDemoDescendC, pos.x,
                                                     pos.y, pos.z, 0, 0);
    }
    return 1;
}

int daDemo_c::func_ov002_020f71c4(daDemo_c *self, int a, int arg) {
    if (arg == 0) {
        if (arg == gKuppaScriptFrame) {
            self->mMagnitude = 0x10000;
        }
        ApproachLinear(self->mMagnitude, 0x1f000, 0x3e3);
        self->mOpacity = self->mMagnitude >> 12;
    }
    {
        struct Vector3 pos;
        int z = self->mPosZ;
        int x = self->mPosX;
        int y = self->mPosY + 0x50000;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        self->mParticleID[0] = Particle::System::New(self->mParticleID[0], kPtclDemoFadeInA, pos.x,
                                                     pos.y, pos.z, 0, 0);
        self->mParticleID[1] = Particle::System::New(self->mParticleID[1], kPtclDemoFadeInB, pos.x,
                                                     pos.y, pos.z, 0, 0);
        self->mParticleID[2] = Particle::System::New(self->mParticleID[2], kPtclDemoFadeInC, pos.x,
                                                     pos.y, pos.z, 0, 0);
    }
    return 1;
}

int daDemo_c::func_ov002_020f72bc() {
    *((u32 *)&this->mScaleX) = 0x3000;
    *((u32 *)&this->mScaleY) = 0x3000;
    *((u32 *)&this->mScaleZ) = 0x3000;
    {
        s16 *p = &this->mAngleY;
        *p = *p + 0x400;
    }
    this->mOpacity = 0x1f;

    mParticleID[0] =
        Particle::System::New(mParticleID[0], kPtclDemoSpinA, *((Fix12i *)&this->mPosX),
                              *((Fix12i *)&this->mPosY), *((Fix12i *)&this->mPosZ), 0, 0);
    mParticleID[1] =
        Particle::System::New(mParticleID[1], kPtclDemoSpinB, *(Fix12i *)(&this->mPosX),
                              *(Fix12i *)(&this->mPosY), *(Fix12i *)(&this->mPosZ), 0, 0);
    mParticleID[2] =
        Particle::System::New(mParticleID[2], kPtclDemoSpinC, *(Fix12i *)(&this->mPosX),
                              *(Fix12i *)(&this->mPosY), *((Fix12i *)&this->mPosZ), 0, 0);
    return 1;
}

int daDemo_c::func_ov002_020f7384(unsigned char *flag, int val) {
    if (*flag != 0) {
        if (val == gKuppaScriptFrame)
            func_02012790(0x17);
        ApproachLinear(this->mPosY, 0xfa000, 0xa000);
        this->mOpacity = 0x1f;
    } else {
        if (val == gKuppaScriptFrame)
            func_02012790(0x16);
        if (ApproachLinear(this->mPosY, 0x32000, 0x5000) != 0)
            this->mOpacity = 0;
    }
    return 1;
}

/* Script handler 4. On the record's start frame, when in[0] is non-zero, it
 * stores a fixed vector at mModel + 0x50 (the param_c tail base), sets fixed
 * angles and calls func_0201f138. Every frame mPos = that vector times
 * INV_VIEW_MATRIX_ASR_3, then << 3; mMagnitude approaches in[0] << 12 by
 * in[1] << 8 (func_0201ef38 is called when it arrives at 0 with in[1]
 * non-zero) and drives mOpacity and the blend registers at 0x4000050. */

int daDemo_c::func_ov002_020f7410(unsigned char *in, int sel) {
    unsigned char *self = (unsigned char *)this;
    int a = in[0] << 12;
    int b = in[1] << 8;
    int t;
    if (a != 0 && sel == gKuppaScriptFrame) {
        Vector3 *p = ((param_c &)*(simpleModel_c *)this->mModel).mScale;
        p->x = -0x5d00;
        p->y = 0x1800;
        p->z = -0x16000;
        this->mAngleX = 0;
        this->mAngleY = 0xffffb3c0;
        this->mAngleZ = 0;
        func_0201f138();
    }
    MulVec3Mat4x3(((param_c &)*(simpleModel_c *)this->mModel).mScale, (Matrix4x3 *)&data_0209b41c,
                  (&this->mPos));
    Vec3_LslInPlace(&this->mPosX, 3);
    if (ApproachLinear(this->mMagnitude, a, b) != 0 && a == 0 && b != 0)
        func_0201ef38();
    *(char *)(&this->mOpacity) = *(int *)(&this->mMagnitude) >> 12;
    {
        unsigned char r = *(unsigned char *)(&this->mOpacity);
        if (r != 0 || a != 0) {
            t = (r * 0xa00) >> 12;
            if (t > 0x10)
                t = 0x10;
            G2x::SetBlendAlpha((volatile u16 *)0x4000050, 8, 1, t, 0x10 - t);
        }
    }
    return 1;
}

/* Script handler 3. On the start frame, when arg1[0] is non-zero, it sets
 * fixed angles and clears mRate. Every frame Math_Function_0203b0fc moves
 * mRate toward 1.0 (arg1[0] non-zero) or 0, and the vector at mAnmModel +
 * 0x64 (the param_c tail base) becomes base + (sViewEnd - base) *
 * mRate, with base sViewStart or sViewIdle by the same
 * test. mPos = that vector times INV_VIEW_MATRIX_ASR_3, then << 3; mMagnitude
 * approaches arg1[0] << 12 by arg1[1] << 8 and drives mOpacity. */

int daDemo_c::func_ov002_020f7538(unsigned char *arg1, int arg2) {
    int a = arg1[0];
    int b = arg1[1];
    int f1 = a << 0xc;
    int f2 = b << 8;
    int *base;
    int scale;
    VecFx32 tmp, diff, scaled, sum;

    if (f1 != 0 && arg2 == gKuppaScriptFrame) {
        this->mAngleX = 0xd00;
        this->mAngleY = -0x7300;
        this->mAngleZ = 0x2f00;
        this->mRate = 0;
    }

    /* First call stores each view-space endpoint and registers ~Vector3(). */
    static Vector3 sViewStart(0x2800, 0xfffee900, -0x4200);
    static Vector3 sViewEnd(0x3300, 0xfffee900, -0x5e00);
    static Vector3 sViewIdle(0x4c00, -0xf200, -0xff00);

    if (f1 != 0) {
        base = (int *)&sViewStart;
        scale = 0x1000;
    } else {
        base = (int *)&sViewIdle;
        scale = 0;
    }

    Math_Function_0203b0fc(&this->mRate, scale, 0x40, 0x100);
    Vec3_Sub((Vector3 *)&diff, (const Vector3 *)&sViewEnd, (const Vector3 *)base);
    tmp.x = diff.x;
    tmp.y = diff.y;
    tmp.z = diff.z;
    Vec3_MulScalar(&scaled, &tmp, this->mRate);
    Vec3_Add((Vector3 *)&sum, (const Vector3 *)base, (const Vector3 *)&scaled);

    {
        Vector3 *dp = ((param_c &)*(anmModel_c *)this->mAnmModel).mScale;
        dp->x = sum.x;
        dp->y = sum.y;
        dp->z = sum.z;
    }
    MulVec3Mat4x3(((param_c &)*(anmModel_c *)this->mAnmModel).mScale, (Matrix4x3 *)&data_0209b41c,
                  &this->mPos);
    Vec3_LslInPlace((int *)(&this->mPosX), 3);
    ApproachLinear(this->mMagnitude, f1, f2);
    this->mOpacity = this->mMagnitude >> 0xc;
    return 1;
}

int daDemo_c::func_ov002_020f7780(void *unused, int mode) {
    dCamera_c *cam = (dCamera_c *)gCamera;

    if (mode == gKuppaScriptFrame) {
        if (mode == 0) {
            data_0208ee44 = 3;
            return 1;
        }

        {
            int v[3];
            unsigned char idx;
            v[0] = *(int *)((char *)data_ov002_0210b970 + 0);
            v[1] = *(int *)((char *)data_ov002_0210b970 + 4);
            v[2] = *(int *)((char *)data_ov002_0210b970 + 8);

            idx = *(volatile unsigned char *)&gLocalPlayerId;
            {
                short ang = ((dActor_c *)gPlayers[idx])->mAngleY;

                Vec3_RotateYAndTranslate(&this->mPosX, (int *)&cam->pos, (short)(ang + 0x8000), v);
            }
        }

        this->mPrevAngleX = 0x1000;
        this->mPrevAngleY = -0x7000;
        this->mAngleY = this->mPrevAngleY;
        this->mOpacity = 0x1f;
    } else {
        short state = *&this->mPhase;
        if (state <= 0x6a) {
            if (state > 0x3c) {
                Math_Function_0203b0fc(&this->mMagnitude, -0xa000, 0xcc, 0x7fffffff);
                short *p94 = &this->mPrevAngleY;
                short *p92 = &this->mPrevAngleX;
                *p94 = *p94 + 0x78;
                *p92 = *p92 + 0x40;

                short h = Vec3_HorzAngle((&this->mPos), &cam->pos);
                ApproachLinear2(this->mAngleY, h, 0x200);
            }

            state = *&this->mPhase;
            if (state > 0x69) {
                this->mPrevAngleX = 0xe00;
            }
        } else {
            Math_Function_0203b0fc(&this->mMagnitude, 0x3c000, 0xcc, 0x7fffffff);

            short h = Vec3_HorzAngle((&this->mPos), &cam->pos);
            ApproachLinear2(this->mAngleY, h, 0x200);

            state = *&this->mPhase;
            if (state < 0xa9) {
                UpdateAngle(*(short *)&this->mPrevAngleY, 0x1800, 0x1e, 0x4000);
            }
            ApproachLinear(this->mPrevAngleX, -0x3000, 0x70);
        }
    }

    {
        unsigned short a = *((unsigned short *)&this->mPrevAngleX);
        int t = this->mMagnitude;
        short s0 = data_02082214[((a >> 4) << 1) + 1];
        this->mHorzSpeed = FX_MUL(t, s0);
    }
    {
        unsigned short a = *(unsigned short *)(&this->mPrevAngleX);
        int t = this->mMagnitude;
        int s1 = -data_02082214[(a >> 4) << 1];
        this->mVertSpeed = FX_MUL(t, s1);
    }

    {
        short *pfc = &this->mPhase;
        *pfc = *pfc + 1;
    }

    return 1;
}

int daDemo_c::func_ov002_020f79c0(daDemo_c *self, void *unused, int mode) {

    if (mode == gKuppaScriptFrame) {
        self->mMagnitude = 0x578000;
        *&self->mPhase = -0x1000;
        *&self->mPhaseStep = 0x800;
        self->mRate = -0xc8000;
        self->mPrevAngleY = -0x8000;
        self->mAngleY = -0x4000;
        self->mPrevAngleX = 0x800;
        self->mOpacity = 0x1f;
    } else {
        int v[3];
        int w[3];
        int h;
        int t;

        {
            short *p94 = &self->mPrevAngleY;
            *p94 = *p94 + 0x200;
        }
        Math_Function_0203b0fc(&self->mMagnitude, 0x64000, 0x7a, 0x7fffffff);

        self->mAngleX = cstd::atan2(self->mPosY - 0x190000, 0xc8000);

        ApproachAngle(&self->mAngleY, (short)(self->mPrevAngleY + 0x8000), 4, 0x4000, 0);

        data_ov002_02110ddc[2] = self->mMagnitude;
        v[0] = data_ov002_0210b964[0];
        v[1] = data_ov002_0210b964[1];
        v[2] = data_ov002_0210b964[2];
        w[0] = data_ov002_02110ddc[0];
        w[1] = data_ov002_02110ddc[1];
        w[2] = data_ov002_02110ddc[2];
        Vec3_RotateYAndTranslate(&self->mPosX, v, self->mPrevAngleY, w);

        h = (unsigned short)self->mPhase;
        t = data_02082214[(h >> 4) * 2 + 1];
        {
            int *p60 = (int *)(&self->mPosY);
            *p60 = *p60 + FX_MUL(t, 0x96000);
        }

        {
            short *pfc = &self->mPhase;
            *pfc = *pfc + *&self->mPhaseStep;
        }
        ApproachAngle(&self->mPhaseStep, 0x200, 0x14, 0x4000, 0);

        {
            int *p5c = &self->mPosX;
            *p5c = *p5c + self->mRate;
        }
        Math_Function_0203b0fc(&self->mRate, 0, 0xcc, 0x7fffffff);
    }
    return 1;
}

int daDemo_c::func_ov002_020f7bb8(daDemo_c *self, unsigned char *p, u16 id) {
    Vector3 v0, v1;
    int src[3];
    Fix12i dist;
    s16 vertAngle, heading;
    dCamera_c *cam;
    unsigned int tbl1, tbl2;
    int i;

    self->mOpacity = 0;
    if (id == gKuppaScriptFrame) {
        self->mMagnitude = 0;
        self->mPhase = 0;
    }

    tbl1 = ReadUnalignedInt(p);
    tbl2 = ReadUnalignedInt(p + 4);

    if (MovePointAlongSpline(&v1, &self->mPhase, &self->mMagnitude, (CutsceneSpline *)tbl1) != 0) {
        return 0;
    }
    if (MovePointAlongSpline(&v0, &self->mPhase, &self->mMagnitude, (CutsceneSpline *)tbl2) != 0) {
        return 0;
    }

    self->mOpacity = 0x1f;
    cam = (dCamera_c *)gCamera;

    AddVec3((Vector3 *)&v0, &cam->pos, (Vector3 *)&v0);
    dist = Vec3_Dist(&v0, &cam->pos);

    {
        s16 t1 = cam->mAngleX;
        vertAngle = Vec3_VertAngle(&v0, &cam->pos);
        vertAngle += t1;
    }
    {
        s16 t2 = cam->mAngleY;
        heading = Vec3_HorzAngle(&v0, &cam->pos);
        heading += t2;
    }

    src[0] = 0;
    src[1] = FX_MUL(dist, data_02082214[(((u16)vertAngle) >> 4) * 2]);
    src[2] = FX_MUL(dist, data_02082214[(((u16)vertAngle) >> 4) * 2 + 1]);

    Vec3_RotateYAndTranslate((int *)(&self->mPosX), (int *)&cam->pos, heading, src);

    self->mAngleX = cstd::atan2(v1.y, Vec3_HorzLen(&v1));
    self->mAngleY = cstd::atan2(v1.x, v1.z);

    return 1;
}

void daDemo_c::RunCommand(unsigned char *p, int a2, int a3) {
    if ((data_ov002_02110b00 & 1) == 0) {
        data_ov002_02110b00 |= 1;
        data_ov002_0211104c[0] = *(PMF3 *)&data_ov002_0210b708;
        data_ov002_0211104c[1] = *(PMF3 *)&data_ov002_0210b678;
        data_ov002_0211104c[2] = data_ov002_0210b618;
        data_ov002_0211104c[3] = data_ov002_0210b620;
        data_ov002_0211104c[4] = data_ov002_0210b640;
        data_ov002_0211104c[5] = data_ov002_0210b628;
        data_ov002_0211104c[6] = data_ov002_0210b648;
        data_ov002_0211104c[7] = *(PMF3 *)&data_ov002_0210b8c0;
        data_ov002_0211104c[8] = *(PMF3 *)&data_ov002_0210b900;
        data_ov002_0211104c[9] = *(PMF3 *)&data_ov002_0210b938;
        data_ov002_0211104c[10] = *(PMF3 *)&data_ov002_0210b930;
        data_ov002_0211104c[11] = *(PMF3 *)&data_ov002_0210b928;
        data_ov002_0211104c[12] = *(PMF3 *)&data_ov002_0210b920;
        data_ov002_0211104c[13] = *(PMF3 *)&data_ov002_0210b918;
        data_ov002_0211104c[14] = *(PMF3 *)&data_ov002_0210b910;
        data_ov002_0211104c[15] = *(PMF3 *)&data_ov002_0210b908;
        data_ov002_0211104c[16] = *(PMF3 *)&data_ov002_0210b750;
        data_ov002_0211104c[17] = *(PMF3 *)&data_ov002_0210b8f8;
        data_ov002_0211104c[18] = *(PMF3 *)&data_ov002_0210b8f0;
        data_ov002_0211104c[19] = *(PMF3 *)&data_ov002_0210b8e8;
        data_ov002_0211104c[20] = *(PMF3 *)&data_ov002_0210b8e0;
        data_ov002_0211104c[21] = *(PMF3 *)&data_ov002_0210b8d8;
        data_ov002_0211104c[22] = *(PMF3 *)&data_ov002_0210b8d0;
        data_ov002_0211104c[23] = *(PMF3 *)&data_ov002_0210b8c8;
    }
    (this->*data_ov002_0211104c[p[6]])(p + 7, a2, a3);
}

int daDemo_c::CleanupResources() {
    int r1 = param1;
    if (r1 == 0x2e)
        return func_ov002_020f63a0();
    if (r1 == 0x2f)
        return func_ov002_020f23d0();
    if (mModel)
        delete mModel;
    if (mAnmModel)
        delete mAnmModel;
    return 1;
}

/* daDemo_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daDemo_c::OnPendingDestroy() {}

int daDemo_c::Render() {
    if (param1 == 0x19) {
        struct {
            char *p;
            char *cur;
            Matrix4x3 *src;
        } s;
        s.src = &data_0209b41c;
        s.p = (char *)&mModel->mat4x3;
        s.cur = s.p;
        *(Matrix4x3 *)s.cur = *s.src;
        int *tbl = data_ov002_0210bb7c;
        int i = 0;
        do {
            ((Matrix4x3 *)s.p)->m[9] = tbl[0];
            ((Matrix4x3 *)s.p)->m[10] = tbl[1];
            ((Matrix4x3 *)s.p)->m[11] = tbl[2];
            mModel->Render(0);
            tbl += 3;
            i++;
        } while ((unsigned)i < 3u);
        return 1;
    }
    unsigned char op = mOpacity;
    if (op == 0)
        return 1;
    if (mModel != 0) {
        mModel->ApplyOpacity(op, 0);
        mModel->Render(&mScale);
    } else if (mAnmModel != 0) {
        ((anmModel_c *)mAnmModel)->func_ov002_020f65b8();
        mAnmModel->ApplyOpacity(mOpacity, 0);
        mAnmModel->Render(&mScale);
    }
    return 1;
}

int daDemo_c::Behavior() {
    Vector3 v;
    VecFx32 asr;
    int new_var2;
    s32 t;
    t = this->param1;
    if (t == 0x2e) {
        return func_ov002_020f63d4();
    }
    if (t == 0x2f) {
        return func_ov002_020f23f0();
    }
    if ((((u32)t) >= 0x1a) && (((u32)t) <= 0x2d)) {
        UpdatePosWithOnlySpeed(0);
    } else {
        UpdatePos(0);
    }
    {
        anmModel_c *g = (anmModel_c *)this->mAnmModel;
        if ((g != 0) && (g->mSnapToFloor != 0)) {
            {
                s32 xx = this->mPosX;
                s32 yy = this->mPosY;
                s32 zz = this->mPosZ;
                new_var2 = yy + 0x96000;
                v.x = xx;
                v.y = new_var2;
                v.z = zz;
            }
            dBgCh_Gnd ground;
            ground.SetObjAndPos(v, 0);
            if (ground.DetectClsn() != 0) {
                s32 h = ground.clsnY;
                if ((this->mPosY) < h) {
                    this->mPosY = h;
                    this->mGroundFlags |= 1;
                }
            }
        }
    }
    Vec3_Asr((Vector3 *)&asr, (Vector3 *)&this->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    t = this->param1;
    if ((((u32)t) >= 0x1a) && (((u32)t) <= 0x2d)) {
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, this->mAngleX, this->mAngleY,
                                               this->mAngleZ);
    } else {
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, this->mAngleX, this->mAngleY,
                                               this->mAngleZ);
    }
    if (this->mModel != 0) {
        this->mModel->mat4x3 = data_020a0e68;
    }
    if (this->mAnmModel != 0) {
        this->mAnmModel->mat4x3 = data_020a0e68;
        (((anmModel_c *)this->mAnmModel))->func_ov002_020f64ac((char *)&data_ov002_0210bc88);
        (((anmModel_c *)this->mAnmModel))->func_ov002_020f65ec();
    }
    return 1;
}

struct DemoModelFile : SharedFilePtr {
    DemoModelFile(unsigned int fileID);
    ~DemoModelFile();
};
struct DemoAnimFile : SharedFilePtr {
    DemoAnimFile(unsigned int fileID);
    ~DemoAnimFile();
};
struct DemoTexSeqFile : SharedFilePtr {
    DemoTexSeqFile(unsigned int fileID);
    ~DemoTexSeqFile();
};

int daDemo_c::InitResources() {
    int t;

    if (param1 == 0x12) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        if (((anmModel_c *)this->mAnmModel)
                ->func_ov002_020f6618(&data_ov085_0213074c, 1,
                                      (SharedFilePtr **)&data_ov002_0210b60c, 1, 1,
                                      (SharedFilePtr **)&data_ov002_0210b608, -1) == 0)
            return 0;
    } else if (param1 == 0x13) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        t = (*(volatile unsigned char *)&gGameMode == 2);
        if (t == 0) {
            if (((anmModel_c *)this->mAnmModel)
                    ->func_ov002_020f6618(&data_ov002_02110b98, 1,
                                          (SharedFilePtr **)&data_ov002_0210b610, 1, 1,
                                          (SharedFilePtr **)&data_ov002_0210b600, 0x16) == 0)
                return 0;
        } else {
            if (((anmModel_c *)this->mAnmModel)
                    ->func_ov002_020f6618(&data_ov002_02110c18, 0xD,
                                          (SharedFilePtr **)&data_ov002_0210bcf0, 1, 0xD,
                                          (SharedFilePtr **)&data_ov002_0210bd24, 0x16) == 0)
                return 0;
        }
    } else if (param1 >= 0x14 && param1 <= 0x16) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0)
            return 0;
        if (((simpleModel_c *)this->mModel)->func_ov002_020f6960(&data_ov002_02110b70, -1) == 0)
            return 0;
    } else if (param1 == 0x17) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0)
            return 0;
        if (((simpleModel_c *)this->mModel)->func_ov002_020f6960(&data_ov002_02110b50, 0x19) == 0)
            return 0;
    } else if (param1 == 0x18) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0)
            return 0;
        if (((simpleModel_c *)this->mModel)
                ->func_ov002_020f6960((SharedFilePtr *)&data_ov002_0211094c, -1) == 0)
            return 0;
    } else if (param1 == 0x19) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0)
            return 0;
        if (((simpleModel_c *)this->mModel)->func_ov002_020f6960(&data_ov002_02110b78, 0x13) == 0)
            return 0;
    } else if (param1 >= 0x1A && param1 <= 0x2D) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        if (((anmModel_c *)this->mAnmModel)
                ->func_ov002_020f6618((SharedFilePtr *)data_ov009_02113c20, 1,
                                      (SharedFilePtr **)&data_ov002_0210b604, 0, 0, 0, -1) == 0)
            return 0;
    }
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

extern "C" daDemo_c *daDemo_c_classInit(void) {
    return new daDemo_c;
}

extern "C" {
int data_ov002_0210b658[2] = {(int)0x81f020f0, (int)0xffff0100};
int data_ov002_0210b6e8[2] = {(int)0x81f020f0, (int)0xffff0118};
int data_ov002_0210b768[2] = {(int)0x41f800f8, (int)0xffff701e};
int data_ov002_0210b868[2] = {(int)0x41f800f8, (int)0xffff705e};
struct OamAttr *data_ov002_0210b6a8[2] = {(struct OamAttr *)data_ov002_0210b768,
                                          (struct OamAttr *)data_ov002_0210b868};

W data_ov002_0210b650 = {(int)&StaffRoll::func_ov002_020f5b8c, 0};
W data_ov002_0210b660 = {(int)&StaffRoll::func_ov002_020f5e58, 0};
W data_ov002_0210b668 = {(int)&StaffRoll::func_ov002_020f5894, 0};
W data_ov002_0210b670 = {(int)&StaffRoll::func_ov002_020f5e58, 0};
W data_ov002_0210b678 = {(int)&daDemo_c::func_ov002_020f79c0, 0};
W data_ov002_0210b680 = {(int)&StaffRoll::func_ov002_020f5c40, 0};
W data_ov002_0210b688 = {(int)&StaffRoll::func_ov002_020f5e98, 0};
W data_ov002_0210b690 = {(int)&StaffRoll::func_ov002_020f5ef0, 0};
W data_ov002_0210b698 = {(int)&StaffRoll::func_ov002_020f5e98, 0};
W data_ov002_0210b6a0 = {(int)&StaffRoll::func_ov002_020f5ee4, 0};
W data_ov002_0210b6b0 = {(int)&StaffRoll::func_ov002_020f35e4, 0};
W data_ov002_0210b6b8 = {(int)&StaffRoll::func_ov002_020f5cd0, 0};
W data_ov002_0210b6c0 = {(int)&StaffRoll::func_ov002_020f3310, 0};
W data_ov002_0210b6c8 = {(int)&StaffRoll::func_ov002_020f5bcc, 0};
W data_ov002_0210b6d0 = {(int)&StaffRoll::func_ov002_020f5b98, 0};
W data_ov002_0210b6d8 = {(int)&StaffRoll::func_ov002_020f3ba0, 0};
W data_ov002_0210b6e0 = {(int)&StaffRoll::func_ov002_020f2a78, 0};
W data_ov002_0210b6f0 = {(int)&StaffRoll::func_ov002_020f2a88, 0};
W data_ov002_0210b6f8 = {(int)&StaffRoll::func_ov002_020f2aec, 0};
W data_ov002_0210b700 = {(int)&StaffRoll::func_ov002_020f3d98, 0};
W data_ov002_0210b708 = {(int)&daDemo_c::func_ov002_020f7bb8, 0};
W data_ov002_0210b710 = {(int)&StaffRoll::func_ov002_020f5328, 0};
W data_ov002_0210b718 = {(int)&StaffRoll::func_ov002_020f5efc, 0};
W data_ov002_0210b720 = {(int)&StaffRoll::func_ov002_020f5bf4, 0};
W data_ov002_0210b728 = {(int)&StaffRoll::func_ov002_020f40fc, 0};
W data_ov002_0210b730 = {(int)&StaffRoll::func_ov002_020f4710, 0};
W data_ov002_0210b738 = {(int)&StaffRoll::func_ov002_020f4a2c, 0};
W data_ov002_0210b740 = {(int)&StaffRoll::func_ov002_020f4d70, 0};
W data_ov002_0210b748 = {(int)&StaffRoll::func_ov002_020f5010, 0};
W data_ov002_0210b750 = {(int)&daDemo_c::func_ov002_020f6b28, 0};
W data_ov002_0210b758 = {(int)&StaffRoll::func_ov002_020f55b4, 0};
W data_ov002_0210b760 = {(int)&StaffRoll::func_ov002_020f30f8, 0};
W data_ov002_0210b770 = {(int)&StaffRoll::func_ov002_020f32e4, 0};
W data_ov002_0210b778 = {(int)&StaffRoll::func_ov002_020f5678, 0};
W data_ov002_0210b780 = {(int)&StaffRoll::func_ov002_020f569c, 0};
W data_ov002_0210b788 = {(int)&StaffRoll::func_ov002_020f57c0, 0};
W data_ov002_0210b790 = {(int)&StaffRoll::func_ov002_020f2d70, 0};
W data_ov002_0210b798 = {(int)&StaffRoll::func_ov002_020f335c, 0};
W data_ov002_0210b7a0 = {(int)&StaffRoll::func_ov002_020f340c, 0};
W data_ov002_0210b7a8 = {(int)&StaffRoll::func_ov002_020f5848, 0};
W data_ov002_0210b7b0 = {(int)&StaffRoll::func_ov002_020f34bc, 0};
W data_ov002_0210b7b8 = {(int)&StaffRoll::func_ov002_020f5b24, 0};
W data_ov002_0210b7c0 = {(int)&StaffRoll::func_ov002_020f5b7c, 0};
W data_ov002_0210b7c8 = {(int)&StaffRoll::func_ov002_020f5b88, 0};
W data_ov002_0210b7d0 = {(int)&StaffRoll::func_ov002_020f3740, 0};
W data_ov002_0210b7d8 = {(int)&StaffRoll::func_ov002_020f378c, 0};
W data_ov002_0210b7e0 = {(int)&StaffRoll::func_ov002_020f5e38, 0};
W data_ov002_0210b7e8 = {(int)&StaffRoll::func_ov002_020f5c88, 0};
W data_ov002_0210b7f0 = {(int)&StaffRoll::func_ov002_020f5eb8, 0};
W data_ov002_0210b7f8 = {(int)&StaffRoll::func_ov002_020f5d34, 0};
W data_ov002_0210b800 = {(int)&StaffRoll::func_ov002_020f3ae8, 0};
W data_ov002_0210b808 = {(int)&StaffRoll::func_ov002_020f5e38, 0};
W data_ov002_0210b810 = {(int)&StaffRoll::func_ov002_020f3d38, 0};
W data_ov002_0210b818 = {(int)&StaffRoll::func_ov002_020f5b84, 0};
W data_ov002_0210b820 = {(int)&StaffRoll::func_ov002_020f5b80, 0};
W data_ov002_0210b828 = {(int)&StaffRoll::func_ov002_020f5e38, 0};
W data_ov002_0210b830 = {(int)&StaffRoll::func_ov002_020f5e98, 0};
W data_ov002_0210b838 = {(int)&StaffRoll::func_ov002_020f5e18, 0};
W data_ov002_0210b840 = {(int)&StaffRoll::func_ov002_020f5e78, 0};
W data_ov002_0210b848 = {(int)&StaffRoll::func_ov002_020f562c, 0};
W data_ov002_0210b850 = {(int)&StaffRoll::func_ov002_020f5ed8, 0};
W data_ov002_0210b858 = {(int)&StaffRoll::func_ov002_020f5e58, 0};
W data_ov002_0210b860 = {(int)&StaffRoll::func_ov002_020f5e78, 0};
W data_ov002_0210b870 = {(int)&StaffRoll::func_ov002_020f2bf4, 0};
W data_ov002_0210b878 = {(int)&StaffRoll::func_ov002_020f2d0c, 0};
W data_ov002_0210b880 = {(int)&StaffRoll::func_ov002_020f5e78, 0};
W data_ov002_0210b890 = {(int)&StaffRoll::func_ov002_020f3de4, 0};
W data_ov002_0210b898 = {(int)&StaffRoll::func_ov002_020f5e98, 0};
W data_ov002_0210b8a0 = {(int)&StaffRoll::func_ov002_020f5e78, 0};
W data_ov002_0210b8a8 = {(int)&StaffRoll::func_ov002_020f5e38, 0};
W data_ov002_0210b8b0 = {(int)&StaffRoll::func_ov002_020f43cc, 0};
W data_ov002_0210b8b8 = {(int)&StaffRoll::func_ov002_020f5e58, 0};
W data_ov002_0210b8c0 = {(int)&daDemo_c::func_ov002_020f71c4, 0};
W data_ov002_0210b8c8 = {(int)&daDemo_c::func_ov002_020f26c4, 0};
W data_ov002_0210b8d0 = {(int)func_ov002_020f26d4, 0};
W data_ov002_0210b8d8 = {(int)&daDemo_c::func_ov002_020f63f8, 0};
W data_ov002_0210b8e0 = {(int)&daDemo_c::func_ov002_020f6448, 0};
W data_ov002_0210b8e8 = {(int)&daDemo_c::func_ov002_020f6a9c, 0};
W data_ov002_0210b8f0 = {(int)&daDemo_c::func_ov002_020f6ab8, 0};
W data_ov002_0210b8f8 = {(int)&daDemo_c::func_ov002_020f6ae4, 0};
W data_ov002_0210b900 = {(int)&daDemo_c::func_ov002_020f7038, 0};
W data_ov002_0210b908 = {(int)&daDemo_c::func_ov002_020f6b4c, 0};
W data_ov002_0210b910 = {(int)&daDemo_c::func_ov002_020f6bc0, 0};
W data_ov002_0210b918 = {(int)&daDemo_c::func_ov002_020f6c24, 0};
W data_ov002_0210b920 = {(int)&daDemo_c::func_ov002_020f6c34, 0};
W data_ov002_0210b928 = {(int)&daDemo_c::func_ov002_020f6c60, 0};
W data_ov002_0210b930 = {(int)&daDemo_c::func_ov002_020f6e48, 0};
W data_ov002_0210b938 = {(int)&daDemo_c::func_ov002_020f7020, 0};

DemoModelFile data_ov002_02110b50(0x49b);
DemoModelFile data_ov002_02110b98(0x3ef);
DemoModelFile data_ov002_02110c18(0x3f3);
DemoAnimFile data_ov002_02110bf8(0x3f4);
DemoAnimFile data_ov002_02110c10(0x3d6);
DemoAnimFile data_ov002_02110bd0(0x3da);
DemoAnimFile data_ov002_02110b28(0x3ea);
DemoAnimFile data_ov002_02110bf0(0x3e0);
DemoAnimFile data_ov002_02110b68(0x3e2);
DemoAnimFile data_ov002_02110ba0(0x3e4);
DemoAnimFile data_ov002_02110bb0(0x3dc);
DemoAnimFile data_ov002_02110b48(0x3de);
DemoAnimFile data_ov002_02110b30(0x3e6);
DemoAnimFile data_ov002_02110be8(0x3d8);
DemoAnimFile data_ov002_02110c08(0x3e8);
DemoAnimFile data_ov002_02110c00(0x3ec);
DemoAnimFile data_ov002_02110bd8(0x3ed);
DemoTexSeqFile data_ov002_02110b60(0x3d7);
DemoTexSeqFile data_ov002_02110be0(0x3d9);
DemoTexSeqFile data_ov002_02110b90(0x3db);
DemoTexSeqFile data_ov002_02110bc0(0x3dd);
DemoTexSeqFile data_ov002_02110b80(0x3df);
DemoTexSeqFile data_ov002_02110b88(0x3e1);
DemoTexSeqFile data_ov002_02110b38(0x3e3);
DemoTexSeqFile data_ov002_02110ba8(0x3e5);
DemoTexSeqFile data_ov002_02110b20(0x3e7);
DemoTexSeqFile data_ov002_02110b40(0x3e9);
DemoTexSeqFile data_ov002_02110b58(0x3eb);
DemoTexSeqFile data_ov002_02110bb8(0x3ee);
DemoTexSeqFile data_ov002_02110b10(0x3f5);
DemoTexSeqFile data_ov002_02110bc8(0x3f8);
DemoTexSeqFile data_ov002_02110b18(0x3fa);
DemoModelFile data_ov002_02110b70(0x461);
DemoModelFile data_ov002_02110b78(0x475);

PMF0 data_ov002_02110f9c[22] = {
    data_ov002_0210b718.pmf0, data_ov002_0210b690.pmf0, data_ov002_0210b6a0.pmf0,
    data_ov002_0210b850.pmf0, data_ov002_0210b7f0.pmf0, data_ov002_0210b898.pmf0,
    data_ov002_0210b688.pmf0, data_ov002_0210b698.pmf0, data_ov002_0210b830.pmf0,
    data_ov002_0210b880.pmf0, data_ov002_0210b8a0.pmf0, data_ov002_0210b840.pmf0,
    data_ov002_0210b860.pmf0, data_ov002_0210b8b8.pmf0, data_ov002_0210b660.pmf0,
    data_ov002_0210b858.pmf0, data_ov002_0210b670.pmf0, data_ov002_0210b7e0.pmf0,
    data_ov002_0210b828.pmf0, data_ov002_0210b8a8.pmf0, data_ov002_0210b808.pmf0,
    data_ov002_0210b838.pmf0};
PMF0 data_ov002_02110f34[13] = {
    data_ov002_0210b7f8.pmf0, data_ov002_0210b6b8.pmf0, data_ov002_0210b7e8.pmf0,
    data_ov002_0210b680.pmf0, data_ov002_0210b720.pmf0, data_ov002_0210b6c8.pmf0,
    data_ov002_0210b6d0.pmf0, data_ov002_0210b650.pmf0, data_ov002_0210b7c8.pmf0,
    data_ov002_0210b818.pmf0, data_ov002_0210b820.pmf0, data_ov002_0210b7c0.pmf0,
    data_ov002_0210b7b8.pmf0};
PMF data_ov002_02110e7c[4] = {data_ov002_0210b668.pmf, data_ov002_0210b7a8.pmf,
                              data_ov002_0210b848.pmf, data_ov002_0210b700.pmf};
PMF data_ov002_02110e64[3] = {data_ov002_0210b788.pmf, data_ov002_0210b780.pmf,
                              data_ov002_0210b778.pmf};
PMF data_ov002_02110eec[9] = {
    data_ov002_0210b758.pmf, data_ov002_0210b710.pmf, data_ov002_0210b748.pmf,
    data_ov002_0210b740.pmf, data_ov002_0210b738.pmf, data_ov002_0210b730.pmf,
    data_ov002_0210b8b0.pmf, data_ov002_0210b728.pmf, data_ov002_0210b890.pmf};
PMF data_ov002_02110e34[3] = {data_ov002_0210b810.pmf, data_ov002_0210b6d8.pmf,
                              data_ov002_0210b800.pmf};
Entry37 data_ov002_02110e4c[3] = {
    {data_ov002_0210b7d8.pmf37}, {data_ov002_0210b7d0.pmf37}, {data_ov002_0210b6c0.pmf37}};
PMF data_ov002_02110e9c[4] = {data_ov002_0210b6b0.pmf, data_ov002_0210b7b0.pmf,
                              data_ov002_0210b7a0.pmf, data_ov002_0210b798.pmf};
PMF data_ov002_02110e24[2] = {data_ov002_0210b770.pmf, data_ov002_0210b760.pmf};
Entry2dd4 data_ov002_02110ebc[6] = {{data_ov002_0210b790.pmf2dd4}, {data_ov002_0210b878.pmf2dd4},
                                    {data_ov002_0210b870.pmf2dd4}, {data_ov002_0210b6f8.pmf2dd4},
                                    {data_ov002_0210b6f0.pmf2dd4}, {data_ov002_0210b6e0.pmf2dd4}};
}
