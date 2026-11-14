//cpp
/* engine/particle/Particle.cpp -- Particle TU (arm9 0x020226d4..0x020233d4)
 *
 * The singleton particle engine's core translation unit: dPa_c's lifecycle
 * (construct, Initialise, Update, destruct; the tree called the class
 * Particle::SysTracker until notes/particle-dpa-naming.md), the System::New front door and
 * its fixed-effect-id wrapper family, the Texture VRAM allocators, RenderAll,
 * and SetSelfDestructFlag. The constructor publishes the current dPa_c in the
 * global pointer gParticleControl; every wrapper reaches it through that.
 *
 * mwccarm emits .text in reverse source order here, so the definitions below
 * run ROM-descending; the roster above reads ROM-ascending. */
#include "types.h"
#include "Heap.h"
#include "decl_common.h"
#include "msl/string.h"
#include "dExtSimpleModel_c.h"
#include "dPa_c.h"
#include "Particle__Texture.h"
#include "Particle__Manager.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/os/cache.h"
#include "Memory.h"
#include "private/particle_resource_header.h"

void func_02049ee8(Particle::Manager *manager, Matrix4x3 *renderState);
extern "C" {

void DecompressLZ16(const void* src, void* dst);
}

extern Matrix4x3 data_0209b3ec;
extern u32 sParticlePalVramLowCursor;
extern u32 sParticlePalVramHighCursor;
extern signed char gCurrentSublevel;

dPa_c::dPa_c() {
    gParticleControl = this;
    mManager = 0;
    mRunningSlidingDustSystemID = 0;
    mMegaRunningSlidingDustSystemID = 0;
    mBigSplashSystemID = 0;
    mSplashSystemID = 0;
    mBubbleSystemID = 0;
    mRippleSystemID = 0;
    mRedFlameSystemID = 0;
    mBlueFlameSystemID = 0;
    mFlamethrowerRedASystemID = 0;
    mFlamethrowerRedBSystemID = 0;
    mFitWaterRaisedCallback.waterOffset = 0x4b000;
}

dPa_c::~dPa_c() {
    if (sParticleHeapBase != 0) {
        mLevel.cleanup();
        ::operator delete[](sParticleHeapBase);
        sParticleHeapBase = 0;
    }
    if (mResourceFile != data_02075f14) {
        Memory::Deallocate(mResourceFile);
    }
    gParticleControl = 0;
}

extern "C" char *func_02023178(int size) {
    char *block = (char *)sParticleHeapCursor;
    sParticleHeapCursor = block + size;
    return block;
}

namespace Particle {

u32 Texture::AllocTexVram(u32 size, bool isTexel4x4)
{
    if (isTexel4x4) {
        u32 address = sParticleTex4x4VramCursor;
        sParticleTex4x4VramCursor = address + size;
        return address;
    }

    return dExtSimpleModel_c::GetVramOffset(size);
}

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

}

void dPa_c::Initialise() {
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
        char *base = (char *)::operator new[](allocSize);
        char* end = base + allocSize;
        sParticleHeapBase = base;
        sParticleHeapCursor = base;
        sParticleHeapEnd = end;
    }

    sParticleTex4x4VramCursor = func_02045d10();
    sParticlePalVramLowCursor = func_02045cf0();
    sParticlePalVramHighCursor = func_02045ce0();

    mManager = (Particle::Manager *)func_0204a4c8(func_02023178, countA, countB, 0x1a, 0x3e);
    mManager->mPolygonAttr = 0x8000;

    if (memcmp((u8 *)data_02075f14, data_0208f668, 4) != 0) {
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
        DC_StoreRange(dst, size);
        mResourceFile = dst;
    }

    func_0204a17c(mManager, (char *)mResourceFile);
    func_0204a0dc(mManager, (u32(*)(u32, u32))Particle::Texture::AllocTexVram);
    func_0204a028(mManager, (u32(*)(u32, u32))Particle::Texture::AllocPalVram);

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

void dPa_c::Update() {
    mLevel.execute();
    func_02049f58(mManager);
}

namespace Particle {

void RenderAll()
{
    dPa_c *particle = gParticleControl;
    if (!particle)
        return;

    func_02049ee8(particle->mManager, &data_0209b3ec);
}

void *System::NewSimple(u32 uniqueID, fx32 x, fx32 y, fx32 z) {
    Vector3 pos;
    pos.x = x >> 3;
    pos.y = y >> 3;
    pos.z = z >> 3;
    return gParticleControl->mManager->AddSystem(uniqueID, pos);
}

System *System::FromUniqueID(u32 uniqueID) {
    dPa_c::level_c::emitter_c *emitter = gParticleControl->mLevel.get(uniqueID);
    return emitter->mEmitter;
}

/* A live uniqueID moves its system to the new position (and direction, when
 * one is given) and re-runs the callback's SpawnParticles; anything else
 * starts a fresh system of definitionID. */
int System::New(u32 uniqueID, u32 definitionID, fx32 x, fx32 y, fx32 z, const Vector3_16f *dir,
                dPa_c::level_c::callback_c *callback) {
    dPa_c::level_c::emitter_c *found;
    dPa_c::level_c *level;
    dPa_c::level_c::emitter_c *emitter;
    Vector3 pos;
    pos.x = x >> 3;
    level = &gParticleControl->mLevel;
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

u32 System::NewCheckWater(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                          const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mCheckWaterCallback);
}

u32 System::NewCheckWaterRipple(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                                const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mCheckWaterRippleCallback);
}

u32 System::NewFitWater(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                        const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mFitWaterCallback);
}

u32 System::NewFitWaterRaised(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                              const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mFitWaterRaisedCallback);
}

/* System::New with dPa_c's checkYoganCallback_c ("yogan" is lava);
   daObjLava_c's popping lava bubbles are its one caller. */
u32 System::NewCheckYogan(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                          const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mCheckYoganCallback);
}
}

namespace Particle {

u32 System::NewWeather(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z, const Vector3_16f *dir,
                       u8 numWeatherEffectsNow) {
    dPa_c::level_c::clipCallback_c *callback = &gParticleControl->mWeatherCallback;
    callback->enabled = numWeatherEffectsNow;
    return New(uniqueID, effectID, x, y, z, dir, callback);
}

u32 System::NewCleanParticle(u32 uniqueID, u32 effectID, fx32 x, fx32 y, fx32 z,
                             const Vector3_16f *dir) {
    return New(uniqueID, effectID, x, y, z, dir, &gParticleControl->mCleanParticleCallback);
}

void RunningSlidingDustAt(fx32 x, fx32 y, fx32 z) {
    gParticleControl->mRunningSlidingDustSystemID =
        System::New(gParticleControl->mRunningSlidingDustSystemID, kPtclRunningDust, x, y, z, 0,
                    &gParticleControl->mRunningSlidingDustCallback);
}
}

namespace Particle {

/* RunningSlidingDustAt for Mega Mario: daPly_c calls this instead whenever
   mIsMega is set. Effect 0x61 is effect 0xda's dust (same texture, shape,
   lifetime and animation) at twice the emitter radius. */
void MegaRunningSlidingDustAt(fx32 x, fx32 y, fx32 z) {
    gParticleControl->mMegaRunningSlidingDustSystemID =
        System::New(gParticleControl->mMegaRunningSlidingDustSystemID, kPtclMegaRunningDust, x, y,
                    z, 0, &gParticleControl->mMegaRunningSlidingDustCallback);
}
}

namespace Particle {

void System::NewBigSplash(fx32 x, fx32 y, fx32 z) {
    gParticleControl->mBigSplashSystemID = New(gParticleControl->mBigSplashSystemID, kPtclBigSplash,
                                               x, y, z, 0, &gParticleControl->mBigSplashCallback);
}
}

namespace Particle {

u32 System::NewSplash(fx32 x, fx32 y, fx32 z) {
    dPa_c *particle = gParticleControl;
    u32 result =
        New(particle->mSplashSystemID, kPtclSplash, x, y, z, 0, &particle->mBigSplashCallback);
    gParticleControl->mSplashSystemID = result;
    return result;
}

u32 System::NewBubble(fx32 x, fx32 y, fx32 z) {
    dPa_c *particle = gParticleControl;
    u32 result =
        New(particle->mBubbleSystemID, kPtclBubble, x, y, z, 0, &particle->mBubbleCallback);
    gParticleControl->mBubbleSystemID = result;
    return result;
}
}

namespace Particle {

u32 System::NewRipple(fx32 x, fx32 y, fx32 z) {
    dPa_c *particle = gParticleControl;
    u32 result =
        New(particle->mRippleSystemID, kPtclRipple, x, y, z, 0, &particle->mRippleCallback);
    gParticleControl->mRippleSystemID = result;
    return result;
}
}

namespace Particle {

/* daObjFire_c's RED_FLAME. Effect 0x118 is kPtclFlame's red-orange
   flipbook (textures 14..21) as a continuous emitter. */
u32 System::NewRedFlame(fx32 x, fx32 y, fx32 z) {
    dPa_c *particle = gParticleControl;
    u32 result =
        New(particle->mRedFlameSystemID, kPtclObjRedFire, x, y, z, 0, &particle->mRedFlameCallback);
    gParticleControl->mRedFlameSystemID = result;
    return result;
}

/* daObjFire_c's BLUE_FLAME (actor 0x13d): effect 0x117, NewRedFlame's
   emitter on the other flame flipbook (textures 6..13). */
u32 System::NewBlueFlame(fx32 x, fx32 y, fx32 z) {
    dPa_c *particle = gParticleControl;
    u32 result = New(particle->mBlueFlameSystemID, kPtclObjBlueFire, x, y, z, 0,
                     &particle->mBlueFlameCallback);
    gParticleControl->mBlueFlameSystemID = result;
    return result;
}

void System::NewFlamethrowerRedA(fx32 x, fx32 y, fx32 z, s16 scale, s16 velocity) {
    gParticleControl->mFlamethrowerRedACallback.scale = scale;
    gParticleControl->mFlamethrowerRedACallback.velocity = velocity;
    gParticleControl->mFlamethrowerRedASystemID =
        New(gParticleControl->mFlamethrowerRedASystemID, kPtclFlamethrowerRedA, x, y, z, 0,
            &gParticleControl->mFlamethrowerRedACallback);
}

void System::NewFlamethrowerRedB(fx32 x, fx32 y, fx32 z, s16 scale, s16 velocity) {
    gParticleControl->mFlamethrowerRedBCallback.scale = scale;
    gParticleControl->mFlamethrowerRedBCallback.velocity = velocity;
    gParticleControl->mFlamethrowerRedBSystemID =
        New(gParticleControl->mFlamethrowerRedBSystemID, kPtclFlamethrowerRedB, x, y, z, 0,
            &gParticleControl->mFlamethrowerRedBCallback);
}

void System::NewFlamethrowerBlueA(fx32 x, fx32 y, fx32 z, s16 scale, s16 velocity) {
    gParticleControl->mFlamethrowerBlueACallback.scale = scale;
    gParticleControl->mFlamethrowerBlueACallback.velocity = velocity;
    gParticleControl->mFlamethrowerBlueASystemID =
        New(gParticleControl->mFlamethrowerBlueASystemID, kPtclFlamethrowerBlueA, x, y, z, 0,
            &gParticleControl->mFlamethrowerBlueACallback);
}

void System::NewFlamethrowerBlueB(fx32 x, fx32 y, fx32 z, s16 scale, s16 velocity) {
    gParticleControl->mFlamethrowerBlueBCallback.scale = scale;
    gParticleControl->mFlamethrowerBlueBCallback.velocity = velocity;
    gParticleControl->mFlamethrowerBlueBSystemID =
        New(gParticleControl->mFlamethrowerBlueBSystemID, kPtclFlamethrowerBlueB, x, y, z, 0,
            &gParticleControl->mFlamethrowerBlueBCallback);
}
}

namespace Particle {

void SetSelfDestructFlag(u32 definitionID)
{
    gParticleControl->mManager->mDefinitions[definitionID].data->flags |= 0x4000;
}

}
