//cpp
// @symbol _ZN8Particle19SetSelfDestructFlagEj
// @symbol _ZN8Particle6System20NewFlamethrowerBlueBEiiiss
// @symbol _ZN8Particle6System20NewFlamethrowerBlueAEiiiss
// @symbol _ZN8Particle6System19NewFlamethrowerRedBEiiiss
// @symbol _ZN8Particle6System19NewFlamethrowerRedAEiiiss
// @symbol _ZN8Particle6System11NewBlueFireEiii
// @symbol _ZN8Particle6System10NewRedFireEiii
// @symbol _ZN8Particle6System9NewRippleEiii
// @symbol _ZN8Particle6System9NewBubbleEiii
// @symbol _ZN8Particle6System9NewSplashEiii
// @symbol _ZN8Particle6System12NewBigSplashEiii
// @symbol _ZN8Particle17MegaRunningDustAtEiii
// @symbol _ZN8Particle20RunningSlidingDustAtEiii
// @symbol _ZN8Particle6System16NewCleanParticleEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System10NewWeatherEjjiiiPK11Vector3_16fh
// @symbol _ZN8Particle6System13NewCheckYoganEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System17NewFitWaterRaisedEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System11NewFitWaterEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System19NewCheckWaterRippleEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System13NewCheckWaterEjjiiiPK11Vector3_16f
// @symbol _ZN8Particle6System3NewEjjiiiPK11Vector3_16fPN5dPa_c7level_c10callback_cE
// @symbol _ZN8Particle6System12FromUniqueIDEj
// @symbol _ZN8Particle6System9NewSimpleEjiii
// @symbol _ZN8Particle9RenderAllEv
// @symbol _ZN5dPa_c6UpdateEv
// @symbol _ZN5dPa_c10InitialiseEv
// @symbol _ZN8Particle7Texture12AllocPalVramEjb
// @symbol _ZN8Particle7Texture12AllocTexVramEjb
// @symbol func_02023178
// @symbol _ZN5dPa_cD1Ev
// @symbol _ZN5dPa_cC1Ev
/* engine/particle/Particle.cpp -- Particle TU (arm9 0x020226d4..0x020233d4)
 *
 * The singleton particle engine's core translation unit: dPa_c's lifecycle
 * (construct, Initialise, Update, destruct; the tree called the class
 * Particle::SysTracker until notes/particle-dpa-naming.md), the System::New front door and
 * its fixed-effect-id wrapper family, the Texture VRAM allocators, RenderAll,
 * and SetSelfDestructFlag. The current dPa_c is the single global
 * at data_0209ee74; every wrapper reaches it directly.
 *
 * mwccarm emits .text in reverse source order here, so the definitions below
 * run ROM-descending; the roster above reads ROM-ascending. */
#include "types.h"
#include "Heap.h"
#include "decl_common.h"
#include "dExtSimpleModel_c.h"
#include "dPa_c.h"
#include "Particle__Texture.h"
#include "Particle__Manager.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "CP15.h"
#include "Memory.h"
#include "private/particle_resource_header.h"

extern "C" {
void func_0203cbc0(void *p);
void func_02049ee8(void* manager, void* renderState);
void DecompressLZ16(const void* src, void* dst);
}

extern Matrix4x3 data_0209b3ec;
extern u32 data_0209ee84;
extern u32 data_0209ee8c;
extern signed char gCurrentSublevel;

// @symbol _ZN5dPa_cC1Ev
dPa_c::dPa_c()
{
    data_0209ee74 = this;
    mManager = 0;
    mRunningSlidingDustSystemID = 0;
    mMegaRunningDustSystemID = 0;
    mBigSplashSystemID = 0;
    mSplashSystemID = 0;
    mBubbleSystemID = 0;
    mRippleSystemID = 0;
    mRedFireSystemID = 0;
    mBlueFireSystemID = 0;
    mFlamethrowerRedASystemID = 0;
    mFlamethrowerRedBSystemID = 0;
    mFitWaterRaisedCallback.waterOffset = 0x4b000;
}

// @symbol _ZN5dPa_cD1Ev
dPa_c::~dPa_c()
{
    if (data_0209ee80 != 0) {
        mLevel.cleanup();
        func_0203cbc0(data_0209ee80);
        data_0209ee80 = 0;
    }
    if (mResourceFile != data_02075f14) {
        Memory::Deallocate(mResourceFile);
    }
    data_0209ee74 = 0;
}

extern "C" {

// @symbol func_02023178
char *func_02023178(int size) {
    char *block = (char *)data_0209ee78;
    data_0209ee78 = block + size;
    return block;
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

    return dExtSimpleModel_c::GetVramOffset(size);
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

}

// @symbol _ZN5dPa_c10InitialiseEv
void dPa_c::Initialise()
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
        data_0209ee80 = base;
        data_0209ee78 = base;
        data_0209ee7c = end;
    }

    data_0209ee88 = func_02045d10();
    data_0209ee84 = func_02045cf0();
    data_0209ee8c = func_02045ce0();

    mManager = (Particle::Manager*)func_0204a4c8(
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
        (u32 (*)(const void *, u32))Particle::Texture::AllocTexVram);
    func_0204a028(mManager,
        (u32 (*)(u32, u32))Particle::Texture::AllocPalVram);

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

// @symbol _ZN5dPa_c6UpdateEv
void dPa_c::Update()
{
    mLevel.execute();
    func_02049f58(mManager);
}

namespace Particle {

// @symbol _ZN8Particle9RenderAllEv
void RenderAll()
{
    dPa_c *particle = data_0209ee74;
    if (!particle)
        return;

    func_02049ee8(particle->mManager, &data_0209b3ec);
}

// @symbol _ZN8Particle6System9NewSimpleEjiii
void *System::NewSimple(u32 uniqueID, Fix12i x, Fix12i y, Fix12i z) {
    Vector3 pos;
    pos.x = x >> 3;
    pos.y = y >> 3;
    pos.z = z >> 3;
    return data_0209ee74->mManager->AddSystem(uniqueID, pos);
}

// @symbol _ZN8Particle6System12FromUniqueIDEj
System *System::FromUniqueID(u32 uniqueID)
{
    dPa_c::level_c::emitter_c *emitter =
        data_0209ee74->mLevel.get(uniqueID);
    return emitter->mEmitter;
}

/* A live uniqueID moves its system to the new position (and direction, when
 * one is given) and re-runs the callback's SpawnParticles; anything else
 * starts a fresh system of definitionID. */
// @symbol _ZN8Particle6System3NewEjjiiiPK11Vector3_16fPN5dPa_c7level_c10callback_cE
int System::New(u32 uniqueID, u32 definitionID, Fix12i x, Fix12i y, Fix12i z,
                const Vector3_16f *dir, dPa_c::level_c::callback_c *callback) {
    dPa_c::level_c::emitter_c *found;
    dPa_c::level_c *level;
    dPa_c::level_c::emitter_c *emitter;
    Vector3 pos;
    pos.x = x >> 3;
    level = &data_0209ee74->mLevel;
    pos.y = y >> 3;
    pos.z = (z >> 1) >> 2;
    found = level->get(uniqueID);
    emitter = found;
    if (emitter) {
        System *system;
        dPa_c::level_c::callback_c *entryCallback;
        emitter->mStatus = 1;
        system = emitter->mEmitter;
        system->position.x = pos.x;
        system->position.y = pos.y;
        system->position.z = pos.z;
        if (dir != 0) {
            system->direction.x = dir->x;
            system->direction.y = dir->y;
            system->direction.z = dir->z;
        }
        entryCallback = emitter->mCallback;
        if (entryCallback != 0) {
            entryCallback->SpawnParticles(*system);
        }
        return emitter->mId;
    }
    return level->entry(definitionID, pos, dir, callback);
}
}

namespace Particle {

// @symbol _ZN8Particle6System13NewCheckWaterEjjiiiPK11Vector3_16f
u32 System::NewCheckWater(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                          const Vector3_16f *dir)
{
    return New(uniqueID, effectID, x, y, z, dir,
               &data_0209ee74->mCheckWaterCallback);
}

// @symbol _ZN8Particle6System19NewCheckWaterRippleEjjiiiPK11Vector3_16f
u32 System::NewCheckWaterRipple(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                                const Vector3_16f *dir)
{
    return New(uniqueID, effectID, x, y, z, dir,
               &data_0209ee74->mCheckWaterRippleCallback);
}

// @symbol _ZN8Particle6System11NewFitWaterEjjiiiPK11Vector3_16f
u32 System::NewFitWater(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                        const Vector3_16f *dir)
{
    return New(uniqueID, effectID, x, y, z, dir,
               &data_0209ee74->mFitWaterCallback);
}

// @symbol _ZN8Particle6System17NewFitWaterRaisedEjjiiiPK11Vector3_16f
u32 System::NewFitWaterRaised(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                              const Vector3_16f *dir)
{
    return New(uniqueID, effectID, x, y, z, dir,
               &data_0209ee74->mFitWaterRaisedCallback);
}

// @symbol _ZN8Particle6System13NewCheckYoganEjjiiiPK11Vector3_16f
u32 System::NewCheckYogan(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                          const Vector3_16f *dir)
{
    return New(uniqueID, effectID, x, y, z, dir,
               &data_0209ee74->mCheckYoganCallback);
}
}

namespace Particle {

// @symbol _ZN8Particle6System10NewWeatherEjjiiiPK11Vector3_16fh
u32 System::NewWeather(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                       const Vector3_16f *dir, u8 numWeatherEffectsNow) {
    dPa_c::level_c::clipCallback_c *callback = &data_0209ee74->mWeatherCallback;
    callback->enabled = numWeatherEffectsNow;
    return New(uniqueID, effectID, x, y, z, dir, callback);
}

// @symbol _ZN8Particle6System16NewCleanParticleEjjiiiPK11Vector3_16f
u32 System::NewCleanParticle(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                              const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &data_0209ee74->mCleanParticleCallback);
}

// @symbol _ZN8Particle20RunningSlidingDustAtEiii
void RunningSlidingDustAt(Fix12i x, Fix12i y, Fix12i z) {
    data_0209ee74->mRunningSlidingDustSystemID =
        System::New(data_0209ee74->mRunningSlidingDustSystemID, kPtclRunningDust, x, y, z, 0,
                    &data_0209ee74->mRunningSlidingDustCallback);
}

}

namespace Particle {

// @symbol _ZN8Particle17MegaRunningDustAtEiii
void MegaRunningDustAt(Fix12i x, Fix12i y, Fix12i z)
{
    data_0209ee74->mMegaRunningDustSystemID =
        System::New(data_0209ee74->mMegaRunningDustSystemID, kPtclMegaRunningDust, x, y, z, 0,
                    &data_0209ee74->mMegaRunningDustCallback);
}
}

namespace Particle {

// @symbol _ZN8Particle6System12NewBigSplashEiii
void System::NewBigSplash(Fix12i x, Fix12i y, Fix12i z) {
    data_0209ee74->mBigSplashSystemID = New(data_0209ee74->mBigSplashSystemID, kPtclBigSplash, x, y, z, 0,
                                            &data_0209ee74->mBigSplashCallback);
}

}

namespace Particle {

// @symbol _ZN8Particle6System9NewSplashEiii
u32 System::NewSplash(Fix12i x, Fix12i y, Fix12i z)
{
    dPa_c *particle = data_0209ee74;
    u32 result = New(particle->mSplashSystemID, kPtclSplash, x, y, z, 0,
                     &particle->mBigSplashCallback);
    data_0209ee74->mSplashSystemID = result;
    return result;
}

// @symbol _ZN8Particle6System9NewBubbleEiii
u32 System::NewBubble(Fix12i x, Fix12i y, Fix12i z)
{
    dPa_c *particle = data_0209ee74;
    u32 result = New(particle->mBubbleSystemID, kPtclBubble, x, y, z, 0,
                     &particle->mBubbleCallback);
    data_0209ee74->mBubbleSystemID = result;
    return result;
}
}

namespace Particle {

// @symbol _ZN8Particle6System9NewRippleEiii
u32 System::NewRipple(Fix12i x, Fix12i y, Fix12i z) {
    dPa_c *particle = data_0209ee74;
    u32 result =
        New(particle->mRippleSystemID, kPtclRipple, x, y, z, 0, &particle->mRippleCallback);
    data_0209ee74->mRippleSystemID = result;
    return result;
}

}

namespace Particle {

// @symbol _ZN8Particle6System10NewRedFireEiii
u32 System::NewRedFire(Fix12i x, Fix12i y, Fix12i z)
{
    dPa_c *particle = data_0209ee74;
    u32 result = New(particle->mRedFireSystemID, kPtclObjRedFire, x, y, z, 0,
                     &particle->mRedFireCallback);
    data_0209ee74->mRedFireSystemID = result;
    return result;
}

// @symbol _ZN8Particle6System11NewBlueFireEiii
u32 System::NewBlueFire(Fix12i x, Fix12i y, Fix12i z)
{
    dPa_c *particle = data_0209ee74;
    u32 result = New(particle->mBlueFireSystemID, kPtclObjBlueFire, x, y, z, 0,
                     &particle->mBlueFireCallback);
    data_0209ee74->mBlueFireSystemID = result;
    return result;
}

// @symbol _ZN8Particle6System19NewFlamethrowerRedAEiiiss
void System::NewFlamethrowerRedA(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    data_0209ee74->mFlamethrowerRedACallback.scale = scale;
    data_0209ee74->mFlamethrowerRedACallback.velocity = velocity;
    data_0209ee74->mFlamethrowerRedASystemID =
        New(data_0209ee74->mFlamethrowerRedASystemID, kPtclFlamethrowerRedA, x, y, z, 0,
            &data_0209ee74->mFlamethrowerRedACallback);
}

// @symbol _ZN8Particle6System19NewFlamethrowerRedBEiiiss
void System::NewFlamethrowerRedB(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    data_0209ee74->mFlamethrowerRedBCallback.scale = scale;
    data_0209ee74->mFlamethrowerRedBCallback.velocity = velocity;
    data_0209ee74->mFlamethrowerRedBSystemID =
        New(data_0209ee74->mFlamethrowerRedBSystemID, kPtclFlamethrowerRedB, x, y, z, 0,
            &data_0209ee74->mFlamethrowerRedBCallback);
}

// @symbol _ZN8Particle6System20NewFlamethrowerBlueAEiiiss
void System::NewFlamethrowerBlueA(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    data_0209ee74->mFlamethrowerBlueACallback.scale = scale;
    data_0209ee74->mFlamethrowerBlueACallback.velocity = velocity;
    data_0209ee74->mFlamethrowerBlueASystemID =
        New(data_0209ee74->mFlamethrowerBlueASystemID, kPtclFlamethrowerBlueA, x, y, z, 0,
            &data_0209ee74->mFlamethrowerBlueACallback);
}

// @symbol _ZN8Particle6System20NewFlamethrowerBlueBEiiiss
void System::NewFlamethrowerBlueB(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity) {
    data_0209ee74->mFlamethrowerBlueBCallback.scale = scale;
    data_0209ee74->mFlamethrowerBlueBCallback.velocity = velocity;
    data_0209ee74->mFlamethrowerBlueBSystemID =
        New(data_0209ee74->mFlamethrowerBlueBSystemID, kPtclFlamethrowerBlueB, x, y, z, 0,
            &data_0209ee74->mFlamethrowerBlueBCallback);
}
}

namespace Particle {

// @symbol _ZN8Particle19SetSelfDestructFlagEj
void SetSelfDestructFlag(u32 definitionID)
{
    data_0209ee74->mManager->mDefinitions[definitionID].data->flags |= 0x4000;
}

}
