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
 *   func_ov002_020f26d4 is a ldr/bx veneer that tail-calls
 *   func_ov002_020f2630 through a raw void(*)(void); a member pointer
 *   cannot spell that cast, so both stay free.
 *   The mStaffRoll heap block is daDemo_c::StaffRoll, a record recovered
 *   from its helpers (no RTTI names it). The helpers and the PMF/PMF0/
 *   PMF37/PMF2dd4 dispatch tables over it are its members; the tables'
 *   .data descriptors still name the member symbols because a pointer
 *   to member does not convert to the int word a descriptor holds.
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

/* Field offsets inside one star, window or credit entry. Where a helper
   indexes an entry, the ROM's code adds the field offset to the block
   first and the scaled index second ([base, i*stride] addressing, with
   unencodable sums like 0x1c9 in the literal pool); the arr[i] member
   spelling folds the index into the element pointer first and compiles
   differently. */
#define STAR_OFF(m) ((int)&((StaffRoll *)0)->stars[0].m)
#define WINDOW_OFF(m) ((int)&((StaffRoll *)0)->window[0].m)
#define CREDIT_OFF(m) ((int)&((StaffRoll *)0)->credit[0].m)

/* shadow typedef 's16' */
typedef signed short s16;

/* shadow struct 'OamEnt' */
struct OamEnt {
    s16 x;       /* 0 */
    s16 y;       /* 2 */
    s16 ang;     /* 4 */
    s16 pad;     /* 6 */
    int enabled; /* 8 */
};

/* shadow struct 'ParticleCallback' */
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

/* Eight-byte {code, 0} descriptor words. The flat int-pair initializer is
   what keeps the record in .data; the state tables below read each entry
   back through the matching pointer-to-member member, which copies the
   same eight bytes the old reinterpret cast did. */
union W {
    int w[2];
    PMF pmf;
    PMF0 pmf0;
    PMF3 pmf3;
    PMF37 pmf37;
    PMF2dd4 pmf2dd4;
};

/* shadow struct 'Ent' */
struct Ent {
    int out0;
    int out4;
    char pad8[8];
    int vx;
    char pad14[0x1a];
    u16 angle;
    char pad30[0x1c];
};

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

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov002_020f237c, NOT applied:
struct Obj {
    char _pad0[0xd4];
    struct ScrollState* scroll; /* 0xd4 *\/
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov002_020f2e30, NOT applied:
struct Obj {
    char pad1c4[0x1c4];
    unsigned short f1c4;   /* +0x1c4 *\/
    unsigned short f1c6;   /* +0x1c6 *\/
    unsigned char f1c8;    /* +0x1c8 *\/
    unsigned char f1c9;    /* +0x1c9 *\/
    unsigned char f1ca;    /* +0x1ca *\/
    unsigned char f1cb;    /* +0x1cb *\/
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'E', from the legacy file for func_ov002_020f5678, NOT applied:
struct E { char pad[0x4c]; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'E', from the legacy file for func_ov002_020f57c0, NOT applied:
struct E {
  char p0[8];
  int f8;
  int fc;
  char p10[0x20];
  unsigned short f30;
  unsigned short f32;
  char p34[6];
  unsigned short f3a;
  char p3c[9];
  unsigned char f45;
  char p46;
  unsigned char f47;
  char p48;
  unsigned char f49;
  char p4a[2];
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Ent', from the legacy file for func_ov002_020f5b98, NOT applied:
struct Ent {
    char pad44[0x44];
    unsigned char f44;   /* 0x44 *\/
    char pad45;          /* 0x45 *\/
    unsigned char f46;   /* 0x46 *\/
    unsigned char f47;   /* 0x47 *\/
    char pad48[2];       /* 0x48 *\/
    unsigned char f4a;   /* 0x4a *\/
    char pad4b[0x4c - 0x4b];
};
*/

/* TUBUILD CONFLICT -- alternate body of struct 'E', from the legacy file for func_ov002_020f5cd0, NOT applied:
struct E {
    s32 f0;       /* 0x00 *\/
    s32 f4;       /* 0x04 *\/
    char p8[0x28-8];
    s32 f28;      /* 0x28 *\/
    u16 f2c;      /* 0x2c *\/
    char p2e[0x30-0x2e];
    u16 f30;      /* 0x30 *\/
    u16 f32;      /* 0x32 *\/
    char p34[0x3a-0x34];
    u16 f3a;      /* 0x3a *\/
    char p3c[0x4c-0x3c];
};
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'void', from the legacy file for func_ov002_020f5f0c, NOT applied:
typedef void (C::*PMF)();
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov002_020f63a0, NOT applied:
struct Obj { char pad[0xd8]; void *p; /* 0xd8 *\/ };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Ent', from the legacy file for func_ov002_020f6514, NOT applied:
struct Ent { void *p0; void *p4; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov002_020f65b8, NOT applied:
struct Obj { char pad[0x7c]; void *seq; /* 0x7c *\/ };
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'void', from the legacy file for _ZN8daDemo_c10anmModel_cD0Ev, NOT applied:
typedef void (*VFN)(void *);
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'void', from the legacy file for _ZN8daDemo_c10anmModel_cD1Ev, NOT applied:
typedef void (*VFN)(void *);
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for func_ov002_020f6b4c, NOT applied:
typedef short s16;
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for func_ov002_020f6b4c, NOT applied:
struct Obj {
    char pad5c[0x5c];
    struct Vector3 v5c;   /* +0x5c *\/
    char pad8e[0x8e - (0x5c + 0xc)];
    s16 f8e;              /* +0x8e *\/
    char pad94[0x94 - 0x90];
    s16 f94;              /* +0x94 *\/
    char pad98[0x98 - 0x96];
    int f98;             /* +0x98 *\/
};
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for func_ov002_020f6bc0, NOT applied:
typedef short s16;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for func_ov002_020f6f48, NOT applied:
typedef short s16;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for func_ov002_020f7bb8, NOT applied:
typedef short s16;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'void', from the legacy file for func_ov002_020f7d74, NOT applied:
typedef void (C::*PMF)(unsigned char*, int, int);
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN8daDemo_c16CleanupResourcesEv, NOT applied:
struct Obj { virtual void v00(); virtual void m04(); };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN8daDemo_c13InitResourcesEv, NOT applied:
struct Obj {
    char pad0[8];
    unsigned int unk8;
    char pad_c[0x74];
    int unk80;
    int unk84;
    int unk88;
    char pad_8c[0x50];
    void *unkDC;
    void *unkE0;
};
*/

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
/* local extern dropped: decl_common.h already declares _ZN6Memory16operator_delete2EPv(void*) */
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
/* local extern dropped: decl_common.h already declares LoadFont(u8) */
extern PMF0 data_ov002_02110f9c[];
extern void SetSubBg3Offset(int a, int b);
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *p);
extern void func_02056434(const void *src, int offset, int count);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void func_02056374(const void *src, u32 offset, u32 count);
/* local extern dropped: decl_common.h declares func_ov002_020f6514(unsigned char*, void*, unsigned char) */
extern void func_02012694(unsigned int, void *);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
/* local extern dropped: decl_common.h declares data_ov002_0210bc88 as 'int'; callers cast the address */
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern int *Vec3_LslInPlace(int *v, int sh);
extern u16 gKuppaScriptFrame;
extern char *gCamera;
extern int data_ov002_0210b614;
extern int data_ov002_0210b958;
/* local extern dropped: decl_common.h declares data_0209b41c */
extern s16 Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);
extern int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern void func_0201f138(void);
extern void func_0201ef38(void);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);
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
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzLen(const Vector3 *v);
extern PMF3 data_ov002_0210b618;
extern PMF3 data_ov002_0210b620;
extern PMF3 data_ov002_0210b640;
extern PMF3 data_ov002_0210b628;
extern PMF3 data_ov002_0210b648;
/* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2a78Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2a88Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2aecEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2bf4Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2d0cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f2d70Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f30f8Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f32e4Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3310Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f335cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f340cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f34bcEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f35e4Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3740Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f378cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3ae8Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3ba0Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3d38Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3d98Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f3de4Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f40fcEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f43ccEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f4710Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f4a2cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f4d70Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5010Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5328Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f55b4Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f562cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5678Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f569cEi(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f57c0Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5848Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5894Ei(StaffRoll *,
                                              int); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b7cEv(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b80Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b84Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b88Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b8cEv(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b98Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5bccEv(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5bf4Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5c40Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5c88Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5cd0Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5d34Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5e18Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5e38Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5e58Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5e78Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5e98Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5eb8Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5ed8Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5ee4Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5ef0Ev(StaffRoll *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5efcEv(StaffRoll *); /* local extern: descriptor address */
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
/* TUBUILD CONFLICT -- alternate declaration of gSubScreenLayers, from the legacy file for func_ov002_020f23f0, NOT applied: extern u8 gSubScreenLayers; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f2aec, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, unsigned short a, unsigned short b, unsigned short c, int d); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f2bf4, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, unsigned short a, unsigned short b, int c, unsigned short d); */
/* TUBUILD CONFLICT -- alternate declaration of func_0201ef50, from the legacy file for func_ov002_020f2e30, NOT applied: extern void func_0201ef50(unsigned int x); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f3de4, NOT applied: extern int (char *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f40fc, NOT applied: extern int (char *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd5atan2E5Fix12IiES1_, from the legacy file for func_ov002_020f43cc, NOT applied: extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f43cc, NOT applied: extern int (void *a)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4710, NOT applied: extern int (void *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4a2c, NOT applied: extern int (char *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4d70, NOT applied: extern int (void *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f5010, NOT applied: extern int (char *c)->func_ov002_020f5a94(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f2790, from the legacy file for func_ov002_020f5990, NOT applied: extern "C" void (char* c)->func_ov002_020f2790(int p1,int p2,int p3,s16 p4); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8SaveData19IsCharacterUnlockedEj, from the legacy file for func_ov002_020f5a94, NOT applied: extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int idx); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f2984, from the legacy file for func_ov002_020f5ad4, NOT applied: extern void (char* p)->func_ov002_020f2984(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e38, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e58, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e78, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e98, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5eb8, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5ed8, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5ee4, NOT applied: extern void (void *)->func_ov002_020f3828(); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Memory16operator_delete2EPv, from the legacy file for func_ov002_020f5f60, NOT applied: extern void _ZN6Memory16operator_delete2EPv(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of gSubScreenLayers, from the legacy file for func_ov002_020f5fe4, NOT applied: extern u8 gSubScreenLayers; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Memory16operator_delete2EPv, from the legacy file for func_ov002_020f63a0, NOT applied: extern void _ZN6Memory16operator_delete2EPv(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN15dExtFrameCtrl_c7AdvanceEv, from the legacy file for func_ov002_020f65ec, NOT applied: extern int _ZN15dExtFrameCtrl_c7AdvanceEv(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of ReadUnalignedShort, from the legacy file for func_ov002_020f6bc0, NOT applied: extern s16 ReadUnalignedShort(unsigned char *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f6514, from the legacy file for func_ov002_020f6c34, NOT applied: extern void func_ov002_020f6514(void *p, void *tbl, unsigned char v); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov002_020f6c60, NOT applied: extern unsigned int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7Vector3D1Ev, from the legacy file for func_ov002_020f6c60, NOT applied: extern int _ZN7Vector3D1Ev; */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f6f48, NOT applied: extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE, from the legacy file for func_ov002_020f7038, NOT applied: extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( unsigned int a, unsigned int b, int p2, int p3, int p4, void* p5, void* p6); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE, from the legacy file for func_ov002_020f71c4, NOT applied: extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( unsigned int a, unsigned int b, int p2, int p3, int p4, void* p5, void* p6); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE, from the legacy file for func_ov002_020f72bc, NOT applied: extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( u32 slot, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* rot, struct Callback* cb); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov002_020f7384, NOT applied: extern int _Z14ApproachLinearRiii(int* v, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f7384, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f7410, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov002_020f7410, NOT applied: extern void MulVec3Mat4x3(void *out, void *m, void *v); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_LslInPlace, from the legacy file for func_ov002_020f7410, NOT applied: extern void Vec3_LslInPlace(void *v, int sh); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov002_020f7410, NOT applied: extern int _Z14ApproachLinearRiii(int *p, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f7410, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *reg, unsigned short a, unsigned short b, int c, int d); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov002_020f7538, NOT applied: extern void MulVec3Mat4x3(void* a, void* b, void* c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_LslInPlace, from the legacy file for func_ov002_020f7538, NOT applied: extern void Vec3_LslInPlace(void* v, int n); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov002_020f7538, NOT applied: extern int _Z14ApproachLinearRiii(int* dst, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f7538, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f7780, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of gCamera, from the legacy file for func_ov002_020f7780, NOT applied: extern void *gCamera; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f7780, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7780, NOT applied: extern short Vec3_HorzAngle(const int *v0, const int *v1); */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f79c0, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f79c0, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of ApproachAngle, from the legacy file for func_ov002_020f79c0, NOT applied: extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep); */
/* TUBUILD CONFLICT -- alternate declaration of gCamera, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void *gCamera; */
/* TUBUILD CONFLICT -- alternate declaration of AddVec3, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd5atan2E5Fix12IiES1_, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x); */
/* The file handles and their registration nodes are defined at the
   bottom of this file; only the guard word and the 12-byte gap between
   the two out-of-order nodes stay here so the function-local Vector3
   statics keep their retail addresses. */
int data_ov002_02110b00;
int data_ov002_02110ddc[3];
/* TUBUILD CONFLICT -- alternate declaration of data_0209b41c, from the legacy file for _ZN8daDemo_c6RenderEv, NOT applied: extern M48 data_0209b41c; */
}

daDemo_c::~daDemo_c() {}

/* the D0 deleting destructor this file is enrolled for emits from the
   ~daDemo_c definition at ordinal 0 below; objisolate keeps the
   variant this file's delinks entry names. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020f1fcc, 0x020f1fcc, size 0x128 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020f20f4, 0x020f20f4, size 0x11c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020f2210, 0x020f2210, size 0x130 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020f2340, 0x020f2340, size 0x3c */
/* -------------------------------------------------------------------------- */
/* Draw the two centre sprites at x 0x80. */

void daDemo_c::func_ov002_020f2340() {
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[0]);
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[1]);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020f237c, 0x020f237c, size 0x54 */
/* -------------------------------------------------------------------------- */
/* Scroll the two sub-screen BG layers in opposite directions (bg1 x down
 * one, bg0 x up one) and write both offsets to the hardware. */

void daDemo_c::func_ov002_020f237c() {
    mScroll->bg1X -= 1;
    mScroll->bg0X += 1;
    SetSubBg0Offset(mScroll->bg0X, mScroll->bg0Y);
    SetSubBg1Offset(mScroll->bg1X, mScroll->bg1Y);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020f23d0, 0x020f23d0, size 0x20 */
/* -------------------------------------------------------------------------- */
/* CleanupResources for param1 0x2f: free the scroll block. */

int daDemo_c::func_ov002_020f23d0() {
    Memory::operator_delete2(this->mScroll);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020f23f0, 0x020f23f0, size 0x240 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020f2630, 0x020f2630, size 0x94 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov002_020f26c4, 0x020f26c4, size 0x10 */
/* -------------------------------------------------------------------------- */
/* Script handler (table entry 23): mCue = p[0], the value 23f0 waits on. */

int daDemo_c::func_ov002_020f26c4(unsigned char *src) {
    this->mCue = src[0];
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020f26d4, 0x020f26d4, size 0xc */
/* -------------------------------------------------------------------------- */
/* func_ov002_020f26d4 @ 0x20f26d4 (ov002) -- tail-call veneer to func_ov002_020f2630 (0x20f2630).
 * ldr ip, [pc]; bx ip; .word 0x20f2630
 */
extern "C" { /* ldr/bx veneer: a member pointer cannot spell the void(*)(void) cast */
void func_ov002_020f26d4(void) {
    ((void (*)(void))func_ov002_020f2630)();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov002_020f26e0, 0x020f26e0, size 0xb0 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov002_020f2790, 0x020f2790, size 0x58 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2790(int p1, int p2, int p3, s16 p4) {
    this->kira.unk308 = p3;
    this->kira.unk30c = p4;
    this->kiraParticle =
        Particle::System::New(this->kiraParticle, kPtclEndingStarSparkle, p1 << 12, p2 << 12, 0, 0,
                              (dPa_c::level_c::callback_c *)((struct ParticleCallback *)&this->kira));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020f27e8, 0x020f27e8, size 0x170 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov002_020f2958, 0x020f2958, size 0x2c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2958() {
    this->burstActive = 1;
    this->burstDefParam = 0x199;
    this->burstSysParam = 0x199;
    this->burstTimer = 0;
    this->burstPeriods = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020f2984, 0x020f2984, size 0xc */
/* -------------------------------------------------------------------------- */
int StaffRoll::func_ov002_020f2984() {
    this->burstActive = 0;
    return (int)this;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020f2990, 0x020f2990, size 0xb8 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov002_020f2a48, 0x020f2a48, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2a48() {
    /* 0x1d8/0x1d9 and the position: see func_ov002_020f2990. */
    this->endActive = 1;
    this->endVisible = 1;
    this->endX = 524288;
    this->endY = 393216;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020f2a68, 0x020f2a68, size 0x10 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2a68() {
    /* 0x1d8/0x1d9: see func_ov002_020f2990. */
    this->endActive = 0;
    this->endVisible = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020f2a78, 0x020f2a78, size 0x10 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2a78(int i) {
    this->credit[i].active = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020f2a88, 0x020f2a88, size 0x64 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2a88(int i) {
    unsigned long new_var;
    *((short *)(((char *)this + CREDIT_OFF(timer)) + (i * 8))) =
        (*((u16 *)(((char *)this + CREDIT_OFF(timer)) + (i * 8)))) + 1;
    new_var = i;
    if ((*((u16 *)(((char *)this + (new_var * 8)) + CREDIT_OFF(timer)))) < 0x39) {
        return;
    }
    *((u16 *)(((char *)this + (new_var * 8)) + CREDIT_OFF(timer))) = 0;
    *((u8 *)(((char *)this + CREDIT_OFF(step)) + (new_var * 8))) =
        (*((u8 *)(((char *)this + CREDIT_OFF(step)) + (new_var * 8)))) + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020f2aec, 0x020f2aec, size 0x108 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2aec(int idx) {
    StaffRoll *s = this;

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov002_020f2bf4, 0x020f2bf4, size 0x118 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2bf4(int idx) {
    StaffRoll *s = this;

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020f2d0c, 0x020f2d0c, size 0x64 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2d0c(int i) {
    long new_var;
    *((short *)(((char *)this + CREDIT_OFF(timer)) + (i * 8))) =
        (*((unsigned short *)(((char *)this + CREDIT_OFF(timer)) + (i * 8)))) + 1;
    new_var = 8;
    StaffRoll *r = (StaffRoll *)((char *)this + (i * new_var));
    if (r->credit[0].timer < 0x39) {
        return;
    }
    r->credit[0].timer = 0;
    *((unsigned char *)(((char *)this + CREDIT_OFF(step)) + (i * new_var))) =
        (*((unsigned char *)(((char *)this + CREDIT_OFF(step)) + (i * new_var)))) + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov002_020f2d70, 0x020f2d70, size 0x64 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2d70(int i) {
    StaffRoll *p;
    p = (StaffRoll *)((char *)this + CREDIT_OFF(timer));
    *((short *)(((char *)p) + (i * 8))) = (*((unsigned short *)(((char *)p) + (i * 8)))) + 1;
    p = (StaffRoll *)((char *)this + (i * 8));
    if (p->credit[0].timer < 0x1e) {
        return;
    }
    p->credit[0].timer = 0;
    *((unsigned char *)(((char *)this + CREDIT_OFF(step)) + (i * 8))) =
        (*((unsigned char *)(((char *)this + CREDIT_OFF(step)) + (i * 8)))) + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020f2dd4, 0x020f2dd4, size 0x5c */
/* -------------------------------------------------------------------------- */
extern Entry2dd4 data_ov002_02110ebc[];
void StaffRoll::func_ov002_020f2dd4() {
    if (this->credit[0].active == 0)
        return;
    (this->*data_ov002_02110ebc[this->credit[0].step].pmf)(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- StartCredit, 0x020f2e30, size 0x70 */
/* -------------------------------------------------------------------------- */
/* StartCredit at 0x020f2e30
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov002_020f2ea0, 0x020f2ea0, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f2ea0() {
    this->credit[0].active = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov002_020f2eac, 0x020f2eac, size 0x6c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov002_020f2f18, 0x020f2f18, size 0x4c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov002_020f2f64, 0x020f2f64, size 0x38 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov002_020f2f9c, 0x020f2f9c, size 0x138 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov002_020f30d4, 0x020f30d4, size 0x24 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f30d4() {
    StaffRoll *p = this;
    int i;
    for (i = 0; i < 5; i++) {
        p->dolls[0].active = 0;
        p->dolls[0].visible = 0;
        p = (StaffRoll *)((Doll *)p + 1);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov002_020f30f8, 0x020f30f8, size 0x1ec */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f30f8(int i) {
    int idx = i * 0x30;
    unsigned short t;
    int oldA, oldB;
    int cc, dd, ee, ff;

    t = *(unsigned short *)((char *)this + WINDOW_OFF(delay) + idx);
    if (t != 0) {
        *(unsigned short *)((char *)this + WINDOW_OFF(delay) + idx) = t - 1;
        return;
    }

    *(int *)((char *)this + WINDOW_OFF(left) + idx) -=
        *(int *)((char *)this + WINDOW_OFF(stepX) + idx);
    *(int *)((char *)this + WINDOW_OFF(top) + idx) -=
        *(int *)((char *)this + WINDOW_OFF(stepY) + idx);
    *(int *)((char *)this + WINDOW_OFF(right) + idx) +=
        *(int *)((char *)this + WINDOW_OFF(stepX) + idx);
    *(int *)((char *)this + WINDOW_OFF(bottom) + idx) +=
        *(int *)((char *)this + WINDOW_OFF(stepY) + idx);

    oldA = *(int *)((char *)this + WINDOW_OFF(bgX) + idx);
    oldB = *(int *)((char *)this + WINDOW_OFF(bgY) + idx);
    *(int *)((char *)this + WINDOW_OFF(bgX) + idx) =
        oldA + *(int *)((char *)this + WINDOW_OFF(stepX) + idx);
    *(int *)((char *)this + WINDOW_OFF(bgY) + idx) +=
        *(int *)((char *)this + WINDOW_OFF(stepY) + idx);

    if (*(int *)((char *)this + WINDOW_OFF(bgX) + idx) >= 0)
        *(int *)((char *)this + WINDOW_OFF(bgX) + idx) = 0;
    if (*(int *)((char *)this + WINDOW_OFF(bgY) + idx) >= 0)
        *(int *)((char *)this + WINDOW_OFF(bgY) + idx) = 0;
    if (*(int *)((char *)this + WINDOW_OFF(left) + idx) < 0)
        *(int *)((char *)this + WINDOW_OFF(left) + idx) = 0;
    if (*(int *)((char *)this + WINDOW_OFF(top) + idx) < 0)
        *(int *)((char *)this + WINDOW_OFF(top) + idx) = 0;
    if (*(int *)((char *)this + WINDOW_OFF(right) + idx) >= 0xff000)
        *(int *)((char *)this + WINDOW_OFF(right) + idx) = 0xff000;
    if (*(int *)((char *)this + WINDOW_OFF(bottom) + idx) >= 0xc0000)
        *(int *)((char *)this + WINDOW_OFF(bottom) + idx) = 0xc0000;
    if (*(int *)((char *)this + WINDOW_OFF(bgX) + idx) > 0)
        *(int *)((char *)this + WINDOW_OFF(bgX) + idx) = 0;
    if (*(int *)((char *)this + WINDOW_OFF(bgY) + idx) > 0)
        *(int *)((char *)this + WINDOW_OFF(bgY) + idx) = 0;

    cc = *(int *)((char *)this + WINDOW_OFF(bgX) + idx) - oldA;
    func_ov002_020f2f18(cc, *(int *)((char *)this + WINDOW_OFF(bgY) + idx) - oldB);

    ee = *(int *)((char *)this + WINDOW_OFF(right) + idx) >> 12;
    ff = *(int *)((char *)this + WINDOW_OFF(bottom) + idx) >> 12;
    cc = *(int *)((char *)this + WINDOW_OFF(left) + idx) >> 12;
    dd = *(int *)((char *)this + WINDOW_OFF(top) + idx) >> 12;
    *(unsigned short *)0x4001042 = ((cc << 8) & 0xff00) | (ee & 0xff);
    *(unsigned short *)0x4001046 = ((dd << 8) & 0xff00) | (ff & 0xff);
    SetSubBg1Offset(*(int *)((char *)this + WINDOW_OFF(bgX) + idx) >> 12,
                    *(int *)((char *)this + WINDOW_OFF(bgY) + idx) >> 12);
    SetSubBg2Offset(*(int *)((char *)this + WINDOW_OFF(bgX) + idx) >> 12,
                    *(int *)((char *)this + WINDOW_OFF(bgY) + idx) >> 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov002_020f32e4, 0x020f32e4, size 0x2c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f32e4(int i) {
    this->window[i].delay = 0x10;
    this->window[i].stepX = 0x1800;
    this->window[i].stepY = 0x1800;
    this->window[i].step = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov002_020f3310, 0x020f3310, size 0x4c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3310(int idx) {
    unsigned char sel = this->window[idx].step;
    (this->*data_ov002_02110e24[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov002_020f335c, 0x020f335c, size 0xb0 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f335c(int i) {
    StaffRoll *s = this;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov002_020f340c, 0x020f340c, size 0xb0 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f340c(int i) {
    StaffRoll *s = this;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov002_020f34bc, 0x020f34bc, size 0x128 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f34bc(int idx) {
    int k = idx * 0x30;
    int flag;
    int a, b, c, d;
    int bs, cs, ds, as_;
    *(int *)((unsigned char *)this + WINDOW_OFF(left) + k) -= 0x1800;
    *(int *)((unsigned char *)this + WINDOW_OFF(top) + k) -= 0x1800;
    *(int *)((unsigned char *)this + WINDOW_OFF(right) + k) += 0x1800;
    *(int *)((unsigned char *)this + WINDOW_OFF(bottom) + k) += 0x1800;
    flag = 0;
    if (*(int *)((unsigned char *)this + WINDOW_OFF(left) + k) <= 0x80000)
        *(int *)((unsigned char *)this + WINDOW_OFF(left) + k) = 0x80000;
    if (*(int *)((unsigned char *)this + WINDOW_OFF(top) + k) <= 0x56000)
        *(int *)((unsigned char *)this + WINDOW_OFF(top) + k) = 0x56000;
    if (*(int *)((unsigned char *)this + WINDOW_OFF(right) + k) >= 0xfc000)
        *(int *)((unsigned char *)this + WINDOW_OFF(right) + k) = 0xfc000;
    if (*(int *)((unsigned char *)this + WINDOW_OFF(bottom) + k) >= 0xbc000) {
        *(int *)((unsigned char *)this + WINDOW_OFF(bottom) + k) = 0xbc000;
        flag++;
    }
    if (flag != 0)
        (*(unsigned char *)((unsigned char *)this + WINDOW_OFF(step) + k))++;
    a = *(int *)((unsigned char *)this + WINDOW_OFF(top) + k);
    b = *(int *)((unsigned char *)this + WINDOW_OFF(left) + k);
    c = *(int *)((unsigned char *)this + WINDOW_OFF(right) + k);
    d = *(int *)((unsigned char *)this + WINDOW_OFF(bottom) + k);
    bs = b >> 12;
    cs = c >> 12;
    ds = d >> 12;
    as_ = a >> 12;
    *(unsigned short *)0x4001042 = ((bs << 8) & 0xff00) | (cs & 0xff);
    *(unsigned short *)0x4001046 = ((as_ << 8) & 0xff00) | (ds & 0xff);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov002_020f35e4, 0x020f35e4, size 0x15c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f35e4(int i) {
    int off = i * 0x30;
    *(int *)((char *)this + WINDOW_OFF(left) + off) = 0xe0000;
    *(int *)((char *)this + WINDOW_OFF(top) + off) = 0x66000;
    *(int *)((char *)this + WINDOW_OFF(right) + off) = 0xe0000;
    *(int *)((char *)this + WINDOW_OFF(bottom) + off) = 0x66000;
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
        int a = *(int *)((char *)this + WINDOW_OFF(left) + off) >> 12;
        int d = *(int *)((char *)this + WINDOW_OFF(right) + off) >> 12;
        int e = *(int *)((char *)this + WINDOW_OFF(bottom) + off) >> 12;
        int b = *(int *)((char *)this + WINDOW_OFF(top) + off) >> 12;
        *((volatile unsigned short *)0x4001042) =
            (unsigned short)(((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *)0x4001046) =
            (unsigned short)(((b << 8) & 0xff00) | (e & 0xff));
    }
    *(int *)((char *)this + WINDOW_OFF(bgX) + off) = -0x60000;
    *(int *)((char *)this + WINDOW_OFF(bgY) + off) = -0x38000;
    SetSubBg1Offset(*(int *)((char *)this + WINDOW_OFF(bgX) + off) >> 12,
                    *(int *)((char *)this + WINDOW_OFF(bgY) + off) >> 12);
    SetSubBg2Offset(*(int *)((char *)this + WINDOW_OFF(bgX) + off) >> 12,
                    *(int *)((char *)this + WINDOW_OFF(bgY) + off) >> 12);
    gSubScreenLayers |= 6;
    *(unsigned char *)((char *)this + WINDOW_OFF(step) + off) =
        *(unsigned char *)((char *)this + WINDOW_OFF(step) + off) + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov002_020f3740, 0x020f3740, size 0x4c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3740(int idx) {
    unsigned char sel = this->window[idx].step;
    (this->*data_ov002_02110e9c[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov002_020f378c, 0x020f378c, size 0x14 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f378c(int idx) {
    this->window[idx].step = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov002_020f37a0, 0x020f37a0, size 0x5c */
/* -------------------------------------------------------------------------- */
extern Entry37 data_ov002_02110e4c[];
void StaffRoll::func_ov002_020f37a0() {
    if (this->window[0].active == 0)
        return;
    (this->*data_ov002_02110e4c[this->window[0].kind].pmf)(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov002_020f37fc, 0x020f37fc, size 0x2c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f37fc() {
    func_ov002_020f3828();
    this->window[0].active = 1;
    this->window[0].kind = 2;
    this->window[0].step = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov002_020f3828, 0x020f3828, size 0x12c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov002_020f3954, 0x020f3954, size 0x18 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3954() {
    this->window[0].active = 1;
    this->window[0].kind = 1;
    this->window[0].step = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov002_020f396c, 0x020f396c, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f396c() {
    this->window[0].active = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov002_020f3978, 0x020f3978, size 0x74 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov002_020f39ec, 0x020f39ec, size 0xfc */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov002_020f3ae8, 0x020f3ae8, size 0xb8 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3ae8(int idx) {
    Star *arr = this->stars;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov002_020f3ba0, 0x020f3ba0, size 0x198 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3ba0(int i) {
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
        *(int *)((char *)this + STAR_OFF(x) + off) += FX_MUL(tv, spd);
    }

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
        *(int *)((char *)this + STAR_OFF(y) + off) += FX_MUL(tv, spd);
    }

    *(int *)((char *)this + STAR_OFF(speed) + off) += 0x30;

    dx = 0xe0 - (*(int *)((char *)this + STAR_OFF(x) + off) >> 12);
    dy = 0x46 - (*(int *)((char *)this + STAR_OFF(y) + off) >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        *(unsigned char *)((char *)this + STAR_OFF(step) + off) += 1;
        *(unsigned short *)((char *)this + STAR_OFF(delay) + off) = 0;
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = 0xc000;
        *(int *)((char *)this + STAR_OFF(radius) + off) = 0x20000;
        *(int *)((char *)this + STAR_OFF(x) + off) = 0xe0000;
        *(int *)((char *)this + STAR_OFF(y) + off) = 0x46000;
        return;
    }

    target = cstd::atan2(dy, dx);
    ang = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
    if (target > ang) {
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) += 0x80;
        if (target <= *(unsigned short *)((char *)this + STAR_OFF(heading) + off))
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
    } else {
        if (ang <= target)
            return;
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -= 0x80;
        if (*(unsigned short *)((char *)this + STAR_OFF(heading) + off) <= target)
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov002_020f3d38, 0x020f3d38, size 0x60 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3d38(int i) {
    int off = i * 0x4c;
    if (*(u16 *)((char *)this + STAR_OFF(delay) + off) != 0) {
        *(u16 *)((char *)this + STAR_OFF(delay) + off) =
            *(u16 *)((char *)this + STAR_OFF(delay) + off) - 1;
        return;
    }
    *(s32 *)((char *)this + STAR_OFF(x) + off) = -0x10000;
    *(s32 *)((char *)this + off + STAR_OFF(y)) = -0x10000;
    *(s32 *)((char *)this + off + STAR_OFF(speed)) = 0x2000;
    *(u16 *)((char *)this + off + STAR_OFF(heading)) = 0;
    *(u16 *)((char *)this + STAR_OFF(delay) + off) = 0;
    *(u8 *)((char *)this + STAR_OFF(step) + off) = *(u8 *)((char *)this + STAR_OFF(step) + off) + 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov002_020f3d98, 0x020f3d98, size 0x4c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f3d98(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110e34[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov002_020f3de4, 0x020f3de4, size 0x318 */
/* -------------------------------------------------------------------------- */
/* Per-slot update of one 0x4c-byte cutscene record. func_ov002_020f5a94
 * takes c so r0 stays live through the orbit path to the call. */
void StaffRoll::func_ov002_020f3de4(int i) {
    char *c = (char *)this;
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;
    int j;

    v = *(unsigned short *)(c + STAR_OFF(delay) + off);
    if (v != 0) {
        *(unsigned short *)(c + STAR_OFF(delay) + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + STAR_OFF(phase) + off) == 0) {
        idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + STAR_OFF(speed) + off);
            *(int *)(c + STAR_OFF(x) + off) += FX_MUL(tv, spd);
        }

        idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + STAR_OFF(speed) + off);
            *(int *)(c + STAR_OFF(y) + off) += FX_MUL(tv, spd);
        }

        if (*(int *)(c + STAR_OFF(speed) + off) >= 0x2000)
            *(int *)(c + STAR_OFF(speed) + off) -= 0x120;

        dx = (*(int *)(c + STAR_OFF(targetX) + off) - *(int *)(c + STAR_OFF(x) + off)) >> 12;
        dy = (*(int *)(c + STAR_OFF(targetY) + off) - *(int *)(c + STAR_OFF(y) + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
            *(int *)(c + STAR_OFF(orbitTravel) + off) = 0;
            *(unsigned short *)(c + STAR_OFF(heading) + off) = 0xc000;
            return;
        }

        target = cstd::atan2(dy, dx);
        ang = *(unsigned short *)(c + STAR_OFF(heading) + off);
        if (target > ang) {
            *(unsigned short *)(c + STAR_OFF(heading) + off) += 0x100;
            if (target <= *(unsigned short *)(c + STAR_OFF(heading) + off))
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
        } else {
            if (ang <= target)
                return;
            *(unsigned short *)(c + STAR_OFF(heading) + off) -= 0x100;
            if (*(unsigned short *)(c + STAR_OFF(heading) + off) <= target)
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
        }
        return;
    }

    idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)(c + STAR_OFF(radius) + off);
        *(int *)(c + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
    }
    idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)(c + STAR_OFF(radius) + off);
        *(int *)(c + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
    }
    *(unsigned short *)(c + STAR_OFF(heading) + off) +=
        *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
    *(int *)(c + STAR_OFF(orbitTravel) + off) += *(unsigned short *)(c + STAR_OFF(angSpeed) + off);

    if (i != func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000)
        return;

    Star *p = this->stars;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- func_ov002_020f40fc, 0x020f40fc, size 0x2d0 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f40fc(int i) {
    int off = i * 0x4c;

    if (*(unsigned short *)((char *)this + STAR_OFF(frame) + off) == 0) {
        if (*(unsigned short *)((char *)this + STAR_OFF(frameTimer) + off) == 0) {
            *(unsigned short *)((char *)this + STAR_OFF(loops) + off) += 1;
        }
    }

    if (*(unsigned char *)((char *)this + STAR_OFF(phase) + off) == 0) {
        int idx;
        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }
        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
        *(int *)((char *)this + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);

        if (this->stars[0].phase != 0) {
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) += 8;
        } else {
            if (*(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) < 0x200) {
                *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) += 6;
                if (*(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) > 0x200)
                    *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) = 0x200;
            }
        }

        if ((unsigned int)*(int *)((char *)this + STAR_OFF(orbitTravel) + off) >=
                (unsigned int)data_ov002_02100120[i] &&
            this->holdOrbit == 0) {
            *(unsigned char *)((char *)this + STAR_OFF(phase) + off) += 1;
            *(unsigned short *)((char *)this + STAR_OFF(flyTimer) + off) = 0;
            *(int *)((char *)this + STAR_OFF(speed) + off) = 0x2000;
        }

        if (func_ov002_020f5a94() != 3)
            return;
        if ((unsigned int)*(int *)((char *)this + STAR_OFF(orbitTravel) + off) < 0xc000)
            return;
        if (i != 2)
            return;
        if (this->holdOrbit != 0)
            return;
        *(unsigned char *)((char *)this + STAR_OFF(phase) + off) += 1;
        *(unsigned short *)((char *)this + STAR_OFF(flyTimer) + off) = 0;
        *(int *)((char *)this + STAR_OFF(speed) + off) = 0x2000;
        return;
    }

    *(unsigned short *)((char *)this + STAR_OFF(flyTimer) + off) += 1;
    *(int *)((char *)this + STAR_OFF(x) + off) += *(int *)((char *)this + STAR_OFF(speed) + off);
    *(int *)((char *)this + STAR_OFF(speed) + off) += 0x300;
    if (this->stars[0].flyTimer < 0x70)
        return;
    {
        int off2 = (int)((long long)i) * 0x4c;
        *(unsigned char *)((char *)this + STAR_OFF(step) + off2) += 1;
        *(unsigned short *)((char *)this + STAR_OFF(loops) + off2) = 0;
        *(unsigned char *)((char *)this + STAR_OFF(phase) + off) = 0;
        *(int *)((char *)this + STAR_OFF(radius) + off2) = 0x38000;
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off2) = 0;
        *(unsigned short *)((char *)this + STAR_OFF(delay) + off2) = data_ov002_02100130[i] << 6;
        *(int *)((char *)this + STAR_OFF(x) + off) = -0x10000;
        *(int *)((char *)this + STAR_OFF(y) + off2) = 0;
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off2) = 0x100;
        *(int *)((char *)this + STAR_OFF(speed) + off) = 0x5800;
        *(int *)((char *)this + STAR_OFF(targetX) + off2) = 0x80000;
        *(int *)((char *)this + STAR_OFF(targetY) + off2) = 0x28000;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- func_ov002_020f43cc, 0x020f43cc, size 0x344 */
/* -------------------------------------------------------------------------- */
/* recovered: per-slot update of a 4-entry cursor/target table (0x4c-byte
 * records at c). Slot i first burns its hold timer (+0x30); while its arrival
 * flag (+0x48) is clear it steps position (+0/+4) along heading (+0x2e) at
 * speed (+8), decelerates at the -0x2000 floor, and either snaps onto the
 * target (+0x1c/+0x20) within 2 units (flag++, timer reset, heading 0xc000)
 * or turns the heading toward atan2(dy,dx) in 0x100 steps. Arrived slots
 * orbit at radius +0x10 around (0x80000,0x60000) with angular speed +0x42.
 * When the last slot ((c)->func_ov002_020f5a94()-1) has orbited past 0x10000,
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
    int idx = i;
    char *px;
    char *s;
    int off = idx * 0x4c;
    {
        unsigned short v = *(unsigned short *)((char *)this + STAR_OFF(delay) + off);
        if (v != 0) {
            *(unsigned short *)((char *)this + STAR_OFF(delay) + off) = v - 1;
            return;
        }
    }
    if (*(unsigned char *)((char *)this + STAR_OFF(phase) + off) == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) += FX_MUL(tv, spd);
        }
        idxa = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) += FX_MUL(tv, spd);
        }
        px = (char *)this + off;
        if (*(int *)((char *)this + STAR_OFF(speed) + off) <= -0x2000)
            *(int *)((char *)this + STAR_OFF(speed) + off) =
                *(int *)((char *)this + STAR_OFF(speed) + off) + 0x120;
        {
            s = (char *)this + idx * 0x4c;
            int dx = (((Star *)s)->targetX - ((Star *)px)->x) >> 12;
            int dy = (((Star *)s)->targetY - *(int *)((char *)this + STAR_OFF(y) + off)) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                (*(unsigned char *)((char *)this + STAR_OFF(phase) + off))++;
                ((Star *)s)->orbitTravel = 0;
                *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)cstd::atan2(dy, dx);
                unsigned short cur = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
                if (a > cur) {
                    unsigned short nv;
                    *(unsigned short *)((char *)this + STAR_OFF(heading) + off) += 0x100;
                    nv = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
                    if (a <= nv)
                        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -= 0x100;
                    nv2 = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
                    if (nv2 <= a)
                        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = a;
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
        idxa = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }
        idxa = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
        *(int *)((char *)this + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
    }
    if (idx != func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000U)
        return;
    {
        int j;
        Star *p = this->stars;
        for (j = 0; j < 4; j++) {
            if (p->active != 0) {
                (p->step)++;
                p->loops = 0;
                p->phase = 0;
                p->radius = 0x38000;
                p->heading = data_ov002_021000b8[j];
                if (func_ov002_020f5a94() == 3) {
                    if (j == 2)
                        p->heading = 0;
                }
                p->orbitTravel = 0;
            }
            p++;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- func_ov002_020f4710, 0x020f4710, size 0x31c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f4710(int i) {
    int off = i * 0x4c;

    if (*(u8 *)((char *)this + STAR_OFF(phase) + off) == 0) {
        int idx;

        idx = *(u16 *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(s32 *)((char *)this + STAR_OFF(radius) + off);
            *(s32 *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }

        idx = *(u16 *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(s32 *)((char *)this + STAR_OFF(radius) + off);
            *(s32 *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(u16 *)((char *)this + STAR_OFF(heading) + off) -=
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off);
        *(s32 *)((char *)this + STAR_OFF(orbitTravel) + off) +=
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off);

        if (this->stars[0].phase != 0) {
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) += 8;
        } else if (*(u16 *)((char *)this + STAR_OFF(angSpeed) + off) < 0x200) {
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) += 6;
            if (*(u16 *)((char *)this + STAR_OFF(angSpeed) + off) > 0x200)
                *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) = 0x200;
        }

        if ((u32) * (s32 *)((char *)this + STAR_OFF(orbitTravel) + off) <
            (u32)data_ov002_02100100[i])
            return;
        if (this->holdOrbit != 0)
            return;

        (*(u8 *)((char *)this + STAR_OFF(phase) + off))++;
        *(u16 *)((char *)this + STAR_OFF(flyTimer) + off) = 0;

        if (i == 0 || i == 3) {
            *(s32 *)((char *)this + STAR_OFF(speed) + off) = 0x2000;
        } else {
            *(s32 *)((char *)this + STAR_OFF(speed) + off) = -0x2000;
        }

        if (func_ov002_020f5a94() != 3)
            return;
        if (i != 2)
            return;
        if (this->holdOrbit == 0)
            *(s32 *)((char *)this + STAR_OFF(speed) + off) = 0x2000;
        return;
    }

    (*(u16 *)((char *)this + STAR_OFF(flyTimer) + off))++;
    *(s32 *)((char *)this + STAR_OFF(x) + off) += *(s32 *)((char *)this + STAR_OFF(speed) + off);

    if (func_ov002_020f5a94() == 3 && i == 2) {
        *(s32 *)((char *)this + STAR_OFF(speed) + off) += 0x300;
    } else if (i == 0 || i == 3) {
        *(s32 *)((char *)this + STAR_OFF(speed) + off) += 0x300;
    } else {
        *(s32 *)((char *)this + STAR_OFF(speed) + off) -= 0x300;
    }

    if (this->stars[0].flyTimer < 0x60)
        return;

    {
        int o2 = (int)((unsigned long long)i) * 0x4c;
        (*(u8 *)((char *)this + STAR_OFF(step) + o2))++;
        *(u16 *)((char *)this + STAR_OFF(loops) + o2) = 0;
        *(u8 *)((char *)this + STAR_OFF(phase) + off) = 0;
        *(s32 *)((char *)this + STAR_OFF(radius) + o2) = 0x38000;
        *(u16 *)((char *)this + STAR_OFF(heading) + o2) = 0x8000;
        *(u16 *)((char *)this + STAR_OFF(delay) + o2) = (u16)(data_ov002_02100110[i] << 6);

        if (func_ov002_020f5a94() == 3) {
            if (i == 2)
                *(u16 *)((char *)this + STAR_OFF(delay) + o2) = 0x40;
        }
    }

    *(s32 *)((char *)this + STAR_OFF(x) + off) = 0x110000;
    *(s32 *)((char *)this + STAR_OFF(y) + off) = 0;
    *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) = 0x100;
    *(s32 *)((char *)this + STAR_OFF(speed) + off) = 0x5800;
    *(s32 *)((char *)this + STAR_OFF(targetX) + off) = 0x80000;
    *(s32 *)((char *)this + STAR_OFF(targetY) + off) = 0x28000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- func_ov002_020f4a2c, 0x020f4a2c, size 0x344 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f4a2c(int i) {
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = *(unsigned short *)((char *)this + STAR_OFF(delay) + off);
    if (v != 0) {
        *(unsigned short *)((char *)this + STAR_OFF(delay) + off) = v - 1;
        return;
    }

    if (*(unsigned char *)((char *)this + STAR_OFF(phase) + off) == 0) {
        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) += FX_MUL(tv, spd);
        }

        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) += FX_MUL(tv, spd);
        }

        if (*(int *)((char *)this + STAR_OFF(speed) + off) <= -0x2000)
            *(int *)((char *)this + STAR_OFF(speed) + off) =
                *(int *)((char *)this + STAR_OFF(speed) + off) + 0x120;

        dx = (*(int *)((char *)this + STAR_OFF(targetX) + off) -
              *(int *)((char *)this + STAR_OFF(x) + off)) >>
             12;
        dy = (*(int *)((char *)this + STAR_OFF(targetY) + off) -
              *(int *)((char *)this + STAR_OFF(y) + off)) >>
             12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)((char *)this + STAR_OFF(phase) + off) += 1;
            *(int *)((char *)this + STAR_OFF(orbitTravel) + off) = 0;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) += 0x100;
            nv = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            if (target <= nv)
                *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -= 0x100;
            nv2 = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            if (nv2 <= target)
                *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
            return;
        }
        return;
    }

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
        *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
    }

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
        *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
    }

    *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -=
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
    *(int *)((char *)this + STAR_OFF(orbitTravel) + off) +=
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);

    if (i != func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        Star *p = this->stars;
        for (j = 0; j < 4; j++) {
            if (p->active != 0) {
                p->step += 1;
                p->loops = 0;
                p->phase = 0;
                p->radius = 0x38000;
                p->heading = data_ov002_021000c8[j];
                if (func_ov002_020f5a94() == 3) {
                    if (j == 2)
                        p->heading = 0;
                }
                p->orbitTravel = 0;
            }
            p++;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_ov002_020f4d70, 0x020f4d70, size 0x2a0 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f4d70(int i) {
    int off = i * 0x4c;

    if (*(u16 *)((char *)this + STAR_OFF(frame) + off) == 0) {
        if (*(u16 *)((char *)this + STAR_OFF(frameTimer) + off) == 0)
            (*(u16 *)((char *)this + STAR_OFF(loops) + off))++;
    }

    if (*(u8 *)((char *)this + STAR_OFF(phase) + off) == 0) {
        int idx;

        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }

        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(u16 *)((char *)this + STAR_OFF(heading) + off) +=
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off);
        *(s32 *)((char *)this + STAR_OFF(orbitTravel) + off) +=
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off);

        if (this->stars[0].phase != 0) {
            *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) += 8;
        } else {
            if (*(u16 *)((char *)this + STAR_OFF(angSpeed) + off) < 0x200) {
                *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) += 6;
                if (*(u16 *)((char *)this + STAR_OFF(angSpeed) + off) > 0x200)
                    *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) = 0x200;
            }
        }

        if ((u32) * (s32 *)((char *)this + STAR_OFF(orbitTravel) + off) <
            (u32)data_ov002_021000e0[i])
            return;

        if (this->holdOrbit != 0)
            return;

        (*(u8 *)((char *)this + STAR_OFF(phase) + off))++;
        *(u16 *)((char *)this + STAR_OFF(flyTimer) + off) = 0;
        *(s32 *)((char *)this + STAR_OFF(speed) + off) = -0x2000;
        return;
    }

    (*(u16 *)((char *)this + STAR_OFF(flyTimer) + off))++;
    *(s32 *)((char *)this + STAR_OFF(x) + off) += *(s32 *)((char *)this + STAR_OFF(speed) + off);
    *(s32 *)((char *)this + STAR_OFF(speed) + off) -= 0x300;

    if (this->stars[0].flyTimer < 0x70)
        return;

    {
        int o2 = (unsigned)i * 0x4c;
        (*(u8 *)((char *)this + STAR_OFF(step) + o2))++;
        *(s32 *)((char *)this + STAR_OFF(radius) + o2) = 0x38000;
        *(u16 *)((char *)this + STAR_OFF(heading) + o2) = 0x8000;
        *(u8 *)((char *)this + STAR_OFF(phase) + off) = 0;
        *(u16 *)((char *)this + STAR_OFF(loops) + o2) = 0;
        *(u16 *)((char *)this + STAR_OFF(delay) + o2) = data_ov002_021000f0[i] << 6;

        if (func_ov002_020f5a94() == 3) {
            if (i == 2)
                *(u16 *)((char *)this + STAR_OFF(delay) + o2) = 0x40;
        }
    }

    *(s32 *)((char *)this + STAR_OFF(x) + off) = 0x110000;
    *(s32 *)((char *)this + STAR_OFF(y) + off) = 0;
    *(u16 *)((char *)this + STAR_OFF(angSpeed) + off) = 0x100;
    *(s32 *)((char *)this + STAR_OFF(speed) + off) = 0x5800;
    *(s32 *)((char *)this + STAR_OFF(targetX) + off) = 0x80000;
    *(s32 *)((char *)this + STAR_OFF(targetY) + off) = 0x28000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- func_ov002_020f5010, 0x020f5010, size 0x318 */
/* -------------------------------------------------------------------------- */
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
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = *(unsigned short *)((char *)this + STAR_OFF(delay) + off);
    if (v != 0) {
        *(unsigned short *)((char *)this + STAR_OFF(delay) + off) = v - 1;
        return;
    }

    if (*(unsigned char *)((char *)this + STAR_OFF(phase) + off) == 0) {
        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) += FX_MUL(tv, spd);
        }

        idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)((char *)this + STAR_OFF(speed) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) += FX_MUL(tv, spd);
        }

        if (*(int *)((char *)this + STAR_OFF(speed) + off) >= 0x2000)
            *(int *)((char *)this + STAR_OFF(speed) + off) =
                *(int *)((char *)this + STAR_OFF(speed) + off) - 0x120;

        dx = (*(int *)((char *)this + STAR_OFF(targetX) + off) -
              *(int *)((char *)this + STAR_OFF(x) + off)) >>
             12;
        dy = (*(int *)((char *)this + STAR_OFF(targetY) + off) -
              *(int *)((char *)this + STAR_OFF(y) + off)) >>
             12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)((char *)this + STAR_OFF(phase) + off) += 1;
            *(int *)((char *)this + STAR_OFF(orbitTravel) + off) = 0;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) += 0x100;
            nv = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            if (target <= nv)
                *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)((char *)this + STAR_OFF(heading) + off) -= 0x100;
            nv2 = *(unsigned short *)((char *)this + STAR_OFF(heading) + off);
            if (nv2 <= target)
                *(unsigned short *)((char *)this + STAR_OFF(heading) + off) = target;
            return;
        }
        return;
    }

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
        *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
    }

    idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
        *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
    }

    *(unsigned short *)((char *)this + STAR_OFF(heading) + off) +=
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
    *(int *)((char *)this + STAR_OFF(orbitTravel) + off) +=
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);

    if (i != func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)this->stars[0].orbitTravel < 0x10000)
        return;

    {
        int j;
        Star *p = this->stars;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- func_ov002_020f5328, 0x020f5328, size 0x28c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5328(int i) {
    int off = i * 0x4c;

    if (*(unsigned char *)((char *)this + STAR_OFF(phase) + off) == 0) {
        int idx;
        {
            idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }
        {
            idx = *(unsigned short *)((char *)this + STAR_OFF(heading) + off) >> 4;
            short tv = data_02082214[idx * 2];
            int spd = *(int *)((char *)this + STAR_OFF(radius) + off);
            *(int *)((char *)this + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(unsigned short *)((char *)this + STAR_OFF(heading) + off) +=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);
        *(int *)((char *)this + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off);

        if (this->stars[0].phase != 0) {
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) += 8;
        } else if (*(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) < 0x200) {
            *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) += 6;
            if (*(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) > 0x200)
                *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off) = 0x200;
        }

        if ((unsigned int)*(int *)((char *)this + STAR_OFF(orbitTravel) + off) <
            (unsigned int)data_ov002_02100150[i])
            return;
        if (this->holdOrbit != 0)
            return;

        *(unsigned char *)((char *)this + STAR_OFF(phase) + off) += 1;
        *(unsigned short *)((char *)this + STAR_OFF(flyTimer) + off) = 0;

        if (i == 1 || i == 3) {
            *(int *)((char *)this + STAR_OFF(speed) + off) = 0x2000;
        } else {
            *(int *)((char *)this + STAR_OFF(speed) + off) = -0x2000;
        }
        return;
    }

    *(unsigned short *)((char *)this + STAR_OFF(flyTimer) + off) += 1;
    *(int *)((char *)this + STAR_OFF(x) + off) += *(int *)((char *)this + STAR_OFF(speed) + off);
    if (i == 1 || i == 3) {
        *(int *)((char *)this + STAR_OFF(speed) + off) += 0x300;
    } else {
        *(int *)((char *)this + STAR_OFF(speed) + off) -= 0x300;
    }

    if (this->stars[0].flyTimer < 0x60)
        return;

    {
        int off2 = (int)((unsigned long long)i) * 0x4c;
        *(unsigned char *)((char *)this + STAR_OFF(step) + off2) += 1;
        *(unsigned short *)((char *)this + STAR_OFF(loops) + off2) = 0;
        *(unsigned char *)((char *)this + STAR_OFF(phase) + off) = 0;
        *(int *)((char *)this + STAR_OFF(radius) + off2) = 0x38000;
        *(unsigned short *)((char *)this + STAR_OFF(heading) + off2) = 0;
        *(unsigned short *)((char *)this + STAR_OFF(delay) + off2) =
            (unsigned short)(data_ov002_02100160[i] << 6);
        *(int *)((char *)this + STAR_OFF(x) + off) = -0x10000;
        *(int *)((char *)this + STAR_OFF(y) + off2) = 0;
        *(unsigned short *)((char *)this + STAR_OFF(angSpeed) + off2) = 0x100;
        *(int *)((char *)this + STAR_OFF(speed) + off) = 0x5800;
        *(int *)((char *)this + STAR_OFF(targetX) + off2) = 0x80000;
        *(int *)((char *)this + STAR_OFF(targetY) + off2) = 0x28000;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov002_020f55b4, 0x020f55b4, size 0x78 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f55b4(int i) {
    Star *e = (Star *)((char *)this + i * 0x4c);
    if (e->frame != 0x10)
        return;
    *(unsigned char *)((char *)this + STAR_OFF(step) + i * 0x4c) += 1;
    e->radius = 0x38000;
    e->heading = data_ov002_021000d8[i];
    e->phase = 0;
    e->loops = 0;
    e->orbitTravel = 0;
    e->angSpeed = 0x200;
    e->kiraDefParam = 0x1000;
    e->kiraSysParam = 0x200;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- func_ov002_020f562c, 0x020f562c, size 0x4c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f562c(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110eec[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov002_020f5678, 0x020f5678, size 0x24 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5678(int i) {
    Star *e = &this->stars[i];
    e->kiraDefParam = 0xccc;
    e->kiraSysParam = 0x155;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- func_ov002_020f569c, 0x020f569c, size 0x124 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f569c(int i) {
    char *p = (char *)this;
    int ax;
    int by;
    int tx;
    int ty;
    int idx;

    idx = i * 0x4c;

    if (*(unsigned short *)(p + STAR_OFF(delay) + idx) != 0) {
        *(unsigned short *)(p + STAR_OFF(delay) + idx) =
            *(unsigned short *)(p + STAR_OFF(delay) + idx) - 1;
        return;
    }

    *(int *)(p + idx + STAR_OFF(kiraDefParam)) = 0x1000;
    *(short *)(p + idx + STAR_OFF(kiraSysParam)) = 0x200;

    *(int *)(p + STAR_OFF(x) + idx) =
        *(int *)(p + STAR_OFF(x) + idx) + *(int *)(p + STAR_OFF(speed) + idx);
    *(int *)(p + STAR_OFF(y) + idx) =
        *(int *)(p + STAR_OFF(y) + idx) + *(int *)(p + STAR_OFF(speedY) + idx);

    tx = data_ov002_02100190[i * 2];
    ty = data_ov002_02100190[i * 2 + 1];
    ax = *(int *)(p + STAR_OFF(x) + idx) >> 12;
    by = *(int *)(p + STAR_OFF(y) + idx) >> 12;

    if (*(int *)(p + STAR_OFF(speed) + idx) > 0) {
        if (ax > tx)
            *(int *)(p + STAR_OFF(x) + idx) = tx << 12;
    } else {
        if (ax < tx)
            *(int *)(p + STAR_OFF(x) + idx) = tx << 12;
    }

    if (*(int *)(p + STAR_OFF(speedY) + idx) > 0) {
        if (by > ty)
            *(int *)(p + STAR_OFF(y) + idx) = ty << 12;
    } else {
        if (by < ty)
            *(int *)(p + STAR_OFF(y) + idx) = ty << 12;
    }

    ax = *(int *)(p + STAR_OFF(x) + idx) >> 12;
    by = *(int *)(p + STAR_OFF(y) + idx) >> 12;

    if (ax == tx && by == ty) {
        *(unsigned char *)(p + STAR_OFF(step) + idx) =
            *(unsigned char *)(p + STAR_OFF(step) + idx) + 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov002_020f57c0, 0x020f57c0, size 0x88 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f57c0(int idx) {
    if (stars[idx].frame != 0)
        return;
    if (stars[idx].frameTimer != 0)
        return;
    stars[idx].sparkling = 1;
    stars[idx].visible = 1;
    stars[idx].step++;
    stars[idx].speed = data_ov002_02100170[idx * 2];
    stars[idx].speedY = data_ov002_02100170[idx * 2 + 1];
    if (idx == 0)
        stars[idx].delay = 0x78;
    else
        stars[idx].delay = 0x20;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov002_020f5848, 0x020f5848, size 0x4c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5848(int idx) {
    unsigned char sel = this->stars[idx].step;
    (this->*data_ov002_02110e64[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- func_ov002_020f5894, 0x020f5894, size 0xfc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5894(int i) {
    int off;
    unsigned short *p34;
    short s2c;
    if (i != 0)
        return;
    off = i * 0x4c;
    p34 = (unsigned short *)((char *)this + STAR_OFF(kiraTimer) + off);
    *p34 = *p34 + 1;
    if (*(int *)((char *)this + STAR_OFF(kiraDefParam) + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int *)((char *)this + STAR_OFF(kiraDefParam) + off) =
                *(int *)((char *)this + STAR_OFF(kiraDefParam) + off) + 0x199;
            if (*(int *)((char *)this + STAR_OFF(kiraDefParam) + off) >= 0x1000)
                *(int *)((char *)this + STAR_OFF(kiraDefParam) + off) = 0x1000;
        }
    }
    s2c = *(short *)((char *)this + STAR_OFF(kiraSysParam) + off);
    if (s2c != 0x800) {
        if (*(unsigned short *)((char *)this + off + STAR_OFF(kiraPeriods)) >= 5) {
            if ((*p34 & 3) == 3) {
                *(short *)((char *)this + STAR_OFF(kiraSysParam) + off) = s2c + 0x199;
                if (*(short *)((char *)this + STAR_OFF(kiraSysParam) + off) >= 0x800)
                    *(short *)((char *)this + STAR_OFF(kiraSysParam) + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e)
        return;
    *p34 = 0;
    {
        unsigned short *p3e = (unsigned short *)((char *)this + STAR_OFF(kiraPeriods) + off);
        *p3e = *p3e + 1;
    }
}
#undef STAR_OFF
#undef WINDOW_OFF
#undef CREDIT_OFF

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov002_020f5990, 0x020f5990, size 0xdc */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- func_ov002_020f5a6c, 0x020f5a6c, size 0x28 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- func_ov002_020f5a94, 0x020f5a94, size 0x40 */
/* -------------------------------------------------------------------------- */
int StaffRoll::func_ov002_020f5a94() {
    unsigned char count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (SaveData::IsCharacterUnlocked(i))
            count = count + 1;
    }
    return count;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- func_ov002_020f5ad4, 0x020f5ad4, size 0x50 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- func_ov002_020f5b24, 0x020f5b24, size 0x58 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b24() {
    gSubScreenLayers &= ~6;
    gSubScreenLayers |= 8;
    func_ov002_020f2f64();
    *(volatile unsigned int *)0x4001000 &= ~0xe000;
    this->window[0].active = 0;
    func_ov002_020f2a48();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- func_ov002_020f5b7c, 0x020f5b7c, size 0x4 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b7c() {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- func_ov002_020f5b80, 0x020f5b80, size 0x4 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b80() {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- func_ov002_020f5b84, 0x020f5b84, size 0x4 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b84() {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- func_ov002_020f5b88, 0x020f5b88, size 0x4 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b88() {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- func_ov002_020f5b8c, 0x020f5b8c, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b8c() {
    this->holdOrbit = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- func_ov002_020f5b98, 0x020f5b98, size 0x34 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5b98() {
    StaffRoll::Star *e = stars;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- func_ov002_020f5bcc, 0x020f5bcc, size 0x28 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5bcc() {
    Star *p = this->stars;
    int i;
    for (i = 0; i < 4; i++) {
        if (p->active)
            p->holdFrame = 1;
        p++;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 81 -- func_ov002_020f5bf4, 0x020f5bf4, size 0x4c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 82 -- func_ov002_020f5c40, 0x020f5c40, size 0x48 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 83 -- func_ov002_020f5c88, 0x020f5c88, size 0x48 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 84 -- func_ov002_020f5cd0, 0x020f5cd0, size 0x64 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5cd0() {
    StaffRoll::Star *c = stars;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 85 -- func_ov002_020f5d34, 0x020f5d34, size 0xa4 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5d34() {
    int n = func_ov002_020f5a94();
    int i = 0;
    Star *e;
    int v;
    if (n <= 0)
        goto done;
    e = this->stars;
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
    func_ov002_020f2958();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 86 -- func_ov002_020f5dd8, 0x020f5dd8, size 0x40 */
/* -------------------------------------------------------------------------- */
extern PMF0 data_ov002_02110f34[];
void StaffRoll::func_ov002_020f5dd8(int idx) {
    (this->*data_ov002_02110f34[idx])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 87 -- func_ov002_020f5e18, 0x020f5e18, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5e18() {
    func_ov002_020f37fc();
    func_ov002_020f2f9c(4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 88 -- func_ov002_020f5e38, 0x020f5e38, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5e38() {
    func_ov002_020f3828();
    func_ov002_020f2f9c(4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 89 -- func_ov002_020f5e58, 0x020f5e58, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5e58() {
    func_ov002_020f3828();
    func_ov002_020f2f9c(3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 90 -- func_ov002_020f5e78, 0x020f5e78, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5e78() {
    func_ov002_020f3828();
    func_ov002_020f2f9c(2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 91 -- func_ov002_020f5e98, 0x020f5e98, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5e98() {
    func_ov002_020f3828();
    func_ov002_020f2f9c(1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 92 -- func_ov002_020f5eb8, 0x020f5eb8, size 0x20 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5eb8() {
    func_ov002_020f3828();
    func_ov002_020f2f9c(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 93 -- func_ov002_020f5ed8, 0x020f5ed8, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5ed8() {
    func_ov002_020f3828();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 94 -- func_ov002_020f5ee4, 0x020f5ee4, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5ee4() {
    func_ov002_020f3828();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 95 -- func_ov002_020f5ef0, 0x020f5ef0, size 0xc */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5ef0() {
    func_ov002_020f3954();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 96 -- func_ov002_020f5efc, 0x020f5efc, size 0x10 */
/* -------------------------------------------------------------------------- */
/* func_ov002_020f5efc @ 0x20f5efc (ov002) -- tail-call veneer to LoadFont (0x201fcd4) with r0=0.
 */
void StaffRoll::func_ov002_020f5efc() {
    LoadFont(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 97 -- func_ov002_020f5f0c, 0x020f5f0c, size 0x54 */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5f0c(int idx) {
    (this->*data_ov002_02110f9c[idx])();
    this->StartCredit(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 98 -- func_ov002_020f5f60, 0x020f5f60, size 0x2c */
/* -------------------------------------------------------------------------- */
void StaffRoll::func_ov002_020f5f60() {
    void *p = this->starChars;
    if (p == 0)
        return;
    Memory::operator_delete2(p);
    this->starChars = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 99 -- func_ov002_020f5f8c, 0x020f5f8c, size 0x2c */
/* -------------------------------------------------------------------------- */
int StaffRoll::func_ov002_020f5f8c() {
    this->func_ov002_020f26e0();
    func_ov002_020f3978();
    func_ov002_020f2eac();
    return func_ov002_020f2990();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 100 -- func_ov002_020f5fb8, 0x020f5fb8, size 0x2c */
/* -------------------------------------------------------------------------- */
int StaffRoll::func_ov002_020f5fb8() {
    func_ov002_020f5990();
    func_ov002_020f37a0();
    func_ov002_020f2dd4();
    return func_ov002_020f27e8();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 101 -- func_ov002_020f5fe4, 0x020f5fe4, size 0x3bc */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 102 -- func_ov002_020f63a0, 0x020f63a0, size 0x34 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f63a0() {
    this->mStaffRoll->func_ov002_020f5f60();
    if (this->mStaffRoll != 0) {
        Memory::operator_delete2(this->mStaffRoll);
        this->mStaffRoll = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 103 -- func_ov002_020f63d4, 0x020f63d4, size 0x24 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f63d4() {
    this->mStaffRoll->func_ov002_020f5fb8();
    this->mStaffRoll->func_ov002_020f5f8c();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 104 -- func_ov002_020f63f8, 0x020f63f8, size 0x2c */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f63f8(unsigned char *src) {
    this->mCue = *src;
    this->mStaffRoll->func_ov002_020f5dd8((int)this->mCue);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 105 -- func_ov002_020f6424, 0x020f6424, size 0x24 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6424() {
    this->mStaffRoll->func_ov002_020f5f0c((int)this->mCue);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 106 -- func_ov002_020f6448, 0x020f6448, size 0x64 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6448(unsigned char *arg1) {
    StaffRoll *p;
    this->mCue = *arg1;
    p = new StaffRoll;
    this->mStaffRoll = p;
    if (this->mStaffRoll == 0)
        return 0;
    this->mStaffRoll->func_ov002_020f5fe4();
    return func_ov002_020f6424();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 107 -- func_ov002_020f64ac, 0x020f64ac, size 0x68 */
/* -------------------------------------------------------------------------- */

void daDemo_c::anmModel_c::func_ov002_020f64ac(char *r4) {
    Advance();
    if (!Finished())
        return;
    signed char r2 = *(signed char *)(r4 + (mAnimEntry << 2) + 3);
    if (r2 < 0)
        return;
    func_ov002_020f6514((u8 *)r4, (u8)(r2 & 0xff));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 108 -- func_ov002_020f6514, 0x020f6514, size 0xa4 */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 109 -- func_ov002_020f65b8, 0x020f65b8, size 0x34 */
/* -------------------------------------------------------------------------- */

void daDemo_c::anmModel_c::func_ov002_020f65b8() {
    if (mTexSeq == 0)
        return;
    mTexSeq->Update(data);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 110 -- func_ov002_020f65ec, 0x020f65ec, size 0x2c */
/* -------------------------------------------------------------------------- */

void daDemo_c::anmModel_c::func_ov002_020f65ec() {
    if (mTexSeq != 0)
        mTexSeq->Advance();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 111 -- func_ov002_020f6618, 0x020f6618, size 0x160 */
/* -------------------------------------------------------------------------- */
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

/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 114 -- func_ov002_020f6960, 0x020f6960, size 0x48 */
/* -------------------------------------------------------------------------- */

int daDemo_c::simpleModel_c::func_ov002_020f6960(SharedFilePtr *fp, int n) {
    mFile = fp;
    BMD_File *f = (BMD_File *)dExtSimpleModel_c::LoadFile(*mFile);
    return ((dExtModel_c *)this)->SetFile(f, 1, n) != 0 ? 1 : 0;
}

/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
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
daDemo_c::simpleModel_c::~simpleModel_c() {
    SharedFilePtr *file = mFile;
    if (file != 0) {
        file->Release();
    }
}
#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 117 -- func_ov002_020f6a50, 0x020f6a50, size 0x4c */
/* -------------------------------------------------------------------------- */

daDemo_c::param_c::param_c() {
    mScale[0].x = 0;
    mScale[0].y = 0;
    mScale[0].z = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 118 -- func_ov002_020f6a9c, 0x020f6a9c, size 0x1c */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6a9c() {
    MarkForDestruction();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 119 -- func_ov002_020f6ab8, 0x020f6ab8, size 0x2c */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6ab8(unsigned char *p) {
    unsigned int id = ReadUnalignedInt(p);
    Sound::Play(1, id, mCamSpacePos);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 120 -- func_ov002_020f6ae4, 0x020f6ae4, size 0x44 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6ae4(unsigned char *p) {
    unsigned int id = ReadUnalignedInt(p);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, id, mCamSpacePos, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 121 -- func_ov002_020f6b28, 0x020f6b28, size 0x24 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6b28(unsigned char *p) {
    unsigned int v = ReadUnalignedInt(p);
    func_02012694(v, &mCamSpacePosX);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 122 -- func_ov002_020f6b4c, 0x020f6b4c, size 0x74 */
/* -------------------------------------------------------------------------- */
/* Script handler 15: turn to face the point whose whole-unit coordinates
 * are the unaligned shorts at p+0/+2/+4, and take the short at p+6 as the
 * horizontal speed. The legacy per-function file recorded its match under
 * mwccarm 1.2/sp2p3; this TU builds under the 2004/b56 pin. */

int daDemo_c::func_ov002_020f6b4c(unsigned char *p) {
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 123 -- func_ov002_020f6bc0, 0x020f6bc0, size 0x64 */
/* -------------------------------------------------------------------------- */

s32 daDemo_c::func_ov002_020f6bc0(unsigned char *data) {
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 124 -- func_ov002_020f6c24, 0x020f6c24, size 0x10 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6c24(unsigned char *src) {
    this->mOpacity = src[0];
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 125 -- func_ov002_020f6c34, 0x020f6c34, size 0x2c */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f6c34(unsigned char *src) {
    unsigned char v = *src;
    ((anmModel_c *)this->mAnmModel)->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, v);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 126 -- func_ov002_020f6c60, 0x020f6c60, size 0x1e8 */
/* -------------------------------------------------------------------------- */
inline unsigned int inline_fn(unsigned int arg0) {
    return arg0;
}

int daDemo_c::func_ov002_020f6c60(void *arg1, int arg2, int arg3) {
    u8 *flag = (u8 *)arg1;
    VecFx32 v;
    if (arg2 == gKuppaScriptFrame) {
        if ((*flag) != 0) {
            int r = (inline_fn(RandomIntInternal(&data_0209e650)) >> 20) & 0xfff;
            int m = FX_MUL(r, 0xa000);
            this->mMagnitude = m + 0x19000;
        }
        AddVec3((Vector3 *)&this->mPosX, &((dCamera_c *)gCamera)->pos,
                (Vector3 *)&this->mPosX);
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
    if ((*flag) == 0) {
        /* First call stores the look-at point and registers ~Vector3(). */
        static __attribute__((aligned(8))) Vector3 sLookTarget(0xffdd6000, 0xbe4000, 0x2ae000);
        func_ov002_020f6f48(&sLookTarget, 0x20);
    } else {
        MulVec3Mat4x3((Vector3 *)&data_ov002_0210b958, (Matrix4x3 *)&data_0209b41c, (Vector3 *)&v);
        Vec3_LslInPlace((int *)&v, 3);
        func_ov002_020f6f48((Vector3 *)&v, 8);
    }
    this->mPrevAngleX = this->mAngleX;
    this->mPrevAngleY = this->mAngleY;
    this->mPrevAngleZ = this->mAngleZ;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 127 -- func_ov002_020f6e48, 0x020f6e48, size 0x100 */
/* -------------------------------------------------------------------------- */
/* Script handler 10: position (p[0..5], whole units shifted into 20.12) and
 * angles (p[6..11]) from the record, mMagnitude = p[12] << 12, scale 0xb33, and
 * the animation's currFrame set from a random draw scaled by
 * numFramesAndFlags. */

int daDemo_c::func_ov002_020f6e48(unsigned char *p) {
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 128 -- func_ov002_020f6f48, 0x020f6f48, size 0xd8 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 129 -- func_ov002_020f7020, 0x020f7020, size 0x18 */
/* -------------------------------------------------------------------------- */
/* Script handler 9: clear mFlags bit 0x2. */

int daDemo_c::func_ov002_020f7020() {
    this->mFlags &= ~0x2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 130 -- func_ov002_020f7038, 0x020f7038, size 0x18c */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f7038(int a, int arg) {
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 131 -- func_ov002_020f71c4, 0x020f71c4, size 0xf8 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f71c4(int a, int arg) {
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 132 -- func_ov002_020f72bc, 0x020f72bc, size 0xc8 */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 133 -- func_ov002_020f7384, 0x020f7384, size 0x8c */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 134 -- func_ov002_020f7410, 0x020f7410, size 0x128 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 135 -- func_ov002_020f7538, 0x020f7538, size 0x248 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 136 -- func_ov002_020f7780, 0x020f7780, size 0x240 */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 137 -- func_ov002_020f79c0, 0x020f79c0, size 0x1f8 */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f79c0(void *unused, int mode) {

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
        t = data_02082214[(h >> 4) * 2 + 1];
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 138 -- func_ov002_020f7bb8, 0x020f7bb8, size 0x1bc */
/* -------------------------------------------------------------------------- */

int daDemo_c::func_ov002_020f7bb8(unsigned char *p, u16 id) {
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

    Vec3_RotateYAndTranslate((int *)(&this->mPosX), (int *)&cam->pos, heading, src);

    this->mAngleX = cstd::atan2(v1.y, Vec3_HorzLen(&v1));
    this->mAngleY = cstd::atan2(v1.x, v1.z);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 139 -- daDemo_c::RunCommand, 0x020f7d74, size 0x2b4 */
/* -------------------------------------------------------------------------- */
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
    /* The guards are compiled in: bare deletes build 0x70, not the ROM's 0x80. */
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

/* This TU's own SharedFilePtr wrappers: their externs sit in decl_common.h
   above and their object definitions with __sinit's below. Declared here,
   not earlier, because a local type before func_ov002_020f6c60 would shift
   the compiler-numbered suffix on that function's sLookTarget static. */
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

/* Each new-expression inlines the nested class's implicit construction:
 * param_c's constructor on the tail base, the model base's C2, then the
 * class vptr and, for anmModel_c, the dExtFrameCtrl_c base's secondary vptr
 * at +0x50. The model base sits at offset 0, so storing the result in
 * mModel/mAnmModel needs no null test. */
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

/* Reconstructed source-style name: SM64DS proves daDemo_c through RTTI,
 * allocation size, vtable identity, and the DEMO registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: CutsceneObject_Spawn.
 *
 * `new daDemo_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x104), dActor_c's base constructor, then the vptr
 * store. */
extern "C" daDemo_c *daDemo_c_classInit(void) {
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
extern "C" {
/* Each record is the 8-byte {address, 0} pointer-to-member value the ROM
   stores in .data: a non-virtual member of a class with no base offset
   encodes as its function address plus a zero this-adjustment. The
   definitions name the member directly, so the word carries the address
   without a mangled spelling. */

extern int _ZN8daDemo_c19func_ov002_020f79c0EPvi(daDemo_c *, void *,
                                                 int); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f7bb8EPht(daDemo_c *, unsigned char *,
                                      unsigned short); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6b28EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int _ZN8daDemo_c19func_ov002_020f71c4Eii(daDemo_c *, int,
                                                int); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f26c4EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f63f8EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6448EPh(daDemo_c *,
                                     unsigned char *);      /* local extern: descriptor address */
extern int _ZN8daDemo_c19func_ov002_020f6a9cEv(daDemo_c *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6ab8EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6ae4EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int _ZN8daDemo_c19func_ov002_020f7038Eii(daDemo_c *, int,
                                                int); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6b4cEPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6bc0EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6c24EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6c34EPh(daDemo_c *,
                                     unsigned char *); /* local extern: descriptor address */
extern int _ZN8daDemo_c19func_ov002_020f6c60EPvii(daDemo_c *, void *, int,
                                                  int); /* local extern: descriptor address */
extern int
_ZN8daDemo_c19func_ov002_020f6e48EPh(daDemo_c *,
                                     unsigned char *);      /* local extern: descriptor address */
extern int _ZN8daDemo_c19func_ov002_020f7020Ev(daDemo_c *); /* local extern: descriptor address */
extern void
_ZN8daDemo_c9StaffRoll19func_ov002_020f5b24Ev(StaffRoll *); /* local extern: descriptor address */

int data_ov002_0210b658[2] = {(int)0x81f020f0, (int)0xffff0100};
int data_ov002_0210b6e8[2] = {(int)0x81f020f0, (int)0xffff0118};
int data_ov002_0210b768[2] = {(int)0x41f800f8, (int)0xffff701e};
int data_ov002_0210b868[2] = {(int)0x41f800f8, (int)0xffff705e};
struct OamAttr *data_ov002_0210b6a8[2] = {(struct OamAttr *)data_ov002_0210b768,
                                          (struct OamAttr *)data_ov002_0210b868};

W data_ov002_0210b650 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b8cEv, 0};
W data_ov002_0210b660 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e58Ev, 0};
W data_ov002_0210b668 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5894Ei, 0};
W data_ov002_0210b670 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e58Ev, 0};
W data_ov002_0210b678 = {(int)_ZN8daDemo_c19func_ov002_020f79c0EPvi, 0};
W data_ov002_0210b680 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5c40Ev, 0};
W data_ov002_0210b688 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e98Ev, 0};
W data_ov002_0210b690 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5ef0Ev, 0};
W data_ov002_0210b698 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e98Ev, 0};
W data_ov002_0210b6a0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5ee4Ev, 0};
W data_ov002_0210b6b0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f35e4Ei, 0};
W data_ov002_0210b6b8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5cd0Ev, 0};
W data_ov002_0210b6c0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3310Ei, 0};
W data_ov002_0210b6c8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5bccEv, 0};
W data_ov002_0210b6d0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b98Ev, 0};
W data_ov002_0210b6d8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3ba0Ei, 0};
W data_ov002_0210b6e0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2a78Ei, 0};
W data_ov002_0210b6f0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2a88Ei, 0};
W data_ov002_0210b6f8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2aecEi, 0};
W data_ov002_0210b700 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3d98Ei, 0};
W data_ov002_0210b708 = {(int)_ZN8daDemo_c19func_ov002_020f7bb8EPht, 0};
W data_ov002_0210b710 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5328Ei, 0};
W data_ov002_0210b718 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5efcEv, 0};
W data_ov002_0210b720 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5bf4Ev, 0};
W data_ov002_0210b728 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f40fcEi, 0};
W data_ov002_0210b730 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f4710Ei, 0};
W data_ov002_0210b738 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f4a2cEi, 0};
W data_ov002_0210b740 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f4d70Ei, 0};
W data_ov002_0210b748 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5010Ei, 0};
W data_ov002_0210b750 = {(int)_ZN8daDemo_c19func_ov002_020f6b28EPh, 0};
W data_ov002_0210b758 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f55b4Ei, 0};
W data_ov002_0210b760 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f30f8Ei, 0};
W data_ov002_0210b770 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f32e4Ei, 0};
W data_ov002_0210b778 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5678Ei, 0};
W data_ov002_0210b780 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f569cEi, 0};
W data_ov002_0210b788 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f57c0Ei, 0};
W data_ov002_0210b790 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2d70Ei, 0};
W data_ov002_0210b798 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f335cEi, 0};
W data_ov002_0210b7a0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f340cEi, 0};
W data_ov002_0210b7a8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5848Ei, 0};
W data_ov002_0210b7b0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f34bcEi, 0};
W data_ov002_0210b7b8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b24Ev, 0};
W data_ov002_0210b7c0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b7cEv, 0};
W data_ov002_0210b7c8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b88Ev, 0};
W data_ov002_0210b7d0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3740Ei, 0};
W data_ov002_0210b7d8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f378cEi, 0};
W data_ov002_0210b7e0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e38Ev, 0};
W data_ov002_0210b7e8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5c88Ev, 0};
W data_ov002_0210b7f0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5eb8Ev, 0};
W data_ov002_0210b7f8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5d34Ev, 0};
W data_ov002_0210b800 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3ae8Ei, 0};
W data_ov002_0210b808 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e38Ev, 0};
W data_ov002_0210b810 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3d38Ei, 0};
W data_ov002_0210b818 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b84Ev, 0};
W data_ov002_0210b820 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5b80Ev, 0};
W data_ov002_0210b828 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e38Ev, 0};
W data_ov002_0210b830 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e98Ev, 0};
W data_ov002_0210b838 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e18Ev, 0};
W data_ov002_0210b840 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e78Ev, 0};
W data_ov002_0210b848 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f562cEi, 0};
W data_ov002_0210b850 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5ed8Ev, 0};
W data_ov002_0210b858 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e58Ev, 0};
W data_ov002_0210b860 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e78Ev, 0};
W data_ov002_0210b870 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2bf4Ei, 0};
W data_ov002_0210b878 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f2d0cEi, 0};
W data_ov002_0210b880 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e78Ev, 0};
W data_ov002_0210b890 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f3de4Ei, 0};
W data_ov002_0210b898 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e98Ev, 0};
W data_ov002_0210b8a0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e78Ev, 0};
W data_ov002_0210b8a8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e38Ev, 0};
W data_ov002_0210b8b0 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f43ccEi, 0};
W data_ov002_0210b8b8 = {(int)_ZN8daDemo_c9StaffRoll19func_ov002_020f5e58Ev, 0};
W data_ov002_0210b8c0 = {(int)_ZN8daDemo_c19func_ov002_020f71c4Eii, 0};
W data_ov002_0210b8c8 = {(int)_ZN8daDemo_c19func_ov002_020f26c4EPh, 0};
W data_ov002_0210b8d0 = {(int)func_ov002_020f26d4, 0};
W data_ov002_0210b8d8 = {(int)_ZN8daDemo_c19func_ov002_020f63f8EPh, 0};
W data_ov002_0210b8e0 = {(int)_ZN8daDemo_c19func_ov002_020f6448EPh, 0};
W data_ov002_0210b8e8 = {(int)_ZN8daDemo_c19func_ov002_020f6a9cEv, 0};
W data_ov002_0210b8f0 = {(int)_ZN8daDemo_c19func_ov002_020f6ab8EPh, 0};
W data_ov002_0210b8f8 = {(int)_ZN8daDemo_c19func_ov002_020f6ae4EPh, 0};
W data_ov002_0210b900 = {(int)_ZN8daDemo_c19func_ov002_020f7038Eii, 0};
W data_ov002_0210b908 = {(int)_ZN8daDemo_c19func_ov002_020f6b4cEPh, 0};
W data_ov002_0210b910 = {(int)_ZN8daDemo_c19func_ov002_020f6bc0EPh, 0};
W data_ov002_0210b918 = {(int)_ZN8daDemo_c19func_ov002_020f6c24EPh, 0};
W data_ov002_0210b920 = {(int)_ZN8daDemo_c19func_ov002_020f6c34EPh, 0};
W data_ov002_0210b928 = {(int)_ZN8daDemo_c19func_ov002_020f6c60EPvii, 0};
W data_ov002_0210b930 = {(int)_ZN8daDemo_c19func_ov002_020f6e48EPh, 0};
W data_ov002_0210b938 = {(int)_ZN8daDemo_c19func_ov002_020f7020Ev, 0};

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
