//cpp
/* dPa_c::level_c, arm9 .text 0x02021a04..0x02021e40.
 *
 * The level-emitter registry: 64 emitter_c slots chained in 16 hash
 * buckets by id & 0xf. emitter_c::entry and emitter_c::cleanup set one
 * slot up and tear it down. level_c walks the slots, creates a system and
 * adds it to or cuts it from the table.
 *
 * Names: `dPa_c::level_c` is ROM RTTI (the callback classes are nested in
 * it). The member names follow Twilight Princess's dPa_control_c::level_c,
 * whose routines are this algorithm; see include/dPa_c.h.
 *
 * leftovers:
 * - emitter_c::entry copies mEmitter into a local before writing the
 *   direction. Writing through the member directly DIFFs entry.
 * - emitter_c::cleanup clears callbackFlags bit 0 and then sets it. A
 *   single |= on the field DIFFs cleanup.
 * - execute widens mStatus == 1 to an int before testing it. A plain
 *   if on emitter->mStatus DIFFs execute.
 * - gParticleControl is the global dPa_c. This TU does not own it.
 */
#include "Particle__System.h"

bool dPa_c::level_c::emitter_c::entry(
    u32 id, u32 nameId, Vector3& position,
    const Vector3_16f *direction,
    callback_c *callback)
{
    Particle::SystemDefinition::Data& data =
        *gParticleControl->mManager->mDefinitions[nameId].data;
    data.flags &= ~0x4000;
    mEmitter = gParticleControl->mManager->AddSystem(nameId, position);
    if (mEmitter == 0)
        return false;
    if (nameId == 0x52 || nameId == 0x50) {
        if (mEmitter->polygonIDs.min <= 5 && mEmitter->polygonIDs.max >= 5)
            mEmitter->polygonIDs.current = 5;
    }
    mSavedCallbackValue = data.callbackValue;
    data.callbackValue = 0;
    if (direction != 0) {
        Particle::System *createdSystem = mEmitter;
        createdSystem->direction.x = direction->x;
        createdSystem->direction.y = direction->y;
        createdSystem->direction.z = direction->z;
    }
    if (callback != 0)
        callback->SpawnParticles(*mEmitter);
    mId = id;
    mNameId = nameId;
    mStatus = 1;
    mCallback = callback;
    mPre = 0;
    mNext = 0;
    return true;
}

void dPa_c::level_c::emitter_c::cleanup()
{
    Particle::SystemDefinition::Data& data =
        *gParticleControl->mManager->mDefinitions[mNameId].data;
    u32 *flagsAddress;
    u32 flags;
    data.callbackValue = mSavedCallbackValue;
    flagsAddress = &mEmitter->callbackFlags;
    flags = *flagsAddress;
    flags &= ~1;
    flags |= 1;
    *flagsAddress = flags;
    mId = 0;
}

dPa_c::level_c::level_c()
{
    emitter_c* emitter = mEmitter;
    int i;
    do {
        emitter->mId = 0;
        emitter++;
    } while (emitter != &mEmitter[0x40]);
    mLastId = 0;
    mFreeNo = 0;
    for (i = 0; i < 0x10; i++)
        mTable[i] = 0;
}

void dPa_c::level_c::execute()
{
    emitter_c* emitter = mEmitter;
    int i;
    for (i = 0; i < 0x40; i++, emitter++) {
        if (emitter->mId == 0)
            continue;
        if (emitter->mCallback != 0) {
            bool callbackActive = emitter->mStatus == 1;
            if (emitter->mCallback->OnUpdate(*emitter->mEmitter, callbackActive))
                emitter->mStatus = 1;
        }
        {
            int isActive = (int)(emitter->mStatus == 1);
            if (isActive != 0) {
                emitter->mStatus = 0;
                continue;
            }
        }
        cutTable(emitter);
        mFreeNo = i;
    }
}

void dPa_c::level_c::cleanup()
{
    emitter_c* emitter = mEmitter;
    int i;
    for (i = 0; i < 0x40; i++, emitter++) {
        if (emitter->mId != 0) {
            emitter->mStatus = 0;
            cutTable(emitter);
            mFreeNo = i;
        }
    }
}

dPa_c::level_c::emitter_c *dPa_c::level_c::get(u32 id)
{
    if (id == 0)
        return 0;
    emitter_c *emitter = mTable[id & 0xf];
    while (emitter) {
        if (id == emitter->mId)
            return emitter;
        emitter = emitter->mNext;
    }
    return 0;
}

u32 dPa_c::level_c::entry(
    u32 nameId, Vector3& position, const Vector3_16f *direction,
    callback_c *callback)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (mEmitter[mFreeNo].mId == 0) {
            ++mLastId;
            if (mLastId == 0)
                ++mLastId;
            if (!mEmitter[mFreeNo].entry(mLastId, nameId, position,
                                         direction, callback))
                return 0;
            addTable(&mEmitter[mFreeNo]);
            return mLastId;
        }
        mFreeNo = (mFreeNo + 1) % 0x40;
    }
    return 0;
}

void dPa_c::level_c::addTable(emitter_c *emitter)
{
    int index = emitter->mId & 0xf;
    emitter_c* head = mTable[index];
    if (head == 0) {
        mTable[index] = emitter;
    } else {
        head->mPre = emitter;
        emitter->mNext = mTable[index];
        mTable[index] = emitter;
    }
}

void dPa_c::level_c::cutTable(emitter_c *emitter)
{
    emitter_c* pre = emitter->mPre;
    emitter_c* next = emitter->mNext;
    if (pre == 0)
        mTable[emitter->mId & 0xf] = next;
    else
        pre->mNext = next;
    if (next != 0)
        next->mPre = pre;
    emitter->cleanup();
}
