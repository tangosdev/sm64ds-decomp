//cpp
/* ov006/dScMgJump_c -- 17 of the class's 18 functions
 * (.text 0x020edec0..0x020ee994).
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. OnYoshiTryEat (0x020ee994) is not matched yet and lives in its
 * own file. The destructor is inline in the header, which makes
 * InitResources the key function that emits the vtable.
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

/* The same eight bytes as a plain pair: the state functions copy the next
 * state into 0x5004 whole. Blocked from being a typed pointer-to-member until
 * the state functions' signature is recovered. */
struct Pair { int w[2]; };

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

/* Defined below. */
void func_ov006_020ee3bc(char *c);
void func_ov006_020ee4e0(char *p);
void func_ov006_020ee598(char *p);

extern int  data_020a0e68;
extern u8   data_0209d45c;
extern s16  data_02082614;
extern int  data_ov006_02140418;
extern int  data_ov006_02140304;
extern int  data_ov006_02140434;
extern struct Pair data_ov006_0213cb4c;
extern struct Pair data_ov006_0213cb54;
extern struct Pair data_ov006_0213cb5c;
extern struct Pair data_ov006_0213cb6c;
extern struct Pair data_ov006_0213cb74;
extern struct Pair data_ov006_0213cb7c;
extern struct Pair data_ov006_0213cb84;
}

namespace GX   { void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size); }
namespace G3X  { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { s32 fdiv(s32 a, s32 b); }
short ApproachLinear2(short &value, short target, short step);

// @symbol _ZN11dScMgJump_c13OnTurnIntoEggEi
/* Slot 19. unk_5014 is the same countdown the state functions below use;
 * it keeps its placeholder name until the header names those fields. */
int dScMgJump_c::OnTurnIntoEgg(int sel)
{
    if (sel == 0) {
        if (unk_0bc % 5 == 4) {
            if (ApproachLinear2(unk_5014, 0, 1) != 0) {
                if (data_ov006_02140428 < 3) {
                    func_ov006_020c72dc();
                    func_02012790(0x26);
                    unk_5014 = 0x3c;
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

// @symbol func_ov006_020ee658
/* The state functions below are unnamed in symbols.txt and take the object
 * as raw bytes, because dScMgJump_c.h only has placeholders from 0x5004 to
 * 0x5016. What the code shows: 0x5004 is the current state (see Behavior),
 * 0x500c accumulates 0x5010 >> 12 each tick and 0x5014 is an s16 countdown.
 * Naming those fields is what unblocks typed access here.
 *
 * This one arms the countdown; its only caller is OnYoshiTryEat. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee658(char *raw) {
    *(short *)(raw + 0x5014) = 0x78;
    *(int *)(raw + 0x500c) = 0;
    *(int *)(raw + 0x5010) = 0x4000;
    *(struct Pair *)(raw + 0x5004) = data_ov006_0213cb54;
}
}

// @symbol func_ov006_020ee5b8
/* Counts down 0x5014, then shows the prompt and moves to the next state.
 * The legacy file's `#pragma optimize_for_size on` is not needed here and
 * would leak into every function in this file if carried. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee5b8(char* raw){
    dScMgJump_c *self = (dScMgJump_c *)raw;
    int left;
    short *countdown = (short *)(raw + 0x5014);
    char* base = raw + 0x5000;
    *countdown = *countdown - 1;
    left = *(short*)(base + 0x14);
    if (left == 0) {
        FreeGfxSlotsById(0x1d);
        if (self->mPromptBlinkCount == 0) {
            self->mPromptEnabled = 1;
            self->mPromptBlinkCount = 1;
            self->mPromptBlinkTimer = 0;
        }
        func_ov006_020c7490();
        func_ov006_020ee598(raw);
    } else if (left == 0x77) {
        func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);
    }
    func_ov006_020c42bc();
}
}

// @symbol func_ov006_020ee598
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee598(char *raw) {
    *(struct Pair *)(raw + 0x5004) = data_ov006_0213cb84;
}
}

// @symbol func_ov006_020ee508
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee508(char *raw){
    int *acc = (int *)(raw + 0x500c);
    *acc += *(int *)(raw + 0x5010) >> 12;
    if (*(int *)(raw + 0x500c) > 0x1000) {
        *acc -= 0x1000;
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
    if(data_ov006_02140428 == 0){
        func_ov006_020ee4e0(raw);
        return;
    }
    if(data_ov006_02140304 != 0)
        return;
    func_ov006_020ee3bc(raw);
}
}

// @symbol func_ov006_020ee4e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee4e0(char *raw)
{
    *(unsigned short *)(raw + 0x5014) = 0x20;
    *(struct Pair *)(raw + 0x5004) = data_ov006_0213cb5c;
}
}

// @symbol func_ov006_020ee44c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee44c(char *raw)
{
  int *acc = (int *)(raw + 0x500c);
  char *hi = raw + 0x5000;
  *acc += *(int *)((raw + 0x5000) + 0x10) >> 12;
  if (*(int *)(hi + 0xc) > 0x1000)
  {
    *acc -= 0x1000;
  }
  func_ov006_020c42bc();
  func_ov006_020c712c();
  if (ApproachLinear2(*(short *) (raw + 0x5014), 0, 1) == 0)
  {
    return;
  }
  func_ov006_020c7388();
  func_ov006_020c40e8();
  *(struct Pair *)(raw + 0x5004) = data_ov006_0213cb4c;
}
}

// @symbol func_ov006_020ee3ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee3ec(char* raw)
{
    dScMgJump_c *self = (dScMgJump_c *)raw;
    int ready = func_ov006_020c7300();
    if (ready) {
        ready = func_ov006_020c4060();
        if (ready) {
            func_ov004_020b0a54(0x12);
            self->mPromptEnabled = 0;
            *(u16 *)(raw + 0x5014) = 0;
            *(struct Pair *)(raw + 0x5004) = data_ov006_0213cb74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}
}

// @symbol func_ov006_020ee3bc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee3bc(char *raw)
{
    func_ov006_020c7418();
    *(struct Pair*)(raw + 0x5004) = data_ov006_0213cb6c;
}
}

// @symbol func_ov006_020ee2c4
/* Its writes to 0x5004 and 0x5014 are what bound dScMgD3DBase_c's size
 * from above. `shared` is the ov004 scene data; its type is not recovered. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee2c4(char* raw){
  dScMgJump_c *self = (dScMgJump_c *)raw;
  *(int *)(raw + 0x500c) += *(int *)(raw + 0x5010) >> 12;
  *(int *)(raw + 0x5010) += 0x600;
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
  func_ov004_020adb1c(self->mHudScore);
  func_ov004_020b0a54(0);
  self->mPromptEnabled = 0;
  *(short*)(raw+0x5014) = 0;
  *(struct Pair*)(raw+0x5004) = data_ov006_0213cb7c;
}
}

// @symbol func_ov006_020ee2c0
/* An empty state. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee2c0(void)
{
}
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
    char *c = (char *)this;

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
        *(int *)((char *)p + 0x18) = *(int *)(c + 0x500c);

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
