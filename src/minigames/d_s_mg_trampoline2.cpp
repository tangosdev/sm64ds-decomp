//cpp
#pragma opt_loop_invariants off
/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in.
 *
 * Functions are in reverse ROM order; do not reorder.
 * opt_loop_invariants is off for the whole file. TUBUILD CONFLICT lines
 * stay; the manifest pairs them.
 *
 * State callbacks stay free functions; their original names are unknown.
 * func_ov006_02123cb4 keeps raw int offsets for mTimer, mPattern, mScoreGate,
 * mScriptDone and mWaveStep: member form DIFFs there (code shifts; the
 * function grows). Its mRamp access matches.
 * 0x7acc has no matched read. The factory still builds the arrays by offset.
 * Model / TextureTransformer / SysTracker C1/D1 stay mangled (built on raw
 * storage); NewUnkCallback818 takes Fix12 by value, which costs the caller
 * bytes (notes/mwccarm-codegen.md 6az); the G2 BG getters stay mangled because
 * decl_common.h's global G2 hides the namespace; SetBlendAlpha keeps a
 * local declaration.
 */

#include "common.h"
#include "types.h"
#include "dScMgTrampoline2_c.h"
#include "Particle__System.h"
#define func_ov006_021227c8 func_ov006_021227c8_decl_common_shadow
#define func_ov006_02122c68 func_ov006_02122c68_decl_common_shadow
#define _ZTV18dScMgTrampoline2_c _ZTV18dScMgTrampoline2_c_decl_common_shadow
#include "decl_common.h"
#undef func_ov006_021227c8
#undef func_ov006_02122c68
#undef _ZTV18dScMgTrampoline2_c

extern int _ZTV18dScMgTrampoline2_c[];

/* C, Obj, B4, P2, S2, T, SpawnDef and the forward declarations below are
 * local stand-ins for types that have no header yet (issue #2497). */
struct C {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(void *arg);
};

struct SharedFilePtr { void Release(); };

struct BMD_File;

/* Shadow of the scene's +0x68/+0x6c, in dScMgBase_c padding.
   The brush layer. */
struct Obj {
    unsigned char pad0[0x68];
    unsigned char brushOn;
    unsigned char pad1[3];
    int bgLayer;
};

struct Vector3_16f;

typedef void (dScMgTrampoline2_c::*dScMgTrampoline2_cState)();

struct Vec3i {
    int x;
    int y;
    int z;
};

struct B4 {
    unsigned char v;
    unsigned char pad[3];
};

struct P2 { int words[2]; };

union StateValue {
    int words[2];
    double asDouble;
};

struct TrampolineTerrorProfile {
    void *(*spawn)(void);
    s16 behaviorPriority;
    s16 renderPriority;
};

typedef char TrampolineTerrorProfile_size_must_be_8[
    sizeof(TrampolineTerrorProfile) == 8 ? 1 : -1];

struct Tbl { int a, b, c; };

typedef unsigned char u8;

typedef unsigned short u16;

typedef struct {
    int count;
    int base;
    short *a;
    short *b;
    short *c;
} SpawnDef;

struct S2 { int words[2]; };

/* TUBUILD CONFLICT -- alternate body of struct 'Model', from the legacy file for func_ov006_02122c68, NOT applied:
struct Model { ~Model(); };
*/

extern "C" {
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
extern Matrix4x3 data_020a0e68;
extern "C" void func_ov006_02122814(char *c);
extern void AddVec3(struct Vector3* a, struct Vector3* b, struct Vector3* c);
extern "C" SharedFilePtr data_ov006_02142f80;
extern int data_ov006_02142f74;
extern unsigned char *data_ov006_02142f7c;
extern void func_ov006_0212287c(char *obj);
extern void func_ov006_021228bc(void *obj, void *kind);
extern void func_ov006_02122a1c(void* self);
extern void *_ZN5ModelC1Ev(void *model);
extern void _ZN5ModelD1Ev(void *model);
extern int data_ov006_02142f78[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern char *_ZN2G212GetBG0ScrPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern char *_ZN2G212GetBG3ScrPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern const unsigned char data_ov006_0212f1ac[];
extern char* data_0209f5bc;
extern int data_ov006_0213fbc4;
unsigned int func_02012790(unsigned int a);
int func_ov006_020d0c38(u16* a, u16* b);
extern "C" void func_ov004_020ad90c(void);
void func_0203cd80(int *m, short angle);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void func_ov006_020eef90(void);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0212f328;
extern int func_ov006_021238d0(int kind);
extern unsigned short func_ov006_0212382c(int column, int side);
extern int func_ov006_021237c8(int kind);
extern int func_ov006_0212373c(int kind);
extern void func_0203cc28(int *vector, int angle);
extern char *func_ov006_020cefa4(int resource, int *up, int variant, int face);
extern volatile unsigned char data_020a0e40[];
extern struct B4 data_020a0de8[];
extern struct B4 data_020a0de9[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];
extern int func_ov004_020ae5c4(int a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int data_ov006_0213fbd0[];
extern void func_ov006_020cedf0(void);
extern int func_ov006_020ca7b8(void);
extern union StateValue data_ov006_0213fbe0;
extern void func_ov006_020ca840(void *c);
extern struct P2 data_ov006_0213fbf0;
extern int data_ov006_02140818;
extern int data_ov006_0213b0f0;
extern int data_ov006_02140830;
extern int data_ov006_021405bc;
extern int data_ov006_02140828;
extern int data_ov006_0213fc10[];
extern SpawnDef data_ov006_0212f2b0[];
extern void func_ov006_020d0ac0(void);
extern void func_ov006_020cac30(void);
extern void func_ov006_020cad3c(int a);
extern void func_ov006_02123c78(char *c);
extern void func_ov006_020ca8e0(void);
extern void func_ov006_02123428(char *c);
extern void func_ov006_020caa08(void* a);
extern struct S2 data_ov006_0213fbe8;
extern void func_ov006_020d0b78(void);
extern void func_ov006_02124040(char *c);
extern s16 data_ov006_0212e058;
extern s16 data_ov006_0212e050;
extern s16 data_ov006_0212e048;
extern short data_ov006_0212e044;
extern struct P2 data_ov006_0213fbd8;
extern "C" void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern "C" void func_ov006_020d0b04(u32 a);
extern void func_ov006_02124228(char *self);
extern volatile s16 data_020a0dbc[];
extern void Camera_UpdateMatrices(void *cam);
extern u8 data_0209d45c;
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *ptr);
extern void func_ov006_02122e20(void *self, int a);
extern int data_ov006_0213fc34[];
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
extern void *_ZN11dScMgBase_cC2Ev(void *scene);
extern void _ZN8Particle10SysTrackerC1Ev(void *tracker);
extern void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
extern int _ZTV14dScMgD3DBase_c[];
extern void _ZN6Player29TryExitCharacterDoorWithIntroEv(void);
extern void func_ov006_020d100c(void);
extern void func_ov006_020efc08(void *object);
extern void func_ov006_021225a8(void);
extern void *_ZN18TextureTransformerC1Ev(void *transformer);
/* local extern: only sibling minigame headers (dScMgAmida_c.h and four
   others) declare it; this TU includes none of them */
extern void NullDestructor_0203d47c(void);
extern void func_0203d738(void);
extern int func_ov006_02122c90(char *object);
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_02122814, from the legacy file for func_ov006_02122a4c, NOT applied: extern void func_ov006_02122814(char *obj); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f7c, from the legacy file for func_ov006_02122b24, NOT applied: extern int data_ov006_02142f7c; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f7c, from the legacy file for func_ov006_02122b88, NOT applied: extern unsigned char *data_ov006_02142f7c; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f7c, from the legacy file for func_ov006_02122c04, NOT applied: extern int data_ov006_02142f7c; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02142f80, from the legacy file for func_ov006_02122c68, NOT applied: extern SharedFilePtr data_ov006_02142f80; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_02122e20, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054efc, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054efc(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054ea8, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054ea8(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for _ZN18dScMgTrampoline2_c11OnAttacked2Ev, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN18dScMgTrampoline2_c11OnAttacked2Ev, NOT applied: void MultiStore16(u16 val, char* dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov006_021237c8, NOT applied: extern int RandomIntInternal(void* seed); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02123b24, NOT applied: extern void MultiStore16(unsigned short a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123bf4, NOT applied: extern int _Z14ApproachLinearRiii(int &v, int a, int b); */
/* Both callers use the existing ov006 helpers' void(void) contracts. The
 * unrelated Player::St_Null_Init at the same RAM address belongs to ov002. */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123cb4, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203cc28, from the legacy file for func_ov006_02123cb4, NOT applied: extern void func_0203cc28(int *p, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020cefa4, from the legacy file for func_ov006_02123cb4, NOT applied: extern char *func_ov006_020cefa4(int a0, int *a1, int a2, int a3); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02124088, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02124088, NOT applied: extern void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02124088, NOT applied: extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov004_020ae5c4, from the legacy file for func_ov006_02124088, NOT applied: extern int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_0212e048, from the legacy file for func_ov006_02124228, NOT applied: extern short data_ov006_0212e048; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c13OnTurnIntoEggEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c13OnYoshiTryEatEi, NOT applied: extern "C" s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN18dScMgTrampoline2_c13OnYoshiTryEatEi, NOT applied: extern "C" void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd4fdivEii, from the legacy file for _ZN18dScMgTrampoline2_c13InitResourcesEv, NOT applied: extern s32 _ZN4cstd4fdivEii(s32 a, s32 b); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_021245a8, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_021245a8, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_021245a8, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
 /* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_021245a8, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */

void *dScMgTrampoline2_c_classInit(void);
void func_ov006_02123b20(void);
void func_ov006_02123b24(char *scene);
void func_ov006_02123bf4(char *scene);
void func_ov006_02123cb4(char *scene);
void func_ov006_02124088(char *scene);

/* Data owned by this production TU; the original TU grouping is inferred. */
int data_ov006_0213fbc4 = 1;
TrampolineTerrorProfile g_profile_MG_TRAMPOLINE2 = {
    dScMgTrampoline2_c_classInit, 0x181, 0x181
};
int data_ov006_0213fbd0[] = { (int)func_ov006_02123b20, 0 };
P2 data_ov006_0213fbd8 = {{ (int)func_ov006_02124088, 0 }};
StateValue data_ov006_0213fbe0 = {{ (int)func_ov006_02123b24, 0 }};
S2 data_ov006_0213fbe8 = {{ (int)func_ov006_02123cb4, 0 }};
P2 data_ov006_0213fbf0 = {{ (int)func_ov006_02123bf4, 0 }};
Tbl data_ov006_0213fbf8 = { 0x600, 0xc00, 0xe00 };
int data_ov006_0213fc10[] = { 1000, 5000, 10000, 20000 };
int data_ov006_0213fc20[] = {
    (int)data_ov006_02134d40,
    (int)data_ov006_02134d4c,
    (int)data_ov006_02134d58,
    (int)data_ov006_02134d34,
    (int)data_ov006_02134d28
};
int data_ov006_0213fc34[] = { 0x107, 0x109, 0x104, 0x105, 0x106 };
int *data_ov006_0213fc48[] = {
    data_ov006_021373a4,
    data_ov006_021373fc,
    data_ov006_0213734c,
    data_ov006_021372f4,
    data_ov006_02137454
};
}

/* This overload is C++, unlike the address-spelled C ABI declarations above. */
extern int ApproachLinear(int &x, int target, int step);
short ApproachLinear2(short &value, short target, short step);
namespace G3X { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { int fdiv(int numer, int denom); }
namespace GX {
void LoadBGPltt(const void *src, unsigned int offset, unsigned int size);
void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size);
}

// @symbol func_ov006_021248a8
extern "C" void *func_ov006_021248a8(char *raw)
{
    _ZN5ModelC1Ev(raw + 0xa0);
    _ZN5ModelC1Ev(raw + 0xf0);
    _ZN5ModelC1Ev(raw + 0x140);
    _ZN18TextureTransformerC1Ev(raw + 0x194);
    __cxa_vec_ctor(raw + 0x1a8, 5, 8,
                  (void *)func_0203d738, (void *)NullDestructor_0203d47c);
    return raw;
}

// @symbol dScMgTrampoline2_c_classInit
extern "C" void *dScMgTrampoline2_c_classInit(void)
{
    char *scene = (char *)fBase_c::operator new(0x7bac);
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
        *(int *)scene = (int)&_ZTV18dScMgTrampoline2_c[2];
        __cxa_vec_ctor(scene + 0x500c, 5, 0xdc,
                      (void *)_ZN6Player29TryExitCharacterDoorWithIntroEv,
                      (void *)func_ov006_020ca604);
        __cxa_vec_ctor(scene + 0x5458, 3, 0x32c,
                      (void *)func_ov006_020d100c,
                      (void *)func_ov006_020d1008);
        __cxa_vec_ctor(scene + 0x5ddc, 0xa, 0x1d0,
                      (void *)func_ov006_021248a8,
                      (void *)func_ov006_021227c8);
        __cxa_vec_ctor(scene + 0x6ffc, 0xa, 0x24,
                      (void *)func_ov006_020efc08,
                      (void *)func_ov006_020eed64);
        __cxa_vec_ctor(scene + 0x7164, 0x14, 0x78,
                      (void *)func_ov006_02122c90,
                      (void *)func_ov006_02122c68);
        __cxa_vec_ctor(scene + 0x7ad0, 5, 0x24,
                      (void *)func_ov006_021225a8,
                      (void *)func_ov006_02120938);
    }
    return scene;
}

// @symbol func_ov006_021245a8
extern "C" {
void func_ov006_021245a8(void *arg0)
{
    void *raw = arg0;
    int objChar, objPltt, bgChar, bgScr, bgPltt;
    volatile u16 sp0;

    objChar = LoadFile(data_ov006_0213fc34[GetGameLanguage()]);
    objPltt = LoadFile(0x108);
    bgChar = LoadFile(0xa2);
    bgScr = LoadFile(0xa4);
    bgPltt = LoadFile(0xa3);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9110;
    *(volatile u16 *)0x400000c &= ~0x40;

    {
        char *dst = _ZN2G213GetBG2CharPtrEv();
        sp0 = 0;
        MultiStore16(sp0, dst, 0x4000);
    }
    DecompressLZ16(bgChar, (void *)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    DecompressLZ16(bgScr, _ZN2G212GetBG2ScrPtrEv());
    GX::LoadBGPltt((const void *)bgPltt, 0x1a0, 0x60);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1318;
    *(volatile u16 *)0x400000e &= ~0x40;

    DecompressLZ16(objChar, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)objPltt, 0, 0x60);

    Deallocate((void *)bgChar);
    Deallocate((void *)bgScr);
    Deallocate((void *)bgPltt);
    Deallocate((void *)objChar);
    Deallocate((void *)objPltt);

    func_ov006_02122e20(raw, 3);
}
}

// @symbol _ZN18dScMgTrampoline2_c13InitResourcesEv
/* Vtable slot 0 (the name is inferred): load the screens, set up both
 * cameras and the object pools, then start a round through OnYoshiTryEat. */
s32 dScMgTrampoline2_c::InitResources()
{
    char *raw = (char *)this;
    s32 fov;

    func_ov006_021245a8(raw);
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
    Camera_UpdateMatrices(raw + 0x466c);
    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x400;
    Camera_UpdateMatrices(raw + 0x4728);
    if (func_ov006_020cae9c(raw + 0x500c, 5) == 0)
        return 0;
    func_ov006_02120d8c(raw + 0x7ad0, 5);
    func_ov006_020d0b2c();
    func_ov006_020cef14((char *)(raw + 0x5ddc), 10);
    func_ov006_020ef0d4((int)(raw + 0x6ffc), 10);
    data_ov006_021421b4 = 0;
    func_ov006_02122c04((int)(raw + 0x7164), 0x14);
    func_ov004_020b04d0(0x10);
    OnYoshiTryEat(-1);
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c13OnYoshiTryEatEi
/* Vtable slot 18: reset the round. */
void dScMgTrampoline2_c::OnYoshiTryEat(int /* arg */)
{
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4000050, 1, 0x2e, 0x10, 0x10);

    mTouchStartX = data_020a0dbc[0];
    mTouchStartY = data_020a0dbc[1];
    mTouchX = data_020a0dbc[0];
    mTouchY = data_020a0dbc[1];

    unk_0bc = 0;
    if (unk_0bc > 0x270e)
        unk_0bc = 0x270e;

    func_ov006_020cad3c(0x1000);
    func_ov006_020cac9c(unk_0bc);
    data_ov006_021405bc = 1;
    func_ov006_020d0b04(unk_0bc);
    func_ov006_020cee5c(unk_0bc);
    func_ov006_020eeff0();
    func_ov006_02122b24();
    func_ov006_02120ca0();
    func_ov006_020c8a9c(0, data_ov006_0213fc20[GetGameLanguage()]);

    mInputEnabled = 0;
    mRoundOver = 0;
    mPattern = 0;
    mScriptDone = 0;
    mWaveStep = 0;
    mRamp = 0x14000;

    {
        volatile u16 z;
        char *dst = (char *)func_02054d88();
        z = 0;
        MultiStore16(z, dst, 0x6000);
    }

    func_ov006_02124228((char *)this);
}

// @symbol _ZN18dScMgTrampoline2_c13OnTurnIntoEggEi
/* OnTurnIntoEgg: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnTurnIntoEgg(int /* mode */)
{
    func_ov006_020c8a9c(0, data_ov006_0213fc20[GetGameLanguage()]);
    return 1;
}

// @symbol func_ov006_02124228
extern "C" {
void func_ov006_02124228(char* raw) {
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
    short a, b;
    self->mTimer = 0x5a;
    a = data_ov006_0212e044;
    self->mTouchStartX = a;
    b = data_ov006_0212e048;
    self->mTouchStartY = b;
    self->mTouchX = a;
    self->mTouchY = b;
    self->mDragSoundHandle = 0;
    self->mScoreGate = 0x3e8;
    *(struct P2*)self->mState = data_ov006_0213fbd8;
}
}

// @symbol func_ov006_02124088
extern "C" {
void func_ov006_02124088(char *raw)
{
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
    int counter;

    func_ov006_020d0ac0();
    self->mTimer -= 1;
    counter = self->mTimer;

    if (counter == 0) {
        if (self->mPromptBlinkCount == 0) {
            self->mPromptEnabled = 1;
            self->mPromptBlinkCount = 1;
            self->mPromptBlinkTimer = 0;
        }

        {
            int dst = func_02054d88();
            volatile u16 tmp = 0;
            MultiStore16(tmp, (char *)dst, 0x6000);
        }

        func_ov006_020d0b78();
        func_02012718(0x1af, 0x8000);
        func_ov006_02124040(raw);
        return;
    }

    {
        int t = cstd::fdiv(counter << 12, 0x5a000);
        volatile s16 old9c = self->mTouchX;
        volatile s16 old9e = self->mTouchY;
        int mixRaw = data_ov006_0212e050 * t + data_ov006_0212e058 * (0x1000 - t);
        self->mTouchX = (s16)(mixRaw >> 12);
        self->mTouchY = data_ov006_0212e048;

        self->mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) - 0x800) << 2) >> 12);

        func_ov004_020ae5c4((int)raw, old9c, old9e, self->mTouchX, self->mTouchY, 2, 0xc);

        self->mDragSoundHandle = func_02012468(self->mDragSoundHandle, 2, 0x1b0, 2, 0,
                                              func_020126e8(mixRaw), 0, 0);
    }
}
}

// @symbol func_ov006_02124040
extern "C" {
void func_ov006_02124040(char* raw){
  dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
  self->mTimer = 0xb4;
  self->mInputEnabled = 1;
  func_ov006_020caa08(raw+0x7b00);
  *(struct S2*)self->mState = data_ov006_0213fbe8;
}
}

// @symbol func_ov006_02123cb4
extern "C" {
void func_ov006_02123cb4(char *raw)
{
    int old;

    old = data_ov006_02140818;
    func_ov006_020d0ac0();
    func_ov006_020cedf0();
    func_ov006_020cac30();
    if (old < data_ov006_02140818) {
        int n = data_ov006_02140818 - old;
        int i;
        for (i = 0; i < n; i++)
            ApproachLinear(((dScMgTrampoline2_c *)raw)->mRamp, 0x7000, -0x800);
        if (data_ov006_02140818 == 3)
            *(u8 *)(raw + 0xc3) = 0;
    }
    func_ov006_020cad3c((data_ov006_02140818 / 10) * 64 + 0x1000);
    if (data_ov006_0213b0f0 == 0) {
        ((dScMgTrampoline2_c *)raw)->mInputEnabled = 0;
        func_ov006_02123c78(raw);
        return;
    }
    if (data_ov006_02140830 >= *(int *)(raw + 0x7b90) && data_ov006_021405bc < 5) {
        unsigned int v;
        func_ov006_020ca8e0();
        {
            int *p = (int *)((int)raw + 0xbc);
            *p = *p + 1;
        }
        if (*(unsigned int *)(raw + 0xbc) > 0x270e)
            *(unsigned int *)(raw + 0xbc) = 0x270e;
        v = *(unsigned int *)(raw + 0xbc);
        if (v < 4)
            *(int *)(raw + 0x7b90) = data_ov006_0213fc10[v];
        else
            *(int *)(raw + 0x7b90) = 0x7fffffff;
        return;
    }
    if (data_ov006_02140828 != 0)
        return;
    /* Member form DIFFs here (bytes, not relocations): these are mTimer,
       mScoreGate, mPattern, mScriptDone and mWaveStep. */
    if (ApproachLinear(*(int *)(raw + 0x7b84), 0, 1) == 0)
        return;
    if (*(int *)(raw + 0x7b94) == 0) {
        SpawnDef *d = &data_ov006_0212f2b0[*(int *)(raw + 0x7b88)];
        int i;
        Vec3i vecA;
        for (i = 0; i < d->count; i++) {
            vecA.x = 0;
            vecA.y = 0x1000;
            vecA.z = 0;
            func_0203cc28((int *)&vecA, d->a[i]);
            func_ov006_020cefa4(d->base + i * 0xc, (int *)&vecA, d->b[i], d->c[i]);
        }
        if (ApproachLinear(*(int *)(raw + 0x7b88), 6, 1)) {
            *(int *)(raw + 0x7b94) = 1;
            *(int *)(raw + 0x7b98) = 0;
        }
    } else {
        if (ApproachLinear(*(int *)(raw + 0x7b98), 4, 1)) {
            unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff;
            SpawnDef *d = &data_ov006_0212f2b0[(int)((rnd >> 19) * 6) >> 12];
            int i;
            Vec3i vecB;
            for (i = 0; i < d->count; i++) {
                vecB.x = 0;
                vecB.y = 0x1000;
                vecB.z = 0;
                func_0203cc28((int *)&vecB, d->a[i]);
                func_ov006_020cefa4(d->base + i * 0xc, (int *)&vecB, d->b[i], d->c[i]);
            }
            *(int *)(raw + 0x7b98) = 0;
        } else {
            func_ov006_02123428(raw);
        }
    }
    *(int *)(raw + 0x7b84) = 0x78;
}
}

// @symbol func_ov006_02123c78
extern "C" {
void func_ov006_02123c78(char *raw) {
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
    func_ov006_020ca840(raw);
    self->mTimer = 0x5a;
    *(struct P2*)self->mState = data_ov006_0213fbf0;
}
}

// @symbol func_ov006_02123bf4
extern "C" void func_ov006_02123bf4(char *raw)
{
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
    func_ov006_020cedf0();
    func_ov006_020cac30();
    if (ApproachLinear(self->mTimer, 0, 1) == 0)
        return;
    if (func_ov006_020ca7b8() == 0)
        return;
    func_ov004_020b0a54(0x12);
    self->mPromptEnabled = 0;
    self->mTimer = 0xb4;
    *(double *)self->mState = data_ov006_0213fbe0.asDouble;
}

// @symbol func_ov006_02123b24
extern "C" {
void func_ov006_02123b24(char *raw)
{
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)raw;
    int idx, b;
    self->mTimer -= 1;
    if (self->mTimer != 0)
    {
        idx = data_020a0e40[0];
        b = 0;
        if (((unsigned char *)data_020a0de8)[idx * 4] != 0 &&
            ((unsigned char *)data_020a0de9)[idx * 4] != 0)
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
    self->mRoundOver = 1;
    {
        int w0 = data_ov006_0213fbd0[0];
        int w1 = data_ov006_0213fbd0[1];
        w0 = w1 ? w0 : w0;
        self->mState[0] = w0;
        self->mState[1] = w1;
    }
}
}

// @symbol func_ov006_02123b20
extern "C" {
void func_ov006_02123b20(void)
{
}
}

// @symbol func_ov006_02123938
extern "C" {
void func_ov006_02123938(void *arg)
{
    dScMgTrampoline2_c *self = (dScMgTrampoline2_c *)arg;
    int i;
    int b;

    if (self->mInputEnabled == 0 || self->mTouchReleased != 0) {
        self->mTouching = 0;
        return;
    }

    i = data_020a0e40[0];
    b = (data_020a0de8[i].v != 0 && data_020a0de9[i].v != 0);
    if (b) {
        self->mTouchX = data_020a0dea[i].v;
        self->mTouchStartX = self->mTouchX;
        self->mTouchY = data_020a0deb[i].v;
        self->mTouchStartY = self->mTouchY;
        self->mTouching = 1;
        self->mDragSoundHandle = 0;
    }

    if (self->mTouching != 1) return;

    i = data_020a0e40[0];
    if (data_020a0de8[i].v != 0) {
        func_ov004_020ae5c4((int)arg, self->mTouchX, self->mTouchY,
                            data_020a0dea[i].v, data_020a0deb[i].v, 2, 4);
        i = data_020a0e40[0];
        self->mTouchX = data_020a0dea[i].v;
        self->mTouchY = data_020a0deb[i].v;
        self->mDragSoundHandle = func_02012468(self->mDragSoundHandle, 2, 0x1b0, 2, 0,
                                             func_020126e8(self->mTouchX << 12), 0, 0);
    } else {
        b = (int)(data_020a0de8[i].v == 0 && data_020a0de9[i].v != 0);
        if (b != 0) {
            self->mTouchReleased = 1;
        }
    }
}
}

// @symbol func_ov006_021238d0
extern "C" {
int func_ov006_021238d0(int x) {
    switch (x) {
    case 0:
    case 1:
    case 2:
        return 0;
    case 5:
    case 8:
        return 1;
    case 3:
    case 4:
        return 2;
    case 6:
    case 9:
        return 3;
    case 7:
    case 10:
        return 4;
    }
    return 0;
}
}

// @symbol func_ov006_0212382c
extern "C" {
unsigned short func_ov006_0212382c(int sel, int b)
{
    unsigned short r = 0;
    switch (sel) {
    case 0:
        if (b == 0) r |= 4;
        break;
    case 1:
        if (b == 0) r |= 0x60;
        else r |= 2;
        break;
    case 2:
        if (b == 0) r |= 0x48;
        break;
    case 3:
        if (b == 0) r |= 0x100;
        else r |= 0x28;
        break;
    case 4:
        if (b == 1) r |= 0x80;
        break;
    }
    return r;
}
}

// @symbol func_ov006_021237c8
extern "C" {
int func_ov006_021237c8(int x) {
    int r;
    if (x < 5) {
        r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
        return r < 0xc00 ? 4 : 5;
    }
    r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    return r < 0xc00 ? 6 : 7;
}
}

// @symbol func_ov006_0212373c
extern "C" {
int func_ov006_0212373c(int a)
{
    int r;
    if (a < 5) {
        r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
        if (r < 0xc00)
            return 4;
        if (r < 0xe00)
            return 5;
        return 8;
    }
    r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    if (r < 0xc00)
        return 6;
    if (r < 0xe00)
        return 8;
    return 7;
}
}

// @symbol func_ov006_02123428
extern "C" void func_ov006_02123428(char *scene)
{
    int occupancy;
    int column;
    int kind;
    int kinds[7];
    short faces[7];
    Vec3i up;
    int i;
    int j;
    int variant;

    occupancy = 0;
    for (i = 0; i < 5; i++) {
        do {
            kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                           & 0x7fffffff) >> 0x13) * 11) >> 12;
        } while ((kind == 7 || kind == 10)
                 && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                            & 0x7fffffff) >> 0x13) > 0x400);

        for (;;) {
            for (j = 0; j < i; j++) {
                if (kind == kinds[j]) {
                    kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                                   & 0x7fffffff) >> 0x13) * 11) >> 12;
                    j = -1;
                }
            }

            column = func_ov006_021238d0(kind);
            {
                int shift = column * 2;
                int rightBit = 2 << shift;
                int leftBit = 1 << shift;
                int savedOccupancy;

                if ((occupancy & rightBit) != 0 &&
                    (occupancy & leftBit) != 0) {
                    do {
                        if (ApproachLinear(kind, 11, 1) != 0)
                            kind = 0;
                    } while ((kind == 7 || kind == 10)
                             && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                                       & 0x7fffffff) >> 0x13) > 0x800);
                    continue;
                }

                if ((savedOccupancy = (int)(volatile int)occupancy,
                     (savedOccupancy & rightBit) != 0)) {
                    kinds[i] = kind;
                    faces[i] = 8;
                    occupancy |= func_ov006_0212382c(column, 1);
                } else if ((savedOccupancy & leftBit) != 0) {
                    kinds[i] = kind;
                    faces[i] = (short)func_ov006_021237c8(kind);
                    occupancy |= func_ov006_0212382c(column, 0);
                } else {
                    kinds[i] = kind;
                    faces[i] = (short)func_ov006_0212373c(kind);
                    switch (faces[i]) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        occupancy |= func_ov006_0212382c(column, 0);
                        break;
                    default:
                        occupancy |= func_ov006_0212382c(column, 1);
                        break;
                    }
                }
            }
            break;
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (kind >= 5 ||
            ((kind & 1) != 0 && (faces[i] == 8 || faces[i] == 5))) {
            func_0203cc28((int *)&up, 0x4000);
        }

        variant = 1;
        if (faces[i] == 6)
            variant = 2;
        func_ov006_020cefa4((int)&data_ov006_0212f328 + kinds[i] * 0xc,
                            (int *)&up, variant, faces[i]);
    }
}

// @symbol _ZN18dScMgTrampoline2_c8BehaviorEv
/* Steps the score-pop list, the particle, and the current state.
   mRamp is written into the particle's byte at +0x58. */
s32 dScMgTrampoline2_c::Behavior()
{
    char *raw = (char *)this;
    int saved = data_ov006_02140830;

    func_ov006_02120c40();
    func_ov006_020eef90();
    func_ov006_02122ab8();
    mParticleID =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticleID, 0xf0, 0x280000, 0x700000, -0x580000, 0);
    void *p = Particle::System::FromUniqueID(mParticleID);
    if (p != 0) {
        *(char *)((char *)p + 0x58) = (char)(mRamp >> 12);
        ApproachLinear(mRamp, 0x14000, 0x200);
    }
    {
        dScMgTrampoline2_cState *pp = (dScMgTrampoline2_cState *)mState;
        (this->**pp)();
    }
    func_ov006_02123938(this);
    if (saved != data_ov006_02140830) {
        func_ov004_020adb1c(data_ov006_02140830);
    }
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c6RenderEv
/* Vtable slot 9. The two volatile stores set the 3D fog and clear registers
 * (0x040004c8 and 0x040004cc) from a rotated vector. */
s32 dScMgTrampoline2_c::Render()
{
    int m[3];
    int count;
    int a1v;
    int i;

    m[0] = 0;
    m[1] = 0;
    m[2] = 0xfffff008;
    func_0203cd80(m, -0x4000);

    *(volatile int *)0x40004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(volatile int *)0x40004cc = 0x7fff;
    *(volatile int *)0x40004cc = 0x40007fff;

    func_ov006_02120c08();
    func_ov006_020eef58();

    if (unk_4664 == 1) {
        count = data_ov006_0213b0f0;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213fc48[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140830, 6);

    if (mRoundOver == 0) {
        func_ov006_020caadc();
        func_ov006_020d09e0();
        func_ov006_020ced84();
    }
    func_ov006_02122a4c();
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c16CleanupResourcesEv
/* Vtable slot 3: two teardown calls. */
s32 dScMgTrampoline2_c::CleanupResources()
{
    func_ov006_020ceedc();
    func_ov004_020ad90c();
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c8OnKickedEv
/* OnKicked: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnKicked()
{
    if (!dScMgD3DBase_c::OnKicked()) return 0;
    if (mMenuOpen == 0) {
        if (unk_4664 == 0) {
            data_0209d45c = data_0209d45c & ~8;
            SetBg2Offset(0, 0);
            *(int *)0x4000018 = 0;
        } else {
            data_0209d45c = data_0209d45c | 8;
            SetBg2Offset(0, 0xc0);
            *(int *)0x4000018 = 0xc00000;
        }
    }
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c8OnPushedEv
/* OnPushed: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnPushed()
{
    return dScMgD3DBase_c::OnPushed() != 0;
}

// @symbol _ZN18dScMgTrampoline2_c11OnAttacked2Ev
/* OnAttacked2: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnAttacked2()
{
    char *raw = (char *)this;

    u16 buf[5];

    if (!dScMgBase_c::OnAttacked2())
        return 0;

    {
        char* obj = data_0209f5bc;
        int (*vf)(char*) = *(int (**)(char*))(*(char**)obj + 0x14);
        if (vf(obj)) {
            data_ov006_0213fbc4 = 0;
        } else {
            char* obj2 = data_0209f5bc;
            int (*vf2)(char*) = *(int (**)(char*))(*(char**)obj2 + 0x1c);
            if (vf2(obj2))
                data_ov006_0213fbc4 = 1;
        }
    }

    if (data_ov006_0213fbc4 != 0)
        return 0;

    if (mMenuOpen != 0)
        return 0;

    if (mTouchReleased != 0) {
        s16 a = *(volatile s16 *)&mTouchStartX;
        if ((a < 0x18 && *(volatile s16 *)&mTouchX < 0x18) ||
            (a > 0xe8 && *(volatile s16 *)&mTouchX > 0xe8)) {
            func_02012790(0xe);
        } else {
            buf[1] = mTouchStartX;
            buf[2] = mTouchStartY;
            buf[3] = mTouchX;
            buf[4] = mTouchY;
            if (func_ov006_020d0c38(&buf[1], &buf[3])) {
                func_02012718(0x1af, (mTouchStartX + mTouchX) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        {
            char* dst = (char*)func_02054d88();
            buf[0] = 0;
            MultiStore16(buf[0], dst, 0x6000);
        }
        mTouching = 0;
        mTouchReleased = 0;
    }
    return 1;
}

// @symbol func_ov006_02122e20
extern "C" {
void func_ov006_02122e20(void *arg0, int layer)
{
    struct Obj *self = (struct Obj *)arg0;
    volatile unsigned short t0;
    volatile unsigned short t1;
    int v = 0x7000;
    char *scrPtr;
    char *charPtr;
    int i;

    self->brushOn = 1;
    self->bgLayer = layer;

    switch (self->bgLayer) {
    case 0:
        scrPtr = _ZN2G212GetBG0ScrPtrEv();
        charPtr = (char *)func_02054efc();
        break;
    case 1:
        scrPtr = (char *)_ZN2G212GetBG1ScrPtrEv();
        charPtr = (char *)func_02054ea8();
        break;
    case 2:
        scrPtr = _ZN2G212GetBG2ScrPtrEv();
        charPtr = _ZN2G213GetBG2CharPtrEv();
        break;
    case 3:
        scrPtr = _ZN2G212GetBG3ScrPtrEv();
        charPtr = (char *)func_02054d88();
        break;
    default:
        return;
    }

    for (i = 0; i < 0x300; i++) {
        t0 = v;
        MultiStore16(t0, scrPtr, 2);
        v = (unsigned short)(v + 1);
        scrPtr += 2;
    }

    t1 = 0;
    MultiStore16(t1, charPtr, 0x6000);
    GX::LoadBGPltt(data_ov006_0212f1ac, 0xe0, 0x20);
}
}

// @symbol _ZN18dScMgTrampoline2_c9Virtual88Eiiii
/* Vtable slot 34 (name not evidenced): plot an n by n square of 4-bit pixels
 * into the current touch-screen layer. The same body as dScMgTrampoline_c's,
 * with its own scratch buffer. */
void dScMgTrampoline2_c::Virtual88(int x_base, int y, int val, int n)
{
    void *obj = (void *)this;

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
                        int mode = ((Obj *)obj)->bgLayer;

                        switch (mode)
                        {
                            case 0:
                                ret = func_02054efc();
                                break;
                            case 1:
                                ret = func_02054ea8();
                                break;
                            case 2:
                                ret = _ZN2G213GetBG2CharPtrEv();
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

                        MultiCopy_Int(addr, data_ov006_02142f78, 4);
                        data_ov006_02142f78[0] =
                            (data_ov006_02142f78[0] &
                             (-1 ^ (mask << ((x & 7) * 4)))) |
                            (val << ((x & 7) * 4));
                        MultiCopy_Int(data_ov006_02142f78, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

// @symbol func_ov006_02122c90
extern "C" {
int func_ov006_02122c90(char *raw)
{
    _ZN5ModelC1Ev(raw);
    *(short *)(raw + 0x76) = 0;
    return (int)raw;
}
}

// @symbol func_ov006_02122c68
extern "C" Model* func_ov006_02122c68(Model* m){
  data_ov006_02142f80.Release();
  _ZN5ModelD1Ev(m);
  return m;
}

// @symbol func_ov006_02122c04
extern "C" {
void func_ov006_02122c04(int x, int count) {
    int i;
    data_ov006_02142f7c = (unsigned char *)x;
    data_ov006_02142f74 = count;
    for (i = 0; i < data_ov006_02142f74; i++) {
        func_ov006_02122a1c((void*)(data_ov006_02142f7c + i * 0x78));
    }
}
}

// @symbol func_ov006_02122b88
extern "C" {
void func_ov006_02122b88(unsigned char *c)
{
  int i = 0;
  int n = data_ov006_02142f74;
  unsigned char *base;
  unsigned char *p;
  if (n <= 0)
  {
    return;
  }
  base = data_ov006_02142f7c;
  p = base;
  do
  {
    if ((*((short *) (p + 0x76))) == 0)
    {
      func_ov006_021228bc(base + (i * 0x78), c);
      return;
    }
    i++;
    p += 0x78;
  }
  while (i < data_ov006_02142f74);
}
}

// @symbol func_ov006_02122b24
extern "C" {
void func_ov006_02122b24(void){
  int i = 0;
  if(data_ov006_02142f74 > 0){
    int off = 0;
    do {
      *(short*)(data_ov006_02142f7c + off + 0x76) = 0;
      i++;
      off += 0x78;
    } while(i < data_ov006_02142f74);
  }
}
}

// @symbol func_ov006_02122ab8
extern "C" {
void func_ov006_02122ab8(void)
{
    int i;
    for (i = 0; i < data_ov006_02142f74; i++) {
        char *obj = (char *)data_ov006_02142f7c + i * 0x78;
        if (*(short *)(obj + 0x76) != 0) {
            func_ov006_0212287c(obj);
        }
    }
}
}

// @symbol func_ov006_02122a4c
extern "C" {
void func_ov006_02122a4c(void)
{
    int i;
    for (i = 0; i < data_ov006_02142f74; i++) {
        char *obj = (char *)data_ov006_02142f7c + i * 0x78;
        if (*(short *)(obj + 0x76) != 0) {
            func_ov006_02122814(obj);
        }
    }
}
}

// @symbol func_ov006_02122a1c
extern "C" void func_ov006_02122a1c(void* raw)
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov006_02142f80);
    ((ModelBase *)raw)->SetFile(file, 1, -1);
}

extern "C" {
extern struct Tbl data_ov006_0213fbf8;
extern unsigned short data_ov006_0212f144[];
extern int RandomIntInternal(int* seed);
extern void func_020169f4(char* self, int kind);

// @symbol func_ov006_021228bc
void func_ov006_021228bc(void *r0, void *r1) {
    char* raw = (char *)r0;
    int value = (int)r1;
    struct Tbl t = data_ov006_0213fbf8;
    unsigned int rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
    int idx;
    if ((int)rnd < t.a) {
        idx = 0;
    } else if ((int)rnd < t.b) {
        idx = 1;
    } else if ((int)rnd < t.c) {
        idx = 2;
    } else {
        idx = 3;
    }
    func_020169f4(raw, data_ov006_0212f144[idx]);
    int scale = value * 0xA00;
    *(short*)(raw + 0x76) = 0xF0;
    if (scale > 0x4000) {
        scale = 0x4000;
    }
    int size = scale + 0x4000;
    *(int*)(raw + 0x50) = size;
    *(int*)(raw + 0x54) = size;
    *(int*)(raw + 0x58) = size;
    *(int*)(raw + 0x5c) = 0x60000;
    *(int*)(raw + 0x60) = 0x100000;
    *(int*)(raw + 0x64) = 0x18000;
    int drop = -0x1C00;
    *(int*)(raw + 0x68) = drop;
    *(int*)(raw + 0x6c) = drop;
    *(int*)(raw + 0x70) = 0;
    unsigned int rnd2 = (unsigned int)RandomIntInternal(&data_0209e650);
    int* p1 = (int*)(raw + 0x5c);
    int v1 = *p1 + ((((rnd2 & 0x7fffffff) >> 0x13) - 0x800) * 0x40);
    *p1 = v1;
    int rnd3 = RandomIntInternal(&data_0209e650);
    int* p2 = (int*)(raw + 0x68);
    int adj3 = (int)(((unsigned int)rnd3 & 0x7fffffff) >> 0x13);
    *p2 -= adj3 >> 1;
    int rnd4 = RandomIntInternal(&data_0209e650);
    int* p3 = (int*)(raw + 0x6c);
    int adj4 = (int)(((unsigned int)rnd4 & 0x7fffffff) >> 0x13);
    *p3 -= adj4 >> 1;
    func_02016a14(raw, 0);
    func_02016a04(raw, 0);
}
}

// @symbol func_ov006_0212287c
extern "C" {
void func_ov006_0212287c(char* raw){
  ApproachLinear2(*(short *)(raw + 0x76), 0, 1);
  short* p = (short*)((int)raw + 0x74);
  *p = *p + 0x400;
  AddVec3((struct Vector3*)(raw+0x5c), (struct Vector3*)(raw+0x68), (struct Vector3*)(raw+0x5c));
}
}

// @symbol func_ov006_02122814
extern "C" void func_ov006_02122814(char *raw) {
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(raw + 0x5c), *(int *)(raw + 0x60), *(int *)(raw + 0x64));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(raw + 0x74));
    *(Matrix4x3 *)(raw + 0x1c) = data_020a0e68;
    ((C *)raw)->v5(raw + 0x50);
}

extern "C" {
/* local extern: no header declares it (the include/ hits are comments) */
extern void __cxa_vec_cleanup(void* a, int b, int c, void* d);
extern void _ZN18TextureTransformerD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
// @symbol func_ov006_021227c8
int func_ov006_021227c8(char* raw){
  __cxa_vec_cleanup(raw+0x1a8, 5, 8, (void*)NullDestructor_0203d47c);
  _ZN18TextureTransformerD1Ev(raw+0x194);
  _ZN5ModelD1Ev(raw+0x140);
  _ZN5ModelD1Ev(raw+0xf0);
  _ZN5ModelD1Ev(raw+0xa0);
  return (int)raw;
}
}

/* D1 and D0 come from the inline destructor in dScMgTrampoline2_c.h. */
