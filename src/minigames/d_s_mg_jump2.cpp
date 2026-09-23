//cpp
/* ov006/dScMgJump2_c -- the second jump minigame, 29 functions
 * (.text 0x020eebe8..0x020efc08).
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder. The legacy file for func_ov006_020ef5ac had
 * `#pragma opt_propagation off`; it is not needed, and carried here it
 * would apply to every function in the file.
 *
 * deslop
 * Leftover: the func_ov006 helpers and data homes keep linker names;
 *   naming belongs at their definitions.
 */

#include "dScMgJump2_c.h"
#include "Model.h"
#include "SharedFilePtr.h"
#include "types.h"
#include "decl_common.h"

/* The list link at the head of each Elem. */
struct Node { struct Node* next; };

/* The 0x24-byte entries of mArray3, read back from what the functions below
 * do with them. Local because nothing names the type yet. */
struct Elem {
    struct Elem *next;      /* 0x00 -- the live list */
    int   x, y;             /* 0x04 -- 20.12 position */
    int   targetX, targetY; /* 0x0c */
    int   speedX, speedY;   /* 0x14 */
    short life;             /* 0x1c -- frames left */
    short sprite;           /* 0x1e */
    short active;           /* 0x20 */
    short _22;
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

extern "C" {
extern void func_ov006_020eeef4(struct Node* node);
extern void func_0203d630(int* p, int m);
extern struct Node* data_ov006_021421c0;
extern void func_ov006_020eed68(char *c);
extern void func_ov006_020eedc8(char *p);
extern int data_ov006_021421bc;
extern struct Elem *data_ov006_021421b0;
extern void func_ov006_020eee3c(int *thiz, int a1, int a2, int a3);
extern void *data_ov006_021421b8;
void func_ov004_020ad90c(void);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern int data_020a0e68;
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
unsigned int a, unsigned int b, int c, int d, int e, const Vector3_16f *f);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
void func_ov006_020eef90(void);
extern void* func_ov006_020c7300(void*);
extern void* func_ov006_020c4060(void*);
extern void func_ov004_020adb1c(int);
extern void func_ov004_020b0a54(int);
extern void func_ov006_020c42bc(void);
extern void func_ov006_020c712c(void);
extern int data_ov006_02140308;
extern struct G2 data_ov006_0213cc74;
extern struct G2 data_ov006_0213cc9c;
extern void func_ov006_020c4148(void);
extern void func_ov006_020c6f8c(int a);
extern void func_ov006_020ef580(char *self);
extern int data_ov006_02140304;
extern int data_ov006_02140328;
extern int data_ov006_02140428;
extern struct G2 data_ov006_0213cc84;
extern void func_ov006_020c7490(void);
extern void func_ov006_020ef768(char *self);
extern struct G2 data_ov006_0213cc94;
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, int dst);
extern void Camera_UpdateMatrices(void *cam);
extern int func_ov006_020c4684(void *ptr, int n);
extern int func_ov006_020c7574(void *base, int count);
extern u8 data_0209d45c;
extern s16 data_02082614;
/* TUBUILD CONFLICT -- alternate declaration of func_ov004_020adb1c, from the legacy file for func_ov006_020ef5ac, NOT applied: extern void func_ov004_020adb1c(int self); */
/* TUBUILD CONFLICT -- alternate declaration of GetGameLanguage, from the legacy file for _ZN12dScMgJump2_c13InitResourcesEv, NOT applied: extern s32 GetGameLanguage(void); */
/* TUBUILD CONFLICT -- alternate declaration of Matrix4x3_FromTranslation, from the legacy file for _ZN12dScMgJump2_c13InitResourcesEv, NOT applied: extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_021421b8, from the legacy file for _ZN12dScMgJump2_c13InitResourcesEv, NOT applied: extern int *data_ov006_021421b8; */
}

namespace GX   { void LoadOBJPltt(const void *src, unsigned int offset, unsigned int size); }
namespace G3X  { void SetFog(bool enable, int mode, int slope, int offset); }
namespace cstd { s32 fdiv(s32 a, s32 b); }
short ApproachLinear2(short &value, short target, short step);
int   ApproachLinear(int &value, int target, int step);

extern "C" {  /* .c-derived member: C linkage for the whole block */
void *_ZN7fBase_cnwEj(u32 sz);
void _ZN11dScMgBase_cC2Ev(void *p);
void __cxa_vec_ctor(void *o, int a, int b, void *f1, void *f2);
void _ZN8Particle10SysTrackerC1Ev(void *p);
void _ZN5ModelC1Ev(void *p);
extern char _ZTV14dScMgD3DBase_c[];
void _ZN16dMgJump3DMario_cD1Ev(void *p);
void _ZN16dMgJump3DMario_cC1Ev();
void func_ov006_020c6f70();
void func_ov006_020eed64();
void func_ov006_020efc08();
void func_ov006_020c6f3c();

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
        __cxa_vec_ctor(raw + 0x500c, 3, 0xb8, (void *)_ZN16dMgJump3DMario_cC1Ev, (void *)_ZN16dMgJump3DMario_cD1Ev);
        __cxa_vec_ctor(raw + 0x5234, 6, 0xf0, (void *)func_ov006_020c6f70, (void *)func_ov006_020c6f3c);
        __cxa_vec_ctor(raw + 0x57d4, 0x10, 0x24, (void *)func_ov006_020efc08, (void *)func_ov006_020eed64);
        _ZN5ModelC1Ev(raw + 0x5a14);
    }
    return raw;
}
}

// @symbol _ZN12dScMgJump2_c13OnYoshiTryEatEi
/* Slot 18: resets the round and arms the start countdown. */
void dScMgJump2_c::OnYoshiTryEat(int /* arg */)
{
  func_ov006_020c72b4();
  func_ov006_020c719c(0, 0);
  data_ov006_02140328 = 3;
  func_ov006_020c44b4(0, 0);
  func_ov006_020eeff0();
  func_ov006_020ef7f8((char *)this);
}

// @symbol _ZN12dScMgJump2_c13OnTurnIntoEggEi
/* Slot 19. */
int dScMgJump2_c::OnTurnIntoEgg(int /* mode */)
{
    func_ov006_020c8a9c(0, 0);
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

    tiles = LoadFile(data_ov006_0213cce0[GetGameLanguage()]);
    palette = LoadFile(0xd1);
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

    if (func_ov006_020c4684(mArray2, 6) == 0)
        return 0;

    func_ov006_020ef0d4((int)mArray3, 0x10);

    data_ov006_02140314 = 1;
    if (func_ov006_020c7574(mPlayers, 3) == 0)
        return 0;

    data_ov006_021421b8 = &data_ov006_021421c4;
    bmdFile = Model::LoadFile(*(SharedFilePtr *)&data_ov006_021421c4);

    if (((Model *)mModel)->SetFile((BMD_File *)bmdFile, 1, -1) == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(struct M48 *)&((Model *)mModel)->mat4x3 = *(struct M48 *)&data_020a0e68;
        return 0;
    }

    func_ov004_020b04d0(0x10);
    ((Base *)this)->m48(-1);
    return 1;
}

/* The state functions below are unnamed in symbols.txt and take the object
 * as raw bytes, because dScMgJump2_c.h only has placeholders from 0x5a64 to
 * 0x5a78. What the code shows: 0x5004 is the current state (see Behavior),
 * 0x5a64 (unk_5a64) accumulates 0x5a68 >> 12 each tick and wraps at 0x1000,
 * and 0x5a74 is an s16 countdown. Naming those fields is what unblocks typed
 * access here.
 *
 * This one arms the countdown and installs the next state. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef7f8(char* raw){
  *(short*)(raw+0x5a74) = 0x78;
  *(int*)(raw+0x5a64) = 0;
  *(int*)(raw+0x5a68) = 0x4000;
  *(struct G2*)(raw+0x5004) = data_ov006_0213cc94;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef794(char *raw)
{
    s16 *countdown = (s16 *)(raw + 0x5a74);
    *countdown = *countdown - 1;
    if (*(s16 *)(raw + 0x5a74) == 0) {
        dScMgJump2_c *scene = (dScMgJump2_c *)raw;
        if (scene->mPromptBlinkCount == 0) {
            scene->mPromptEnabled = 1;
            scene->mPromptBlinkCount = 1;
            scene->mPromptBlinkTimer = 0;
        }
        func_ov006_020c7490();
        func_ov006_020ef768(raw);
    }
    func_ov006_020c42bc();
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef768(char *raw)
{
    *(short *)(raw + 0x5a74) = 0xf0;
    *(struct G2 *)(raw + 0x5004) = data_ov006_0213cc84;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef5ac(char *raw)
{
    s16 done;
    int prevScore = data_ov006_02140308;
    int *acc = (int *)(raw + 0x5a64);
    int mode;

    *acc += *(int *)(raw + 0x5a68) >> 12;
    if (*(int *)(raw + 0x5a64) > 0x1000)
        *acc -= 0x1000;

    done = 0;
    mode = data_ov006_02140304;
    switch (mode) {
    case 0:
        done = 1;
        break;
    case 1:
        {
            int rc = ApproachLinear2(*(s16 *)(raw + 0x5a74), done, 2);
            if (rc)
                done = 1;
        }
        break;
    default:
        {
            int rc = ApproachLinear2(*(s16 *)(raw + 0x5a74), done, 1);
            if (rc)
                done = 1;
        }
        break;
    }

    if (done != 0 && data_ov006_02140304 < data_ov006_02140328) {
        int value;
        s16 countdown;
        func_ov006_020c4148();
        *(s16 *)(raw + 0x5a74) = 0xf0 - data_ov006_02140308 * 6;
        countdown = *(s16 *)(raw + 0x5a74);
        if (countdown < 0x3c)
            countdown = 0x3c;
        *(s16 *)(raw + 0x5a74) = countdown;
        value = (data_ov006_02140308 << 4) + 0x1600;
        if (value > 0x2200)
            value = 0x2200;
        func_ov006_020c6f8c(value);
    }

    func_ov006_020c42bc();
    func_ov006_020c712c();

    if (prevScore != data_ov006_02140308) {
        switch (data_ov006_02140308) {
        case 3:
            data_ov006_02140328 = 4;
            ((dScMgJump2_c *)raw)->mPromptEnabled = 0;
            break;
        case 0xa:
            data_ov006_02140328 = 5;
            break;
        case 0x14:
            data_ov006_02140328 = 6;
            break;
        }
        func_ov004_020adb1c(data_ov006_02140308);
    }

    if (data_ov006_02140428 != 0)
        return;
    func_ov006_020ef580(raw);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef580(char *raw)
{
    *(short *)(raw + 0x5a74) = 0x20;
    *(struct G2 *)(raw + 0x5004) = data_ov006_0213cc9c;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020c42bc(void);
void func_ov006_020c712c(void);
void func_ov006_020c7388(void);
void func_ov006_020c40e8(void);
extern struct G2 data_ov006_0213cc8c;
void func_ov006_020ef4ec(char *raw)
{
  int *acc = (int *)(raw + 0x5a64);
  *acc += *(int *)(raw + 0x5a68) >> 12;
  if (*(int *)(raw + 0x5a64) > 0x1000)
  {
    *acc -= 0x1000;
  }
  func_ov006_020c42bc();
  func_ov006_020c712c();
  if (ApproachLinear2(*(short *)(raw + 0x5a74), 0, 1) == 0)
  {
    return;
  }
  func_ov006_020c7388();
  func_ov006_020c40e8();
  *(struct G2 *)(raw + 0x5004) = data_ov006_0213cc8c;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef480(char *raw){
    void *obj = func_ov006_020c7300(raw);
    if(obj){
        obj = func_ov006_020c4060(obj);
        if(obj){
            func_ov004_020adb1c(data_ov006_02140308);
            func_ov004_020b0a54(0x12);
            ((dScMgJump2_c *)raw)->mPromptEnabled = 0;
            *(struct G2 *)(raw + 0x5004) = data_ov006_0213cc74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef47c(void)
{
}
}

// @symbol _ZN12dScMgJump2_c8BehaviorEv
/* Slot 6. unk_5a6c is a particle system's unique ID, handed to
 * NewUnkCallback818 and back each frame. The object FromUniqueID returns is
 * untyped, so its fields at 0x50 and 0x58 stay raw. Both Particle::System
 * calls keep their mangled spellings: declaring the real Fix12<int>
 * parameters changes how the call passes them. */
s32 dScMgJump2_c::Behavior()
{
    unk_5a6c = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unk_5a6c, 0xf0, 0x400000, 0x800000, -0x480000, 0);
    void *system = _ZN8Particle6System12FromUniqueIDEj(unk_5a6c);
    if (system != 0) {
        *(int *)((char *)system + 0x50) = 0x4000;
        *(unsigned char *)((char *)system + 0x58) = 0x2c;
    }
    func_ov006_020eef90();
    {
        dScMgJump2_cState *pp = (dScMgJump2_cState *)pad_5004;
        (this->**pp)();
    }
    return 1;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_0203cd80(int* m, int x);

void func_ov006_020ef2b8(void) {
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

    func_ov004_020b1a5c(data_ov006_02140308, 4);
    func_ov006_020eef58();
    func_ov006_020ef2b8();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0xffead000, -0xa0000);
    *(struct M48 *)&((Model *)mModel)->mat4x3 = *(struct M48 *)&data_020a0e68;

    scale = *(struct S3 *)&data_ov006_0213ccb0;

    {
        int *obj = (int *)(mModel + 8);
        anim = (void *)obj[1];
        func_02045e44((void *)obj, 1, 0);
    }
    *(int *)((char *)anim + 0x18) = unk_5a64;
    {
        Model *model = (Model *)mModel;
        model->Render((Vector3 *)&scale);
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    if (unk_4664 == 1) {
        x = 0x6e;
        count = data_ov006_02140428;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213cccc[idx][1], (void *)x, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, x, 0xc, -1, -1);
            }
            x += 0x12;
        }
    }

    return 1;
}

// @symbol _ZN12dScMgJump2_c16CleanupResourcesEv
/* Slot 3. data_ov006_021421b8 points at the model's file handle; it is
 * released and cleared. */
s32 dScMgJump2_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov006_021421b8)->Release();
    data_ov006_021421b8 = 0;
    func_ov004_020ad90c();
    return 1;
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef0d4(int table, int count){
  data_ov006_021421b0 = (struct Elem *)table;
  data_ov006_021421bc = count;
  data_ov006_021421b4 = 0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ef05c(int x, int y, int sprite)
{
    int i;
    for (i = data_ov006_021421bc - 1; i >= 0; i--) {
        if (data_ov006_021421b0[i].active == 0) {
            func_ov006_020eee3c((int *)&data_ov006_021421b0[i], x, y, sprite);
            return;
        }
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eeff0(void){
    int i;
    for(i=0;i<data_ov006_021421bc;i++){
        data_ov006_021421b0[i].active = 0;
    }
    data_ov006_021421c0=0;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eef90(void)
{
    int i = 0;
    if (i < data_ov006_021421bc) {
        int offset = 0;
        do {
            func_ov006_020eedc8((char *)data_ov006_021421b0 + offset);
            i++;
            offset += 0x24;
        } while (i < data_ov006_021421bc);
    }
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eef58(void) {
    void *node = data_ov006_021421c0;
    if (node == 0) return;
    do {
        func_ov006_020eed68((char *)node);
        node = *(void **)node;
    } while (node != 0);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eef40(void *node){
  *(void**)node = data_ov006_021421c0;
  data_ov006_021421c0 = (struct Node *)node;
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eeef4(struct Node* node) {
    struct Node* prev = data_ov006_021421c0;
    if (prev == node) {
        data_ov006_021421c0 = node->next;
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

extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov004_020b04c0(void);
void func_ov006_020eef40(void *node);

void func_ov006_020eee3c(int *raw, int startX, int startY, int spriteId)
{
    struct Elem *e = (struct Elem *)raw;
    e->x = startX;
    e->y = startY;
    e->speedX = 0;
    e->speedY = 0x4000;
    e->targetX = startX;
    e->targetY = startY - 0x40000;
    if (e->targetY < 0x8000) {
        int limit = func_ov004_020b04c0() << 12;
        if (e->targetY > -limit) {
            e->targetY = 0x8000;
            goto done;
        }
    }
    if (e->targetY >= -0xb8000 - (func_ov004_020b04c0() << 12))
        goto done;
    e->targetY = -0xb8000 - (func_ov004_020b04c0() << 12);
done:
    e->life = 0x80;
    e->active = 1;
    e->sprite = (short)spriteId;
    func_ov006_020eef40(raw);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eedc8(char* raw) {
    struct Elem *e = (struct Elem *)raw;
    if (e->active == 0) return;
    if (ApproachLinear2(e->life, 0, 1) != 0) {
        func_ov006_020eeef4((struct Node *)raw);
        e->active = 0;
    }
    ApproachLinear(e->x, e->targetX, e->speedX);
    ApproachLinear(e->y, e->targetY, e->speedY);
    func_0203d630(&e->speedX, 0xf00);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eed68(char *raw)
{
    struct Elem *e = (struct Elem *)raw;
    if (e->active == 0) return;
    func_ov004_020b2444(e->x >> 12, e->y >> 12,
                        e->sprite, -1, -1, 0, data_ov006_021421b4);
}
}

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eed64(void)
{
}
}

// @symbol _ZN12dScMgJump2_cD0Ev
/* Generated from the destructor below. */

// @symbol _ZN12dScMgJump2_cD1Ev
/* Defined in the class body in include/dScMgJump2_c.h, which explains why.
   mwccarm emits D1 and D0 from it, in this order. */
