//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_trampoline2_AssetId {
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCG_BIN = 0x00a2,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCL_BIN = 0x00a3,
    ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NSC_BIN = 0x00a4,
    ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN = 0x0108,
    ASSET_MINI_STAR_BMD = 0x0219
};

#pragma opt_loop_invariants off
/* dScMgTrampoline2_c -- Trampoline Terror. Draw lines on the touch screen
 * so Mario can bounce through the shapes that drop in.
 *
 * Functions are in reverse ROM order; do not reorder.
 * opt_loop_invariants is off for the whole file. TUBUILD CONFLICT lines
 * stay; the manifest pairs them.
 *
 * leftovers:
 * - dScMgTrampoline2_c::StatePlay keeps raw int offsets for mTimer,
 *   mPattern, mScoreGate, mScriptDone and mWaveStep: member form DIFFs
 *   there (code shifts; the function grows). Its mRamp access matches.
 * - 0x7acc has no matched read. The factory still builds the arrays by
 *   offset.
 * - Model / TextureTransformer / SysTracker C1/D1 stay mangled (built on
 *   raw storage); NewUnkCallback818 takes Fix12 by value, which costs the
 *   caller bytes (notes/mwccarm-codegen.md 6az); the G2 BG getters stay
 *   mangled because decl_common.h's global G2 hides the namespace;
 *   SetBlendAlpha keeps a local declaration.
 * - The 0x78-element pool helpers stay free; their element type is not
 *   recovered.
 */

#include "common.h"
#include "types.h"
#include "dScMgTrampoline2_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#define Trmp2_DestructJumperModels func_ov006_021227c8_decl_common_shadow
#define Trmp2Spinner_Destruct func_ov006_02122c68_decl_common_shadow
#define _ZTV18dScMgTrampoline2_c _ZTV18dScMgTrampoline2_c_decl_common_shadow
#include "decl_common.h"
#include "PlayerInput.h"
/* local: the ROM re-reads the slot byte at each use here, so keep the read volatile. */
#define ACTIVE_SLOT (*(volatile unsigned char *)&gActivePlayerSlot)
/* local: these readers only reproduce through a 4-byte-record view of the lane. */
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))
#undef Trmp2_DestructJumperModels
#undef Trmp2Spinner_Destruct
#undef _ZTV18dScMgTrampoline2_c

extern int _ZTV18dScMgTrampoline2_c[];

/* C, Obj, P2, S2, T, SpawnDef and the forward declarations below are
 * local stand-ins for types that have no header yet (issue #2497). */
struct C {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(void *arg);
};

/* SharedFilePtr has no fields; the two words are the handle's 8 bytes. The
 * cartridge's ctor/dtor imports are the functions the wrapper's aliases name. */
struct Trampoline2ModelFilePtr : SharedFilePtr {
    u32 words[2];

    Trampoline2ModelFilePtr(u32 fileID);
    ~Trampoline2ModelFilePtr();
};

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

struct Vec3i {
    int x;
    int y;
    int z;
};

struct P2 { int words[2]; };

union StateValue {
    int words[2];
    double asDouble;
};

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

/* TUBUILD CONFLICT -- alternate body of struct 'Model', from the legacy file for Trmp2Spinner_Destruct, NOT applied:
struct Model { ~Model(); };
*/

extern "C" {
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
extern Matrix4x3 data_020a0e68;
extern "C" void Trmp2Spinner_UpdateMatrix(char *c);
extern void AddVec3(struct Vector3* a, struct Vector3* b, struct Vector3* c);
extern "C" Trampoline2ModelFilePtr sTrmp2StarModelFile;
extern int sTrmp2SpinnerCount;
extern unsigned char *sTrmp2Spinners;
extern void Trmp2Spinner_Update(char *obj);
extern void Trmp2Spinner_Init(void *obj, void *kind);
extern void Trmp2Spinner_SetModel(void* self);
extern void *_ZN5ModelC1Ev(void *model);
extern void _ZN5ModelD1Ev(void *model);
extern int sTrmp2BrushWord[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern char *_ZN2G212GetBG0ScrPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern char *_ZN2G212GetBG3ScrPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern const unsigned char sTrmp2BrushPltt[];
extern char* data_0209f5bc;
extern int sTrmp2TouchBlocked;
unsigned int func_02012790(unsigned int a);
int TrmpLine_Create(u16* a, u16* b);
extern "C" void Mg_CommitRecords(void);
void func_0203cd80(int *m, short angle);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void Jump2Popup_UpdateAll(void);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int sTrmp2ItemKinds;
extern int Trmp2_ColumnForKind(int kind);
extern unsigned short Trmp2_OccupancyBits(int column, int side);
extern int Trmp2_PickFaceA(int kind);
extern int Trmp2_PickFaceB(int kind);
extern void func_0203cc28(int *vector, int angle);
extern char *TrmpItem_Spawn(int resource, int *up, int variant, int face);
extern int Mg_DrawLine(int a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int pmf_dScMgTrampoline2_c_StateDone[];
extern void TrmpItem_UpdateAll(void);
extern int Trmp2Mario_Hide(void);
extern union StateValue pmf_dScMgTrampoline2_c_StateWaitExit;
extern void Trmp2Mario_EnterAll(void *c);
extern struct P2 pmf_dScMgTrampoline2_c_StateResults;
extern int sTrmpItemsHit;
extern int sTrmp2Lives;
extern int sTrmp2Score;
extern int sTrmp2MarioCount;
extern int sTrmpItemsActive;
extern int sTrmp2ScoreGates[];
extern SpawnDef sTrmp2SpawnDefs[];
extern void TrmpLine_UpdateAll(void);
extern void Trmp2Mario_UpdateAll(void);
extern void Trmp2Mario_SetTuning(int a);
extern void Trmp2Mario_MoveToStart(void);
extern void Trmp2Mario_PlaceAtStart(void* a);
extern struct S2 pmf_dScMgTrampoline2_c_StatePlay;
extern void TrmpLine_CreateStartLineT2(void);
extern s16 sTrmp2IntroFromX;
extern s16 sTrmp2IntroToX;
extern s16 sTrmpStartTouchY;
extern short sTrmpStartTouchX;
extern struct P2 pmf_dScMgTrampoline2_c_StateIntro;
extern "C" void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern "C" void TrmpLine_ClearAll(u32 a);
extern volatile s16 data_020a0dbc[];
extern void Camera_UpdateMatrices(void *cam);
extern u8 data_0209d45c;
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *ptr);
extern int sTrmp2NcgByLanguage[];
extern int sTrmpPuffEnglish[];
extern int sTrmpPuffSpanish[];
extern int sTrmpPuffFrench[];
extern int sTrmpPuffGerman[];
extern int sTrmpPuffItalian[];
extern int sMgTextSpritesEnglish[];
extern int sMgTextSpritesSpanish[];
extern int sMgTextSpritesFrench[];
extern int sMgTextSpritesGerman[];
extern int sMgTextSpritesItalian[];
extern void *_ZN11dScMgBase_cC2Ev(void *scene);
extern void _ZN8Particle10SysTrackerC1Ev(void *tracker);
extern void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
extern int _ZTV14dScMgD3DBase_c[];
extern void Trmp2Mario_Construct(void);
extern void TrmpLine_Reset(void);
extern void Jump2Popup_ElemCtor(void *object);
extern void Trmp_Nothing2(void);
extern void *_ZN18TextureTransformerC1Ev(void *transformer);
/* local extern: only sibling minigame headers (dScMgAmida_c.h and four
   others) declare it; this TU includes none of them */
extern void NullDestructor_0203d47c(void);
extern void func_0203d738(void);
extern int Trmp2Spinner_Construct(char *object);
/* TUBUILD CONFLICT -- alternate declaration of Trmp2Spinner_UpdateMatrix, from the legacy file for Trmp2Spinner_UpdateMatricesAll, NOT applied: extern void Trmp2Spinner_UpdateMatrix(char *obj); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_HideAll, NOT applied: extern int sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_InitAll, NOT applied: extern unsigned char *sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2Spinners, from the legacy file for Trmp2Spinner_LoadAll, NOT applied: extern int sTrmp2Spinners; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmp2StarModelFile, from the legacy file for Trmp2Spinner_Destruct, NOT applied: extern SharedFilePtr sTrmp2StarModelFile; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_02122e20, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054efc, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054efc(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054ea8, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054ea8(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02122e20, NOT applied: extern char *func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for _ZN18dScMgTrampoline2_c16OnGraphCallback0Ev, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN18dScMgTrampoline2_c16OnGraphCallback0Ev, NOT applied: void MultiStore16(u16 val, char* dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for Trmp2_PickFaceA, NOT applied: extern int RandomIntInternal(void* seed); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02123b24, NOT applied: extern void MultiStore16(unsigned short a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123bf4, NOT applied: extern int _Z14ApproachLinearRiii(int &v, int a, int b); */
/* Both callers use the existing ov006 helpers' void(void) contracts. The
 * unrelated Player::St_Null_Init at the same RAM address belongs to ov002. */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_02123cb4, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203cc28, from the legacy file for func_ov006_02123cb4, NOT applied: extern void func_0203cc28(int *p, int angle); */
/* TUBUILD CONFLICT -- alternate declaration of TrmpItem_Spawn, from the legacy file for func_ov006_02123cb4, NOT applied: extern char *TrmpItem_Spawn(int a0, int *a1, int a2, int a3); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02124088, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02124088, NOT applied: extern void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02124088, NOT applied: extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h); */
/* TUBUILD CONFLICT -- alternate declaration of Mg_DrawLine, from the legacy file for func_ov006_02124088, NOT applied: extern int Mg_DrawLine(void *a, int b, int c, int d, int e, int f, int g); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchY, from the legacy file for func_ov006_02124228, NOT applied: extern short sTrmpStartTouchY; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c11CanEndStateEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN18dScMgTrampoline2_c10StartRoundEi, NOT applied: extern "C" s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN18dScMgTrampoline2_c10StartRoundEi, NOT applied: extern "C" void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd4fdivEii, from the legacy file for _ZN18dScMgTrampoline2_c13InitResourcesEv, NOT applied: extern s32 _ZN4cstd4fdivEii(s32 a, s32 b); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_021245a8, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_021245a8, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_021245a8, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_021245a8, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */

dScMgTrampoline2_c *dScMgTrampoline2_c_classInit(void);
/* Literal aliases used only to make the five ROM PMF relocations static data.
   Their definitions below are real compiler-spelled C++ members. Each is a
   local extern: a pointer to member cannot convert to the int word a record
   entry holds, so dScMgTrampoline2_c.h's declarations cannot fill the
   table. */
extern "C" void _ZN18dScMgTrampoline2_c9StateDoneEv(void);      /* local extern: PMF -> int */
extern "C" void _ZN18dScMgTrampoline2_c13StateWaitExitEv(void); /* local extern: PMF -> int */
extern "C" void _ZN18dScMgTrampoline2_c12StateResultsEv(void);  /* local extern: PMF -> int */
extern "C" void _ZN18dScMgTrampoline2_c9StatePlayEv(void);      /* local extern: PMF -> int */
extern "C" void _ZN18dScMgTrampoline2_c10StateIntroEv(void);    /* local extern: PMF -> int */

/* Data owned by this production TU; the original TU grouping is inferred. */
int sTrmp2TouchBlocked = 1;
BaseSpawnInfo<dScMgTrampoline2_c> g_profile_MG_TRAMPOLINE2 = {
    dScMgTrampoline2_c_classInit,
    0x181,
    0x181,
};
int pmf_dScMgTrampoline2_c_StateDone[] = { (int)_ZN18dScMgTrampoline2_c9StateDoneEv, 0 };
P2 pmf_dScMgTrampoline2_c_StateIntro = {{ (int)_ZN18dScMgTrampoline2_c10StateIntroEv, 0 }};
StateValue pmf_dScMgTrampoline2_c_StateWaitExit = {{ (int)_ZN18dScMgTrampoline2_c13StateWaitExitEv, 0 }};
S2 pmf_dScMgTrampoline2_c_StatePlay = {{ (int)_ZN18dScMgTrampoline2_c9StatePlayEv, 0 }};
P2 pmf_dScMgTrampoline2_c_StateResults = {{ (int)_ZN18dScMgTrampoline2_c12StateResultsEv, 0 }};
Tbl sTrmp2SpinnerSpeeds = {0x600, 0xc00, 0xe00};
int sTrmp2ScoreGates[] = {1000, 5000, 10000, 20000};
int sTrmp2PuffXByLanguage[] = {(int)sTrmpPuffEnglish, (int)sTrmpPuffSpanish, (int)sTrmpPuffFrench,
                               (int)sTrmpPuffGerman, (int)sTrmpPuffItalian};
int sTrmp2NcgByLanguage[] = {0x107, 0x109, 0x104, 0x105, 0x106};
int *sTrmp2TextSpritesByLanguage[] = {sMgTextSpritesEnglish, sMgTextSpritesSpanish,
                                      sMgTextSpritesFrench, sMgTextSpritesGerman,
                                      sMgTextSpritesItalian};
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

// @symbol TrmpItem_Construct
extern "C" void *TrmpItem_Construct(char *raw)
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
extern "C" dScMgTrampoline2_c *dScMgTrampoline2_c_classInit(void) {
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
        __cxa_vec_ctor(scene + 0x500c, 5, 0xdc, (void *)Trmp2Mario_Construct,
                       (void *)Trmp2Mario_Destruct);
        __cxa_vec_ctor(scene + 0x5458, 3, 0x32c,
                      (void *)TrmpLine_Reset,
                      (void *)TrmpLine_Destroy);
        __cxa_vec_ctor(scene + 0x5ddc, 0xa, 0x1d0,
                      (void *)TrmpItem_Construct,
                      (void *)Trmp2_DestructJumperModels);
        __cxa_vec_ctor(scene + 0x6ffc, 0xa, 0x24,
                      (void *)Jump2Popup_ElemCtor,
                      (void *)Jump2Popup_ElemDtor);
        __cxa_vec_ctor(scene + 0x7164, 0x14, 0x78,
                      (void *)Trmp2Spinner_Construct,
                      (void *)Trmp2Spinner_Destruct);
        __cxa_vec_ctor(scene + 0x7ad0, 5, 0x24,
                      (void *)Trmp_Nothing2,
                      (void *)Trmp_Nothing);
    }
    return (dScMgTrampoline2_c *)scene;
}

// @symbol _ZN18dScMgTrampoline2_c11InitDisplayEv
void dScMgTrampoline2_c::InitDisplay()
{
    int objChar, objPltt, bgChar, bgScr, bgPltt;
    volatile u16 sp0;

    objChar = LoadFile(sTrmp2NcgByLanguage[GetGameLanguage()]);
    objPltt = LoadFile(ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN);
    bgChar = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCG_BIN);
    bgScr = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NSC_BIN);
    bgPltt = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_HARD_NCL_BIN);

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

    InitBrush(3);
}

// @symbol _ZN18dScMgTrampoline2_c13InitResourcesEv
/* Vtable slot 0 (the name is inferred): load the screens, set up both
 * cameras and the object pools, then start a round through StartRound. */
s32 dScMgTrampoline2_c::InitResources()
{
    char *raw = (char *)this;
    s32 fov;

    InitDisplay();
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
    if (Trmp2Mario_InitPool(raw + 0x500c, 5) == 0)
        return 0;
    TrmpFx_InitPool(raw + 0x7ad0, 5);
    TrmpLine_LoadTextures();
    TrmpItem_LoadAll((char *)(raw + 0x5ddc), 10);
    Jump2Popup_InitPool((int)(raw + 0x6ffc), 10);
    data_ov006_021421b4 = 0;
    Trmp2Spinner_LoadAll((int)(raw + 0x7164), 0x14);
    Mg_SetTopScreenYOffset(0x10);
    StartRound(-1);
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c10StartRoundEi
/* Vtable slot 18: reset the round. */
void dScMgTrampoline2_c::StartRound(int /* arg */)
{
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4000050, 1, 0x2e, 0x10, 0x10);

    mTouchStartX = data_020a0dbc[0];
    mTouchStartY = data_020a0dbc[1];
    mTouchX = data_020a0dbc[0];
    mTouchY = data_020a0dbc[1];

    mLevel = 0;
    if (mLevel > 0x270e)
        mLevel = 0x270e;

    Trmp2Mario_SetTuning(0x1000);
    Trmp2Mario_ResetAll(mLevel);
    sTrmp2MarioCount = 1;
    TrmpLine_ClearAll(mLevel);
    TrmpItem_HideAll(mLevel);
    Jump2Popup_ClearAll();
    Trmp2Spinner_HideAll();
    TrmpFx_ClearAll();
    Puff_ClearAll(0, sTrmp2PuffXByLanguage[GetGameLanguage()]);

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

    BeginIntro();
}

// @symbol _ZN18dScMgTrampoline2_c11CanEndStateEi
/* CanEndState: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::CanEndState(int /* mode */)
{
    Puff_ClearAll(0, sTrmp2PuffXByLanguage[GetGameLanguage()]);
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c10BeginIntroEv
void dScMgTrampoline2_c::BeginIntro() {
    short a, b;
    mTimer = 0x5a;
    a = sTrmpStartTouchX;
    mTouchStartX = a;
    b = sTrmpStartTouchY;
    mTouchStartY = b;
    mTouchX = a;
    mTouchY = b;
    mDragSoundHandle = 0;
    mScoreGate = 0x3e8;
    *(struct P2*)mState = pmf_dScMgTrampoline2_c_StateIntro;
}

// @symbol _ZN18dScMgTrampoline2_c10StateIntroEv
void dScMgTrampoline2_c::StateIntro()
{
    int counter;

    TrmpLine_UpdateAll();
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

        TrmpLine_CreateStartLineT2();
        func_02012718(0x1af, 0x8000);
        BeginPlay();
        return;
    }

    {
        int t = cstd::fdiv(counter << 12, 0x5a000);
        volatile s16 old9c = mTouchX;
        volatile s16 old9e = mTouchY;
        int mixRaw = sTrmp2IntroToX * t + sTrmp2IntroFromX * (0x1000 - t);
        mTouchX = (s16)(mixRaw >> 12);
        mTouchY = sTrmpStartTouchY;

        mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) - 0x800) << 2) >> 12);

        Mg_DrawLine((int)this, old9c, old9e, mTouchX, mTouchY, 2, 0xc);

        mDragSoundHandle = func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0,
                                              func_020126e8(mixRaw), 0, 0);
    }
}

// @symbol _ZN18dScMgTrampoline2_c9BeginPlayEv
void dScMgTrampoline2_c::BeginPlay(){
  mTimer = 0xb4;
  mInputEnabled = 1;
  Trmp2Mario_PlaceAtStart((char *)this + 0x7b00);
  *(struct S2*)mState = pmf_dScMgTrampoline2_c_StatePlay;
}

// @symbol _ZN18dScMgTrampoline2_c9StatePlayEv
void dScMgTrampoline2_c::StatePlay()
{
    char *raw = (char *)this;
    int old;

    old = sTrmpItemsHit;
    TrmpLine_UpdateAll();
    TrmpItem_UpdateAll();
    Trmp2Mario_UpdateAll();
    if (old < sTrmpItemsHit) {
        int n = sTrmpItemsHit - old;
        int i;
        for (i = 0; i < n; i++)
            ApproachLinear(mRamp, 0x7000, -0x800);
        if (sTrmpItemsHit == 3)
            mPromptEnabled = 0;
    }
    Trmp2Mario_SetTuning((sTrmpItemsHit / 10) * 64 + 0x1000);
    if (sTrmp2Lives == 0) {
        mInputEnabled = 0;
        BeginResults();
        return;
    }
    if (sTrmp2Score >= mScoreGate && sTrmp2MarioCount < 5) {
        unsigned int v;
        Trmp2Mario_MoveToStart();
        {
            int *p = (int *)((int)raw + 0xbc);
            *p = *p + 1;
        }
        if (mLevel > 0x270e)
            mLevel = 0x270e;
        v = mLevel;
        if (v < 4)
            mScoreGate = sTrmp2ScoreGates[v];
        else
            mScoreGate = 0x7fffffff;
        return;
    }
    if (sTrmpItemsActive != 0)
        return;
    /* Member form DIFFs here (bytes, not relocations): these are mTimer,
       mScoreGate, mPattern, mScriptDone and mWaveStep. */
    if (ApproachLinear(mTimer, 0, 1) == 0)
        return;
    if (mScriptDone == 0) {
        SpawnDef *d = &sTrmp2SpawnDefs[mPattern];
        int i;
        Vec3i vecA;
        for (i = 0; i < d->count; i++) {
            vecA.x = 0;
            vecA.y = 0x1000;
            vecA.z = 0;
            func_0203cc28((int *)&vecA, d->a[i]);
            TrmpItem_Spawn(d->base + i * 0xc, (int *)&vecA, d->b[i], d->c[i]);
        }
        if (ApproachLinear(mPattern, 6, 1)) {
            mScriptDone = 1;
            mWaveStep = 0;
        }
    } else {
        if (ApproachLinear(mWaveStep, 4, 1)) {
            unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff;
            SpawnDef *d = &sTrmp2SpawnDefs[(int)((rnd >> 19) * 6) >> 12];
            int i;
            Vec3i vecB;
            for (i = 0; i < d->count; i++) {
                vecB.x = 0;
                vecB.y = 0x1000;
                vecB.z = 0;
                func_0203cc28((int *)&vecB, d->a[i]);
                TrmpItem_Spawn(d->base + i * 0xc, (int *)&vecB, d->b[i], d->c[i]);
            }
            mWaveStep = 0;
        } else {
            SpawnWave();
        }
    }
    mTimer = 0x78;
}

// @symbol _ZN18dScMgTrampoline2_c12BeginResultsEv
void dScMgTrampoline2_c::BeginResults() {
    Trmp2Mario_EnterAll((char *)this);
    mTimer = 0x5a;
    *(struct P2*)mState = pmf_dScMgTrampoline2_c_StateResults;
}

// @symbol _ZN18dScMgTrampoline2_c12StateResultsEv
void dScMgTrampoline2_c::StateResults()
{
    TrmpItem_UpdateAll();
    Trmp2Mario_UpdateAll();
    if (ApproachLinear(mTimer, 0, 1) == 0)
        return;
    if (Trmp2Mario_Hide() == 0)
        return;
    Mg_SetSharedState(MG_STATE_GAME_OVER);
    mPromptEnabled = 0;
    mTimer = 0xb4;
    *(double *)mState = pmf_dScMgTrampoline2_c_StateWaitExit.asDouble;
}

// @symbol _ZN18dScMgTrampoline2_c13StateWaitExitEv
void dScMgTrampoline2_c::StateWaitExit()
{
    int idx, b;
    mTimer -= 1;
    if (mTimer != 0)
    {
        idx = ACTIVE_SLOT;
        b = 0;
        if (gTouchHeld[idx * 4] != 0 &&
            gTouchEdge[idx * 4] != 0)
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
        int w0 = pmf_dScMgTrampoline2_c_StateDone[0];
        int w1 = pmf_dScMgTrampoline2_c_StateDone[1];
        w0 = w1 ? w0 : w0;
        mState[0] = w0;
        mState[1] = w1;
    }
}

// @symbol _ZN18dScMgTrampoline2_c9StateDoneEv
void dScMgTrampoline2_c::StateDone()
{
}

// @symbol _ZN18dScMgTrampoline2_c16UpdateTouchInputEv
void dScMgTrampoline2_c::UpdateTouchInput()
{
    int i;
    int b;

    if (mInputEnabled == 0 || mTouchReleased != 0) {
        mTouching = 0;
        return;
    }

    i = ACTIVE_SLOT;
    b = (TOUCH_REC(gTouchHeld)[i][0] != 0 && TOUCH_REC(gTouchEdge)[i][0] != 0);
    if (b) {
        mTouchX = TOUCH_REC(gTouchX)[i][0];
        mTouchStartX = mTouchX;
        mTouchY = TOUCH_REC(gTouchY)[i][0];
        mTouchStartY = mTouchY;
        mTouching = 1;
        mDragSoundHandle = 0;
    }

    if (mTouching != 1) return;

    i = ACTIVE_SLOT;
    if (TOUCH_REC(gTouchHeld)[i][0] != 0) {
        Mg_DrawLine((int)this, mTouchX, mTouchY,
                            TOUCH_REC(gTouchX)[i][0], TOUCH_REC(gTouchY)[i][0], 2, 4);
        i = ACTIVE_SLOT;
        mTouchX = TOUCH_REC(gTouchX)[i][0];
        mTouchY = TOUCH_REC(gTouchY)[i][0];
        mDragSoundHandle = func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0,
                                             func_020126e8(mTouchX << 12), 0, 0);
    } else {
        b = (int)(TOUCH_REC(gTouchHeld)[i][0] == 0 && TOUCH_REC(gTouchEdge)[i][0] != 0);
        if (b != 0) {
            mTouchReleased = 1;
        }
    }
}

// @symbol Trmp2_ColumnForKind
extern "C" {
int Trmp2_ColumnForKind(int x) {
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

// @symbol Trmp2_OccupancyBits
extern "C" {
unsigned short Trmp2_OccupancyBits(int sel, int b)
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

// @symbol Trmp2_PickFaceA
extern "C" {
int Trmp2_PickFaceA(int x) {
    int r;
    if (x < 5) {
        r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
        return r < 0xc00 ? 4 : 5;
    }
    r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    return r < 0xc00 ? 6 : 7;
}
}

// @symbol Trmp2_PickFaceB
extern "C" {
int Trmp2_PickFaceB(int a)
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

// @symbol _ZN18dScMgTrampoline2_c9SpawnWaveEv
void dScMgTrampoline2_c::SpawnWave()
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

            column = Trmp2_ColumnForKind(kind);
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
                    occupancy |= Trmp2_OccupancyBits(column, 1);
                } else if ((savedOccupancy & leftBit) != 0) {
                    kinds[i] = kind;
                    faces[i] = (short)Trmp2_PickFaceA(kind);
                    occupancy |= Trmp2_OccupancyBits(column, 0);
                } else {
                    kinds[i] = kind;
                    faces[i] = (short)Trmp2_PickFaceB(kind);
                    switch (faces[i]) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        occupancy |= Trmp2_OccupancyBits(column, 0);
                        break;
                    default:
                        occupancy |= Trmp2_OccupancyBits(column, 1);
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
        TrmpItem_Spawn((int)&sTrmp2ItemKinds + kinds[i] * 0xc, (int *)&up, variant, faces[i]);
    }
}

// @symbol _ZN18dScMgTrampoline2_c8BehaviorEv
/* Steps the score-pop list, the particle, and the current state.
   mRamp is written into the particle's byte at +0x58. */
s32 dScMgTrampoline2_c::Behavior()
{
    int saved = sTrmp2Score;

    TrmpFx_UpdateAll();
    Jump2Popup_UpdateAll();
    Trmp2Spinner_UpdateAll();
    mParticleID =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticleID, 0xf0, 0x280000, 0x700000, -0x580000, 0);
    void *p = Particle::System::FromUniqueID(mParticleID);
    if (p != 0) {
        *(char *)((char *)p + 0x58) = (char)(mRamp >> 12);
        ApproachLinear(mRamp, 0x14000, 0x200);
    }
    {
        State *pp = (State *)mState;
        (this->**pp)();
    }
    UpdateTouchInput();
    if (saved != sTrmp2Score) {
        Mg_SubmitScore(sTrmp2Score);
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

    TrmpFx_DrawAll();
    Jump2Popup_DrawAll();

    if (mDrawSubScreen == 1) {
        count = sTrmp2Lives;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)sTrmp2TextSpritesByLanguage[idx][1], (void *)a1v, 0xc,
                              (void *)0);
            } else {
                RenderOamMainScreen(sTrmpLifeIconSprite, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    Mg_DrawTopRightCounter(sTrmp2Score, 6);

    if (mRoundOver == 0) {
        Trmp2Mario_DrawAll();
        TrmpLine_DrawAll();
        TrmpItem_UpdateMatricesAll();
    }
    Trmp2Spinner_UpdateMatricesAll();
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c16CleanupResourcesEv
/* Vtable slot 3: two teardown calls. */
s32 dScMgTrampoline2_c::CleanupResources()
{
    TrmpItem_ReleaseFiles();
    Mg_CommitRecords();
    return 1;
}

// @symbol _ZN18dScMgTrampoline2_c16OnGraphCallback2Ev
/* OnGraphCallback2: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback2()
{
    if (!dScMgD3DBase_c::OnGraphCallback2()) return 0;
    if (mMenuOpen == 0) {
        if (mDrawSubScreen == 0) {
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

// @symbol _ZN18dScMgTrampoline2_c16OnGraphCallback3Ev
/* OnGraphCallback3: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback3()
{
    return dScMgD3DBase_c::OnGraphCallback3() != 0;
}

// @symbol _ZN18dScMgTrampoline2_c16OnGraphCallback0Ev
/* OnGraphCallback0: the vtable slot is this class's own; the name is inferred. */
int dScMgTrampoline2_c::OnGraphCallback0()
{

    u16 buf[5];

    if (!dScMgBase_c::OnGraphCallback0())
        return 0;

    {
        char* obj = data_0209f5bc;
        int (*vf)(char*) = *(int (**)(char*))(*(char**)obj + 0x14);
        if (vf(obj)) {
            sTrmp2TouchBlocked = 0;
        } else {
            char* obj2 = data_0209f5bc;
            int (*vf2)(char*) = *(int (**)(char*))(*(char**)obj2 + 0x1c);
            if (vf2(obj2))
                sTrmp2TouchBlocked = 1;
        }
    }

    if (sTrmp2TouchBlocked != 0)
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
            if (TrmpLine_Create(&buf[1], &buf[3])) {
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

// @symbol _ZN18dScMgTrampoline2_c9InitBrushEi
void dScMgTrampoline2_c::InitBrush(int layer)
{
    struct Obj *self = (struct Obj *)this;
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
    GX::LoadBGPltt(sTrmp2BrushPltt, 0xe0, 0x20);
}

// @symbol _ZN18dScMgTrampoline2_c11PaintSquareEiiii
/* Vtable slot 34 (name not evidenced): plot an n by n square of 4-bit pixels
 * into the current touch-screen layer. The same body as dScMgTrampoline_c's,
 * with its own scratch buffer. */
void dScMgTrampoline2_c::PaintSquare(int x_base, int y, int val, int n)
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

                        MultiCopy_Int(addr, sTrmp2BrushWord, 4);
                        sTrmp2BrushWord[0] = (sTrmp2BrushWord[0] & (-1 ^ (mask << ((x & 7) * 4)))) |
                                             (val << ((x & 7) * 4));
                        MultiCopy_Int(sTrmp2BrushWord, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

// @symbol Trmp2Spinner_Construct
extern "C" {
int Trmp2Spinner_Construct(char *raw)
{
    _ZN5ModelC1Ev(raw);
    *(short *)(raw + 0x76) = 0;
    return (int)raw;
}
}

// @symbol Trmp2Spinner_Destruct
extern "C" Model* Trmp2Spinner_Destruct(Model* m){
    sTrmp2StarModelFile.Release();
    _ZN5ModelD1Ev(m);
    return m;
}

// @symbol Trmp2Spinner_LoadAll
extern "C" {
void Trmp2Spinner_LoadAll(int x, int count) {
    int i;
    sTrmp2Spinners = (unsigned char *)x;
    sTrmp2SpinnerCount = count;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        Trmp2Spinner_SetModel((void *)(sTrmp2Spinners + i * 0x78));
    }
}
}

// @symbol Trmp2Spinner_InitAll
extern "C" {
void Trmp2Spinner_InitAll(unsigned char *c)
{
  int i = 0;
  int n = sTrmp2SpinnerCount;
  unsigned char *base;
  unsigned char *p;
  if (n <= 0)
  {
    return;
  }
  base = sTrmp2Spinners;
  p = base;
  do
  {
    if ((*((short *) (p + 0x76))) == 0)
    {
      Trmp2Spinner_Init(base + (i * 0x78), c);
      return;
    }
    i++;
    p += 0x78;
  } while (i < sTrmp2SpinnerCount);
}
}

// @symbol Trmp2Spinner_HideAll
extern "C" {
void Trmp2Spinner_HideAll(void){
  int i = 0;
  if (sTrmp2SpinnerCount > 0) {
      int off = 0;
      do {
          *(short *)(sTrmp2Spinners + off + 0x76) = 0;
          i++;
          off += 0x78;
      } while (i < sTrmp2SpinnerCount);
  }
}
}

// @symbol Trmp2Spinner_UpdateAll
extern "C" {
void Trmp2Spinner_UpdateAll(void)
{
    int i;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        char *obj = (char *)sTrmp2Spinners + i * 0x78;
        if (*(short *)(obj + 0x76) != 0) {
            Trmp2Spinner_Update(obj);
        }
    }
}
}

// @symbol Trmp2Spinner_UpdateMatricesAll
extern "C" {
void Trmp2Spinner_UpdateMatricesAll(void)
{
    int i;
    for (i = 0; i < sTrmp2SpinnerCount; i++) {
        char *obj = (char *)sTrmp2Spinners + i * 0x78;
        if (*(short *)(obj + 0x76) != 0) {
            Trmp2Spinner_UpdateMatrix(obj);
        }
    }
}
}

// @symbol Trmp2Spinner_SetModel
extern "C" void Trmp2Spinner_SetModel(void* raw)
{
    BMD_File *file = (BMD_File *)Model::LoadFile(sTrmp2StarModelFile);
    ((ModelBase *)raw)->SetFile(file, 1, -1);
}

extern "C" {
extern struct Tbl sTrmp2SpinnerSpeeds;
extern unsigned short sTrmp2SpinnerColors[];
extern int RandomIntInternal(int* seed);
extern void func_020169f4(char* self, int kind);

// @symbol Trmp2Spinner_Init
void Trmp2Spinner_Init(void *r0, void *r1) {
    char* raw = (char *)r0;
    int value = (int)r1;
    struct Tbl t = sTrmp2SpinnerSpeeds;
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
    func_020169f4(raw, sTrmp2SpinnerColors[idx]);
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

// @symbol Trmp2Spinner_Update
extern "C" {
void Trmp2Spinner_Update(char* raw){
  ApproachLinear2(*(short *)(raw + 0x76), 0, 1);
  short* p = (short*)((int)raw + 0x74);
  *p = *p + 0x400;
  AddVec3((struct Vector3*)(raw+0x5c), (struct Vector3*)(raw+0x68), (struct Vector3*)(raw+0x5c));
}
}

// @symbol Trmp2Spinner_UpdateMatrix
extern "C" void Trmp2Spinner_UpdateMatrix(char *raw) {
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
// @symbol Trmp2_DestructJumperModels
int Trmp2_DestructJumperModels(char* raw){
  __cxa_vec_cleanup(raw+0x1a8, 5, 8, (void*)NullDestructor_0203d47c);
  _ZN18TextureTransformerD1Ev(raw+0x194);
  _ZN5ModelD1Ev(raw+0x140);
  _ZN5ModelD1Ev(raw+0xf0);
  _ZN5ModelD1Ev(raw+0xa0);
  return (int)raw;
}
}

/* D1 and D0 come from the inline destructor in dScMgTrampoline2_c.h. */

/* sTrmp2StarModelFile is the model SharedFilePtr this Init LoadFile's,
 * defined here at the end of the file (constructed as file ID 537) so its
 * constructor does not enter .text. */
Trampoline2ModelFilePtr sTrmp2StarModelFile(ASSET_MINI_STAR_BMD);
