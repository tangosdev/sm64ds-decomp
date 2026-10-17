//cpp
/* ov006/dScMgJump2_c -- the second jump minigame, 30 functions
 * (.text 0x020eebe8..0x020efc0c), ending with the factory
 * dScMgJump2_c_classInit and the element constructor Jump2Popup_ElemCtor
 * it registers.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. The legacy file for State_Play had
 * `#pragma opt_propagation off`; it is not needed, and carried here it
 * would apply to every function in the file.
 *
 * leftovers:
 *   - Jump2Popup_InitPool, ..ef05c, ..eeff0, ..eef90, ..eef58 and ..ef2b8
 *     stay free: none takes the object (they walk the element list globals
 *     or program the light registers), so a receiver exists only to be
 *     passed, not used. Jump2Popup_ElemDtor and Jump2Popup_ElemCtor are
 *     the shared empty element destructor and constructor --
 *     dScMgTrampoline2_c's factory and array cleanup use them too -- and
 *     are registered through the C __cxa_vec_ctor boundary.
 *   - pad_5004 stays bytes: the state functions copy the whole eight-byte
 *     PMF record into it with a struct assign, and Behavior calls through
 *     it. See the header's state-machine note.
 *   - The func_ov006_020cNNN and func_ov004_* externs are foreign callees
 *     (the board objects' TUs and the minigame base), not this TU's own.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_jump2_AssetId {
    ASSET_D_2D_MG_JUMP_NCL_BIN = 0x00d1,
    ASSET_JUMP_BACK01_BMD = 0x0203,
    ASSET_JUMP_BACK02_BMD = 0x0204
};

#include "dScMgJump2_c.h"
#include "Model.h"
#include "SharedFilePtr.h"
#include "types.h"
#include "decl_common.h"
#include "Particle__System.h"

/* The list link at the head of each Jump2Popup. Its two methods link `this` into
 * and back out of the live list at sJump2PopupList. */
struct Jump2PopupLink {
    struct Jump2PopupLink *next;
    void Link();   /* push */
    void Unlink();   /* remove */
};

/* The 0x24-byte entries of mArray3, read back from what the functions below
 * do with them. Local because nothing names the type yet. */
struct Jump2Popup {
    struct Jump2Popup *next; /* 0x00 -- the live list */
    int   x, y;             /* 0x04 -- 20.12 position */
    int   targetX, targetY; /* 0x0c */
    int   speedX, speedY;   /* 0x14 */
    short life;             /* 0x1c -- frames left */
    short sprite;           /* 0x1e */
    short active;           /* 0x20 */
    short _22;

    void Spawn(int startX, int startY, int spriteId);  /* spawn */
    void Tick();                                      /* update */
    void DrawNumber();                                      /* draw   */
};

/* Whole-matrix copies go through this so they stay one block move. */
struct M48 { int w[12]; };

/* Three words as an array, not three ints: as separate members the copy
 * in Render is scalarised and grows by 12 bytes. */
struct S3 { int w[3]; };

struct Vector3_16f;

/* The state callback stored at 0x5004 (pad_5004 in the header). */
typedef void (dScMgJump2_c::*dScMgJump2_cState)();

/* The same eight bytes as a plain pair: the state functions copy the next
 * state into 0x5004 whole. */
struct G2 { int w[2]; };

/* Vtable shim for InitResources' last call, a virtual dispatch through slot
 * 18 of this class's own table. Calling StartRound by name would compile
 * to a direct branch instead. */
struct Base {
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
    virtual void m48(int x);
};

/* File-scope objects at the end of this file construct the two model
 * handles (files 0x203 and 0x204). mwcc emits __sinit_d_s_mg_jump2.cpp
 * from those definitions. The wrapper name is local; the handle
 * constructor and destructor are the ROM resource-family functions,
 * aliased in the manifest. */
struct Jump2ModelFile : SharedFilePtr {
    u32 words[2];

    Jump2ModelFile(u32 fileID);
    ~Jump2ModelFile();
};

extern Jump2ModelFile gOV006JumpBack01ModelFile; /* model file 0x203 */
extern Jump2ModelFile sJump2Back02ModelFile;     /* model file 0x204 */

extern "C" {
extern void func_0203d630(int* p, int m);
extern struct Jump2PopupLink *sJump2PopupList;
extern int sJump2PopupCount;
extern struct Jump2Popup *sJump2Popups;
extern void *sJump2BackFile;
void Mg_CommitRecords(void);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern int data_020a0e68;
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void Jump2Popup_UpdateAll(void);
extern void* Jump3D_AllIdle(void*);
extern void* Heyho_AllIdle(void*);
extern void Mg_SubmitScore(int);
extern void Mg_SetSharedState(int);
extern void Heyho_SeparateAll(void);
extern void Jump3D_UpdateAll(void);
extern int sJumpComboCount;
extern struct G2 pmf_dScMgJump2_c_State_Idle;
extern struct G2 pmf_dScMgJump2_c_State_End;
extern void Heyho_SettleAll(void);
extern void Jump3D_SetTuning(int a);
extern int sHeyhoUnsettledCount;
extern int sHeyhoCount;
extern int sJump3DMariosLeft;
extern struct G2 pmf_dScMgJump2_c_State_Play;
extern void Jump3D_StartRoundLayout(void);
extern struct G2 pmf_dScMgJump2_c_State_Result;
extern struct G2 pmf_dScMgJump2_c_State_Intro;
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, int dst);
extern void Camera_UpdateMatrices(void *cam);
extern void Jump3D_SetLowGravity(void);
extern void Heyho_ReactAll(void);
extern int Mg_GetTopScreenYOffset(void);
extern int Heyho_InitPool(void *ptr, int n);
extern int Jump3D_InitPool(void *base, int count);
extern u8 data_0209d45c;
extern s16 data_02082614;
}

namespace GX   { void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size); }
namespace G3X  { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { s32 fdiv(s32 a, s32 b); }
short ApproachLinear2(short &value, short target, short step);
int   ApproachLinear(int &value, int target, int step);

extern "C" {  /* .c-derived member: C linkage for the whole block */
void *_ZN11dScMgBase_cC2Ev(void *p);
void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                    void (*ctor)(void *), void (*dtor)(void *));
void _ZN8Particle10SysTrackerC1Ev(void *p);
void *_ZN5ModelC1Ev(void *p);
extern char _ZTV14dScMgD3DBase_c[];
dMgJump3DMario_c *_ZN16dMgJump3DMario_cD1Ev(dMgJump3DMario_c *object);
dMgJump3DMario_c *_ZN16dMgJump3DMario_cC1Ev(dMgJump3DMario_c *object);
int Heyho_Construct(char *object);

// @symbol Jump2Popup_ElemCtor
/* The empty constructor of mArray3's 0x24-byte elements, the partner of
 * the shared empty destructor Jump2Popup_ElemDtor. The factory below hands
 * the pair to __cxa_vec_ctor; dScMgTrampoline2_c's factory reuses both.
 * Written above the factory because this TU emits .text in reverse source
 * order, and the ROM has it directly after the factory. */
void Jump2Popup_ElemCtor(void *object)
{
}

/* The array runtime passes each element address and ignores callback results.
 * The casts below adapt the existing lifecycle entries at that ABI boundary. */
// @symbol dScMgJump2_c_classInit
/* Builds the scene by hand (operator new, base constructor, vtables, member
 * construction) because the class has no constructor declared yet. The empty
 * loop steps over the two 0xbc-byte cameras, which have no constructor.
 * Historical alias: dScMgJump2_c_Spawn. */
void *dScMgJump2_c_classInit()
{
    char *raw = (char *)_ZN7fBase_cnwEj(0x5a78);
    if (raw) {
        char *e;
        char *end;
        _ZN11dScMgBase_cC2Ev(raw);
        *(char **)raw = _ZTV14dScMgD3DBase_c;
        *(u16 *)(raw + 0x4664) = 0;
        e = raw + 0x466c;
        end = raw;
        end = end + 0x47e4;
        do {
            e += 0xbc;
        } while (e != end);
        _ZN8Particle10SysTrackerC1Ev(raw + 0x47e4);
        *(char **)raw = (char *)&_ZTV12dScMgJump2_c[2];
        __cxa_vec_ctor(raw + 0x500c, 3, 0xb8, (void (*)(void *))_ZN16dMgJump3DMario_cC1Ev, (void (*)(void *))_ZN16dMgJump3DMario_cD1Ev);
        __cxa_vec_ctor(raw + 0x5234, 6, 0xf0, (void (*)(void *))Heyho_Construct, (void (*)(void *))Heyho_Destruct);
        __cxa_vec_ctor(raw + 0x57d4, 0x10, 0x24, Jump2Popup_ElemCtor, Jump2Popup_ElemDtor);
        _ZN5ModelC1Ev(raw + 0x5a14);
    }
    return raw;
}
}

// @symbol _ZN12dScMgJump2_c10StartRoundEi
/* Slot 18: resets the round and arms the start countdown. */
void dScMgJump2_c::StartRound(int /* arg */)
{
  Jump3D_DisableInput();
  Jump3D_Arrange(0, 0);
  sHeyhoCount = 3;
  Heyho_SpawnAll(0, 0);
  Jump2Popup_ClearAll();
  BeginIntro();
}

// @symbol _ZN12dScMgJump2_c11CanEndStateEi
/* Slot 19. */
int dScMgJump2_c::CanEndState(int /* mode */)
{
    Puff_ClearAll(0, 0);
    return 1;
}

// @symbol _ZN12dScMgJump2_c13InitResourcesEv
/* Slot 0. The two camera blocks at 0x466c and 0x4728 are untyped padding
 * in dScMgD3DBase_c.h, so they are still passed by offset. */
s32 dScMgJump2_c::InitResources()
{
    char *base = (char *)this;
    int tiles, palette;
    s32 fov;
    void *bmdFile;

    tiles = LoadFile(sJump2NcgByLanguage[GetGameLanguage()]);
    palette = LoadFile(ASSET_D_2D_MG_JUMP_NCL_BIN);
    DecompressLZ16(tiles, 0x6400000);
    GX::LoadOBJPltt((const void *)palette, 0, 0x80);
    Deallocate((void *)tiles);
    Deallocate((void *)palette);

    data_0209d45c = 0x11;
    G3X::SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = cstd::fdiv(0xc0000, (s32)data_02082614);

    this->mCamera0EyeX = 0;
    this->mCamera0EyeY = -0x64000;
    this->mCamera0EyeZ = 0;
    this->mCamera0TargetX = 0;
    this->mCamera0TargetY = 0;
    this->mCamera0TargetZ = fov;
    this->mCamera0Angle = 0x800;
    Camera_UpdateMatrices(base + 0x466c);

    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x800;
    Camera_UpdateMatrices(base + 0x4728);

    if (Heyho_InitPool(mArray2, 6) == 0)
        return 0;

    Jump2Popup_InitPool((int)mArray3, 0x10);

    sHeyhoScorePopups = 1;
    if (Jump3D_InitPool(mPlayers, 3) == 0)
        return 0;

    sJump2BackFile = &sJump2Back02ModelFile;
    bmdFile = Model::LoadFile(*(SharedFilePtr *)&sJump2Back02ModelFile);

    if (((Model *)mModel)->SetFile((BMD_File *)bmdFile, 1, -1) == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(struct M48 *)&((Model *)mModel)->mat4x3 = *(struct M48 *)&data_020a0e68;
        return 0;
    }

    Mg_SetTopScreenYOffset(0x10);
    ((Base *)this)->m48(-1);
    return 1;
}

/* The state functions: five are the exec targets the PMF records at
 * pmf_dScMgJump2_c_State_Idle..9c point into (see the header's state-machine note);
 * the other three are the transitions that install a record and arm
 * mStateTimer. mAnimPhase accumulates mAnimRate >> 12 each tick and wraps at
 * 0x1000.
 *
 * This one arms the countdown and installs the next state. */
// @symbol _ZN12dScMgJump2_c10BeginIntroEv
void dScMgJump2_c::BeginIntro(){
    mStateTimer = 0x78;
    mAnimPhase = 0;
    mAnimRate = 0x4000;
    *(struct G2 *)pad_5004 = pmf_dScMgJump2_c_State_Intro;
}

// @symbol _ZN12dScMgJump2_c11State_IntroEv
void dScMgJump2_c::State_Intro()
{
    s16 *countdown = &mStateTimer;
    *countdown = *countdown - 1;
    if (mStateTimer == 0) {
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        Jump3D_StartRoundLayout();
        BeginPlay();
    }
    Heyho_SeparateAll();
}

// @symbol _ZN12dScMgJump2_c9BeginPlayEv
void dScMgJump2_c::BeginPlay()
{
    mStateTimer = 0xf0;
    *(struct G2 *)pad_5004 = pmf_dScMgJump2_c_State_Play;
}

// @symbol _ZN12dScMgJump2_c10State_PlayEv
void dScMgJump2_c::State_Play()
{
    s16 done;
    int prevScore = sJumpComboCount;
    int *acc = &mAnimPhase;
    int mode;

    *acc += mAnimRate >> 12;
    if (mAnimPhase > 0x1000)
        *acc -= 0x1000;

    done = 0;
    mode = sHeyhoUnsettledCount;
    switch (mode) {
    case 0:
        done = 1;
        break;
    case 1:
        {
        int rc = ApproachLinear2(mStateTimer, done, 2);
        if (rc)
            done = 1;
        }
        break;
    default:
        {
        int rc = ApproachLinear2(mStateTimer, done, 1);
        if (rc)
            done = 1;
        }
        break;
    }

    if (done != 0 && sHeyhoUnsettledCount < sHeyhoCount) {
        int value;
        s16 countdown;
        Heyho_SettleAll();
        mStateTimer = 0xf0 - sJumpComboCount * 6;
        countdown = mStateTimer;
        if (countdown < 0x3c)
            countdown = 0x3c;
        mStateTimer = countdown;
        value = (sJumpComboCount << 4) + 0x1600;
        if (value > 0x2200)
            value = 0x2200;
        Jump3D_SetTuning(value);
    }

    Heyho_SeparateAll();
    Jump3D_UpdateAll();

    if (prevScore != sJumpComboCount) {
        switch (sJumpComboCount) {
        case 3:
            sHeyhoCount = 4;
            mPromptEnabled = 0;
            break;
        case 0xa:
            sHeyhoCount = 5;
            break;
        case 0x14:
            sHeyhoCount = 6;
            break;
        }
        Mg_SubmitScore(sJumpComboCount);
    }

    if (sJump3DMariosLeft != 0)
        return;
    BeginEnd();
}

// @symbol _ZN12dScMgJump2_c8BeginEndEv
void dScMgJump2_c::BeginEnd()
{
    mStateTimer = 0x20;
    *(struct G2 *)pad_5004 = pmf_dScMgJump2_c_State_End;
}

// @symbol _ZN12dScMgJump2_c9State_EndEv
void dScMgJump2_c::State_End()
{
    int *acc = &mAnimPhase;
    *acc += mAnimRate >> 12;
    if (mAnimPhase > 0x1000) {
        *acc -= 0x1000;
    }
  Heyho_SeparateAll();
  Jump3D_UpdateAll();
  if (ApproachLinear2(mStateTimer, 0, 1) == 0) {
      return;
  }
  Jump3D_SetLowGravity();
  Heyho_ReactAll();
  *(struct G2 *)pad_5004 = pmf_dScMgJump2_c_State_Result;
}

// @symbol _ZN12dScMgJump2_c12State_ResultEv
void dScMgJump2_c::State_Result(){
    void *obj = Jump3D_AllIdle(this);
    if(obj){
        obj = Heyho_AllIdle(obj);
        if(obj){
            Mg_SubmitScore(sJumpComboCount);
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            mPromptEnabled = 0;
            *(struct G2 *)pad_5004 = pmf_dScMgJump2_c_State_Idle;
        }
    }
    Heyho_SeparateAll();
    Jump3D_UpdateAll();
}

// @symbol _ZN12dScMgJump2_c10State_IdleEv
void dScMgJump2_c::State_Idle()
{
}

// @symbol _ZN12dScMgJump2_c8BehaviorEv
/* Slot 6. mParticleId is a particle system's unique ID, handed to
 * NewUnkCallback818 and back each frame. The object FromUniqueID returns is
 * untyped, so its fields at 0x50 and 0x58 stay raw. Both Particle::System
 * calls keep their mangled spellings: declaring the real Fix12<int>
 * parameters changes how the call passes them. */
s32 dScMgJump2_c::Behavior()
{
    mParticleId = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        mParticleId, 0xf0, 0x400000, 0x800000, -0x480000, 0);
    void *system = Particle::System::FromUniqueID(mParticleId);
    if (system != 0) {
        *(int *)((char *)system + 0x50) = 0x4000;
        *(unsigned char *)((char *)system + 0x58) = 0x2c;
    }
    Jump2Popup_UpdateAll();
    {
        dScMgJump2_cState *pp = (dScMgJump2_cState *)pad_5004;
        (this->**pp)();
    }
    return 1;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_0203cd80(int* m, int x);

void Jump2_SetLights(void) {
    int light0[3];
    int light1[3];
    *(volatile int*)0x40004cc = 0x7fff;
    *(volatile int*)0x40004cc = 0x40007fff;
    light0[0] = 0;
    light0[1] = 0;
    light0[2] = 0xfffff008;
    func_0203cd80(light0, 0x2000);
    func_0203ccd4((struct Vector3 *)light0, 0);
    *(volatile int*)0x40004c8 =
        (((short)light0[0] >> 3) & 0x3ff) |
        ((((short)light0[1] >> 3) & 0x3ff) << 10) |
        ((((short)light0[2] >> 3) & 0x3ff) << 20);
    light1[2] = 0xfffff008;
    light1[0] = 0;
    light1[1] = 0;
    *(volatile int*)0x4000440 = 2;
    *(volatile int*)0x4000454 = 0;
    func_0203cd80(light1, 0x2000);
    func_0203ccd4((struct Vector3 *)light1, 0);
    *(volatile int*)0x40004c8 =
        ((((short)light1[0] >> 3) & 0x3ff) |
        ((((short)light1[1] >> 3) & 0x3ff) << 10) |
        ((((short)light1[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
}
}

// @symbol _ZN12dScMgJump2_c6RenderEv
/* Slot 9. mModel is raw bytes in the header (the destructor has to destroy
 * it before the arrays), so it is cast to Model here. The component block
 * at mModel + 8 is reached by offset: naming it through Model changes the
 * code. */
s32 dScMgJump2_c::Render()
{
    struct S3 scale;
    void *anim;
    int count;
    int x;
    int i;

    Mg_DrawTopRightCounter(sJumpComboCount, 4);
    Jump2Popup_DrawAll();
    Jump2_SetLights();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0xffead000, -0xa0000);
    *(struct M48 *)&((Model *)mModel)->mat4x3 = *(struct M48 *)&data_020a0e68;

    scale = *(struct S3 *)&sJump2RenderScale;

    {
        int *obj = (int *)(mModel + 8);
        anim = (void *)obj[1];
        func_02045e44((void *)obj, 1, 0);
    }
    *(int *)((char *)anim + 0x18) = mAnimPhase;
    {
        Model *model = (Model *)mModel;
        model->Render((Vector3 *)&scale);
    }

    Jump3D_DrawAll();
    Heyho_DrawAll();

    if (mDrawSubScreen == 1) {
        x = 0x6e;
        count = sJump3DMariosLeft;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)sJump2TextSpritesByLanguage[idx][1], (void *)x, 0xc,
                              (void *)0);
            } else {
                RenderOamMainScreen(sJumpLifeIconSprite, x, 0xc, -1, -1);
            }
            x += 0x12;
        }
    }

    return 1;
}

// @symbol _ZN12dScMgJump2_c16CleanupResourcesEv
/* Slot 3. sJump2BackFile points at the model's file handle; it is
 * released and cleared. */
s32 dScMgJump2_c::CleanupResources()
{
    ((SharedFilePtr *)sJump2BackFile)->Release();
    sJump2BackFile = 0;
    Mg_CommitRecords();
    return 1;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_InitPool(int table, int count){
    sJump2Popups = (struct Jump2Popup *)table;
    sJump2PopupCount = count;
    data_ov006_021421b4 = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_Spawn(int x, int y, int sprite)
{
    int i;
    for (i = sJump2PopupCount - 1; i >= 0; i--) {
        if (sJump2Popups[i].active == 0) {
            sJump2Popups[i].Spawn(x, y, sprite);
            return;
        }
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_ClearAll(void){
    int i;
    for (i = 0; i < sJump2PopupCount; i++) {
        sJump2Popups[i].active = 0;
    }
    sJump2PopupList = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_UpdateAll(void)
{
    int i = 0;
    if (i < sJump2PopupCount) {
        int offset = 0;
        do {
            ((Jump2Popup *)((char *)sJump2Popups + offset))->Tick();
            i++;
            offset += 0x24;
        } while (i < sJump2PopupCount);
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_DrawAll(void) {
    void *node = sJump2PopupList;
    if (node == 0) return;
    do {
        ((Jump2Popup *)node)->DrawNumber();
        node = *(void **)node;
    } while (node != 0);
}
}

// @symbol _ZN14Jump2PopupLink4LinkEv
void Jump2PopupLink::Link() {
    *(void **)this = sJump2PopupList;
    sJump2PopupList = this;
}

// @symbol _ZN14Jump2PopupLink6UnlinkEv
void Jump2PopupLink::Unlink() {
    struct Jump2PopupLink *prev = sJump2PopupList;
    if (prev == this) {
        sJump2PopupList = this->next;
        return;
    }
    struct Jump2PopupLink *cur = prev->next;
    while (cur) {
        if (cur == this) {
            prev->next = this->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

// @symbol _ZN10Jump2Popup5SpawnEiii
void Jump2Popup::Spawn(int startX, int startY, int spriteId) {
    x = startX;
    y = startY;
    speedX = 0;
    speedY = 0x4000;
    targetX = startX;
    targetY = startY - 0x40000;
    if (targetY < 0x8000) {
        int limit = Mg_GetTopScreenYOffset() << 12;
        if (targetY > -limit) {
            targetY = 0x8000;
            goto done;
        }
    }
    if (targetY >= -0xb8000 - (Mg_GetTopScreenYOffset() << 12))
        goto done;
    targetY = -0xb8000 - (Mg_GetTopScreenYOffset() << 12);
done:
    life = 0x80;
    active = 1;
    sprite = (short)spriteId;
    ((Jump2PopupLink *)this)->Link();
}

// @symbol _ZN10Jump2Popup4TickEv
void Jump2Popup::Tick() {
    if (active == 0) return;
    if (ApproachLinear2(life, 0, 1) != 0) {
        ((Jump2PopupLink *)this)->Unlink();
        active = 0;
    }
    ApproachLinear(x, targetX, speedX);
    ApproachLinear(y, targetY, speedY);
    func_0203d630(&speedX, 0xf00);
}

// @symbol _ZN10Jump2Popup10DrawNumberEv
void Jump2Popup::DrawNumber() {
    if (active == 0) return;
    Mg_DrawBigNumber(x >> 12, y >> 12,
                        sprite, -1, -1, 0, data_ov006_021421b4);
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void Jump2Popup_ElemDtor(void *object)
{
}
}

// @symbol _ZN12dScMgJump2_cD0Ev
/* Generated from the destructor below. */

// @symbol _ZN12dScMgJump2_cD1Ev
/* Defined in the class body in include/dScMgJump2_c.h, which explains why.
   mwccarm emits D1 and D0 from it, in this order. */

/* Static-init globals (was the handwritten __sinit_ov006_02130f00 shard).
 * Definition order is the retail initializer's construction order: the
 * 0x203 model handle, then the 0x204 model handle. */
Jump2ModelFile gOV006JumpBack01ModelFile(ASSET_JUMP_BACK01_BMD);
Jump2ModelFile sJump2Back02ModelFile(ASSET_JUMP_BACK02_BMD);
