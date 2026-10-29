//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
/* daDemo_c -- the KuppaScript cutscene actor, registry profile 352 (0x160),
 * debug name DEMO.
 *
 * How it is driven. ProcessKuppaScript walks the current script's records
 * (s[0] record length, s[1] command, s[2..3] start frame, s[4..5] end frame,
 * compared against gKuppaScriptFrame). Commands 0x12 and up go to
 * RunDemoCommand: the command byte is used as a param1. FindDemoActor looks
 * for a live actor with that param1 among the 16 IDs at gDemoActorIDs; if
 * none, SpawnDemoActor spawns actor 0x160 with that param1 into a free slot.
 * func_ov002_020f7d74 then calls entry s[6] of the 24-entry daDemo_c::*
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
 *   func_ov002_020f20f4 compiles to the ROM's loop only as C++ (every
 *   C-flavored spelling walks a byte-offset induction variable).
 *   func_ov002_020f2aec/2bf4/335c/340c/39ec keep opt_common_subs off
 *   brackets; 20f4 keeps opt_strength_reduction / opt_loop_invariants
 *   off. The brackets bind per-member only under defer_codegen off.
 *   func_ov002_020f27e8 and the OAM tail family return a callee's r0 by
 *   falling off the end; the callees are declared int so the passthrough
 *   is spelled.
 *   func_ov002_020f7d74, the dispatcher, stays extern "C" and free: its
 *   caller RunDemoCommand (src/KuppaScript.cpp) binds it by that name.
 *   func_ov002_020f26d4 is a ldr/bx veneer that tail-calls
 *   func_ov002_020f2630 through a raw void(*)(void); a member pointer
 *   cannot spell that cast, so both stay free.
 *   The mStaffRoll heap block is daDemo_c::StaffRoll, a record recovered
 *   from its helpers (no RTTI names it). The helpers take it as char *
 *   and stay free, as do the C/PMF/PMF0 dispatch tables over it; C, C37
 *   and C2dd4 remain as the tables' receiver types. Indexed entry fields
 *   are spelled with STAR_OFF and its siblings where the ROM adds the
 *   field offset before the index.
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

#include "types.h"
#include "CP15.h"
#include "G2.h"
#include "G2x.h"
#include "GX.h"
#include "Memory.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "daDemo_c.h"
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

struct OamAttr;

#pragma defer_codegen off

/* The staff-roll block's helpers are free functions that take the block
   as char *; they read it through this record. */
typedef daDemo_c::StaffRoll StaffRoll;

/* Field offsets inside one star, window or credit entry. Where a helper
   indexes an entry, the ROM adds the field offset to the block first and
   the scaled index second; the member spelling folds the two into one
   multiply-add and comes out shorter. */
#define STAR_OFF(m) ((int)&((StaffRoll *)0)->stars[0].m)
#define WINDOW_OFF(m) ((int)&((StaffRoll *)0)->window[0].m)
#define CREDIT_OFF(m) ((int)&((StaffRoll *)0)->credit[0].m)

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

/* shadow struct 'C' */
struct C;

/* shadow typedef 'void' */
typedef void (C::*PMF)(int);

/* pointer-to-member taking no args (dispatch tables at 02110f34/02110f9c) */
typedef void (C::*PMF0)();

/* pointer-to-member taking (cmd, a2, a3) — func_ov002_020f7d74's table */
typedef void (daDemo_c::*PMF3)(unsigned char*, int, int);

/* dispatch receiver of func_ov002_020f37a0: the StaffRoll block seen through
   window[0]'s control bytes (StaffRoll::Window active, kind, step). */
struct C37 {
    unsigned char pad[0x15c];
    unsigned char active; /* 0x15c */
    unsigned char kind;   /* 0x15d */
    unsigned char step;   /* 0x15e */
};
typedef void (C37::*PMF37)(int);
struct Entry37 { PMF37 pmf; };

/* dispatch receiver of func_ov002_020f2dd4: the StaffRoll block seen through
   credit[0]'s control bytes (StaffRoll::Credit active, step). */
struct C2dd4 {
    unsigned char pad[0x1c8];
    unsigned char active;
    unsigned char step;
};
typedef void (C2dd4::*PMF2dd4)(int);
struct Entry2dd4 { PMF2dd4 pmf; };

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

/* shadow typedef 'Vec3' */
typedef struct
{
  s32 x;
  s32 y;
  s32 z;
} Vec3;

/* shadow struct 'Callback' */
struct Callback;

/* shadow typedef 'u8' */
typedef unsigned char u8;

/* shadow typedef 'u16' */
typedef unsigned short u16;

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow typedef 'Vector3' */

/* shadow struct 'M48' */
struct M48 { int w[12]; };

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

/* TUBUILD CONFLICT -- alternate body of typedef 'Vec3', from the legacy file for func_ov002_020f7538, NOT applied:
typedef struct Vec3 { int x, y, z; } Vec3;
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
extern unsigned char data_0209d454;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern OamAttr data_ov002_0210bddc;
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
/* local extern dropped: decl_common.h already declares _ZN6Memory16operator_delete2EPv(void*) */
/* The scene fader: dScene_c::SetFaders(dFdBrightness_c *) assigns it. Kept
   as the void * this file has always declared (the tree's declarations
   disagree); 23f0 casts at the one use. */
extern void* data_0209f5bc;
extern u8 data_ov002_02111144;
extern u32 data_020a0db0;
extern int func_ov002_020f2630(daDemo_c *self);
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
extern PMF data_ov002_02110e24[];
extern PMF data_ov002_02110e9c[];
extern void func_ov002_020f3828(int *c);
extern void MultiCopyHalf(void *src, void *dst, unsigned int count);
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
extern "C" void func_ov002_020f39ec(char* c, int i);
extern PMF data_ov002_02110e7c[];
extern "C" void func_ov002_020f5990(char* c);
extern void func_ov002_020f5a6c(char* p);
extern void func_ov002_020f396c(char* p);
extern void func_ov002_020f30d4(char* p);
extern void func_ov002_020f2ea0(char* p);
extern void func_ov002_020f2a68(char* p);
extern void func_ov002_020f2f64(char* self);
extern void func_ov002_020f2a48(char *p);
extern void func_ov002_020f2958(char* c);
extern void func_ov002_020f37fc(char*);
extern void func_ov002_020f2f9c(void *, int);
extern void func_ov002_020f3954(char *);
/* local extern dropped: decl_common.h already declares LoadFont(u8) */
void func_ov002_020f2e30(StaffRoll *, int);
extern PMF0 data_ov002_02110f9c[];
extern void SetSubBg3Offset(int a, int b);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void* dst);
extern void Deallocate(void* p);
extern void func_02056434(const void* src, int offset, int count);
extern void func_020562b4(const void* src, u32 offset, u32 count);
extern void func_02056374(const void* src, u32 offset, u32 count);
extern void func_ov002_020f5ad4(char* c);
extern void func_ov002_020f5f60(char *c);
extern int func_ov002_020f5fb8(void* c);
extern int func_ov002_020f5f8c(void* c);
extern void func_ov002_020f5dd8(C* c, int idx);
extern void func_ov002_020f37a0(C37* c);
extern void func_ov002_020f2dd4(C2dd4* c);
extern void func_ov002_020f26e0(void *c);
extern void func_ov002_020f2eac(void *c);
extern void func_ov002_020f3978(char *c);
extern int func_ov002_020f27e8(char *c);
extern int func_ov002_020f2990(char *c);
extern void func_ov002_020f5f0c(C* c, int idx);
extern void func_ov002_020f5fe4(char *c);
/* local extern dropped: decl_common.h declares func_ov002_020f6514(unsigned char*, void*, unsigned char) */
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern void func_02012694(unsigned int, void*);
extern short ReadUnalignedShort(unsigned char *p);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
/* local extern dropped: decl_common.h declares data_ov002_0210bc88 as 'int'; callers cast the address */
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern int* Vec3_LslInPlace(int *v, int sh);
extern u16 gKuppaScriptFrame;
extern char *data_0209f318;
extern int data_ov002_0210b614;
extern int data_ov002_0210b958;
/* local extern dropped: decl_common.h declares data_0209b41c */
extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern void func_0201f138(void);
extern void func_0201ef38(void);
extern void Math_Function_0203b0fc(int* p, int target, int scale, int max);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_MulScalar(Vec3* out, const Vec3* in, int scale);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);

/* local extern dropped: decl_common.h declares data_0209b41c */
extern int data_0208ee44;
extern int data_ov002_0210b970[3];
extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern int data_ov002_0210b964[3];
/* local extern dropped: decl_common.h declares func_02008b4c(void*,void*,void*,void*) */
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzLen(const Vector3 *v);
extern PMF3 data_ov002_0210b708;
extern PMF3 data_ov002_0210b678;
extern PMF3 data_ov002_0210b618;
extern PMF3 data_ov002_0210b620;
extern PMF3 data_ov002_0210b640;
extern PMF3 data_ov002_0210b628;
extern PMF3 data_ov002_0210b648;
extern PMF3 data_ov002_0210b8c0;
extern PMF3 data_ov002_0210b900;
extern PMF3 data_ov002_0210b938;
extern PMF3 data_ov002_0210b930;
extern PMF3 data_ov002_0210b928;
extern PMF3 data_ov002_0210b920;
extern PMF3 data_ov002_0210b918;
extern PMF3 data_ov002_0210b910;
extern PMF3 data_ov002_0210b908;
extern PMF3 data_ov002_0210b750;
extern PMF3 data_ov002_0210b8f8;
extern PMF3 data_ov002_0210b8f0;
extern PMF3 data_ov002_0210b8e8;
extern PMF3 data_ov002_0210b8e0;
extern PMF3 data_ov002_0210b8d8;
extern PMF3 data_ov002_0210b8d0;
extern PMF3 data_ov002_0210b8c8;
extern PMF3 data_ov002_0211104c[24];
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s32 x, s32 y, s32 z);
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 data_0209b41c;
extern unsigned char data_0209f2d8;
extern char data_ov002_0211094c;
extern JugemModelFile data_ov085_0213074c;
/* TUBUILD CONFLICT -- alternate declaration of data_0209d454, from the legacy file for func_ov002_020f23f0, NOT applied: extern u8 data_0209d454; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f2aec, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, unsigned short a, unsigned short b, unsigned short c, int d); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f2bf4, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, unsigned short a, unsigned short b, int c, unsigned short d); */
/* TUBUILD CONFLICT -- alternate declaration of func_0201ef50, from the legacy file for func_ov002_020f2e30, NOT applied: extern void func_0201ef50(unsigned int x); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f3de4, NOT applied: extern int func_ov002_020f5a94(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f40fc, NOT applied: extern int func_ov002_020f5a94(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd5atan2E5Fix12IiES1_, from the legacy file for func_ov002_020f43cc, NOT applied: extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f43cc, NOT applied: extern int func_ov002_020f5a94(void *a); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4710, NOT applied: extern int func_ov002_020f5a94(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4a2c, NOT applied: extern int func_ov002_020f5a94(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f4d70, NOT applied: extern int func_ov002_020f5a94(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f5a94, from the legacy file for func_ov002_020f5010, NOT applied: extern int func_ov002_020f5a94(char *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f2790, from the legacy file for func_ov002_020f5990, NOT applied: extern "C" void func_ov002_020f2790(char* c, int p1, int p2, int p3, s16 p4); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8SaveData19IsCharacterUnlockedEj, from the legacy file for func_ov002_020f5a94, NOT applied: extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int idx); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f2984, from the legacy file for func_ov002_020f5ad4, NOT applied: extern void func_ov002_020f2984(char* p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e38, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e58, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e78, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5e98, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5eb8, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5ed8, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov002_020f3828, from the legacy file for func_ov002_020f5ee4, NOT applied: extern void func_ov002_020f3828(void *); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Memory16operator_delete2EPv, from the legacy file for func_ov002_020f5f60, NOT applied: extern void _ZN6Memory16operator_delete2EPv(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209d454, from the legacy file for func_ov002_020f5fe4, NOT applied: extern u8 data_0209d454; */
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
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov002_020f7780, NOT applied: extern void *data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f7780, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7780, NOT applied: extern short Vec3_HorzAngle(const int *v0, const int *v1); */
/* TUBUILD CONFLICT -- alternate declaration of gKuppaScriptFrame, from the legacy file for func_ov002_020f79c0, NOT applied: extern unsigned short gKuppaScriptFrame; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f79c0, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of ApproachAngle, from the legacy file for func_ov002_020f79c0, NOT applied: extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void *data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of AddVec3, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd5atan2E5Fix12IiES1_, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x); */
/* BSS between the four function-local Vector3 statics. Each object is
   its own section so the linker can place the statics at their retail
   addresses. The five char objects match decl_common.h; the following
   symbol is 8-aligned so the slot stays 8 bytes. */
int data_ov002_02110b00;
char data_ov002_02110b10[8];
char data_ov002_02110b18[8];
char data_ov002_02110b20[8];
char data_ov002_02110b28[8];
char data_ov002_02110b30[8];
char data_ov002_02110b38[8];
char data_ov002_02110b40[8];
char data_ov002_02110b48[8];
char data_ov002_02110b50;
__attribute__((aligned(8))) char data_ov002_02110b58[8];
char data_ov002_02110b60[8];
char data_ov002_02110b68[8];
char data_ov002_02110b70;
__attribute__((aligned(8))) char data_ov002_02110b78;
__attribute__((aligned(8))) char data_ov002_02110b80[8];
char data_ov002_02110b88[8];
char data_ov002_02110b90[8];
char data_ov002_02110b98;
__attribute__((aligned(8))) char data_ov002_02110ba0[8];
char data_ov002_02110ba8[8];
char data_ov002_02110bb0[8];
char data_ov002_02110bb8[8];
char data_ov002_02110bc0[8];
char data_ov002_02110bc8[8];
char data_ov002_02110bd0[8];
char data_ov002_02110bd8[8];
char data_ov002_02110be0[8];
char data_ov002_02110be8[8];
char data_ov002_02110bf0[8];
char data_ov002_02110bf8[8];
char data_ov002_02110c00[8];
char data_ov002_02110c08[8];
char data_ov002_02110c10[8];
char data_ov002_02110c18;
char data_ov002_02110c2c[12];
char data_ov002_02110c38[12];
char data_ov002_02110c44[12];
char data_ov002_02110c50[12];
char data_ov002_02110c5c[12];
char data_ov002_02110c68[12];
char data_ov002_02110c74[12];
char data_ov002_02110c80[12];
char data_ov002_02110c8c[12];
char data_ov002_02110c98[12];
char data_ov002_02110ca4[12];
char data_ov002_02110cb0[12];
char data_ov002_02110cbc[12];
char data_ov002_02110cc8[12];
char data_ov002_02110cd4[12];
char data_ov002_02110ce0[12];
char data_ov002_02110cec[12];
char data_ov002_02110cf8[12];
char data_ov002_02110d04[12];
char data_ov002_02110d10[12];
char data_ov002_02110d1c[12];
char data_ov002_02110d28[12];
char data_ov002_02110d34[12];
char data_ov002_02110d40[12];
char data_ov002_02110d4c[12];
char data_ov002_02110d58[12];
char data_ov002_02110d64[12];
char data_ov002_02110d70[12];
char data_ov002_02110d7c[12];
char data_ov002_02110d88[12];
char data_ov002_02110d94[12];
char data_ov002_02110da0[12];
char data_ov002_02110dc4[12];
char data_ov002_02110dd0[12];
int data_ov002_02110ddc[3];
/* TUBUILD CONFLICT -- alternate declaration of data_0209b41c, from the legacy file for _ZN8daDemo_c6RenderEv, NOT applied: extern M48 data_0209b41c; */
}

// @symbol _ZN8daDemo_cD1Ev
daDemo_c::~daDemo_c()
{
}

// @symbol _ZN8daDemo_cD0Ev
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daDemo_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" daDemo_c *_ZN8daDemo_cD0Ev(daDemo_c *thiz)
{
    thiz->daDemo_c::~daDemo_c();  /* the D1 body, through the one host symbol */
    daDemo_c::operator delete(thiz);    /* the class-specific delete D0 ends with */
    return thiz;
}
#endif
/* the D0 deleting destructor this file is enrolled for emits from the
   ~daDemo_c definition at ordinal 0 below; objisolate keeps the
   variant this file's delinks entry names. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020f1fcc, 0x020f1fcc, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f1fccEv
/* Sub-screen fade step (param1 0x2f, states 2 and 3). While fadeLevel is
 * non-zero it counts fadeTimer to 0x18, then drops fadeLevel by one and calls
 * G2x::SetBlendAlpha on 0x4001050 with (4, 0x28, fadeLevel, 0x10 - fadeLevel).
 * When fadeLevel reaches zero it clears bit 2 of data_0209d454, writes that
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
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 0x28, v, 0x10 - v);
        return;
    }
    data_0209d454 &= ~4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 0, 0x28, 0xc, 4);
    mScroll->state++;
    for (i = 0; i < 3; i++) {
        mScroll->spriteDelay[i] = (i + 1) << 4;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020f20f4, 0x020f20f4, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f20f4Ev
/* Moving-sprite step. For each active slot, on frames where FRAME_TIMER bit 0
 * is clear, spriteX += spriteVelX and the RNG is stepped (result unused); a
 * slot that reaches x 0x140 moving right or x -0x40 moving left goes idle with
 * spriteDelay (slot + 1) << 5. Every active slot is drawn each frame. */
/* This member is C++, not C: under every C-flavored path here (the legacy .c
   file, an extern "C" block, #pragma cplusplus off) the backend strength-
   reduces the shared `d + i*2` into an r8 byte-walk; the ROM recomputes it
   (`add r0, r0, r9, lsl #1`), which only the C++ backend emits. extern "C" on
   the definition keeps the unmangled cartridge spelling without changing the
   backend. */
extern "C" struct OamAttr *data_ov002_0210be1c[];
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off

void daDemo_c::func_ov002_020f20f4() {
    int i = 0;
    int zpos = 0;
    int zneg = 0;
    int two = 2;
    int m1 = -1;
    for (; i < 3; i++) {
        char *d = (char *)this->mScroll;
        if ((((ScrollState *)d)->spriteActive[i]) != 0) {
            if ((data_020a0db0 & 1) == 0) {
                /* q is &spriteX[i] through a base stepped by i * 2:
                   q = &spriteX[i] builds 4 bytes long, and reading
                   spriteX[i] directly builds 4 bytes short. */
                s16 *q = ((ScrollState *)(d + i * 2))->spriteX;
                s16 vel = ((ScrollState *)d)->spriteVelX[i];
                s16 pos = *q;
                int *seed = &data_0209e650;
                *q = (s16)(pos + vel);
                RandomIntInternal(seed);
                d = (char *)this->mScroll;
                if (((ScrollState *)d)->spriteVelX[i] >= 0) {
                    if (((ScrollState *)d)->spriteX[i] >= 0x140) {
                        ((ScrollState *)d)->spriteActive[i] = (u8)zpos;
                        d = (char *)this->mScroll;
                        ((ScrollState *)d)->spriteDelay[i] = (s16)((i + 1) << 5);
                    }
                } else if (((ScrollState *)d)->spriteX[i] <= (-0x40)) {
                    ((ScrollState *)d)->spriteActive[i] = (u8)zneg;
                    d = (char *)this->mScroll;
                    ((ScrollState *)d)->spriteDelay[i] = (s16)((i + 1) << 5);
                }
            }
            d = (char *)this->mScroll;
            OAM::RenderSub(data_ov002_0210be1c[i], ((ScrollState *)d)->spriteX[i],
                           ((ScrollState *)d)->spriteY[i], m1, two);
        }
    }
}
#pragma opt_strength_reduction on
#pragma opt_loop_invariants on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020f2210, 0x020f2210, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f2210Ev
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020f2340, 0x020f2340, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f2340Ev
/* Draw the two centre sprites at x 0x80. */

void daDemo_c::func_ov002_020f2340()
{
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[0]);
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, mScroll->centerSpriteY[1]);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020f237c, 0x020f237c, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f237cEv
/* Scroll the two sub-screen BG layers in opposite directions (bg1 x down
 * one, bg0 x up one) and write both offsets to the hardware. */

void daDemo_c::func_ov002_020f237c()
{
    mScroll->bg1X -= 1;
    mScroll->bg0X += 1;
    SetSubBg0Offset(mScroll->bg0X, mScroll->bg0Y);
    SetSubBg1Offset(mScroll->bg1X, mScroll->bg1Y);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020f23d0, 0x020f23d0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f23d0Ev
/* CleanupResources for param1 0x2f: free the scroll block. */

int daDemo_c::func_ov002_020f23d0()
{
    Memory::operator_delete2(this->mScroll);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020f23f0, 0x020f23f0, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f23f0Ev
/* Behavior for param1 0x2f: the scroll block's seven states.
 *   0  each frame the scene fader (data_0209f5bc) reports IsAtStart,
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
 *      bg1Y 0xe0, next state and bits 0..1 of data_0209d454 cleared into
 *      bits 8..12 of 0x4001000. Also runs the horizontal scroll (237c)
 *   6  nothing
 * Every state then runs 2340, 2210 and 20f4. */

int daDemo_c::func_ov002_020f23f0()
{
    switch (mScroll->state) {
    case 0:
        if (((dFdBrightness_c *)data_0209f5bc)->IsAtStart() != 0) {
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
                data_0209d454 = data_0209d454 & ~3;
                *(volatile u32*)0x4001000 = (*(volatile u32*)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
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
// @symbol func_ov002_020f2630
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2630(daDemo_c *self){
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
// @symbol _ZN8daDemo_c19func_ov002_020f26c4EPh
/* Script handler (table entry 23): mCue = p[0], the value 23f0 waits on. */

int daDemo_c::func_ov002_020f26c4(unsigned char *src)
{
    this->mCue = src[0];
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020f26d4, 0x020f26d4, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f26d4
/* func_ov002_020f26d4 @ 0x20f26d4 (ov002) -- tail-call veneer to func_ov002_020f2630 (0x20f2630).
 * ldr ip, [pc]; bx ip; .word 0x20f2630
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f26d4(void) {
    ((void (*)(void))func_ov002_020f2630)();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov002_020f26e0, 0x020f26e0, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f26e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f26e0(void *thiz)
{
    /* The kira callback's tracking records: dPa_c.h keeps them as opaque
       words, so OamEnt views each one as the sparkle sprite it draws. */
    struct OamEnt *e = (struct OamEnt *)((StaffRoll *)thiz)->kira.trackingRecords;
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov002_020f2790, 0x020f2790, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2790
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2790(char* c, int p1, int p2, int p3, s16 p4)
{
    ((StaffRoll *)c)->kira.unk308 = p3;
    ((StaffRoll *)c)->kira.unk30c = p4;
    ((StaffRoll *)c)->kiraParticle = Particle::System::New(
        ((StaffRoll *)c)->kiraParticle, 0x3e, p1 << 12, p2 << 12, 0, 0,
        (Particle::Callback *)((struct ParticleCallback *)&((StaffRoll *)c)->kira));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020f27e8, 0x020f27e8, size 0x170 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f27e8
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov002_020f2958, 0x020f2958, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2958
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020f2984, 0x020f2984, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2984
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2984(char *p)
{
    ((StaffRoll *)p)->burstActive = 0;
    return (int)p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020f2990, 0x020f2990, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2990
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov002_020f2a48, 0x020f2a48, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a48
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a48(char *p)
{
    /* 0x1d8/0x1d9 and the position: see func_ov002_020f2990. */
    ((StaffRoll *)p)->endActive = 1;
    ((StaffRoll *)p)->endVisible = 1;
    ((StaffRoll *)p)->endX = 524288;
    ((StaffRoll *)p)->endY = 393216;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020f2a68, 0x020f2a68, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a68(char *p)
{
    /* 0x1d8/0x1d9: see func_ov002_020f2990. */
    ((StaffRoll *)p)->endActive = 0;
    ((StaffRoll *)p)->endVisible = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020f2a78, 0x020f2a78, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a78
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a78(char *p, int i)
{
    ((StaffRoll *)p)->credit[i].active = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020f2a88, 0x020f2a88, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a88
extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a88(char *c, int i) {
    unsigned long new_var;
    *((short *)((c + CREDIT_OFF(timer)) + (i * 8))) =
        (*((u16 *)((c + CREDIT_OFF(timer)) + (i * 8)))) + 1;
    new_var = i;
    if ((*((u16 *)((c + (new_var * 8)) + CREDIT_OFF(timer)))) < 0x39) {
        return;
    }
    *((u16 *)((c + (new_var * 8)) + CREDIT_OFF(timer))) = 0;
    *((u8 *)((c + CREDIT_OFF(step)) + (new_var * 8))) =
        (*((u8 *)((c + CREDIT_OFF(step)) + (new_var * 8)))) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020f2aec, 0x020f2aec, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2aec
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f2aec(char *c, int idx)
{
    unsigned char cnt;

    if (*(unsigned short *)(c + idx * 8 + CREDIT_OFF(timer)) == 0) {
        data_0209d454 |= 1;
    }
    *(unsigned short *)(c + CREDIT_OFF(timer) + idx * 8) += 1;
    *(unsigned short *)(c + CREDIT_OFF(blendTimer) + idx * 8) += 1;

    if (*(unsigned short *)(c + idx * 8 + CREDIT_OFF(blendTimer)) >= 2) {
        *(unsigned short *)(c + idx * 8 + CREDIT_OFF(blendTimer)) = 0;
        *(unsigned char *)(c + CREDIT_OFF(blend) + idx * 8) += 1;
        cnt = *(unsigned char *)(c + idx * 8 + CREDIT_OFF(blend));
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x36, cnt, 0x10 - cnt);
    }

    if (*(unsigned short *)(c + idx * 8 + CREDIT_OFF(timer)) < 30) {
        return;
    }
    *(unsigned short *)(c + idx * 8 + CREDIT_OFF(timer)) = 0;
    *(unsigned char *)(c + CREDIT_OFF(step) + idx * 8) += 1;
    *(volatile unsigned short *)0x4001050 = 0;
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov002_020f2bf4, 0x020f2bf4, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2bf4
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f2bf4(char *c, int idx)
{
    unsigned char cnt;

    *(unsigned short *)(c + CREDIT_OFF(timer) + idx * 8) += 1;
    *(unsigned short *)(c + CREDIT_OFF(blendTimer) + idx * 8) += 1;

    if (*(unsigned short *)(c + idx * 8 + CREDIT_OFF(blendTimer)) >= 2) {
        *(unsigned short *)(c + idx * 8 + CREDIT_OFF(blendTimer)) = 0;
        *(unsigned char *)(c + CREDIT_OFF(blend) + idx * 8) += 1;
        cnt = *(unsigned char *)(c + idx * 8 + CREDIT_OFF(blend));
        G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x36, 0x10 - cnt, cnt);
    }

    if (*(unsigned short *)(c + idx * 8 + CREDIT_OFF(timer)) < 30) {
        return;
    }
    *(unsigned short *)(c + idx * 8 + CREDIT_OFF(timer)) = 0;
    *(unsigned char *)(c + CREDIT_OFF(step) + idx * 8) += 1;
    *(unsigned char *)(c + CREDIT_OFF(message) + idx * 8) += 1;
    *(unsigned char *)(c + idx * 8 + CREDIT_OFF(blend)) = 0;
    *(unsigned short *)(c + idx * 8 + CREDIT_OFF(blendTimer)) = 0;
    data_0209d454 &= ~1;
    func_0201ef50(*(unsigned char *)(c + idx * 8 + CREDIT_OFF(message)));
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020f2d0c, 0x020f2d0c, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2d0c
extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2d0c(char *c, int i) {
    long new_var;
    *((short *)((c + CREDIT_OFF(timer)) + (i * 8))) =
        (*((unsigned short *)((c + CREDIT_OFF(timer)) + (i * 8)))) + 1;
    new_var = 8;
    char *r = c + (i * new_var);
    if (((StaffRoll *)r)->credit[0].timer < 0x39) {
        return;
    }
    ((StaffRoll *)r)->credit[0].timer = 0;
    *((unsigned char *)((c + CREDIT_OFF(step)) + (i * new_var))) =
        (*((unsigned char *)((c + CREDIT_OFF(step)) + (i * new_var)))) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov002_020f2d70, 0x020f2d70, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2d70
extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2d70(char *c, int i) {
    char *p;
    p = c + CREDIT_OFF(timer);
    *((short *)(p + (i * 8))) = (*((unsigned short *)(p + (i * 8)))) + 1;
    p = c + (i * 8);
    if (((StaffRoll *)p)->credit[0].timer < 0x1e) {
        return;
    }
    ((StaffRoll *)p)->credit[0].timer = 0;
    *((unsigned char *)((c + CREDIT_OFF(step)) + (i * 8))) =
        (*((unsigned char *)((c + CREDIT_OFF(step)) + (i * 8)))) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020f2dd4, 0x020f2dd4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2dd4
extern Entry2dd4 data_ov002_02110ebc[];
extern "C" void func_ov002_020f2dd4(C2dd4* c){
    if (c->active == 0)
        return;
    (c->*data_ov002_02110ebc[c->step].pmf)(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov002_020f2e30, 0x020f2e30, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2e30
/* func_ov002_020f2e30 at 0x020f2e30
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2e30(StaffRoll *self, int n) {
    if (n == 0) return;
    if (n == 0x15) return;
    self->credit[0].active = 1;
    self->credit[0].step = 0;
    self->credit[0].timer = 0;
    self->credit[0].blendTimer = 0;
    self->credit[0].blend = 0;
    n = (n - 1) << 1;
    self->credit[0].message = (unsigned char)n;
    func_0201f108();
    func_0201ef50(n & 0xff);
    data_0209d454 |= 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov002_020f2ea0, 0x020f2ea0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2ea0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2ea0(char *p)
{
    ((StaffRoll *)p)->credit[0].active = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov002_020f2eac, 0x020f2eac, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2eac
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov002_020f2f18, 0x020f2f18, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2f18
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov002_020f2f64, 0x020f2f64, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2f64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2f64(char* self)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (((StaffRoll *)self)->dolls[0].active != 0) {
            ((StaffRoll *)self)->dolls[0].frame++;
        }
        self += 0x14;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov002_020f2f9c, 0x020f2f9c, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2f9c
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov002_020f30d4, 0x020f30d4, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f30d4
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov002_020f30f8, 0x020f30f8, size 0x1ec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f30f8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f30f8(char *o, int i)
{
    int idx = i * 0x30;
    unsigned short t;
    int oldA, oldB;
    int cc, dd, ee, ff;

    t = *(unsigned short *)(o + WINDOW_OFF(delay) + idx);
    if (t != 0) {
        *(unsigned short *)(o + WINDOW_OFF(delay) + idx) = t - 1;
        return;
    }

    *(int *)(o + WINDOW_OFF(left) + idx) -= *(int *)(o + WINDOW_OFF(stepX) + idx);
    *(int *)(o + WINDOW_OFF(top) + idx) -= *(int *)(o + WINDOW_OFF(stepY) + idx);
    *(int *)(o + WINDOW_OFF(right) + idx) += *(int *)(o + WINDOW_OFF(stepX) + idx);
    *(int *)(o + WINDOW_OFF(bottom) + idx) += *(int *)(o + WINDOW_OFF(stepY) + idx);

    oldA = *(int *)(o + WINDOW_OFF(bgX) + idx);
    oldB = *(int *)(o + WINDOW_OFF(bgY) + idx);
    *(int *)(o + WINDOW_OFF(bgX) + idx) = oldA + *(int *)(o + WINDOW_OFF(stepX) + idx);
    *(int *)(o + WINDOW_OFF(bgY) + idx) += *(int *)(o + WINDOW_OFF(stepY) + idx);

    if (*(int *)(o + WINDOW_OFF(bgX) + idx) >= 0)
        *(int *)(o + WINDOW_OFF(bgX) + idx) = 0;
    if (*(int *)(o + WINDOW_OFF(bgY) + idx) >= 0)
        *(int *)(o + WINDOW_OFF(bgY) + idx) = 0;
    if (*(int *)(o + WINDOW_OFF(left) + idx) < 0)
        *(int *)(o + WINDOW_OFF(left) + idx) = 0;
    if (*(int *)(o + WINDOW_OFF(top) + idx) < 0)
        *(int *)(o + WINDOW_OFF(top) + idx) = 0;
    if (*(int *)(o + WINDOW_OFF(right) + idx) >= 0xff000)
        *(int *)(o + WINDOW_OFF(right) + idx) = 0xff000;
    if (*(int *)(o + WINDOW_OFF(bottom) + idx) >= 0xc0000)
        *(int *)(o + WINDOW_OFF(bottom) + idx) = 0xc0000;
    if (*(int *)(o + WINDOW_OFF(bgX) + idx) > 0)
        *(int *)(o + WINDOW_OFF(bgX) + idx) = 0;
    if (*(int *)(o + WINDOW_OFF(bgY) + idx) > 0)
        *(int *)(o + WINDOW_OFF(bgY) + idx) = 0;

    cc = *(int *)(o + WINDOW_OFF(bgX) + idx) - oldA;
    func_ov002_020f2f18(o, cc, *(int *)(o + WINDOW_OFF(bgY) + idx) - oldB);

    ee = *(int *)(o + WINDOW_OFF(right) + idx) >> 12;
    ff = *(int *)(o + WINDOW_OFF(bottom) + idx) >> 12;
    cc = *(int *)(o + WINDOW_OFF(left) + idx) >> 12;
    dd = *(int *)(o + WINDOW_OFF(top) + idx) >> 12;
    *(unsigned short *)0x4001042 = ((cc << 8) & 0xff00) | (ee & 0xff);
    *(unsigned short *)0x4001046 = ((dd << 8) & 0xff00) | (ff & 0xff);
    SetSubBg1Offset(*(int *)(o + WINDOW_OFF(bgX) + idx) >> 12,
                    *(int *)(o + WINDOW_OFF(bgY) + idx) >> 12);
    SetSubBg2Offset(*(int *)(o + WINDOW_OFF(bgX) + idx) >> 12,
                    *(int *)(o + WINDOW_OFF(bgY) + idx) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov002_020f32e4, 0x020f32e4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f32e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f32e4(StaffRoll *self, int i) {
    self->window[i].delay = 0x10;
    self->window[i].stepX = 0x1800;
    self->window[i].stepY = 0x1800;
    self->window[i].step = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov002_020f3310, 0x020f3310, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3310
extern "C" void func_ov002_020f3310(C* self, int idx) {
    unsigned char sel = ((StaffRoll *)self)->window[idx].step;
    (self->*data_ov002_02110e24[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov002_020f335c, 0x020f335c, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f335c
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f335c(char *o, int i) {
    int idx = i * 0x30;
    int *fld = (int *)(o + WINDOW_OFF(top) + idx);
    *fld = *fld + 0x1800;
    if (*fld >= 0x66000) {
        *fld = 0x66000;
        ((StaffRoll *)o)->window[i].active = 0;
        ((StaffRoll *)o)->window[i].step = 0;
    }
    char *b = o + idx;
    int f140 = ((StaffRoll *)b)->window[0].left >> 0xc;
    int f148 = ((StaffRoll *)b)->window[0].right >> 0xc;
    int f14c = ((StaffRoll *)b)->window[0].bottom >> 0xc;
    int fv   = *fld >> 0xc;
    *(volatile unsigned short *)0x4001042 = (unsigned short)(((f140 << 8) & 0xff00) | (f148 & 0xff));
    *(volatile unsigned short *)0x4001046 = (unsigned short)(((fv   << 8) & 0xff00) | (f14c & 0xff));
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov002_020f340c, 0x020f340c, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f340c
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f340c(char *o, int i) {
    int idx = i * 0x30;
    int *fld = (int *)(o + WINDOW_OFF(top) + idx);
    *fld = *fld + 0x1800;
    if (*fld >= 0x66000) {
        *fld = 0x66000;
        ((StaffRoll *)o)->window[i].active = 0;
        ((StaffRoll *)o)->window[i].step = 0;
    }
    char *b = o + idx;
    int f140 = ((StaffRoll *)b)->window[0].left >> 0xc;
    int f148 = ((StaffRoll *)b)->window[0].right >> 0xc;
    int f14c = ((StaffRoll *)b)->window[0].bottom >> 0xc;
    int fv   = *fld >> 0xc;
    *(volatile unsigned short *)0x4001042 = (unsigned short)(((f140 << 8) & 0xff00) | (f148 & 0xff));
    *(volatile unsigned short *)0x4001046 = (unsigned short)(((fv   << 8) & 0xff00) | (f14c & 0xff));
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov002_020f34bc, 0x020f34bc, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f34bc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f34bc(unsigned char *s, int idx)
{
    int k = idx * 0x30;
    int flag;
    int a, b, c, d;
    int bs, cs, ds, as_;
    *(int *)(s + WINDOW_OFF(left) + k) -= 0x1800;
    *(int *)(s + WINDOW_OFF(top) + k) -= 0x1800;
    *(int *)(s + WINDOW_OFF(right) + k) += 0x1800;
    *(int *)(s + WINDOW_OFF(bottom) + k) += 0x1800;
    flag = 0;
    if (*(int *)(s + WINDOW_OFF(left) + k) <= 0x80000)
        *(int *)(s + WINDOW_OFF(left) + k) = 0x80000;
    if (*(int *)(s + WINDOW_OFF(top) + k) <= 0x56000)
        *(int *)(s + WINDOW_OFF(top) + k) = 0x56000;
    if (*(int *)(s + WINDOW_OFF(right) + k) >= 0xfc000)
        *(int *)(s + WINDOW_OFF(right) + k) = 0xfc000;
    if (*(int *)(s + WINDOW_OFF(bottom) + k) >= 0xbc000) {
        *(int *)(s + WINDOW_OFF(bottom) + k) = 0xbc000;
        flag++;
    }
    if (flag != 0)
        (*(unsigned char *)(s + WINDOW_OFF(step) + k))++;
    a = *(int *)(s + WINDOW_OFF(top) + k);
    b = *(int *)(s + WINDOW_OFF(left) + k);
    c = *(int *)(s + WINDOW_OFF(right) + k);
    d = *(int *)(s + WINDOW_OFF(bottom) + k);
    bs = b >> 12;
    cs = c >> 12;
    ds = d >> 12;
    as_ = a >> 12;
    *(unsigned short *)0x4001042 = ((bs << 8) & 0xff00) | (cs & 0xff);
    *(unsigned short *)0x4001046 = ((as_ << 8) & 0xff00) | (ds & 0xff);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov002_020f35e4, 0x020f35e4, size 0x15c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f35e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f35e4(char *c, int i)
{
    int off = i * 0x30;
    *(int *)(c + WINDOW_OFF(left) + off) = 0xe0000;
    *(int *)(c + WINDOW_OFF(top) + off) = 0x66000;
    *(int *)(c + WINDOW_OFF(right) + off) = 0xe0000;
    *(int *)(c + WINDOW_OFF(bottom) + off) = 0x66000;
    *((volatile unsigned int *) 0x4001000) = ((*((volatile unsigned int *) 0x4001000)) & (~0xe000)) | 0x4000;
    *((volatile unsigned short *) 0x4001048) = (unsigned short) (((((*((volatile unsigned short *) 0x4001048)) & (~0x3f00)) | 0x1700) & 0xFFFFFFFFFFFFFFFFull) | 0x2000);
    *((volatile unsigned short *) 0x400104a) = (unsigned short) (((((*((volatile unsigned short *) 0x400104a)) & (~0x3f)) | 0x11) & 0xFFFFFFFFFFFFFFFFull) | 0x20);
    {
        int a = *(int *)(c + WINDOW_OFF(left) + off) >> 12;
        int d = *(int *)(c + WINDOW_OFF(right) + off) >> 12;
        int e = *(int *)(c + WINDOW_OFF(bottom) + off) >> 12;
        int b = *(int *)(c + WINDOW_OFF(top) + off) >> 12;
        *((volatile unsigned short *) 0x4001042) = (unsigned short) (((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *) 0x4001046) = (unsigned short) (((b << 8) & 0xff00) | (e & 0xff));
    }
    *(int *)(c + WINDOW_OFF(bgX) + off) = -0x60000;
    *(int *)(c + WINDOW_OFF(bgY) + off) = -0x38000;
    SetSubBg1Offset(*(int *)(c + WINDOW_OFF(bgX) + off) >> 12,
                    *(int *)(c + WINDOW_OFF(bgY) + off) >> 12);
    SetSubBg2Offset(*(int *)(c + WINDOW_OFF(bgX) + off) >> 12,
                    *(int *)(c + WINDOW_OFF(bgY) + off) >> 12);
    data_0209d454 |= 6;
    *(unsigned char *)(c + WINDOW_OFF(step) + off) =
        *(unsigned char *)(c + WINDOW_OFF(step) + off) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov002_020f3740, 0x020f3740, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3740
extern "C" void func_ov002_020f3740(C* self, int idx) {
    unsigned char sel = ((StaffRoll *)self)->window[idx].step;
    (self->*data_ov002_02110e9c[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov002_020f378c, 0x020f378c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f378c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f378c(StaffRoll *self, int idx) {
    self->window[idx].step = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov002_020f37a0, 0x020f37a0, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f37a0
extern Entry37 data_ov002_02110e4c[];
extern "C" void func_ov002_020f37a0(C37* c){
    if (c->active == 0)
        return;
    (c->*data_ov002_02110e4c[c->kind].pmf)(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov002_020f37fc, 0x020f37fc, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f37fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f37fc(char *c)
{
    func_ov002_020f3828((int*)c);
    ((StaffRoll *)c)->window[0].active = 1;
    ((StaffRoll *)c)->window[0].kind = 2;
    ((StaffRoll *)c)->window[0].step = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov002_020f3828, 0x020f3828, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3828
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3828(int *c)
{
    ((StaffRoll *)c)->window[0].left = 0x80000;
    ((StaffRoll *)c)->window[0].top = 0x66000;
    ((StaffRoll *)c)->window[0].right = 0xfc000;
    ((StaffRoll *)c)->window[0].bottom = 0xbc000;
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
        int a = ((StaffRoll *)c)->window[0].left >> 12;
        int d = ((StaffRoll *)c)->window[0].right >> 12;
        int e = ((StaffRoll *)c)->window[0].bottom >> 12;
        int b = ((StaffRoll *)c)->window[0].top >> 12;
        *((volatile unsigned short *)0x4001042) =
            (unsigned short)(((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *)0x4001046) =
            (unsigned short)(((b << 8) & 0xff00) | (e & 0xff));
    }
    ((StaffRoll *)c)->window[0].bgX = -0x60000;
    ((StaffRoll *)c)->window[0].bgY = -0x38000;
    SetSubBg1Offset(((StaffRoll *)c)->window[0].bgX >> 12, ((StaffRoll *)c)->window[0].bgY >> 12);
    SetSubBg2Offset(((StaffRoll *)c)->window[0].bgX >> 12, ((StaffRoll *)c)->window[0].bgY >> 12);
    data_0209d454 |= 6;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov002_020f3954, 0x020f3954, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3954
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3954(char *p)
{
    ((StaffRoll *)p)->window[0].active = 1;
    ((StaffRoll *)p)->window[0].kind = 1;
    ((StaffRoll *)p)->window[0].step = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov002_020f396c, 0x020f396c, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f396c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f396c(char *p)
{
    ((StaffRoll *)p)->window[0].active = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov002_020f3978, 0x020f3978, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3978
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov002_020f39ec, 0x020f39ec, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f39ec
extern "C" { /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f39ec(char *self, int idx) {
    char pad[4];
    (void)pad[0];
    int off = idx * 0x4c;
    unsigned short *timerBase = &((StaffRoll *)self)->stars[0].frameTimer;
    unsigned short *ctr = (unsigned short *)((char *)timerBase + off);
    *ctr = (unsigned short)(*ctr + 1);
    if (*ctr < 4)
        return;
    int zero = 0;
    *ctr = (unsigned short)zero;
    {
        unsigned short *frameBase = &((StaffRoll *)self)->stars[0].frame;
        unsigned short *ctr2 = (unsigned short *)((char *)frameBase + off);
        *ctr2 = (unsigned short)(*ctr2 + 1);
        char *row = self + off;
        if (((StaffRoll *)row)->stars[0].holdFrame != 0) {
            if (*ctr2 >= 0x10)
                *ctr2 = 0x10;
        } else if (*ctr2 >= 0x20) {
            *ctr2 = (unsigned short)zero;
        }
    }

    int i = 0;
    int acc = i;
    int dst_off = i;
    char *row = self + off;
    unsigned base = 0x6600000u;
    base += 0x2000u;
    base += (unsigned)(idx << 8);
    int n100 = 0x100;
    for (; i < 4; i++) {
        int v = data_ov002_021002a0[((StaffRoll *)row)->stars[0].frame];
        int src_off = acc + ((v & 7) << 8);
        if (v & 8)
            src_off += 0x2000;
        MultiCopyHalf((void *)(((StaffRoll *)self)->starChars + src_off),
                      (void *)(base + (unsigned)dst_off), (unsigned)n100);
        acc += 0x800;
        dst_off += 0x400;
    }
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov002_020f3ae8, 0x020f3ae8, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3ae8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3ae8(char *base, int idx)
{
    StaffRoll::Star *arr = (StaffRoll::Star *)base;
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov002_020f3ba0, 0x020f3ba0, size 0x198 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3ba0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3ba0(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;

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

    *(int *)(c + STAR_OFF(speed) + off) += 0x30;

    dx = 0xe0 - (*(int *)(c + STAR_OFF(x) + off) >> 12);
    dy = 0x46 - (*(int *)(c + STAR_OFF(y) + off) >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        *(unsigned char *)(c + STAR_OFF(step) + off) += 1;
        *(unsigned short *)(c + STAR_OFF(delay) + off) = 0;
        *(unsigned short *)(c + STAR_OFF(heading) + off) = 0xc000;
        *(int *)(c + STAR_OFF(radius) + off) = 0x20000;
        *(int *)(c + STAR_OFF(x) + off) = 0xe0000;
        *(int *)(c + STAR_OFF(y) + off) = 0x46000;
        return;
    }

    target = cstd::atan2(dy, dx);
    ang = *(unsigned short *)(c + STAR_OFF(heading) + off);
    if (target > ang) {
        *(unsigned short *)(c + STAR_OFF(heading) + off) += 0x80;
        if (target <= *(unsigned short *)(c + STAR_OFF(heading) + off))
            *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
    } else {
        if (ang <= target)
            return;
        *(unsigned short *)(c + STAR_OFF(heading) + off) -= 0x80;
        if (*(unsigned short *)(c + STAR_OFF(heading) + off) <= target)
            *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov002_020f3d38, 0x020f3d38, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3d38
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3d38(char *c, int i)
{
    int off = i * 0x4c;
    if (*(u16 *)(c + STAR_OFF(delay) + off) != 0) {
        *(u16 *)(c + STAR_OFF(delay) + off) = *(u16 *)(c + STAR_OFF(delay) + off) - 1;
        return;
    }
    *(s32 *)(c + STAR_OFF(x) + off) = -0x10000;
    *(s32 *)(c + off + STAR_OFF(y)) = -0x10000;
    *(s32 *)(c + off + STAR_OFF(speed)) = 0x2000;
    *(u16 *)(c + off + STAR_OFF(heading)) = 0;
    *(u16 *)(c + STAR_OFF(delay) + off) = 0;
    *(u8 *)(c + STAR_OFF(step) + off) = *(u8 *)(c + STAR_OFF(step) + off) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov002_020f3d98, 0x020f3d98, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3d98
extern "C" void func_ov002_020f3d98(C* self, int idx) {
    unsigned char sel = ((StaffRoll *)self)->stars[idx].step;
    (self->*data_ov002_02110e34[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov002_020f3de4, 0x020f3de4, size 0x318 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3de4
/* Per-slot update of one 0x4c-byte cutscene record. func_ov002_020f5a94
 * takes c so r0 stays live through the orbit path to the call. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3de4(char *c, int i)
{
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- func_ov002_020f40fc, 0x020f40fc, size 0x2d0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f40fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f40fc(char *c, int i)
{
    int off = i * 0x4c;

    if (*(unsigned short *)(c + STAR_OFF(frame) + off) == 0) {
        if (*(unsigned short *)(c + STAR_OFF(frameTimer) + off) == 0) {
            *(unsigned short *)(c + STAR_OFF(loops) + off) += 1;
        }
    }

    if (*(unsigned char *)(c + STAR_OFF(phase) + off) == 0) {
        int idx;
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

        *(unsigned short *)(c + STAR_OFF(heading) + off) -=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
        *(int *)(c + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);

        if (((StaffRoll *)c)->stars[0].phase != 0) {
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off) += 8;
        } else {
            if (*(unsigned short *)(c + STAR_OFF(angSpeed) + off) < 0x200) {
                *(unsigned short *)(c + STAR_OFF(angSpeed) + off) += 6;
                if (*(unsigned short *)(c + STAR_OFF(angSpeed) + off) > 0x200)
                    *(unsigned short *)(c + STAR_OFF(angSpeed) + off) = 0x200;
            }
        }

        if ((unsigned int)*(int *)(c + STAR_OFF(orbitTravel) + off) >=
                (unsigned int)data_ov002_02100120[i] &&
            ((StaffRoll *)c)->holdOrbit == 0) {
            *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
            *(unsigned short *)(c + STAR_OFF(flyTimer) + off) = 0;
            *(int *)(c + STAR_OFF(speed) + off) = 0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if ((unsigned int)*(int *)(c + STAR_OFF(orbitTravel) + off) < 0xc000)
            return;
        if (i != 2)
            return;
        if (((StaffRoll *)c)->holdOrbit != 0)
            return;
        *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
        *(unsigned short *)(c + STAR_OFF(flyTimer) + off) = 0;
        *(int *)(c + STAR_OFF(speed) + off) = 0x2000;
        return;
    }

    *(unsigned short *)(c + STAR_OFF(flyTimer) + off) += 1;
    *(int *)(c + STAR_OFF(x) + off) += *(int *)(c + STAR_OFF(speed) + off);
    *(int *)(c + STAR_OFF(speed) + off) += 0x300;
    if (((StaffRoll *)c)->stars[0].flyTimer < 0x70)
        return;
    {
        int off2 = (int)((long long)i) * 0x4c;
        *(unsigned char *)(c + STAR_OFF(step) + off2) += 1;
        *(unsigned short *)(c + STAR_OFF(loops) + off2) = 0;
        *(unsigned char *)(c + STAR_OFF(phase) + off) = 0;
        *(int *)(c + STAR_OFF(radius) + off2) = 0x38000;
        *(unsigned short *)(c + STAR_OFF(heading) + off2) = 0;
        *(unsigned short *)(c + STAR_OFF(delay) + off2) = data_ov002_02100130[i] << 6;
        *(int *)(c + STAR_OFF(x) + off) = -0x10000;
        *(int *)(c + STAR_OFF(y) + off2) = 0;
        *(unsigned short *)(c + STAR_OFF(angSpeed) + off2) = 0x100;
        *(int *)(c + STAR_OFF(speed) + off) = 0x5800;
        *(int *)(c + STAR_OFF(targetX) + off2) = 0x80000;
        *(int *)(c + STAR_OFF(targetY) + off2) = 0x28000;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- func_ov002_020f43cc, 0x020f43cc, size 0x344 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f43cc
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
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f43cc(char *c, int i) {
    int idx = i;
    char *px;
    char *s;
    int off = idx * 0x4c;
    {
        unsigned short v = *(unsigned short *)(c + STAR_OFF(delay) + off);
        if (v != 0) {
            *(unsigned short *)(c + STAR_OFF(delay) + off) = v - 1;
            return;
        }
    }
    if (*(unsigned char *)(c + STAR_OFF(phase) + off) == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + STAR_OFF(speed) + off);
            *(int *)(c + STAR_OFF(x) + off) += FX_MUL(tv, spd);
        }
        idxa = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + STAR_OFF(speed) + off);
            *(int *)(c + STAR_OFF(y) + off) += FX_MUL(tv, spd);
        }
        px = c + off;
        if (*(int *)(c + STAR_OFF(speed) + off) <= -0x2000)
            *(int *)(c + STAR_OFF(speed) + off) = *(int *)(c + STAR_OFF(speed) + off) + 0x120;
        {
            s = c + idx * 0x4c;
            int dx = (((StaffRoll *)s)->stars[0].targetX - *(int *)px) >> 12;
            int dy = (((StaffRoll *)s)->stars[0].targetY - *(int *)(c + STAR_OFF(y) + off)) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                (*(unsigned char *)(c + STAR_OFF(phase) + off))++;
                ((StaffRoll *)s)->stars[0].orbitTravel = 0;
                *(unsigned short *)(c + STAR_OFF(heading) + off) = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)cstd::atan2(dy, dx);
                unsigned short cur = *(unsigned short *)(c + STAR_OFF(heading) + off);
                if (a > cur) {
                    unsigned short nv;
                    *(unsigned short *)(c + STAR_OFF(heading) + off) += 0x100;
                    nv = *(unsigned short *)(c + STAR_OFF(heading) + off);
                    if (a <= nv)
                        *(unsigned short *)(c + STAR_OFF(heading) + off) = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    *(unsigned short *)(c + STAR_OFF(heading) + off) -= 0x100;
                    nv2 = *(unsigned short *)(c + STAR_OFF(heading) + off);
                    if (nv2 <= a)
                        *(unsigned short *)(c + STAR_OFF(heading) + off) = a;
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
        idxa = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + STAR_OFF(radius) + off);
            *(int *)(c + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }
        idxa = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + STAR_OFF(radius) + off);
            *(int *)(c + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }
        *(unsigned short *)(c + STAR_OFF(heading) + off) -=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
        *(int *)(c + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
    }
    if (idx != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)((StaffRoll *)c)->stars[0].orbitTravel < 0x10000U)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- func_ov002_020f4710, 0x020f4710, size 0x31c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f4710
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f4710(char *c, int i)
{
    int off = i * 0x4c;

    if (*(u8 *)(c + STAR_OFF(phase) + off) == 0) {
        int idx;

        idx = *(u16 *)(c + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(s32 *)(c + STAR_OFF(radius) + off);
            *(s32 *)(c + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }

        idx = *(u16 *)(c + STAR_OFF(heading) + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(s32 *)(c + STAR_OFF(radius) + off);
            *(s32 *)(c + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(u16 *)(c + STAR_OFF(heading) + off) -= *(u16 *)(c + STAR_OFF(angSpeed) + off);
        *(s32 *)(c + STAR_OFF(orbitTravel) + off) += *(u16 *)(c + STAR_OFF(angSpeed) + off);

        if (((StaffRoll *)c)->stars[0].phase != 0) {
            *(u16 *)(c + STAR_OFF(angSpeed) + off) += 8;
        } else if (*(u16 *)(c + STAR_OFF(angSpeed) + off) < 0x200) {
            *(u16 *)(c + STAR_OFF(angSpeed) + off) += 6;
            if (*(u16 *)(c + STAR_OFF(angSpeed) + off) > 0x200)
                *(u16 *)(c + STAR_OFF(angSpeed) + off) = 0x200;
        }

        if ((u32) * (s32 *)(c + STAR_OFF(orbitTravel) + off) < (u32)data_ov002_02100100[i])
            return;
        if (((StaffRoll *)c)->holdOrbit != 0)
            return;

        (*(u8 *)(c + STAR_OFF(phase) + off))++;
        *(u16 *)(c + STAR_OFF(flyTimer) + off) = 0;

        if (i == 0 || i == 3) {
            *(s32 *)(c + STAR_OFF(speed) + off) = 0x2000;
        } else {
            *(s32 *)(c + STAR_OFF(speed) + off) = -0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if (i != 2)
            return;
        if (((StaffRoll *)c)->holdOrbit == 0)
            *(s32 *)(c + STAR_OFF(speed) + off) = 0x2000;
        return;
    }

    (*(u16 *)(c + STAR_OFF(flyTimer) + off))++;
    *(s32 *)(c + STAR_OFF(x) + off) += *(s32 *)(c + STAR_OFF(speed) + off);

    if (func_ov002_020f5a94(c) == 3 && i == 2) {
        *(s32 *)(c + STAR_OFF(speed) + off) += 0x300;
    } else if (i == 0 || i == 3) {
        *(s32 *)(c + STAR_OFF(speed) + off) += 0x300;
    } else {
        *(s32 *)(c + STAR_OFF(speed) + off) -= 0x300;
    }

    if (((StaffRoll *)c)->stars[0].flyTimer < 0x60)
        return;

    {
        int o2 = (int)((unsigned long long)i) * 0x4c;
        (*(u8 *)(c + STAR_OFF(step) + o2))++;
        *(u16 *)(c + STAR_OFF(loops) + o2) = 0;
        *(u8 *)(c + STAR_OFF(phase) + off) = 0;
        *(s32 *)(c + STAR_OFF(radius) + o2) = 0x38000;
        *(u16 *)(c + STAR_OFF(heading) + o2) = 0x8000;
        *(u16 *)(c + STAR_OFF(delay) + o2) = (u16)(data_ov002_02100110[i] << 6);

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                *(u16 *)(c + STAR_OFF(delay) + o2) = 0x40;
        }
    }

    *(s32 *)(c + STAR_OFF(x) + off) = 0x110000;
    *(s32 *)(c + STAR_OFF(y) + off) = 0;
    *(u16 *)(c + STAR_OFF(angSpeed) + off) = 0x100;
    *(s32 *)(c + STAR_OFF(speed) + off) = 0x5800;
    *(s32 *)(c + STAR_OFF(targetX) + off) = 0x80000;
    *(s32 *)(c + STAR_OFF(targetY) + off) = 0x28000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- func_ov002_020f4a2c, 0x020f4a2c, size 0x344 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f4a2c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f4a2c(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

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

        if (*(int *)(c + STAR_OFF(speed) + off) <= -0x2000)
            *(int *)(c + STAR_OFF(speed) + off) = *(int *)(c + STAR_OFF(speed) + off) + 0x120;

        dx = (*(int *)(c + STAR_OFF(targetX) + off) - *(int *)(c + STAR_OFF(x) + off)) >> 12;
        dy = (*(int *)(c + STAR_OFF(targetY) + off) - *(int *)(c + STAR_OFF(y) + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
            *(int *)(c + STAR_OFF(orbitTravel) + off) = 0;
            *(unsigned short *)(c + STAR_OFF(heading) + off) = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = *(unsigned short *)(c + STAR_OFF(heading) + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)(c + STAR_OFF(heading) + off) += 0x100;
            nv = *(unsigned short *)(c + STAR_OFF(heading) + off);
            if (target <= nv)
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)(c + STAR_OFF(heading) + off) -= 0x100;
            nv2 = *(unsigned short *)(c + STAR_OFF(heading) + off);
            if (nv2 <= target)
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
            return;
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

    *(unsigned short *)(c + STAR_OFF(heading) + off) -=
        *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
    *(int *)(c + STAR_OFF(orbitTravel) + off) += *(unsigned short *)(c + STAR_OFF(angSpeed) + off);

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)((StaffRoll *)c)->stars[0].orbitTravel < 0x10000)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_ov002_020f4d70, 0x020f4d70, size 0x2a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f4d70
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f4d70(char *c, int i)
{
    int off = i * 0x4c;

    if (*(u16 *)(c + STAR_OFF(frame) + off) == 0) {
        if (*(u16 *)(c + STAR_OFF(frameTimer) + off) == 0)
            (*(u16 *)(c + STAR_OFF(loops) + off))++;
    }

    if (*(u8 *)(c + STAR_OFF(phase) + off) == 0) {
        int idx;

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

        *(u16 *)(c + STAR_OFF(heading) + off) += *(u16 *)(c + STAR_OFF(angSpeed) + off);
        *(s32 *)(c + STAR_OFF(orbitTravel) + off) += *(u16 *)(c + STAR_OFF(angSpeed) + off);

        if (((StaffRoll *)c)->stars[0].phase != 0) {
            *(u16 *)(c + STAR_OFF(angSpeed) + off) += 8;
        } else {
            if (*(u16 *)(c + STAR_OFF(angSpeed) + off) < 0x200) {
                *(u16 *)(c + STAR_OFF(angSpeed) + off) += 6;
                if (*(u16 *)(c + STAR_OFF(angSpeed) + off) > 0x200)
                    *(u16 *)(c + STAR_OFF(angSpeed) + off) = 0x200;
            }
        }

        if ((u32) * (s32 *)(c + STAR_OFF(orbitTravel) + off) < (u32)data_ov002_021000e0[i])
            return;

        if (((StaffRoll *)c)->holdOrbit != 0)
            return;

        (*(u8 *)(c + STAR_OFF(phase) + off))++;
        *(u16 *)(c + STAR_OFF(flyTimer) + off) = 0;
        *(s32 *)(c + STAR_OFF(speed) + off) = -0x2000;
        return;
    }

    (*(u16 *)(c + STAR_OFF(flyTimer) + off))++;
    *(s32 *)(c + STAR_OFF(x) + off) += *(s32 *)(c + STAR_OFF(speed) + off);
    *(s32 *)(c + STAR_OFF(speed) + off) -= 0x300;

    if (((StaffRoll *)c)->stars[0].flyTimer < 0x70)
        return;

    {
        int o2 = (unsigned)i * 0x4c;
        (*(u8 *)(c + STAR_OFF(step) + o2))++;
        *(s32 *)(c + STAR_OFF(radius) + o2) = 0x38000;
        *(u16 *)(c + STAR_OFF(heading) + o2) = 0x8000;
        *(u8 *)(c + STAR_OFF(phase) + off) = 0;
        *(u16 *)(c + STAR_OFF(loops) + o2) = 0;
        *(u16 *)(c + STAR_OFF(delay) + o2) = data_ov002_021000f0[i] << 6;

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                *(u16 *)(c + STAR_OFF(delay) + o2) = 0x40;
        }
    }

    *(s32 *)(c + STAR_OFF(x) + off) = 0x110000;
    *(s32 *)(c + STAR_OFF(y) + off) = 0;
    *(u16 *)(c + STAR_OFF(angSpeed) + off) = 0x100;
    *(s32 *)(c + STAR_OFF(speed) + off) = 0x5800;
    *(s32 *)(c + STAR_OFF(targetX) + off) = 0x80000;
    *(s32 *)(c + STAR_OFF(targetY) + off) = 0x28000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- func_ov002_020f5010, 0x020f5010, size 0x318 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5010
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
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5010(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

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
            *(int *)(c + STAR_OFF(speed) + off) = *(int *)(c + STAR_OFF(speed) + off) - 0x120;

        dx = (*(int *)(c + STAR_OFF(targetX) + off) - *(int *)(c + STAR_OFF(x) + off)) >> 12;
        dy = (*(int *)(c + STAR_OFF(targetY) + off) - *(int *)(c + STAR_OFF(y) + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
            *(int *)(c + STAR_OFF(orbitTravel) + off) = 0;
            *(unsigned short *)(c + STAR_OFF(heading) + off) = 0xc000;
            return;
        }

        {
            int raw = cstd::atan2(dy, dx);
            ang = *(unsigned short *)(c + STAR_OFF(heading) + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)(c + STAR_OFF(heading) + off) += 0x100;
            nv = *(unsigned short *)(c + STAR_OFF(heading) + off);
            if (target <= nv)
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)(c + STAR_OFF(heading) + off) -= 0x100;
            nv2 = *(unsigned short *)(c + STAR_OFF(heading) + off);
            if (nv2 <= target)
                *(unsigned short *)(c + STAR_OFF(heading) + off) = target;
            return;
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

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)((StaffRoll *)c)->stars[0].orbitTravel < 0x10000)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- func_ov002_020f5328, 0x020f5328, size 0x28c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5328
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5328(char *c, int i)
{
    int off = i * 0x4c;

    if (*(unsigned char *)(c + STAR_OFF(phase) + off) == 0) {
        int idx;
        {
            idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + STAR_OFF(radius) + off);
            *(int *)(c + STAR_OFF(x) + off) = FX_MUL(tv, spd) + 0x80000;
        }
        {
            idx = *(unsigned short *)(c + STAR_OFF(heading) + off) >> 4;
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + STAR_OFF(radius) + off);
            *(int *)(c + STAR_OFF(y) + off) = FX_MUL(tv, spd) + 0x60000;
        }

        *(unsigned short *)(c + STAR_OFF(heading) + off) +=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);
        *(int *)(c + STAR_OFF(orbitTravel) + off) +=
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off);

        if (((StaffRoll *)c)->stars[0].phase != 0) {
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off) += 8;
        } else if (*(unsigned short *)(c + STAR_OFF(angSpeed) + off) < 0x200) {
            *(unsigned short *)(c + STAR_OFF(angSpeed) + off) += 6;
            if (*(unsigned short *)(c + STAR_OFF(angSpeed) + off) > 0x200)
                *(unsigned short *)(c + STAR_OFF(angSpeed) + off) = 0x200;
        }

        if ((unsigned int)*(int *)(c + STAR_OFF(orbitTravel) + off) <
            (unsigned int)data_ov002_02100150[i])
            return;
        if (((StaffRoll *)c)->holdOrbit != 0)
            return;

        *(unsigned char *)(c + STAR_OFF(phase) + off) += 1;
        *(unsigned short *)(c + STAR_OFF(flyTimer) + off) = 0;

        if (i == 1 || i == 3) {
            *(int *)(c + STAR_OFF(speed) + off) = 0x2000;
        } else {
            *(int *)(c + STAR_OFF(speed) + off) = -0x2000;
        }
        return;
    }

    *(unsigned short *)(c + STAR_OFF(flyTimer) + off) += 1;
    *(int *)(c + STAR_OFF(x) + off) += *(int *)(c + STAR_OFF(speed) + off);
    if (i == 1 || i == 3) {
        *(int *)(c + STAR_OFF(speed) + off) += 0x300;
    } else {
        *(int *)(c + STAR_OFF(speed) + off) -= 0x300;
    }

    if (((StaffRoll *)c)->stars[0].flyTimer < 0x60)
        return;

    {
        int off2 = (int)((unsigned long long)i) * 0x4c;
        *(unsigned char *)(c + STAR_OFF(step) + off2) += 1;
        *(unsigned short *)(c + STAR_OFF(loops) + off2) = 0;
        *(unsigned char *)(c + STAR_OFF(phase) + off) = 0;
        *(int *)(c + STAR_OFF(radius) + off2) = 0x38000;
        *(unsigned short *)(c + STAR_OFF(heading) + off2) = 0;
        *(unsigned short *)(c + STAR_OFF(delay) + off2) =
            (unsigned short)(data_ov002_02100160[i] << 6);
        *(int *)(c + STAR_OFF(x) + off) = -0x10000;
        *(int *)(c + STAR_OFF(y) + off2) = 0;
        *(unsigned short *)(c + STAR_OFF(angSpeed) + off2) = 0x100;
        *(int *)(c + STAR_OFF(speed) + off) = 0x5800;
        *(int *)(c + STAR_OFF(targetX) + off2) = 0x80000;
        *(int *)(c + STAR_OFF(targetY) + off2) = 0x28000;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov002_020f55b4, 0x020f55b4, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f55b4
extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f55b4(char *c, int i) {
    char *e = c + i * 0x4c;
    if (((StaffRoll *)e)->stars[0].frame != 0x10)
        return;
    *(unsigned char *)(c + STAR_OFF(step) + i * 0x4c) += 1;
    ((StaffRoll *)e)->stars[0].radius = 0x38000;
    ((StaffRoll *)e)->stars[0].heading = data_ov002_021000d8[i];
    ((StaffRoll *)e)->stars[0].phase = 0;
    ((StaffRoll *)e)->stars[0].loops = 0;
    ((StaffRoll *)e)->stars[0].orbitTravel = 0;
    ((StaffRoll *)e)->stars[0].angSpeed = 0x200;
    ((StaffRoll *)e)->stars[0].kiraDefParam = 0x1000;
    ((StaffRoll *)e)->stars[0].kiraSysParam = 0x200;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- func_ov002_020f562c, 0x020f562c, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f562c
extern "C" void func_ov002_020f562c(C* self, int idx) {
    unsigned char sel = ((StaffRoll *)self)->stars[idx].step;
    (self->*data_ov002_02110eec[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov002_020f5678, 0x020f5678, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5678
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5678(char *base, int i)
{
    StaffRoll::Star *e = (StaffRoll::Star *)base + i;
    e->kiraDefParam = 0xccc;
    e->kiraSysParam = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- func_ov002_020f569c, 0x020f569c, size 0x124 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f569c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f569c(void *self, int i)
{
    char *p = (char *)self;
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

    if (ax == tx && by == ty)
    {
        *(unsigned char *)(p + STAR_OFF(step) + idx) =
            *(unsigned char *)(p + STAR_OFF(step) + idx) + 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov002_020f57c0, 0x020f57c0, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f57c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f57c0(StaffRoll::Star *arr, int idx) {
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov002_020f5848, 0x020f5848, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5848
extern "C" void func_ov002_020f5848(C* self, int idx) {
    unsigned char sel = ((StaffRoll *)self)->stars[idx].step;
    (self->*data_ov002_02110e64[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- func_ov002_020f5894, 0x020f5894, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5894
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5894(char *a, int i)
{
    int off;
    unsigned short *p34;
    short s2c;
    if (i != 0) return;
    off = i * 0x4c;
    p34 = (unsigned short *)(a + STAR_OFF(kiraTimer) + off);
    *p34 = *p34 + 1;
    if (*(int *)(a + STAR_OFF(kiraDefParam) + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int *)(a + STAR_OFF(kiraDefParam) + off) =
                *(int *)(a + STAR_OFF(kiraDefParam) + off) + 0x199;
            if (*(int *)(a + STAR_OFF(kiraDefParam) + off) >= 0x1000)
                *(int *)(a + STAR_OFF(kiraDefParam) + off) = 0x1000;
        }
    }
    s2c = *(short *)(a + STAR_OFF(kiraSysParam) + off);
    if (s2c != 0x800) {
        if (*(unsigned short *)(a + off + STAR_OFF(kiraPeriods)) >= 5) {
            if ((*p34 & 3) == 3) {
                *(short *)(a + STAR_OFF(kiraSysParam) + off) = s2c + 0x199;
                if (*(short *)(a + STAR_OFF(kiraSysParam) + off) >= 0x800)
                    *(short *)(a + STAR_OFF(kiraSysParam) + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e) return;
    *p34 = 0;
    {
        unsigned short *p3e = (unsigned short *)(a + STAR_OFF(kiraPeriods) + off);
        *p3e = *p3e + 1;
    }
}
#undef STAR_OFF
#undef WINDOW_OFF
#undef CREDIT_OFF
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov002_020f5990, 0x020f5990, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5990
void func_ov002_020f5990(char* c)
{
    int i;
    StaffRoll::Star *s = ((StaffRoll *)c)->stars;
    {
        int *cnt = &((StaffRoll *)c)->kiraStar;
        *cnt = *cnt + 1;
        *cnt = *cnt & 3;
    }
    for (i = 0; i < 4; i++, s++) {
        if (s->active == 0)
            continue;
        (((C *)c)->*data_ov002_02110e7c[s->state])(i);
        func_ov002_020f39ec(c, i);
        if (s->sparkling == 0)
            continue;
        if (i != ((StaffRoll *)c)->kiraStar)
            continue;
        func_ov002_020f2790(c, (s16)(s->x >> 12), (s16)(s->y >> 12), s->kiraDefParam,
                            s->kiraSysParam);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- func_ov002_020f5a6c, 0x020f5a6c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5a6c
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- func_ov002_020f5a94, 0x020f5a94, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5a94
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- func_ov002_020f5ad4, 0x020f5ad4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5ad4
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- func_ov002_020f5b24, 0x020f5b24, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b24
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b24(char *thiz)
{
    data_0209d454 &= ~6;
    data_0209d454 |= 8;
    func_ov002_020f2f64(thiz);
    *(volatile unsigned int *)0x4001000 &= ~0xe000;
    ((StaffRoll *)thiz)->window[0].active = 0;
    func_ov002_020f2a48(thiz);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- func_ov002_020f5b7c, 0x020f5b7c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b7c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b7c(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- func_ov002_020f5b80, 0x020f5b80, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b80
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b80(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- func_ov002_020f5b84, 0x020f5b84, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b84
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b84(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- func_ov002_020f5b88, 0x020f5b88, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b88
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b88(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- func_ov002_020f5b8c, 0x020f5b8c, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b8c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b8c(char *p)
{
    ((StaffRoll *)p)->holdOrbit = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- func_ov002_020f5b98, 0x020f5b98, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b98
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b98(StaffRoll::Star *e) {
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- func_ov002_020f5bcc, 0x020f5bcc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5bcc
extern "C" { /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5bcc(char *p) {
    int i;
    for (i = 0; i < 4; i++) {
        if (((StaffRoll *)p)->stars[0].active)
            ((StaffRoll *)p)->stars[0].holdFrame = 1;
        p += 0x4c;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 81 -- func_ov002_020f5bf4, 0x020f5bf4, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5bf4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5bf4(char* c)
{
    if (((StaffRoll *)c)->stars[3].active == 0)
        return;
    ((StaffRoll *)c)->stars[3].x = 0x80000;
    ((StaffRoll *)c)->stars[3].y = 0x60000;
    ((StaffRoll *)c)->stars[3].delay = 0;
    ((StaffRoll *)c)->stars[3].state = 1;
    ((StaffRoll *)c)->stars[3].kiraDefParam = 0xccc;
    ((StaffRoll *)c)->stars[3].kiraSysParam = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 82 -- func_ov002_020f5c40, 0x020f5c40, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5c40
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5c40(char *p)
{
    if (!((StaffRoll *)p)->stars[2].active)
        return;
    ((StaffRoll *)p)->stars[2].x = 0x80000;
    ((StaffRoll *)p)->stars[2].y = 0x60000;
    ((StaffRoll *)p)->stars[2].delay = 0;
    ((StaffRoll *)p)->stars[2].state = 1;
    ((StaffRoll *)p)->stars[2].kiraDefParam = 0xccc;
    ((StaffRoll *)p)->stars[2].kiraSysParam = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 83 -- func_ov002_020f5c88, 0x020f5c88, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5c88
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5c88(char* c)
{
    if (((StaffRoll *)c)->stars[1].active == 0)
        return;
    ((StaffRoll *)c)->stars[1].x = 0x80000;
    ((StaffRoll *)c)->stars[1].y = 0x60000;
    ((StaffRoll *)c)->stars[1].delay = 0;
    ((StaffRoll *)c)->stars[1].state = 1;
    ((StaffRoll *)c)->stars[1].kiraDefParam = 0xccc;
    ((StaffRoll *)c)->stars[1].kiraSysParam = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 84 -- func_ov002_020f5cd0, 0x020f5cd0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5cd0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5cd0(StaffRoll::Star *c) {
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 85 -- func_ov002_020f5d34, 0x020f5d34, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5d34
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5d34(u8 *self)
{
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 86 -- func_ov002_020f5dd8, 0x020f5dd8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5dd8
extern PMF0 data_ov002_02110f34[];
extern "C" void func_ov002_020f5dd8(C* c, int idx) {
  (c->*data_ov002_02110f34[idx])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 87 -- func_ov002_020f5e18, 0x020f5e18, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5e18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5e18(void *t)
{
    func_ov002_020f37fc((char*)t);
    func_ov002_020f2f9c(t, 4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 88 -- func_ov002_020f5e38, 0x020f5e38, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5e38
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5e38(void *t)
{
    func_ov002_020f3828((int*)t);
    func_ov002_020f2f9c(t, 4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 89 -- func_ov002_020f5e58, 0x020f5e58, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5e58
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5e58(void *t)
{
    func_ov002_020f3828((int*)t);
    func_ov002_020f2f9c(t, 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 90 -- func_ov002_020f5e78, 0x020f5e78, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5e78
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5e78(void *t)
{
    func_ov002_020f3828((int*)t);
    func_ov002_020f2f9c(t, 2);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 91 -- func_ov002_020f5e98, 0x020f5e98, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5e98
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5e98(void *t)
{
    func_ov002_020f3828((int*)t);
    func_ov002_020f2f9c(t, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 92 -- func_ov002_020f5eb8, 0x020f5eb8, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5eb8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5eb8(void *t)
{
    func_ov002_020f3828((int*)t);
    func_ov002_020f2f9c(t, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 93 -- func_ov002_020f5ed8, 0x020f5ed8, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5ed8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5ed8(void *t) { func_ov002_020f3828((int*)t); }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 94 -- func_ov002_020f5ee4, 0x020f5ee4, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5ee4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5ee4(void *t) { func_ov002_020f3828((int*)t); }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 95 -- func_ov002_020f5ef0, 0x020f5ef0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5ef0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5ef0(char *p) { func_ov002_020f3954(p); }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 96 -- func_ov002_020f5efc, 0x020f5efc, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5efc
/* func_ov002_020f5efc @ 0x20f5efc (ov002) -- tail-call veneer to LoadFont (0x201fcd4) with r0=0.
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5efc(void) {
    LoadFont(0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 97 -- func_ov002_020f5f0c, 0x020f5f0c, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5f0c
extern "C" void func_ov002_020f5f0c(C *c, int idx) {
    (c->*data_ov002_02110f9c[idx])();
    func_ov002_020f2e30((StaffRoll *)c, idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 98 -- func_ov002_020f5f60, 0x020f5f60, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5f60
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5f60(char *c)
{
    void *p = ((StaffRoll *)c)->starChars;
    if (p == 0)
        return;
    Memory::operator_delete2(p);
    ((StaffRoll *)c)->starChars = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 99 -- func_ov002_020f5f8c, 0x020f5f8c, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5f8c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f5f8c(void* c){
  func_ov002_020f26e0(c);
  func_ov002_020f3978((char*)c);
  func_ov002_020f2eac(c);
  return func_ov002_020f2990((char*)c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 100 -- func_ov002_020f5fb8, 0x020f5fb8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5fb8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f5fb8(void* c){
  func_ov002_020f5990((char*)c);
  func_ov002_020f37a0((C37*)c);
  func_ov002_020f2dd4((C2dd4*)c);
  return func_ov002_020f27e8((char*)c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 101 -- func_ov002_020f5fe4, 0x020f5fe4, size 0x3bc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5fe4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5fe4(char* c)
{
    int f;
    int r5;

    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    GXS::SetGraphicsMode(0);

    data_0209d454 &= ~0x1f;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCG_BIN);
    DecompressLZ16(f, (void*)0x6400000);
    Deallocate((void*)f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_END_EUR_NCL_BIN);
    GX::LoadOBJPltt((const void *)f, 0, 0x200);
    Deallocate((void*)f);

    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3);
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1100;
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & ~3);
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & 0x43) | 0xe08;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & ~3) | 2;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & 0x43) | 0x800;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCG_BIN);
    DecompressLZ16(f, (void *)G2S::GetBG1CharPtr());
    Deallocate((void*)f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_4BIT_NCL_BIN);
    GX::LoadBGPltt((const void *)f, 0, 2);
    GXS::LoadBGPltt((const void *)f, 0, 0x200);
    Deallocate((void*)f);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK1_NSC_BIN);
    func_02056434((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 2;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0xc90;

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
    Deallocate((void*)r5);

    r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_8BIT_NCL_BIN);
    CP15::FlushAndInvalidateDataCache((u32)r5, 0x200);
    GX::SetBankForSubBGExtPltt(0x80);
    GXS::BeginLoadBGExtPltt();
    GXS::LoadBGExtPltt((const void *)r5, 0x4000, 0x200);
    GXS::LoadBGExtPltt((const void *)r5, 0x6000, 0x200);
    GXS::EndLoadBGExtPltt();
    Deallocate((void*)r5);

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_BRIGHT_NSC_BIN);
    func_020562b4((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & ~3) | 2;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0xa90;

    f = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DARK2_NSC_BIN);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    data_0209d454 |= 0x10;

    {
        int f6 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCG_BIN);
        r5 = LoadFile(ASSET_D_2D_STAFF_ROLL_CAKE_DOLL_NCL_BIN);
        DecompressLZ16(f6, (void*)0x6600000);
        GXS::LoadOBJPltt((const void *)r5, 0, 0x200);
        Deallocate((void*)f6);
        Deallocate((void*)r5);
    }

    ((StaffRoll *)c)->starChars = 0;
    if (((StaffRoll *)c)->starChars == 0) {
        ((StaffRoll *)c)->starChars = (u8 *)Memory::operator_new2(0x4000);
    }
    f = LoadFile(ASSET_D_2D_ED_STAR_NCG_BIN);
    DecompressLZ16(f, ((StaffRoll *)c)->starChars);
    Deallocate((void*)f);

    func_ov002_020f5ad4(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 102 -- func_ov002_020f63a0, 0x020f63a0, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f63a0Ev

int daDemo_c::func_ov002_020f63a0()
{
    func_ov002_020f5f60((char *)this->mStaffRoll);
    if (this->mStaffRoll != 0) {
        Memory::operator_delete2(this->mStaffRoll);
        this->mStaffRoll = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 103 -- func_ov002_020f63d4, 0x020f63d4, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f63d4Ev

int daDemo_c::func_ov002_020f63d4()
{
    func_ov002_020f5fb8(this->mStaffRoll);
    func_ov002_020f5f8c(this->mStaffRoll);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 104 -- func_ov002_020f63f8, 0x020f63f8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f63f8EPh

int daDemo_c::func_ov002_020f63f8(unsigned char *src)
{
    this->mCue = *src;
    func_ov002_020f5dd8((C *)this->mStaffRoll, (int)this->mCue);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 105 -- func_ov002_020f6424, 0x020f6424, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6424Ev

int daDemo_c::func_ov002_020f6424()
{
    func_ov002_020f5f0c((C *)this->mStaffRoll, (int)this->mCue);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 106 -- func_ov002_020f6448, 0x020f6448, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6448EPh

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 107 -- func_ov002_020f64ac, 0x020f64ac, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f64acEPc

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 108 -- func_ov002_020f6514, 0x020f6514, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f6514EPvh

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 109 -- func_ov002_020f65b8, 0x020f65b8, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f65b8Ev

void daDemo_c::anmModel_c::func_ov002_020f65b8()
{
    if (mTexSeq == 0)
        return;
    mTexSeq->Update(data);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 110 -- func_ov002_020f65ec, 0x020f65ec, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f65ecEv

void daDemo_c::anmModel_c::func_ov002_020f65ec() {
    if (mTexSeq != 0)
        mTexSeq->Advance();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 111 -- func_ov002_020f6618, 0x020f6618, size 0x160 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f6618EP13SharedFilePtriPS2_ihS3_i
int daDemo_c::anmModel_c::func_ov002_020f6618(SharedFilePtr *mdl, int nAnims, SharedFilePtr **anims,
                        int arg5, unsigned char texByte, SharedFilePtr **texs, int tsData)
{
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

// @symbol _ZN8daDemo_c10anmModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
 * The D0 variant here emits from the ~anmModel_c definition at ordinal 113. */

// @symbol _ZN8daDemo_c10anmModel_cD1Ev
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 114 -- func_ov002_020f6960, 0x020f6960, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c13simpleModel_c19func_ov002_020f6960EP13SharedFilePtri

int daDemo_c::simpleModel_c::func_ov002_020f6960(SharedFilePtr *fp, int n)
{
    mFile = fp;
    BMD_File *f = (BMD_File *)dExtSimpleModel_c::LoadFile(*mFile);
    return ((dExtModel_c *)this)->SetFile(f, 1, n) != 0 ? 1 : 0;
}

// @symbol _ZN8daDemo_c13simpleModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
 * The D0 variant here emits from the ~simpleModel_c definition at ordinal 116. */

// @symbol _ZN8daDemo_c13simpleModel_cD1Ev
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 117 -- func_ov002_020f6a50, 0x020f6a50, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c7param_cC2Ev

daDemo_c::param_c::param_c() {
    mScale[0].x = 0;
    mScale[0].y = 0;
    mScale[0].z = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 118 -- func_ov002_020f6a9c, 0x020f6a9c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6a9cEv

int daDemo_c::func_ov002_020f6a9c()
{
    MarkForDestruction();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 119 -- func_ov002_020f6ab8, 0x020f6ab8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6ab8EPh

int daDemo_c::func_ov002_020f6ab8(unsigned char *p)
{
    unsigned int id = ReadUnalignedInt(p);
    Sound::Play(1, id, mCamSpacePos);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 120 -- func_ov002_020f6ae4, 0x020f6ae4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6ae4EPh

int daDemo_c::func_ov002_020f6ae4(unsigned char *p)
{
    unsigned int id = ReadUnalignedInt(p);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, id, mCamSpacePos, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 121 -- func_ov002_020f6b28, 0x020f6b28, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6b28EPh

int daDemo_c::func_ov002_020f6b28(unsigned char *p)
{
    unsigned int v = ReadUnalignedInt(p);
    func_02012694(v, &mCamSpacePosX);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 122 -- func_ov002_020f6b4c, 0x020f6b4c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6b4cEPh
/* Script handler 15: turn to face the point whose whole-unit coordinates
 * are the unaligned shorts at p+0/+2/+4, and take the short at p+6 as the
 * horizontal speed. The legacy per-function file recorded its match under
 * mwccarm 1.2/sp2p3; this TU builds under the 2004/b56 pin. */

int daDemo_c::func_ov002_020f6b4c(unsigned char *p)
{
    struct { int x, y, z; } target;
    int y, z;
    z = ReadUnalignedShort(p + 4) << 12;
    y = ReadUnalignedShort(p + 2) << 12;
    target.x = ReadUnalignedShort(p) << 12;
    target.y = y;
    target.z = z;
    this->mAngleY = Vec3_HorzAngle(&this->mPos, (const Vector3 *)&target);
    this->mPrevAngleY = this->mAngleY;
    this->mHorzSpeed = ReadUnalignedShort(p + 6);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 123 -- func_ov002_020f6bc0, 0x020f6bc0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6bc0EPh

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 124 -- func_ov002_020f6c24, 0x020f6c24, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6c24EPh

int daDemo_c::func_ov002_020f6c24(unsigned char *src)
{
    this->mOpacity = src[0];
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 125 -- func_ov002_020f6c34, 0x020f6c34, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6c34EPh

int daDemo_c::func_ov002_020f6c34(unsigned char *src)
{
    unsigned char v = *src;
    ((anmModel_c *)this->mAnmModel)->func_ov002_020f6514((u8 *)&data_ov002_0210bc88, v);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 126 -- func_ov002_020f6c60, 0x020f6c60, size 0x1e8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6c60EPvii
extern "C" {  /* .c-derived member: C linkage for the whole block */
inline unsigned int inline_fn(unsigned int arg0)
{
  return arg0;
}


}

int daDemo_c::func_ov002_020f6c60(void *arg1, int arg2, int arg3)
{
  u8 *flag = (u8 *) arg1;
  Vec3 v;
  if (arg2 == gKuppaScriptFrame) {
      if ((*flag) != 0) {
          int r = (inline_fn(RandomIntInternal(&data_0209e650)) >> 20) & 0xfff;
          int m = FX_MUL(r, 0xa000);
          this->mMagnitude = m + 0x19000;
      }
      AddVec3((Vec3 *)(&this->mPosX), (Vec3 *)&((dCamera_c *)data_0209f318)->pos,
              (Vec3 *)(&this->mPosX));
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
    MulVec3Mat4x3((Vector3*)&data_ov002_0210b958, (Matrix4x3*)&data_0209b41c, (Vector3*)&v);
    Vec3_LslInPlace((int*)&v, 3);
    func_ov002_020f6f48((Vector3*)&v, 8);
  }
  this->mPrevAngleX = this->mAngleX;
  this->mPrevAngleY = this->mAngleY;
  this->mPrevAngleZ = this->mAngleZ;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 127 -- func_ov002_020f6e48, 0x020f6e48, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6e48EPh
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 128 -- func_ov002_020f6f48, 0x020f6f48, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6f48EP7Vector3i
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 129 -- func_ov002_020f7020, 0x020f7020, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7020Ev
/* Script handler 9: clear mFlags bit 0x2. */

int daDemo_c::func_ov002_020f7020()
{
    this->mFlags &= ~0x2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 130 -- func_ov002_020f7038, 0x020f7038, size 0x18c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7038Eii

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
        mParticleID[0] = Particle::System::New(mParticleID[0], 0x3b, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[1] = Particle::System::New(mParticleID[1], 0x3c, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[2] = Particle::System::New(mParticleID[2], 0x3d, pos.x, pos.y, pos.z, 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 131 -- func_ov002_020f71c4, 0x020f71c4, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f71c4Eii

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
        mParticleID[0] = Particle::System::New(mParticleID[0], 0x38, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[1] = Particle::System::New(mParticleID[1], 0x39, pos.x, pos.y, pos.z, 0, 0);
        mParticleID[2] = Particle::System::New(mParticleID[2], 0x3a, pos.x, pos.y, pos.z, 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 132 -- func_ov002_020f72bc, 0x020f72bc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f72bcEv

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
        Particle::System::New(mParticleID[0], 0x31, *((Fix12i *)&this->mPosX),
                              *((Fix12i *)&this->mPosY), *((Fix12i *)&this->mPosZ), 0, 0);
    mParticleID[1] =
        Particle::System::New(mParticleID[1], 0x32, *(Fix12i *)(&this->mPosX),
                              *(Fix12i *)(&this->mPosY), *(Fix12i *)(&this->mPosZ), 0, 0);
    mParticleID[2] =
        Particle::System::New(mParticleID[2], 0x33, *(Fix12i *)(&this->mPosX),
                              *(Fix12i *)(&this->mPosY), *((Fix12i *)&this->mPosZ), 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 133 -- func_ov002_020f7384, 0x020f7384, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7384EPhi

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 134 -- func_ov002_020f7410, 0x020f7410, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7410EPhi
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
        register Vector3 *p = ((param_c &)*(simpleModel_c *)this->mModel).mScale;
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
            if (t > 0x10) t = 0x10;
            G2x::SetBlendAlpha((volatile u16 *)0x4000050, 8, 1, t, 0x10 - t);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 135 -- func_ov002_020f7538, 0x020f7538, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7538EPhi
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
    Vec3 tmp, diff, scaled, sum;

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
    Vec3_Sub(&diff, (Vec3 *)&sViewEnd, (Vec3 *)base);
    tmp.x = diff.x;
    tmp.y = diff.y;
    tmp.z = diff.z;
    Vec3_MulScalar(&scaled, &tmp, this->mRate);
    Vec3_Add(&sum, (Vec3 *)base, &scaled);

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
// @symbol _ZN8daDemo_c19func_ov002_020f7780EPvi

int daDemo_c::func_ov002_020f7780(void *unused, int mode)
{
    dCamera_c *cam = (dCamera_c *)data_0209f318;

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

            idx = *(volatile unsigned char *)&data_0209f250;
            {
                short ang = ((dActor_c *)data_0209f394[idx])->mAngleY;

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
// @symbol _ZN8daDemo_c19func_ov002_020f79c0EPvi

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
// @symbol _ZN8daDemo_c19func_ov002_020f7bb8EPht

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

    if (func_02008b4c(&v1, &this->mPhase, &this->mMagnitude, (char *)tbl1) != 0) {
        return 0;
    }
    if (func_02008b4c(&v0, &this->mPhase, &this->mMagnitude, (char *)tbl2) != 0) {
        return 0;
    }

    this->mOpacity = 0x1f;
    cam = (dCamera_c *)data_0209f318;

    AddVec3((Vec3 *)&v0, (Vec3 *)&cam->pos, (Vec3 *)&v0);
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
/* ROM ordinal 139 -- func_ov002_020f7d74, 0x020f7d74, size 0x2b4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f7d74
extern "C" void func_ov002_020f7d74(daDemo_c* self, unsigned char* p, int a2, int a3)
{
    if ((data_ov002_02110b00 & 1) == 0) {
        data_ov002_02110b00 |= 1;
        data_ov002_0211104c[0] = data_ov002_0210b708;
        data_ov002_0211104c[1] = data_ov002_0210b678;
        data_ov002_0211104c[2] = data_ov002_0210b618;
        data_ov002_0211104c[3] = data_ov002_0210b620;
        data_ov002_0211104c[4] = data_ov002_0210b640;
        data_ov002_0211104c[5] = data_ov002_0210b628;
        data_ov002_0211104c[6] = data_ov002_0210b648;
        data_ov002_0211104c[7] = data_ov002_0210b8c0;
        data_ov002_0211104c[8] = data_ov002_0210b900;
        data_ov002_0211104c[9] = data_ov002_0210b938;
        data_ov002_0211104c[10] = data_ov002_0210b930;
        data_ov002_0211104c[11] = data_ov002_0210b928;
        data_ov002_0211104c[12] = data_ov002_0210b920;
        data_ov002_0211104c[13] = data_ov002_0210b918;
        data_ov002_0211104c[14] = data_ov002_0210b910;
        data_ov002_0211104c[15] = data_ov002_0210b908;
        data_ov002_0211104c[16] = data_ov002_0210b750;
        data_ov002_0211104c[17] = data_ov002_0210b8f8;
        data_ov002_0211104c[18] = data_ov002_0210b8f0;
        data_ov002_0211104c[19] = data_ov002_0210b8e8;
        data_ov002_0211104c[20] = data_ov002_0210b8e0;
        data_ov002_0211104c[21] = data_ov002_0210b8d8;
        data_ov002_0211104c[22] = data_ov002_0210b8d0;
        data_ov002_0211104c[23] = data_ov002_0210b8c8;
    }
    (self->*data_ov002_0211104c[p[6]])(p + 7, a2, a3);
}

// @symbol _ZN8daDemo_c16CleanupResourcesEv
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

// @symbol _ZN8daDemo_c16OnPendingDestroyEv
/* daDemo_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daDemo_c::OnPendingDestroy()
{
}

// @symbol _ZN8daDemo_c6RenderEv
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
            M48 *src;
        } s;
        s.src = (M48 *)&data_0209b41c;
        s.p = (char *)&mModel->mat4x3;
        s.cur = s.p;
        *(M48 *)s.cur = *s.src;
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

// @symbol _ZN8daDemo_c8BehaviorEv
int daDemo_c::Behavior()
{
    Vector3 v;
    Vec3 asr;
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
    Vec3_Asr(&asr, (Vec3 *)(&this->mPosX), 3);
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

// @symbol _ZN8daDemo_c13InitResourcesEv
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
                ->func_ov002_020f6618((SharedFilePtr *)&data_ov085_0213074c, 1,
                                      (SharedFilePtr **)&data_ov002_0210b60c, 1, 1,
                                      (SharedFilePtr **)&data_ov002_0210b608, -1) == 0)
            return 0;
    } else if (param1 == 0x13) {
        this->mAnmModel = new anmModel_c;
        if (this->mAnmModel == 0)
            return 0;
        t = (*(volatile unsigned char *)&data_0209f2d8 == 2);
        if (t == 0) {
            if (((anmModel_c *)this->mAnmModel)
                    ->func_ov002_020f6618((SharedFilePtr *)&data_ov002_02110b98, 1,
                                          (SharedFilePtr **)&data_ov002_0210b610, 1, 1,
                                          (SharedFilePtr **)&data_ov002_0210b600, 0x16) == 0)
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
                ->func_ov002_020f6618((SharedFilePtr *)data_ov009_02113c20, 1,
                                      (SharedFilePtr **)&data_ov002_0210b604, 0, 0, 0, -1) == 0)
            return 0;
    }
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

// @symbol daDemo_c_classInit
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
