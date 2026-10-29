//cpp
/* dBgCh -- the collision-query root (ROM RTTI _ZTS5dBgCh). Owns the
 * contiguous .text run 0x02035354..0x0203553c, ending where
 * d_bg_ch_actr.cpp begins. The lifecycle group is defined first: D2, D0, D1,
 * then C2 at the tail (C1 has no ROM counterpart and is deadstripped);
 * plain members follow in reverse ROM order, so the earliest (IsOwnedBy)
 * lands last.
 */
// leftovers: the five func_020354xx flag helpers stay extern "C" free
// functions taking the object by pointer (renaming them is a symbols.txt
// change, per include/dBgCh.h); their parameter spellings follow the
// declarations callers already use (include/decl_common.h).

#include "dBgCh.h"
#include "dActor_c.h"

// @symbol _ZN5dBgChC2Ev
dBgCh::dBgCh()
{
    mDetectFlags = 1;
    mOwner = 0;
    mOwnerUniqueID = -1;
}

// @symbol _ZN5dBgChD2Ev
// @symbol _ZN5dBgChD0Ev
// @symbol _ZN5dBgChD1Ev
dBgCh::~dBgCh()
{
}

// @symbol _ZN5dBgCh21StopDetectingOrdinaryEv
void dBgCh::StopDetectingOrdinary()
{
    mDetectFlags &= ~1;
}

// @symbol func_020354b0
extern "C" int func_020354b0(unsigned char *query) {
    return reinterpret_cast<dBgCh *>(query)->mDetectFlags & 1;
}

// @symbol _ZN5dBgCh19StartDetectingWaterEv
void dBgCh::StartDetectingWater()
{
    mDetectFlags |= 2;
}

// @symbol _ZN5dBgCh18StopDetectingWaterEv
void dBgCh::StopDetectingWater()
{
    mDetectFlags &= ~2;
}

// @symbol func_0203547c
extern "C" int func_0203547c(unsigned char *query) {
    return reinterpret_cast<dBgCh *>(query)->mDetectFlags & 2;
}

// @symbol func_02035468
extern "C" void func_02035468(char *query) {
    reinterpret_cast<dBgCh *>(query)->mDetectFlags |= 4;
}

// @symbol func_0203545c
extern "C" int func_0203545c(unsigned char *query) {
    return reinterpret_cast<dBgCh *>(query)->mDetectFlags & 4;
}

// @symbol _ZN5dBgCh19StartDetectingToxicEv
void dBgCh::StartDetectingToxic()
{
    mDetectFlags |= 8;
}

// @symbol func_0203543c
extern "C" int func_0203543c(unsigned char *query) {
    return reinterpret_cast<dBgCh *>(query)->mDetectFlags & 8;
}

// @symbol _ZN5dBgCh27StartIgnoringVanishSurfacesEv
void dBgCh::StartIgnoringVanishSurfaces() {
    mDetectFlags |= 0x20;
}

// @symbol _ZN5dBgCh26StopIgnoringVanishSurfacesEv
void dBgCh::StopIgnoringVanishSurfaces() {
    mDetectFlags &= ~0x20;
}

// @symbol _ZNK5dBgCh24IsIgnoringVanishSurfacesEv
int dBgCh::IsIgnoringVanishSurfaces() const {
    return mDetectFlags & 0x20;
}

// @symbol _ZN5dBgCh24StartIgnoringType14WallsEv
void dBgCh::StartIgnoringType14Walls() {
    mDetectFlags |= 0x40;
}

// @symbol _ZN5dBgCh23StopIgnoringType14WallsEv
void dBgCh::StopIgnoringType14Walls() {
    mDetectFlags &= ~0x40;
}

// @symbol _ZNK5dBgCh21IsIgnoringType14WallsEv
int dBgCh::IsIgnoringType14Walls() const {
    return mDetectFlags & 0x40;
}

// @symbol _ZN5dBgCh8SetActorEP8dActor_c
void dBgCh::SetActor(dActor_c *actor) {
    if (!actor) {
        mOwner = 0;
        mOwnerUniqueID = -1;
    } else {
        mOwner = actor;
        mOwnerUniqueID = actor->uniqueID;
    }
}

// @symbol _ZN5dBgChaSERKS_
dBgCh &dBgCh::operator=(const dBgCh &other) {
    mDetectFlags = other.mDetectFlags;
    mOwnerUniqueID = other.mOwnerUniqueID;
    mOwner = other.mOwner;
    return *this;
}

// @symbol _ZNK5dBgCh9IsOwnedByEP8dActor_c
bool dBgCh::IsOwnedBy(dActor_c *actor) const {
    if (actor == 0 || mOwner == 0)
        return false;
    u32 uniqueID = actor->uniqueID;
    if (mOwner == actor && mOwnerUniqueID == uniqueID)
        return true;
    return false;
}
