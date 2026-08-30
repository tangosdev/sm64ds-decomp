/* Particle::SysTracker -- the game's single particle-engine owner, as a real
 * C++ class in its real namespace.
 *
 * This is the C++ spelling of the object that include/Particle.h still models
 * as a flat C `struct Particle` (kept for the thirteen C99 files that reach it
 * through that name and cannot see a namespace at all) and that include/Stage.h
 * carries a fourth-wall copy of, embedded at Stage+0x50. The three declarations
 * agree field for field and name for name; only this one is the class the ROM's
 * mangled names actually describe, so the members whose symbols are
 * _ZN8Particle10SysTracker* are defined against it.
 *
 * Per-field evidence: notes/system-provenance.md and include/Particle.h's own
 * header comment, which carries the full derivation of the callback bank. In
 * brief --
 *   0x000 mResourceFile: Initialise stores either data_02075f14 in place or a
 *         heap buffer it fills with DecompressLZ16, then hands it to the
 *         manager via func_0204a17c; ~SysTracker frees it only in the second
 *         case.
 *   0x004 mManager: the Particle::Manager. Initialise allocates it with
 *         func_0204a4c8 and pokes 0x8000 into its +0x30; Update passes it
 *         straight to func_02049f58.
 *   0x008 mContents: Particle::SysTracker::Contents, a real nested class --
 *         System::FromUniqueID calls
 *         Particle::SysTracker::Contents::FindData(this + 8, uniqueID), and
 *         Update calls Contents::Update on the same address.
 *
 * Contents' extent is pinned from both ends: FindData indexes
 * `contents + 0x708 + (uniqueID & 0xf) * 4`, so the bucket array is sixteen
 * words at +0x708, and 0x708 + 16*4 = 0x748 lands exactly on the callback bank
 * that Particle.h's evidence places at object offset 0x750 = 0x8 + 0x748. The
 * bucket entries are chained through a node whose unique ID is at +0x00 and
 * whose next pointer is at +0x18. The constructor's 0x40-entry walk from +0x8
 * to the bucket array at +0x708 proves the node's 0x1c stride.
 *
 * Field NAMES cannot change codegen. */
#ifndef PARTICLE__SYSTRACKER_H
#define PARTICLE__SYSTRACKER_H
#include "dPa_c.h"

struct Vector3_16f;

namespace Particle {

struct System;
struct Manager;

struct SysTracker {
    /* Particle::SysTracker::Contents -- the live-system registry. Its pool
       extent and stride are fixed by the exact constructor; FindData fixes the
       bucket heads and chain fields. */
    struct Contents {
        /* One live-system registry node. Field and method names are readable
           inferences, not ROM-authenticated original spellings. The offsets
           and relationships are fixed by the contiguous registry routines. */
        struct Entry {
            u32 uniqueID;      /* 0x00 */
            u32 definitionID;  /* 0x04 - index into Manager::mDefinitions */
            u16 savedValue;    /* 0x08 - restored when the System is released */
            u8  active;        /* 0x0a; cleared before an entry is removed */
            u8  pad_00b;
            System *system;    /* 0x0c */
            dPa_c::level_c::callback_c *callback; /* 0x10 */
            Entry *prev;       /* 0x14 */
            Entry *next;       /* 0x18 */

            void Reset();
            bool Initialise(u32 newUniqueID, u32 newDefinitionID,
                            Vector3& position, const Vector3_16f *direction,
                            dPa_c::level_c::callback_c *newCallback);
        };

        u32   unk_000;
        u8    mCurrentIndex;
        u8    pad_005[0x3];
        Entry mEntries[0x40];
        Entry *mBuckets[16]; /* 0x708 - chain heads, indexed by uniqueID & 0xf */

        Contents();
        Entry *FindData(u32 uniqueID) const;
        void Unlink(Entry& entry);
        void Link(Entry& entry);
        u32 Create(u32 definitionID, Vector3& position,
                   const Vector3_16f *direction,
                   dPa_c::level_c::callback_c *callback);
        void Clear();
        void Update();
    };

    void    *mResourceFile;  /* 0x000 */
    Manager *mManager;       /* 0x004 */
    Contents mContents;      /* 0x008 */
    /* --- callback bank --- */
    s32 mRunningSlidingDustSystemID;  /* 0x750 */
    dPa_c::level_c::simpleCallback_c mRunningSlidingDustCallback; /* 0x754 */
    s32 mSystemID_75c;      /* 0x75c */
    dPa_c::level_c::simpleCallback_c mCallback_760; /* 0x760 */
    s32 mBigSplashSystemID; /* 0x768 */
    dPa_c::level_c::splashCallback_c mBigSplashCallback; /* 0x76c */
    s32 mSystemID_774;      /* 0x774 */
    dPa_c::level_c::splashCallback_c mCallback_778; /* 0x778 */
    s32 mSystemID_780;      /* 0x780 */
    dPa_c::level_c::bubbleCallback_c mCallback_784; /* 0x784 */
    s32 mRippleSystemID;    /* 0x78c */
    dPa_c::level_c::fitWaterSimpleCallback_c mRippleCallback; /* 0x790 */
    s32 mSystemID_798;      /* 0x798 */
    dPa_c::level_c::simpleCallback_c mCallback_79c; /* 0x79c */
    s32 mSystemID_7a4;      /* 0x7a4 */
    dPa_c::level_c::simpleCallback_c mCallback_7a8; /* 0x7a8 */
    s32 mSystemID_7b0;      /* 0x7b0 */
    dPa_c::level_c::scaleCallback_c mCallback_7b4; /* 0x7b4 */
    s32 mSystemID_7c0;      /* 0x7c0 */
    dPa_c::level_c::scaleCallback_c mCallback_7c4; /* 0x7c4 */
    u8 pad_7d0[0x4];
    dPa_c::level_c::scaleCallback_c mCallback_7d4; /* 0x7d4 */
    u8 pad_7e0[0x4];
    dPa_c::level_c::scaleCallback_c mCallback_7e4; /* 0x7e4 */
    dPa_c::level_c::checkWaterCallback_c mCallback_7f0; /* 0x7f0 */
    dPa_c::level_c::checkWaterRippleCallback_c mCallback_7f4; /* 0x7f4 */
    dPa_c::level_c::fitWaterCallback_c mCallback_7f8; /* 0x7f8 */
    dPa_c::level_c::fitWaterCallback_c mCallback_800; /* 0x800 */
    dPa_c::level_c::checkYoganCallback_c mCallback_808; /* 0x808 */
    dPa_c::level_c::clipCallback_c mWeatherCallback; /* 0x810 */
    dPa_c::level_c::cleanParticleCallback_c mCallback_818; /* 0x818 */

    SysTracker();
    ~SysTracker();
    void Initialise();
    void Update();
};

/* tools/check_header_offsets.py does not parse a struct inside a namespace, so
   these stand in for it: 0x81c is the whole object include/Particle.h models and
   0x748 is Contents' span from FindData's +0x708 bucket array to the callback
   bank at object offset 0x750. */
typedef char Particle_SysTracker_size_must_be_0x81c[
    sizeof(SysTracker) == 0x81c ? 1 : -1];
typedef char Particle_SysTracker_Contents_size_must_be_0x748[
    sizeof(SysTracker::Contents) == 0x748 ? 1 : -1];

}

/* The flat spelling this header used to declare, kept so the file that met the
   class under that name keeps compiling unchanged. */
typedef Particle::SysTracker Particle__SysTracker;

#endif
