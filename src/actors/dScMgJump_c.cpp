//cpp
/* ov006/dScMgJump_c -- all 18 of the class's functions
 * (.text 0x020edec0..0x020eeafc).
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. The destructor is inline in the header, which makes
 * InitResources the key function that emits the vtable.
 *
 * leftovers:
 * - unk_0bc (dScMgBase_c) and unk_4664 (dScMgD3DBase_c) stay placeholders:
 *   they are read here as the `unk_0bc % 5` egg check and the Render flag
 *   test, but naming them is the shared header's change -- every minigame
 *   scene reads them under the placeholder spellings.
 * - `base + 0x466c` / `base + 0x4728` still address the two per-minigame
 *   cameras by offset: include/dScMgD3DBase_c.h already calls them camera
 *   elements and records "typing it as a real array is its own change".
 * - struct C must stay a non-polymorphic stand-in for the state PMF at
 *   0x5004: mwccarm lays out a pointer-to-member of a polymorphic class
 *   differently, so Behavior cannot dispatch through dScMgJump_c itself.
 * - data_ov006_0213cb4c..0213cb84 are ROM-owned {fn, 0} table rows kept as
 *   address-named StatePmf externs; data_ov004_020beb68 is the ov004 shared
 *   scene data, read raw at +0xb4/+0xb8 because its type is not recovered.
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
 * 18 of this class's own table. Calling OnYoshiTryEat by name would compile
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
    u32 words[2];
    MgJumpModelFilePtr(u32 fileID);
    ~MgJumpModelFilePtr();
};
extern MgJumpModelFilePtr data_ov006_02142188;
extern MgJumpModelFilePtr data_ov006_02142190;

/* Everything below that decl_common.h does not declare. Keep it above the
 * first function: tools/tiers.py charges text between two functions to the
 * earlier one. */
extern "C" {
void func_ov004_020ad90c(void);
void func_ov004_020b1e34(void *a, int b, int c, int d);
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

void func_ov006_020c40e8(void);
void func_ov006_020c42bc(void);
void func_ov006_020c712c(void);
void func_ov006_020c7388(void);
void func_ov006_020c7418(void);
void func_ov006_020c7490(void);
int  func_ov006_020c7300(void);
int  func_ov006_020c4060(void);
int  func_ov006_020c4684(char *ptr, int n);
int  func_ov006_020c7574(char *base, int count);
void func_ov006_020c72c8(void);
int  RandomIntInternal(int *seed);

extern int  data_020a0e68;
extern int  data_0209e650;
extern int  data_ov006_0213cb48;
extern u8   data_0209d45c;
extern s16  data_02082614;
extern int  data_ov006_02140418;
extern int  data_ov006_02140304;
extern int  data_ov006_02140434;
extern dScMgJump_c::StatePmf data_ov006_0213cb4c;
extern dScMgJump_c::StatePmf data_ov006_0213cb54;
extern dScMgJump_c::StatePmf data_ov006_0213cb5c;
extern dScMgJump_c::StatePmf data_ov006_0213cb6c;
extern dScMgJump_c::StatePmf data_ov006_0213cb74;
extern dScMgJump_c::StatePmf data_ov006_0213cb7c;
extern dScMgJump_c::StatePmf data_ov006_0213cb84;
}

namespace GX   { void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size); }
namespace G3X  { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { s32 fdiv(s32 a, s32 b); }
short ApproachLinear2(short &value, short target, short step);

// @symbol _ZN11dScMgJump_c13OnYoshiTryEatEi
/* Slot 18. 0 is an eat (one more egg, capped like the HUD score), 0x12 a
 * miss that clears the count and the shared scene's copy of it; anything
 * else just publishes the count. From the fifteenth egg on the next egg is
 * drawn at random, redrawn while it repeats the previous draw.
 *
 * The draw is written `if (unk_0bc < 15) idx = unk_0bc; else { redraw }`
 * rather than `idx = unk_0bc; if (idx >= 15) { redraw }`. The two are the
 * same code to a reader, but only the first reproduces the ROM: with the
 * guard directly ahead of the loop mwccarm hoists the seed address, the
 * previous-draw address and the 10 into r6-r8 and the function comes out a
 * word short. The if-converted then-arm sits between the guard and the loop
 * header, which keeps the three in the loop body, as the cartridge has them. */
void dScMgJump_c::OnYoshiTryEat(int arg)
{
    u32 idx;

    if (arg == 0) {
        unk_0bc = unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else if (arg == 0x12) {
        unk_0bc = 0;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(mHudScore);
    } else {
        u32 count = unk_0bc;
        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = count;
    }

    if (unk_0bc != 0)
        func_ov006_020c72b4();
    else
        func_ov006_020c72c8();

    if (unk_0bc < 0xf) {
        idx = unk_0bc;
    } else {
        do {
            idx = (int)((u32)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xa >> 0xc;
        } while (idx == (u32)data_ov006_0213cb48);
        data_ov006_0213cb48 = idx;
    }

    func_ov006_020c719c(unk_0bc, idx);

    if (unk_0bc < 0xf)
        data_ov006_02140328 = 3;
    else if (unk_0bc < 0x14)
        data_ov006_02140328 = 4;
    else if (unk_0bc < 0x19)
        data_ov006_02140328 = 5;
    else
        data_ov006_02140328 = 6;

    func_ov006_020c44b4((void *)unk_0bc, idx);
    func_ov006_020ee658();
}

// @symbol _ZN11dScMgJump_c13OnTurnIntoEggEi
/* Slot 19. mStateTimer is the same countdown the state functions below use. */
int dScMgJump_c::OnTurnIntoEgg(int sel)
{
    if (sel == 0) {
        if (unk_0bc % 5 == 4) {
            if (ApproachLinear2(mStateTimer, 0, 1) != 0) {
                if (data_ov006_02140428 < 3) {
                    func_ov006_020c72dc();
                    func_02012790(0x26);
                    mStateTimer = 0x3c;
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
    func_ov006_020c8a9c(0, 0);
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

    if (func_ov006_020c4684((char *)mArray2, 6) == 0)
        return 0;
    if (func_ov006_020c7574((char *)mPlayers, 3) == 0)
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
    func_ov004_020b04d0(0x10);
    ((Base *)this)->m48(-1);
    return 1;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee658Ev
/* The state functions below are unnamed in the ROM. mState is the current
 * state (see Behavior), mAccum gains mAccumStep >> 12 each tick and
 * mStateTimer counts down to the change.
 *
 * This one arms the countdown; its only caller is OnYoshiTryEat. */
void dScMgJump_c::func_ov006_020ee658()
{
    mStateTimer = 0x78;
    mAccum = 0;
    mAccumStep = 0x4000;
    mState = data_ov006_0213cb54;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee5b8Ev
/* Counts down mStateTimer, then shows the prompt and moves to the next state.
 * The legacy file's `#pragma optimize_for_size on` is not needed here and
 * would leak into every function in this file if carried. */
void dScMgJump_c::func_ov006_020ee5b8()
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
        func_ov006_020c7490();
        func_ov006_020ee598();
    } else if (left == 0x77) {
        func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);
    }
    func_ov006_020c42bc();
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee598Ev
void dScMgJump_c::func_ov006_020ee598()
{
    mState = data_ov006_0213cb84;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee508Ev
void dScMgJump_c::func_ov006_020ee508()
{
    int *acc = &mAccum;
    *acc += mAccumStep >> 12;
    if (mAccum > 0x1000) {
        *acc -= 0x1000;
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
    if(data_ov006_02140428 == 0){
        func_ov006_020ee4e0();
        return;
    }
    if(data_ov006_02140304 != 0)
        return;
    func_ov006_020ee3bc();
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee4e0Ev
void dScMgJump_c::func_ov006_020ee4e0()
{
    mStateTimer = 0x20;
    mState = data_ov006_0213cb5c;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee44cEv
void dScMgJump_c::func_ov006_020ee44c()
{
  int *acc = &mAccum;
  *acc += mAccumStep >> 12;
  if (mAccum > 0x1000)
  {
    *acc -= 0x1000;
  }
  func_ov006_020c42bc();
  func_ov006_020c712c();
  if (ApproachLinear2(mStateTimer, 0, 1) == 0)
  {
    return;
  }
  func_ov006_020c7388();
  func_ov006_020c40e8();
  mState = data_ov006_0213cb4c;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee3ecEv
void dScMgJump_c::func_ov006_020ee3ec()
{
    int ready = func_ov006_020c7300();
    if (ready) {
        ready = func_ov006_020c4060();
        if (ready) {
            func_ov004_020b0a54(0x12);
            mPromptEnabled = 0;
            mStateTimer = 0;
            mState = data_ov006_0213cb74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee3bcEv
void dScMgJump_c::func_ov006_020ee3bc()
{
    func_ov006_020c7418();
    mState = data_ov006_0213cb6c;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee2c4Ev
/* Its writes to 0x5004 and 0x5014 are what bound dScMgD3DBase_c's size
 * from above. `shared` is the ov004 scene data; its type is not recovered. */
void dScMgJump_c::func_ov006_020ee2c4()
{
  mAccum += mAccumStep >> 12;
  mAccumStep += 0x600;
  func_ov006_020c712c();
  int same = (int)(data_ov006_02140434 == data_ov006_02140418);
  if(same == 0) return;
  char* shared = (char *)data_ov004_020beb68;
  if(shared != 0){
    if(*(int*)(shared+0xb4) < 0x270f){
      *(int *)(shared + 0xb4) += 1;
    }
    if(*(int*)(shared+0xb4) > *(int*)(shared+0xb8)){
      *(int*)(shared+0xb8) = *(int*)(shared+0xb4);
    }
  }
  func_ov004_020adb1c(mHudScore);
  func_ov004_020b0a54(0);
  mPromptEnabled = 0;
  mStateTimer = 0;
  mState = data_ov006_0213cb7c;
}

// @symbol _ZN11dScMgJump_c19func_ov006_020ee2c0Ev
/* An empty state. */
void dScMgJump_c::func_ov006_020ee2c0()
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
/* Slot 9. Draws the instruction rows while unk_4664 == 1, then programs the
 * light vector and light color registers at 0x040004c8 and 0x040004cc
 * (lights 0 and 1, hence volatile) and renders the model. The row
 * sprites and the ov006 draw helpers are unnamed in symbols.txt. */
s32 dScMgJump_c::Render()
{

    if (unk_4664 == 1) {
        int count;
        int y;
        int i;

        func_ov004_020b1e34(this, 0xe0, 0x14, 1);

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

    func_ov006_020c70d0();
    func_ov006_020c425c();

    return 1;
}

// @symbol _ZN11dScMgJump_c16CleanupResourcesEv
/* Slot 3. Releases the model file; `this` is never read. */
s32 dScMgJump_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov006_02142184)->Release();
    data_ov006_02142184 = 0;
    func_ov004_020ad90c();
    return 1;
}

/* The destructor (D1 at 0x020edec0, D0 at 0x020edf54) is defined inline in
 * dScMgJump_c.h; defining it here would emit D0 before D1 and break the
 * ROM order. */

/* The scene's model file handles, registered in file-ID order; file 0x203
 * sits at 0x02142190, file 0x204 at 0x02142188. */
MgJumpModelFilePtr data_ov006_02142190(0x203);
MgJumpModelFilePtr data_ov006_02142188(0x204);
