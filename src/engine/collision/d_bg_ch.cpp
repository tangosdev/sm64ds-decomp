//cpp
/* dBgCh -- the collision-query root (ROM RTTI _ZTS5dBgCh). Owns the
 * contiguous .text run 0x02035354..0x0203553c, ending where
 * d_bg_ch_actr.cpp begins. The lifecycle group is defined first: D2, D0, D1,
 * then C2 at the tail (C1 has no ROM counterpart and is deadstripped);
 * plain members follow in reverse ROM order, so the earliest (IsOwnedBy)
 * lands last.
 */

#include "dBgCh.h"
#include "dActor_c.h"

dBgCh::dBgCh()
{
    mDetectFlags = 1;
    mOwner = 0;
    mOwnerUniqueID = -1;
}

dBgCh::~dBgCh()
{
}

void dBgCh::StopDetectingOrdinary()
{
    mDetectFlags &= ~1;
}

int dBgCh::IsDetectingOrdinary() const {
    return mDetectFlags & 1;
}

void dBgCh::StartDetectingWater()
{
    mDetectFlags |= 2;
}

void dBgCh::StopDetectingWater()
{
    mDetectFlags &= ~2;
}

int dBgCh::IsDetectingWater() const {
    return mDetectFlags & 2;
}

void dBgCh::EnableCameraMode() {
    mDetectFlags |= 4;
}

int dBgCh::IsCameraMode() const {
    return mDetectFlags & 4;
}

void dBgCh::StartDetectingToxic()
{
    mDetectFlags |= 8;
}

int dBgCh::IsDetectingToxic() const {
    return mDetectFlags & 8;
}

void dBgCh::StartIgnoringVanishSurfaces() {
    mDetectFlags |= 0x20;
}

void dBgCh::StopIgnoringVanishSurfaces() {
    mDetectFlags &= ~0x20;
}

int dBgCh::IsIgnoringVanishSurfaces() const {
    return mDetectFlags & 0x20;
}

void dBgCh::StartIgnoringType14Walls() {
    mDetectFlags |= 0x40;
}

void dBgCh::StopIgnoringType14Walls() {
    mDetectFlags &= ~0x40;
}

int dBgCh::IsIgnoringType14Walls() const {
    return mDetectFlags & 0x40;
}

void dBgCh::SetActor(dActor_c *actor) {
    if (!actor) {
        mOwner = 0;
        mOwnerUniqueID = -1;
    } else {
        mOwner = actor;
        mOwnerUniqueID = actor->uniqueID;
    }
}

dBgCh &dBgCh::operator=(const dBgCh &other) {
    mDetectFlags = other.mDetectFlags;
    mOwnerUniqueID = other.mOwnerUniqueID;
    mOwner = other.mOwner;
    return *this;
}

bool dBgCh::IsOwnedBy(dActor_c *actor) const {
    if (actor == 0 || mOwner == 0)
        return false;
    u32 uniqueID = actor->uniqueID;
    if (mOwner == actor && mOwnerUniqueID == uniqueID)
        return true;
    return false;
}
