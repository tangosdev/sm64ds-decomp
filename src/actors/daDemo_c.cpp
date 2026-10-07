//cpp
/* daDemo_c -- the cutscene object used by the intro/demo sequences (DEMO
 * profile, spawn id on the registry).
 *
 * ROM evidence: _ZTS8daDemo_c at ov002:0x0210b940, _ZTI8daDemo_c at
 * 0x0210b94c (si_class over _ZTI8dActor_c at arm9:0x0208e390),
 * _ZTV8daDemo_c at 0x0210bd60. The nested model helpers read as
 * _ZTSN8daDemo_c10anmModel_cE / _ZTSN8daDemo_c13simpleModel_cE over the
 * shared _ZTIN8daDemo_c7param_cE tail base at 0x0210b888. The run is
 * 0x020f1f70..0x020f8808.
 *
 * daDemo_c's out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1 at 0x020f1f70 and D0 at
 * 0x020f1f94, then a D2 the cartridge does not keep; anmModel_c and
 * simpleModel_c emit the same D1/D0/D2 shape. Source order is
 * ROM-ascending so emission order stays ROM-ascending; the compiler
 * places simpleModel_c's D0/D1 pair out of order on its own. The
 * `_ZThn80_` Animation-base thunks live in the classInit TU at
 * ov002:0x020f8838/0x020f8848; this TU's copies are discarded.
 *
 * The vague-linkage typeinfo copies name the model family by the
 * project's own spellings (ModelAnim for dExtAnmModel_c, Model for
 * dExtSimpleModel_c, Animation for dExtFrameCtrl_c, ModelBase for
 * dExtModel_c), so they are deadstripped and the derived records'
 * base pointers are aliased onto the canonical _ZTI rows. See
 * notes/model-rtti-names.md.
 *
 * deslop leftovers:
 *   func_ov002_020f20f4 compiles to the ROM's loop only as C++ (every
 *   C-flavored spelling walks a byte-offset induction variable).
 *   func_ov002_020f2aec/2bf4/335c/340c/39ec keep opt_common_subs off
 *   brackets; 23f0 and 20f4 keep opt_strength_reduction /
 *   opt_loop_invariants off. The brackets bind per-member only under
 *   defer_codegen off.
 *   func_ov002_020f27e8 and the OAM tail family return a callee's r0 by
 *   falling off the end; the callees are declared int so the passthrough
 *   is spelled.
 *   The 24-entry KuppaScript dispatch table data_ov002_0211104c is a
 *   daDemo_c::* member-pointer table; its handlers and the directly-called
 *   actor helpers are daDemo_c members. func_ov002_020f7d74, the dispatcher,
 *   stays extern "C" and free: its caller func_0200e494 is arm9 C.
 *   func_ov002_020f26d4 is a ldr/bx veneer that tail-calls
 *   func_ov002_020f2630 through a raw void(*)(void); a member pointer
 *   cannot spell that cast, so both stay free.
 *   The unk_0d8 heap block (0x518 bytes, edStarKiraCallback_c embedded at
 *   +0x200) has no proven class, so its helpers and the C/PMF/PMF0
 *   dispatch tables over it stay free.
 *   Shadow windows Obj, OamEnt, ScrollState, E, C and S remain where a shared
 *   class is not yet proven; the ones over daDemo_c were moved onto real
 *   members (mModel, mModelAnim, mScaleX/Y/Z, param1, mOpacity).
 *   common.h must precede daDemo_c.h: ModelAnim.h's nested Matrix4x3
 *   otherwise wins and scalarizes Behavior's 12-word matrix copy.
 */

#include "types.h"
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"
#include "daDemo_c.h"
#include "SharedFilePtr.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"

struct OamAttr;

#pragma defer_codegen off

/* shadow struct 'Obj' */
struct Obj {
    char pad[0xd4];
    unsigned char *d4;
};

/* shadow struct 'OAM' */
struct OAM { static void RenderSub(OamAttr *, int, int); };

/* shadow typedef 's16' */
typedef signed short s16;

/* shadow struct 'ScrollState' */
struct ScrollState {
    s16 x0;  /* 0x0 */
    s16 x1;  /* 0x2 */
    s16 y0;  /* 0x4 */
    s16 y1;  /* 0x6 */
};

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

/* shadow struct 'S' */
struct S { unsigned char b[0x30]; };

/* shadow struct 'C' */
struct C;

/* shadow typedef 'void' */
typedef void (C::*PMF)(int);

/* pointer-to-member taking no args (dispatch tables at 02110f34/02110f9c) */
typedef void (C::*PMF0)();

/* pointer-to-member taking (cmd, a2, a3) — func_ov002_020f7d74's table */
typedef void (daDemo_c::*PMF3)(unsigned char*, int, int);

/* array-teardown callback used by the anmModel_c destructor */
typedef void (*VFN)(void *);

/* dispatch receiver of func_ov002_020f37a0 — control bytes at +0x15c/+0x15d */
struct C37 { unsigned char pad[0x15c]; unsigned char g; unsigned char idx; };
typedef void (C37::*PMF37)(int);
struct Entry37 { PMF37 pmf; };

/* dispatch receiver of func_ov002_020f2dd4 — control bytes at +0x1c8/+0x1c9 */
struct C2dd4 { unsigned char pad[0x1c8]; unsigned char g; unsigned char idx; };
typedef void (C2dd4::*PMF2dd4)(int);
struct Entry2dd4 { PMF2dd4 pmf; };

/* shadow struct 'E' */
struct E { unsigned char d[0x30]; };

/* shadow struct 'E5cd0' — anim-entry window of func_ov002_020f5cd0 */
struct E5cd0 {
    s32 f0;              /* 0x00 */
    s32 f4;              /* 0x04 */
    char p8[0x28 - 8];
    s32 f28;             /* 0x28 */
    u16 f2c;             /* 0x2c */
    char p2e[0x30 - 0x2e];
    u16 f30;             /* 0x30 */
    u16 f32;             /* 0x32 */
    char p34[0x3a - 0x34];
    u16 f3a;             /* 0x3a */
    char p3c[0x4c - 0x3c];
};

/* shadow struct 'E57c0' — anim-entry window of func_ov002_020f57c0 */
struct E57c0 {
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

/* shadow struct 'E5678' — 0x4c-stride window of func_ov002_020f5678 */
struct E5678 { char pad[0x4c]; };

/* shadow struct 'Ent5b98' — anim-entry window of func_ov002_020f5b98 */
struct Ent5b98 {
    char pad44[0x44];
    unsigned char f44;
    char pad45;
    unsigned char f46;
    unsigned char f47;
    char pad48[2];
    unsigned char f4a;
    char pad4b[0x4c - 0x4b];
};

/* shadow struct 'Obj2e30' — flag window of func_ov002_020f2e30 */
struct Obj2e30 {
    char pad1c4[0x1c4];
    unsigned short f1c4;
    unsigned short f1c6;
    unsigned char f1c8;
    unsigned char f1c9;
    unsigned char f1ca;
    unsigned char f1cb;
};

/* shadow struct 'ObjSeq' — TextureSequence window of func_ov002_020f65b8 */
struct ObjSeq { char pad[0x7c]; void *seq; };

/* shadow struct 'ObjV' — vtable window of daDemo_c::CleanupResources */
struct ObjV { virtual void v00(); virtual void m04(); };

/* shadow struct 'Obj6b' — receiver window of func_ov002_020f6b4c */
struct Obj6b {
    char pad5c[0x5c];
    struct Vector3 v5c;
    char pad8e[0x8e - (0x5c + 0xc)];
    s16 f8e;
    char pad94[0x94 - 0x90];
    s16 f94;
    char pad98[0x98 - 0x96];
    int f98;
};

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

/* shadow struct 'Sub' */
struct Sub {
    int f0;
    int f4;
    char pad8[0x28 - 8];
    int f28;
    s16 f2c;
    char pad2e[0x44 - 0x2e];
    u8 f44;
    u8 pad45;
    u8 f46;
    char pad47[0x49 - 0x47];
    u8 f49;
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

/* shadow typedef 's64' */
typedef long long s64;

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

/* shadow struct 'ModelBaseSh' — vtable window used by daDemo_c::Render;
   slot 5 is the ROM's render-entry method */
struct ModelBaseSh {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void m(int arg);
};

/* shadow typedef 'Obj' */
typedef struct Obj Obj;

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

#define LP(x) ((void*)(int)(x))
#define LI(x) ((int)((long long)(int)(x)))
#define A24 ((u16 *)(thiz->d4 + 0x24))
#define A30 ((u8 *)(thiz->d4 + 0x30))
#define A0C ((s16 *)(thiz->d4 + 0xc))
#define A1C ((s16 *)(thiz->d4 + 0x1c))
#define A14 ((u16 *)(thiz->d4 + 0x14))

extern "C" {
extern void _ZN3G2x13SetBlendAlphaEPVttttj( volatile void *reg, unsigned short a, unsigned short b, int c, int d);
extern unsigned char data_0209d454;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern OamAttr data_ov002_0210bddc;
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
/* local extern dropped: decl_common.h already declares _ZN6Memory16operator_delete2EPv(void*) */
extern void* data_0209f5bc;
extern u8 data_ov002_02111144;
extern u32 data_020a0db0;
extern void* _Znwj(unsigned int);
extern int func_ov002_020f2630(char* c);
extern struct OamAttr *data_ov002_0210b6a8[];
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi( int a, struct OamAttr *b, int c, int d, int e, int f, int g, int h);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void* rot, struct ParticleCallback* callback);
extern int data_ov002_02100140[];
extern void func_ov002_020f2790(char *c, int p1, int p2, int p3, short p4);
extern int func_ov002_020f2984(char *p);
extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii( int a, struct OamAttr* attr, int x, int y, int e, int f, int g, int h, int i, int j);
extern struct OamAttr* data_ov002_0210b988[];
extern void func_0201ef50(unsigned char v);
extern void func_0201f108(void);
extern int _ZN3OAM9RenderSubEP7OamAttriiii(struct OamAttr*, int, int, int, int);
extern struct OamAttr* data_ov002_0210bba0[];
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
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
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
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
void func_ov002_020f2e30(struct Obj2e30*, int);
extern PMF0 data_ov002_02110f9c[];
extern void SetSubBg3Offset(int a, int b);
extern void _ZN3GXS15SetGraphicsModeEi(int m);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void* dst);
extern void Deallocate(void* p);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern u32 _ZN3G2S13GetBG1CharPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_02056434(const void* src, int offset, int count);
extern char* _ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u16 a);
extern void _ZN3GXS18BeginLoadBGExtPlttEv(void);
extern void _ZN3GXS13LoadBGExtPlttEPKvjj(const void* p, u32 a, u32 b);
extern void _ZN3GXS16EndLoadBGExtPlttEv(void);
extern void func_020562b4(const void* src, u32 offset, u32 count);
extern void func_02056374(const void* src, u32 offset, u32 count);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void* _ZN6Memory13operator_new2Ej(u32 sz);
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
extern void _ZN5dPa_c7level_c20edStarKiraCallback_cC1Ev(char *self);
extern void func_ov002_020f5fe4(char *c);
extern int _ZN15dExtFrameCtrl_c7AdvanceEv(char*); /* local extern: untyped this. */
extern int _ZN15dExtFrameCtrl_c8FinishedEv(char*); /* local extern: untyped this. */
/* local extern dropped: decl_common.h declares func_ov002_020f6514(unsigned char*, void*, unsigned char) */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *thiz, void *mc);
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File*, int, int);
extern void __cxa_vec_ctor(void* obj, int a, int b, void* cb1, void* cb2);
extern void *_ZN7Vector3D1Ev(void *object);
extern void func_0203d384(void);
extern void _ZN7fBase_c18MarkForDestructionEv(void *);
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern int _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, struct Vector3 *v);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int e);
extern void func_02012694(unsigned int, void*);
extern short ReadUnalignedShort(unsigned char *p);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
/* local extern dropped: decl_common.h declares data_ov002_0210bc88 as 'int'; callers cast the address */
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void func_0201267c(int a, void *p);
extern void func_020731dc(void *object, void *destructor, void **node);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern int* Vec3_LslInPlace(int *v, int sh);
extern u16 data_0209b274;
extern char *data_0209f318;
extern int data_ov002_0210b614;
extern int data_ov002_02110b0c;
extern int data_ov002_02110c20[];
extern int data_ov002_02110db8;
extern int data_ov002_0210b958;
/* local extern dropped: decl_common.h declares data_0209b41c */
extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern void ApproachAngle(s16* target, s16 cur, int a, int step, int flag);
extern int _Z14ApproachLinearRiii(int* p, int a, int b);
extern void func_02012790(int x);
extern void func_0201f138(void);
extern void func_0201ef38(void);
extern void Math_Function_0203b0fc(int* p, int target, int scale, int max);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_MulScalar(Vec3* out, const Vec3* in, int scale);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern int data_ov002_02110b04;
extern int data_ov002_02110afc;
extern int data_ov002_02110b08;
extern int data_ov002_02110df4[];
extern int data_ov002_02110e0c[];
extern int data_ov002_02110e18[];
extern int data_ov002_02110de8[];
extern int data_ov002_02110e00[];
extern int data_ov002_02110dac[];
/* local extern dropped: decl_common.h declares data_0209b41c */
extern int data_0208ee44;
extern int data_ov002_0210b970[3];
extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void _Z15ApproachLinear2Rsss(short *v, short t, short s);
extern void _Z11UpdateAngleRssis(void *p, short a, int step, short d);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern int data_ov002_0210b964[3];
extern int data_ov002_02110ddc[3];
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
extern int data_ov002_02110b00;
extern PMF3 data_ov002_0211104c[24];
void _ZN9ModelBase12ApplyOpacityEjj(void* m, unsigned int opacity, unsigned int unused); /* local extern: untyped model pointer. */
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *c, void *cyl); /* local extern: untyped this, null cylinder. */
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *cyl); /* local extern: untyped this, null cylinder. */
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s32 x, s32 y, s32 z);
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 data_0209b41c;
extern unsigned char data_0209f2d8;
extern char data_ov002_0211094c;
extern char data_ov085_0213074c;
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
/* TUBUILD CONFLICT -- alternate declaration of _Znwj, from the legacy file for func_ov002_020f6448, NOT applied: extern void* _Znwj(unsigned int sz); */
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
/* TUBUILD CONFLICT -- alternate declaration of data_0209b274, from the legacy file for func_ov002_020f7384, NOT applied: extern unsigned short data_0209b274; */
/* TUBUILD CONFLICT -- alternate declaration of data_0209b274, from the legacy file for func_ov002_020f7410, NOT applied: extern unsigned short data_0209b274; */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov002_020f7410, NOT applied: extern void MulVec3Mat4x3(void *out, void *m, void *v); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_LslInPlace, from the legacy file for func_ov002_020f7410, NOT applied: extern void Vec3_LslInPlace(void *v, int sh); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov002_020f7410, NOT applied: extern int _Z14ApproachLinearRiii(int *p, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN3G2x13SetBlendAlphaEPVttttj, from the legacy file for func_ov002_020f7410, NOT applied: extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *reg, unsigned short a, unsigned short b, int c, int d); */
/* TUBUILD CONFLICT -- alternate declaration of MulVec3Mat4x3, from the legacy file for func_ov002_020f7538, NOT applied: extern void MulVec3Mat4x3(void* a, void* b, void* c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_LslInPlace, from the legacy file for func_ov002_020f7538, NOT applied: extern void Vec3_LslInPlace(void* v, int n); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov002_020f7538, NOT applied: extern int _Z14ApproachLinearRiii(int* dst, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209b274, from the legacy file for func_ov002_020f7538, NOT applied: extern unsigned short data_0209b274; */
/* TUBUILD CONFLICT -- alternate declaration of data_0209b274, from the legacy file for func_ov002_020f7780, NOT applied: extern unsigned short data_0209b274; */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov002_020f7780, NOT applied: extern void *data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f7780, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7780, NOT applied: extern short Vec3_HorzAngle(const int *v0, const int *v1); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209b274, from the legacy file for func_ov002_020f79c0, NOT applied: extern unsigned short data_0209b274; */
/* TUBUILD CONFLICT -- alternate declaration of Math_Function_0203b0fc, from the legacy file for func_ov002_020f79c0, NOT applied: extern void Math_Function_0203b0fc(int *p, int target, int scale, int max); */
/* TUBUILD CONFLICT -- alternate declaration of ApproachAngle, from the legacy file for func_ov002_020f79c0, NOT applied: extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void *data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of AddVec3, from the legacy file for func_ov002_020f7bb8, NOT applied: extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd5atan2E5Fix12IiES1_, from the legacy file for func_ov002_020f7bb8, NOT applied: extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209b41c, from the legacy file for _ZN8daDemo_c6RenderEv, NOT applied: extern M48 data_0209b41c; */
/* TUBUILD CONFLICT -- alternate declaration of _Znwj, from the legacy file for _ZN8daDemo_c13InitResourcesEv, NOT applied: extern "C" void *_Znwj(int sz); */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8daDemo_cD1Ev, 0x020f1f70, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_cD1Ev
daDemo_c::~daDemo_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8daDemo_cD0Ev, 0x020f1f94, size 0x38 */
/* -------------------------------------------------------------------------- */
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

void daDemo_c::func_ov002_020f1fcc()
{
    unsigned char *s = ((unsigned char *)this->unk_0d4);
    int i, v;
    if (*(unsigned short *)(s + 0x2e) == 0)
        return;
    (*(unsigned short *)((int)(s + 0x2c)))++;
    s = ((unsigned char *)this->unk_0d4);
    if (*(unsigned short *)(s + 0x2c) != 0x18)
        return;
    *(unsigned short *)(s + 0x2c) = 0;
    s = ((unsigned char *)this->unk_0d4);
    (*(unsigned short *)((int)(s + 0x2e)))--;
    s = ((unsigned char *)this->unk_0d4);
    v = *(unsigned short *)(s + 0x2e);
    if (v != 0) {
        _ZN3G2x13SetBlendAlphaEPVttttj(
            (volatile void *)0x4001050, 4, 0x28, v, 0x10 - v);
        return;
    }
    data_0209d454 &= ~4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    _ZN3G2x13SetBlendAlphaEPVttttj(
        (volatile void *)0x4001050, 0, 0x28, 0xc, 4);
    s = ((unsigned char *)this->unk_0d4);
    (*(unsigned char *)((int)(s + 0x34)))++;
    for (i = 0; i < 3; i++) {
        s = ((unsigned char *)this->unk_0d4);
        ((unsigned short *)(s + 0x24))[i] = (i + 1) << 4;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020f20f4, 0x020f20f4, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f20f4Ev
/* This member is C++, not C: under every C-flavored path here (the legacy .c
   file, an extern "C" block, #pragma cplusplus off) the backend strength-
   reduces the shared `d + i*2` into an r8 byte-walk; the ROM recomputes it
   (`add r0, r0, r9, lsl #1`), which only the C++ backend emits. extern "C" on
   the definition keeps the unmangled cartridge spelling without changing the
   backend. */
extern "C" struct OamAttr *data_ov002_0210be1c[];
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off

void daDemo_c::func_ov002_020f20f4()
{
  char *self = (char *)this;
  int i = 0;
  int zpos = 0;
  int zneg = 0;
  int two = 2;
  int m1 = -1;
  for (; i < 3; i++)
  {
    char *d = *((char **) (&this->unk_0d4));
    if ((*((u8 *) ((d + i) + 0x30))) != 0)
    {
      if ((data_020a0db0 & 1) == 0)
      {
        s16 *o = (s16 *) (d + ((i & 0xFFFFFFFFFFFFFFFF) * 2));
        s16 *q = (s16 *) ((int) (((char *) ((s16 *) (d + (i * 2)))) + 0xc));
        s16 vel = *((s16 *) (((char *) o) + 0x1c));
        s16 pos = *q;
        int *seed = &data_0209e650;
        *q = (s16) (pos + vel);
        RandomIntInternal(seed);
        d = *((char **) (&this->unk_0d4));
        int idx2 = ((unsigned long long) i) * 2;
        if ((*((s16 *) ((d + idx2) + 0x1c))) >= 0)
        {
          if ((*((s16 *) ((d + idx2) + 0xc))) >= 0x140)
          {
            *((u8 *) ((d + i) + 0x30)) = (u8) zpos;
            d = *((char **) (&this->unk_0d4));
            *((s16 *) ((d + idx2) + 0x24)) = (s16) ((i + 1) << 5);
          }
        }
        else
          if ((*((s16 *) ((d + idx2) + 0xc))) <= (-0x40))
        {
          *((u8 *) ((d + i) + 0x30)) = (u8) zneg;
          d = *((char **) (&this->unk_0d4));
          *((s16 *) ((d + idx2) + 0x24)) = (s16) ((i + 1) << 5);
        }
      }
      d = *((char **) (&this->unk_0d4));
      _ZN3OAM9RenderSubEP7OamAttriiii(data_ov002_0210be1c[i], *((s16 *) ((d + (i * 2)) + 0xc)), *((s16 *) ((d + (i * 2)) + 0x14)), m1, two);
    }
  }
}
#pragma opt_strength_reduction on
#pragma opt_loop_invariants on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020f2210, 0x020f2210, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f2210Ev

void daDemo_c::func_ov002_020f2210()
{
    Obj *thiz = (Obj *)this;
    int i;
    int base;
    u8 state;
    state = *(u8 *)(thiz->d4 + 0x34);
    if (state >= 3 && state > 5) return;
    i = 0;
    base = 0;
    for (; i < 3; i++) {
        u16 *p = (u16 *)(LI(thiz->d4) + LI(i) * 2);
        if (p[0x12] != 0) {
            *(volatile u16 *)LP(p + 0x12) -= 1;
            if (A24[i] == 0) {
                if (A30[i] == 0) {
                    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650);
                    if (r & 1) {
                        A0C[i] = -0x40;
                        A1C[i] = 1;
                    } else {
                        A0C[i] = 0x140;
                        A1C[i] = -1;
                    }
                    A14[i] = (u16)((r >> 16) % 48 + base);
                    A30[i] = 1;
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

void daDemo_c::func_ov002_020f2340()
{
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, *(unsigned short *)(*(char **)(&this->unk_0d4) + 8));
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, *(unsigned short *)(*(char **)(&this->unk_0d4) + 0xa));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020f237c, 0x020f237c, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f237cEv
/* func_ov002_020f237c — scroll sub-screen BG layers using a 4-short state
 * block pointed to by this+0xd4: y-- , x++ , then push offsets to HW.
 * Callees: SetSubBg0Offset, SetSubBg1Offset.
 */

void daDemo_c::func_ov002_020f237c()
{
    Obj *self = (Obj *)this;
    *(s16*)((char*)self->d4 + 2) -= 1;
    ((ScrollState *)self->d4)->x0 += 1;
    SetSubBg0Offset(((ScrollState *)self->d4)->x0, ((ScrollState *)self->d4)->y0);
    SetSubBg1Offset(((ScrollState *)self->d4)->x1, ((ScrollState *)self->d4)->y1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020f23d0, 0x020f23d0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f23d0Ev

int daDemo_c::func_ov002_020f23d0()
{
    _ZN6Memory16operator_delete2EPv(this->unk_0d4);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020f23f0, 0x020f23f0, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f23f0Ev
#pragma opt_strength_reduction off

int daDemo_c::func_ov002_020f23f0()
{
    switch (*(u8*)(((u8*)this->unk_0d4) + 0x34)) {
    case 0:
        {
            void* o = data_0209f5bc;
            if (((int (**)(void*))*(void**)o)[5](o) != 0) {
                {
                    u16* p = (u16*)(((int)((u8*)this->unk_0d4) + 8));
                    *p = *p + 1;
                }
                {
                    u16* p = (u16*)(((int)((u8*)this->unk_0d4) + 0xa));
                    *p = *p - 1;
                }
                if (*(u16*)(((u8*)this->unk_0d4) + 8) == 8) {
                    u8* p = (u8*)(((int)((u8*)this->unk_0d4) + 0x34));
                    *p = *p + 1;
                }
            }
        }
        break;
    case 1:
        if (this->unk_100 == 3) {
            data_ov002_02111144 = 1;
            {
                u8* p = (u8*)(((int)((u8*)this->unk_0d4) + 0x34));
                *p = *p + 1;
            }
        }
        break;
    case 2:
        if ((data_020a0db0 & 1) == 0) {
            {
                s16* p = (s16*)(((int)((u8*)this->unk_0d4) + 6));
                *p = *p + 1;
            }
            {
                s16* p = (s16*)(((int)((u8*)this->unk_0d4) + 4));
                *p = *p - 1;
            }
            if (*(s16*)(((u8*)this->unk_0d4) + 6) >= 0x80) {
                int i;
                int v;
                {
                    u8* p = (u8*)(((int)((u8*)this->unk_0d4) + 0x34));
                    *p = *p + 1;
                }
                i = 0;
                v = 1;
                for (; i < 3; i++) {
                    *(u16*)(((u8*)this->unk_0d4) + i * 2 + 0x24) = v;
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
        if (this->unk_100 == 4) {
            u8* p = (u8*)(((int)((u8*)this->unk_0d4) + 0x34));
            *p = *p + 1;
        }
        func_ov002_020f237c();
        break;
    case 5:
        if ((data_020a0db0 & 1) == 0) {
            {
                s16* p = (s16*)(((int)((u8*)this->unk_0d4) + 6));
                *p = *p + 1;
            }
            {
                s16* p = (s16*)(((int)((u8*)this->unk_0d4) + 4));
                *p = *p - 1;
            }
            if (*(s16*)(((u8*)this->unk_0d4) + 6) >= 0xe0) {
                {
                    u8* p = (u8*)(((int)((u8*)this->unk_0d4) + 0x34));
                    *p = *p + 1;
                }
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
#pragma opt_strength_reduction on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020f2630, 0x020f2630, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2630
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2630(char* c){
  short* p;
  *(void**)(c+0xd4) = _Znwj(0x36);
  (*(short**)(c+0xd4))[0] = 0x30;
  (*(short**)(c+0xd4))[2] = 0x20;
  (*(short**)(c+0xd4))[1] = 0;
  (*(short**)(c+0xd4))[3] = 0;
  (*(unsigned short**)(c+0xd4))[4] = 0xfff8;
  (*(short**)(c+0xd4))[5] = 0xc8;
  (*(short**)(c+0xd4))[0x17] = 0x10;
  p = *(short**)(c+0xd4); SetSubBg0Offset(p[0], p[2]);
  p = *(short**)(c+0xd4); SetSubBg1Offset(p[1], p[3]);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov002_020f26c4, 0x020f26c4, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f26c4EPh

int daDemo_c::func_ov002_020f26c4(unsigned char *src)
{
    unsigned char *dst = (unsigned char *)this;
    dst[0x100] = src[0];
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
    struct OamEnt *e = (struct OamEnt *)((char *)thiz + 0x208);
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
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            1, data_ov002_0210b6a8[idx], e->x, e->y,
            -1, 1, ((7 - s) << 10) + 0x1000, 0);
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
    *(int*)(c+0x508) = p3;
    *(s16*)(c+0x50c) = p4;
    *(u32*)(c+0x1fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c+0x1fc),
        0x3e,
        p1<<12, p2<<12, 0,
        0,
        (struct ParticleCallback*)(c+0x200));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020f27e8, 0x020f27e8, size 0x170 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f27e8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f27e8(char *c)
{
    if (*(unsigned char *)(c + 0x1ed) == 0) return *(unsigned char *)(c + 0x1ed);

    *(unsigned char *)(c + 0x1ec) += 1;

    if (*(int *)(c + 0x1e4) != 0x1000) {
        if (*(unsigned char *)(c + 0x1ec) == data_ov002_02100140[*(unsigned short *)(c + 0x1ea) & 3]) {
            *(int *)(c + 0x1e4) += 0x199;
            if (*(int *)(c + 0x1e4) >= 0x1000) *(int *)(c + 0x1e4) = 0x1000;
        }
    }

    if (*(short *)(c + 0x1e8) != 0x800) {
        if (*(unsigned short *)(c + 0x1ea) >= 5) {
            if ((*(unsigned char *)(c + 0x1ec) & 3) == 3) {
                *(short *)(c + 0x1e8) += 0x199;
                if (*(short *)(c + 0x1e8) >= 0x800) *(short *)(c + 0x1e8) = 0x800;
            }
        }
    }

    func_ov002_020f2790(c, 0x80, 0x60, *(int *)(c + 0x1e4), *(short *)(c + 0x1e8));
    func_ov002_020f2790(c, 0x80, 0x60, *(int *)(c + 0x1e4), *(short *)(c + 0x1e8));

    if (*(unsigned char *)(c + 0x1ec) >= 0x1e) {
        *(unsigned char *)(c + 0x1ec) = 0;
        *(unsigned short *)(c + 0x1ea) += 1;
    }
    if (*(unsigned short *)(c + 0x1ea) < 0xa) return *(unsigned short *)(c + 0x1ea);
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
    *(unsigned char*)(c + 0x1ed) = 1;
    *(int*)(c + 0x1e4) = 0x199;
    *(unsigned short*)(c + 0x1e8) = 0x199;
    *(unsigned char*)(c + 0x1ec) = 0;
    *(unsigned short*)(c + 0x1ea) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020f2984, 0x020f2984, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2984
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020f2984(char *p)
{
    p[493] = 0;
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

    if (*(u8*)(c + 0x1d9) == 0) return (int)c;

    x = *(int*)(c + 0x1cc) >> 0xc;
    y = *(int*)(c + 0x1d0) >> 0xc;
    idx = 0;
    if (GetOwnerLanguage() == 3) idx = 1;
    else if (GetOwnerLanguage() == 2 || GetOwnerLanguage() == 5) idx = 2;
    else if (GetOwnerLanguage() == 4) idx = 3;

    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov002_0210b988[idx], x, y, -1, 1, 0x1000, 0x1000, 0, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov002_020f2a48, 0x020f2a48, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a48
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a48(char *p)
{
    *(char *)(p + 0x1d8) = 1;
    *(char *)(p + 0x1d9) = 1;
    *(int *)(p + 0x1cc) = 524288;
    *(int *)(p + 0x1d0) = 393216;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020f2a68, 0x020f2a68, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a68(char *p)
{
    *(char *)(p + 0x1d8) = 0;
    *(char *)(p + 0x1d9) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020f2a78, 0x020f2a78, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a78
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a78(char *p, int i)
{
    *(char *)(p + i * 8 + 0x1c8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020f2a88, 0x020f2a88, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2a88
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2a88(char *c, int i)
{
  unsigned long new_var;
  *((short *) ((c + 0x1c4) + (i * 8))) = (*((u16 *) ((c + 0x1c4) + (i * 8)))) + 1;
  new_var = i;
  if ((*((u16 *) ((c + (new_var * 8)) + 0x1c4))) < 0x39)
  {
    return;
  }
  *((u16 *) ((c + (new_var * 8)) + 0x1c4)) = 0;
  *((u8 *) ((c + 0x1c9) + (new_var * 8))) = (*((u8 *) ((c + 0x1c9) + (new_var * 8)))) + 1;
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

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) == 0) {
        data_0209d454 |= 1;
    }
    *(unsigned short *)(c + 0x1c4 + idx * 8) += 1;
    *(unsigned short *)(c + 0x1c6 + idx * 8) += 1;

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) >= 2) {
        *(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) = 0;
        *(unsigned char *)(c + 0x1ca + idx * 8) += 1;
        cnt = *(unsigned char *)(c + idx * 8 + 0x1ca);
        _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4001050, 1, 0x36, cnt, 0x10 - cnt);
    }

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) < 30) {
        return;
    }
    *(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) = 0;
    *(unsigned char *)(c + 0x1c9 + idx * 8) += 1;
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

    *(unsigned short *)(c + 0x1c4 + idx * 8) += 1;
    *(unsigned short *)(c + 0x1c6 + idx * 8) += 1;

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) >= 2) {
        *(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) = 0;
        *(unsigned char *)(c + 0x1ca + idx * 8) += 1;
        cnt = *(unsigned char *)(c + idx * 8 + 0x1ca);
        _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4001050, 1, 0x36, 0x10 - cnt, cnt);
    }

    if (*(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) < 30) {
        return;
    }
    *(unsigned short *)(c + idx * 8 + 0x100 + 0xc4) = 0;
    *(unsigned char *)(c + 0x1c9 + idx * 8) += 1;
    *(unsigned char *)(c + 0x1cb + idx * 8) += 1;
    *(unsigned char *)(c + idx * 8 + 0x1ca) = 0;
    *(unsigned short *)(c + idx * 8 + 0x100 + 0xc6) = 0;
    data_0209d454 &= ~1;
    func_0201ef50(*(unsigned char *)(c + idx * 8 + 0x1cb));
}
#pragma opt_common_subs on
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020f2d0c, 0x020f2d0c, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2d0c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2d0c(char *c, int i)
{
  long new_var;
  *((short *) ((c + 0x1c4) + (i * 8))) = (*((unsigned short *) ((c + 0x1c4) + (i * 8)))) + 1;
  new_var = 8;
  char *r = c + (i * new_var);
  char *b = r + 0x100;
  if ((*((unsigned short *) (b + 0xc4))) < 0x39)
  {
    return;
  }
  *((short *) ((r + 0x100) + 0xc4)) = 0;
  *((unsigned char *) ((c + 0x1c9) + (i * new_var))) = (*((unsigned char *) ((c + 0x1c9) + (i * new_var)))) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov002_020f2d70, 0x020f2d70, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2d70
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2d70(char *c, int i)
{
  char *p;
  p = c + 0x1c4;
  *((short *) (p + (i * 8))) = (*((unsigned short *) (p + (i * 8)))) + 1;
  p = c + (i * 8);
  if ((*((unsigned short *) (p + 0x1c4))) < 0x1e)
  {
    return;
  }
  *((short *) (p + 0x1c4)) = 0;
  *((unsigned char *) ((c + 0x1c9) + (i * 8))) = (*((unsigned char *) ((c + 0x1c9) + (i * 8)))) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020f2dd4, 0x020f2dd4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2dd4
extern Entry2dd4 data_ov002_02110ebc[];
extern "C" void func_ov002_020f2dd4(C2dd4* c){
  if(c->g==0) return;
  (c->*data_ov002_02110ebc[c->idx].pmf)(0);
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
void func_ov002_020f2e30(struct Obj2e30 *self, int n) {
    if (n == 0) return;
    if (n == 0x15) return;
    self->f1c8 = 1;
    self->f1c9 = 0;
    self->f1c4 = 0;
    self->f1c6 = 0;
    self->f1ca = 0;
    n = (n - 1) << 1;
    self->f1cb = (unsigned char)n;
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
    p[456] = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov002_020f2eac, 0x020f2eac, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f2eac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f2eac(void* c) {
  int i;
  char* p = (char*)c;
  for (i = 0; i < 5; i++) {
    if (*(unsigned char*)(p+0x171) != 0) {
      _ZN3OAM9RenderSubEP7OamAttriiii(
          data_ov002_0210bba0[*(unsigned char*)(p+0x172)],
          *(int*)(p+0x160) >> 12,
          *(int*)(p+0x164) >> 12,
          -1,
          *(unsigned char*)(p+0x173));
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
        if (*(u8*)(c + 0x170) != 0) {
            *(s32*)(((int)c + 0x160)) -= a;
            *(s32*)(((int)c + 0x164)) -= b;
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
        if (*(unsigned char*)(self + 0x170) != 0) {
            (*(unsigned char*)(((int)self + 0x172)))++;
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
            if (_ZN8SaveData19IsCharacterUnlockedEj(2)) ok++;
        } else {
            if (_ZN8SaveData19IsCharacterUnlockedEj(1)) ok++;
        }
        if (ok) {
            int* p;
            *(u8*)(sl + 0x170) = 1;
            *(u8*)(sl + 0x171) = 1;
            p = data_ov002_0210b97c[idx];
            *(int*)(sl + 0x160) = (p[i * 4 + 0] + 0xe0) << 12;
            *(int*)(sl + 0x164) = (p[i * 4 + 1] + 0x66) << 12;
            *(u8*)(sl + 0x173) = (u8)p[i * 4 + 2];
            *(u8*)(sl + 0x172) = (u8)p[i * 4 + 3];
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
        *(unsigned char *)(p + 0x170) = 0;
        *(unsigned char *)(p + 0x171) = 0;
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

    t = *(unsigned short *)(o + 0x158 + idx);
    if (t != 0) {
        *(unsigned short *)(o + 0x158 + idx) = t - 1;
        return;
    }

    *(int *)(o + 0x140 + idx) -= *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x144 + idx) -= *(int *)(o + 0x154 + idx);
    *(int *)(o + 0x148 + idx) += *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x14c + idx) += *(int *)(o + 0x154 + idx);

    oldA = *(int *)(o + 0x130 + idx);
    oldB = *(int *)(o + 0x134 + idx);
    *(int *)(o + 0x130 + idx) = oldA + *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x134 + idx) += *(int *)(o + 0x154 + idx);

    if (*(int *)(o + 0x130 + idx) >= 0) *(int *)(o + 0x130 + idx) = 0;
    if (*(int *)(o + 0x134 + idx) >= 0) *(int *)(o + 0x134 + idx) = 0;
    if (*(int *)(o + 0x140 + idx) < 0) *(int *)(o + 0x140 + idx) = 0;
    if (*(int *)(o + 0x144 + idx) < 0) *(int *)(o + 0x144 + idx) = 0;
    if (*(int *)(o + 0x148 + idx) >= 0xff000) *(int *)(o + 0x148 + idx) = 0xff000;
    if (*(int *)(o + 0x14c + idx) >= 0xc0000) *(int *)(o + 0x14c + idx) = 0xc0000;
    if (*(int *)(o + 0x130 + idx) > 0) *(int *)(o + 0x130 + idx) = 0;
    if (*(int *)(o + 0x134 + idx) > 0) *(int *)(o + 0x134 + idx) = 0;

    cc = *(int *)(o + 0x130 + idx) - oldA;
    func_ov002_020f2f18(o, cc, *(int *)(o + 0x134 + idx) - oldB);

    ee = *(int *)(o + 0x148 + idx) >> 12;
    ff = *(int *)(o + 0x14c + idx) >> 12;
    cc = *(int *)(o + 0x140 + idx) >> 12;
    dd = *(int *)(o + 0x144 + idx) >> 12;
    *(unsigned short *)0x4001042 = ((cc << 8) & 0xff00) | (ee & 0xff);
    *(unsigned short *)0x4001046 = ((dd << 8) & 0xff00) | (ff & 0xff);
    SetSubBg1Offset(*(int *)(o + 0x130 + idx) >> 12, *(int *)(o + 0x134 + idx) >> 12);
    SetSubBg2Offset(*(int *)(o + 0x130 + idx) >> 12, *(int *)(o + 0x134 + idx) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov002_020f32e4, 0x020f32e4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f32e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f32e4(struct S *arr, int i)
{
    char *p = (char*)&arr[i];
    *(unsigned short*)(p + 0x158) = 0x10;
    *(int*)(p + 0x150) = 0x1800;
    *(int*)(p + 0x154) = 0x1800;
    *(unsigned char*)(p + 0x15e) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov002_020f3310, 0x020f3310, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3310
extern "C" void func_ov002_020f3310(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x30 + 0x15e));
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
    int *fld = (int *)(o + 0x144 + idx);
    *fld = *fld + 0x1800;
    if (*fld >= 0x66000) {
        *fld = 0x66000;
        *(unsigned char *)(o + idx + 0x15c) = 0;
        *(unsigned char *)(o + idx + 0x15e) = 0;
    }
    char *b = o + idx;
    int f140 = *(int *)(b + 0x140) >> 0xc;
    int f148 = *(int *)(b + 0x148) >> 0xc;
    int f14c = *(int *)(b + 0x14c) >> 0xc;
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
    int *fld = (int *)(o + 0x144 + idx);
    *fld = *fld + 0x1800;
    if (*fld >= 0x66000) {
        *fld = 0x66000;
        *(unsigned char *)(o + idx + 0x15c) = 0;
        *(unsigned char *)(o + idx + 0x15e) = 0;
    }
    char *b = o + idx;
    int f140 = *(int *)(b + 0x140) >> 0xc;
    int f148 = *(int *)(b + 0x148) >> 0xc;
    int f14c = *(int *)(b + 0x14c) >> 0xc;
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
    *(int *)(s + 0x140 + k) -= 0x1800;
    *(int *)(s + 0x144 + k) -= 0x1800;
    *(int *)(s + 0x148 + k) += 0x1800;
    *(int *)(s + 0x14c + k) += 0x1800;
    flag = 0;
    if (*(int *)(s + 0x140 + k) <= 0x80000) *(int *)(s + 0x140 + k) = 0x80000;
    if (*(int *)(s + 0x144 + k) <= 0x56000) *(int *)(s + 0x144 + k) = 0x56000;
    if (*(int *)(s + 0x148 + k) >= 0xfc000) *(int *)(s + 0x148 + k) = 0xfc000;
    if (*(int *)(s + 0x14c + k) >= 0xbc000) { *(int *)(s + 0x14c + k) = 0xbc000; flag++; }
    if (flag != 0) (*(unsigned char *)(s + 0x15e + k))++;
    a = *(int *)(s + 0x144 + k);
    b = *(int *)(s + 0x140 + k);
    c = *(int *)(s + 0x148 + k);
    d = *(int *)(s + 0x14c + k);
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
    *(int *)(c + 0x140 + off) = 0xe0000;
    *(int *)(c + 0x144 + off) = 0x66000;
    *(int *)(c + 0x148 + off) = 0xe0000;
    *(int *)(c + 0x14c + off) = 0x66000;
    *((volatile unsigned int *) 0x4001000) = ((*((volatile unsigned int *) 0x4001000)) & (~0xe000)) | 0x4000;
    *((volatile unsigned short *) 0x4001048) = (unsigned short) (((((*((volatile unsigned short *) 0x4001048)) & (~0x3f00)) | 0x1700) & 0xFFFFFFFFFFFFFFFFull) | 0x2000);
    *((volatile unsigned short *) 0x400104a) = (unsigned short) (((((*((volatile unsigned short *) 0x400104a)) & (~0x3f)) | 0x11) & 0xFFFFFFFFFFFFFFFFull) | 0x20);
    {
        int a = *(int *)(c + 0x140 + off) >> 12;
        int d = *(int *)(c + 0x148 + off) >> 12;
        int e = *(int *)(c + 0x14c + off) >> 12;
        int b = *(int *)(c + 0x144 + off) >> 12;
        *((volatile unsigned short *) 0x4001042) = (unsigned short) (((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *) 0x4001046) = (unsigned short) (((b << 8) & 0xff00) | (e & 0xff));
    }
    *(int *)(c + 0x130 + off) = -0x60000;
    *(int *)(c + 0x134 + off) = -0x38000;
    SetSubBg1Offset(*(int *)(c + 0x130 + off) >> 12, *(int *)(c + 0x134 + off) >> 12);
    SetSubBg2Offset(*(int *)(c + 0x130 + off) >> 12, *(int *)(c + 0x134 + off) >> 12);
    data_0209d454 |= 6;
    *(unsigned char *)(c + 0x15e + off) = *(unsigned char *)(c + 0x15e + off) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov002_020f3740, 0x020f3740, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3740
extern "C" void func_ov002_020f3740(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x30 + 0x15e));
    (self->*data_ov002_02110e9c[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov002_020f378c, 0x020f378c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f378c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f378c(struct E *base, int idx){
  base[idx].d[0x15e] = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov002_020f37a0, 0x020f37a0, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f37a0
extern Entry37 data_ov002_02110e4c[];
extern "C" void func_ov002_020f37a0(C37* c){
  if(c->g==0) return;
  (c->*data_ov002_02110e4c[c->idx].pmf)(0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov002_020f37fc, 0x020f37fc, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f37fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f37fc(char *c)
{
    func_ov002_020f3828((int*)c);
    *(unsigned char*)(c + 0x15c) = 1;
    *(unsigned char*)(c + 0x15d) = 2;
    *(unsigned char*)(c + 0x15e) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov002_020f3828, 0x020f3828, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3828
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f3828(int *c)
{
  c[0x140 / 4] = 0x80000;
  c[0x144 / 4] = 0x66000;
  c[0x148 / 4] = 0xfc000;
  c[0x14c / 4] = 0xbc000;
  *((volatile unsigned int *) 0x4001000) = ((*((volatile unsigned int *) 0x4001000)) & (~0xe000)) | 0x4000;
  *((volatile unsigned short *) 0x4001048) = (unsigned short) (((((*((volatile unsigned short *) 0x4001048)) & (~0x3f00)) | 0x1700) & 0xFFFFFFFFFFFFFFFFull) | 0x2000);
  *((volatile unsigned short *) 0x400104a) = (unsigned short) (((((*((volatile unsigned short *) 0x400104a)) & (~0x3f)) | 1) & 0xFFFFFFFFFFFFFFFFull) | 0x20);
  {
    int a = c[0x140 / 4] >> 12;
    int d = c[0x148 / 4] >> 12;
    int e = c[0x14c / 4] >> 12;
    int b = c[0x144 / 4] >> 12;
    *((volatile unsigned short *) 0x4001042) = (unsigned short) (((a << 8) & 0xff00) | (d & 0xff));
    *((volatile unsigned short *) 0x4001046) = (unsigned short) (((b << 8) & 0xff00) | (e & 0xff));
  }
  c[0x130 / 4] = -0x60000;
  c[0x134 / 4] = -0x38000;
  SetSubBg1Offset(c[0x130 / 4] >> 12, c[0x134 / 4] >> 12);
  SetSubBg2Offset(c[0x130 / 4] >> 12, c[0x134 / 4] >> 12);
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
    *(char *)(p + 0x15c) = 1;
    *(char *)(p + 0x15d) = 1;
    *(char *)(p + 0x15e) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov002_020f396c, 0x020f396c, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f396c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f396c(char *p)
{
    p[348] = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov002_020f3978, 0x020f3978, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3978
extern "C" {
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
extern void* data_ov002_0210b998[];
void func_ov002_020f3978(char* c){
  int i;
  for(i = 0; i < 4; i++){
    if(*(unsigned char*)(c+0x45)){
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, data_ov002_0210b998[i],
        *(int*)c >> 0xc, *(int*)(c+4) >> 0xc, -1, 1, 0);
    }
    c += 0x4c;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov002_020f39ec, 0x020f39ec, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f39ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_common_subs off
void func_ov002_020f39ec(char *self, int idx)
{
  char pad[4];
  (void)pad[0];
  int off = idx * 0x4c;
  unsigned short *p32 = (unsigned short *)(self + 0x32);
  unsigned short *ctr = (unsigned short *)((char *)p32 + off);
  *ctr = (unsigned short)(*ctr + 1);
  if (*ctr < 4) return;
  int zero = 0;
  *ctr = (unsigned short)zero;
  {
    unsigned short *p3a = (unsigned short *)(self + 0x3a);
    unsigned short *ctr2 = (unsigned short *)((char *)p3a + off);
    *ctr2 = (unsigned short)(*ctr2 + 1);
    char *row = self + off;
    if (*((unsigned char *)(row + 0x4a)) != 0) {
      if (*ctr2 >= 0x10) *ctr2 = 0x10;
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
    int v = data_ov002_021002a0[*((unsigned short *)(row + 0x3a))];
    int src_off = acc + ((v & 7) << 8);
    if (v & 8) src_off += 0x2000;
    MultiCopyHalf((void *)((*((int *)(self + 0x510))) + src_off), (void *)(base + (unsigned)dst_off), (unsigned)n100);
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
  struct Ent *arr = (struct Ent *) base;
  int a;
  int b;
  a = data_02082214[((arr[idx].angle >> 4) * 2) + 1];
  b = arr[idx].vx;
  arr[idx].out0 = ((int) (((((s64) a) * b) + 0x800) >> 0xc)) + 0xe0000;
  a = data_02082214[(arr[idx].angle >> 4) * 2];
  b = arr[idx].vx;
  arr[idx].out4 = ((int) (((((s64) a) * b) + 0x800) >> 0xc)) + 0x66000;
  if (arr[idx].vx > 0x18000)
  {
    arr[idx].vx = arr[idx].vx - 0x10;
  }
  arr[idx].angle += 0x200;
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

    idx = *(unsigned short*)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int*)(c + 8 + off);
        *(int*)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    idx = *(unsigned short*)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int*)(c + 8 + off);
        *(int*)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    *(int*)(c + 8 + off) += 0x30;

    dx = 0xe0 - (*(int*)(c + 0 + off) >> 12);
    dy = 0x46 - (*(int*)(c + 4 + off) >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        *(unsigned char*)(c + 0x47 + off) += 1;
        *(unsigned short*)(c + 0x30 + off) = 0;
        *(unsigned short*)(c + 0x2e + off) = 0xc000;
        *(int*)(c + 0x10 + off) = 0x20000;
        *(int*)(c + 0 + off) = 0xe0000;
        *(int*)(c + 4 + off) = 0x46000;
        return;
    }

    target = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
    ang = *(unsigned short*)(c + 0x2e + off);
    if (target > ang) {
        *(unsigned short*)(c + 0x2e + off) += 0x80;
        if (target <= *(unsigned short*)(c + 0x2e + off))
            *(unsigned short*)(c + 0x2e + off) = target;
    } else {
        if (ang <= target)
            return;
        *(unsigned short*)(c + 0x2e + off) -= 0x80;
        if (*(unsigned short*)(c + 0x2e + off) <= target)
            *(unsigned short*)(c + 0x2e + off) = target;
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
    if (*(u16*)(c + 0x30 + off) != 0) {
        *(u16*)(c + 0x30 + off) = *(u16*)(c + 0x30 + off) - 1;
        return;
    }
    *(s32*)(c + off) = -0x10000;
    *(s32*)(c + off + 4) = -0x10000;
    *(s32*)(c + off + 8) = 0x2000;
    *(u16*)(c + off + 0x2e) = 0;
    *(u16*)(c + 0x30 + off) = 0;
    *(u8*)(c + 0x47 + off) = *(u8*)(c + 0x47 + off) + 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov002_020f3d98, 0x020f3d98, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f3d98
extern "C" void func_ov002_020f3d98(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
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

    v = *(unsigned short *)(c + 0x30 + off);
    if (v != 0) {
        *(unsigned short *)(c + 0x30 + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        if (*(int *)(c + 8 + off) >= 0x2000)
            *(int *)(c + 8 + off) -= 0x120;

        dx = (*(int *)(c + 0x1c + off) - *(int *)(c + 0 + off)) >> 12;
        dy = (*(int *)(c + 0x20 + off) - *(int *)(c + 4 + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + 0x48 + off) += 1;
            *(int *)(c + 0x24 + off) = 0;
            *(unsigned short *)(c + 0x2e + off) = 0xc000;
            return;
        }

        target = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        ang = *(unsigned short *)(c + 0x2e + off);
        if (target > ang) {
            *(unsigned short *)(c + 0x2e + off) += 0x100;
            if (target <= *(unsigned short *)(c + 0x2e + off))
                *(unsigned short *)(c + 0x2e + off) = target;
        } else {
            if (ang <= target)
                return;
            *(unsigned short *)(c + 0x2e + off) -= 0x100;
            if (*(unsigned short *)(c + 0x2e + off) <= target)
                *(unsigned short *)(c + 0x2e + off) = target;
        }
        return;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
    }
    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
    }
    *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
    *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000)
        return;

    for (j = 0; j < 4; j++) {
        if (*(unsigned char *)(c + 0x44) != 0) {
            *(unsigned char *)(c + 0x47) = 1;
            *(unsigned short *)(c + 0x3c) = 0;
            *(unsigned char *)(c + 0x48) = 0;
            *(int *)(c + 0x10) = 0x38000;
            *(unsigned short *)(c + 0x2e) = data_ov002_021000c0[j];
            *(int *)(c + 0x24) = 0;
            *(unsigned short *)(c + 0x42) = 0x200;
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

    if (*(unsigned short*)(c + 0x3a + off) == 0) {
        if (*(unsigned short*)(c + 0x32 + off) == 0) {
            *(unsigned short*)(c + 0x3c + off) += 1;
        }
    }

    if (*(unsigned char*)(c + 0x48 + off) == 0) {
        int idx;
        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(unsigned short*)(c + 0x2e + off) -= *(unsigned short*)(c + 0x42 + off);
        *(int*)(c + 0x24 + off) += *(unsigned short*)(c + 0x42 + off);

        if (*(unsigned char*)(c + 0x48) != 0) {
            *(unsigned short*)(c + 0x42 + off) += 8;
        } else {
            if (*(unsigned short*)(c + 0x42 + off) < 0x200) {
                *(unsigned short*)(c + 0x42 + off) += 6;
                if (*(unsigned short*)(c + 0x42 + off) > 0x200)
                    *(unsigned short*)(c + 0x42 + off) = 0x200;
            }
        }

        if ((unsigned int)*(int*)(c + 0x24 + off) >= (unsigned int)data_ov002_02100120[i]
            && *(unsigned char*)(c + 0x514) == 0) {
            *(unsigned char*)(c + 0x48 + off) += 1;
            *(unsigned short*)(c + 0x36 + off) = 0;
            *(int*)(c + 8 + off) = 0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if ((unsigned int)*(int*)(c + 0x24 + off) < 0xc000)
            return;
        if (i != 2)
            return;
        if (*(unsigned char*)(c + 0x514) != 0)
            return;
        *(unsigned char*)(c + 0x48 + off) += 1;
        *(unsigned short*)(c + 0x36 + off) = 0;
        *(int*)(c + 8 + off) = 0x2000;
        return;
    }

    *(unsigned short*)(c + 0x36 + off) += 1;
    *(int*)(c + 0 + off) += *(int*)(c + 8 + off);
    *(int*)(c + 8 + off) += 0x300;
    if (*(unsigned short*)(c + 0x36) < 0x70)
        return;
    {
        int off2 = (int)((long long)i) * 0x4c;
        *(unsigned char*)(c + 0x47 + off2) += 1;
        *(unsigned short*)(c + 0x3c + off2) = 0;
        *(unsigned char*)(c + 0x48 + off) = 0;
        *(int*)(c + 0x10 + off2) = 0x38000;
        *(unsigned short*)(c + 0x2e + off2) = 0;
        *(unsigned short*)(c + 0x30 + off2) = data_ov002_02100130[i] << 6;
        *(int*)(c + 0 + off) = -0x10000;
        *(int*)(c + 4 + off2) = 0;
        *(unsigned short*)(c + 0x42 + off2) = 0x100;
        *(int*)(c + 8 + off) = 0x5800;
        *(int*)(c + 0x1c + off2) = 0x80000;
        *(int*)(c + 0x20 + off2) = 0x28000;
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
        unsigned short v = *(unsigned short *)(c + 0x30 + off);
        if (v != 0) {
            *(unsigned short *)(c + 0x30 + off) = v - 1;
            return;
        }
    }
    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + 8 + off);
            *(int *)(c + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }
        px = c + off;
        if (*(int *)(c + 8 + off) <= -0x2000)
            *(int *)(c + 8 + off) = *(int *)(c + 8 + off) + 0x120;
        {
            s = c + idx * 0x4c;
            int dx = (*(int *)(s + 0x1c) - *(int *)px) >> 12;
            int dy = (*(int *)(s + 0x20) - *(int *)(c + 4 + off)) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                (*(unsigned char *)(c + 0x48 + off))++;
                *(int *)(s + 0x24) = 0;
                *(unsigned short *)(c + 0x2e + off) = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
                unsigned short cur = *(unsigned short *)(c + 0x2e + off);
                if (a > cur) {
                    unsigned short nv;
                    *(unsigned short *)(c + 0x2e + off) += 0x100;
                    nv = *(unsigned short *)(c + 0x2e + off);
                    if (a <= nv)
                        *(unsigned short *)(c + 0x2e + off) = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    *(unsigned short *)(c + 0x2e + off) -= 0x100;
                    nv2 = *(unsigned short *)(c + 0x2e + off);
                    if (nv2 <= a)
                        *(unsigned short *)(c + 0x2e + off) = a;
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
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + 0x10 + off);
            *(int *)(c + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }
        *(unsigned short *)(c + 0x2e + off) -= *(unsigned short *)(c + 0x42 + off);
        *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);
    }
    if (idx != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000U)
        return;
    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (*(unsigned char *)(p + 0x44) != 0) {
                (*(unsigned char *)(p + 0x47))++;
                *(unsigned short *)(p + 0x3c) = 0;
                *(unsigned char *)(p + 0x48) = 0;
                *(int *)(p + 0x10) = 0x38000;
                *(unsigned short *)(p + 0x2e) = data_ov002_021000b8[j];
                if (func_ov002_020f5a94(c) == 3) {
                    if (j == 2) *(unsigned short *)(p + 0x2e) = 0;
                }
                *(int *)(p + 0x24) = 0;
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

    if (*(u8*)(c + 0x48 + off) == 0) {
        int idx;

        idx = *(u16*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(s32*)(c + 0x10 + off);
            *(s32*)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }

        idx = *(u16*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(s32*)(c + 0x10 + off);
            *(s32*)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(u16*)(c + 0x2e + off) -= *(u16*)(c + 0x42 + off);
        *(s32*)(c + 0x24 + off) += *(u16*)(c + 0x42 + off);

        if (*(u8*)(c + 0x48) != 0) {
            *(u16*)(c + 0x42 + off) += 8;
        } else if (*(u16*)(c + 0x42 + off) < 0x200) {
            *(u16*)(c + 0x42 + off) += 6;
            if (*(u16*)(c + 0x42 + off) > 0x200)
                *(u16*)(c + 0x42 + off) = 0x200;
        }

        if ((u32)*(s32*)(c + 0x24 + off) < (u32)data_ov002_02100100[i])
            return;
        if (*(u8*)(c + 0x514) != 0)
            return;

        (*(u8*)(c + 0x48 + off))++;
        *(u16*)(c + 0x36 + off) = 0;

        if (i == 0 || i == 3) {
            *(s32*)(c + 8 + off) = 0x2000;
        } else {
            *(s32*)(c + 8 + off) = -0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if (i != 2)
            return;
        if (*(u8*)(c + 0x514) == 0)
            *(s32*)(c + 8 + off) = 0x2000;
        return;
    }

    (*(u16*)(c + 0x36 + off))++;
    *(s32*)(c + 0 + off) += *(s32*)(c + 8 + off);

    if (func_ov002_020f5a94(c) == 3 && i == 2) {
        *(s32*)(c + 8 + off) += 0x300;
    } else if (i == 0 || i == 3) {
        *(s32*)(c + 8 + off) += 0x300;
    } else {
        *(s32*)(c + 8 + off) -= 0x300;
    }

    if (*(u16*)(c + 0x36) < 0x60)
        return;

    {
        int o2 = (int)((unsigned long long)i) * 0x4c;
        (*(u8*)(c + 0x47 + o2))++;
        *(u16*)(c + 0x3c + o2) = 0;
        *(u8*)(c + 0x48 + off) = 0;
        *(s32*)(c + 0x10 + o2) = 0x38000;
        *(u16*)(c + 0x2e + o2) = 0x8000;
        *(u16*)(c + 0x30 + o2) = (u16)(data_ov002_02100110[i] << 6);

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                *(u16*)(c + 0x30 + o2) = 0x40;
        }
    }

    *(s32*)(c + 0 + off) = 0x110000;
    *(s32*)(c + 4 + off) = 0;
    *(u16*)(c + 0x42 + off) = 0x100;
    *(s32*)(c + 8 + off) = 0x5800;
    *(s32*)(c + 0x1c + off) = 0x80000;
    *(s32*)(c + 0x20 + off) = 0x28000;
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

    v = *(unsigned short *)(c + 0x30 + off);
    if (v != 0) {
        *(unsigned short *)(c + 0x30 + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        if (*(int *)(c + 8 + off) <= -0x2000)
            *(int *)(c + 8 + off) = *(int *)(c + 8 + off) + 0x120;

        dx = (*(int *)(c + 0x1c + off) - *(int *)(c + 0 + off)) >> 12;
        dy = (*(int *)(c + 0x20 + off) - *(int *)(c + 4 + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + 0x48 + off) += 1;
            *(int *)(c + 0x24 + off) = 0;
            *(unsigned short *)(c + 0x2e + off) = 0xc000;
            return;
        }

        {
            int raw = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            ang = *(unsigned short *)(c + 0x2e + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)(c + 0x2e + off) += 0x100;
            nv = *(unsigned short *)(c + 0x2e + off);
            if (target <= nv)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)(c + 0x2e + off) -= 0x100;
            nv2 = *(unsigned short *)(c + 0x2e + off);
            if (nv2 <= target)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        return;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
    }

    *(unsigned short *)(c + 0x2e + off) -= *(unsigned short *)(c + 0x42 + off);
    *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000)
        return;

    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (*(unsigned char *)(p + 0x44) != 0) {
                *(unsigned char *)(p + 0x47) += 1;
                *(unsigned short *)(p + 0x3c) = 0;
                *(unsigned char *)(p + 0x48) = 0;
                *(int *)(p + 0x10) = 0x38000;
                *(unsigned short *)(p + 0x2e) = data_ov002_021000c8[j];
                if (func_ov002_020f5a94(c) == 3) {
                    if (j == 2)
                        *(unsigned short *)(p + 0x2e) = 0;
                }
                *(int *)(p + 0x24) = 0;
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

    if (*(u16*)(c + 0x3a + off) == 0) {
        if (*(u16*)(c + 0x32 + off) == 0)
            (*(u16*)(c + 0x3c + off))++;
    }

    if (*(u8*)(c + 0x48 + off) == 0) {
        int idx;

        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }

        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(u16*)(c + 0x2e + off) += *(u16*)(c + 0x42 + off);
        *(s32*)(c + 0x24 + off) += *(u16*)(c + 0x42 + off);

        if (*(u8*)(c + 0x48) != 0) {
            *(u16*)(c + 0x42 + off) += 8;
        } else {
            if (*(u16*)(c + 0x42 + off) < 0x200) {
                *(u16*)(c + 0x42 + off) += 6;
                if (*(u16*)(c + 0x42 + off) > 0x200)
                    *(u16*)(c + 0x42 + off) = 0x200;
            }
        }

        if ((u32)*(s32*)(c + 0x24 + off) < (u32)data_ov002_021000e0[i])
            return;

        if (*(u8*)(c + 0x514) != 0)
            return;

        (*(u8*)(c + 0x48 + off))++;
        *(u16*)(c + 0x36 + off) = 0;
        *(s32*)(c + 8 + off) = -0x2000;
        return;
    }

    (*(u16*)(c + 0x36 + off))++;
    *(s32*)(c + 0 + off) += *(s32*)(c + 8 + off);
    *(s32*)(c + 8 + off) -= 0x300;

    if (*(u16*)(c + 0x36) < 0x70)
        return;

    {
        int o2 = (unsigned)i * 0x4c;
        (*(u8*)(c + 0x47 + o2))++;
        *(s32*)(c + 0x10 + o2) = 0x38000;
        *(u16*)(c + 0x2e + o2) = 0x8000;
        *(u8*)(c + 0x48 + off) = 0;
        *(u16*)(c + 0x3c + o2) = 0;
        *(u16*)(c + 0x30 + o2) = data_ov002_021000f0[i] << 6;

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                *(u16*)(c + 0x30 + o2) = 0x40;
        }
    }

    *(s32*)(c + 0 + off) = 0x110000;
    *(s32*)(c + 4 + off) = 0;
    *(u16*)(c + 0x42 + off) = 0x100;
    *(s32*)(c + 8 + off) = 0x5800;
    *(s32*)(c + 0x1c + off) = 0x80000;
    *(s32*)(c + 0x20 + off) = 0x28000;
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

    v = *(unsigned short *)(c + 0x30 + off);
    if (v != 0) {
        *(unsigned short *)(c + 0x30 + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        if (*(int *)(c + 8 + off) >= 0x2000)
            *(int *)(c + 8 + off) = *(int *)(c + 8 + off) - 0x120;

        dx = (*(int *)(c + 0x1c + off) - *(int *)(c + 0 + off)) >> 12;
        dy = (*(int *)(c + 0x20 + off) - *(int *)(c + 4 + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + 0x48 + off) += 1;
            *(int *)(c + 0x24 + off) = 0;
            *(unsigned short *)(c + 0x2e + off) = 0xc000;
            return;
        }

        {
            int raw = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            ang = *(unsigned short *)(c + 0x2e + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)(c + 0x2e + off) += 0x100;
            nv = *(unsigned short *)(c + 0x2e + off);
            if (target <= nv)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)(c + 0x2e + off) -= 0x100;
            nv2 = *(unsigned short *)(c + 0x2e + off);
            if (nv2 <= target)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        return;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
    }

    *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
    *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000)
        return;

    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (*(unsigned char *)(p + 0x44) != 0) {
                *(unsigned char *)(p + 0x47) += 1;
                *(unsigned short *)(p + 0x3c) = 0;
                *(unsigned char *)(p + 0x48) = 0;
                *(int *)(p + 0x10) = 0x38000;
                *(unsigned short *)(p + 0x2e) = data_ov002_021000d0[j];
                *(int *)(p + 0x24) = 0;
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

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        int idx;
        {
            idx = *(unsigned short *)(c + 0x2e + off) >> 4;
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 0x00 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        {
            idx = *(unsigned short *)(c + 0x2e + off) >> 4;
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 0x04 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
        *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

        if (*(unsigned char *)(c + 0x48) != 0) {
            *(unsigned short *)(c + 0x42 + off) += 8;
        } else if (*(unsigned short *)(c + 0x42 + off) < 0x200) {
            *(unsigned short *)(c + 0x42 + off) += 6;
            if (*(unsigned short *)(c + 0x42 + off) > 0x200)
                *(unsigned short *)(c + 0x42 + off) = 0x200;
        }

        if ((unsigned int)*(int *)(c + 0x24 + off) < (unsigned int)data_ov002_02100150[i])
            return;
        if (*(unsigned char *)(c + 0x514) != 0)
            return;

        *(unsigned char *)(c + 0x48 + off) += 1;
        *(unsigned short *)(c + 0x36 + off) = 0;

        if (i == 1 || i == 3) {
            *(int *)(c + 0x08 + off) = 0x2000;
        } else {
            *(int *)(c + 0x08 + off) = -0x2000;
        }
        return;
    }

    *(unsigned short *)(c + 0x36 + off) += 1;
    *(int *)(c + 0x00 + off) += *(int *)(c + 0x08 + off);
    if (i == 1 || i == 3) {
        *(int *)(c + 0x08 + off) += 0x300;
    } else {
        *(int *)(c + 0x08 + off) -= 0x300;
    }

    if (*(unsigned short *)(c + 0x36) < 0x60)
        return;

    {
        int off2 = (int)((unsigned long long)i) * 0x4c;
        *(unsigned char *)(c + 0x47 + off2) += 1;
        *(unsigned short *)(c + 0x3c + off2) = 0;
        *(unsigned char *)(c + 0x48 + off) = 0;
        *(int *)(c + 0x10 + off2) = 0x38000;
        *(unsigned short *)(c + 0x2e + off2) = 0;
        *(unsigned short *)(c + 0x30 + off2) = (unsigned short)(data_ov002_02100160[i] << 6);
        *(int *)(c + 0x00 + off) = -0x10000;
        *(int *)(c + 0x04 + off2) = 0;
        *(unsigned short *)(c + 0x42 + off2) = 0x100;
        *(int *)(c + 0x08 + off) = 0x5800;
        *(int *)(c + 0x1c + off2) = 0x80000;
        *(int *)(c + 0x20 + off2) = 0x28000;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov002_020f55b4, 0x020f55b4, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f55b4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f55b4(char* c, int i){
  char* e = c + i*0x4c;
  if(*(unsigned short*)(e+0x3a) != 0x10) return;
  *(unsigned char*)(c + 0x47 + i*0x4c) += 1;
  *(int*)(e+0x10) = 0x38000;
  *(unsigned short*)(e+0x2e) = data_ov002_021000d8[i];
  *(unsigned char*)(e+0x48) = 0;
  *(unsigned short*)(e+0x3c) = 0;
  *(int*)(e+0x24) = 0;
  *(unsigned short*)(e+0x42) = 0x200;
  *(int*)(e+0x28) = 0x1000;
  *(unsigned short*)(e+0x2c) = 0x200;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- func_ov002_020f562c, 0x020f562c, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f562c
extern "C" void func_ov002_020f562c(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
    (self->*data_ov002_02110eec[sel])(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov002_020f5678, 0x020f5678, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5678
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5678(char *base, int i)
{
    struct E5678 *e = (struct E5678 *)base + i;
    *(int *)((char *)e + 0x28) = 0xccc;
    *(unsigned short *)((char *)e + 0x2c) = 0x155;
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

    if (*(unsigned short *)(p + 0x30 + idx) != 0)
    {
        *(unsigned short *)(p + 0x30 + idx) = *(unsigned short *)(p + 0x30 + idx) - 1;
        return;
    }

    *(int *)(p + idx + 0x28) = 0x1000;
    *(short *)(p + idx + 0x2c) = 0x200;

    *(int *)(p + idx) = *(int *)(p + idx) + *(int *)(p + 8 + idx);
    *(int *)(p + 4 + idx) = *(int *)(p + 4 + idx) + *(int *)(p + 0xc + idx);

    tx = data_ov002_02100190[i * 2];
    ty = data_ov002_02100190[i * 2 + 1];
    ax = *(int *)(p + idx) >> 12;
    by = *(int *)(p + 4 + idx) >> 12;

    if (*(int *)(p + 8 + idx) > 0)
    {
        if (ax > tx)
            *(int *)(p + idx) = tx << 12;
    }
    else
    {
        if (ax < tx)
            *(int *)(p + idx) = tx << 12;
    }

    if (*(int *)(p + 0xc + idx) > 0)
    {
        if (by > ty)
            *(int *)(p + 4 + idx) = ty << 12;
    }
    else
    {
        if (by < ty)
            *(int *)(p + 4 + idx) = ty << 12;
    }

    ax = *(int *)(p + idx) >> 12;
    by = *(int *)(p + 4 + idx) >> 12;

    if (ax == tx && by == ty)
    {
        *(unsigned char *)(p + 0x47 + idx) = *(unsigned char *)(p + 0x47 + idx) + 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov002_020f57c0, 0x020f57c0, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f57c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f57c0(struct E57c0* arr, int idx) {
  if (arr[idx].f3a != 0) return;
  if (arr[idx].f32 != 0) return;
  arr[idx].f49 = 1;
  arr[idx].f45 = 1;
  arr[idx].f47++;
  arr[idx].f8 = data_ov002_02100170[idx*2];
  arr[idx].fc = data_ov002_02100170[idx*2+1];
  if (idx == 0) arr[idx].f30 = 0x78;
  else arr[idx].f30 = 0x20;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov002_020f5848, 0x020f5848, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5848
extern "C" void func_ov002_020f5848(C* self, int idx) {
    unsigned char sel = *((unsigned char*)((char*)self + idx * 0x4c + 0x47));
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
    p34 = (unsigned short*)(a + 0x34 + off);
    *p34 = *p34 + 1;
    if (*(int*)(a + 0x28 + off) != 0x1000) {
        unsigned short v = *p34;
        if (v == 0xf || v == 0x1e) {
            *(int*)(a + 0x28 + off) = *(int*)(a + 0x28 + off) + 0x199;
            if (*(int*)(a + 0x28 + off) >= 0x1000) *(int*)(a + 0x28 + off) = 0x1000;
        }
    }
    s2c = *(short*)(a + 0x2c + off);
    if (s2c != 0x800) {
        if (*(unsigned short*)(a + off + 0x3e) >= 5) {
            if ((*p34 & 3) == 3) {
                *(short*)(a + 0x2c + off) = s2c + 0x199;
                if (*(short*)(a + 0x2c + off) >= 0x800) *(short*)(a + 0x2c + off) = 0x800;
            }
        }
    }
    if (*p34 < 0x1e) return;
    *p34 = 0;
    {
        unsigned short *p3e = (unsigned short*)(a + 0x3e + off);
        *p3e = *p3e + 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov002_020f5990, 0x020f5990, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5990
void func_ov002_020f5990(char* c)
{
    int i;
    Sub* s = (Sub*)c;
    {
        int* cnt = (int*)(((int)c + 0x1f8));
        *cnt = *cnt + 1;
        *cnt = *cnt & 3;
    }
    for (i = 0; i < 4; i++, s = (Sub*)((char*)s + 0x4c)) {
        if (s->f44 == 0) continue;
        (((C*)c)->*data_ov002_02110e7c[s->f46])(i);
        func_ov002_020f39ec(c, i);
        if (s->f49 == 0) continue;
        if (i != *(int*)(c + 0x1f8)) continue;
        func_ov002_020f2790(c, (s16)(s->f0 >> 12), (s16)(s->f4 >> 12), s->f28, s->f2c);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- func_ov002_020f5a6c, 0x020f5a6c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5a6c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5a6c(char *p){
  int i;
  for(i=0;i<4;i++){
    *(unsigned char*)(p+0x44)=0;
    *(unsigned char*)(p+0x45)=0;
    *(unsigned char*)(p+0x4a)=0;
    p+=0x4c;
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
        if (_ZN8SaveData19IsCharacterUnlockedEj(i))
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
    *(int*)(c+0x1f0) = 0;
    *(int*)(c+0x1f4) = 0;
    *(int*)(c+0x1f8) = 0;
    *(unsigned char*)(c+0x514) = 0;
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
    thiz[0x15c] = 0;
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
    p[1300] = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- func_ov002_020f5b98, 0x020f5b98, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5b98
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5b98(struct Ent5b98 *e)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (e->f44 != 0) {
            e->f46 = 2;
            e->f47 = 0;
            e->f4a = 0;
        }
        e++;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- func_ov002_020f5bcc, 0x020f5bcc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5bcc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5bcc(char *p){
  int i;
  for(i=0;i<4;i++){
    if(*(unsigned char*)(p+0x44)) *(unsigned char*)(p+0x4a)=1;
    p+=0x4c;
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
    if (*(unsigned char*)(c+0x128) == 0) return;
    *(int*)(c+0xe4) = 0x80000;
    *(int*)(c+0xe8) = 0x60000;
    *(unsigned short*)(c+0x114) = 0;
    *(unsigned char*)(c+0x12a) = 1;
    *(int*)(c+0x10c) = 0xccc;
    *(unsigned short*)(c+0x110) = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 82 -- func_ov002_020f5c40, 0x020f5c40, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5c40
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5c40(char *p)
{
    if (!*(unsigned char *)(p + 0xdc))
        return;
    *(int *)(p + 0x98) = 0x80000;
    *(int *)(p + 0x9c) = 0x60000;
    *(short *)(p + 0xc8) = 0;
    *(unsigned char *)(p + 0xde) = 1;
    *(int *)(p + 0xc0) = 0xccc;
    *(short *)(p + 0xc4) = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 83 -- func_ov002_020f5c88, 0x020f5c88, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5c88
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5c88(char* c)
{
    if (*(unsigned char*)(c+0x90) == 0) return;
    *(int*)(c+0x4c) = 0x80000;
    *(int*)(c+0x50) = 0x60000;
    *(unsigned short*)(c+0x7c) = 0;
    *(unsigned char*)(c+0x92) = 1;
    *(int*)(c+0x74) = 0xccc;
    *(unsigned short*)(c+0x78) = 0x155;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 84 -- func_ov002_020f5cd0, 0x020f5cd0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5cd0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5cd0(struct E5cd0 *c)
{
    int i;
    if (*(u8*)((char*)c + 0x44) == 0)
        return;
    c->f0 = 0x80000;
    c->f4 = 0x60000;
    c->f30 = 0;
    *(u8*)((char*)c + 0x46) = 1;
    c->f28 = 0xccc;
    c->f2c = 0x155;
    for (i = 0; i < 4; i++) {
        c->f3a = 0;
        c->f32 = 0;
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
        *(u8 *)(e + 0x44) = 1;
        *(u8 *)(e + 0x45) = 0;
        *(s16 *)(e + 0x2e) = 0;
        *(s16 *)(e + 0x38) = 0;
        *(int *)(e + 0) = 0x80000;
        *(int *)(e + 4) = 0x60000;
        *(s16 *)(e + 0x30) = v;
        *(s16 *)(e + 0x32) = 0;
        *(s16 *)(e + 0x34) = 0;
        *(s16 *)(e + 0x3e) = 0;
        *(int *)(e + 0x28) = 0x199;
        *(s16 *)(e + 0x2c) = 0x199;
        *(s16 *)(e + 0x38) = 0;
        *(u8 *)(e + 0x46) = 0;
        *(u8 *)(e + 0x47) = 0;
        *(u8 *)(e + 0x49) = 0;
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
extern "C" void func_ov002_020f5f0c(C* c, int idx){
  (c->*data_ov002_02110f9c[idx])();
  func_ov002_020f2e30((Obj2e30*)c, idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 98 -- func_ov002_020f5f60, 0x020f5f60, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f5f60
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f5f60(char *c)
{
    void *p = *(void**)(c + 0x510);
    if (p == 0)
        return;
    _ZN6Memory16operator_delete2EPv(p);
    *(void**)(c + 0x510) = 0;
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
    _ZN3GXS15SetGraphicsModeEi(0);

    data_0209d454 &= ~0x1f;

    f = LoadFile(0x267);
    DecompressLZ16(f, (void*)0x6400000);
    Deallocate((void*)f);

    f = LoadFile(0x268);
    _ZN2GX11LoadOBJPlttEPKvjj((const void*)f, 0, 0x200);
    Deallocate((void*)f);

    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3);
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1100;
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & ~3);
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & 0x43) | 0xe08;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & ~3) | 2;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & 0x43) | 0x800;

    f = LoadFile(0x24f);
    DecompressLZ16(f, (void*)_ZN3G2S13GetBG1CharPtrEv());
    Deallocate((void*)f);

    f = LoadFile(0x250);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)f, 0, 0x200);
    Deallocate((void*)f);

    f = LoadFile(0x254);
    func_02056434((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 2;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0xc90;

    if (GetOwnerLanguage() == 5) {
        r5 = LoadFile(0x271);
    } else if (GetOwnerLanguage() == 4) {
        r5 = LoadFile(0x26f);
    } else if (GetOwnerLanguage() == 3) {
        r5 = LoadFile(0x26d);
    } else if (GetOwnerLanguage() == 2) {
        r5 = LoadFile(0x26b);
    } else {
        r5 = LoadFile(0x251);
    }

    DecompressLZ16(r5, (void*)_ZN3G2S13GetBG3CharPtrEv());
    Deallocate((void*)r5);

    r5 = LoadFile(0x252);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)r5, 0x200);
    _ZN2GX22SetBankForSubBGExtPlttEt(0x80);
    _ZN3GXS18BeginLoadBGExtPlttEv();
    _ZN3GXS13LoadBGExtPlttEPKvjj((const void*)r5, 0x4000, 0x200);
    _ZN3GXS13LoadBGExtPlttEPKvjj((const void*)r5, 0x6000, 0x200);
    _ZN3GXS16EndLoadBGExtPlttEv();
    Deallocate((void*)r5);

    f = LoadFile(0x253);
    func_020562b4((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & ~3) | 2;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0xa90;

    f = LoadFile(0x255);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    data_0209d454 |= 0x10;

    {
        int f6 = LoadFile(0x256);
        r5 = LoadFile(0x257);
        DecompressLZ16(f6, (void*)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void*)r5, 0, 0x200);
        Deallocate((void*)f6);
        Deallocate((void*)r5);
    }

    *(int*)(c + 0x510) = 0;
    if (*(int*)(c + 0x510) == 0) {
        *(int*)(c + 0x510) = (int)_ZN6Memory13operator_new2Ej(0x4000);
    }
    f = LoadFile(0x22c);
    DecompressLZ16(f, *(void**)(c + 0x510));
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
    func_ov002_020f5f60((char*)this->unk_0d8);
    if (this->unk_0d8 != 0) {
        _ZN6Memory16operator_delete2EPv(this->unk_0d8);
        this->unk_0d8 = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 103 -- func_ov002_020f63d4, 0x020f63d4, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f63d4Ev

int daDemo_c::func_ov002_020f63d4()
{
    char *c = (char *)this;
    func_ov002_020f5fb8(*(void**)((char*)c+0xd8));
    func_ov002_020f5f8c(*(void**)((char*)c+0xd8));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 104 -- func_ov002_020f63f8, 0x020f63f8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f63f8EPh

int daDemo_c::func_ov002_020f63f8(unsigned char *src)
{
    this->unk_100 = *src;
    func_ov002_020f5dd8((C*)this->unk_0d8, (int)this->unk_100);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 105 -- func_ov002_020f6424, 0x020f6424, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6424Ev

int daDemo_c::func_ov002_020f6424()
{
    func_ov002_020f5f0c((C*)this->unk_0d8, (int)this->unk_100);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 106 -- func_ov002_020f6448, 0x020f6448, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6448EPh

int daDemo_c::func_ov002_020f6448(unsigned char *arg1)
{
    char *p;
    this->unk_100 = *arg1;
    p = (char*)_Znwj(0x518);
    if (p)
        _ZN5dPa_c7level_c20edStarKiraCallback_cC1Ev(p + 0x200);
    this->unk_0d8 = p;
    if (this->unk_0d8 == 0)
        return 0;
    func_ov002_020f5fe4((char*)this->unk_0d8);
    return func_ov002_020f6424();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 107 -- func_ov002_020f64ac, 0x020f64ac, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f64acEPc

void daDemo_c::anmModel_c::func_ov002_020f64ac(char *r4)
{
    char *c = (char *)this;
  _ZN15dExtFrameCtrl_c7AdvanceEv(c+0x50);
  if(!_ZN15dExtFrameCtrl_c8FinishedEv(c+0x50)) return;
  signed char r2 = *(signed char*)(r4 + (*(unsigned char*)(c+0x82) << 2) + 3);
  if(r2 < 0) return;
  func_ov002_020f6514((u8*)r4, (u8)(r2 & 0xff));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 108 -- func_ov002_020f6514, 0x020f6514, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f6514EPvh

void daDemo_c::anmModel_c::func_ov002_020f6514(void *tbl, unsigned char arg)
{
    unsigned char *self = (unsigned char *)this;
    u8 *e;
    int v;
    *(u8 *)(self + 0x82) = arg;
    e = (u8*)tbl + *(u8 *)(self + 0x82) * 4;
    v = (*(u8 *)(e + 2) == 0) ? 0x40000000 : 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        self,
        (void*)((struct Ent **)*(void **)(self + 0x74))[*(u8 *)(e + 0)]->out4,
        v, 0x1000, 0);
    {
        void *ts = *(void **)(self + 0x7c);
        s8 idx;
        if (ts == 0) return;
        idx = *(s8 *)(e + 1);
        if (idx < 0) return;
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            ts,
            (void*)((struct Ent **)*(void **)(self + 0x78))[idx]->out4,
            v, 0x1000, 0);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 109 -- func_ov002_020f65b8, 0x020f65b8, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f65b8Ev

void daDemo_c::anmModel_c::func_ov002_020f65b8()
{
    ObjSeq *o = (ObjSeq *)this;
    if (o->seq == 0)
        return;
    _ZN15TextureSequence6UpdateER15ModelComponents(o->seq, (char *)o + 8);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 110 -- func_ov002_020f65ec, 0x020f65ec, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f65ecEv

int daDemo_c::anmModel_c::func_ov002_020f65ec()
{
    char *c = (char *)this;
    void *p = *(void**)(c + 0x7c);
    if (p == 0)
        return (int)p;
    return _ZN15dExtFrameCtrl_c7AdvanceEv((char*)p);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 111 -- func_ov002_020f6618, 0x020f6618, size 0x160 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_c19func_ov002_020f6618EP13SharedFilePtriPS2_ihS3_i
#include "TextureSequence.h"
struct BTP_File;
extern "C" {
void* _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(SharedFilePtr& f); /* local extern: untyped file pointer. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, int fx, unsigned int e);
void* _Znwj(unsigned int sz);
void* _ZN15TextureSequenceC1Ev(void* self);
void* _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int fx, unsigned int e);
}

int daDemo_c::anmModel_c::func_ov002_020f6618(SharedFilePtr *mdl, int nAnims, SharedFilePtr **anims,
                        int arg5, unsigned char texByte, SharedFilePtr **texs, int tsData)
{
    char *self = (char *)this;
    int i;
    void* ts;
    *(SharedFilePtr**)(self + 0x70) = mdl;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self,
            _ZN5Model8LoadFileER13SharedFilePtr(**(SharedFilePtr**)(self + 0x70)), 1, tsData) == 0)
        return 0;
    self[0x80] = (char)nAnims;
    *(SharedFilePtr***)(self + 0x74) = anims;
    for (i = 0; i < *(unsigned char*)(self + 0x80); i++) {
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(*(*(SharedFilePtr***)(self + 0x74))[i]);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self,
        *(void**)((char*)(*(SharedFilePtr***)(self + 0x74))[0] + 4), 0, 0x1000, 0);
    if (texByte) {
        ts = _Znwj(0x14);
        if (ts) ts = _ZN15TextureSequenceC1Ev(ts);
        *(void**)(self + 0x7c) = ts;
        if (*(void**)(self + 0x7c) == 0)
            return 0;
        self[0x81] = texByte;
        *(SharedFilePtr***)(self + 0x78) = texs;
        for (i = 0; i < *(unsigned char*)(self + 0x81); i++) {
            void* bmd = *(void**)((char*)(*(SharedFilePtr**)(self + 0x70)) + 4);
            void* btp = _ZN15TextureSequence8LoadFileER13SharedFilePtr(*(*(SharedFilePtr***)(self + 0x78))[i]);
            TextureSequence::Prepare(*(BMD_File*)bmd, *(BTP_File*)btp);
        }
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(*(void**)(self + 0x7c),
            *(void**)((char*)(*(SharedFilePtr***)(self + 0x78))[0] + 4), 0, 0x1000, 0);
    }
    self[0x83] = (char)arg5;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 112 -- _ZN8daDemo_c10anmModel_cD0Ev, 0x020f6778, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
 * The D0 variant here emits from the ~anmModel_c definition at ordinal 113. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 113 -- _ZN8daDemo_c10anmModel_cD1Ev, 0x020f6870, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c10anmModel_cD1Ev
/* D1, the complete-object destructor.
 *
 * The shape here is the whole finding. The cartridge destroys the ModelAnim /
 * Model BASE before the Vector3 array at the class's own tail -- a normal
 * C++ destructor destroys members first and never reaches it. param_c is a
 * real base declared FIRST, so bases destroy in reverse declaration order:
 * the model base runs before the Vector3 array, and param_c lands at the
 * object tail (0x64/0x50), which is exactly what the vmi typeinfo records
 * encode.
 *
 * The base holds only the array. The remaining fields are read at raw offsets
 * rather than declared, because a declared member would sit BEFORE the tail
 * base in the layout and push the array off 0x50/0x64. */
daDemo_c::anmModel_c::~anmModel_c()
{
    char *c = (char *)this;
    void *p;
    int i;

    p = *(void **)(c + 0x70);
    if (p != 0) ((SharedFilePtr *)(p))->Release();
    for (i = 0; i < *(unsigned char *)(c + 0x80); i++) {
        p = (*(void ***)(c + 0x74))[i];
        if (p != 0) ((SharedFilePtr *)(p))->Release();
    }
    if (*(void **)(c + 0x7c) != 0) {
        for (i = 0; i < *(unsigned char *)(c + 0x81); i++) {
            p = (*(void ***)(c + 0x78))[i];
            if (p != 0) ((SharedFilePtr *)(p))->Release();
        }
        p = *(void **)(c + 0x7c);
        if (p != 0) {
            (*(VFN)((*(int **)p)[1]))(p);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 114 -- func_ov002_020f6960, 0x020f6960, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c13simpleModel_c19func_ov002_020f6960EP13SharedFilePtri

int daDemo_c::simpleModel_c::func_ov002_020f6960(SharedFilePtr *fp, int n)
{
    char *self = (char *)this;
    *(SharedFilePtr**)(self + 0x5c) = fp;
    BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(*(*(SharedFilePtr**)(self + 0x5c)));
    return _ZN9ModelBase7SetFileEP8BMD_Fileii(self, f, 1, n) != 0 ? 1 : 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 115 -- _ZN8daDemo_c13simpleModel_cD0Ev, 0x020f69a8, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c13simpleModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs.
 * The D0 variant here emits from the ~simpleModel_c definition at ordinal 116. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 116 -- _ZN8daDemo_c13simpleModel_cD1Ev, 0x020f6a00, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c13simpleModel_cD1Ev
/* D1, the complete-object destructor.
 *
 * The shape here is the whole finding. The cartridge destroys the ModelAnim /
 * Model BASE before the Vector3 array at the class's own tail -- a normal
 * C++ destructor destroys members first and never reaches it. param_c is a
 * real base declared FIRST, so bases destroy in reverse declaration order:
 * the model base runs before the Vector3 array, and param_c lands at the
 * object tail (0x64/0x50), which is exactly what the vmi typeinfo records
 * encode.
 *
 * The base holds only the array. The remaining fields are read at raw offsets
 * rather than declared, because a declared member would sit BEFORE the tail
 * base in the layout and push the array off 0x50/0x64. */
/* defer_codegen on for this one definition: it is the second MI destructor
 * defined in the TU, and under defer off the second family emits D1 before
 * D0, which is not the cartridge's order. Deferred, its D2/D0/D1 flush at
 * the next definition and land D0 below D1. */
#pragma defer_codegen on
daDemo_c::simpleModel_c::~simpleModel_c()
{
    SharedFilePtr *file = *(SharedFilePtr **)((char *)this + 0x5c);
    if (file != 0) {
        file->Release();
    }
}
#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 117 -- func_ov002_020f6a50, 0x020f6a50, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c7param_c19func_ov002_020f6a50Ev

void* daDemo_c::param_c::func_ov002_020f6a50()
{
    char *c = (char *)this;
    __cxa_vec_ctor(c, 1, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
    *(int*)c = 0;
    *(int*)(c+4) = 0;
    *(int*)(c+8) = 0;
    return c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 118 -- func_ov002_020f6a9c, 0x020f6a9c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6a9cEv

int daDemo_c::func_ov002_020f6a9c()
{
    void *t = this;
    _ZN7fBase_c18MarkForDestructionEv(t);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 119 -- func_ov002_020f6ab8, 0x020f6ab8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6ab8EPh

int daDemo_c::func_ov002_020f6ab8(unsigned char *p)
{
    char *c = (char *)this;
    unsigned int id = ReadUnalignedInt(p);
    _ZN5Sound4PlayEjjRK7Vector3(1, id, (struct Vector3*)(c + 0x74));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 120 -- func_ov002_020f6ae4, 0x020f6ae4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6ae4EPh

int daDemo_c::func_ov002_020f6ae4(unsigned char *p)
{
    unsigned int r = ReadUnalignedInt(p);
    char *thiz = (char *)this;
    *(unsigned int *)(thiz + 0xe4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(thiz + 0xe4), 3, r, thiz + 0x74, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 121 -- func_ov002_020f6b28, 0x020f6b28, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6b28EPh

int daDemo_c::func_ov002_020f6b28(unsigned char *p)
{
    unsigned char *c = (unsigned char *)this;
    unsigned int v=ReadUnalignedInt(p);
    func_02012694(v, c+0x74);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 122 -- func_ov002_020f6b4c, 0x020f6b4c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6b4cEPh
/* func_ov002_020f6b4c at 0x020f6b4c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int daDemo_c::func_ov002_020f6b4c(unsigned char *p)
{
    Obj6b *self = (Obj6b *)this;
    struct { int z, y, x; } v;
    int y, x;
    x = ReadUnalignedShort(p + 4) << 12;
    y = ReadUnalignedShort(p + 2) << 12;
    v.z = ReadUnalignedShort(p) << 12;
    v.y = y;
    v.x = x;
    self->f8e = Vec3_HorzAngle(&self->v5c, (const struct Vector3 *)&v);
    self->f94 = self->f8e;
    self->f98 = ReadUnalignedShort(p + 6);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 123 -- func_ov002_020f6bc0, 0x020f6bc0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6bc0EPh

s32 daDemo_c::func_ov002_020f6bc0(unsigned char *data)
{
    char *self = (char *)this;
    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    this->mPosX = x;
    this->mPosY = y;
    this->mPosZ = z;
    this->mAngleY = ReadUnalignedShort(data + 6);
    *(s16*)(&this->mPrevAngleY) = *(s16*)(self + 0x8e);
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
    ((anmModel_c*)this->mModelAnim)->func_ov002_020f6514((u8*)&data_ov002_0210bc88, v);
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
  char *c = (char *) this;
  u8 *flag = (u8 *) arg1;
  Vec3 v;
  if (arg2 == data_0209b274)
  {
    if ((*flag) != 0)
    {
      int r = (inline_fn(RandomIntInternal(&data_0209e650)) >> 20) & 0xfff;
      int m = (s32) (((((long long) r) * 0xa000) + 0x800) >> 12);
      this->unk_0f4 = m + 0x19000;
    }
    AddVec3((Vec3 *) (&this->mPosX), (Vec3 *) (data_0209f318 + 0x8c), (Vec3 *) (&this->mPosX));
    if (arg2 > 0x2bc)
    {
      *((s32 *) ((&this->mPosY))) -= 0x96000;
    }
    if (arg2 != data_ov002_0210b614)
    {
      func_0201267c(0x6a, c + 0x74);
      data_ov002_0210b614 = arg2;
    }
  }
  if (arg3 == data_0209b274)
  {
    this->mOpacity = 0;
    return 1;
  }
  this->mOpacity = 0x1f;
  if ((*flag) == 0)
  {
    if ((data_ov002_02110b0c & 1) == 0)
    {
      data_ov002_02110c20[0] = 0xffdd6000;
      data_ov002_02110c20[1] = 0xbe4000;
      data_ov002_02110c20[2] = 0x2ae000;
      func_020731dc(data_ov002_02110c20, (void *)(&_ZN7Vector3D1Ev), (void **) (&data_ov002_02110db8));
      data_ov002_02110b0c |= 1;
    }
    func_ov002_020f6f48((Vector3 *) data_ov002_02110c20, 0x20);
  }
  else
  {
    MulVec3Mat4x3((Vector3*)&data_ov002_0210b958, (Matrix4x3*)&data_0209b41c, (Vector3*)&v);
    Vec3_LslInPlace((int*)&v, 3);
    func_ov002_020f6f48((Vector3*)&v, 8);
  }
  this->mPrevAngleX = this->mAngleX;
  this->mPrevAngleY = this->mAngleY;
  this->mPrevAngleZ = *((s16 *) (c + 0x90));
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 127 -- func_ov002_020f6e48, 0x020f6e48, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6e48EPh

int daDemo_c::func_ov002_020f6e48(unsigned char *p)
{
    char *self = (char *)this;
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
  this->mAngleX = (short) f;
  this->mAngleY = (short) e;
  this->mAngleZ = (short) d;
  this->mPrevAngleX = this->mAngleX;
  this->mPrevAngleY = this->mAngleY;
  this->mPrevAngleZ = this->mAngleZ;
  this->unk_0f4 = (new_var = p[0xc]) << 12;
  this->mScaleX = 0xb33;
  this->mScaleY = 0xb33;
  this->mScaleZ = 0xb33;
  rv = *((int *) ((*((int *) (self - -0xe0))) + 0x54));
  new_var2 = RandomIntInternal(&data_0209e650);
  *((int *) ((*((int *) (&this->mModelAnim))) + 0x58)) = ((unsigned short) (((int) (((((long long) ((int) ((((unsigned int) new_var2) >> 20) & 0xfff))) * rv) + 0x800) >> 12)) >> 12)) << 12;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 128 -- func_ov002_020f6f48, 0x020f6f48, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f6f48EP7Vector3i
/* recovered: shared common types */

void daDemo_c::func_ov002_020f6f48(struct Vector3 *v, int amt)
{
    char *c = (char *)this;
    int* p;
    ApproachAngle(&this->mAngleX, Vec3_VertAngle(((struct Vector3*)&this->mPosX), v), amt, 0x4000, 0);
    ApproachAngle(&this->mAngleY, Vec3_HorzAngle(((struct Vector3*)&this->mPosX), v), amt, 0x4000, 0);
    p = (int*)(((char*)this->mModelAnim) + 0x1c);
    *(int*)(c+0xa4) = (int)(((s64)this->unk_0f4 * *(int*)((char*)p + 0x18) + 0x800) >> 12);
    *(int*)(&this->mVertSpeed) = (int)(((s64)*(int*)(&this->unk_0f4) * *(int*)((char*)p + 0x1c) + 0x800) >> 12);
    *(int*)(c+0xac) = (int)(((s64)*(int*)(&this->unk_0f4) * *(int*)((char*)p + 0x20) + 0x800) >> 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 129 -- func_ov002_020f7020, 0x020f7020, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7020Ev
// Clear flag bit 0x2 in the u32 at self+0xb0, return 1. u64-mask launder forces the
// base to materialize (add r2,self,#0xb0) instead of folding the offset.

int daDemo_c::func_ov002_020f7020()
{
    char *self = (char *)this;
    *(unsigned int *)(self + 0xb0) &= ~0x2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 130 -- func_ov002_020f7038, 0x020f7038, size 0x18c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7038Eii
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daDemo_c::func_ov002_020f7038(int a, int arg)
{
    char *c = (char *)this;
    if (arg == data_0209b274) {
        *(int*)(c + 0xe8) = 0;
        *(int*)(c + 0xec) = 0;
        *(int*)(c + 0xf0) = 0;
    }
    if (this->mPosY >= 0x514000) {
        if (*(int*)(c + 0xf8) < 0x3c000) {
            *(int*)(c + 0xf8) += 0x5000;
        }
    } else {
        if (*(int*)(c + 0xf8) > 0x1a000) {
            *(int*)(c + 0xf8) -= 0x2000;
        }
    }
    {
        void* obj = ((void*)this->mModelAnim);
        if (*(unsigned char*)((char*)obj + 0x82) == 0 && (this->unk_103 & 1)) {
            ((anmModel_c*)obj)->func_ov002_020f6514((u8*)&data_ov002_0210bc88, 1);
        }
    }
    this->mPosY -=
        (int)(((long long)*(int*)(c + 0xf8) * 0x199 + 0x800) >> 12);
    {
        struct Vector3 pos;
        int z = this->mPosZ;
        int x = this->mPosX;
        int y = this->mPosY + 0x50000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(int*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xe8), 0x3b, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xec), 0x3c, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xf0), 0x3d, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 131 -- func_ov002_020f71c4, 0x020f71c4, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f71c4Eii
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daDemo_c::func_ov002_020f71c4(int a, int arg)
{
    char *c = (char *)this;
    if (arg == 0) {
        if (arg == data_0209b274) {
            this->unk_0f4 = 0x10000;
        }
        _Z14ApproachLinearRiii(&this->unk_0f4, 0x1f000, 0x3e3);
        this->mOpacity = this->unk_0f4 >> 12;
    }
    {
        struct Vector3 pos;
        int z = this->mPosZ;
        int x = this->mPosX;
        int y = this->mPosY + 0x50000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(int*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xe8), 0x38, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xec), 0x39, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
        *(int*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int*)(c + 0xf0), 0x3a, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2], 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 132 -- func_ov002_020f72bc, 0x020f72bc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f72bcEv

int daDemo_c::func_ov002_020f72bc()
{
    char *c = (char *)this;
    *((u32*)&this->mScaleX) = 0x3000;
    *((u32*)&this->mScaleY) = 0x3000;
    *((u32*)&this->mScaleZ) = 0x3000;
    {
        s16* p = &this->mAngleY;
        *p = *p + 0x400;
    }
    this->mOpacity = 0x1f;

    *(u32*)(c + 0xe8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xe8), 0x31, *((Fix12i*)&this->mPosX), *((Fix12i*)&this->mPosY), *((Fix12i*)&this->mPosZ), 0, 0);
    *(u32*)(c + 0xec) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xec), 0x32, *(Fix12i*)(&this->mPosX), *(Fix12i*)(&this->mPosY), *(Fix12i*)(&this->mPosZ), 0, 0);
    *(u32*)(c + 0xf0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0xf0), 0x33, *(Fix12i*)(&this->mPosX), *(Fix12i*)(c + 0x60), *((Fix12i*)&this->mPosZ), 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 133 -- func_ov002_020f7384, 0x020f7384, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7384EPhi

int daDemo_c::func_ov002_020f7384(unsigned char *flag, int val)
{
    char *c = (char *)this;
    if (*flag != 0) {
        if (val == data_0209b274) func_02012790(0x17);
        _Z14ApproachLinearRiii((int*)(&this->mPosY), 0xfa000, 0xa000);
        c[0x102] = 0x1f;
    } else {
        if (val == data_0209b274) func_02012790(0x16);
        if (_Z14ApproachLinearRiii((int*)(c + 0x60), 0x32000, 0x5000) != 0)
            c[0x102] = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 134 -- func_ov002_020f7410, 0x020f7410, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7410EPhi

int daDemo_c::func_ov002_020f7410(unsigned char *in, int sel)
{
    unsigned char *self = (unsigned char *)this;
    int a = in[0] << 12;
    int b = in[1] << 8;
    int t;
    if (a != 0 && sel == data_0209b274) {
        register int *p = (int *)(((int)(((unsigned char *)this->mModel)) + 0x50));
        p[0] = -0x5d00;
        p[1] = 0x1800;
        p[2] = -0x16000;
        this->mAngleX = 0;
        this->mAngleY = 0xffffb3c0;
        this->mAngleZ = 0;
        func_0201f138();
    }
    MulVec3Mat4x3((Vector3*)(((unsigned char *)this->mModel) + 0x50), (Matrix4x3*)&data_0209b41c, ((Vector3*)&this->mPosX));
    Vec3_LslInPlace(&this->mPosX, 3);
    if (_Z14ApproachLinearRiii(&this->unk_0f4, a, b) != 0 && a == 0 && b != 0)
        func_0201ef38();
    *(char *)(&this->mOpacity) = *(int *)(&this->unk_0f4) >> 12;
    {
        unsigned char r = *(unsigned char *)(&this->mOpacity);
        if (r != 0 || a != 0) {
            t = (r * 0xa00) >> 12;
            if (t > 0x10) t = 0x10;
            _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4000050, 8, 1, t, 0x10 - t);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 135 -- func_ov002_020f7538, 0x020f7538, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7538EPhi

int daDemo_c::func_ov002_020f7538(unsigned char *arg1, int arg2)
{
    char *c = (char *)this;
  int a = arg1[0];
  int b = arg1[1];
  int f1 = a << 0xc;
  int f2 = b << 8;
  int* base;
  int scale;
  Vec3 tmp, diff, scaled, sum;

  if (f1 != 0 && arg2 == data_0209b274) {
    this->mAngleX = 0xd00;
    this->mAngleY = -0x7300;
    this->mAngleZ = 0x2f00;
    *(int*)(c + 0xf8) = 0;
  }

  if (!(data_ov002_02110b04 & 1)) {
    data_ov002_02110df4[0] = 0x2800;
    data_ov002_02110df4[1] = 0xfffee900;
    data_ov002_02110df4[2] = -0x4200;
    func_020731dc(data_ov002_02110df4, (void *)_ZN7Vector3D1Ev, (void**)data_ov002_02110de8);
    data_ov002_02110b04 |= 1;
  }
  if (!(data_ov002_02110afc & 1)) {
    data_ov002_02110e0c[0] = 0x3300;
    data_ov002_02110e0c[1] = 0xfffee900;
    data_ov002_02110e0c[2] = -0x5e00;
    func_020731dc(data_ov002_02110e0c, (void *)_ZN7Vector3D1Ev, (void**)data_ov002_02110e00);
    data_ov002_02110afc |= 1;
  }
  if (!(data_ov002_02110b08 & 1)) {
    data_ov002_02110e18[0] = 0x4c00;
    data_ov002_02110e18[1] = -0xf200;
    data_ov002_02110e18[2] = -0xff00;
    func_020731dc(data_ov002_02110e18, (void *)_ZN7Vector3D1Ev, (void**)data_ov002_02110dac);
    data_ov002_02110b08 |= 1;
  }

  if (f1 != 0) {
    base = data_ov002_02110df4;
    scale = 0x1000;
  } else {
    base = data_ov002_02110e18;
    scale = 0;
  }

  Math_Function_0203b0fc((int*)(c + 0xf8), scale, 0x40, 0x100);
  Vec3_Sub(&diff, (Vec3*)data_ov002_02110e0c, (Vec3*)base);
  tmp.x = diff.x;
  tmp.y = diff.y;
  tmp.z = diff.z;
  Vec3_MulScalar(&scaled, &tmp, *(int*)(c + 0xf8));
  Vec3_Add(&sum, (Vec3*)base, &scaled);

  {
    char* dp = (char*)(((char*)this->mModelAnim) + 0x64);
    *(int*)(dp) = sum.x;
    *(int*)(dp + 4) = sum.y;
    *(int*)(dp + 8) = sum.z;
  }
  MulVec3Mat4x3((Vector3*)(*(char**)(&this->mModelAnim) + 0x64), (Matrix4x3*)&data_0209b41c, (Vector3*)(&this->mPosX));
  Vec3_LslInPlace((int*)(&this->mPosX), 3);
  _Z14ApproachLinearRiii((int*)(c + 0xf4), f1, f2);
  this->mOpacity = this->unk_0f4 >> 0xc;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 136 -- func_ov002_020f7780, 0x020f7780, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7780EPvi

int daDemo_c::func_ov002_020f7780(void *unused, int mode)
{
    char *self = (char *)this;
    char *c = self;
    char *r4 = (char *)data_0209f318;

    if (mode == data_0209b274) {
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
                char *p = (char *)(data_0209f394);
                short ang = *(short *)((char*)(*(void **)(p + (idx << 2))) + 0x8e);

                Vec3_RotateYAndTranslate(&this->mPosX, (int *)(r4 + 0x8c), (short)(ang + 0x8000), v);
            }
        }

        this->mPrevAngleX = 0x1000;
        this->mPrevAngleY = -0x7000;
        this->mAngleY = this->mPrevAngleY;
        this->mOpacity = 0x1f;
    } else {
        short state = *(short *)(c + 0xfc);
        if (state <= 0x6a) {
            if (state > 0x3c) {
                Math_Function_0203b0fc(&this->unk_0f4, -0xa000, 0xcc, 0x7fffffff);
                short *p94 = &this->mPrevAngleY;
                short *p92 = &this->mPrevAngleX;
                *p94 = *p94 + 0x78;
                *p92 = *p92 + 0x40;

                short h = Vec3_HorzAngle(((const Vector3 *)&this->mPosX), (const Vector3 *)(r4 + 0x8c));
                _Z15ApproachLinear2Rsss(&this->mAngleY, h, 0x200);
            }

            state = *(short *)(c + 0xfc);
            if (state > 0x69) {
                this->mPrevAngleX = 0xe00;
            }
        } else {
            Math_Function_0203b0fc(&this->unk_0f4, 0x3c000, 0xcc, 0x7fffffff);

            short h = Vec3_HorzAngle(((const Vector3 *)&this->mPosX), (const Vector3 *)(r4 + 0x8c));
            _Z15ApproachLinear2Rsss(&this->mAngleY, h, 0x200);

            state = *(short *)(c + 0xfc);
            if (state < 0xa9) {
                _Z11UpdateAngleRssis(((void *)&this->mPrevAngleY), 0x1800, 0x1e, 0x4000);
            }
            _Z14ApproachLinearRsss(&this->mPrevAngleX, -0x3000, 0x70);
        }
    }

    {
        unsigned short a = *((unsigned short *)&this->mPrevAngleX);
        int t = this->unk_0f4;
        short s0 = data_02082214[((a >> 4) << 1) + 1];
        *(int *)(c + 0x98) = (int)(((long long)t * s0 + 0x800) >> 12);
    }
    {
        unsigned short a = *(unsigned short *)(&this->mPrevAngleX);
        int t = this->unk_0f4;
        int s1 = -data_02082214[(a >> 4) << 1];
        this->mVertSpeed = (int)(((long long)t * s1 + 0x800) >> 12);
    }

    {
        short *pfc = (short *)(c + 0xfc);
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
    char *self = (char *)this;
    char *c = self;

    if (mode == data_0209b274) {
        this->unk_0f4 = 0x578000;
        *(short *)(c + 0xfc) = -0x1000;
        *(short *)(c + 0xfe) = 0x800;
        *(int *)(c + 0xf8) = -0xc8000;
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
        Math_Function_0203b0fc(&this->unk_0f4, 0x64000, 0x7a, 0x7fffffff);

        this->mAngleX = _ZN4cstd5atan2E5Fix12IiES1_(this->mPosY - 0x190000, 0xc8000);

        ApproachAngle(&this->mAngleY, (short)(this->mPrevAngleY + 0x8000), 4, 0x4000, 0);

        data_ov002_02110ddc[2] = this->unk_0f4;
        v[0] = data_ov002_0210b964[0];
        v[1] = data_ov002_0210b964[1];
        v[2] = data_ov002_0210b964[2];
        w[0] = data_ov002_02110ddc[0];
        w[1] = data_ov002_02110ddc[1];
        w[2] = data_ov002_02110ddc[2];
        Vec3_RotateYAndTranslate(&this->mPosX, v, this->mPrevAngleY, w);

        h = *(unsigned short *)(c + 0xfc);
        t = data_02082214[(h >> 4) * 2 + 1];
        {
            int *p60 = (int *)(&this->mPosY);
            *p60 = *p60 + (int)((((long long)t * 0x96000LL) + 0x800) >> 12);
        }

        {
            short *pfc = (short *)(c + 0xfc);
            *pfc = *pfc + *(short *)(c + 0xfe);
        }
        ApproachAngle((short *)(c + 0xfe), 0x200, 0x14, 0x4000, 0);

        {
            int *p5c = &this->mPosX;
            *p5c = *p5c + *(int *)(c + 0xf8);
        }
        Math_Function_0203b0fc((int *)(c + 0xf8), 0, 0xcc, 0x7fffffff);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 138 -- func_ov002_020f7bb8, 0x020f7bb8, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c19func_ov002_020f7bb8EPht

int daDemo_c::func_ov002_020f7bb8(unsigned char *p, u16 id)
{
    char *c = (char *)this;
    Vector3 v0, v1;
    int src[3];
    Fix12i dist;
    s16 vertAngle, heading;
    char *obj;
    unsigned int tbl1, tbl2;
    int i;

    this->mOpacity = 0;
    if (id == data_0209b274) {
        this->unk_0f4 = 0;
        *(u16 *)(c + 0xfc) = 0;
    }

    tbl1 = ReadUnalignedInt(p);
    tbl2 = ReadUnalignedInt(p + 4);

    if (func_02008b4c(&v1, (short *)(c + 0xfc), &this->unk_0f4, (char *)tbl1) != 0) {
        return 0;
    }
    if (func_02008b4c(&v0, (short *)(c + 0xfc), &this->unk_0f4, (char *)tbl2) != 0) {
        return 0;
    }

    this->mOpacity = 0x1f;
    obj = data_0209f318;

    AddVec3((Vec3*)&v0, (Vec3 *)(obj + 0x8c), (Vec3*)&v0);
    dist = Vec3_Dist(&v0, (Vector3 *)(obj + 0x8c));

    {
        s16 t1 = *(s16 *)(obj + 0x17e);
        vertAngle = Vec3_VertAngle(&v0, (Vector3 *)(obj + 0x8c));
        vertAngle += t1;
    }
    {
        s16 t2 = *(s16 *)(obj + 0x17c);
        heading = Vec3_HorzAngle(&v0, (Vector3 *)(obj + 0x8c));
        heading += t2;
    }

    src[0] = 0;
    src[1] = (int)(((s64)dist * data_02082214[(((u16)vertAngle) >> 4) * 2] + 0x800) >> 12);
    src[2] = (int)(((s64)dist * data_02082214[(((u16)vertAngle) >> 4) * 2 + 1] + 0x800) >> 12);

    Vec3_RotateYAndTranslate((int *)(&this->mPosX), (int *)(obj + 0x8c), heading, src);

    *(s16 *)(&this->mAngleX) = _ZN4cstd5atan2E5Fix12IiES1_(v1.y, Vec3_HorzLen(&v1));
    *(s16 *)(c + 0x8e) = _ZN4cstd5atan2E5Fix12IiES1_(v1.x, v1.z);

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 140 -- _ZN8daDemo_c16CleanupResourcesEv, 0x020f8028, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daDemo_c::CleanupResources()
{
  int r1 = param1;
  if (r1 == 0x2e) return func_ov002_020f63a0();
  if (r1 == 0x2f) return func_ov002_020f23d0();
  ObjV* a = *(ObjV**)((char*)&mModel);
  if (a) if (a) a->m04();
  ObjV* b = *(ObjV**)((char*)&mModelAnim);
  if (b) if (b) b->m04();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 141 -- _ZN8daDemo_c16OnPendingDestroyEv, 0x020f80a8, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c16OnPendingDestroyEv
/* daDemo_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daDemo_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 142 -- _ZN8daDemo_c6RenderEv, 0x020f80ac, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daDemo_c::Render()
{
  if (param1 == 0x19){
    struct { char* p; char* cur; M48* src; } s;
    s.src = (M48*)&data_0209b41c;
    s.p = *(char**)((char*)&mModel) + 0x1c;
    s.cur = s.p;
    *(M48*)s.cur = *s.src;
    int* tbl = data_ov002_0210bb7c;
    int i = 0;
    int zero = 0;
    do {
      *(int*)(s.p + 0x24) = tbl[0];
      *(int*)(s.p + 0x28) = tbl[1];
      *(int*)(s.p + 0x2c) = tbl[2];
      ((ModelBaseSh*)*(void**)((char*)&mModel))->m(zero);
      tbl += 3;
      i++;
    } while ((unsigned)i < 3u);
    return 1;
  }
  unsigned char op = mOpacity;
  if (op == 0) return 1;
  {
    void* a = *(void**)((char*)&mModel);
    if (a != 0){
      a = (void*)((int)a);
      _ZN9ModelBase12ApplyOpacityEjj(a, op, 0);
      ((ModelBaseSh*)*(void**)((char*)&mModel))->m((int)((char*)&mScaleX));
    } else {
      void* b = *(void**)((char*)&mModelAnim);
      if (b != 0){
        b = (void*)((int)b);
        ((anmModel_c*)b)->func_ov002_020f65b8();
        _ZN9ModelBase12ApplyOpacityEjj(*(void**)((char*)&mModelAnim), mOpacity, 0);
        ((ModelBaseSh*)*(void**)((char*)&mModelAnim))->m((int)((char*)&mScaleX));
      }
    }
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 143 -- _ZN8daDemo_c8BehaviorEv, 0x020f81c4, size 0x1e0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daDemo_c::Behavior()
{
  char *c = (char *) ((void *)this);
  Vector3 v;
  Vec3 asr;
  int new_var2;
  s32 t;
  t = *((s32 *) (c + 8));
  if (t == 0x2e)
  {
    return func_ov002_020f63d4();
  }
  if (t == 0x2f)
  {
    return func_ov002_020f23f0();
  }
  if ((((u32) t) >= 0x1a) && (((u32) t) <= 0x2d))
  {
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, 0);
  }
  else
  {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
  }
  {
    char *g = *((char **) (&this->mModelAnim));
    if ((g != 0) && ((*((u8 *) (g + 0x83))) != 0))
    {
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
      if (ground.DetectClsn() != 0)
      {
        s32 h = ground.clsnY;
        if ((this->mPosY) < h)
        {
          this->mPosY = h;
          *((u8 *) ((&this->unk_103))) |= 1;
        }
      }
    }
  }
  Vec3_Asr(&asr, (Vec3 *) (&this->mPosX), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
  t = *((s32 *) (c + 8));
  if ((((u32) t) >= 0x1a) && (((u32) t) <= 0x2d))
  {
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, this->mAngleX, this->mAngleY, this->mAngleZ);
  }
  else
  {
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, this->mAngleX, this->mAngleY, this->mAngleZ);
  }
  if ((*((char **) (&this->mModel))) != 0)
  {
    *((Matrix4x3 *) ((*((char **) (&this->mModel))) + 0x1c)) = data_020a0e68;
  }
  if ((*((char **) (&this->mModelAnim))) != 0)
  {
    *((Matrix4x3 *) ((*((char **) (&this->mModelAnim))) + 0x1c)) = data_020a0e68;
    (((anmModel_c*)this->mModelAnim))->func_ov002_020f64ac((char*)&data_ov002_0210bc88);
    (((anmModel_c*)this->mModelAnim))->func_ov002_020f65ec();
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 144 -- _ZN8daDemo_c13InitResourcesEv, 0x020f83a4, size 0x464 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDemo_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daDemo_c::InitResources()
{
    void *p;
    int t;

    if (param1 == 0x12) {
        p = _Znwj(0x84);
        if (p) {
            ((param_c*)((char *)p + 0x64))->func_ov002_020f6a50();
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        this->mModelAnim = (ModelAnim *)p;
        if (this->mModelAnim == 0) return 0;
        if (((anmModel_c*)this->mModelAnim)->func_ov002_020f6618((SharedFilePtr*)&data_ov085_0213074c, 1, (SharedFilePtr**)&data_ov002_0210b60c, 1, 1, (SharedFilePtr**)&data_ov002_0210b608, -1) == 0) return 0;
    } else if (param1 == 0x13) {
        p = _Znwj(0x84);
        if (p) {
            ((param_c*)((char *)p + 0x64))->func_ov002_020f6a50();
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        this->mModelAnim = (ModelAnim *)p;
        if (this->mModelAnim == 0) return 0;
        t = (*(volatile unsigned char *)&data_0209f2d8 == 2);
        if (t == 0) {
            if (((anmModel_c*)this->mModelAnim)->func_ov002_020f6618((SharedFilePtr*)&data_ov002_02110b98, 1, (SharedFilePtr**)&data_ov002_0210b610, 1, 1, (SharedFilePtr**)&data_ov002_0210b600, 0x16) == 0) return 0;
        } else {
            if (((anmModel_c*)this->mModelAnim)->func_ov002_020f6618((SharedFilePtr*)&data_ov002_02110c18, 0xD, (SharedFilePtr**)&data_ov002_0210bcf0, 1, 0xD, (SharedFilePtr**)&data_ov002_0210bd24, 0x16) == 0) return 0;
        }
    } else if (param1 >= 0x14 && param1 <= 0x16) {
        p = _Znwj(0x60);
        if (p) {
            ((param_c*)((char *)p + 0x50))->func_ov002_020f6a50();
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        this->mModel = (Model *)p;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b70, -1) == 0) return 0;
    } else if (param1 == 0x17) {
        p = _Znwj(0x60);
        if (p) {
            ((param_c*)((char *)p + 0x50))->func_ov002_020f6a50();
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        this->mModel = (Model *)p;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b50, 0x19) == 0) return 0;
    } else if (param1 == 0x18) {
        p = _Znwj(0x60);
        if (p) {
            ((param_c*)((char *)p + 0x50))->func_ov002_020f6a50();
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        this->mModel = (Model *)p;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_0211094c, -1) == 0) return 0;
    } else if (param1 == 0x19) {
        p = _Znwj(0x60);
        if (p) {
            ((param_c*)((char *)p + 0x50))->func_ov002_020f6a50();
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        this->mModel = (Model *)p;
        if (this->mModel == 0) return 0;
        if (((simpleModel_c*)this->mModel)->func_ov002_020f6960((SharedFilePtr*)&data_ov002_02110b78, 0x13) == 0) return 0;
    } else if (param1 >= 0x1A && param1 <= 0x2D) {
        p = _Znwj(0x84);
        if (p) {
            ((param_c*)((char *)p + 0x64))->func_ov002_020f6a50();
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        this->mModelAnim = (ModelAnim *)p;
        if (this->mModelAnim == 0) return 0;
        if (((anmModel_c*)this->mModelAnim)->func_ov002_020f6618((SharedFilePtr*)data_ov009_02113c20, 1, (SharedFilePtr**)&data_ov002_0210b604, 0, 0, 0, -1) == 0) return 0;
    }
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}
