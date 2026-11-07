//cpp
// @symbol _ZN8Particle19SetSelfDestructFlagEj
// @symbol func_020226fc
// @symbol func_02022774
// @symbol func_020227ec
// @symbol func_02022864
// @symbol func_020228dc
// @symbol func_0202293c
// @symbol _ZN8Particle6System9NewRippleEiii
// @symbol func_020229f0
// @symbol func_02022a4c
// @symbol _ZN8Particle6System12NewBigSplashEiii
// @symbol _ZN8Particle24MegaRunningSlidingDustAtEiii
// @symbol _ZN8Particle20RunningSlidingDustAtEiii
// @symbol _ZN8Particle6System17NewUnkCallback818EjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System10NewWeatherEjjiiiPK11Vector3_16fh
// @symbol func_02022c3c
// @symbol func_02022c80
// @symbol func_02022cbc
// @symbol func_02022d00
// @symbol func_02022d44
// @symbol _ZN8Particle6System3NewEjjiiiPK11Vector3_16fPNS_8CallbackE
// @symbol _ZN8Particle6System12FromUniqueIDEj
// @symbol _ZN8Particle6System9NewSimpleEjiii
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
 * and SetSelfDestructFlag. The constructor publishes the one SysTracker in the
 * global pointer gParticleSysTracker; every wrapper reaches it through that.
 *
 * mwccarm emits .text in reverse source order here, so the definitions below
 * run ROM-descending; the roster above reads ROM-ascending. */
#include "types.h"
#include "Heap.h"
#include "decl_common.h"
#include "dExtSimpleModel_c.h"
#include "Particle__SysTracker.h"
#include "Particle__Texture.h"
#include "Particle__Manager.h"
#include "Particle__System.h"
#include "CP15.h"
#include "Memory.h"
#include "private/particle_resource_header.h"

extern "C" {
void func_0203cbc0(void *p);
void func_02049ee8(void* manager, void* renderState);
void DecompressLZ16(const void* src, void* dst);
}

extern Matrix4x3 data_0209b3ec;
extern u32 sParticlePalVramLowCursor;
extern u32 sParticlePalVramHighCursor;
extern signed char gCurrentSublevel;

namespace Particle {

// @symbol _ZN8Particle10SysTrackerC1Ev
SysTracker::SysTracker()
{
    gParticleSysTracker = this;
    mManager = 0;
    mRunningSlidingDustSystemID = 0;
    mMegaRunningSlidingDustSystemID = 0;
    mBigSplashSystemID = 0;
    mSystemID_774 = 0;
    mBubbleSystemID = 0;
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
    if (sParticleHeapBase != 0) {
        mContents.Clear();
        func_0203cbc0(sParticleHeapBase);
        sParticleHeapBase = 0;
    }
    if (mResourceFile != data_02075f14) {
        Memory::Deallocate(mResourceFile);
    }
    gParticleSysTracker = 0;
}

}

extern "C" {

// @symbol func_02023178
char *func_02023178(int size) {
    char *block = (char *)sParticleHeapCursor;
    sParticleHeapCursor = block + size;
    return block;
}
}

namespace Particle {

// @symbol _ZN8Particle7Texture12AllocTexVramEjb
u32 Texture::AllocTexVram(u32 size, bool isTexel4x4)
{
    if (isTexel4x4) {
        u32 address = sParticleTex4x4VramCursor;
        sParticleTex4x4VramCursor = address + size;
        return address;
    }

    return dExtSimpleModel_c::GetVramOffset(size);
}

// @symbol _ZN8Particle7Texture12AllocPalVramEjb
u32 Texture::AllocPalVram(u32 size, bool fromLowAddress)
{
    if (fromLowAddress) {
        u32 alignedSize = (size + 7) & ~7;
        u32 address = sParticlePalVramLowCursor;
        sParticlePalVramLowCursor = address + alignedSize;
        return address;
    }

    u32 alignedSize = (size + 15) & ~15;
    u32 address = sParticlePalVramHighCursor - alignedSize;
    sParticlePalVramHighCursor = address;
    return address;
}

// @symbol _ZN8Particle10SysTracker10InitialiseEv
void SysTracker::Initialise()
{
    signed char v = gCurrentSublevel;
    unsigned int allocSize = 0x8c00;
    int countA = 0x28;
    int countB = 0x100;

    if (v == 0x24 || v == 0x26 || v == 0x28) {
        countA = 0x40;
        countB = 0x140;
        allocSize = 0xa800;
    }

    {
        char *base = (char *)Memory::operator_new2(allocSize);
        char* end = base + allocSize;
        sParticleHeapBase = base;
        sParticleHeapCursor = base;
        sParticleHeapEnd = end;
    }

    sParticleTex4x4VramCursor = func_02045d10();
    sParticlePalVramLowCursor = func_02045cf0();
    sParticlePalVramHighCursor = func_02045ce0();

    mManager = (Manager*)func_0204a4c8(
        func_02023178, countA, countB, 0x1a, 0x3e);
    mManager->mPolygonAttr = 0x8000;

    if (func_0206e28c((u8*)data_02075f14, data_0208f668, 4) != 0) {
        mResourceFile = data_02075f14;
    } else {
        /* The compressed stream starts at +4; its header word carries the
           decompressed size in the upper 24 bits. The (int) cast keeps the
           file address in one register for both uses: without it the
           compiler reloads the address from the literal pool. */
        char *file = (char *)(int)data_02075f14;
        u32 size = *(u32 *)(file + 4) >> 8;
        void *dst = Memory::Allocate(size);
        DecompressLZ16(file + 4, dst);
        CP15::FlushDataCache((unsigned int)dst, size);
        mResourceFile = dst;
    }

    func_0204a17c(mManager, mResourceFile);
    func_0204a0dc(mManager,
        (u32 (*)(const void *, u32))Texture::AllocTexVram);
    func_0204a028(mManager,
        (u32 (*)(u32, u32))Texture::AllocPalVram);

    if (mResourceFile != data_02075f14) {
        /* The textures are in VRAM now; keep only the resource's resident
           part. The (long long) widening is a register-allocation lever:
           without it trimmedSize and heap trade r4 and r5. */
        Heap *heap = Memory::defaultHeapPtr;
        u32 trimmedSize = (long long)((ParticleResourceHeader *)mResourceFile)->residentSize;
        heap->_Sizeof(mResourceFile);
        heap->Reallocate(mResourceFile, trimmedSize);
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
    SysTracker *tracker = gParticleSysTracker;
    if (!tracker)
        return;

    func_02049ee8(tracker->mManager, &data_0209b3ec);
}

// @symbol _ZN8Particle6System9NewSimpleEjiii
void *System::NewSimple(u32 uniqueID, Fix12i x, Fix12i y, Fix12i z) {
    Vector3 pos;
    pos.x = x >> 3;
    pos.y = y >> 3;
    pos.z = z >> 3;
    return gParticleSysTracker->mManager->AddSystem(uniqueID, pos);
}

// @symbol _ZN8Particle6System12FromUniqueIDEj
System *System::FromUniqueID(u32 uniqueID) {
    SysTracker::Contents::Entry *entry = gParticleSysTracker->mContents.FindData(uniqueID);
    return entry->system;
}

/* A live uniqueID moves its system to the new position (and direction, when
 * one is given) and re-runs the callback's SpawnParticles; anything else
 * starts a fresh system of definitionID. */
// @symbol _ZN8Particle6System3NewEjjiiiPK11Vector3_16fPNS_8CallbackE
int System::New(u32 uniqueID, u32 definitionID, Fix12i x, Fix12i y, Fix12i z,
                const Vector3_16f *dir, Callback *callback) {
    SysTracker::Contents::Entry *found;
    SysTracker::Contents *contents;
    SysTracker::Contents::Entry *entry;
    Vector3 pos;
    pos.x = x >> 3;
    contents = &gParticleSysTracker->mContents;
    pos.y = y >> 3;
    pos.z = (z >> 1) >> 2;
    found = contents->FindData(uniqueID);
    entry = found;
    if (entry) {
        System *system;
        dPa_c::level_c::callback_c *entryCallback;
        entry->active = 1;
        system = entry->system;
        system->position.x = pos.x;
        system->position.y = pos.y;
        system->position.z = pos.z;
        if (dir != 0) {
            system->direction.x = dir->x;
            system->direction.y = dir->y;
            system->direction.z = dir->z;
        }
        entryCallback = entry->callback;
        if (entryCallback != 0) {
            entryCallback->SpawnParticles(*system);
        }
        return entry->uniqueID;
    }
    return contents->Create(definitionID, pos, dir, (dPa_c::level_c::callback_c *)callback);
}
}

extern "C" {

// @symbol func_02022d44
u32 func_02022d44(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir)
{
    return Particle::System::New(uniqueID, effectID, x, y, z, (const Vector3_16f *)dir,
                                 (Particle::Callback *)&gParticleSysTracker->mCheckWaterCallback);
}

// @symbol func_02022d00
u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir) {
    return Particle::System::New(
        uniqueID, effectID, x, y, z, (const Vector3_16f *)dir,
        (Particle::Callback *)&gParticleSysTracker->mCheckWaterRippleCallback);
}

// @symbol func_02022cbc
u32 func_02022cbc(int uniqueID, int effectID, s32 x, s32 y, s32 z, int dir)
{
    return Particle::System::New(uniqueID, effectID, x, y, z, (const Vector3_16f *)dir,
                                 (Particle::Callback *)&gParticleSysTracker->mCallback_7f8);
}

// @symbol func_02022c80
u32 func_02022c80(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir)
{
    return Particle::System::New(uniqueID, effectID, x, y, z, (const Vector3_16f *)dir,
                                 (Particle::Callback *)&gParticleSysTracker->mCallback_800);
}

// @symbol func_02022c3c
u32 func_02022c3c(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir)
{
    return Particle::System::New(uniqueID, effectID, x, y, z, (const Vector3_16f *)dir,
                                 (Particle::Callback *)&gParticleSysTracker->mCheckYoganCallback);
}
}

namespace Particle {

// @symbol _ZN8Particle6System10NewWeatherEjjiiiPK11Vector3_16fh
u32 System::NewWeather(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                       const Vector3_16f *dir, u8 numWeatherEffectsNow) {
    dPa_c::level_c::clipCallback_c *callback = &gParticleSysTracker->mWeatherCallback;
    callback->enabled = numWeatherEffectsNow;
    return New(uniqueID, effectID, x, y, z, dir, (Callback *)callback);
}

// @symbol _ZN8Particle6System17NewUnkCallback818EjjiiiPK11Vector3_16f
u32 System::NewUnkCallback818(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                              const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir,
               (Callback *)&gParticleSysTracker->mCleanParticleCallback);
}

// @symbol _ZN8Particle20RunningSlidingDustAtEiii
void RunningSlidingDustAt(Fix12i x, Fix12i y, Fix12i z) {
    gParticleSysTracker->mRunningSlidingDustSystemID =
        System::New(gParticleSysTracker->mRunningSlidingDustSystemID, 0xda, x, y, z, 0,
                    (Callback *)&gParticleSysTracker->mRunningSlidingDustCallback);
}

// @symbol _ZN8Particle24MegaRunningSlidingDustAtEiii
/* RunningSlidingDustAt for Mega Mario: daPly_c calls this instead whenever
   mIsMega is set. Effect 0x61 is effect 0xda's dust (same texture, shape,
   lifetime and animation) at twice the emitter radius. */
void MegaRunningSlidingDustAt(Fix12i x, Fix12i y, Fix12i z) {
    gParticleSysTracker->mMegaRunningSlidingDustSystemID =
        System::New(gParticleSysTracker->mMegaRunningSlidingDustSystemID, 0x61, x, y, z, 0,
                    (Callback *)&gParticleSysTracker->mMegaRunningSlidingDustCallback);
}

// @symbol _ZN8Particle6System12NewBigSplashEiii
void System::NewBigSplash(Fix12i x, Fix12i y, Fix12i z) {
    gParticleSysTracker->mBigSplashSystemID =
        New(gParticleSysTracker->mBigSplashSystemID, 0xdd, x, y, z, 0,
            (Callback *)&gParticleSysTracker->mBigSplashCallback);
}
}

extern "C" {

// @symbol func_02022a4c
u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z)
{
    Particle::SysTracker *tracker = gParticleSysTracker;
    u32 result = Particle::System::New(tracker->mSystemID_774, 0xdf, x, y, z, 0,
                                       (Particle::Callback *)&tracker->mBigSplashCallback);
    gParticleSysTracker->mSystemID_774 = result;
    return result;
}

// @symbol func_020229f0
u32 func_020229f0(Fix12i x, Fix12i y, Fix12i z)
{
    Particle::SysTracker *tracker = gParticleSysTracker;
    u32 result = Particle::System::New(tracker->mBubbleSystemID, 0xc4, x, y, z, 0,
                                       (Particle::Callback *)&tracker->mBubbleCallback);
    gParticleSysTracker->mBubbleSystemID = result;
    return result;
}
}

namespace Particle {

// @symbol _ZN8Particle6System9NewRippleEiii
u32 System::NewRipple(Fix12i x, Fix12i y, Fix12i z) {
    SysTracker *tracker = gParticleSysTracker;
    u32 result =
        New(tracker->mRippleSystemID, 0x109, x, y, z, 0, (Callback *)&tracker->mRippleCallback);
    gParticleSysTracker->mRippleSystemID = result;
    return result;
}

}

extern "C" {

// @symbol func_0202293c
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z)
{
    Particle::SysTracker *tracker = gParticleSysTracker;
    u32 result = Particle::System::New(tracker->mSystemID_798, 0x118, x, y, z, 0,
                                       (Particle::Callback *)&tracker->mCallback_79c);
    gParticleSysTracker->mSystemID_798 = result;
    return result;
}

// @symbol func_020228dc
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z)
{
    Particle::SysTracker *tracker = gParticleSysTracker;
    u32 result = Particle::System::New(tracker->mSystemID_7a4, 0x117, x, y, z, 0,
                                       (Particle::Callback *)&tracker->mCallback_7a8);
    gParticleSysTracker->mSystemID_7a4 = result;
    return result;
}

// @symbol func_02022864
void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    gParticleSysTracker->mCallback_7b4.scale = scale;
    gParticleSysTracker->mCallback_7b4.velocity = velocity;
    gParticleSysTracker->mSystemID_7b0 =
        Particle::System::New(gParticleSysTracker->mSystemID_7b0, 0x51, x, y, z, 0,
                              (Particle::Callback *)&gParticleSysTracker->mCallback_7b4);
}

// @symbol func_020227ec
void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    gParticleSysTracker->mCallback_7c4.scale = scale;
    gParticleSysTracker->mCallback_7c4.velocity = velocity;
    gParticleSysTracker->mSystemID_7c0 =
        Particle::System::New(gParticleSysTracker->mSystemID_7c0, 0x52, x, y, z, 0,
                              (Particle::Callback *)&gParticleSysTracker->mCallback_7c4);
}

// @symbol func_02022774
void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    gParticleSysTracker->mCallback_7d4.scale = scale;
    gParticleSysTracker->mCallback_7d4.velocity = velocity;
    gParticleSysTracker->mSystemID_7d0 =
        Particle::System::New(gParticleSysTracker->mSystemID_7d0, 0x4f, x, y, z, 0,
                              (Particle::Callback *)&gParticleSysTracker->mCallback_7d4);
}

// @symbol func_020226fc
void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    gParticleSysTracker->mCallback_7e4.scale = scale;
    gParticleSysTracker->mCallback_7e4.velocity = velocity;
    gParticleSysTracker->mSystemID_7e0 =
        Particle::System::New(gParticleSysTracker->mSystemID_7e0, 0x50, x, y, z, 0,
                              (Particle::Callback *)&gParticleSysTracker->mCallback_7e4);
}
}

namespace Particle {

// @symbol _ZN8Particle19SetSelfDestructFlagEj
void SetSelfDestructFlag(u32 definitionID)
{
    gParticleSysTracker->mManager->mDefinitions[definitionID].data->flags |= 0x4000;
}

}
