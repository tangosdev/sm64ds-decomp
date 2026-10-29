//cpp
/* Particle::SysTracker::Contents, arm9 .text 0x02021a04..0x02021e40.
 *
 * The live-system registry: 64 entries and 16 buckets, hashed by
 * uniqueID & 0xf. Entry::Initialise and Entry::Reset set one node up
 * and tear it down. Contents walks the pool, creates a system and
 * links or unlinks the node.
 *
 * leftovers:
 * - Entry::Initialise copies system into a local before writing the
 *   direction. Writing through the member directly DIFFs Initialise.
 * - Entry::Reset clears callbackFlags bit 0 and then sets it. A
 *   single |= on the field DIFFs Reset.
 * - Update widens active == 1 to an int before testing it. A plain
 *   if on entry->active DIFFs Update.
 * - system->flags.unk_000, unk_006 and unk_00c stay the bitfield
 *   names on Particle::System.
 * - data_0209ee74 is the global tracker. This TU does not own it.
 */

#include "Particle__System.h"

namespace Particle {

// @symbol _ZN8Particle10SysTracker8Contents5Entry10InitialiseEjjR7Vector3PK11Vector3_16fPN5dPa_c7level_c10callback_cE
bool SysTracker::Contents::Entry::Initialise(
    u32 newUniqueID, u32 newDefinitionID, Vector3& position,
    const Vector3_16f *direction,
    dPa_c::level_c::callback_c *newCallback)
{
    SystemDefinition::Data& data =
        *data_0209ee74->mManager->mDefinitions[newDefinitionID].data;

    data.flags &= ~0x4000;
    system = data_0209ee74->mManager->AddSystem(newDefinitionID, position);
    if (system == 0)
        return false;

    if (newDefinitionID == 0x52 || newDefinitionID == 0x50) {
        if (system->flags.unk_000 <= 5 && system->flags.unk_006 >= 5)
            system->flags.unk_00c = 5;
    }

    savedValue = data.callbackValue;
    data.callbackValue = 0;

    if (direction != 0) {
        System *createdSystem = system;
        createdSystem->direction.x = direction->x;
        createdSystem->direction.y = direction->y;
        createdSystem->direction.z = direction->z;
    }

    if (newCallback != 0)
        newCallback->SpawnParticles(*system);

    uniqueID = newUniqueID;
    definitionID = newDefinitionID;
    active = 1;
    callback = newCallback;
    prev = 0;
    next = 0;
    return true;
}

// @symbol _ZN8Particle10SysTracker8Contents5Entry5ResetEv
void SysTracker::Contents::Entry::Reset()
{
    SystemDefinition::Data& data =
        *data_0209ee74->mManager->mDefinitions[definitionID].data;
    u32 *flagsAddress;
    u32 flags;

    data.callbackValue = savedValue;
    flagsAddress = &system->callbackFlags;
    flags = *flagsAddress;
    flags &= ~1;
    flags |= 1;
    *flagsAddress = flags;
    uniqueID = 0;
}

// @symbol _ZN8Particle10SysTracker8ContentsC1Ev
SysTracker::Contents::Contents()
{
    Entry* entry = mEntries;
    int i;

    do {
        entry->uniqueID = 0;
        entry++;
    } while (entry != &mEntries[0x40]);

    mLastUniqueID = 0;
    mCurrentIndex = 0;
    for (i = 0; i < 0x10; i++)
        mBuckets[i] = 0;
}

// @symbol _ZN8Particle10SysTracker8Contents6UpdateEv
void SysTracker::Contents::Update()
{
    Entry* entry = mEntries;
    int i;

    for (i = 0; i < 0x40; i++, entry++) {
        if (entry->uniqueID == 0)
            continue;

        if (entry->callback != 0) {
            bool callbackActive = entry->active == 1;
            if (entry->callback->OnUpdate(*entry->system, callbackActive))
                entry->active = 1;
        }

        {
            int isActive = (int)(entry->active == 1);
            if (isActive != 0) {
                entry->active = 0;
                continue;
            }
        }

        Unlink(*entry);
        mCurrentIndex = i;
    }
}

// @symbol _ZN8Particle10SysTracker8Contents5ClearEv
void SysTracker::Contents::Clear()
{
    Entry* entry = mEntries;
    int i;

    for (i = 0; i < 0x40; i++, entry++) {
        if (entry->uniqueID != 0) {
            entry->active = 0;
            Unlink(*entry);
            mCurrentIndex = i;
        }
    }
}

// @symbol _ZNK8Particle10SysTracker8Contents8FindDataEj
SysTracker::Contents::Entry *SysTracker::Contents::FindData(u32 uniqueID) const
{
    if (uniqueID == 0)
        return 0;

    Entry *entry = mBuckets[uniqueID & 0xf];
    while (entry) {
        if (uniqueID == entry->uniqueID)
            return entry;
        entry = entry->next;
    }
    return 0;
}

// @symbol _ZN8Particle10SysTracker8Contents6CreateEjR7Vector3PK11Vector3_16fPN5dPa_c7level_c10callback_cE
u32 SysTracker::Contents::Create(
    u32 definitionID, Vector3& position, const Vector3_16f *direction,
    dPa_c::level_c::callback_c *callback)
{
    int i;

    for (i = 0; i < 0x40; i++) {
        if (mEntries[mCurrentIndex].uniqueID == 0) {
            ++mLastUniqueID;
            if (mLastUniqueID == 0)
                ++mLastUniqueID;

            if (!mEntries[mCurrentIndex].Initialise(mLastUniqueID, definitionID, position,
                                                    direction, callback))
                return 0;

            Link(mEntries[mCurrentIndex]);
            return mLastUniqueID;
        }

        mCurrentIndex = (mCurrentIndex + 1) % 0x40;
    }

    return 0;
}

// @symbol _ZN8Particle10SysTracker8Contents4LinkERNS1_5EntryE
void SysTracker::Contents::Link(Entry& entry)
{
    int index = entry.uniqueID & 0xf;
    Entry* head = mBuckets[index];

    if (head == 0) {
        mBuckets[index] = &entry;
    } else {
        head->prev = &entry;
        entry.next = mBuckets[index];
        mBuckets[index] = &entry;
    }
}

// @symbol _ZN8Particle10SysTracker8Contents6UnlinkERNS1_5EntryE
void SysTracker::Contents::Unlink(Entry& entry)
{
    Entry* prev = entry.prev;
    Entry* next = entry.next;

    if (prev == 0)
        mBuckets[entry.uniqueID & 0xf] = next;
    else
        prev->next = next;

    if (next != 0)
        next->prev = prev;

    entry.Reset();
}

} // namespace Particle
