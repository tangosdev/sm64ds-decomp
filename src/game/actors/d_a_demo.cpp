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
 * ROM evidence: _ZTS8daDemo_c at ov002:0x0210b940, _ZTI8daDemo_c at
 * 0x0210b94c (si_class over _ZTI8dActor_c at arm9:0x0208e390),
 * _ZTV8daDemo_c at 0x0210bd60. The nested model helpers read as
 * _ZTSN8daDemo_c10anmModel_cE / _ZTSN8daDemo_c13simpleModel_cE over the
 * shared _ZTIN8daDemo_c7param_cE tail base at 0x0210b888. The run is
 * 0x020f1f70..0x020f8858. The registry factory daDemo_c_classInit ends
 * at 0x020f8838; the two `_ZThn80_` thunks close the run.
 *
 * daDemo_c's out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1 at 0x020f1f70 and D0 at
 * 0x020f1f94, then a D2 the cartridge does not keep; anmModel_c and
 * simpleModel_c emit the same D1/D0/D2 shape. Source order is
 * ROM-ascending so emission order stays ROM-ascending; the compiler
 * places simpleModel_c's D0/D1 pair out of order on its own. The
 * `_ZThn80_` frame-controller-base thunks are not written out: mwccarm
 * emits them with the vtables, after this factory, at
 * ov002:0x020f8838 (D0) and 0x020f8848 (D1).
 *
 * The nested classes derive from the arm9 model classes, so the
 * vague-linkage typeinfo copies this object emits are
 * _ZTI14dExtAnmModel_c, _ZTI17dExtSimpleModel_c, _ZTI11dExtModel_c and
 * _ZTI15dExtFrameCtrl_c. Each is a canonical import of the arm9 record
 * (notes/model-rtti-names.md); this TU does not emit a homeless _ZTI.
 *
 * deslop leftovers:
 *   func_ov002_020f2aec/2bf4/335c/340c/39ec keep opt_common_subs off
 *   brackets. The brackets bind per-member only under defer_codegen off.
 *   func_ov002_020f27e8 and the OAM tail family return a callee's r0 by
 *   falling off the end; the callees are declared int so the passthrough
 *   is spelled.
 *   daDemo_c::RunCommand, the dispatcher RunDemoCommand (src/KuppaScript.cpp)
 *   calls, is a member: it takes the actor in r0 like any other.
 *   RunCommand's command table is a function-local static of member
 *   pointers, filled on first use behind its guard word; each handler is
 *   cast to the table's (unsigned char *, int, int) signature.
 *   func_ov002_020f26d4 is a member whose body is `return
 *   func_ov002_020f2630();`; under -interworking mwccarm emits that tail
 *   call as ldr ip, [pc]; bx ip; .word, the 12-byte shape in the ROM.
 *   The mStaffRoll heap block is daDemo_c::StaffRoll, a record recovered
 *   from its helpers (no RTTI names it). The step handlers are its
 *   members: the ten __sinit state tables hold &StaffRoll:: pointers, and
 *   the dispatchers call (this->*table[step])(i) on the credit and window
 *   entries. The {addr, 0} words in .data before the tables are the
 *   compiler's literals for those initializers. Indexed entry fields
 *   are the members. 43cc's speed clamp, 55b4's step and 5894's kira
 *   fields add the index in bytes to the field address.
 *   common.h must precede daDemo_c.h: dExtAnmModel_c.h's nested Matrix4x3
 *   otherwise wins and scalarizes Behavior's 12-word matrix copy.
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
#include "nitro/os/cache.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/g2.h"
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
#include "nitro/mi/memory.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

struct OamAttr;

#pragma defer_codegen off

/* The staff-roll block's helpers are free functions that take the block
   as char *; they read it through this record. */
typedef daDemo_c::StaffRoll StaffRoll;

/* shadow typedef 's16' */
typedef signed short s16;

/* shadow struct 'OamEnt' */
struct OamEnt {
    s16 x;        /* 0 */
    s16 y;        /* 2 */
    s16 ang;      /* 4 */
    s16 pad;      /* 6 */
    int enabled;  /* 8 */
};

/* shadow struct 'ParticleCallback' */
struct ParticleCallback;

/* pointer-to-member taking (cmd, a2, a3) — RunCommand's table */
typedef void (daDemo_c::*PMF3)(unsigned char*, int, int);

/* The StaffRoll's dispatch tables: a step handler takes the star, window or
   credit slot it advances; a cue handler takes nothing. */
typedef void (StaffRoll::*StaffRollStep)(int);
typedef void (StaffRoll::*StaffRollCue)();

/* shadow struct 'Ent' */
struct Ent
{
  int out0;
  int out4;
  char pad8[8];
  int vx;
  char pad14[0x1a];
  u16 angle;
  char pad30[0x1c];
};

/* shadow struct 'SharedFilePtr' */
struct SharedFilePtr;

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'Vector3' */

/* shadow typedef 's32' */
typedef int s32;

/* shadow typedef 'u8' */
typedef unsigned char u8;

/* shadow typedef 'u16' */
typedef unsigned short u16;

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow typedef 'Vector3' */

/* Lakitu's model, animation and texture-sequence file handles, owned and
 * constructed by d_a_c_jugem.cpp's static init. Same TU-local wrapper
 * spellings as the owning TU. */
struct JugemModelFile : SharedFilePtr {
    JugemModelFile(u32 fileID);
    ~JugemModelFile();
};

struct JugemAnimationFilePtr : SharedFilePtr {
    JugemAnimationFilePtr(u32 fileID);
    ~JugemAnimationFilePtr();
};

struct JugemTexSequenceFilePtr : SharedFilePtr {
    JugemTexSequenceFilePtr(u32 fileID);
    ~JugemTexSequenceFilePtr();
};

extern "C" {
extern unsigned char gSubScreenLayers;
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
extern struct OamAttr *data_ov002_0210b6a8[];

extern u32 d_a_demoNewHold(u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void *rot,
                           struct ParticleCallback *callback);
extern int data_ov002_02100140[];
extern void func_ov002_020f2790(char *c, int p1, int p2, int p3, short p4);
extern int func_ov002_020f2984(char *p);
extern int GetOwnerLanguage(void);
extern void OamRender10_d_a_demo(int a, struct OamAttr *attr, int x, int y, int e, int f, int g,
                                 int h, int i, int j);
extern struct OamAttr* data_ov002_0210b988[];
extern void func_0201ef50(unsigned char v);
extern void func_0201f108(void);
extern struct OamAttr* data_ov002_0210bba0[];
extern int func_ov002_020f5a94(void *c);
extern int* data_ov002_0210b97c[];
extern void func_ov002_020f2f18(char *c, int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern StaffRollStep data_ov002_02110e24[];
extern StaffRollStep data_ov002_02110e9c[];
extern void func_ov002_020f3828(int *c);
extern int data_ov002_021002a0[];
extern short atan2_hold_d_a_demo_1(int y, int x);
extern StaffRollStep data_ov002_02110e34[];
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
extern StaffRollStep data_ov002_02110eec[];
extern int data_ov002_02100190[];
extern int data_ov002_02100170[];
extern StaffRollStep data_ov002_02110e64[];
extern void func_ov002_020f39ec(char *c, int i);
extern StaffRollStep data_ov002_02110e7c[];
extern void func_ov002_020f5a6c(char* p);
extern void func_ov002_020f396c(char* p);
extern void func_ov002_020f30d4(char* p);
extern void func_ov002_020f2ea0(char* p);
extern void func_ov002_020f2a68(char* p);
extern void func_ov002_020f2958(char* c);
extern void func_ov002_020f37fc(char*);
extern void func_ov002_020f2f9c(void *, int);
extern void func_ov002_020f3954(char *);
/* local extern dropped: decl_common.h already declares LoadFont(u8) */
extern StaffRollCue data_ov002_02110f9c[];
extern void SetSubBg3Offset(int a, int b);
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void* p);
extern void GXS_LoadBG1Scr(const void *src, int offset, int count);
extern void GXS_LoadBG3Scr(const void *src, u32 offset, u32 count);
extern void GXS_LoadBG2Scr(const void *src, u32 offset, u32 count);
extern void func_ov002_020f5ad4(char* c);
extern void func_ov002_020f5f60(char *c);
extern int func_ov002_020f5fb8(void* c);
extern int func_ov002_020f5f8c(void* c);
extern void func_ov002_020f2eac(void *c);
extern void func_ov002_020f3978(char *c);
extern int func_ov002_020f27e8(char *c);
extern int func_ov002_020f2990(char *c);
extern void func_ov002_020f5fe4(char *c);
/* local extern dropped: decl_common.h declares func_ov002_020f6514(unsigned char*, void*, unsigned char) */
extern void func_02012694(unsigned int, void*);
/* local extern dropped: decl_common.h declares data_ov002_0210bc88 as 'int'; callers cast the address */
extern int* Vec3_LslInPlace(int *v, int sh);
extern u16 gKuppaScriptFrame;
extern char *gCamera;
extern int data_ov002_0210b958;
/* local extern dropped: decl_common.h declares data_0209b41c */
extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern void func_0201f138(void);
extern void func_0201ef38(void);
extern void Math_Function_0203b0fc(int* p, int target, int scale, int max);
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern void Vec3_MulScalar(VecFx32 *out, const VecFx32 *in, int scale);
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

/* local extern dropped: decl_common.h declares data_0209b41c */
extern int data_0208ee44;
extern int data_ov002_0210b970[3];
extern unsigned char gLocalPlayerId;
extern void *gPlayers[];
/* void *: every caller passes &mPosX and POD int[3] scratch; retyping that
   scratch as Vector3 renumbers this TU's @NNN literals. */
extern void Vec3_RotateYAndTranslate(void *out, void *in, short angle, void *src);
extern int data_ov002_0210b964[3];
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 data_0209b41c;
extern unsigned char gGameMode;
extern char data_ov002_0211094c;
extern JugemModelFile data_ov085_0213074c;
extern SharedFilePtr *data_ov002_0210b600[1];
extern SharedFilePtr *data_ov002_0210b604[1];
extern SharedFilePtr *data_ov002_0210b608[1];
extern SharedFilePtr *data_ov002_0210b60c[1];
extern SharedFilePtr *data_ov002_0210b610[1];
extern int data_ov002_0210b614;
extern JugemTexSequenceFilePtr data_ov085_0213073c;
extern JugemAnimationFilePtr data_ov085_02130744;
/* The file handles and their registration nodes are defined at the
   bottom of this file; only the 12-byte gap between the two out-of-order
   nodes stays here so the function-local Vector3 statics keep their retail
   addresses. */
int data_ov002_02110ddc[3];
}

daDemo_c::~daDemo_c()
{
}

/* the D0 deleting destructor this file is enrolled for emits from the
   ~daDemo_c definition at ordinal 0 below; objisolate keeps the
   variant this file's delinks entry names. */

/* Sub-screen fade step (param1 0x2f, states 2 and 3). While fadeLevel is
 * non-zero it counts fadeTimer to 0x18, then drops fadeLevel by one and calls
 * G2x_SetBlendAlpha_ on 0x4001050 with (4, 0x28, fadeLevel, 0x10 - fadeLevel).
 * When fadeLevel reaches zero it clears bit 2 of gSubScreenLayers, writes that
 * mask into bits 8..12 of 0x4001000, sets the blend to (0, 0x28, 0xc, 4),
 * advances the state and gives the three sprite slots delays 0x10/0x20/0x30. */

void daDemo_c::func_ov002_020f1fcc()
{
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
        G2S_SetBlendAlpha(4, 0x28, v, 0x10 - v);
        return;
    }
    gSubScreenLayers &= ~4;
    GXS_SetVisiblePlane(gSubScreenLayers);
    G2S_SetBlendAlpha(0, 0x28, 0xc, 4);
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

void daDemo_c::func_ov002_020f2210()
{
    int i;
    int base;
    u8 state;
    state = mScroll->state;
    /* The redundant `state >= 3` is compiled into the ROM: `state > 5` alone
       builds 8 bytes short. */
    if (state >= 3 && state > 5) return;
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

void daDemo_c::func_ov002_020f2340()
{
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[0]);
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[1]);
}

/* Scroll the two sub-screen BG layers in opposite directions (bg1 x down
 * one, bg0 x up one) and write both offsets to the hardware. */

void daDemo_c::func_ov002_020f237c()
{
    mScroll->bg1X -= 1;
    mScroll->bg0X += 1;
    SetSubBg0Offset(mScroll->bg0X, mScroll->bg0Y);
    SetSubBg1Offset(mScroll->bg1X, mScroll->bg1Y);
}

/* CleanupResources for param1 0x2f: free the scroll block. */

int daDemo_c::func_ov002_020f23d0()
{
    ::operator delete(this->mScroll);
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

int daDemo_c::func_ov002_020f23f0()
{
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
                GXS_SetVisiblePlane(gSubScreenLayers);
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

int daDemo_c::func_ov002_020f2630() {
    this->mScroll = (daDemo_c::ScrollState *)::operator new(0x36);
    this->mScroll->bg0X = 0x30;
    this->mScroll->bg0Y = 0x20;
    this->mScroll->bg1X = 0;
    this->mScroll->bg1Y = 0;
    this->mScroll->centerSpriteY[0] = 0xfff8;
    this->mScroll->centerSpriteY[1] = 0xc8;
    this->mScroll->fadeLevel = 0x10;
    SetSubBg0Offset(this->mScroll->bg0X, this->mScroll->bg0Y);
    SetSubBg1Offset(this->mScroll->bg1X, this->mScroll->bg1Y);
    return 1;
}

/* Script handler (table entry 23): mCue = p[0], the value 23f0 waits on. */

int daDemo_c::func_ov002_020f26c4(unsigned char *src)
{
    this->mCue = src[0];
    return 1;
}

/* func_ov002_020f26d4 @ 0x20f26d4 (ov002) -- tail call to func_ov002_020f2630 (0x20f2630),
 * emitted as ldr ip, [pc]; bx ip; .word 0x20f2630. Command 22 in RunCommand's table.
 */
int daDemo_c::func_ov002_020f26d4() {
    return func_ov002_020f2630();
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

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2790(char* c, int p1, int p2, int p3, s16 p4)
{
    ((StaffRoll *)c)->kira.unk308 = p3;
    ((StaffRoll *)c)->kira.unk30c = p4;
    ((StaffRoll *)c)->kiraParticle = Particle::System::New(
        ((StaffRoll *)c)->kiraParticle, kPtclEndingStarSparkle, p1 << 12, p2 << 12, 0, 0,
        (dPa_c::level_c::callback_c *)((struct ParticleCallback *)&((StaffRoll *)c)->kira));
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f27e8(char *c)
{
    if (((StaffRoll *)c)->burstActive == 0)
        return ((StaffRoll *)c)->burstActive;

    ((StaffRoll *)c)->burstTimer += 1;

    if (((StaffRoll *)c)->burstDefParam != 0x1000) {
        if (((StaffRoll *)c)->burstTimer ==
            data_ov002_02100140[((StaffRoll *)c)->burstPeriods & 3]) {
            ((StaffRoll *)c)->burstDefParam += 0x199;
            if (((StaffRoll *)c)->burstDefParam >= 0x1000)
                ((StaffRoll *)c)->burstDefParam = 0x1000;
        }
    }

    if (((StaffRoll *)c)->burstSysParam != 0x800) {
        if (((StaffRoll *)c)->burstPeriods >= 5) {
            if ((((StaffRoll *)c)->burstTimer & 3) == 3) {
                ((StaffRoll *)c)->burstSysParam += 0x199;
                if (((StaffRoll *)c)->burstSysParam >= 0x800)
                    ((StaffRoll *)c)->burstSysParam = 0x800;
            }
        }
    }

    func_ov002_020f2790(c, 0x80, 0x60, ((StaffRoll *)c)->burstDefParam,
                        ((StaffRoll *)c)->burstSysParam);
    func_ov002_020f2790(c, 0x80, 0x60, ((StaffRoll *)c)->burstDefParam,
                        ((StaffRoll *)c)->burstSysParam);

    if (((StaffRoll *)c)->burstTimer >= 0x1e) {
        ((StaffRoll *)c)->burstTimer = 0;
        ((StaffRoll *)c)->burstPeriods += 1;
    }
    if (((StaffRoll *)c)->burstPeriods < 0xa)
        return ((StaffRoll *)c)->burstPeriods;
    return func_ov002_020f2984(c);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2958(char *c)
{
    ((StaffRoll *)c)->burstActive = 1;
    ((StaffRoll *)c)->burstDefParam = 0x199;
    ((StaffRoll *)c)->burstSysParam = 0x199;
    ((StaffRoll *)c)->burstTimer = 0;
    ((StaffRoll *)c)->burstPeriods = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2984(char *p)
{
    ((StaffRoll *)p)->burstActive = 0;
    return (int)p;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2990(char* c)
{
    int x, y;
    int idx;

    if (((StaffRoll *)c)->endVisible == 0)
        return (int)c;

    x = ((StaffRoll *)c)->endX >> 0xc;
    y = ((StaffRoll *)c)->endY >> 0xc;
    idx = 0;
    if (GetOwnerLanguage() == 3) idx = 1;
    else if (GetOwnerLanguage() == 2 || GetOwnerLanguage() == 5) idx = 2;
    else if (GetOwnerLanguage() == 4) idx = 3;

    OAM::Render(0, data_ov002_0210b988[idx], x, y, -1, 1, 0x1000, 0x1000, 0, -1);
}
}

void StaffRoll::func_ov002_020f2a48() {
    /* 0x1d8/0x1d9 and the position: see func_ov002_020f2990. */
    endActive = 1;
    endVisible = 1;
    endX = 524288;
    endY = 393216;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a68(char *p)
{
    /* 0x1d8/0x1d9: see func_ov002_020f2990. */
    ((StaffRoll *)p)->endActive = 0;
    ((StaffRoll *)p)->endVisible = 0;
}
}

void StaffRoll::func_ov002_020f2a78(int i) {
    this->credit[i].active = 0;
}

void StaffRoll::func_ov002_020f2a88(int i) {
    this->credit[i].timer++;
    if (this->credit[i].timer < 0x39)
        return;
    this->credit[i].timer = 0;
    this->credit[i].step++;
}

void StaffRoll::func_ov002_020f2aec(int idx) {
    char *c = (char *)this;
    StaffRoll *s = (StaffRoll *)c;

    if (s->credit[idx].timer == 0) {
        gSubScreenLayers |= 1;
    }
    s->credit[idx].timer++;
    s->credit[idx].blendTimer++;

    if (s->credit[idx].blendTimer >= 2) {
        s->credit[idx].blendTimer = 0;
        s->credit[idx].blend++;
        unsigned char cnt = s->credit[idx].blend;
        G2S_SetBlendAlpha(1, 0x36, cnt, 0x10 - cnt);
    }

    if (s->credit[idx].timer < 30) {
        return;
    }
    s->credit[idx].timer = 0;
    s->credit[idx].step++;
    G2S_BlendNone();
}

void StaffRoll::func_ov002_020f2bf4(int idx) {
    char *c = (char *)this;
    StaffRoll *s = (StaffRoll *)c;

    s->credit[idx].timer++;
    s->credit[idx].blendTimer++;

    if (s->credit[idx].blendTimer >= 2) {
        s->credit[idx].blendTimer = 0;
        s->credit[idx].blend++;
        unsigned char cnt = s->credit[idx].blend;
        G2S_SetBlendAlpha(1, 0x36, 0x10 - cnt, cnt);
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

void StaffRoll::func_ov002_020f2d0c(int i) {
    this->credit[i].timer++;
    if (this->credit[i].timer < 0x39)
        return;
    this->credit[i].timer = 0;
    this->credit[i].step++;
}

void StaffRoll::func_ov002_020f2d70(int i) {
    this->credit[i].timer++;
    if (this->credit[i].timer < 0x1e)
        return;
    this->credit[i].timer = 0;
    this->credit[i].step++;
}

extern StaffRollStep data_ov002_02110ebc[];
void StaffRoll::func_ov002_020f2dd4() {
    if (this->credit[0].active == 0)
        return;
    (this->*data_ov002_02110ebc[this->credit[0].step])(0);
}

/* StartCredit at 0x020f2e30
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
void StaffRoll::StartCredit(int n) {
    if (n == 0) return;
    if (n == 0x15) return;
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

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2ea0(char *p)
{
    ((StaffRoll *)p)->credit[0].active = 0;
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2eac(void *c) {
    int i;
    char *p = (char *)c;
    for (i = 0; i < 5; i++) {
        if (((StaffRoll *)p)->dolls[0].visible != 0) {
            OAM::RenderSub(data_ov002_0210bba0[((StaffRoll *)p)->dolls[0].frame],
                           ((StaffRoll *)p)->dolls[0].x >> 12, ((StaffRoll *)p)->dolls[0].y >> 12,
                           -1, ((StaffRoll *)p)->dolls[0].priority);
        }
        p += 0x14;
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2f18(char* c, int a, int b)
{
    int i = 0;
    do {
        if (((StaffRoll *)c)->dolls[0].active != 0) {
            ((StaffRoll *)c)->dolls[0].x -= a;
            ((StaffRoll *)c)->dolls[0].y -= b;
        }
        i++;
        c += 0x14;
    } while (i < 5);
}
}

void StaffRoll::func_ov002_020f2f64() {
    int i;
    for (i = 0; i < 5; i++) {
        if (dolls[i].active != 0) {
            dolls[i].frame++;
        }
    }
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2f9c(void* self, int count)
{
    char* sl = (char*)self;
    int i;
    int idx = func_ov002_020f5a94(self) - 2;
    if (idx < 0) idx = 0;
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
            int* p;
            ((StaffRoll *)sl)->dolls[0].active = 1;
            ((StaffRoll *)sl)->dolls[0].visible = 1;
            p = data_ov002_0210b97c[idx];
            ((StaffRoll *)sl)->dolls[0].x = (p[i * 4 + 0] + 0xe0) << 12;
            ((StaffRoll *)sl)->dolls[0].y = (p[i * 4 + 1] + 0x66) << 12;
            ((StaffRoll *)sl)->dolls[0].priority = (u8)p[i * 4 + 2];
            ((StaffRoll *)sl)->dolls[0].frame = (u8)p[i * 4 + 3];
        }
        sl += 0x14;
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f30d4(char *p)
{
    int i;
    for (i = 0; i < 5; i++) {
        ((StaffRoll *)p)->dolls[0].active = 0;
        ((StaffRoll *)p)->dolls[0].visible = 0;
        p += 0x14;
    }
}
}

void StaffRoll::func_ov002_020f30f8(int i) {
    char *o = (char *)this;
    unsigned short t;
    int oldA, oldB;
    int cc, dd, ee, ff;

    t = this->window[i].delay;
    if (t != 0) {
        this->window[i].delay = t - 1;
        return;
    }

    this->window[i].left -= this->window[i].stepX;
    this->window[i].top -= this->window[i].stepY;
    this->window[i].right += this->window[i].stepX;
    this->window[i].bottom += this->window[i].stepY;

    oldA = this->window[i].bgX;
    oldB = this->window[i].bgY;
    this->window[i].bgX = oldA + this->window[i].stepX;
    this->window[i].bgY += this->window[i].stepY;

    if (this->window[i].bgX >= 0)
        this->window[i].bgX = 0;
    if (this->window[i].bgY >= 0)
        this->window[i].bgY = 0;
    if (this->window[i].left < 0)
        this->window[i].left = 0;
    if (this->window[i].top < 0)
        this->window[i].top = 0;
    if (this->window[i].right >= 0xff000)
        this->window[i].right = 0xff000;
    if (this->window[i].bottom >= 0xc0000)
        this->window[i].bottom = 0xc0000;
    if (this->window[i].bgX > 0)
        this->window[i].bgX = 0;
    if (this->window[i].bgY > 0)
        this->window[i].bgY = 0;

    cc = this->window[i].bgX - oldA;
    func_ov002_020f2f18(o, cc, this->window[i].bgY - oldB);

    ee = this->window[i].right >> 12;
    ff = this->window[i].bottom >> 12;
    cc = this->window[i].left >> 12;
    dd = this->window[i].top >> 12;
    G2S_SetWnd1Position(cc, dd, ee, ff);
    SetSubBg1Offset(this->window[i].bgX >> 12, this->window[i].bgY >> 12);
    SetSubBg2Offset(this->window[i].bgX >> 12, this->window[i].bgY >> 12);
}

void StaffRoll::func_ov002_020f32e4(int i) {
    this->window[i].delay = 0x10;
    this->window[i].stepX = 0x1800;
    this->window[i].stepY = 0x1800;
    this->window[i].step = 1;
}

void StaffRoll::func_ov002_020f3310(int idx) {
    unsigned char sel = this->window[idx].step;
    (this->*data_ov002_02110e24[sel])(idx);
}

void StaffRoll::func_ov002_020f335c(int i) {
    this->window[i].top += 0x1800;
    if (this->window[i].top >= 0x66000) {
        this->window[i].top = 0x66000;
        this->window[i].active = 0;
        this->window[i].step = 0;
    }
    int left = this->window[i].left >> 0xc;
    int right = this->window[i].right >> 0xc;
    int bottom = this->window[i].bottom >> 0xc;
    int top = this->window[i].top >> 0xc;
    G2S_SetWnd1Position(left, top, right, bottom);
}

void StaffRoll::func_ov002_020f340c(int i) {
    this->window[i].top += 0x1800;
    if (this->window[i].top >= 0x66000) {
        this->window[i].top = 0x66000;
        this->window[i].active = 0;
        this->window[i].step = 0;
    }
    int left = this->window[i].left >> 0xc;
    int right = this->window[i].right >> 0xc;
    int bottom = this->window[i].bottom >> 0xc;
    int top = this->window[i].top >> 0xc;
    G2S_SetWnd1Position(left, top, right, bottom);
}

void StaffRoll::func_ov002_020f34bc(int idx) {
    int flag;
    int a, b, c, d;
    int bs, cs, ds, as_;
    this->window[idx].left -= 0x1800;
    this->window[idx].top -= 0x1800;
    this->window[idx].right += 0x1800;
    this->window[idx].bottom += 0x1800;
    flag = 0;
    if (this->window[idx].left <= 0x80000)
        this->window[idx].left = 0x80000;
    if (this->window[idx].top <= 0x56000)
        this->window[idx].top = 0x56000;
    if (this->window[idx].right >= 0xfc000)
        this->window[idx].right = 0xfc000;
    if (this->window[idx].bottom >= 0xbc000) {
        this->window[idx].bottom = 0xbc000;
        flag++;
    }
    if (flag != 0)
        this->window[idx].step++;
    a = this->window[idx].top;
    b = this->window[idx].left;
    c = this->window[idx].right;
    d = this->window[idx].bottom;
    bs = b >> 12;
    cs = c >> 12;
    ds = d >> 12;
    as_ = a >> 12;
    G2S_SetWnd1Position(bs, as_, cs, ds);
}

void StaffRoll::func_ov002_020f35e4(int i) {
    this->window[i].left = 0xe0000;
    this->window[i].top = 0x66000;
    this->window[i].right = 0xe0000;
    this->window[i].bottom = 0x66000;
    GXS_SetVisibleWnd(GX_WNDMASK_W1);
    G2S_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                               GX_WND_PLANEMASK_OBJ,
                           true);
    G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_OBJ, true);
    {
        int a = this->window[i].left >> 12;
        int d = this->window[i].right >> 12;
        int e = this->window[i].bottom >> 12;
        int b = this->window[i].top >> 12;
        G2S_SetWnd1Position(a, b, d, e);
    }
    this->window[i].bgX = -0x60000;
    this->window[i].bgY = -0x38000;
    SetSubBg1Offset(this->window[i].bgX >> 12, this->window[i].bgY >> 12);
    SetSubBg2Offset(this->window[i].bgX >> 12, this->window[i].bgY >> 12);
    gSubScreenLayers |= 6;
    this->window[i].step = this->window[i].step + 1;
}

void StaffRoll::func_ov002_020f3740(int idx) {
    unsigned char sel = this->window[idx].step;
    (this->*data_ov002_02110e9c[sel])(idx);
}

void StaffRoll::func_ov002_020f378c(int idx) {
    this->window[idx].step = 0;
}

extern StaffRollStep data_ov002_02110e4c[];
void StaffRoll::func_ov002_020f37a0() {
    if (this->window[0].active == 0)
        return;
    (this->*data_ov002_02110e4c[this->window[0].kind])(0);
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f37fc(char *c)
{
    func_ov002_020f3828((int*)c);
    ((StaffRoll *)c)->window[0].active = 1;
    ((StaffRoll *)c)->window[0].kind = 2;
    ((StaffRoll *)c)->window[0].step = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3828(int *c)
{
    ((StaffRoll *)c)->window[0].left = 0x80000;
    ((StaffRoll *)c)->window[0].top = 0x66000;
    ((StaffRoll *)c)->window[0].right = 0xfc000;
    ((StaffRoll *)c)->window[0].bottom = 0xbc000;
    GXS_SetVisibleWnd(GX_WNDMASK_W1);
    G2S_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                               GX_WND_PLANEMASK_OBJ,
                           true);
    G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0, true);
    {
        int a = ((StaffRoll *)c)->window[0].left >> 12;
        int d = ((StaffRoll *)c)->window[0].right >> 12;
        int e = ((StaffRoll *)c)->window[0].bottom >> 12;
        int b = ((StaffRoll *)c)->window[0].top >> 12;
        G2S_SetWnd1Position(a, b, d, e);
    }
    ((StaffRoll *)c)->window[0].bgX = -0x60000;
    ((StaffRoll *)c)->window[0].bgY = -0x38000;
    SetSubBg1Offset(((StaffRoll *)c)->window[0].bgX >> 12, ((StaffRoll *)c)->window[0].bgY >> 12);
    SetSubBg2Offset(((StaffRoll *)c)->window[0].bgX >> 12, ((StaffRoll *)c)->window[0].bgY >> 12);
    gSubScreenLayers |= 6;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3954(char *p)
{
    ((StaffRoll *)p)->window[0].active = 1;
    ((StaffRoll *)p)->window[0].kind = 1;
    ((StaffRoll *)p)->window[0].step = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f396c(char *p)
{
    ((StaffRoll *)p)->window[0].active = 0;
}
}

extern "C" {
extern void *data_ov002_0210b998[];
void func_ov002_020f3978(char *c) {
    int i;
    for (i = 0; i < 4; i++) {
        if (((StaffRoll *)c)->stars[0].visible) {
            OAM::Render(true, (OamAttr *)data_ov002_0210b998[i],
                        ((StaffRoll *)c)->stars[0].x >> 0xc, ((StaffRoll *)c)->stars[0].y >> 0xc,
                        -1, 1, 0);
        }
        c += 0x4c;
    }
}
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f39ec(char *self, int idx) {
    StaffRoll *s = (StaffRoll *)self;
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
        MI_CpuCopy16(s->starChars + src,
                     (u8 *)G2S_GetOBJCharPtr() + 0x2000 + idx * 0x100 + i * 0x400, 0x100);
    }
}
}

void StaffRoll::func_ov002_020f3ae8(int idx) {
    char *base = (char *)this;
    StaffRoll::Star *arr = (StaffRoll::Star *)base;
    int a;
    int b;
    a = FX_SinCosTable_[((arr[idx].heading >> 4) * 2) + 1];
    b = arr[idx].radius;
    arr[idx].x = FX_MUL(a, b) + 0xe0000;
    a = FX_SinCosTable_[(arr[idx].heading >> 4) * 2];
    b = arr[idx].radius;
    arr[idx].y = FX_MUL(a, b) + 0x66000;
    if (arr[idx].radius > 0x18000) {
        arr[idx].radius = arr[idx].radius - 0x10;
    }
    arr[idx].heading += 0x200;
}

void StaffRoll::func_ov002_020f3ba0(int i) {
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2 + 1];
        int spd = this->stars[i].speed;
        this->stars[i].x += FX_MUL(tv, spd);
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2];
        int spd = this->stars[i].speed;
        this->stars[i].y += FX_MUL(tv, spd);
    }

    this->stars[i].speed += 0x30;

    dx = 0xe0 - (this->stars[i].x >> 12);
    dy = 0x46 - (this->stars[i].y >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        this->stars[i].step += 1;
        this->stars[i].delay = 0;
        this->stars[i].heading = 0xc000;
        this->stars[i].radius = 0x20000;
        this->stars[i].x = 0xe0000;
        this->stars[i].y = 0x46000;
        return;
    }

    target = cstd::atan2(dy, dx);
    ang = this->stars[i].heading;
    if (target > ang) {
        this->stars[i].heading += 0x80;
        if (target <= this->stars[i].heading)
            this->stars[i].heading = target;
    } else {
        if (ang <= target)
            return;
        this->stars[i].heading -= 0x80;
        if (this->stars[i].heading <= target)
            this->stars[i].heading = target;
    }
}

void StaffRoll::func_ov002_020f3d38(int i) {
    if (this->stars[i].delay != 0) {
        this->stars[i].delay = this->stars[i].delay - 1;
        return;
    }
    this->stars[i].x = -0x10000;
    this->stars[i].y = -0x10000;
    this->stars[i].speed = 0x2000;
    this->stars[i].heading = 0;
    this->stars[i].delay = 0;
    this->stars[i].step = this->stars[i].step + 1;
}

void StaffRoll::func_ov002_020f3d98(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110e34[sel])(idx);
}

/* Per-slot update of one 0x4c-byte cutscene record. func_ov002_020f5a94
 * takes c so r0 stays live through the orbit path to the call. */
void StaffRoll::func_ov002_020f3de4(int i) {
    char *c = (char *)this;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;
    int j;

    v = this->stars[i].delay;
    if (v != 0) {
        this->stars[i].delay = v - 1;
        return;
    }

    if (this->stars[i].phase == 0) {
        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].speed;
            this->stars[i].x += FX_MUL(tv, spd);
        }

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].speed;
            this->stars[i].y += FX_MUL(tv, spd);
        }

        if (this->stars[i].speed >= 0x2000)
            this->stars[i].speed -= 0x120;

        dx = (this->stars[i].targetX - this->stars[i].x) >> 12;
        dy = (this->stars[i].targetY - this->stars[i].y) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            this->stars[i].phase += 1;
            this->stars[i].orbitTravel = 0;
            this->stars[i].heading = 0xc000;
            return;
        }

        target = cstd::atan2(dy, dx);
        ang = this->stars[i].heading;
        if (target > ang) {
            this->stars[i].heading += 0x100;
            if (target <= this->stars[i].heading)
                this->stars[i].heading = target;
        } else {
            if (ang <= target)
                return;
            this->stars[i].heading -= 0x100;
            if (this->stars[i].heading <= target)
                this->stars[i].heading = target;
        }
        return;
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2 + 1];
        int spd = this->stars[i].radius;
        this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }
    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2];
        int spd = this->stars[i].radius;
        this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }
    this->stars[i].heading += this->stars[i].angSpeed;
    this->stars[i].orbitTravel += this->stars[i].angSpeed;

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)((StaffRoll *)c)->stars[0].orbitTravel < 0x10000)
        return;

    for (j = 0; j < 4; j++) {
        if (((StaffRoll *)c)->stars[0].active != 0) {
            ((StaffRoll *)c)->stars[0].step = 1;
            ((StaffRoll *)c)->stars[0].loops = 0;
            ((StaffRoll *)c)->stars[0].phase = 0;
            ((StaffRoll *)c)->stars[0].radius = 0x38000;
            ((StaffRoll *)c)->stars[0].heading = data_ov002_021000c0[j];
            ((StaffRoll *)c)->stars[0].orbitTravel = 0;
            ((StaffRoll *)c)->stars[0].angSpeed = 0x200;
        }
        c += 0x4c;
    }
}

void StaffRoll::func_ov002_020f40fc(int i) {
    char *c = (char *)this;

    if (this->stars[i].frame == 0) {
        if (this->stars[i].frameTimer == 0) {
            this->stars[i].loops += 1;
        }
    }

    if (this->stars[i].phase == 0) {
        int idx;
        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].radius;
            this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }
        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].radius;
            this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        this->stars[i].heading -= this->stars[i].angSpeed;
        this->stars[i].orbitTravel += this->stars[i].angSpeed;

        if (this->stars[0].phase != 0) {
            this->stars[i].angSpeed += 8;
        } else {
            if (this->stars[i].angSpeed < 0x200) {
                this->stars[i].angSpeed += 6;
                if (this->stars[i].angSpeed > 0x200)
                    this->stars[i].angSpeed = 0x200;
            }
        }

        if ((unsigned int)this->stars[i].orbitTravel >= (unsigned int)data_ov002_02100120[i] &&
            this->holdOrbit == 0) {
            this->stars[i].phase += 1;
            this->stars[i].flyTimer = 0;
            this->stars[i].speed = 0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if ((unsigned int)this->stars[i].orbitTravel < 0xc000)
            return;
        if (i != 2)
            return;
        if (this->holdOrbit != 0)
            return;
        this->stars[i].phase += 1;
        this->stars[i].flyTimer = 0;
        this->stars[i].speed = 0x2000;
        return;
    }

    this->stars[i].flyTimer += 1;
    this->stars[i].x += this->stars[i].speed;
    this->stars[i].speed += 0x300;
    if (this->stars[0].flyTimer < 0x70)
        return;
    {
        this->stars[i].step += 1;
        this->stars[i].loops = 0;
        this->stars[i].phase = 0;
        this->stars[i].radius = 0x38000;
        this->stars[i].heading = 0;
        this->stars[i].delay = data_ov002_02100130[i] << 6;
        this->stars[i].x = -0x10000;
        this->stars[i].y = 0;
        this->stars[i].angSpeed = 0x100;
        this->stars[i].speed = 0x5800;
        this->stars[i].targetX = 0x80000;
        this->stars[i].targetY = 0x28000;
    }
}

/* recovered: per-slot update of a 4-entry cursor/target table (0x4c-byte
 * records at c). Slot i first burns its hold timer (+0x30); while its arrival
 * flag (+0x48) is clear it steps position (+0/+4) along heading (+0x2e) at
 * speed (+8), decelerates at the -0x2000 floor, and either snaps onto the
 * target (+0x1c/+0x20) within 2 units (flag++, timer reset, heading 0xc000)
 * or turns the heading toward atan2(dy,dx) in 0x100 steps. Arrived slots
 * orbit at radius +0x10 around (0x80000,0x60000) with angular speed +0x42.
 * When the last slot (func_ov002_020f5a94(c)-1) has orbited past 0x10000,
 * every active row (+0x44) is re-armed: +0x47 bumped, +0x3c/+0x48/+0x24
 * cleared, radius 0x38000, heading from data_ov002_021000b8[j] (0 for j==2
 * when the mode call returns 3).
 *
 * Codegen notes: the function is void (the ROM overwrites r0 in place after
 * both calls); func_ov002_020f5a94 takes c (r0 stays live to the bl). The
 * dx/dy block needs TWO named bases: px = c + off defined before the clamp
 * (colours to sb) and s = c + idx * 0x4c at the block start (recomputes the
 * same product as off under its own value number, so it is the r2 base the
 * tx/ty loads and the +0x24 store share).
 */
void StaffRoll::func_ov002_020f43cc(int i) {
    char *c = (char *)this;
    int idx = i;
    char *px;
    char *s;
    int off = idx * 0x4c;
    {
        unsigned short v = this->stars[idx].delay;
        if (v != 0) {
            this->stars[idx].delay = v - 1;
            return;
        }
    }
    if (this->stars[idx].phase == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = this->stars[idx].heading >> 4;
        {
            tv = FX_SinCosTable_[idxa * 2 + 1];
            spd = this->stars[idx].speed;
            this->stars[idx].x += FX_MUL(tv, spd);
        }
        idxa = this->stars[idx].heading >> 4;
        {
            tv = FX_SinCosTable_[idxa * 2];
            spd = this->stars[idx].speed;
            this->stars[idx].y += FX_MUL(tv, spd);
        }
        px = c + off;
        if (*(int *)((char *)&this->stars[0].speed + off) <= -0x2000)
            *(int *)((char *)&this->stars[0].speed + off) =
                *(int *)((char *)&this->stars[0].speed + off) + 0x120;
        {
            s = c + idx * 0x4c;
            int dx = (((StaffRoll *)s)->stars[0].targetX - *(int *)px) >> 12;
            int dy = (((StaffRoll *)s)->stars[0].targetY - this->stars[idx].y) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                this->stars[idx].phase++;
                ((StaffRoll *)s)->stars[0].orbitTravel = 0;
                this->stars[idx].heading = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)cstd::atan2(dy, dx);
                unsigned short cur = this->stars[idx].heading;
                if (a > cur) {
                    unsigned short nv;
                    this->stars[idx].heading += 0x100;
                    nv = this->stars[idx].heading;
                    if (a <= nv)
                        this->stars[idx].heading = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    this->stars[idx].heading -= 0x100;
                    nv2 = this->stars[idx].heading;
                    if (nv2 <= a)
                        this->stars[idx].heading = a;
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
        idxa = this->stars[idx].heading >> 4;
        {
            tv = FX_SinCosTable_[idxa * 2 + 1];
            spd = this->stars[idx].radius;
            this->stars[idx].x = FX_MUL(tv, spd) + 0x80000;
        }
        idxa = this->stars[idx].heading >> 4;
        {
            tv = FX_SinCosTable_[idxa * 2];
            spd = this->stars[idx].radius;
            this->stars[idx].y = FX_MUL(tv, spd) + 0x60000;
        }
        this->stars[idx].heading -= this->stars[idx].angSpeed;
        this->stars[idx].orbitTravel += this->stars[idx].angSpeed;
    }
    if (idx != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000U)
        return;
    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (((StaffRoll *)p)->stars[0].active != 0) {
                (((StaffRoll *)p)->stars[0].step)++;
                ((StaffRoll *)p)->stars[0].loops = 0;
                ((StaffRoll *)p)->stars[0].phase = 0;
                ((StaffRoll *)p)->stars[0].radius = 0x38000;
                ((StaffRoll *)p)->stars[0].heading = data_ov002_021000b8[j];
                if (func_ov002_020f5a94(c) == 3) {
                    if (j == 2)
                        ((StaffRoll *)p)->stars[0].heading = 0;
                }
                ((StaffRoll *)p)->stars[0].orbitTravel = 0;
            }
            p += 0x4c;
        }
    }
}

void StaffRoll::func_ov002_020f4710(int i) {
    char *c = (char *)this;

    if (this->stars[i].phase == 0) {
        int idx;

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].radius;
            this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].radius;
            this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        this->stars[i].heading -= this->stars[i].angSpeed;
        this->stars[i].orbitTravel += this->stars[i].angSpeed;

        if (this->stars[0].phase != 0) {
            this->stars[i].angSpeed += 8;
        } else if (this->stars[i].angSpeed < 0x200) {
            this->stars[i].angSpeed += 6;
            if (this->stars[i].angSpeed > 0x200)
                this->stars[i].angSpeed = 0x200;
        }

        if ((u32)this->stars[i].orbitTravel < (u32)data_ov002_02100100[i])
            return;
        if (this->holdOrbit != 0)
            return;

        this->stars[i].phase++;
        this->stars[i].flyTimer = 0;

        if (i == 0 || i == 3) {
            this->stars[i].speed = 0x2000;
        } else {
            this->stars[i].speed = -0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if (i != 2)
            return;
        if (this->holdOrbit == 0)
            this->stars[i].speed = 0x2000;
        return;
    }

    this->stars[i].flyTimer++;
    this->stars[i].x += this->stars[i].speed;

    if (func_ov002_020f5a94(c) == 3 && i == 2) {
        this->stars[i].speed += 0x300;
    } else if (i == 0 || i == 3) {
        this->stars[i].speed += 0x300;
    } else {
        this->stars[i].speed -= 0x300;
    }

    if (this->stars[0].flyTimer < 0x60)
        return;

    {
        this->stars[i].step++;
        this->stars[i].loops = 0;
        this->stars[i].phase = 0;
        this->stars[i].radius = 0x38000;
        this->stars[i].heading = 0x8000;
        this->stars[i].delay = (u16)(data_ov002_02100110[i] << 6);

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                this->stars[i].delay = 0x40;
        }
    }

    this->stars[i].x = 0x110000;
    this->stars[i].y = 0;
    this->stars[i].angSpeed = 0x100;
    this->stars[i].speed = 0x5800;
    this->stars[i].targetX = 0x80000;
    this->stars[i].targetY = 0x28000;
}

void StaffRoll::func_ov002_020f4a2c(int i) {
    char *c = (char *)this;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = this->stars[i].delay;
    if (v != 0) {
        this->stars[i].delay = v - 1;
        return;
    }

    if (this->stars[i].phase == 0) {
        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].speed;
            this->stars[i].x += FX_MUL(tv, spd);
        }

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].speed;
            this->stars[i].y += FX_MUL(tv, spd);
        }

        if (this->stars[i].speed <= -0x2000)
            this->stars[i].speed = this->stars[i].speed + 0x120;

        dx = (this->stars[i].targetX - this->stars[i].x) >> 12;
        dy = (this->stars[i].targetY - this->stars[i].y) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            this->stars[i].phase += 1;
            this->stars[i].orbitTravel = 0;
            this->stars[i].heading = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = this->stars[i].heading;
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            this->stars[i].heading += 0x100;
            nv = this->stars[i].heading;
            if (target <= nv)
                this->stars[i].heading = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            this->stars[i].heading -= 0x100;
            nv2 = this->stars[i].heading;
            if (nv2 <= target)
                this->stars[i].heading = target;
            return;
        }
        return;
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2 + 1];
        int spd = this->stars[i].radius;
        this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2];
        int spd = this->stars[i].radius;
        this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }

    this->stars[i].heading -= this->stars[i].angSpeed;
    this->stars[i].orbitTravel += this->stars[i].angSpeed;

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (((StaffRoll *)p)->stars[0].active != 0) {
                ((StaffRoll *)p)->stars[0].step += 1;
                ((StaffRoll *)p)->stars[0].loops = 0;
                ((StaffRoll *)p)->stars[0].phase = 0;
                ((StaffRoll *)p)->stars[0].radius = 0x38000;
                ((StaffRoll *)p)->stars[0].heading = data_ov002_021000c8[j];
                if (func_ov002_020f5a94(c) == 3) {
                    if (j == 2)
                        ((StaffRoll *)p)->stars[0].heading = 0;
                }
                ((StaffRoll *)p)->stars[0].orbitTravel = 0;
            }
            p += 0x4c;
        }
    }
}

void StaffRoll::func_ov002_020f4d70(int i) {
    char *c = (char *)this;

    if (this->stars[i].frame == 0) {
        if (this->stars[i].frameTimer == 0)
            this->stars[i].loops++;
    }

    if (this->stars[i].phase == 0) {
        int idx;

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].radius;
            this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].radius;
            this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        this->stars[i].heading += this->stars[i].angSpeed;
        this->stars[i].orbitTravel += this->stars[i].angSpeed;

        if (this->stars[0].phase != 0) {
            this->stars[i].angSpeed += 8;
        } else {
            if (this->stars[i].angSpeed < 0x200) {
                this->stars[i].angSpeed += 6;
                if (this->stars[i].angSpeed > 0x200)
                    this->stars[i].angSpeed = 0x200;
            }
        }

        if ((u32)this->stars[i].orbitTravel < (u32)data_ov002_021000e0[i])
            return;

        if (this->holdOrbit != 0)
            return;

        this->stars[i].phase++;
        this->stars[i].flyTimer = 0;
        this->stars[i].speed = -0x2000;
        return;
    }

    this->stars[i].flyTimer++;
    this->stars[i].x += this->stars[i].speed;
    this->stars[i].speed -= 0x300;

    if (this->stars[0].flyTimer < 0x70)
        return;

    {
        this->stars[i].step++;
        this->stars[i].radius = 0x38000;
        this->stars[i].heading = 0x8000;
        this->stars[i].phase = 0;
        this->stars[i].loops = 0;
        this->stars[i].delay = data_ov002_021000f0[i] << 6;

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                this->stars[i].delay = 0x40;
        }
    }

    this->stars[i].x = 0x110000;
    this->stars[i].y = 0;
    this->stars[i].angSpeed = 0x100;
    this->stars[i].speed = 0x5800;
    this->stars[i].targetX = 0x80000;
    this->stars[i].targetY = 0x28000;
}

/* recovered: the third of the three per-slot cursor/target updaters in this
 * family (func_ov002_020f43cc, func_ov002_020f4a2c, this one), same 0x4c-byte
 * record layout: hold timer +0x30, arrival flag +0x48, position +0/+4,
 * heading +0x2e, approach speed +8, target +0x1c/+0x20, orbit radius +0x10,
 * angular speed +0x42, orbit angle +0x24.
 *
 * Differences from func_ov002_020f4a2c, each visible in the bytes: the speed
 * clamp works from the +0x2000 side (`cmp #0x2000 / subge #0x120`), the
 * arrived slots orbit with the heading ADDED to the angular speed, the
 * re-arm loop reads data_ov002_021000d0 and has no mode==3 special case.
 * With fewer live values the compiler needs only r4-r8/sb and keeps c in r6.
 */
void StaffRoll::func_ov002_020f5010(int i) {
    char *c = (char *)this;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = this->stars[i].delay;
    if (v != 0) {
        this->stars[i].delay = v - 1;
        return;
    }

    if (this->stars[i].phase == 0) {
        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].speed;
            this->stars[i].x += FX_MUL(tv, spd);
        }

        idx = this->stars[i].heading >> 4;
        {
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].speed;
            this->stars[i].y += FX_MUL(tv, spd);
        }

        if (this->stars[i].speed >= 0x2000)
            this->stars[i].speed = this->stars[i].speed - 0x120;

        dx = (this->stars[i].targetX - this->stars[i].x) >> 12;
        dy = (this->stars[i].targetY - this->stars[i].y) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            this->stars[i].phase += 1;
            this->stars[i].orbitTravel = 0;
            this->stars[i].heading = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = this->stars[i].heading;
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            this->stars[i].heading += 0x100;
            nv = this->stars[i].heading;
            if (target <= nv)
                this->stars[i].heading = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            this->stars[i].heading -= 0x100;
            nv2 = this->stars[i].heading;
            if (nv2 <= target)
                this->stars[i].heading = target;
            return;
        }
        return;
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2 + 1];
        int spd = this->stars[i].radius;
        this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
    }

    idx = this->stars[i].heading >> 4;
    {
        short tv = FX_SinCosTable_[idx * 2];
        int spd = this->stars[i].radius;
        this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
    }

    this->stars[i].heading += this->stars[i].angSpeed;
    this->stars[i].orbitTravel += this->stars[i].angSpeed;

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (((StaffRoll *)p)->stars[0].active != 0) {
                ((StaffRoll *)p)->stars[0].step += 1;
                ((StaffRoll *)p)->stars[0].loops = 0;
                ((StaffRoll *)p)->stars[0].phase = 0;
                ((StaffRoll *)p)->stars[0].radius = 0x38000;
                ((StaffRoll *)p)->stars[0].heading = data_ov002_021000d0[j];
                ((StaffRoll *)p)->stars[0].orbitTravel = 0;
            }
            p += 0x4c;
        }
    }
}

void StaffRoll::func_ov002_020f5328(int i) {

    if (this->stars[i].phase == 0) {
        int idx;
        {
            idx = this->stars[i].heading >> 4;
            short tv = FX_SinCosTable_[idx * 2 + 1];
            int spd = this->stars[i].radius;
            this->stars[i].x = FX_MUL(tv, spd) + 0x80000;
        }
        {
            idx = this->stars[i].heading >> 4;
            short tv = FX_SinCosTable_[idx * 2];
            int spd = this->stars[i].radius;
            this->stars[i].y = FX_MUL(tv, spd) + 0x60000;
        }

        this->stars[i].heading += this->stars[i].angSpeed;
        this->stars[i].orbitTravel += this->stars[i].angSpeed;

        if (this->stars[0].phase != 0) {
            this->stars[i].angSpeed += 8;
        } else if (this->stars[i].angSpeed < 0x200) {
            this->stars[i].angSpeed += 6;
            if (this->stars[i].angSpeed > 0x200)
                this->stars[i].angSpeed = 0x200;
        }

        if ((unsigned int)this->stars[i].orbitTravel < (unsigned int)data_ov002_02100150[i])
            return;
        if (this->holdOrbit != 0)
            return;

        this->stars[i].phase += 1;
        this->stars[i].flyTimer = 0;

        if (i == 1 || i == 3) {
            this->stars[i].speed = 0x2000;
        } else {
            this->stars[i].speed = -0x2000;
        }
        return;
    }

    this->stars[i].flyTimer += 1;
    this->stars[i].x += this->stars[i].speed;
    if (i == 1 || i == 3) {
        this->stars[i].speed += 0x300;
    } else {
        this->stars[i].speed -= 0x300;
    }

    if (this->stars[0].flyTimer < 0x60)
        return;

    {
        this->stars[i].step += 1;
        this->stars[i].loops = 0;
        this->stars[i].phase = 0;
        this->stars[i].radius = 0x38000;
        this->stars[i].heading = 0;
        this->stars[i].delay = (unsigned short)(data_ov002_02100160[i] << 6);
        this->stars[i].x = -0x10000;
        this->stars[i].y = 0;
        this->stars[i].angSpeed = 0x100;
        this->stars[i].speed = 0x5800;
        this->stars[i].targetX = 0x80000;
        this->stars[i].targetY = 0x28000;
    }
}

void StaffRoll::func_ov002_020f55b4(int i) {
    char *c = (char *)this;
    char *e = c + i * 0x4c;
    if (((StaffRoll *)e)->stars[0].frame != 0x10)
        return;
    (*((unsigned char *)&this->stars[0].step + i * 0x4c)) += 1;
    ((StaffRoll *)e)->stars[0].radius = 0x38000;
    ((StaffRoll *)e)->stars[0].heading = data_ov002_021000d8[i];
    ((StaffRoll *)e)->stars[0].phase = 0;
    ((StaffRoll *)e)->stars[0].loops = 0;
    ((StaffRoll *)e)->stars[0].orbitTravel = 0;
    ((StaffRoll *)e)->stars[0].angSpeed = 0x200;
    ((StaffRoll *)e)->stars[0].kiraDefParam = 0x1000;
    ((StaffRoll *)e)->stars[0].kiraSysParam = 0x200;
}

void StaffRoll::func_ov002_020f562c(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110eec[sel])(idx);
}

void StaffRoll::func_ov002_020f5678(int i) {
    char *base = (char *)this;
    StaffRoll::Star *e = (StaffRoll::Star *)base + i;
    e->kiraDefParam = 0xccc;
    e->kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f569c(int i) {
    int ax;
    int by;
    int tx;
    int ty;

    if (this->stars[i].delay != 0) {
        this->stars[i].delay = this->stars[i].delay - 1;
        return;
    }

    this->stars[i].kiraDefParam = 0x1000;
    this->stars[i].kiraSysParam = 0x200;

    this->stars[i].x = this->stars[i].x + this->stars[i].speed;
    this->stars[i].y = this->stars[i].y + this->stars[i].speedY;

    tx = data_ov002_02100190[i * 2];
    ty = data_ov002_02100190[i * 2 + 1];
    ax = this->stars[i].x >> 12;
    by = this->stars[i].y >> 12;

    if (this->stars[i].speed > 0) {
        if (ax > tx)
            this->stars[i].x = tx << 12;
    } else {
        if (ax < tx)
            this->stars[i].x = tx << 12;
    }

    if (this->stars[i].speedY > 0) {
        if (by > ty)
            this->stars[i].y = ty << 12;
    } else {
        if (by < ty)
            this->stars[i].y = ty << 12;
    }

    ax = this->stars[i].x >> 12;
    by = this->stars[i].y >> 12;

    if (ax == tx && by == ty)
    {
        this->stars[i].step = this->stars[i].step + 1;
    }
}

void StaffRoll::func_ov002_020f57c0(int idx) {
    StaffRoll::Star *arr = this->stars;
    if (arr[idx].frame != 0)
        return;
    if (arr[idx].frameTimer != 0)
        return;
    arr[idx].sparkling = 1;
    arr[idx].visible = 1;
    arr[idx].step++;
    arr[idx].speed = data_ov002_02100170[idx * 2];
    arr[idx].speedY = data_ov002_02100170[idx * 2 + 1];
    if (idx == 0)
        arr[idx].delay = 0x78;
    else
        arr[idx].delay = 0x20;
}

void StaffRoll::func_ov002_020f5848(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110e64[sel])(idx);
}

void StaffRoll::func_ov002_020f5894(int i) {
    int off;
    unsigned short *p34;
    short s2c;
    if (i != 0) return;
    off = i * 0x4c;
    p34 = (unsigned short *)((char *)&this->stars[0].kiraTimer + off);
    *p34 = *p34 + 1;
    if (*(int *)((char *)&this->stars[0].kiraDefParam + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int *)((char *)&this->stars[0].kiraDefParam + off) =
                *(int *)((char *)&this->stars[0].kiraDefParam + off) + 0x199;
            if (*(int *)((char *)&this->stars[0].kiraDefParam + off) >= 0x1000)
                *(int *)((char *)&this->stars[0].kiraDefParam + off) = 0x1000;
        }
    }
    s2c = *(short *)((char *)&this->stars[0].kiraSysParam + off);
    if (s2c != 0x800) {
        if (((StaffRoll *)((char *)this + off))->stars[0].kiraPeriods >= 5) {
            if ((*p34 & 3) == 3) {
                *(short *)((char *)&this->stars[0].kiraSysParam + off) = s2c + 0x199;
                if (*(short *)((char *)&this->stars[0].kiraSysParam + off) >= 0x800)
                    *(short *)((char *)&this->stars[0].kiraSysParam + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e) return;
    *p34 = 0;
    {
        unsigned short *p3e = (unsigned short *)((char *)&this->stars[0].kiraPeriods + off);
        *p3e = *p3e + 1;
    }
}

void StaffRoll::func_ov002_020f5990() {
    char *c = (char *)this;
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
        (this->*data_ov002_02110e7c[s->state])(i);
        func_ov002_020f39ec(c, i);
        if (s->sparkling == 0)
            continue;
        if (i != this->kiraStar)
            continue;
        func_ov002_020f2790(c, (s16)(s->x >> 12), (s16)(s->y >> 12), s->kiraDefParam,
                            s->kiraSysParam);
    }
}

extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5a6c(char *p) {
    int i;
    for (i = 0; i < 4; i++) {
        ((StaffRoll *)p)->stars[0].active = 0;
        ((StaffRoll *)p)->stars[0].visible = 0;
        ((StaffRoll *)p)->stars[0].holdFrame = 0;
        p += 0x4c;
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f5a94(void *unused){
    unsigned char count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (SaveData::IsCharacterUnlocked(i))
            count = count + 1;
    }
    return count;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5ad4(char* c)
{
    /* StaffRoll::pad_1f0: two words cleared here and read nowhere. */
    *(int*)(c+0x1f0) = 0;
    *(int*)(c+0x1f4) = 0;
    ((StaffRoll *)c)->kiraStar = 0;
    ((StaffRoll *)c)->holdOrbit = 0;
    func_ov002_020f5a6c(c);
    func_ov002_020f396c(c);
    func_ov002_020f30d4(c);
    func_ov002_020f2ea0(c);
    func_ov002_020f2a68(c);
    func_ov002_020f2984(c);
}
}

void StaffRoll::func_ov002_020f5b24() {
    gSubScreenLayers &= ~6;
    gSubScreenLayers |= 8;
    func_ov002_020f2f64();
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    this->window[0].active = 0;
    func_ov002_020f2a48();
}

void StaffRoll::func_ov002_020f5b7c() {}

void StaffRoll::func_ov002_020f5b80() {}

void StaffRoll::func_ov002_020f5b84() {}

void StaffRoll::func_ov002_020f5b88() {}

void StaffRoll::func_ov002_020f5b8c() {
    this->holdOrbit = 1;
}

void StaffRoll::func_ov002_020f5b98() {
    StaffRoll::Star *e = this->stars;
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

void StaffRoll::func_ov002_020f5bcc() {
    char *p = (char *)this;
    int i;
    for (i = 0; i < 4; i++) {
        if (((StaffRoll *)p)->stars[0].active)
            ((StaffRoll *)p)->stars[0].holdFrame = 1;
        p += 0x4c;
    }
}

void StaffRoll::func_ov002_020f5bf4() {
    if (this->stars[3].active == 0)
        return;
    this->stars[3].x = 0x80000;
    this->stars[3].y = 0x60000;
    this->stars[3].delay = 0;
    this->stars[3].state = 1;
    this->stars[3].kiraDefParam = 0xccc;
    this->stars[3].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5c40() {
    if (!this->stars[2].active)
        return;
    this->stars[2].x = 0x80000;
    this->stars[2].y = 0x60000;
    this->stars[2].delay = 0;
    this->stars[2].state = 1;
    this->stars[2].kiraDefParam = 0xccc;
    this->stars[2].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5c88() {
    if (this->stars[1].active == 0)
        return;
    this->stars[1].x = 0x80000;
    this->stars[1].y = 0x60000;
    this->stars[1].delay = 0;
    this->stars[1].state = 1;
    this->stars[1].kiraDefParam = 0xccc;
    this->stars[1].kiraSysParam = 0x155;
}

void StaffRoll::func_ov002_020f5cd0() {
    StaffRoll::Star *c = this->stars;
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

void StaffRoll::func_ov002_020f5d34() {
    u8 *self = (u8 *)this;
    int n = func_ov002_020f5a94(self);
    int i = 0;
    u8 *e;
    int v;
    if (n <= 0) goto done;
    e = self;
    v = 0;
    do {
        /* +0x38 is Star::pad_38: written twice here, read nowhere. */
        ((StaffRoll *)e)->stars[0].active = 1;
        ((StaffRoll *)e)->stars[0].visible = 0;
        ((StaffRoll *)e)->stars[0].heading = 0;
        ((StaffRoll *)e)->stars[0].pad_38 = 0;
        ((StaffRoll *)e)->stars[0].x = 0x80000;
        ((StaffRoll *)e)->stars[0].y = 0x60000;
        ((StaffRoll *)e)->stars[0].delay = v;
        ((StaffRoll *)e)->stars[0].frameTimer = 0;
        ((StaffRoll *)e)->stars[0].kiraTimer = 0;
        ((StaffRoll *)e)->stars[0].kiraPeriods = 0;
        ((StaffRoll *)e)->stars[0].kiraDefParam = 0x199;
        ((StaffRoll *)e)->stars[0].kiraSysParam = 0x199;
        ((StaffRoll *)e)->stars[0].pad_38 = 0;
        ((StaffRoll *)e)->stars[0].state = 0;
        ((StaffRoll *)e)->stars[0].step = 0;
        ((StaffRoll *)e)->stars[0].sparkling = 0;
        e += 0x4c;
        v += 0x40;
        i++;
    } while (i < n);
done:
    func_ov002_020f2958((char*)self);
}

extern StaffRollCue data_ov002_02110f34[];
void StaffRoll::func_ov002_020f5dd8(int idx) {
    (this->*data_ov002_02110f34[idx])();
}

void StaffRoll::func_ov002_020f5e18() {
    func_ov002_020f37fc((char *)this);
    func_ov002_020f2f9c(this, 4);
}

void StaffRoll::func_ov002_020f5e38() {
    func_ov002_020f3828((int *)this);
    func_ov002_020f2f9c(this, 4);
}

void StaffRoll::func_ov002_020f5e58() {
    func_ov002_020f3828((int *)this);
    func_ov002_020f2f9c(this, 3);
}

void StaffRoll::func_ov002_020f5e78() {
    func_ov002_020f3828((int *)this);
    func_ov002_020f2f9c(this, 2);
}

void StaffRoll::func_ov002_020f5e98() {
    func_ov002_020f3828((int *)this);
    func_ov002_020f2f9c(this, 1);
}

void StaffRoll::func_ov002_020f5eb8() {
    func_ov002_020f3828((int *)this);
    func_ov002_020f2f9c(this, 0);
}

void StaffRoll::func_ov002_020f5ed8() {
    func_ov002_020f3828((int *)this);
}

void StaffRoll::func_ov002_020f5ee4() {
    func_ov002_020f3828((int *)this);
}

void StaffRoll::func_ov002_020f5ef0() {
    char *p = (char *)this;
    func_ov002_020f3954(p);
}

/* func_ov002_020f5efc @ 0x20f5efc (ov002) -- tail-call veneer to LoadFont (0x201fcd4) with r0=0.
 */
void StaffRoll::func_ov002_020f5efc() {
    LoadFont(0);
}

void StaffRoll::func_ov002_020f5f0c(int idx) {
    (this->*data_ov002_02110f9c[idx])();
    this->StartCredit(idx);
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5f60(char *c)
{
    void *p = ((StaffRoll *)c)->starChars;
    if (p == 0)
        return;
    ::operator delete(p);
    ((StaffRoll *)c)->starChars = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f5f8c(void* c){
    ((StaffRoll *)c)->func_ov002_020f26e0();
    func_ov002_020f3978((char *)c);
    func_ov002_020f2eac(c);
    return func_ov002_020f2990((char *)c);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f5fb8(void* c){
    ((StaffRoll *)c)->func_ov002_020f5990();
    ((StaffRoll *)c)->func_ov002_020f37a0();
    ((StaffRoll *)c)->func_ov002_020f2dd4();
    return func_ov002_020f27e8((char *)c);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5fe4(char* c)
{
    void *f;
    void *r5;

    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    gSubScreenLayers &= ~0x1f;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCG_BIN);
    DecompressLZ16(f, G2_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCL_BIN);
    GX_LoadOBJPltt(f, 0, 0x200);
    Deallocate(f);

    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                         GX_BG_CHARBASE_0x00000);
    G2S_SetBG0Priority(0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x7000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_SetBG1Priority(2);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x4000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCG_BIN);
    DecompressLZ16(f, (void *)G2S_GetBG1CharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCL_BIN);
    GX_LoadBGPltt(f, 0, 2);
    GXS_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK1_NSC_BIN);
    GXS_LoadBG1Scr(f, 0, 0x800);
    Deallocate(f);

    G2S_SetBG3Priority(2);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x6000,
                          GX_BG_CHARBASE_0x10000);

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

    DecompressLZ16(r5, (void *)G2S_GetBG3CharPtr());
    Deallocate(r5);

    r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCL_BIN);
    DC_FlushRange(r5, 0x200);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_0123_H);
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(r5, 0x4000, 0x200);
    GXS_LoadBGExtPltt(r5, 0x6000, 0x200);
    GXS_EndLoadBGExtPltt();
    Deallocate(r5);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_BRIGHT_NSC_BIN);
    GXS_LoadBG3Scr(f, 0, 0x800);
    Deallocate(f);

    G2S_SetBG2Priority(2);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_256, GX_BG_SCRBASE_0x5000,
                          GX_BG_CHARBASE_0x10000);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK2_NSC_BIN);
    GXS_LoadBG2Scr(f, 0, 0x800);
    Deallocate(f);

    gSubScreenLayers |= 0x10;

    {
        void *f6 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCG_BIN);
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCL_BIN);
        DecompressLZ16(f6, G2S_GetOBJCharPtr());
        GXS_LoadOBJPltt(r5, 0, 0x200);
        Deallocate(f6);
        Deallocate(r5);
    }

    ((StaffRoll *)c)->starChars = 0;
    if (((StaffRoll *)c)->starChars == 0) {
        ((StaffRoll *)c)->starChars = (u8 *)::operator new[](0x4000);
    }
    f = LoadFile(ASSET_D_2D_ED_STAR_NCG_BIN);
    DecompressLZ16(f, ((StaffRoll *)c)->starChars);
    Deallocate(f);

    func_ov002_020f5ad4(c);
}
}


int daDemo_c::func_ov002_020f63a0()
{
    func_ov002_020f5f60((char *)this->mStaffRoll);
    if (this->mStaffRoll != 0) {
        ::operator delete(this->mStaffRoll);
        this->mStaffRoll = 0;
    }
    return 1;
}


int daDemo_c::func_ov002_020f63d4()
{
    func_ov002_020f5fb8(this->mStaffRoll);
    func_ov002_020f5f8c(this->mStaffRoll);
    return 1;
}


int daDemo_c::func_ov002_020f63f8(unsigned char *src)
{
    this->mCue = *src;
    this->mStaffRoll->func_ov002_020f5dd8((int)this->mCue);
    return 1;
}


int daDemo_c::func_ov002_020f6424()
{
    this->mStaffRoll->func_ov002_020f5f0c((int)this->mCue);
    return 1;
}


int daDemo_c::func_ov002_020f6448(unsigned char *arg1)
{
    StaffRoll *p;
    this->mCue = *arg1;
    p = new StaffRoll;
    this->mStaffRoll = p;
    if (this->mStaffRoll == 0)
        return 0;
    func_ov002_020f5fe4((char *)this->mStaffRoll);
    return func_ov002_020f6424();
}


void daDemo_c::anmModel_c::func_ov002_020f64ac(char *r4)
{
    Advance();
    if (!Finished())
        return;
    signed char r2 = *(signed char *)(r4 + (mAnimEntry << 2) + 3);
    if (r2 < 0)
        return;
    func_ov002_020f6514((u8 *)r4, (u8)(r2 & 0xff));
}


void daDemo_c::anmModel_c::func_ov002_020f6514(void *tbl, unsigned char arg)
{
    u8 *e;
    int v;
    mAnimEntry = arg;
    e = (u8 *)tbl + mAnimEntry * 4;
    v = (e[2] == 0) ? 0x40000000 : 0;
    ((dExtAnmModel_c *)this)->SetAnim((BCA_File *)(mAnimFiles[e[0]]->filePtr), v, 0x1000, 0);
    {
        dExtAnmTexPat_c *ts = mTexSeq;
        s8 idx;
        if (ts == 0) return;
        idx = (s8)e[1];
        if (idx < 0) return;
        ts->SetFile(*(BTP_File *)mTexSeqFiles[idx]->filePtr, v, 0x1000, 0);
    }
}


void daDemo_c::anmModel_c::func_ov002_020f65b8()
{
    if (mTexSeq == 0)
        return;
    mTexSeq->Update(data);
}


void daDemo_c::anmModel_c::func_ov002_020f65ec() {
    if (mTexSeq != 0)
        mTexSeq->Advance();
}

int daDemo_c::anmModel_c::func_ov002_020f6618(SharedFilePtr *mdl, int nAnims, SharedFilePtr **anims,
                        int arg5, unsigned char texByte, SharedFilePtr **texs, int tsData)
{
    char *self = (char *)this;
    int i;
    mModelFile = mdl;
    if (((dExtModel_c *)self)->SetFile(dExtSimpleModel_c::LoadFile(*mModelFile), 1, tsData) == 0)
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
            BTP_File *btp = dExtAnmTexPat_c::LoadFile(*mTexSeqFiles[i]);
            dExtAnmTexPat_c::Prepare(*(BMD_File *)bmd, *btp);
        }
        mTexSeq->SetFile(*(BTP_File *)mTexSeqFiles[0]->filePtr, 0, 0x1000, 0);
    }
    mSnapToFloor = (u8)arg5;
    return 1;
}

/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names, ending in a tail call to operator delete.
 * The D0 variant here emits from the ~anmModel_c definition at ordinal 113. */

/* D1, the complete-object destructor.
 *
 * The shape here is the whole finding. The cartridge destroys the dExtAnmModel_c /
 * dExtSimpleModel_c BASE before the Vector3 array at the class's own tail -- a normal
 * C++ destructor destroys members first and never reaches it. param_c is a
 * real base declared FIRST, so bases destroy in reverse declaration order:
 * the model base runs before the Vector3 array, and param_c lands at the
 * object tail (0x64/0x50), which is exactly what the vmi typeinfo records
 * encode.
 *
 * The base holds only the array. The class's own members come after both
 * bases (0x70..0x83), so declaring them leaves the array at 0x64; deleting
 * mTexSeq is the dExtAnmTexPat_c's own virtual D0, which is the slot-1 call
 * the ROM makes. */
daDemo_c::anmModel_c::~anmModel_c()
{
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


int daDemo_c::simpleModel_c::func_ov002_020f6960(SharedFilePtr *fp, int n)
{
    mFile = fp;
    BMD_File *f = dExtSimpleModel_c::LoadFile(*mFile);
    return ((dExtModel_c *)this)->SetFile(f, 1, n) != 0 ? 1 : 0;
}

/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names, ending in a tail call to operator delete.
 * The D0 variant here emits from the ~simpleModel_c definition at ordinal 116. */

/* D1, the complete-object destructor.
 *
 * The shape here is the whole finding. The cartridge destroys the dExtAnmModel_c /
 * dExtSimpleModel_c BASE before the Vector3 array at the class's own tail -- a normal
 * C++ destructor destroys members first and never reaches it. param_c is a
 * real base declared FIRST, so bases destroy in reverse declaration order:
 * the model base runs before the Vector3 array, and param_c lands at the
 * object tail (0x64/0x50), which is exactly what the vmi typeinfo records
 * encode.
 *
 * The base holds only the array; mFile is the class's own member and lands
 * after both bases, at 0x5c. */
/* defer_codegen on for this one definition: it is the second MI destructor
 * defined in the TU, and under defer off the second family emits D1 before
 * D0, which is not the cartridge's order. Deferred, its D2/D0/D1 flush at
 * the next definition and land D0 below D1. */
#pragma defer_codegen on
daDemo_c::simpleModel_c::~simpleModel_c()
{
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


int daDemo_c::func_ov002_020f6a9c()
{
    MarkForDestruction();
    return 1;
}


int daDemo_c::func_ov002_020f6ab8(unsigned char *p)
{
    unsigned int id = ReadUnalignedInt(p);
    Sound::Play(1, id, mCamSpacePos);
    return 1;
}


int daDemo_c::func_ov002_020f6ae4(unsigned char *p)
{
    unsigned int id = ReadUnalignedInt(p);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, id, mCamSpacePos, 0);
    return 1;
}


int daDemo_c::func_ov002_020f6b28(unsigned char *p)
{
    unsigned int v = ReadUnalignedInt(p);
    func_02012694(v, &mCamSpacePosX);
    return 1;
}

/* Script handler 15: turn to face the point whose whole-unit coordinates
 * are the unaligned shorts at p+0/+2/+4, and take the short at p+6 as the
 * horizontal speed. The legacy per-function file recorded its match under
 * mwccarm 1.2/sp2p3; this TU builds under the 2004/b56 pin. */

int daDemo_c::func_ov002_020f6b4c(unsigned char *p)
{
    Vector3 target;
    int y, z;
    z = ReadUnalignedShort(p + 4) << 12;
    y = ReadUnalignedShort(p + 2) << 12;
    target.x = ReadUnalignedShort(p) << 12;
    target.y = y;
    target.z = z;
    this->mAngleY = Vec3_HorzAngle(&this->mPos, &target);
    this->mPrevAngleY = this->mAngleY;
    this->mHorzSpeed = ReadUnalignedShort(p + 6);
    return 1;
}


s32 daDemo_c::func_ov002_020f6bc0(unsigned char *data)
{
    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    this->mPosX = x;
    this->mPosY = y;
    this->mPosZ = z;
    this->mAngleY = ReadUnalignedShort(data + 6);
    this->mPrevAngleY = this->mAngleY;
    return 1;
}


int daDemo_c::func_ov002_020f6c24(unsigned char *src)
{
    this->mOpacity = src[0];
    return 1;
}


int daDemo_c::func_ov002_020f6c34(unsigned char *src)
{
    unsigned char v = *src;
    ((anmModel_c *)this->mAnmModel)->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, v);
    return 1;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
inline unsigned int inline_fn(unsigned int arg0)
{
  return arg0;
}

}

int daDemo_c::func_ov002_020f6c60(void *arg1, int arg2, int arg3)
{
  u8 *flag = (u8 *) arg1;
  VecFx32 v;
  if (arg2 == gKuppaScriptFrame) {
      if ((*flag) != 0) {
          int r = (inline_fn(RandomIntInternal(&data_0209e650)) >> 20) & 0xfff;
          int m = FX_MUL(r, 0xa000);
          this->mMagnitude = m + 0x19000;
      }
      VEC_Add(&this->mPos, &((dCamera_c *)gCamera)->pos, &this->mPos);
      if (arg2 > 0x2bc) {
          this->mPosY -= 0x96000;
      }
      if (arg2 != data_ov002_0210b614) {
          func_0201267c(0x6a, &this->mCamSpacePos);
          data_ov002_0210b614 = arg2;
      }
  }
  if (arg3 == gKuppaScriptFrame) {
      this->mOpacity = 0;
      return 1;
  }
  this->mOpacity = 0x1f;
  if ((*flag) == 0)
  {
      /* First call stores the look-at point and registers ~Vector3(). */
      static __attribute__((aligned(8))) Vector3 sLookTarget(0xffdd6000, 0xbe4000, 0x2ae000);
      func_ov002_020f6f48(&sLookTarget, 0x20);
  }
  else
  {
      MTX_MultVec43((Vector3 *)&data_ov002_0210b958, (Matrix4x3 *)&data_0209b41c, (Vector3 *)&v);
      Vec3_LslInPlace((int *)&v, 3);
      func_ov002_020f6f48((Vector3 *)&v, 8);
  }
  this->mPrevAngleX = this->mAngleX;
  this->mPrevAngleY = this->mAngleY;
  this->mPrevAngleZ = this->mAngleZ;
  return 1;
}

/* Script handler 10: position (p[0..5], whole units shifted into 20.12) and
 * angles (p[6..11]) from the record, mMagnitude = p[12] << 12, scale 0xb33, and
 * the animation's currFrame set from a random draw scaled by
 * numFramesAndFlags. */

int daDemo_c::func_ov002_020f6e48(unsigned char *p)
{
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
    this->mPosX = c0;
    this->mPosY = b;
    this->mPosZ = a;
    d = ReadUnalignedShort(p + 0xa);
    e = ReadUnalignedShort(p + 8);
    f = ReadUnalignedShort(p + 6);
    this->mAngleX = (short)f;
    this->mAngleY = (short)e;
    this->mAngleZ = (short)d;
    this->mPrevAngleX = this->mAngleX;
    this->mPrevAngleY = this->mAngleY;
    this->mPrevAngleZ = this->mAngleZ;
    this->mMagnitude = (new_var = p[0xc]) << 12;
    this->mScaleX = 0xb33;
    this->mScaleY = 0xb33;
    this->mScaleZ = 0xb33;
    rv = mAnmModel->numFramesAndFlags;
    new_var2 = RandomIntInternal(&data_0209e650);
    mAnmModel->currFrame =
        ((unsigned short)(FX_MUL((int)((((unsigned int)new_var2) >> 20) & 0xfff), rv) >> 12)) << 12;
    return 1;
}

/* Turn mAngleX/mAngleY toward v by 1/amt of the remaining difference per
 * call (step capped at 0x4000, 90 degrees), then set mSpeedX, mVertSpeed and
 * mSpeedZ to mMagnitude times rotation elements 6..8 of the animated model's
 * matrix. */

void daDemo_c::func_ov002_020f6f48(struct Vector3 *v, int amt)
{
    Matrix4x3 *m;
    ApproachAngle(&this->mAngleX, Vec3_VertAngle((&this->mPos), v), amt, 0x4000, 0);
    ApproachAngle(&this->mAngleY, Vec3_HorzAngle((&this->mPos), v), amt, 0x4000, 0);
    m = &this->mAnmModel->mat4x3;
    this->mSpeedX = FX_MUL(this->mMagnitude, m->m[6]);
    this->mVertSpeed = FX_MUL(this->mMagnitude, m->m[7]);
    this->mSpeedZ = FX_MUL(this->mMagnitude, m->m[8]);
}

/* Script handler 9: clear mFlags bit 0x2. */

int daDemo_c::func_ov002_020f7020()
{
    this->mFlags &= ~0x2;
    return 1;
}


int daDemo_c::func_ov002_020f7038(int a, int arg)
{
    if (arg == gKuppaScriptFrame) {
        mParticleID[0] = 0;
        mParticleID[1] = 0;
        mParticleID[2] = 0;
    }
    if (this->mPosY >= 0x514000) {
        if (mRate < 0x3c000) {
            mRate += 0x5000;
        }
    } else {
        if (mRate > 0x1a000) {
            mRate -= 0x2000;
        }
    }
    {
        anmModel_c *obj = (anmModel_c *)this->mAnmModel;
        if (obj->mAnimEntry == 0 && (this->mGroundFlags & 1)) {
            obj->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, 1);
        }
    }
    this->mPosY -= FX_MUL(mRate, 0x199);
    {
        struct Vector3 pos;
        int z = this->mPosZ;
        int x = this->mPosX;
        int y = this->mPosY + 0x50000;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        mParticleID[0] =
            Particle::System::New(mParticleID[0], kPtclDemoDescendA, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[1] =
            Particle::System::New(mParticleID[1], kPtclDemoDescendB, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[2] =
            Particle::System::New(mParticleID[2], kPtclDemoDescendC, pos.x, pos.y, pos.z, 0, 0);
    }
    return 1;
}


int daDemo_c::func_ov002_020f71c4(int a, int arg)
{
    if (arg == 0) {
        if (arg == gKuppaScriptFrame) {
            this->mMagnitude = 0x10000;
        }
        ApproachLinear(this->mMagnitude, 0x1f000, 0x3e3);
        this->mOpacity = this->mMagnitude >> 12;
    }
    {
        struct Vector3 pos;
        int z = this->mPosZ;
        int x = this->mPosX;
        int y = this->mPosY + 0x50000;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        mParticleID[0] =
            Particle::System::New(mParticleID[0], kPtclDemoFadeInA, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[1] =
            Particle::System::New(mParticleID[1], kPtclDemoFadeInB, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[2] =
            Particle::System::New(mParticleID[2], kPtclDemoFadeInC, pos.x, pos.y, pos.z, 0, 0);
    }
    return 1;
}


int daDemo_c::func_ov002_020f72bc()
{
    *((u32*)&this->mScaleX) = 0x3000;
    *((u32*)&this->mScaleY) = 0x3000;
    *((u32*)&this->mScaleZ) = 0x3000;
    {
        s16* p = &this->mAngleY;
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


int daDemo_c::func_ov002_020f7384(unsigned char *flag, int val)
{
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

int daDemo_c::func_ov002_020f7410(unsigned char *in, int sel)
{
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
    MTX_MultVec43(((param_c &)*(simpleModel_c *)this->mModel).mScale, (Matrix4x3 *)&data_0209b41c,
                  (&this->mPos));
    Vec3_LslInPlace(&this->mPosX, 3);
    if (ApproachLinear(this->mMagnitude, a, b) != 0 && a == 0 && b != 0)
        func_0201ef38();
    *(char *)(&this->mOpacity) = *(int *)(&this->mMagnitude) >> 12;
    {
        unsigned char r = *(unsigned char *)(&this->mOpacity);
        if (r != 0 || a != 0) {
            t = (r * 0xa00) >> 12;
            if (t > 0x10) t = 0x10;
            G2_SetBlendAlpha(8, 1, t, 0x10 - t);
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

int daDemo_c::func_ov002_020f7538(unsigned char *arg1, int arg2)
{
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
    MTX_MultVec43(((param_c &)*(anmModel_c *)this->mAnmModel).mScale, (Matrix4x3 *)&data_0209b41c,
                  &this->mPos);
    Vec3_LslInPlace((int *)(&this->mPosX), 3);
    ApproachLinear(this->mMagnitude, f1, f2);
    this->mOpacity = this->mMagnitude >> 0xc;
    return 1;
}


int daDemo_c::func_ov002_020f7780(void *unused, int mode)
{
    dCamera_c *cam = (dCamera_c *)gCamera;

    if (mode == gKuppaScriptFrame) {
        if (mode == 0) {
            data_0208ee44 = 3;
            return 1;
        }

        {
            int v[3];
            unsigned char idx;
            v[0] = data_ov002_0210b970[0];
            v[1] = data_ov002_0210b970[1];
            v[2] = data_ov002_0210b970[2];

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
        short s0 = FX_SinCosTable_[((a >> 4) << 1) + 1];
        this->mHorzSpeed = FX_MUL(t, s0);
    }
    {
        unsigned short a = *(unsigned short *)(&this->mPrevAngleX);
        int t = this->mMagnitude;
        int s1 = -FX_SinCosTable_[(a >> 4) << 1];
        this->mVertSpeed = FX_MUL(t, s1);
    }

    {
        short *pfc = &this->mPhase;
        *pfc = *pfc + 1;
    }

    return 1;
}


int daDemo_c::func_ov002_020f79c0(void *unused, int mode)
{

    if (mode == gKuppaScriptFrame) {
        this->mMagnitude = 0x578000;
        *&this->mPhase = -0x1000;
        *&this->mPhaseStep = 0x800;
        this->mRate = -0xc8000;
        this->mPrevAngleY = -0x8000;
        this->mAngleY = -0x4000;
        this->mPrevAngleX = 0x800;
        this->mOpacity = 0x1f;
    } else {
        int v[3];
        int w[3];
        int h;
        int t;

        {
            short *p94 = &this->mPrevAngleY;
            *p94 = *p94 + 0x200;
        }
        Math_Function_0203b0fc(&this->mMagnitude, 0x64000, 0x7a, 0x7fffffff);

        this->mAngleX = cstd::atan2(this->mPosY - 0x190000, 0xc8000);

        ApproachAngle(&this->mAngleY, (short)(this->mPrevAngleY + 0x8000), 4, 0x4000, 0);

        data_ov002_02110ddc[2] = this->mMagnitude;
        v[0] = data_ov002_0210b964[0];
        v[1] = data_ov002_0210b964[1];
        v[2] = data_ov002_0210b964[2];
        w[0] = data_ov002_02110ddc[0];
        w[1] = data_ov002_02110ddc[1];
        w[2] = data_ov002_02110ddc[2];
        Vec3_RotateYAndTranslate(&this->mPosX, v, this->mPrevAngleY, w);

        h = (unsigned short)this->mPhase;
        t = FX_SinCosTable_[(h >> 4) * 2 + 1];
        {
            int *p60 = (int *)(&this->mPosY);
            *p60 = *p60 + FX_MUL(t, 0x96000);
        }

        {
            short *pfc = &this->mPhase;
            *pfc = *pfc + *&this->mPhaseStep;
        }
        ApproachAngle(&this->mPhaseStep, 0x200, 0x14, 0x4000, 0);

        {
            int *p5c = &this->mPosX;
            *p5c = *p5c + this->mRate;
        }
        Math_Function_0203b0fc(&this->mRate, 0, 0xcc, 0x7fffffff);
    }
    return 1;
}


int daDemo_c::func_ov002_020f7bb8(unsigned char *p, u16 id)
{
    Vector3 v0, v1;
    int src[3];
    Fix12i dist;
    s16 vertAngle, heading;
    dCamera_c *cam;
    unsigned int tbl1, tbl2;
    int i;

    this->mOpacity = 0;
    if (id == gKuppaScriptFrame) {
        this->mMagnitude = 0;
        this->mPhase = 0;
    }

    tbl1 = ReadUnalignedInt(p);
    tbl2 = ReadUnalignedInt(p + 4);

    if (MovePointAlongSpline(&v1, &this->mPhase, &this->mMagnitude, (CutsceneSpline *)tbl1) != 0) {
        return 0;
    }
    if (MovePointAlongSpline(&v0, &this->mPhase, &this->mMagnitude, (CutsceneSpline *)tbl2) != 0) {
        return 0;
    }

    this->mOpacity = 0x1f;
    cam = (dCamera_c *)gCamera;

    VEC_Add((Vector3 *)&v0, &cam->pos, (Vector3 *)&v0);
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
    src[1] = FX_MUL(dist, FX_SinCosTable_[(((u16)vertAngle) >> 4) * 2]);
    src[2] = FX_MUL(dist, FX_SinCosTable_[(((u16)vertAngle) >> 4) * 2 + 1]);

    Vec3_RotateYAndTranslate((int *)(&this->mPosX), (int *)&cam->pos, heading, src);

    this->mAngleX = cstd::atan2(v1.y, Vec3_HorzLen(&v1));
    this->mAngleY = cstd::atan2(v1.x, v1.z);

    return 1;
}

void daDemo_c::RunCommand(unsigned char *p, int a2, int a3) {
    /* Indexed by the command byte p[6]. The handlers take what they read
       from the command, so each is cast to the table's signature. */
    static PMF3 sCommandTable[24] = {
        (PMF3)&daDemo_c::func_ov002_020f7bb8, /* 0 */
        (PMF3)&daDemo_c::func_ov002_020f79c0, /* 1 */
        (PMF3)&daDemo_c::func_ov002_020f7780, /* 2 */
        (PMF3)&daDemo_c::func_ov002_020f7538, /* 3 */
        (PMF3)&daDemo_c::func_ov002_020f7410, /* 4 */
        (PMF3)&daDemo_c::func_ov002_020f7384, /* 5 */
        (PMF3)&daDemo_c::func_ov002_020f72bc, /* 6 */
        (PMF3)&daDemo_c::func_ov002_020f71c4, /* 7 */
        (PMF3)&daDemo_c::func_ov002_020f7038, /* 8 */
        (PMF3)&daDemo_c::func_ov002_020f7020, /* 9 */
        (PMF3)&daDemo_c::func_ov002_020f6e48, /* 10 */
        (PMF3)&daDemo_c::func_ov002_020f6c60, /* 11 */
        (PMF3)&daDemo_c::func_ov002_020f6c34, /* 12 */
        (PMF3)&daDemo_c::func_ov002_020f6c24, /* 13 */
        (PMF3)&daDemo_c::func_ov002_020f6bc0, /* 14 */
        (PMF3)&daDemo_c::func_ov002_020f6b4c, /* 15 */
        (PMF3)&daDemo_c::func_ov002_020f6b28, /* 16 */
        (PMF3)&daDemo_c::func_ov002_020f6ae4, /* 17 */
        (PMF3)&daDemo_c::func_ov002_020f6ab8, /* 18 */
        (PMF3)&daDemo_c::func_ov002_020f6a9c, /* 19 */
        (PMF3)&daDemo_c::func_ov002_020f6448, /* 20 */
        (PMF3)&daDemo_c::func_ov002_020f63f8, /* 21 */
        (PMF3)&daDemo_c::func_ov002_020f26d4, /* 22 */
        (PMF3)&daDemo_c::func_ov002_020f26c4, /* 23 */
    };
    (this->*sCommandTable[p[6]])(p + 7, a2, a3);
}

int daDemo_c::CleanupResources()
{
    int r1 = param1;
    if (r1 == 0x2e)
        return func_ov002_020f63a0();
    if (r1 == 0x2f)
        return func_ov002_020f23d0();
    /* The guards are compiled in: bare deletes build 0x70, not the ROM's 0x80. */
    if (mModel)
        delete mModel;
    if (mAnmModel)
        delete mAnmModel;
    return 1;
}

/* daDemo_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daDemo_c::OnPendingDestroy()
{
}

int daDemo_c::Render() {
    if (param1 == 0x19) {
        /* Draw the model three times: copy INV_VIEW_MATRIX_ASR_3 into its
           mat4x3, then for each entry of data_ov002_0210bb7c overwrite the
           translation column (m[9..11]) and render. The {p, cur, src} struct
           is the allocation shape the ROM compiles; a plain Matrix4x3
           pointer builds the same size with different registers. */
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

int daDemo_c::Behavior()
{
    Vector3 v;
    Vector3 asr;
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
    Vec3_Asr(&asr, &this->mPos, 3);
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

/* Each new-expression inlines the nested class's implicit construction:
 * param_c's constructor on the tail base, the model base's C2, then the
 * class vptr and, for anmModel_c, the dExtFrameCtrl_c base's secondary vptr
 * at +0x50. The model base sits at offset 0, so storing the result in
 * mModel/mAnmModel needs no null test. */
int daDemo_c::InitResources()
{
    int t;

    if (param1 == 0x12) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        if (((anmModel_c *)this->mAnmModel)
                ->func_ov002_020f6618((SharedFilePtr *)&data_ov085_0213074c, 1, data_ov002_0210b60c,
                                      1, 1, data_ov002_0210b608, -1) == 0)
            return 0;
    } else if (param1 == 0x13) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        t = (*(volatile unsigned char *)&gGameMode == 2);
        if (t == 0) {
            if (((anmModel_c *)this->mAnmModel)
                    ->func_ov002_020f6618((SharedFilePtr *)&data_ov002_02110b98, 1,
                                          data_ov002_0210b610, 1, 1, data_ov002_0210b600,
                                          0x16) == 0)
                return 0;
        } else {
            if (((anmModel_c *)this->mAnmModel)
                    ->func_ov002_020f6618((SharedFilePtr *)&data_ov002_02110c18, 0xD,
                                          (SharedFilePtr **)&data_ov002_0210bcf0, 1, 0xD,
                                          (SharedFilePtr **)&data_ov002_0210bd24, 0x16) == 0)
                return 0;
        }
    } else if (param1 >= 0x14 && param1 <= 0x16) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b70, -1) == 0) return 0;
    } else if (param1 == 0x17) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b50, 0x19) == 0) return 0;
    } else if (param1 == 0x18) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_0211094c, -1) == 0) return 0;
    } else if (param1 == 0x19) {
        this->mModel = new simpleModel_c;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b78, 0x13) == 0) return 0;
    } else if (param1 >= 0x1A && param1 <= 0x2D) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        if (((anmModel_c *)this->mAnmModel)
                ->func_ov002_020f6618((SharedFilePtr *)data_ov009_02113c20, 1, data_ov002_0210b604,
                                      0, 0, 0, -1) == 0)
            return 0;
    }
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daDemo_c through RTTI,
 * allocation size, vtable identity, and the DEMO registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: CutsceneObject_Spawn.
 *
 * `new daDemo_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x104), dActor_c's base constructor, then the vptr
 * store. */
extern "C" daDemo_c *daDemo_c_classInit(void)
{
    return new daDemo_c;
}

/* _ZThn80_N8daDemo_c10anmModel_cD0Ev / D1Ev have no source body. They are
 * the non-virtual this-adjustments ~anmModel_c emits for the
 * dExtFrameCtrl_c base at +0x50, and they sort after the vtables so they
 * close .text at 0x020f8838 and 0x020f8848. */

/* File handles and the ten state tables. mwcc emits __sinit_d_a_demo.cpp
   from these definitions: constructors in retail order, then the
   pointer-to-member copies. The 8-byte sources are POD aggregates so the
   copy stays a whole-struct move. */
struct DemoModelFile {
    unsigned int w[2];
    DemoModelFile(unsigned int fileID);
    ~DemoModelFile();
};
struct DemoAnimFile {
    unsigned int w[2];
    DemoAnimFile(unsigned int fileID);
    ~DemoAnimFile();
};
struct DemoTexSeqFile {
    unsigned int w[2];
    DemoTexSeqFile(unsigned int fileID);
    ~DemoTexSeqFile();
};

extern "C" {

int data_ov002_0210b658[2] = {(int)0x81f020f0, (int)0xffff0100};
int data_ov002_0210b6e8[2] = {(int)0x81f020f0, (int)0xffff0118};
int data_ov002_0210b768[2] = {(int)0x41f800f8, (int)0xffff701e};
int data_ov002_0210b868[2] = {(int)0x41f800f8, (int)0xffff705e};
struct OamAttr *data_ov002_0210b6a8[2] = {(struct OamAttr *)data_ov002_0210b768,
                                          (struct OamAttr *)data_ov002_0210b868};


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

/* One-entry file lists InitResources hands to anmModel_c::func_ov002_020f6618:
   the BCA list and the BTP list for param1 0x12 (0x0210b60c, 0x0210b608) and
   0x13 (0x0210b610, 0x0210b600), and the BCA list for 0x1a..0x2d. */
SharedFilePtr *data_ov002_0210b600[1] = {(SharedFilePtr *)&data_ov002_02110b10};
SharedFilePtr *data_ov002_0210b604[1] = {(SharedFilePtr *)data_ov009_02113c28};
SharedFilePtr *data_ov002_0210b608[1] = {&data_ov085_0213073c};
SharedFilePtr *data_ov002_0210b60c[1] = {&data_ov085_02130744};
SharedFilePtr *data_ov002_0210b610[1] = {(SharedFilePtr *)&data_ov002_02110bf8};
/* The last cue time func_ov002_020f6c60 played sound 0x6a for. */
int data_ov002_0210b614 = -1;
int data_ov002_0210b630[2] = {(int)0x81f020f0, (int)0xffff0108};
int data_ov002_0210b638[2] = {(int)0x81f020f0, (int)0xffff0110};

StaffRollCue data_ov002_02110f9c[22] = {
    &StaffRoll::func_ov002_020f5efc, &StaffRoll::func_ov002_020f5ef0,
    &StaffRoll::func_ov002_020f5ee4, &StaffRoll::func_ov002_020f5ed8,
    &StaffRoll::func_ov002_020f5eb8, &StaffRoll::func_ov002_020f5e98,
    &StaffRoll::func_ov002_020f5e98, &StaffRoll::func_ov002_020f5e98,
    &StaffRoll::func_ov002_020f5e98, &StaffRoll::func_ov002_020f5e78,
    &StaffRoll::func_ov002_020f5e78, &StaffRoll::func_ov002_020f5e78,
    &StaffRoll::func_ov002_020f5e78, &StaffRoll::func_ov002_020f5e58,
    &StaffRoll::func_ov002_020f5e58, &StaffRoll::func_ov002_020f5e58,
    &StaffRoll::func_ov002_020f5e58, &StaffRoll::func_ov002_020f5e38,
    &StaffRoll::func_ov002_020f5e38, &StaffRoll::func_ov002_020f5e38,
    &StaffRoll::func_ov002_020f5e38, &StaffRoll::func_ov002_020f5e18};
StaffRollCue data_ov002_02110f34[13] = {
    &StaffRoll::func_ov002_020f5d34, &StaffRoll::func_ov002_020f5cd0,
    &StaffRoll::func_ov002_020f5c88, &StaffRoll::func_ov002_020f5c40,
    &StaffRoll::func_ov002_020f5bf4, &StaffRoll::func_ov002_020f5bcc,
    &StaffRoll::func_ov002_020f5b98, &StaffRoll::func_ov002_020f5b8c,
    &StaffRoll::func_ov002_020f5b88, &StaffRoll::func_ov002_020f5b84,
    &StaffRoll::func_ov002_020f5b80, &StaffRoll::func_ov002_020f5b7c,
    &StaffRoll::func_ov002_020f5b24};
StaffRollStep data_ov002_02110e7c[4] = {
    &StaffRoll::func_ov002_020f5894, &StaffRoll::func_ov002_020f5848,
    &StaffRoll::func_ov002_020f562c, &StaffRoll::func_ov002_020f3d98};
StaffRollStep data_ov002_02110e64[3] = {&StaffRoll::func_ov002_020f57c0,
                                        &StaffRoll::func_ov002_020f569c,
                                        &StaffRoll::func_ov002_020f5678};
StaffRollStep data_ov002_02110eec[9] = {
    &StaffRoll::func_ov002_020f55b4, &StaffRoll::func_ov002_020f5328,
    &StaffRoll::func_ov002_020f5010, &StaffRoll::func_ov002_020f4d70,
    &StaffRoll::func_ov002_020f4a2c, &StaffRoll::func_ov002_020f4710,
    &StaffRoll::func_ov002_020f43cc, &StaffRoll::func_ov002_020f40fc,
    &StaffRoll::func_ov002_020f3de4};
StaffRollStep data_ov002_02110e34[3] = {&StaffRoll::func_ov002_020f3d38,
                                        &StaffRoll::func_ov002_020f3ba0,
                                        &StaffRoll::func_ov002_020f3ae8};
StaffRollStep data_ov002_02110e4c[3] = {&StaffRoll::func_ov002_020f378c,
                                        &StaffRoll::func_ov002_020f3740,
                                        &StaffRoll::func_ov002_020f3310};
StaffRollStep data_ov002_02110e9c[4] = {
    &StaffRoll::func_ov002_020f35e4, &StaffRoll::func_ov002_020f34bc,
    &StaffRoll::func_ov002_020f340c, &StaffRoll::func_ov002_020f335c};
StaffRollStep data_ov002_02110e24[2] = {&StaffRoll::func_ov002_020f32e4,
                                        &StaffRoll::func_ov002_020f30f8};
StaffRollStep data_ov002_02110ebc[6] = {
    &StaffRoll::func_ov002_020f2d70, &StaffRoll::func_ov002_020f2d0c,
    &StaffRoll::func_ov002_020f2bf4, &StaffRoll::func_ov002_020f2aec,
    &StaffRoll::func_ov002_020f2a88, &StaffRoll::func_ov002_020f2a78};
}
