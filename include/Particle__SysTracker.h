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
 *         Update runs func_02021bec on the same address.
 *
 * Contents' extent is pinned from both ends: FindData indexes
 * `contents + 0x708 + (uniqueID & 0xf) * 4`, so the bucket array is sixteen
 * words at +0x708, and 0x708 + 16*4 = 0x748 lands exactly on the callback bank
 * that Particle.h's evidence places at object offset 0x750 = 0x8 + 0x748. The
 * bucket entries are chained through a node whose unique ID is at +0x00 and
 * whose next pointer is at +0x18; the node's total size is NOT evidenced by
 * anything reached from here, so no node type is declared and FindData walks
 * the chain by those two proven offsets.
 *
 * Field NAMES cannot change codegen. */
#ifndef PARTICLE__SYSTRACKER_H
#define PARTICLE__SYSTRACKER_H
#include "types.h"

namespace Particle {

struct System;

struct SysTracker {
    /* Particle::SysTracker::Contents -- the live-system registry. Only its
       bucket array is reached from source; everything below it is opaque. */
    struct Contents {
        /* One live-system registry node. The two names are inferred; the
           offsets are fixed by FindData (+0x00/+0x18) and FromUniqueID
           (+0x0c). */
        struct Entry {
            u32 uniqueID;      /* 0x00 */
            u8  pad_004[0x8];
            System *system;    /* 0x0c */
            u8  pad_010[0x8];
            Entry *next;       /* 0x18 */
        };

        u8    pad_000[0x708];
        Entry *mBuckets[16]; /* 0x708 - chain heads, indexed by uniqueID & 0xf */

        Entry *FindData(u32 uniqueID) const;
    };

    void    *mResourceFile;  /* 0x000 */
    void    *mManager;       /* 0x004 */
    Contents mContents;      /* 0x008 */
    /* --- callback bank --- */
    s32 mRunningSlidingDustSystemID;  /* 0x750 */
    u8  mRunningSlidingDustCallback;  /* 0x754 - SimpleCallback; effect 0xda */
    u8  pad_755[0x7];
    s32 mSystemID_75c;      /* 0x75c */
    u8  mCallback_760;      /* 0x760 - SimpleCallback */
    u8  pad_761[0x7];
    s32 mBigSplashSystemID; /* 0x768 */
    u8  mBigSplashCallback; /* 0x76c - vtable data_0208f3e4; effect 0xdd */
    u8  pad_76d[0x7];
    s32 mSystemID_774;      /* 0x774 */
    u8  mCallback_778;      /* 0x778 - vtable data_0208f3e4, same class as 0x76c */
    u8  pad_779[0x7];
    s32 mSystemID_780;      /* 0x780 */
    u8  mCallback_784;      /* 0x784 - vtable data_0208f3a4 */
    u8  pad_785[0x7];
    s32 mRippleSystemID;    /* 0x78c */
    u8  mRippleCallback;    /* 0x790 - vtable data_0208f444; effect 0x109 */
    u8  pad_791[0x7];
    s32 mSystemID_798;      /* 0x798 */
    u8  mCallback_79c;      /* 0x79c - SimpleCallback */
    u8  pad_79d[0x7];
    s32 mSystemID_7a4;      /* 0x7a4 */
    u8  mCallback_7a8;      /* 0x7a8 - SimpleCallback */
    u8  pad_7a9[0x7];
    s32 mSystemID_7b0;      /* 0x7b0 */
    u8  mCallback_7b4;      /* 0x7b4 - scaleCallback_c */
    u8  pad_7b5[0xb];
    s32 mSystemID_7c0;      /* 0x7c0 */
    u8  mCallback_7c4;      /* 0x7c4 - scaleCallback_c */
    u8  pad_7c5[0xf];
    u8  mCallback_7d4;      /* 0x7d4 - scaleCallback_c */
    u8  pad_7d5[0xf];
    u8  mCallback_7e4;      /* 0x7e4 - scaleCallback_c */
    u8  pad_7e5[0xb];
    u8  mCallback_7f0;      /* 0x7f0 - vtable data_0208f3f4 */
    u8  pad_7f1[0x3];
    u8  mCallback_7f4;      /* 0x7f4 - vtable data_0208f424 */
    u8  pad_7f5[0x3];
    u8  mCallback_7f8;      /* 0x7f8 - vtable data_0208f454 */
    u8  pad_7f9[0x3];
    s32 mCallbackParam_7fc; /* 0x7fc - +4 of the 0x7f8 callback; ctor sets 0x3000 */
    u8  mCallback_800;      /* 0x800 - vtable data_0208f454, same class as 0x7f8 */
    u8  pad_801[0x3];
    s32 mCallbackParam_804; /* 0x804 - +4 of the 0x800 callback; ctor ends at 0x4b000 */
    u8  mCallback_808;      /* 0x808 - vtable data_0208f404 */
    u8  pad_809[0x7];
    u8  mWeatherCallback;   /* 0x810 - vtable data_0208f434; System::NewWeather */
    u8  pad_811[0x3];
    /* System::NewWeather writes its numWeatherEffectsNow argument to
       mWeatherCallback + 4, which is this byte; the constructor seeds it 1. */
    u8  mWeatherCallbackCount; /* 0x814 */
    u8  pad_815[0x3];
    u8  mCallback_818;      /* 0x818 - vtable data_0208f464;
                                       System::NewUnkCallback818 passes it */

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
