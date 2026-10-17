//cpp
/* dScMgTrampoline_c -- Trampoline Time. Draw a line on the touch screen
 * (up to three at once) and bounce Mario to the lit door. Three misses
 * end the round.
 *
 * Functions are written in reverse of ROM order; do not reorder.
 * `opt_loop_invariants off` above PaintSquare has no matching `on`, so it
 * applies to the whole file. TUBUILD CONFLICT comments are paired with
 * the manifest; leave them.
 *
 * leftovers:
 * - The factory still builds the object by hand, so its offset stores
 *   stay raw.
 * - TrmpScorePopup::Render, TrmpScorePopup::Update and TrmpFx_Spawn keep raw offsets:
 *   the member-access form DIFFed there.
 * - The score pool (TrmpFx_Unlink..TrmpFx_InitPool) stays free:
 *   it works on the file-scope live-list globals, not one object, and
 *   TrmpFx_Spawn is the entry dMgTrmpln2Mario_c calls.
 * - Trmp_Nothing2 / Trmp_Nothing stay free: the shared empty
 *   ctor/dtor pair for the TrmpScorePopup pool, used by d_s_mg_trampoline2.cpp too.
 * - ov004 helpers and most data symbols are unnamed.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_trampoline_AssetId {
    ASSET_D_2D_MG_BG_TRAMPOLINE_NCG_BIN = 0x00a5,
    ASSET_D_2D_MG_BG_TRAMPOLINE_NCL_BIN = 0x00a6,
    ASSET_D_2D_MG_BG_TRAMPOLINE_NSC_BIN = 0x00a7,
    ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN = 0x0108
};

#include "dScMgTrampoline_c.h"
#include "SpawnInfo.h"
#include "types.h"
#include "G2x.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
/* local: the ROM re-reads the slot byte at each use here, so keep the read volatile. */
#define ACTIVE_SLOT (*(volatile unsigned char *)&gActivePlayerSlot)
/* local: these readers only reproduce through a 4-byte-record view of the lane. */
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))

namespace cstd { int fdiv(int numerator, int denominator); }
namespace G2   { void *GetBG0ScrPtr(); void *GetBG1ScrPtr(); void *GetBG2ScrPtr(); void *GetBG3ScrPtr(); void *GetBG2CharPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace G3X  { void SetFog(bool enable, int blend, int slope, int offset); }

/* Local stand-ins for types no header declares yet. */
typedef short s16;

struct Node { struct Node* next; };

/* One floating score. Five of these are mArray3. Spawned when a Mario
   lands: x starts at the right edge and drifts left, y eases toward
   targetY, and number is the score drawn by Mg_DrawBigNumber.
   Same 0x24 shape as dScMgJump2_c's Elem. targetX is written and not
   read by this TU's step. */
struct TrmpScorePopup {
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

    void Spawn(int y, int number); /* fills a slot and links it live */
    void Update();                 /* ticks life, eases toward targetY */
    void Render();                 /* draws the number when active */
};

/* Shadow of the scene's +0x68/+0x6c, which sit in dScMgBase_c padding.
   PaintSquare reads the layer; InitBrush writes both. */
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


/* Raw storage form of mwccarm's eight-byte single-inheritance PMF. Behavior
   gives the live scene storage its dScMgTrampoline_c::State meaning at the
   dispatch site; these descriptors retain the raw form because direct
   namespace-scope PMF initialization makes
   mwccarm 2004/b56 emit a __sinit, .ctor entry, and five anonymous data
   temporaries that the cartridge does not contain. */
struct P2 { int a, b; };

// Preserve the original front end's blind two-word aggregate copy in C++ mode.
struct P2Words { int words[2]; };

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
extern void Mg_RenderSpriteBoth(void *a, int b, int c, int d, int e);
extern int sTrmpRowScreenY[];
extern int sTrmpRowMarkerSprite;
extern int _Z15ApproachLinear2Rsss(s16 *dst, s16 a, s16 b);
extern void Mg_DrawBigNumber(int,int,int,int,int,int,int);
extern void TrmpFx_Link(int *self);
extern struct Node *sTrmpFxList;
extern int sTrmpFxCount;
extern int sTrmpFxPool;

extern int sTrmpBrushWord[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern const unsigned char sTrmpBrushPltt[];
extern UnkObj* data_0209f5bc;
extern int sTrmpTouchBlocked;
unsigned int func_02012790(unsigned int a);
int TrmpLine_Create(Vec2s* a, Vec2s* b);

void SetBg2Offset(int a, int b);
int Mg_GetTopScreenYOffset(void);
extern "C" unsigned char data_0209d45c;
extern "C" void Mg_CommitRecords(void *scene);
int  GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern void Mg_DrawLine(void *a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern short sTrmpGoalRow;

extern void Trmp3DMario_UpdateAll(...);
extern int _Z14ApproachLinearRiii(int*,int,int);
extern int Trmp3DMario_CountFinished(void);
extern void Mg_SetSharedState(int);
extern void Trmp3DMario_StartAll(void *c);
extern int sTrmpComboCount;
extern int sTrmp3DLives;
extern int sTrmpMariosInPlay;
extern int sTrmpAnyOnLedge;
extern int data_0209e650;
extern void TrmpLine_UpdateAll(void);
extern int Trmp3DMario_IsOffstage(int *r0);
extern void Trmp3DMario_EnterWalkIn(char *c);
extern int RandomIntInternal(int *seed);
extern "C" void Trmp3DMario_SetSpawnHeight(int n);
extern "C" void Trmp3DMario_SetTuning(int a);
void TrmpLine_CreateStartLineT1(void);
extern s16 sTrmpIntroFromX;
extern s16 sTrmpStartTouchX;
extern s16 sTrmpStartTouchY;
extern void Camera_UpdateMatrices(void *cam);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *ptr);
extern int sTrmpNcgByLanguage[];
extern void InitialiseVramGlobals(void);
extern s16 data_02082414;
extern int Trmp3DMario_InitPool(unsigned char *, int);
extern void TrmpFx_InitPool(void *, int);
extern void TrmpLine_LoadTextures(void);
extern void Mg_SetTopScreenYOffset(int);
extern void Trmp3DMario_ResetAll(unsigned int, int);
extern void TrmpFx_ClearAll(void);
extern int sTrmpPuffXByLanguage[];
extern void Puff_ClearAll(int, int);
extern void func_02012718(int, int);
extern void TrmpFx_UpdateAll(void);
extern void Mg_SubmitScore(int);
extern void TrmpFx_DrawAll(void);
extern int *sTrmpTextSpritesByLanguage[];
extern int sTrmpLifeIconSprite;
extern void Mg_DrawTopRightCounter(int, int);
extern void *sTrmpDoorSignSprites[];
extern void *sTrmpArrowSprite;
extern void Trmp3DMario_DrawAll(void);
extern void TrmpLine_DrawAll(void);
extern void *_ZN11dScMgBase_cC2Ev(void *self);
extern void _ZN8Particle10SysTrackerC1Ev(void *self);
extern void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
extern int _ZTV14dScMgD3DBase_c[];
extern int _ZTV17dScMgTrampoline_c[];
extern void Trmp3DMario_Construct(void);
extern void TrmpLine_Reset(void);
extern void Trmp_Nothing2(void);

dScMgTrampoline_c *dScMgTrampoline_c_classInit(void);
/* Literal aliases used only to make the five ROM PMF relocations static data.
   Their definitions below are real compiler-spelled C++ members. Each is a
   local extern: a pointer to member cannot convert to the int word a P2
   entry holds, so dScMgTrampoline_c.h's declarations cannot fill the table. */
extern "C" void _ZN17dScMgTrampoline_c9StateDoneEv(void);       /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c13StateWaitExitEv(void);  /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c12StateResultsEv(void);   /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c9StatePlayEv(void);       /* local extern: PMF -> int */
extern "C" void _ZN17dScMgTrampoline_c10StateIntroEv(void);     /* local extern: PMF -> int */
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

/* Data owned by the same original TU. */
P2 pmf_dScMgTrampoline_c_StateWaitExit = { (int)_ZN17dScMgTrampoline_c13StateWaitExitEv, 0 };
P2 pmf_dScMgTrampoline_c_StateDone = { (int)_ZN17dScMgTrampoline_c9StateDoneEv, 0 };
P2 pmf_dScMgTrampoline_c_StateIntro = { (int)_ZN17dScMgTrampoline_c10StateIntroEv, 0 };
BaseSpawnInfo<dScMgTrampoline_c> g_profile_MG_TRAMPOLINE = {
    dScMgTrampoline_c_classInit,
    0x180,
    0x180,
};
P2 pmf_dScMgTrampoline_c_StatePlay = { (int)_ZN17dScMgTrampoline_c9StatePlayEv, 0 };
P2 pmf_dScMgTrampoline_c_StateResults = { (int)_ZN17dScMgTrampoline_c12StateResultsEv, 0 };
int sTrmpNcgByLanguage[] = {0x107, 0x109, 0x104, 0x105, 0x106};
int *sTrmpTextSpritesByLanguage[] = {sMgTextSpritesEnglish, sMgTextSpritesSpanish,
                                     sMgTextSpritesFrench, sMgTextSpritesGerman,
                                     sMgTextSpritesItalian};
int sTrmpPuffXByLanguage[] = {(int)sTrmpPuffEnglish, (int)sTrmpPuffSpanish, (int)sTrmpPuffFrench,
                              (int)sTrmpPuffGerman, (int)sTrmpPuffItalian};
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_Link, NOT applied: extern void *sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_DrawAll, NOT applied: extern void *sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxPool, from the legacy file for TrmpFx_ClearAll, NOT applied: extern char* sTrmpFxPool; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpFxList, from the legacy file for TrmpFx_ClearAll, NOT applied: extern int sTrmpFxList; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_02120f18, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054efc, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054efc(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054ea8, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054ea8(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02120f18, NOT applied: extern char *func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for _ZN17dScMgTrampoline_c16OnGraphCallback0Ev, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for _ZN17dScMgTrampoline_c16OnGraphCallback0Ev, NOT applied: void MultiStore16(u16 val, void* dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of Mg_RenderSpriteBoth, from the legacy file for _ZN17dScMgTrampoline_c6RenderEv, NOT applied: void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121778, NOT applied: extern void MultiStore16(unsigned short a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of Trmp3DMario_UpdateAll, from the legacy file for func_ov006_021218fc, NOT applied: extern void Trmp3DMario_UpdateAll(...); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z14ApproachLinearRiii(int *v, int step, int rate); */
/* TUBUILD CONFLICT -- alternate declaration of _Z15ApproachLinear2Rsss, from the legacy file for func_ov006_021218fc, NOT applied: extern int _Z15ApproachLinear2Rsss(short *v, short step, short rate); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_021218fc, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int bank); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpComboCount, from the legacy file for func_ov006_02121bc8, NOT applied: extern "C" int sTrmpComboCount; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_02120a18, from the legacy file for func_ov006_02121cf4, NOT applied: extern void func_ov006_02120a18(char* p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound12PlayBank2_2DEj, from the legacy file for func_ov006_02121cf4, NOT applied: extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n); */
/* TUBUILD CONFLICT -- alternate declaration of TrmpLine_UpdateAll, from the legacy file for func_ov006_02121d64, NOT applied: void TrmpLine_UpdateAll(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_02054d88, from the legacy file for func_ov006_02121d64, NOT applied: int func_02054d88(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_02121d64, NOT applied: void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012718, from the legacy file for func_ov006_02121d64, NOT applied: void func_02012718(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov006_02121d64, NOT applied: int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of Mg_DrawLine, from the legacy file for func_ov006_02121d64, NOT applied: void Mg_DrawLine(void *a, int b, int c, int d, int e, int f, int g); */
/* TUBUILD CONFLICT -- alternate declaration of func_020126e8, from the legacy file for func_ov006_02121d64, NOT applied: int func_020126e8(int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012468, from the legacy file for func_ov006_02121d64, NOT applied: int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h); */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchX, from the legacy file for func_ov006_02121f04, NOT applied: extern short sTrmpStartTouchX; */
/* TUBUILD CONFLICT -- alternate declaration of sTrmpStartTouchY, from the legacy file for func_ov006_02121f04, NOT applied: extern short sTrmpStartTouchY; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN17dScMgTrampoline_c11CanEndStateEi, NOT applied: extern int GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN4cstd4fdivEii, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern s32 _ZN4cstd4fdivEii(s32 a, s32 b); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209d45c, from the legacy file for _ZN17dScMgTrampoline_c13InitResourcesEv, NOT applied: extern u8 data_0209d45c; */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for func_ov006_0212231c, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2G213GetBG2CharPtrEv, from the legacy file for func_ov006_0212231c, NOT applied: extern char *_ZN2G213GetBG2CharPtrEv(void); */
/* TUBUILD CONFLICT -- alternate declaration of MultiStore16, from the legacy file for func_ov006_0212231c, NOT applied: extern void MultiStore16(u16 val, char *dst, int nbytes); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN2GX10LoadBGPlttEPKvjj, from the legacy file for func_ov006_0212231c, NOT applied: extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b); */
}

// @symbol Trmp_Nothing2
extern "C" void Trmp_Nothing2(void)
{
}

// @symbol dScMgTrampoline_c_classInit
extern "C" dScMgTrampoline_c *dScMgTrampoline_c_classInit(void) {
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
                      (void *)Trmp3DMario_Construct,
                      (void *)Trmp3DMario_Destruct);
        __cxa_vec_ctor(scene + 0x534c, 3, 0x32c,
                      (void *)TrmpLine_Reset,
                      (void *)TrmpLine_Destroy);
        __cxa_vec_ctor(scene + 0x5cd0, 5, 0x24,
                      (void *)Trmp_Nothing2,
                      (void *)Trmp_Nothing);
        ((dScMgTrampoline_DoorMark *)(scene + 0x5d84))->Init();
    }
    return (dScMgTrampoline_c *)scene;
}

// @symbol _ZN17dScMgTrampoline_c11InitDisplayEv
void dScMgTrampoline_c::InitDisplay()
{
    int r8, r7, r6, r5, r4;
    volatile u16 sp0;

    r8 = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NCG_BIN);
    r7 = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NSC_BIN);
    r6 = LoadFile(ASSET_D_2D_MG_BG_TRAMPOLINE_NCL_BIN);
    r5 = LoadFile(sTrmpNcgByLanguage[GetGameLanguage()]);
    r4 = LoadFile(ASSET_D_2D_MG_TRAMPOLINE_NCL_BIN);

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

    InitBrush(3);
}

// @symbol _ZN17dScMgTrampoline_c13InitResourcesEv
/* Slot 0. Sets up the two cameras and the fog, then ends with a virtual
 * call to StartRound(-1), the scene's reset hook. */
s32 dScMgTrampoline_c::InitResources()
{
    char *base = (char *)this;
    s32 fov;

    this->mScrollY = 0x20;
    this->mScrollTargetY = this->mScrollY;
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
    Camera_UpdateMatrices(base + 0x466c);
    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x400;
    Camera_UpdateMatrices(base + 0x4728);
    if (Trmp3DMario_InitPool((unsigned char *)(base + 0x500c), 4) == 0)
        return 0;
    TrmpFx_InitPool(base + 0x5cd0, 5);
    TrmpLine_LoadTextures();
    Mg_SetTopScreenYOffset(0x10);
    StartRound(-1);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c10StartRoundEi
extern "C" void TrmpLine_ClearAll(int);
extern volatile s16 data_020a0dbc[];

void dScMgTrampoline_c::StartRound(int /* arg */)
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

    sTrmpAnyOnLedge = 0;
    self->mLevel = 0;
    if ((unsigned int)self->mLevel > 0x270e)
        self->mLevel = 0x270e;
    /* mLevel is u32 on dScMgBase_c (dScMg3DEsp_c's evidence). This function
       divides and takes a remainder of it, where signedness IS codegen -- it
       reads the field SIGNED, which is what this header used to declare
       locally. Cast at the use site rather than retype the shared base. */
    q = ((((s32)self->mLevel % 5) << 12)) / 4;
    self->mScrollY = FX_MUL((0x1000 - q), 0x20) + FX_MUL(q, 0x50);
    self->mScrollTargetY = self->mScrollY;
    self->mScrollHoldTimer = 0;
    Trmp3DMario_ResetAll(self->mLevel, self->mScrollY);

    TrmpLine_ClearAll(self->mLevel);
    TrmpFx_ClearAll();
    Puff_ClearAll(0, sTrmpPuffXByLanguage[GetGameLanguage()]);

    mDoorMark.Hide();

    self->mInputEnabled = 0;
    self->mRoundOver = 0;
    self->mDoorSwitchTimer =
        (((int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13)
            * 0x2d0) >> 12) + 0x2d0;
    self->mDoorBlinkTimer = 0;
    self->mDoorBlinkPhase = 1;
    self->mArrow1X = 0;
    self->mArrow2X = 0;

    SetDoorSide(0);

    {
        char *dst = (char *)func_02054d88();
        fill = 0;
        MultiStore16(fill, dst, 0x6000);
    }

    BeginIntro();
}

// @symbol _ZN17dScMgTrampoline_c11CanEndStateEi
/* dScMgTrampoline_c::CanEndState, from its vtable slot. */
int dScMgTrampoline_c::CanEndState(int /* mode */)
{
    Puff_ClearAll(0, sTrmpPuffXByLanguage[GetGameLanguage()]);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c10BeginIntroEv
void dScMgTrampoline_c::BeginIntro()
{
    short a;
    short b;

    mTimer = 0x5a;
    a = sTrmpStartTouchX;
    mTouchStartX = a;
    b = sTrmpStartTouchY;
    mTouchStartY = b;
    mTouchX = a;
    mTouchY = b;
    mDragSoundHandle = 0;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateIntro;
}

// @symbol _ZN17dScMgTrampoline_c10StateIntroEv
void dScMgTrampoline_c::StateIntro()
{
    char *c = (char *)this;
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

        TrmpLine_CreateStartLineT1();
        func_02012718(0x1af, 0x8000);
        BeginPlay();
        return;
    }

    {
        int t = cstd::fdiv(counter << 12, 0x5a000);
        volatile s16 oldY = mTouchX;
        volatile s16 oldZ = mTouchY;
        int mixRaw = sTrmpStartTouchX * t + sTrmpIntroFromX * (0x1000 - t);

        mTouchX = (s16)(mixRaw >> 12);
        mTouchY = sTrmpStartTouchY;

        mTouchY +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) &
                              ~0x80000000) >> 19) -
               0x800) << 2) >> 12);

        Mg_DrawLine(c, oldY, oldZ,
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
    mDoorMark.Show(mDoorSide);
    mDragSoundHandle = 0;
    Sound::PlayBank2_2D(0x1b6);
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StatePlay;
}

// @symbol _ZN17dScMgTrampoline_c12UpdateScrollEv
int ApproachLinear(int&, int, int);

void dScMgTrampoline_c::UpdateScroll()
{
    int q = sTrmpComboCount / 10;
    int old = mLevel;
    Trmp3DMario_SetSpawnHeight(mScrollY);
    Trmp3DMario_SetTuning((q << 8) + 0x1000);
    if (old != q) {
        mLevel = q;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
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
    int old = sTrmpComboCount;
    TrmpLine_UpdateAll();
    Trmp3DMario_UpdateAll();
    if (sTrmpComboCount > old) {
        if (sTrmpComboCount == 3)
            mPromptEnabled = 0;
    }
    UpdateScroll();

    if (sTrmp3DLives == 0) {
        mInputEnabled = 0;
        BeginResults();
    } else {
        if (_Z14ApproachLinearRiii(&mTimer, 0, 1)) {
            if (sTrmpComboCount >= 5 || sTrmpMariosInPlay < 3) {
                int i;
                char *p = (char *)mArray1;
                for (i = 0; i < 4; i++) {
                    if (Trmp3DMario_IsOffstage((int *)p)) {
                        Trmp3DMario_EnterWalkIn((char *)mArray1 + i * 0xd0);
                        break;
                    }
                    p += 0xd0;
                }
            }
            mTimer = 0x168;
            {
                int amt = sTrmpComboCount * 2;
                if (amt > 0xb4) amt = 0xb4;
                mTimer -= amt;
            }
        }
    }

    if (sTrmpAnyOnLedge == 0) {
        if (_Z15ApproachLinear2Rsss((short *)&mDoorSwitchTimer, 0, 1)) {
            if (GetDoorSide()) {
                SetDoorSide(0);
                mDoorMark.Show(mDoorSide);
            } else {
                SetDoorSide(1);
                mDoorMark.Show(mDoorSide);
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
    Trmp3DMario_StartAll(c);
    mTimer = 0x5a;
    *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateResults;
}

// @symbol _ZN17dScMgTrampoline_c12StateResultsEv
void dScMgTrampoline_c::StateResults()
{
  unsigned char *c = (unsigned char *)this;
  Trmp3DMario_UpdateAll(c);
  if(_Z14ApproachLinearRiii(&mTimer,0,1)==0) return;
  if(Trmp3DMario_CountFinished()==0) return;
  Mg_SetSharedState(MG_STATE_GAME_OVER);
  mPromptEnabled = 0;
  mTimer = 0xb4;
  *(P2Words *)mState = *(P2Words *)&pmf_dScMgTrampoline_c_StateWaitExit;
}

// @symbol _ZN17dScMgTrampoline_c13StateWaitExitEv
void dScMgTrampoline_c::StateWaitExit()
{
    int idx, b;
    mTimer -= 1;
    if (mTimer != 0)
    {
        idx = ACTIVE_SLOT;
        b = 0;
        if (TOUCH_REC(gTouchHeld)[idx][0] != 0 && TOUCH_REC(gTouchEdge)[idx][0] != 0)
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
        int w0 = ((int *)&pmf_dScMgTrampoline_c_StateDone)[0];
        int w1 = ((int *)&pmf_dScMgTrampoline_c_StateDone)[1];
        w0 = w1 ? w0 : w0;
        mState[0] = w0;
        mState[1] = w1;
    }
}

// @symbol _ZN17dScMgTrampoline_c9StateDoneEv
void dScMgTrampoline_c::StateDone()
{
}

// @symbol _ZN17dScMgTrampoline_c11GetDoorSideEv
s16 dScMgTrampoline_c::GetDoorSide()
{
    return mDoorSide;
}

// @symbol _ZN17dScMgTrampoline_c11SetDoorSideEs
void dScMgTrampoline_c::SetDoorSide(short value)
{
    sTrmpGoalRow = value;
    mDoorSide = value;
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
        Mg_DrawLine(this, mTouchX, mTouchY,
                            TOUCH_REC(gTouchX)[i][0], TOUCH_REC(gTouchY)[i][0], 2, 4);
        i = ACTIVE_SLOT;
        mTouchX = TOUCH_REC(gTouchX)[i][0];
        mTouchY = TOUCH_REC(gTouchY)[i][0];
        mDragSoundHandle = func_02012468(mDragSoundHandle, 2, 0x1b0, 2, 0,
                                         func_020126e8(mTouchX << 12), 0, 0);
    } else {
        b = (int)(TOUCH_REC(gTouchHeld)[i][0] == 0 && TOUCH_REC(gTouchEdge)[i][0] != 0);
        if (b != 0) { mTouchReleased = 1; }
    }
}

// @symbol _ZN17dScMgTrampoline_c8BehaviorEv
/* Slot 6. The field at 0x5004 is a pointer to member function: the current
 * state, one of the five State functions. The header keeps it as two raw
 * words, so it is cast to dScMgTrampoline_c::State here to dispatch. */
s32 dScMgTrampoline_c::Behavior()
{
    int saved = sTrmpComboCount;
    TrmpFx_UpdateAll();
    (this->*(*(State *)mState))();
    UpdateTouchInput();
    mDoorMark.Update();
    if (saved != sTrmpComboCount)
        Mg_SubmitScore(sTrmpComboCount);
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

    mDoorMark.Render(mScrollY);
    TrmpFx_DrawAll();

    if (mDrawSubScreen == 1) {
        count = sTrmp3DLives;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)sTrmpTextSpritesByLanguage[idx][1], (void *)a1v, 0xc,
                              (void *)0);
            } else {
                RenderOamMainScreen(sTrmpLifeIconSprite, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    Mg_DrawTopRightCounter(sTrmpComboCount, 4);

    aa = mDoorSide;
    r6 = 1;
    r5 = 1;
    if (aa == 0) {
        if (mDoorBlinkPhase != 0) r6 = 0;
    }
    if (aa != 0) {
        if (mDoorBlinkPhase != 0) r5 = 0;
    }

    t = sTrmpRowScreenY[0] - (mScrollY + mScrollOffsetY);
    Mg_RenderSpriteBoth(sTrmpArrowSprite, mArrow1X + 0xf0, t, -1, 2);
    Mg_RenderSpriteBoth(sTrmpDoorSignSprites[r6], 0xf0, t - 0x20, -1, 2);

    t = sTrmpRowScreenY[1] - (mScrollY + mScrollOffsetY);
    Mg_RenderSpriteBoth(sTrmpArrowSprite, mArrow2X + 0xf0, t, -1, 2);
    Mg_RenderSpriteBoth(sTrmpDoorSignSprites[r5], 0xf0, t - 0x20, -1, 2);

    if (mRoundOver == 0) {
        Trmp3DMario_DrawAll();
        TrmpLine_DrawAll();
    }
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c16CleanupResourcesEv
/* Slot 3. */
s32 dScMgTrampoline_c::CleanupResources()
{
    Mg_CommitRecords(this);
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c16OnGraphCallback2Ev
/* dScMgTrampoline_c::OnGraphCallback2, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback2()
{
    if (!dScMgD3DBase_c::OnGraphCallback2())
        return 0;
    if (mMenuOpen == 0) {
        if (mDrawSubScreen == 0) {
            data_0209d45c &= ~8;
            SetBg2Offset(0, mScrollY + mScrollOffsetY);
            *(volatile int*)0x4000018 = (mScrollY + mScrollOffsetY) << 16 & 0x1ff0000;
        } else {
            data_0209d45c |= 8;
            SetBg2Offset(0, mScrollY + 0xc0 + mScrollOffsetY + Mg_GetTopScreenYOffset());
            *(volatile int*)0x4000018 = (mScrollY + 0xc0 + mScrollOffsetY + Mg_GetTopScreenYOffset()) << 16 & 0x1ff0000;
        }
    }
    return 1;
}

// @symbol _ZN17dScMgTrampoline_c16OnGraphCallback3Ev
/* dScMgTrampoline_c::OnGraphCallback3, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback3()
{
    return dScMgD3DBase_c::OnGraphCallback3() != 0;
}

// @symbol _ZN17dScMgTrampoline_c16OnGraphCallback0Ev
/* dScMgTrampoline_c::OnGraphCallback0, from its vtable slot. */
int dScMgTrampoline_c::OnGraphCallback0()
{
    volatile u16 z;
    Vec2s v1;
    Vec2s v2;
    s16 a;
    int dst;

    if (!dScMgBase_c::OnGraphCallback0())
        return 0;

    if (data_0209f5bc->vt[5](data_0209f5bc)) {
        sTrmpTouchBlocked = 0;
    } else if (data_0209f5bc->vt[7](data_0209f5bc)) {
        sTrmpTouchBlocked = 1;
    }

    if (sTrmpTouchBlocked != 0)
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
            if (TrmpLine_Create(&v1, &v2)) {
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

// @symbol _ZN17dScMgTrampoline_c9InitBrushEi
void dScMgTrampoline_c::InitBrush(int layer)
{
    Obj *self = (Obj *)this;
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
    GX::LoadBGPltt(sTrmpBrushPltt, 0xe0, 0x20);
}

#pragma opt_loop_invariants off

// @symbol _ZN17dScMgTrampoline_c11PaintSquareEiiii
/* Slot 34, the brush stamp. dScMgD3DBase_c does not override this slot,
   so each trampoline class has its own; they differ only in the shape
   table (sTrmpBrushWord here, _02142f78 in dScMgTrampoline2_c).
   bgLayer (+0x6c, in the base padding) picks the character base. No
   wrapped-region branch: a 3D minigame owns the top screen. */
void dScMgTrampoline_c::PaintSquare(int x_base, int y, int val, int n)
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

                        MultiCopy_Int(addr, sTrmpBrushWord, 4);
                        sTrmpBrushWord[0] = (sTrmpBrushWord[0] & (-1 ^ (mask << ((x & 7) * 4)))) |
                                            (val << ((x & 7) * 4));
                        MultiCopy_Int(sTrmpBrushWord, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

// @symbol TrmpFx_InitPool
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_InitPool(void *records, int count) {
    sTrmpFxPool = (int)records;
    sTrmpFxCount = count;
}
}

// @symbol TrmpFx_Spawn
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* TrmpScorePopup* index DIFFed (the scale folded the wrong register). */
void TrmpFx_Spawn(int x, int y) {
  int i;
  for (i = sTrmpFxCount - 1; i >= 0; i--) {
      if (*(short *)(sTrmpFxPool + i * 0x24 + 0x20) == 0) {
          ((TrmpScorePopup *)(sTrmpFxPool + i * 0x24))->Spawn(x, y);
          return;
      }
  }
}
}

// @symbol TrmpFx_ClearAll
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_ClearAll(void){
    int i;
    for (i = 0; i < sTrmpFxCount; i++) {
        ((TrmpScorePopup *)(sTrmpFxPool + i * 0x24))->active = 0;
    }
    sTrmpFxList = 0;
}
}

// @symbol TrmpFx_UpdateAll
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_UpdateAll(void){
  int i = 0;
  if (sTrmpFxCount > 0) {
      int off = 0;
      do {
          ((TrmpScorePopup *)(sTrmpFxPool + off))->Update();
          i++;
          off += 0x24;
      } while (i < sTrmpFxCount);
  }
}
}

// @symbol TrmpFx_DrawAll
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_DrawAll(void) {
    void *node = sTrmpFxList;
    if (node == 0) return;
    do {
        ((TrmpScorePopup *)node)->Render();
        node = *(void **)node;
    } while (node != 0);
}
}

// @symbol TrmpFx_Link
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_Link(int *entryRaw) {
    TrmpScorePopup *entry = (TrmpScorePopup *)entryRaw;
    TrmpScorePopup *node;
    entry->next = sTrmpFxList;
    sTrmpFxList = (Node *)entry;
    node = (TrmpScorePopup *)entry->next;
    int v = entry->y - 0x10000;
    while (node) {
        node->targetY = v;
        node = (TrmpScorePopup *)node->next;
        v -= 0x10000;
    }
}
}

// @symbol TrmpFx_Unlink
extern "C" {  /* .c-derived member: C linkage for the whole block */
void TrmpFx_Unlink(struct Node* node) {
    struct Node *prev = sTrmpFxList;
    if (prev == node) {
        sTrmpFxList = node->next;
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

// @symbol _ZN14TrmpScorePopup5SpawnEii
/* Fills a slot with fixed parameters and tail-calls TrmpFx_Link. */
void TrmpScorePopup::Spawn(int y, int number) {
    x = 0x110000;
    this->y = y;
    speedX = -0x4000;
    speedY = 0;
    targetX = 0xc0000;
    targetY = y;
    life = 0x80;
    active = 1;
    this->number = number;
    TrmpFx_Link((int *)this);
}

extern "C" void func_0203d630(int *p, int m);
// @symbol _ZN14TrmpScorePopup6UpdateEv
/* Member form of x += speedX DIFFed. Offsets are TrmpScorePopup's. */
void TrmpScorePopup::Update() {
    char *self = (char *)this;
    if ((*((short *)(self + 0x20))) == 0) {
        return;
    }
    if (_Z15ApproachLinear2Rsss((short *)(self + 0x1c), 0, 1)) {
        TrmpFx_Unlink((Node *)self);
        *((short *)(self + 0x20)) = 0;
    }
    {
        int *p = (int *)(((int)self + 4));
        *p = *p + (*((int *)(self + 0x14)));
    }
    _Z14ApproachLinearRiii((int *)(self + 8), *((int *)(self + 0x10)), 0x1800);
    func_0203d630((int *)(self + 0x14), 0xf00);
}

// @symbol _ZN14TrmpScorePopup6RenderEv
/* Member form DIFFed (ldrsh of number). Offsets are TrmpScorePopup::active, x, y, number. */
void TrmpScorePopup::Render() {
    char *desc = (char *)this;
    if (*(short *)(desc + 0x20) == 0)
        return;
    Mg_DrawBigNumber(*(int *)(desc + 4) >> 12, *(int *)(desc + 8) >> 12, *(short *)(desc + 0x1e),
                     -1, -1, 0, 0);
}

// @symbol _ZN24dScMgTrampoline_DoorMark4InitEv
void dScMgTrampoline_DoorMark::Init()
{
    active = 0;
    shown = 0;
}

// @symbol _ZN24dScMgTrampoline_DoorMark4HideEv
void dScMgTrampoline_DoorMark::Hide()
{
    active = 0;
    shown = 0;
}

// @symbol _ZN24dScMgTrampoline_DoorMark4ShowEi
void dScMgTrampoline_DoorMark::Show(int side)
{
    row = (s16)side;
    timer = 0x3c;
    shown = 1;
    active = 1;
    Sound::PlayBank2_2D(0x1B7);
}

// @symbol _ZN24dScMgTrampoline_DoorMark6UpdateEv
void dScMgTrampoline_DoorMark::Update()
{
    if (active == 0) return;
    if (_Z15ApproachLinear2Rsss(&timer, 0, 1) != 0) {
        active = 0;
    }
    if (((timer / 10) & 1) != 0) {
        shown = 0;
    } else {
        shown = 1;
    }
}

// @symbol _ZN24dScMgTrampoline_DoorMark6RenderEi
void dScMgTrampoline_DoorMark::Render(int scrollY) {
    if (active == 0) return;
    if (shown == 0) return;
    Mg_RenderSpriteBoth((void *)sTrmpRowMarkerSprite, 0xd0, sTrmpRowScreenY[row] - scrollY - 8, -1,
                        -1);
}

// @symbol Trmp_Nothing
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Trmp_Nothing(void)
{
}
}

