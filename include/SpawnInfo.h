#ifndef SPAWNINFO_H
#define SPAWNINFO_H

#include "types.h"

/* SpawnInfo, the 0x1c actor profile: the registry descriptor an actor's
 * g_profile_<ID> global holds. Every field name follows the code that reads it.
 *
 *   +0x00 classInit   func_02043098 (the framework create entry) calls
 *                     (*data_020a4bb8[idx])(), the first word of the
 *                     registry entry: the allocating factory.
 *   +0x04/+0x06       fBase_c::fBase_c reads data_020a4bb8[actorID] and
 *                     hands these two halfwords to SetBehaviorPriority and
 *                     SetRenderPriority (both take u16). No consumer reads
 *                     +0x04 as an id, but in every registry entry its value
 *                     equals that entry's registry index, so a profile's
 *                     behavior priority is its registry id.
 *   +0x08 actorFlags  dActor_c::dActor_c copies it into mFlags.
 *   +0x0c..+0x18      dActor_c::dActor_c passes the four words to
 *                     dActor_c::SetRanges as clip offset Y, clip radius,
 *                     clip distance (plus 0x7d0000 in game mode 2) and far
 *                     distance, stored in mClipOffsetY, mClipRadius,
 *                     mClipDistance and mFarDistance.
 *
 * See notes/profile-lifecycle-crosswalk.md. SpawnInfo is the project's name for
 * this type (notes/actor-profile-pilot.md, question 4); it and the field names
 * are decomp names, and SM64DS keeps neither. T is the factory's class, so
 * `daFoo_c_classInit` initializes classInit without a cast. Data members only:
 * an inline body here would renumber @NNN temporaries in every consumer. */
template <class T> struct SpawnInfo {
    T *(*classInit)();
    u16 behaviorPriority;
    u16 renderPriority;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

/* 0x1c with 32-bit pointers; the PC port's 64-bit pointers widen classInit. */
#ifndef SM64DS_PLATFORM_PC
typedef char SpawnInfo_size_must_be_0x1c[sizeof(SpawnInfo<void>) == 0x1c ? 1 : -1];
#endif

/* BaseSpawnInfo, the 0x08 base profile: the descriptor a scene or other
 * non-actor process registers. It is SpawnInfo's first three fields, the ones
 * func_02043098 and fBase_c::fBase_c read; the actor tail (flags and the four
 * clip ranges) is read by dActor_c::dActor_c alone. The pilot's six
 * 0x08 rows are this form (notes/actor-profile-pilot.md, answer 1).
 * BaseSpawnInfo is a decomp name like SpawnInfo, chosen to read as its base
 * form. It is a separate aggregate rather than SpawnInfo's base class, so
 * both keep brace initialization. */
template <class T> struct BaseSpawnInfo {
    T *(*classInit)();
    u16 behaviorPriority;
    u16 renderPriority;
};

#ifndef SM64DS_PLATFORM_PC
typedef char BaseSpawnInfo_size_must_be_0x08[sizeof(BaseSpawnInfo<void>) == 0x08 ? 1 : -1];
#endif

#endif /* SPAWNINFO_H */
