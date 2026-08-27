/* Particle::SysTracker -- the game's single particle-engine owner.
 *
 * The struct is still spelled `Particle` here for historical reasons; the
 * ROM's own mangled names call it Particle::SysTracker (see
 * _ZN8Particle10SysTrackerC1Ev / D1Ev / 10InitialiseEv). One instance exists
 * at a time and its address lives in the global data_0209ee74, which the
 * constructor writes and the destructor clears; every free function in the
 * Particle namespace reaches its members through that global.
 *
 * Offsets and widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are inferred from behaviour and cannot change codegen.
 * Per-field evidence: notes/system-provenance.md.
 *
 * The tail of the object (0x750 onwards) is a bank of Particle::Callback
 * subobjects, several of them preceded by a u32 "last system unique ID" slot.
 * The pairing is proven three ways:
 *   * Particle::System::New(uniqueID, effectID, x, y, z, dir, callback) is
 *     called as New(*(u32*)(base+N), effect, ..., base+N+4) and its result is
 *     stored back into *(u32*)(base+N) -- so slot N is the ID and N+4 is the
 *     Callback. RunningSlidingDustAt uses 0x750/0x754, System::NewBigSplash
 *     uses 0x768/0x76c, System::NewRipple uses 0x78c/0x790.
 *   * The constructor zeroes exactly ten s32s in the tail -- 0x750, 0x75c,
 *     0x768, 0x774, 0x780, 0x78c, 0x798, 0x7a4, 0x7b0, 0x7c0 -- and no
 *     others, which is the same ten that sit four bytes below a Callback.
 *   * Every Callback subobject is constructed by storing the base
 *     Particle::Callback vtable data_0208f3b4 and then overwriting it with a
 *     derived one, which is exactly what Particle::SimpleCallback's own
 *     constructor does.
 * Where no New* helper survives to name the effect, the field keeps its
 * offset in the name: the claim is only "this is a Callback" / "this is a
 * system-ID slot", which the evidence above does support.
 */
#ifndef PARTICLE_H
#define PARTICLE_H
#include "types.h"

struct Particle {
    /* The particle resource file, held in place when it is already
       uncompressed and otherwise decompressed onto the heap; SysTracker::
       Initialise picks between the two and ~SysTracker frees it only in the
       second case. Handed to the manager by func_0204a17c. */
    void *mResourceFile;    /* 0x000 */
    /* Particle::Manager. Initialise allocates it (func_0204a4c8) and pokes
       0x8000 into its +0x30; Particle::RenderAll and SysTracker::Update pass
       it straight on, and System::NewSimple / func_02021d1c call
       Particle::Manager::AddSystem on it. */
    void *mManager;         /* 0x004 */
    /* Particle::SysTracker::Contents, a real nested class: the constructor and
       destructor run func_02021c90 / func_02021b98 on it, and
       System::FromUniqueID calls
       Particle::SysTracker::Contents::FindData(this + 8, uniqueID). */
    u8  mContents;          /* 0x008 */
    u8  pad_009[0x747];
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
    u8  mCallback_7b4;      /* 0x7b4 - built by func_020225fc */
    u8  pad_7b5[0xb];
    s32 mSystemID_7c0;      /* 0x7c0 */
    u8  mCallback_7c4;      /* 0x7c4 - built by func_020225fc */
    u8  pad_7c5[0xf];
    u8  mCallback_7d4;      /* 0x7d4 - built by func_020225fc */
    u8  pad_7d5[0xf];
    u8  mCallback_7e4;      /* 0x7e4 - built by func_020225fc */
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

    /* Particle::SimpleCallback -- one of the 8-byte {vtable; s16} objects the
       mCallback_* bytes above mark the offset of. No base is modeled, so its
       constructor stores both the base Particle::Callback vtable and its own
       explicitly, matching what the ROM's constructor does in one flat body. */
    struct SimpleCallback {
        void *vtable;   /* 0x00 */
        s16 unk_004;    /* 0x04 */

        SimpleCallback();
    };
};

#endif
