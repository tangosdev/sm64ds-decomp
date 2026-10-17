//cpp
// @symbol _ZN8Particle19SetSelfDestructFlagEj
// @symbol func_020226fc
// @symbol func_02022774
// @symbol func_020227ec
// @symbol func_02022864
// @symbol func_020228dc
// @symbol func_0202293c
// @symbol _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_
// @symbol func_020229f0
// @symbol func_02022a4c
// @symbol _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_
// @symbol func_02022b04
// @symbol _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_
// @symbol _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f
// @symbol _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj
// @symbol func_02022c3c
// @symbol func_02022c80
// @symbol func_02022cbc
// @symbol func_02022d00
// @symbol func_02022d44
// @symbol _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
// @symbol _ZN8Particle6System12FromUniqueIDEj
// @symbol _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_
// @symbol _ZN8Particle9RenderAllEv
// @symbol _ZN8Particle10SysTracker6UpdateEv
// @symbol _ZN8Particle10SysTracker10InitialiseEv
// @symbol _ZN8Particle7Texture12AllocPalVramEjb
// @symbol _ZN8Particle7Texture12AllocTexVramEjb
// @symbol func_02023178
// @symbol _ZN8Particle10SysTrackerD1Ev
// @symbol _ZN8Particle10SysTrackerC1Ev
/* engine/particle/Particle.cpp -- Particle TU (arm9 0x020226d4..0x020233d4)
 *
 * The singleton particle engine's core translation unit: SysTracker lifecycle
 * (construct, Initialise, Update, destruct), the System::New front door and
 * its fixed-effect-id wrapper family, the Texture VRAM allocators, RenderAll,
 * and SetSelfDestructFlag. The SysTracker object itself is the single global
 * at data_0209ee74; every wrapper reaches it directly.
 *
 * mwccarm emits .text in reverse source order here, so the definitions below
 * run ROM-descending; the roster above reads ROM-ascending. */
#include "types.h"
#include "Heap.h"
#include "decl_common.h"
#include "Model.h"
#include "Particle__SysTracker.h"
#include "Particle__Texture.h"
#include "Particle__Manager.h"
#include "Particle__System.h"

extern "C" {
void func_0203cbc0(void *p);
void func_02049ee8(void* manager, void* renderState);
void DecompressLZ16(const void* src, void* dst);
}

extern int data_0209b3ec;
extern u32 data_0209ee84;
extern u32 data_0209ee8c;
extern signed char data_0209f2f8;
extern void* data_020a0ea0;

struct Vec3 {
    s16 x, y, z;
};

namespace Particle {

// @symbol _ZN8Particle10SysTrackerC1Ev
SysTracker::SysTracker()
{
    data_0209ee74 = this;
    mManager = 0;
    mRunningSlidingDustSystemID = 0;
    mSystemID_75c = 0;
    mBigSplashSystemID = 0;
    mSystemID_774 = 0;
    mSystemID_780 = 0;
    mRippleSystemID = 0;
    mSystemID_798 = 0;
    mSystemID_7a4 = 0;
    mSystemID_7b0 = 0;
    mSystemID_7c0 = 0;
    mCallback_800.waterOffset = 0x4b000;
}

// @symbol _ZN8Particle10SysTrackerD1Ev
SysTracker::~SysTracker()
{
    if (data_0209ee80 != 0) {
        mContents.Clear();
        func_0203cbc0(data_0209ee80);
        data_0209ee80 = 0;
    }
    if (mResourceFile != (void *)data_02075f14) {
        _ZN6Memory10DeallocateEPv(mResourceFile);
    }
    data_0209ee74 = 0;
}

}

extern "C" {

// @symbol func_02023178
char* func_02023178(int n) {
    char* old = (char*)data_0209ee78;
    data_0209ee78 = old + n;
    return old;
}

}

namespace Particle {

// @symbol _ZN8Particle7Texture12AllocTexVramEjb
u32 Texture::AllocTexVram(u32 size, bool isTexel4x4)
{
    if (isTexel4x4) {
        u32 address = data_0209ee88;
        data_0209ee88 = address + size;
        return address;
    }

    return Model::GetVramOffset(size);
}

// @symbol _ZN8Particle7Texture12AllocPalVramEjb
u32 Texture::AllocPalVram(u32 size, bool fromLowAddress)
{
    if (fromLowAddress) {
        u32 alignedSize = (size + 7) & ~7;
        u32 address = data_0209ee84;
        data_0209ee84 = address + alignedSize;
        return address;
    }

    u32 alignedSize = (size + 15) & ~15;
    u32 address = data_0209ee8c - alignedSize;
    data_0209ee8c = address;
    return address;
}

#define M(p) ((long long)(int)(p))

// @symbol _ZN8Particle10SysTracker10InitialiseEv
void SysTracker::Initialise()
{
    signed char v = data_0209f2f8;
    unsigned int allocSize = 0x8c00;
    int countA = 0x28;
    int countB = 0x100;

    if (v == 0x24 || v == 0x26 || v == 0x28) {
        countA = 0x40;
        countB = 0x140;
        allocSize = 0xa800;
    }

    {
        char* base = (char*)_ZN6Memory13operator_new2Ej(allocSize);
        char* end = base + allocSize;
        data_0209ee80 = base;
        data_0209ee78 = base;
        data_0209ee7c = end;
    }

    data_0209ee88 = func_02045d10();
    data_0209ee84 = func_02045cf0();
    data_0209ee8c = func_02045ce0();

    mManager = (Manager*)func_0204a4c8(
        func_02023178, countA, countB, 0x1a, 0x3e);
    *(int*)((char*)mManager + 0x30) = 0x8000;

    if (func_0206e28c((u8*)data_02075f14, data_0208f668, 4) != 0) {
        mResourceFile = data_02075f14;
    } else {
        char* hdr = (char*)(int)M(data_02075f14);
        unsigned int size = (unsigned int)M(*(unsigned int*)(hdr + 4) >> 8);
        void* dst = _ZN6Memory8AllocateEj(size);
        DecompressLZ16(hdr + 4, dst);
        _ZN4CP1514FlushDataCacheEjj((unsigned int)dst, size);
        mResourceFile = dst;
    }

    func_0204a17c(mManager, mResourceFile);
    func_0204a0dc(mManager,
        (u32 (*)(const void *, u32))Texture::AllocTexVram);
    func_0204a028(mManager,
        (u32 (*)(u32, u32))Texture::AllocPalVram);

    if (mResourceFile != data_02075f14) {
        mHeap::Heap_t *heap = (mHeap::Heap_t *)(int)M(data_020a0ea0);
        unsigned int oldSize = (unsigned int)M(*(unsigned int*)((char*)mResourceFile + 0x18));
        heap->_Sizeof(mResourceFile);
        heap->Reallocate(mResourceFile, oldSize);
    }
}

// @symbol _ZN8Particle10SysTracker6UpdateEv
void SysTracker::Update()
{
    mContents.Update();
    func_02049f58(mManager);
}

// @symbol _ZN8Particle9RenderAllEv
void RenderAll()
{
    Particle__SysTracker* tracker = data_0209ee74;
    if (!tracker)
        return;

    func_02049ee8(tracker->mManager, &data_0209b3ec);
}

}

extern "C" {

// @symbol _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_
void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 uniqueID, s32 x, s32 y, s32 z) {
    Vector3 pos;
    s32 xv = x >> 3;
    s32 yv = y >> 3;
    s32 zv = z >> 3;
    pos.x = xv;
    pos.y = yv;
    pos.z = zv;
    Particle::Manager *mgr = data_0209ee74->mManager;
    return mgr->AddSystem(uniqueID, pos);
}

}

namespace Particle {

// @symbol _ZN8Particle6System12FromUniqueIDEj
System *System::FromUniqueID(u32 uniqueID)
{
    SysTracker::Contents::Entry *entry =
        data_0209ee74->mContents.FindData(uniqueID);
    return entry->system;
}

}

extern "C" {

struct ParamBlk
{
  char pad20[0x20];
  int f20;
  int f24;
  int f28;
  char pad2c[0x3c - 0x2c];
  short f3c;
  short f3e;
  short f40;
};
typedef void (*VFn)(void *, void *);
struct VObj
{
  VFn *vtable;
};
struct System
{
  int f0;
  char pad4[6];
  unsigned char fa;
  char padb[1];
  struct ParamBlk *fc;
  struct VObj *f10;
};
struct Args3
{
  int a;
  int b;
  int c;
};


// @symbol _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 type, u32 u, int a, int b, int c, const struct Vec3 *pos, void *cb)
{
  struct System *new_var;
  void *contents;
  struct System *sys;
  struct Args3 args;
  args.a = a >> 3;
  contents = ((char *) data_0209ee74) + 8;
  args.b = b >> 3;
  args.c = (c >> 1) >> 2;
  new_var = (struct System *)((Particle::SysTracker::Contents *)contents)->FindData(type);
  sys = new_var;
  if (sys)
  {
    struct VObj *o;
    struct ParamBlk *p;
    sys->fa = 1;
    p = sys->fc;
    p->f20 = args.a;
    p->f24 = args.b;
    p->f28 = args.c;
    if (pos != 0)
    {
      p->f3c = pos->x;
      p->f3e = pos->y;
      p->f40 = pos->z;
    }
    o = sys->f10;
    if (o != 0)
    {
      o->vtable[0](o, p);
    }
    return sys->f0;
  }
  return (int)((Particle::SysTracker::Contents *)contents)
      ->Create(u, *(Vector3 *)&args, (const Vector3_16f *)pos, (dPa_c::level_c::callback_c *)cb);
}

// @symbol func_02022d44
u32 func_02022d44(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (char*)data_0209ee74 + 0x7f0);
}

// @symbol func_02022d00
u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (char*)data_0209ee74 + 0x7f4);
}

// @symbol func_02022cbc
u32 func_02022cbc(int uniqueID, int effectID, s32 x, s32 y, s32 z, int dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (char*)data_0209ee74 + 0x7f8);
}

// @symbol func_02022c80
u32 func_02022c80(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (char*)data_0209ee74 + 0x800);
}

// @symbol func_02022c3c
u32 func_02022c3c(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (char*)data_0209ee74 + 0x808);
}

// @symbol _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj
u32 _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir, u8 numWeatherEffectsNow)
{
    char* callback = (char*)&data_0209ee74->mWeatherCallback;
    *(u8*)(callback + 4) = numWeatherEffectsNow;
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir, callback);
}

// @symbol _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f
u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const struct Vector3_16f* dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, (const Vec3*)dir,
        (void*)&data_0209ee74->mCallback_818);
}

// @symbol _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z)
{
    data_0209ee74->mRunningSlidingDustSystemID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            data_0209ee74->mRunningSlidingDustSystemID,
            0xda,
            x, y, z,
            (const Vec3*)0,
            (void*)&data_0209ee74->mRunningSlidingDustCallback);
}

// @symbol func_02022b04
void func_02022b04(Fix12i x, Fix12i y, Fix12i z)
{
    *(u32*)((char*)data_0209ee74 + 0x75c) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)((char*)data_0209ee74 + 0x75c),
            0x61,
            x, y, z,
            (const Vec3*)0,
            (char*)data_0209ee74 + 0x760);
}

// @symbol _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_
void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z)
{
    data_0209ee74->mBigSplashSystemID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            data_0209ee74->mBigSplashSystemID,
            0xdd,
            x, y, z,
            (const Vec3*)0,
            (void*)&data_0209ee74->mBigSplashCallback);
}

// @symbol func_02022a4c
u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = (char*)data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x774),
        0xdf,
        x, y, z,
        (const Vec3*)0,
        base + 0x76c);
    *(u32*)((char*)data_0209ee74 + 0x774) = result;
    return result;
}

// @symbol func_020229f0
u32 func_020229f0(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = (char*)data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x780),
        0xc4,
        x, y, z,
        (const Vec3*)0,
        base + 0x784);
    *(u32*)((char*)data_0209ee74 + 0x780) = result;
    return result;
}

// @symbol _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_
u32 _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z)
{
    Particle::SysTracker *tracker = data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        tracker->mRippleSystemID,
        0x109,
        x, y, z,
        (const Vec3*)0,
        (void*)&tracker->mRippleCallback);
    data_0209ee74->mRippleSystemID = result;
    return result;
}

// @symbol func_0202293c
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = (char*)data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x798),
        0x118,
        x, y, z,
        (const Vec3*)0,
        base + 0x79c);
    *(u32*)((char*)data_0209ee74 + 0x798) = result;
    return result;
}

// @symbol func_020228dc
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = (char*)data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x7a4),
        0x117,
        x, y, z,
        (const Vec3*)0,
        base + 0x7a8);
    *(u32*)((char*)data_0209ee74 + 0x7a4) = result;
    return result;
}

// @symbol func_02022864
void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2)
{
    *(s16*)((char*)data_0209ee74 + 0x7ba) = ang1;
    *(s16*)((char*)data_0209ee74 + 0x7bc) = ang2;
    *(u32*)((char*)data_0209ee74 + 0x7b0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)((char*)data_0209ee74 + 0x7b0),
        0x51,
        x, y, z,
        (const Vec3*)0,
        (char*)data_0209ee74 + 0x7b4);
}

// @symbol func_020227ec
void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2)
{
    *(s16*)((char*)data_0209ee74 + 0x7ca) = ang1;
    *(s16*)((char*)data_0209ee74 + 0x7cc) = ang2;
    *(u32*)((char*)data_0209ee74 + 0x7c0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)((char*)data_0209ee74 + 0x7c0),
        0x52,
        x, y, z,
        (const Vec3*)0,
        (char*)data_0209ee74 + 0x7c4);
}

// @symbol func_02022774
void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2)
{
    *(s16*)((char*)data_0209ee74 + 0x7da) = ang1;
    *(s16*)((char*)data_0209ee74 + 0x7dc) = ang2;
    *(u32*)((char*)data_0209ee74 + 0x7d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)((char*)data_0209ee74 + 0x7d0),
        0x4f,
        x, y, z,
        (const Vec3*)0,
        (char*)data_0209ee74 + 0x7d4);
}

// @symbol func_020226fc
void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 arg3, s16 arg4)
{
    Particle::SysTracker *sys1;
    Particle::SysTracker *sys2;
    Particle::SysTracker *sys3;
    Particle::SysTracker *sys4;
    u32 result;

    sys1 = data_0209ee74;
    *(s16*)((u8*)sys1 + 0x7ea) = arg3;
    sys2 = data_0209ee74;
    *(s16*)((u8*)sys2 + 0x7ec) = arg4;
    sys3 = data_0209ee74;
    result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)((u8*)sys3 + 0x7e0),
        0x50,
        x, y, z,
        (const Vec3*)0,
        (u8*)sys3 + 0x7e4);
    sys4 = data_0209ee74;
    *(u32*)((u8*)sys4 + 0x7e0) = result;
}

}

namespace Particle {

// @symbol _ZN8Particle19SetSelfDestructFlagEj
void SetSelfDestructFlag(u32 definitionID)
{
    data_0209ee74->mManager->mDefinitions[definitionID].data->flags |= 0x4000;
}

}
