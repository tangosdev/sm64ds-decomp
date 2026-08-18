//cpp
// @symbol _ZN5Actor9TrackStarEjj
/* Actor::TrackStar(u32 starIdx, u32 markerType) at 0x0200ff94.
 *
 * Registers this actor in STAR_MARKERS, the twelve-slot table of actors
 * currently showing an uncollected-star glint, and returns the slot index it
 * claimed -- or -1 if it claimed none.
 *
 * `this` is not incidental: r0 is stashed in r5 at entry and reappears as
 * SetStarMarker's second argument (`mov r1, r5`), so the actor being
 * registered is the receiver.
 *
 * markerType == 2 selects the "only the star you are playing for" rule. When
 * the requested star is not the selected one (0x0209f220, read `ldrb`) and the
 * current sublevel (0x0209f2f8, read `ldrsb` -- signed) maps to level 0xe or
 * below, nothing is marked at all. Above 0xe every star gets a marker. An
 * already-collected star is bumped from type 2 to 3.
 *
 * The slot counter is s8: its increment is `add`/`lsl #0x18`/`asr #0x18`, a
 * sign-extending byte round-trip. Its bound of 0xc is what fixes the table at
 * twelve entries.
 */
#include "Actor.h"
#include "decl_common.h" /* SublevelToLevel */

extern "C" {
extern u8     data_0209f220[]; /* the star index this mission was entered for */
extern s8     data_0209f2f8[]; /* the current sublevel id */
extern Actor *STAR_MARKERS[12]; /* 0x0209f40c */

void SetStarMarker(s32 slot, Actor *actor, s32 markerType);
int  IsStarCollectedInCurLevel(u32 starIdx);
}

s32 Actor::TrackStar(u32 starIdx, u32 markerType)
{
    s32 type = markerType;

    if (markerType == 2) {
        if (starIdx != data_0209f220[0]) {
            if (SublevelToLevel(data_0209f2f8[0]) <= 0xe)
                return -1;
        }
        if (IsStarCollectedInCurLevel(starIdx))
            type = 3;
    }

    s8 slot;
    for (slot = 0; slot < 0xc; slot++) {
        if (STAR_MARKERS[slot] == 0) {
            SetStarMarker(slot, this, type);
            return slot;
        }
    }
    return -1;
}
