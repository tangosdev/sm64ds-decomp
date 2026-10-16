//cpp
/* ov006/dScMgJump_c -- all 18 of the class's functions
 * (.text 0x020edec0..0x020eeafc).
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. The destructor is inline in the header, which makes
 * InitResources the key function that emits the vtable.
 *
 * leftovers:
 * - mLevel (dScMgBase_c) and mDrawSubScreen (dScMgD3DBase_c) are read here
 *   as the `mLevel % 5` egg check and the Render flag test.
 * - `base + 0x466c` / `base + 0x4728` still address the two per-minigame
 *   cameras by offset: include/dScMgD3DBase_c.h already calls them camera
 *   elements and records "typing it as a real array is its own change".
 * - struct C must stay a non-polymorphic stand-in for the state PMF at
 *   0x5004: mwccarm lays out a pointer-to-member of a polymorphic class
 *   differently, so Behavior cannot dispatch through dScMgJump_c itself.
 * - pmf_dScMgJump_c_State_Result..0213cb84 are the {fn, 0} state rows. g_profile_MG_JUMP
 *   sits between 0213cb5c and 0213cb6c. data_ov006_0213cb48 and
 *   data_ov006_0213cb8c stay the ROM copy. gMgScene is the ov004
 *   shared scene data, read raw at +0xb4/+0xb8 because its type is not recovered.
 * - the func_ov006_020c and func_ov004 externs are other TUs' workers; they
 *   keep address names until their owners name them.
 */

#include "dScMgJump_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "types.h"

/* `C` stands in for the receiver of the state pointer-to-member at 0x5004.
 * It must stay non-polymorphic: mwccarm lays out a pointer-to-member of a
 * polymorphic class differently, so dScMgJump_c itself cannot be used. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x5004]; PMF m; };

/* Whole-matrix copies go through this so they stay one block move. */
struct Mtx43 { int m[12]; };
struct V3 { int w[3]; };

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

/* The two model file handles InitResources swaps between through
 * data_ov006_02142184; a declared-only subclass so the static initializer
 * names the real entry points. */
struct MgJumpModelFilePtr : SharedFilePtr {
    MgJumpModelFilePtr(u32 fileID);
    ~MgJumpModelFilePtr();
};
extern MgJumpModelFilePtr data_ov006_02142188;
extern MgJumpModelFilePtr data_ov006_02142190;

/* Everything below that decl_common.h does not declare. Keep it above the
 * first function: tools/tiers.py charges text between two functions to the
 * earlier one. */
extern "C" {
void Mg_CommitRecords(void);
void Mg_DrawScoreTally(void *a, int b, int c, int d);
int  GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_02012790(int a0);
void FreeGfxSlotsById(int arg);
int  LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Camera_UpdateMatrices(void *cam);

void Heyho_ReactAll(void);
void Heyho_SeparateAll(void);
void Jump3D_UpdateAll(void);
void Jump3D_SetLowGravity(void);
void Jump3D_StopGravity(void);
void Jump3D_StartRoundLayout(void);
int  Jump3D_AllIdle(void);
int  Heyho_AllIdle(void);
int  Heyho_InitPool(char *ptr, int n);
int  Jump3D_InitPool(char *base, int count);
void Jump3D_EnableInput(void);
int  RandomIntInternal(int *seed);

extern int  data_020a0e68;
extern int  data_0209e650;
extern int  data_ov006_0213cb48;
extern u8   data_0209d45c;
extern s16  data_02082614;
extern int  data_ov006_02140418;
extern int  data_ov006_02140304;
extern int  data_ov006_02140434;

/* The state rows are raw {fn, 0} words. A pointer-to-member initializer
 * would emit a static initializer this overlay does not have, so each
 * function is named by the mangled spelling the relocation already uses. */
void _ZN11dScMgJump_c10State_IdleEv(void);
void _ZN11dScMgJump_c11State_FinalEv(void);
void _ZN11dScMgJump_c12State_ResultEv(void);
void _ZN11dScMgJump_c9State_EndEv(void);
void _ZN11dScMgJump_c10State_PlayEv(void);
void _ZN11dScMgJump_c11State_IntroEv(void);
void *dScMgJump_c_classInit(void);

dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Result = {
    (s32)_ZN11dScMgJump_c12State_ResultEv, 0};
dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Intro = {
    (s32)_ZN11dScMgJump_c11State_IntroEv, 0};
dScMgJump_c::StatePmf pmf_dScMgJump_c_State_End = {
    (s32)_ZN11dScMgJump_c9State_EndEv, 0};

/* MG_JUMP's registry profile: the factory, then the same priority in both
 * halfwords. Eight bytes, between the third state row and the fourth. */
struct MgJumpProfile {
    void *(*spawn)(void);
    s16 behaviorPriority;
    s16 renderPriority;
};
typedef char MgJumpProfile_size_must_be_8[
    sizeof(MgJumpProfile) == 8 ? 1 : -1];
MgJumpProfile g_profile_MG_JUMP = {
    dScMgJump_c_classInit, 0x174, 0x174
};

dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Final = {
    (s32)_ZN11dScMgJump_c11State_FinalEv, 0};
dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Idle = {
    (s32)_ZN11dScMgJump_c10State_IdleEv, 0};
dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Idle_2 = {
    (s32)_ZN11dScMgJump_c10State_IdleEv, 0};
dScMgJump_c::StatePmf pmf_dScMgJump_c_State_Play = {
    (s32)_ZN11dScMgJump_c10State_PlayEv, 0};
}

namespace GX   { void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size); }
namespace G3X  { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { s32 fdiv(s32 a, s32 b); }
short ApproachLinear2(short &value, short target, short step);

// @symbol _ZN11dScMgJump_c10StartRoundEi
/* Slot 18. 0 is an eat (one more egg, capped like the HUD score), 0x12 a
 * miss that clears the count and the shared scene's copy of it; anything
 * else just publishes the count. From the fifteenth egg on the next egg is
 * drawn at random, redrawn while it repeats the previous draw.
 *
 * The draw is written `if (mLevel < 15) idx = mLevel; else { redraw }`
 * rather than `idx = mLevel; if (idx >= 15) { redraw }`. The two are the
 * same code to a reader, but only the first reproduces the ROM: with the
 * guard directly ahead of the loop mwccarm hoists the seed address, the
 * previous-draw address and the 10 into r6-r8 and the function comes out a
 * word short. The if-converted then-arm sits between the guard and the loop
 * header, which keeps the three in the loop body, as the cartridge has them. */
void dScMgJump_c::StartRound(int arg)
{
    u32 idx;

    if (arg == 0) {
        mLevel = mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
    } else if (arg == 0x12) {
        mLevel = 0;
        if (mLevel > 0x270e) mLevel = 0x270e;
        if (gMgScene != 0)
            gMgScene->mHudScore = 0;
        Mg_SubmitScore(mHudScore);
    } else {
        u32 count = mLevel;
        if (gMgScene != 0)
            gMgScene->mHudScore = count;
    }

    if (mLevel != 0)
        Jump3D_DisableInput();
    else
        Jump3D_EnableInput();

    if (mLevel < 0xf) {
        idx = mLevel;
    } else {
        do {
            idx = (int)((u32)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xa >> 0xc;
        } while (idx == (u32)data_ov006_0213cb48);
        data_ov006_0213cb48 = idx;
    }

    Jump3D_Arrange(mLevel, idx);

    if (mLevel < 0xf)
        data_ov006_02140328 = 3;
    else if (mLevel < 0x14)
        data_ov006_02140328 = 4;
    else if (mLevel < 0x19)
        data_ov006_02140328 = 5;
    else
        data_ov006_02140328 = 6;

    Heyho_SpawnAll((void *)mLevel, idx);
    BeginIntro();
}

// @symbol _ZN11dScMgJump_c11CanEndStateEi
/* Slot 19. mStateTimer is the same countdown the state functions below use. */
int dScMgJump_c::CanEndState(int sel)
{
    if (sel == 0) {
        if (mLevel % 5 == 4) {
            if (ApproachLinear2(mStateTimer, 0, 1) != 0) {
                if (data_ov006_02140428 < 3) {
                    Jump3D_RestoreCount();
                    func_02012790(0x26);
                    mStateTimer = 0x3c;
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
    Puff_ClearAll(0, 0);
    return 1;
}

// @symbol _ZN11dScMgJump_c13InitResourcesEv
/* Slot 0, and the key function (see the banner). The two camera blocks at
 * 0x466c and 0x4728 are untyped padding in dScMgD3DBase_c.h, so they are
 * still passed by offset. */
s32 dScMgJump_c::InitResources()
{
    char *base = (char *)this;
    s32 idx;
    int buf1, buf2;
    s32 fov;
    void *modelFile;
    int r;

    idx = GetGameLanguage();
    buf1 = LoadFile(data_ov006_0213cbc8[idx]);
    buf2 = LoadFile(0xd1);
    DecompressLZ16((void *)buf1, (void *)0x6400000);
    GX::LoadOBJPltt((void *)buf2, 0, 0x80);
    Deallocate((void *)buf1);
    Deallocate((void *)buf2);

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

    if (Heyho_InitPool((char *)mArray2, 6) == 0)
        return 0;
    if (Jump3D_InitPool((char *)mPlayers, 3) == 0)
        return 0;

    if (param1 & 1) {
        data_ov006_02142184 = &data_ov006_02142188;
    } else {
        data_ov006_02142184 = &data_ov006_02142190;
    }
    modelFile = Model::LoadFile(*(SharedFilePtr *)data_ov006_02142184);
    r = mModel.SetFile((BMD_File *)modelFile, 1, -1);
    if (r == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(struct Mtx43 *)&mModel.mat4x3 = *(struct Mtx43 *)&data_020a0e68;
        return 0;
    }
    Mg_SetTopScreenYOffset(0x10);
    ((Base *)this)->m48(-1);
    return 1;
}

// @symbol _ZN11dScMgJump_c10BeginIntroEv
/* The state functions below are unnamed in the ROM. mState is the current
 * state (see Behavior), mAccum gains mAccumStep >> 12 each tick and
 * mStateTimer counts down to the change.
 *
 * This one arms the countdown; its only caller is StartRound. */
void dScMgJump_c::BeginIntro()
{
    mStateTimer = 0x78;
    mAccum = 0;
    mAccumStep = 0x4000;
    mState = pmf_dScMgJump_c_State_Intro;
}

// @symbol _ZN11dScMgJump_c11State_IntroEv
/* Counts down mStateTimer, then shows the prompt and moves to the next state.
 * The legacy file's `#pragma optimize_for_size on` is not needed here and
 * would leak into every function in this file if carried. */
void dScMgJump_c::State_Intro()
{
    int left;
    short *countdown = &mStateTimer;
    *countdown = *countdown - 1;
    left = mStateTimer;
    if (left == 0) {
        FreeGfxSlotsById(0x1d);
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        Jump3D_StartRoundLayout();
        BeginPlay();
    } else if (left == 0x77) {
        Mg_ShowBanner(0xd, 0x80, 0x60, 1, -1, 0xd);
    }
    Heyho_SeparateAll();
}

// @symbol _ZN11dScMgJump_c9BeginPlayEv
void dScMgJump_c::BeginPlay()
{
    mState = pmf_dScMgJump_c_State_Play;
}

// @symbol _ZN11dScMgJump_c10State_PlayEv
void dScMgJump_c::State_Play()
{
    int *acc = &mAccum;
    *acc += mAccumStep >> 12;
    if (mAccum > 0x1000) {
        *acc -= 0x1000;
    }
    Heyho_SeparateAll();
    Jump3D_UpdateAll();
    if(data_ov006_02140428 == 0){
        BeginEnd();
        return;
    }
    if(data_ov006_02140304 != 0)
        return;
    BeginFinal();
}

// @symbol _ZN11dScMgJump_c8BeginEndEv
void dScMgJump_c::BeginEnd()
{
    mStateTimer = 0x20;
    mState = pmf_dScMgJump_c_State_End;
}

// @symbol _ZN11dScMgJump_c9State_EndEv
void dScMgJump_c::State_End()
{
  int *acc = &mAccum;
  *acc += mAccumStep >> 12;
  if (mAccum > 0x1000)
  {
    *acc -= 0x1000;
  }
  Heyho_SeparateAll();
  Jump3D_UpdateAll();
  if (ApproachLinear2(mStateTimer, 0, 1) == 0)
  {
    return;
  }
  Jump3D_SetLowGravity();
  Heyho_ReactAll();
  mState = pmf_dScMgJump_c_State_Result;
}

// @symbol _ZN11dScMgJump_c12State_ResultEv
void dScMgJump_c::State_Result()
{
    int ready = Jump3D_AllIdle();
    if (ready) {
        ready = Heyho_AllIdle();
        if (ready) {
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            mPromptEnabled = 0;
            mStateTimer = 0;
            mState = pmf_dScMgJump_c_State_Idle;
        }
    }
    Heyho_SeparateAll();
    Jump3D_UpdateAll();
}

// @symbol _ZN11dScMgJump_c10BeginFinalEv
void dScMgJump_c::BeginFinal()
{
    Jump3D_StopGravity();
    mState = pmf_dScMgJump_c_State_Final;
}

// @symbol _ZN11dScMgJump_c11State_FinalEv
/* Its writes to 0x5004 and 0x5014 are what bound dScMgD3DBase_c's size
 * from above. `shared` is the ov004 scene data; its type is not recovered. */
void dScMgJump_c::State_Final()
{
  mAccum += mAccumStep >> 12;
  mAccumStep += 0x600;
  Jump3D_UpdateAll();
  int same = (int)(data_ov006_02140434 == data_ov006_02140418);
  if(same == 0) return;
  dScMgBase_c *shared = gMgScene;
  if(shared != 0){
    if(shared->mHudScore < 0x270f){
      shared->mHudScore += 1;
    }
    if(shared->mHudScore > shared->mBestHudScore){
      shared->mBestHudScore = shared->mHudScore;
    }
  }
  Mg_SubmitScore(mHudScore);
  Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
  mPromptEnabled = 0;
  mStateTimer = 0;
  mState = pmf_dScMgJump_c_State_Idle_2;
}

// @symbol _ZN11dScMgJump_c10State_IdleEv
/* An empty state. */
void dScMgJump_c::State_Idle()
{
}

// @symbol _ZN11dScMgJump_c8BehaviorEv
/* Slot 6: run the current state. */
s32 dScMgJump_c::Behavior()
{
    C *c = (C *)this;
    (c->*c->m)();
    return 1;
}

// @symbol _ZN11dScMgJump_c6RenderEv
/* Slot 9. Draws the instruction rows while mDrawSubScreen == 1, then programs the
 * light vector and light color registers at 0x040004c8 and 0x040004cc
 * (lights 0 and 1, hence volatile) and renders the model. The row
 * sprites and the ov006 draw helpers are unnamed in symbols.txt. */
s32 dScMgJump_c::Render()
{

    if (mDrawSubScreen == 1) {
        int count;
        int y;
        int i;

        Mg_DrawScoreTally(this, 0xe0, 0x14, 1);

        count = data_ov006_02140428;
        y = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213cbb4[idx][1], (void *)y, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, y, 0xc, -1, -1);
            }
            y += 0x12;
        }
    }

    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            (((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20);
        *(volatile int *)0x40004cc = 0x7fff;
        *(volatile int *)0x40004cc = 0x40007fff;
        Matrix4x3_FromTranslation(&data_020a0e68, 0, -0xef000, 0);
    }
    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            ((((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
    }

    *(struct Mtx43 *)&mModel.mat4x3 = *(struct Mtx43 *)&data_020a0e68;

    {
        V3 t;
        void *obj;
        void *p;

        t = *(V3 *)data_ov006_0213cb8c;

        obj = &mModel.data;
        p = *(void **)((char *)obj + 4);
        func_02045e44(obj, 1, 0);
        *(int *)((char *)p + 0x18) = mAccum;

        {
            Model *model = &mModel;
            model->Render((Vector3 *)&t);
        }
    }

    Jump3D_DrawAll();
    Heyho_DrawAll();

    return 1;
}

// @symbol _ZN11dScMgJump_c16CleanupResourcesEv
/* Slot 3. Releases the model file; `this` is never read. */
s32 dScMgJump_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov006_02142184)->Release();
    data_ov006_02142184 = 0;
    Mg_CommitRecords();
    return 1;
}

/* The destructor (D1 at 0x020edec0, D0 at 0x020edf54) is defined inline in
 * dScMgJump_c.h; defining it here would emit D0 before D1 and break the
 * ROM order. */

/* The scene's model file handles, registered in file-ID order; file 0x203
 * sits at 0x02142190, file 0x204 at 0x02142188. */
MgJumpModelFilePtr data_ov006_02142190(0x203);
MgJumpModelFilePtr data_ov006_02142188(0x204);
