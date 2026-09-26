//cpp
/* dScMgTrampoline_c -- Trampoline Time. Draw a line on the touch screen
 * (up to three at once) and bounce Mario to the lit door. Three misses
 * end the round.
 *
 * Functions are written in reverse of ROM order; do not reorder.
 * `opt_loop_invariants off` above Virtual88 has no matching `on`, so it
 * applies to the whole file. TUBUILD CONFLICT comments are paired with
 * the manifest; leave them.
 *
 * The factory still builds the object by hand, so its offset stores stay
 * raw. Score-pop reads in func_ov006_02120a64, func_ov006_02120ab8 and
 * func_ov006_02120d0c stay offsets: the Desc member form DIFFed there.
 * ov004 helpers and most data symbols are unnamed.
 */

#include "dScMgTrampoline_c.h"
#include "types.h"
#include "G2x.h"
#include "Sound.h"

namespace cstd { int fdiv(int numerator, int denominator); }
namespace G2   { void *GetBG0ScrPtr(); void *GetBG1ScrPtr(); void *GetBG2ScrPtr(); void *GetBG3ScrPtr(); void *GetBG2CharPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace G3X  { void SetFog(bool enable, int blend, int slope, int offset); }

/* Local stand-ins for types no header declares yet. */
typedef short s16;

struct Node { struct Node* next; };

/* One floating score. Five of these are mArray3. Spawned when a Mario
   lands: x starts at the right edge and drifts left, y eases toward
   targetY, and number is the score drawn by func_ov004_020b2444.
   Same 0x24 shape as dScMgJump2_c's Elem. targetX is written and not
   read by this TU's step. */
struct Desc {
    Node *next;     /* +0x00 -- the live list */
    int x;          /* +0x04 -- 20.12 */
    int y;          /* +0x08 */
    int targetX;    /* +0x0c */
    int targetY;    /* +0x10 */
    int speedX;     /* +0x14 */
    int speedY;     /* +0x18 -- func_0203d630 damps it with speedX (it
                       scales both words); not applied here */
    u16 life;       /* +0x1c */
    u16 number;     /* +0x1e */
    u16 active;     /* +0x20 */
};

/* Shadow of the scene's +0x68/+0x6c, which sit in dScMgBase_c padding.
   Virtual88 reads the layer; func_ov006_02120f18 writes both. */
struct Obj {
    unsigned char pad0[0x68];
    unsigned char brushOn;
    unsigned char pad1[3];
    int bgLayer;
};

typedef struct UnkObj UnkObj;

struct UnkObj {
    int (**vt)(UnkObj*);
};

typedef struct Vec2s { s16 x, y; } Vec2s;

struct B4 {
    unsigned char v;
    unsigned char pad[3];
};

/* The door mark's layout lives in the header. */
typedef dScMgTrampoline_DoorMark DoorMark;

/* Raw storage form of mwccarm's eight-byte single-inheritance PMF. Behavior
   gives the live scene storage its dScMgTrampoline_c::State meaning at the
   dispatch site; these descriptors retain the raw form because direct
   namespace-scope PMF initialization makes
   mwccarm 2004/b56 emit a __sinit, .ctor entry, and five anonymous data
   temporaries that the cartridge does not contain. */
struct P2 { int a, b; };

// Preserve the original front end's blind two-word aggregate copy in C++ mode.
struct P2Words { int words[2]; };

struct TrampolineTimeProfile {
    void *(*spawn)(void);
    s16 behaviorPriority;
    s16 renderPriority;
};

typedef char TrampolineTimeProfile_size_must_be_8[
    sizeof(TrampolineTimeProfile) == 8 ? 1 : -1];

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_021218c4, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_02121cf4, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'P2', from the legacy file for func_ov006_02121f04, NOT applied:
struct P2 { int a, b; };
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied:
struct Obj {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int a);
};
*/

extern "C" {
extern void func_ov004_020afdd0(void *a, int b, int c, int d, int e);
extern int data_ov006_0212f0c8[];
extern int data_ov006_02134f24;
extern int _Z15ApproachLinear2Rsss(s16 *dst, s16 a, s16 b);
extern void func_ov004_020b2444(int,int,int,int,int,int,int);
extern void func_ov006_02120bc8(int *self);
extern struct Node* data_ov006_02142f64;
extern void func_ov006_02120a64(char *p);
extern void func_ov006_02120ab8(char *);
extern int data_ov006_02142f70;
extern int data_ov006_02142f68;
extern void func_ov006_02120b30(struct Desc *, int, int);

extern int data_ov006_02142f6c[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern const unsigned char data_ov006_0212f0d0[];
extern UnkObj* data_0209f5bc;
extern int data_ov006_0213fa9c;
unsigned int func_02012790(unsigned int a);
int func_ov006_020d0c38(Vec2s* a, Vec2s* b);

void SetBg2Offset(int a, int b);
int func_ov004_020b04c0(void);
extern "C" unsigned char data_0209d45c;
extern "C" void func_ov004_020ad90c(void *scene);
int  GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern volatile unsigned char data_020a0e40[];
extern struct B4 data_020a0de8[];
extern struct B4 data_020a0de9[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];
extern void func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern short data_ov006_02140538;

extern void func_ov006_020cd39c(...);
extern int _Z14ApproachLinearRiii(int*,int,int);
extern int func_ov006_020cd158(void);
extern void func_ov004_020b0a54(int);
extern void func_ov006_020cd1e0(void *c);
extern int data_ov006_02140588;
extern int data_ov006_0213b0ec;
extern int data_ov006_0214058c;
extern int data_ov006_02142f60;
extern int data_0209e650;
extern void func_ov006_020d0ac0(void);
extern int func_ov006_020ccd04(int *r0);
extern void func_ov006_020cc9fc(char *c);
extern short func_ov006_02121768(char *c);
extern void func_ov006_02121750(char *c, short v);
extern void func_ov006_02120a18(u16 *a, int b);
extern int RandomIntInternal(int *seed);
extern "C" void func_ov006_020cd62c(int n);
extern "C" void func_ov006_020cd510(int a);
void func_ov006_020d0bd8(void);
extern s16 data_ov006_0212e04c;
extern s16 data_ov006_0212e044;
extern s16 data_ov006_0212e048;
extern void Camera_UpdateMatrices(void *cam);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *ptr);
extern void func_ov006_02120f18(struct Obj *self, int a);
extern int data_ov006_0213fadc[];
extern void InitialiseVramGlobals(void);
extern s16 data_02082414;
extern int func_ov006_020cd658(unsigned char *, int);
extern void func_ov006_02120d8c(void *, int);
extern void func_ov006_020d0b2c(void);
extern void func_ov004_020b04d0(int);
extern void func_ov006_0212231c(void *);
extern void func_ov006_020cd424(unsigned int, int);
extern void func_ov006_02120ca0(void);
extern int data_ov006_0213fb18[];
extern void func_ov006_02120a44(char *);
extern void func_ov006_020c8a9c(int, int);
extern void func_02012718(int, int);
extern void func_ov006_02120c40(void);
extern void func_ov006_021209ac(short *);
extern void func_ov004_020adb1c(int);
extern void func_ov006_0212093c(short *, int);
extern void func_ov006_02120c08(void);
extern int *data_ov006_0213fb04[];
extern int data_ov006_02134ecc;
extern void func_ov004_020b1a5c(int, int);
extern void *data_ov006_02134f00[];
extern void *data_ov006_02134f08;
extern void func_ov006_020cd270(void);
extern void func_ov006_020d09e0(void);
extern void *_ZN11dScMgBase_cC2Ev(void *self);
extern void _ZN8Particle10SysTrackerC1Ev(void *self);
extern void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
extern int _ZTV14dScMgD3DBase_c[];
extern int _ZTV17dScMgTrampoline_c[];
extern void func_ov006_020cd12c(void);
extern void func_ov006_020d100c(void);
extern void func_ov006_021225a8(void);
extern void func_ov006_02120a54(char *self);

void *dScMgTrampoline_c_classInit(void);
/* Literal aliases used only to make the five ROM PMF relocations static data.
   Their definitions below are real compiler-spelled C++ members. Each is a
   local extern: a pointer to member cannot convert to the int word a P2
   entry holds, so dScMgTrampoline_c.h's declarations cannot fill the table. */
extern "C" void _ZN17dScMgTrampoline_c9StateDoneEv(void);       /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c13StateWaitExitEv(void);  /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c12StateResultsEv(void);   /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c9StatePlayEv(void);       /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c10StateIntroEv(void);     /* local extern: PMF -> int */
extern int data_ov006_02134d40[];
extern int data_ov006_02134d4c[];
extern int data_ov006_02134d58[];
extern int data_ov006_02134d34[];
extern int data_ov006_02134d28[];
extern int data_ov006_021373a4[];
extern int data_ov006_021373fc[];
extern int data_ov006_0213734c[];
extern int data_ov006_021372f4[];
extern int data_ov006_02137454[];

/* Data owned by the same original TU. */
P2 data_ov006_0213faa0 = { (int)_ZN17dScMgTrampoline_c13StateWaitExitEv, 0 };
P2 data_ov006_0213faa8 = { (int)_ZN17dScMgTrampoline_c9StateDoneEv, 0 };
P2 data_ov006_0213fab0 = { (int)_ZN17dScMgTrampoline_c10StateIntroEv, 0 };
TrampolineTimeProfile g_profile_MG_TRAMPOLINE = {
    dScMgTrampoline_c_classInit, 0x180, 0x180
};
P2 data_ov006_0213fac0 = { (int)_ZN17dScMgTrampoline_c9StatePlayEv, 0 };
P2 data_ov006_0213fac8 = { (int)_ZN17dScMgTrampoline_c12StateResultsEv, 0 };
int data_ov006_0213fadc[] = { 0x107, 0x109, 0x104, 0x105, 0x106 };
int *data_ov006_0213fb04[] = {
    data_ov006_021373a4,
    data_ov006_021373fc,
    data_ov006_0213734c,
    data_ov006_021372f4,
    data_ov006_02137454
};
int data_ov006_0213fb18[] = {
    (int)data_ov006_02134d40,
    (int)data_ov006_02134d4c,
    (int)data_ov006_02134d58,
    (int)data_ov006_02134d34,
    (int)data_ov006_02134d28
};
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f64, from the legacy file for func_ov006_02120bc8, NOT applied: extern void *data_ov006_02142f64; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f64, from the legacy file for func_ov006_02120c08, NOT applied: extern void *data_ov006_02142f64; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f68, from the legacy file for func_ov006_02120ca0, NOT applied: extern char* data_ov006_02142f68; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f64, from the legacy file for func_ov006_02120ca0, NOT applied: extern int data_ov006_02142f64; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_02120f18, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054efc, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054efc(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054ea8, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054ea8(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for _ZN17dScMgTrampoline_c11OnAttacked2Ev, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN17dScMgTrampoline_c11OnAttacked2Ev, NOT applied: void MultiStore16(u16 val, void* dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov004_020afdd0, from the legacy file for _ZN17dScMgTrampoline_c6RenderEv, NOT applied: void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121778, NOT applied: extern void MultiStore16(unsigned short a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020cd39c, from the legacy file for func_ov006_021218fc, NOT applied: extern void func_ov006_020cd39c(...); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of _Z15ApproachLinear2Rsss, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z15ApproachLinear2Rsss(short *v, short step, short rate); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_021218fc, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int bank); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02140588, from the legacy file for func_ov006_02121bc8, NOT applied: extern "C" int data_ov006_02140588; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_02120a18, from the legacy file for func_ov006_02121cf4, NOT applied: extern void func_ov006_02120a18(char* p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_02121cf4, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020d0ac0, from the legacy file for func_ov006_02121d64, NOT applied: void func_ov006_020d0ac0(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02121d64, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121d64, NOT applied: void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02121d64, NOT applied: void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov006_02121d64, NOT applied: int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov004_020ae5c4, from the legacy file for func_ov006_02121d64, NOT applied: void func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g); */
/* TUBUILD CONFLICT -- alternate declaration of func_020126e8, from the legacy file for func_ov006_02121d64, NOT applied: int func_020126e8(int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02121d64, NOT applied: int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_0212e044, from the legacy file for func_ov006_02121f04, NOT applied: extern short data_ov006_0212e044; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_0212e048, from the legacy file for func_ov006_02121f04, NOT applied: extern short data_ov006_0212e048; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN17dScMgTrampoline_c13OnTurnIntoEggEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd4fdivEii, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern s32 _ZN4cstd4fdivEii(s32 a, s32 b); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209d45c, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern u8 data_0209d45c; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_0212231c, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_0212231c, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_0212231c, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_0212231c, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */
}

// @symbol func_ov006_021225a8
extern "C" void func_ov006_021225a8(void)
{
}

// @symbol dScMgTrampoline_c_classInit
extern "C" void *dScMgTrampoline_c_classInit(void)
{
    char *scene = (char *)_ZN7fBase_cnwEj(0x5dc8);
    if (scene) {
        char *it;
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)_ZTV14dScMgD3DBase_c;
        *(short *)(scene + 0x4664) = 0;
        it = scene + 0x466c;
        {
            char *end = scene;
            end += 0x47e4;
            do {
                it += 0xbc;
            } while (it != end);
        }
        _ZN8Particle10SysTrackerC1Ev(scene + 0x47e4);
        // This TU owns the vtable object; +2 selects its ABI address point.
        *(int *)scene = (int)(_ZTV17dScMgTrampoline_c + 2);
        __cxa_vec_ctor(scene + 0x500c, 4, 0xd0,
                      (void *)func_ov006_020cd12c,
                      (void *)func_ov006_020ccfc8);
        __cxa_vec_ctor(scene + 0x534c, 3, 0x32c,
                      (void *)func_ov006_020d100c,
                      (void *)func_ov006_020d1008);
        __cxa_vec_ctor(scene + 0x5cd0, 5, 0x24,
                      (void *)func_ov006_021225a8,
                      (void *)func_ov006_02120938);
        func_ov006_02120a54(scene + 0x5d84);
    }
    return scene;
}

// @symbol func_ov006_0212231c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0212231c(void *arg0)
{
    void *sb = arg0;
    int r8, r7, r6, r5, r4;
    volatile u16 sp0;

    r8 = LoadFile(0xa5);
    r7 = LoadFile(0xa7);
    r6 = LoadFile(0xa6);
    r5 = LoadFile(data_ov006_0213fadc[GetGameLanguage()]);
    r4 = LoadFile(0x108);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9110;
    *(volatile u16 *)0x400000c &= ~0x40;

    {
        char *dst = (char *)G2::GetBG2CharPtr();
        sp0 = 0;
        MultiStore16(sp0, dst, 0x4000);
    }
    DecompressLZ16(r8, (void *)((char *)G2::GetBG2CharPtr() + 0x4000));
    DecompressLZ16(r7, (void *)G2::GetBG2ScrPtr());
    GX::LoadBGPltt((const void *)r6, 0x1a0, 0x60);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1318;
    *(volatile u16 *)0x400000e &= ~0x40;

    DecompressLZ16(r5, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)r4, 0, 0x60);

    Deallocate((void *)r8);
    Deallocate((void *)r7);
    Deallocate((void *)r6);
    Deallocate((void *)r5);
    Deallocate((void *)r4);

    func_ov006_02120f18((Obj *)sb, 3);
}
}

// @symbol _ZN17dScMgTrampoline_c13InitResourcesEv
/* Slot 0. Sets up the two cameras and the fog, then ends with a virtual
 * call to OnYoshiTryEat(-1), the scene's reset hook. */
s32 dScMgTrampoline_c::InitResources()
{
    char *base = (char *)this;
    s32 fov;

    this->mScrollY = 0x20;
    this->mScrollTargetY = this->mScrollY;
    func_ov006_0212231c(base);
    data_0209d45c = 0x1d;
    G3X::SetFog(false, 0, 2, 0x1000);
    *(u16 *)0x4000060 = (*(u16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = cstd::fdiv(0xc0000, (s32)data_02082414);
    this->mCamera0EyeX = 0;
    this->mCamera0EyeY = -0x64000;
    this->mCamera0EyeZ = 0;
    this->mCamera0TargetX = 0;
    this->mCamera0TargetY = 0;
    this->mCamera0TargetZ = fov;
    this->mCamera0Angle = 0x400;
    Camera_UpdateMatrices(base + 0x466c);
    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x400;
    Camera_UpdateMatrices(base + 0x4728);
    if (func_ov006_020cd658((unsigned char *)(base + 0x500c), 4) == 0)
        return 0;
    func_ov006_02120d8c(base + 0x5cd0, 5);
    func_ov006_020d0b2c();
    func_ov004_020b04d0(0x10);
    OnYoshiTryEat(-1);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c13OnYoshiTryEatEi
extern "C" void func_ov006_020d0b04(int);
extern volatile s16 data_020a0dbc[];

void dScMgTrampoline_c::OnYoshiTryEat(int /* arg */)
{
    char *o = (char *)this;

    struct dScMgTrampoline_c *self = (struct dScMgTrampoline_c *)(void *)o;
    volatile unsigned short fill;
    int q;

    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 1, 0x3e, 0x10, 0x10);

    self->mTouchStartX = data_020a0dbc[0];
    self->mTouchStartY = data_020a0dbc[1];
    self->mTouchX = data_020a0dbc[0];
    self->mTouchY = data_020a0dbc[1];

    data_ov006_02142f60 = 0;
    self->unk_0bc = 0;
    if ((unsigned int)self->unk_0bc > 0x270e)
        self->unk_0bc = 0x270e;
    /* unk_0bc is u32 on dScMgBase_c (dScMg3DEsp_c's evidence). This function
       divides and takes a remainder of it, where signedness IS codegen -- it
       reads the field SIGNED, which is what this header used to declare
       locally. Cast at the use site rather than retype the shared base. */
    q = ((((s32)self->unk_0bc % 5) << 12)) / 4;
    self->mScrollY =
        (int)(((long long)(0x1000 - q) * 0x20 + 0x800) >> 12) +
        (int)(((long long)q * 0x50 + 0x800) >> 12);
    self->mScrollTargetY = self->mScrollY;
    self->mScrollHoldTimer = 0;
    func_ov006_020cd424(self->unk_0bc, self->mScrollY);

    func_ov006_020d0b04(self->unk_0bc);
    func_ov006_02120ca0();
    func_ov006_020c8a9c(0, data_ov006_0213fb18[GetGameLanguage()]);

    func_ov006_02120a44((char *)&mDoorMark);

    self->mInputEnabled = 0;
    self->mRoundOver = 0;
    self->mDoorSwitchTimer =
        (((int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13)
            * 0x2d0) >> 12) + 0x2d0;
    self->mDoorBlinkTimer = 0;
    self->mDoorBlinkPhase = 1;
    self->mArrow1X = 0;
    self->mArrow2X = 0;

    func_ov006_02121750(o, 0);

    {
        char *dst = (char *)func_02054d88();
        fill = 0;
        MultiStore16(fill, dst, 0x6000);
    }

    BeginIntro();
}

// @symbol _ZN17dScMgTrampoline_c13OnTurnIntoEggEi
/* dScMgTrampoline_c::OnTurnIntoEgg, from its vtable slot. */
int dScMgTrampoline_c::OnTurnIntoEgg(int /* mode */)
{
    func_ov006_020c8a9c(0, data_ov006_0213fb18[GetGameLanguage()]);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c10BeginIntroEv
void dScMgTrampoline_c::BeginIntro()
{
    short a;
    short b;

    mTimer = 0x5a;
    a = data_ov006_0212e044;
    mTouchStartX = a;
    b = data_ov006_0212e048;
    mTouchStartY = b;
    mTouchX = a;
    mTouchY = b;
    mDragSoundHandle = 0;
    *(P2Words *)mState = *(P2Words *)&data_ov006_0213fab0;
}

// @symbol _ZN17dScMgTrampoline_c10StateIntroEv
void dScMgTrampoline_c::StateIntro()
{
    char *c = (char *)this;
    int counter;

    func_ov006_020d0ac0();
    mTimer -= 1;
    counter = mTimer;

    if (counter == 0) {
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }

        {
            int dst = func_02054d88();
            volatile u16 tmp = 0;
            MultiStore16(tmp, (char *)dst, 0x6000);
        }

        func_ov006_020d0bd8();
        func_02012718(0x1af, 0x8000);
        BeginPlay();
        return;
    }

    {
        int t = cstd::fdiv(counter << 12, 0x5a000);
        volatile s16 oldY = mTouchX;
        volatile s16 oldZ = mTouchY;
        int mixRaw = data_ov006_0212e044 * t +
                     data_ov006_0212e04c * (0x1000 - t);

        mTouchX = (s16)(mixRaw >> 12);
        mTouchY = data_ov006_0212e048;

        mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) &
                              ~0x80000000) >> 19) -
               0x800) << 2) >> 12);

        func_ov004_020ae5c4(c, oldY, oldZ,
                            mTouchX,
                            mTouchY, 2, 0xc);

        mDragSoundHandle =
            func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0,
                           func_020126e8(mixRaw), 0, 0);
    }
}

// @symbol _ZN17dScMgTrampoline_c9BeginPlayEv
void dScMgTrampoline_c::BeginPlay()
{
    mTimer = 0x1e;
    mInputEnabled = 1;
    func_ov006_02120a18((u16 *)&mDoorMark, mDoorSide);
    mDragSoundHandle = 0;
    Sound::PlayBank2_2D(0x1b6);
    *(P2Words *)mState = *(P2Words *)&data_ov006_0213fac0;
}

// @symbol _ZN17dScMgTrampoline_c12UpdateScrollEv
int ApproachLinear(int&, int, int);

void dScMgTrampoline_c::UpdateScroll()
{
    int q = data_ov006_02140588 / 10;
    int old = unk_0bc;
    func_ov006_020cd62c(mScrollY);
    func_ov006_020cd510((q << 8) + 0x1000);
    if (old != q) {
        unk_0bc = q;
        if (unk_0bc > 0x270e)
            unk_0bc = 0x270e;
        if (mScrollTargetY < 0x50) {
            mScrollHoldTimer = 0x78;
            Sound::PlayBank2_2D(0x1b2);
        }
    }
    if (ApproachLinear(mScrollHoldTimer, 0, 1) != 0) {
        if (mScrollY == mScrollTargetY) {
            int t;
            mScrollTargetY = (q << 3) + 0x20;
            t = mScrollTargetY;
            if (t > 0x50)
                t = 0x50;
            mScrollTargetY = t;
        }
        ApproachLinear(mScrollY, mScrollTargetY, 2);
        mScrollOffsetY = 0;
    } else {
        mScrollOffsetY = (mScrollHoldTimer & 2) >> 1;
    }
}

// @symbol _ZN17dScMgTrampoline_c9StatePlayEv
void dScMgTrampoline_c::StatePlay()
{
    char *c = (char *)this;
    int old = data_ov006_02140588;
    func_ov006_020d0ac0();
    func_ov006_020cd39c();
    if (data_ov006_02140588 > old) {
        if (data_ov006_02140588 == 3)
            mPromptEnabled = 0;
    }
    UpdateScroll();

    if (data_ov006_0213b0ec == 0) {
        mInputEnabled = 0;
        BeginResults();
    } else {
        if (_Z14ApproachLinearRiii(&mTimer, 0, 1)) {
            if (data_ov006_02140588 >= 5 || data_ov006_0214058c < 3) {
                int i;
                char *p = (char *)mArray1;
                for (i = 0; i < 4; i++) {
                    if (func_ov006_020ccd04((int *)p)) {
                        func_ov006_020cc9fc((char *)mArray1 + i * 0xd0);
                        break;
                    }
                    p += 0xd0;
                }
            }
            mTimer = 0x168;
            {
                int amt = data_ov006_02140588 * 2;
                if (amt > 0xb4) amt = 0xb4;
                mTimer -= amt;
            }
        }
    }

    if (data_ov006_02142f60 == 0) {
        if (_Z15ApproachLinear2Rsss((short *)&mDoorSwitchTimer, 0, 1)) {
            if (func_ov006_02121768(c)) {
                func_ov006_02121750(c, 0);
                func_ov006_02120a18((u16 *)&mDoorMark, mDoorSide);
            } else {
                func_ov006_02121750(c, 1);
                func_ov006_02120a18((u16 *)&mDoorMark, mDoorSide);
            }
            Sound::PlayBank2_2D(0x1b6);
            {
                unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff;
                int prod = (rnd >> 19) * 0x2d0;
                mDoorSwitchTimer = (s16)((prod >> 12) + 0x2d0);
                mDoorBlinkTimer = 0;
                mDoorBlinkPhase = 1;
            }
        }
    }

    if (mDoorSwitchTimer <= 0x3c) {
        if (_Z15ApproachLinear2Rsss((short *)&mDoorBlinkTimer, 0, 1)) {
            mDoorBlinkTimer = 0xa;
            if (mDoorBlinkPhase != 0)
                mDoorBlinkPhase = 0;
            else
                mDoorBlinkPhase = 1;
        }
    }

    if (mDoorSide != 0) {
        _Z14ApproachLinearRiii(&mArrow1X, 0, 1);
        _Z14ApproachLinearRiii(&mArrow2X, 0x20, 1);
    } else {
        _Z14ApproachLinearRiii(&mArrow1X, 0x20, 1);
        _Z14ApproachLinearRiii(&mArrow2X, 0, 1);
    }
}

// @symbol _ZN17dScMgTrampoline_c12BeginResultsEv
void dScMgTrampoline_c::BeginResults()
{
    char *c = (char *)this;
    func_ov006_020cd1e0(c);
    mTimer = 0x5a;
    *(P2Words *)mState = *(P2Words *)&data_ov006_0213fac8;
}

// @symbol _ZN17dScMgTrampoline_c12StateResultsEv
void dScMgTrampoline_c::StateResults()
{
  unsigned char *c = (unsigned char *)this;
  func_ov006_020cd39c(c);
  if(_Z14ApproachLinearRiii(&mTimer,0,1)==0) return;
  if(func_ov006_020cd158()==0) return;
  func_ov004_020b0a54(0x12);
  mPromptEnabled = 0;
  mTimer = 0xb4;
  *(P2Words *)mState = *(P2Words *)&data_ov006_0213faa0;
}

// @symbol _ZN17dScMgTrampoline_c13StateWaitExitEv
void dScMgTrampoline_c::StateWaitExit()
{
    int idx, b;
    mTimer -= 1;
    if (mTimer != 0)
    {
        idx = data_020a0e40[0];
        b = 0;
        if (data_020a0de8[idx].v != 0 && data_020a0de9[idx].v != 0)
            b = 1;
        if (b == 0)
            return;
    }
    {
        int r;
        volatile unsigned short h;
        r = func_02054d88();
        h = 0;
        MultiStore16(h, (char *)r, 0x6000);
    }
    mRoundOver = 1;
    {
        int w0 = ((int *)&data_ov006_0213faa8)[0];
        int w1 = ((int *)&data_ov006_0213faa8)[1];
        w0 = w1 ? w0 : w0;
        mState[0] = w0;
        mState[1] = w1;
    }
}

// @symbol _ZN17dScMgTrampoline_c9StateDoneEv
void dScMgTrampoline_c::StateDone()
{
}

// @symbol func_ov006_02121768
extern "C" {  /* .c-derived member: C linkage for the whole block */
short func_ov006_02121768(char* scene) {
  return ((dScMgTrampoline_c *)scene)->mDoorSide;
}
}

// @symbol func_ov006_02121750
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02121750(char *scene, short value)
{
    data_ov006_02140538 = value;
    ((dScMgTrampoline_c *)scene)->mDoorSide = value;
}
}

// @symbol _ZN17dScMgTrampoline_c16UpdateTouchInputEv
void dScMgTrampoline_c::UpdateTouchInput()
{
    int i;
    int b;

    if (mInputEnabled == 0) {
        mTouching = 0;
        return;
    }

    i = data_020a0e40[0];
    b = (data_020a0de8[i].v != 0 && data_020a0de9[i].v != 0);
    if (b) {
        mTouchX = data_020a0dea[i].v;
        mTouchStartX = mTouchX;
        mTouchY = data_020a0deb[i].v;
        mTouchStartY = mTouchY;
        mTouching = 1;
        mDragSoundHandle = 0;
    }

    if (mTouching != 1) return;

    i = data_020a0e40[0];
    if (data_020a0de8[i].v != 0) {
        func_ov004_020ae5c4(this, mTouchX, mTouchY,
                            data_020a0dea[i].v, data_020a0deb[i].v, 2, 4);
        i = data_020a0e40[0];
        mTouchX = data_020a0dea[i].v;
        mTouchY = data_020a0deb[i].v;
        mDragSoundHandle = func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0,
                                         func_020126e8(mTouchX << 12), 0, 0);
    } else {
        b = (int)(data_020a0de8[i].v == 0 && data_020a0de9[i].v != 0);
        if (b != 0) { mTouchReleased = 1; }
    }
}

// @symbol _ZN17dScMgTrampoline_c8BehaviorEv
/* Slot 6. The field at 0x5004 is a pointer to member function: the current
 * state, one of the five State functions. The header keeps it as two raw
 * words, so it is cast to dScMgTrampoline_c::State here to dispatch. */
s32 dScMgTrampoline_c::Behavior()
{
    int saved = data_ov006_02140588;
    func_ov006_02120c40();
    (this->*(*(State *)mState))();
    UpdateTouchInput();
    func_ov006_021209ac(&mDoorMark.row);
    if (saved != data_ov006_02140588)
        func_ov004_020adb1c(data_ov006_02140588);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c6RenderEv
/* Draws the three miss icons, the two door arrows, and the flashing mark.
   The arrows slide in mArrow1X / mArrow2X; mDoorBlinkPhase picks the frame
   on the lit side. */
s32 dScMgTrampoline_c::Render()
{
    int count;
    int a1v;
    int i;
    int aa;
    int r6, r5;
    int t;

    func_ov006_0212093c(&mDoorMark.row, mScrollY);
    func_ov006_02120c08();

    if (unk_4664 == 1) {
        count = data_ov006_0213b0ec;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void*)data_ov006_0213fb04[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                RenderOamMainScreen(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140588, 4);

    aa = mDoorSide;
    r6 = 1;
    r5 = 1;
    if (aa == 0) {
        if (mDoorBlinkPhase != 0) r6 = 0;
    }
    if (aa != 0) {
        if (mDoorBlinkPhase != 0) r5 = 0;
    }

    t = data_ov006_0212f0c8[0] - (mScrollY + mScrollOffsetY);
    func_ov004_020afdd0(data_ov006_02134f08, mArrow1X + 0xf0, t, -1, 2);
    func_ov004_020afdd0(data_ov006_02134f00[r6], 0xf0, t - 0x20, -1, 2);

    t = data_ov006_0212f0c8[1] - (mScrollY + mScrollOffsetY);
    func_ov004_020afdd0(data_ov006_02134f08, mArrow2X + 0xf0, t, -1, 2);
    func_ov004_020afdd0(data_ov006_02134f00[r5], 0xf0, t - 0x20, -1, 2);

    if (mRoundOver == 0) {
        func_ov006_020cd270();
        func_ov006_020d09e0();
    }
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c16CleanupResourcesEv
/* Slot 3. */
s32 dScMgTrampoline_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c8OnKickedEv
/* dScMgTrampoline_c::OnKicked, from its vtable slot. */
int dScMgTrampoline_c::OnKicked()
{
    if (!dScMgD3DBase_c::OnKicked())
        return 0;
    if (mMenuOpen == 0) {
        if (unk_4664 == 0) {
            data_0209d45c &= ~8;
            SetBg2Offset(0, mScrollY + mScrollOffsetY);
            *(volatile int*)0x4000018 = (mScrollY + mScrollOffsetY) << 16 & 0x1ff0000;
        } else {
            data_0209d45c |= 8;
            SetBg2Offset(0, mScrollY + 0xc0 + mScrollOffsetY + func_ov004_020b04c0());
            *(volatile int*)0x4000018 = (mScrollY + 0xc0 + mScrollOffsetY + func_ov004_020b04c0()) << 16 & 0x1ff0000;
        }
    }
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c8OnPushedEv
/* dScMgTrampoline_c::OnPushed, from its vtable slot. */
int dScMgTrampoline_c::OnPushed()
{
    return dScMgD3DBase_c::OnPushed() != 0;
}

// @symbol _ZN17dScMgTrampoline_c11OnAttacked2Ev
/* dScMgTrampoline_c::OnAttacked2, from its vtable slot. */
int dScMgTrampoline_c::OnAttacked2()
{
    volatile u16 z;
    Vec2s v1;
    Vec2s v2;
    s16 a;
    int dst;

    if (!dScMgBase_c::OnAttacked2())
        return 0;

    if (data_0209f5bc->vt[5](data_0209f5bc)) {
        data_ov006_0213fa9c = 0;
    } else if (data_0209f5bc->vt[7](data_0209f5bc)) {
        data_ov006_0213fa9c = 1;
    }

    if (data_ov006_0213fa9c != 0)
        return 0;

    if (mMenuOpen != 0)
        return 0;

    if (mTouchReleased != 0) {
        a = mTouchStartX;
        if ((a < 0x24 && mTouchX < 0x24) ||
            (a > 0xd4 && mTouchX > 0xd4)) {
            func_02012790(0xe);
        } else {
            v1.x = *(volatile s16 *)&mTouchStartX;
            v1.y = mTouchStartY;
            v2.x = mTouchX;
            v2.y = mTouchY;
            if (func_ov006_020d0c38(&v1, &v2)) {
                func_02012718(0x1af, (mTouchStartX + mTouchX) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        dst = func_02054d88();
        z = 0;
        MultiStore16(z, (char *)dst, 0x6000);
        mTouching = 0;
        mTouchReleased = 0;
    }
    return 1;
}

// @symbol func_ov006_02120f18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120f18(struct Obj *self, int layer)
{
    volatile unsigned short t0;
    volatile unsigned short t1;
    int tile = 0x7000;
    char *scrPtr;
    char *charPtr;
    int i;

    self->brushOn = 1;
    self->bgLayer = layer;

    switch (self->bgLayer) {
    case 0:
        scrPtr = (char *)G2::GetBG0ScrPtr();
        charPtr = (char *)func_02054efc();
        break;
    case 1:
        scrPtr = (char *)G2::GetBG1ScrPtr();
        charPtr = (char *)func_02054ea8();
        break;
    case 2:
        scrPtr = (char *)G2::GetBG2ScrPtr();
        charPtr = (char *)G2::GetBG2CharPtr();
        break;
    case 3:
        scrPtr = (char *)G2::GetBG3ScrPtr();
        charPtr = (char *)func_02054d88();
        break;
    default:
        return;
    }

    for (i = 0; i < 0x300; i++) {
        t0 = tile;
        MultiStore16(t0, scrPtr, 2);
        tile = (unsigned short)(tile + 1);
        scrPtr += 2;
    }

    t1 = 0;
    MultiStore16(t1, charPtr, 0x6000);
    GX::LoadBGPltt(data_ov006_0212f0d0, 0xe0, 0x20);
}
}

#pragma opt_loop_invariants off

// @symbol _ZN17dScMgTrampoline_c9Virtual88Eiiii
/* Slot 34, the brush stamp. dScMgD3DBase_c does not override this slot,
   so each trampoline class has its own; they differ only in the shape
   table (data_ov006_02142f6c here, _02142f78 in dScMgTrampoline2_c).
   bgLayer (+0x6c, in the base padding) picks the character base. No
   wrapped-region branch: a 3D minigame owns the top screen. */
void dScMgTrampoline_c::Virtual88(int x_base, int y, int val, int n)
{
    char *raw = (char *)this;

    int half;
    int x0;
    int j;
    int x;
    int *addr;
    int i;
    int yy;
    int mask = 0xf;

    half = n / 2;
    j = 0;
    if (j < n)
    {
        yy = y - half;
        x0 = x_base - half;
        do
        {
            for (i = 0; i < n; i++)
            {
                x = i + x0;
                if (x >= 0 && x < 0x100)
                {
                    if (yy >= 0 && yy < 0xc0)
                    {
                        void *ret;
                        int mode = ((Obj *)raw)->bgLayer;

                        switch (mode)
                        {
                            case 0:
                                ret = func_02054efc();
                                break;
                            case 1:
                                ret = func_02054ea8();
                                break;
                            case 2:
                                ret = G2::GetBG2CharPtr();
                                break;
                            case 3:
                                ret = (void *)func_02054d88();
                                break;
                            default:
                                return;
                        }

                        addr = (int *)((char *)ret +
                            ((x / 8) + (yy / 8) * 32) * 32 +
                            (yy & 7) * 4);

                        MultiCopy_Int(addr, data_ov006_02142f6c, 4);
                        data_ov006_02142f6c[0] =
                            (data_ov006_02142f6c[0] &
                             (-1 ^ (mask << ((x & 7) * 4)))) |
                            (val << ((x & 7) * 4));
                        MultiCopy_Int(data_ov006_02142f6c, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

// @symbol func_ov006_02120d8c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120d8c(void *records, int count) { data_ov006_02142f68 = (int)records; data_ov006_02142f70 = count; }
}

// @symbol func_ov006_02120d0c
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Desc* index DIFFed (the scale folded the wrong register). */
void func_ov006_02120d0c(int x, int y) {
  int i;
  for (i = data_ov006_02142f70 - 1; i >= 0; i--) {
    if (*(short *)(data_ov006_02142f68 + i * 0x24 + 0x20) == 0) {
      func_ov006_02120b30((Desc *)(data_ov006_02142f68 + i * 0x24), x, y);
      return;
    }
  }
}
}

// @symbol func_ov006_02120ca0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120ca0(void){
    int i;
    for(i=0;i<data_ov006_02142f70;i++){
        ((Desc *)(data_ov006_02142f68 + i * 0x24))->active = 0;
    }
    data_ov006_02142f64=0;
}
}

// @symbol func_ov006_02120c40
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120c40(void){
  int i = 0;
  if(data_ov006_02142f70 > 0){
    int off = 0;
    do {
      func_ov006_02120ab8((char *)(data_ov006_02142f68 + off));
      i++;
      off += 0x24;
    } while(i < data_ov006_02142f70);
  }
}
}

// @symbol func_ov006_02120c08
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120c08(void) {
    void *node = data_ov006_02142f64;
    if (node == 0) return;
    do {
        func_ov006_02120a64((char *)node);
        node = *(void **)node;
    } while (node != 0);
}
}

// @symbol func_ov006_02120bc8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120bc8(int *entryRaw) {
    Desc *entry = (Desc *)entryRaw;
    Desc *node;
    entry->next = data_ov006_02142f64;
    data_ov006_02142f64 = (Node *)entry;
    node = (Desc *)entry->next;
    int v = entry->y - 0x10000;
    while (node) {
        node->targetY = v;
        node = (Desc *)node->next;
        v -= 0x10000;
    }
}
}

// @symbol func_ov006_02120b7c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120b7c(struct Node* node) {
    struct Node* prev = data_ov006_02142f64;
    if (prev == node) {
        data_ov006_02142f64 = node->next;
        return;
    }
    struct Node* cur = prev->next;
    while (cur) {
        if (cur == node) {
            prev->next = node->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
}

// @symbol func_ov006_02120b30
/* Fills a particle descriptor with fixed parameters and tail-calls
 * func_ov006_02120bc8. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120b30(struct Desc *self, int y, int number)
{
    self->x = 0x110000;
    self->y = y;
    self->speedX = -0x4000;
    self->speedY = 0;
    self->targetX = 0xc0000;
    self->targetY = y;
    self->life = 0x80;
    self->active = 1;
    self->number = number;
    func_ov006_02120bc8((int *)self);
}
}

// @symbol func_ov006_02120ab8
extern "C" {  /* .c-derived member: C linkage for the whole block */

int _Z14ApproachLinearRiii(int *a, int b, int c);
void func_0203d630(int *p, int m);
/* Member form of x += speedX DIFFed. Offsets are Desc's. */
void func_ov006_02120ab8(char *self)
{
  if ((*((short *) (self + 0x20))) == 0)
  {
    return;
  }
  if (_Z15ApproachLinear2Rsss((short *) (self + 0x1c), 0, 1))
  {
    func_ov006_02120b7c((Node *)self);
    *((short *) (self + 0x20)) = 0;
  }
  {
    int *p = (int *) (((int) self + 4));
    *p = *p + (*((int *) (self + 0x14)));
  }
  _Z14ApproachLinearRiii((int *) (self + 8), *((int *) (self + 0x10)), 0x1800);
  func_0203d630((int *) (self + 0x14), 0xf00);
}
}

// @symbol func_ov006_02120a64
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Member form DIFFed (ldrsh of number). Offsets are Desc::active, x, y, number. */
void func_ov006_02120a64(char *desc){
  if(*(short*)(desc+0x20)==0) return;
  func_ov004_020b2444(*(int*)(desc+4)>>12,*(int*)(desc+8)>>12,*(short*)(desc+0x1e),-1,-1,0,0);
}
}

// @symbol func_ov006_02120a54
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120a54(char *label)
{
    DoorMark *mark = (DoorMark *)label;
    mark->active = 0;
    mark->shown = 0;
}
}

// @symbol func_ov006_02120a44
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120a44(char *label)
{
    DoorMark *mark = (DoorMark *)label;
    mark->active = 0;
    mark->shown = 0;
}
}

// @symbol func_ov006_02120a18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120a18(unsigned short* label, int row)
{
    DoorMark *mark = (DoorMark *)label;
    mark->row = (s16)row;
    mark->timer = 0x3c;
    mark->shown = 1;
    mark->active = 1;
    Sound::PlayBank2_2D(0x1B7);
}
}

// @symbol func_ov006_021209ac
extern "C" void func_ov006_021209ac(short *label)
{
    DoorMark *mark = (DoorMark *)label;
    if (mark->active == 0) return;
    if (_Z15ApproachLinear2Rsss(&mark->timer, 0, 1) != 0) {
        mark->active = 0;
    }
    if (((mark->timer / 10) & 1) != 0) {
        mark->shown = 0;
    } else {
        mark->shown = 1;
    }
}

// @symbol func_ov006_0212093c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0212093c(short* label, int scrollY) {
    DoorMark *mark = (DoorMark *)label;
    if (mark->active == 0) return;
    if (mark->shown == 0) return;
    func_ov004_020afdd0(
        (void *)data_ov006_02134f24,
        0xd0,
        data_ov006_0212f0c8[mark->row] - scrollY - 8,
        -1,
        -1);
}
}

// @symbol func_ov006_02120938
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_02120938(void)
{
}
}

