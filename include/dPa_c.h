#ifndef DPA_C_H
#define DPA_C_H

#include "types.h"

/* The ROM's RTTI records at 0x0208f304 and 0x0208f360 prove both nested
 * class names and the single-inheritance edge below. Their address-point
 * vtables at 0x0208f3b4 and 0x0208f3c4 each contain two virtual slots.
 *
 * The callback bodies and dispatch ABI prove that their first explicit
 * parameter is a reference-sized Particle system object. `Particle::System`
 * is the established imported spelling, but unlike the callback class names
 * it is not RTTI-backed and remains an inferred exact-source spelling. */
namespace Particle { struct System; struct Manager; }
struct Vector3_16f;

struct dPa_c {
    struct level_c {
        class callback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class simpleCallback_c : public callback_c {
        public:
            s16 value;              /* 0x004 */
            simpleCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class scaleCallback_c : public simpleCallback_c {
        public:
            /* Descriptive names; the offsets and uses are body-proven. */
            s16 scale;              /* 0x006 - reuses base tail padding */
            s16 velocity;           /* 0x008 */
            u8 timer;               /* 0x00a */
            scaleCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class edStarKiraCallback_c : public simpleCallback_c {
        public:
            struct trackingRecordStorage_c {
                u32 words[3];
            };

            /* 64 independently initialized 0xc-byte tracking records. Their
             * historical element type name is not present in ROM RTTI. */
            trackingRecordStorage_c trackingRecords[0x40]; /* 0x008 */
            s32 unk308;                                   /* 0x308 */
            s16 unk30c;                                   /* 0x30c */

            edStarKiraCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class checkYoganCallback_c : public simpleCallback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class fitWaterSimpleCallback_c : public simpleCallback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class fitWaterCallback_c : public callback_c {
        public:
            /* Descriptive name; the offset and water-height use are body-proven. */
            s32 waterOffset;         /* 0x004 */

            fitWaterCallback_c() : waterOffset(0x3000) {}

            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class checkWaterCallback_c : public callback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class bubbleCallback_c : public simpleCallback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class splashCallback_c : public simpleCallback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class clipCallback_c : public callback_c {
        public:
            /* Zero expires every active particle; nonzero clips them. */
            u8 enabled;             /* 0x004 */

            clipCallback_c() : enabled(1) {}

            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class cleanParticleCallback_c : public callback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool done);
        };

        class checkWaterRippleCallback_c : public callback_c {
        public:
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        /* The "level" emitters: systems a caller keeps alive by asking for
         * them again every frame under the id entry() handed back.
         * execute() releases any emitter nobody asked for since the last
         * frame. 64 slots, chained in 16 hash buckets by id & 0xf.
         *
         * Only `dPa_c::level_c` is ROM-proven (the callback RTTI above is
         * nested in it, and every slot stores a level_c::callback_c). The
         * member names come from Zelda: Twilight Princess's
         * dPa_control_c::level_c (EAD, 2006; names from Nintendo's symbol
         * maps), whose routines are this code's algorithm almost line for
         * line: the pre-incremented id that skips 0, mFreeNo stepping
         * (n + 1) % max, addTable/cutTable on a doubly linked bucket chain,
         * execute() clearing or cutting, cleanup() cutting every live slot. */
        struct emitter_c {
            u32 mId;              /* 0x00 - 0 marks a free slot */
            u32 mNameId;          /* 0x04 - the effect: index into Manager::mDefinitions */
            u16 mSavedCallbackValue; /* 0x08 - the definition's callbackValue, restored by cleanup() */
            u8  mStatus;          /* 0x0a - 1 while requested this frame; execute() clears it */
            u8  pad_00b;
            Particle::System *mEmitter; /* 0x0c */
            callback_c *mCallback;      /* 0x10 */
            emitter_c *mPre;            /* 0x14 */
            emitter_c *mNext;           /* 0x18 */

            void cleanup();
            bool entry(u32 id, u32 nameId, Vector3& position,
                       const Vector3_16f *direction, callback_c *callback);
        };

        /* entry() pre-increments this for each new emitter and skips 0,
           which get() treats as "no emitter". */
        u32 mLastId;
        u8  mFreeNo;              /* 0x004 - where entry() looks for a free slot first */
        u8  pad_005[0x3];
        emitter_c mEmitter[0x40]; /* 0x008 */
        emitter_c *mTable[16];    /* 0x708 - bucket heads, indexed by id & 0xf */

        level_c();
        emitter_c *get(u32 id);
        void cutTable(emitter_c *emitter);
        void addTable(emitter_c *emitter);
        u32 entry(u32 nameId, Vector3& position,
                  const Vector3_16f *direction, callback_c *callback);
        void cleanup();
        void execute();
    };

    /* dPa_c itself: the scene's particle owner. Every scene that shows
     * particles embeds one (dScStage_c +0x50, dEntObj_c +0x50,
     * dScMgSingle3DBase_c +0x471c, dScMgD3DBase_c +0x47e4); the constructor
     * makes it the current one (gParticleControl) and the destructor clears it.
     *
     * The name is the ROM's scope, not a guess: this object holds the
     * level_c at +0x8 and, from +0x750, the bank of level_c callbacks the
     * fixed-effect helpers pass to the level emitters, and its constructor,
     * destructor, Initialise and Update are in the same translation unit as
     * level_c and the callbacks (arm9 0x02021a04..0x02023204). The tree
     * called it Particle::SysTracker until notes/particle-dpa-naming.md.
     *
     *   0x000 mResourceFile: Initialise stores either data_02075f14 in place
     *         or a heap buffer it fills with DecompressLZ16, then hands it to
     *         the manager via func_0204a17c; ~dPa_c frees it only in the
     *         second case.
     *   0x004 mManager: the Particle::Manager. Initialise allocates it with
     *         func_0204a4c8 and pokes 0x8000 into its +0x30; Update passes
     *         it straight to func_02049f58.
     *   0x008 mLevel: get() indexes `level + 0x708 + (id & 0xf) * 4`, so the
     *         bucket array is sixteen words at +0x708, and 0x708 + 16*4 =
     *         0x748 lands exactly on the callback bank at 0x750 = 0x8 + 0x748.
     *
     * The callback bank: System::New(id, effect, x, y, z, dir, callback) is
     * called as New(*(u32*)(this+N), effect, ..., this+N+4) and its result is
     * stored back into *(u32*)(this+N), so slot N is the level id and N+4 is
     * its callback (RunningSlidingDustAt 0x750/0x754, NewBigSplash
     * 0x768/0x76c, NewRipple 0x78c/0x790). The constructor zeroes exactly
     * the ten ids 0x750..0x7c0; NewFlamethrowerBlueA and NewFlamethrowerBlueB
     * use 0x7d0 and 0x7e0 the same way without the constructor clearing
     * them. Each callback is constructed by storing callback_c's vtable
     * (0x0208f3b4) and then the derived one. Field names cannot change
     * codegen. */
    void *mResourceFile;      /* 0x000 */
    Particle::Manager *mManager; /* 0x004 */
    level_c mLevel;           /* 0x008 */
    /* The callback bank. Each level emitter the New* wrappers keep alive
     * has an id slot and the callback it was started with; the members
     * are named for that effect. The callbacks from 0x7f0 on have
     * no id slot (their callers keep the id); they take their type's name,
     * except the clipCallback_c NewWeather starts.
     * mFitWaterRaisedCallback is the fitWaterCallback_c whose waterOffset
     * the constructor raises from 3.0 to 75.0. */
    s32 mRunningSlidingDustSystemID;                               /* 0x750 */
    level_c::simpleCallback_c mRunningSlidingDustCallback;         /* 0x754 */
    s32 mMegaRunningSlidingDustSystemID;                                  /* 0x75c */
    level_c::simpleCallback_c mMegaRunningSlidingDustCallback;            /* 0x760 */
    s32 mBigSplashSystemID;                                        /* 0x768 */
    level_c::splashCallback_c mBigSplashCallback;                  /* 0x76c */
    s32 mSplashSystemID;                                           /* 0x774 */
    /* Constructed but never passed: NewSplash hands New &mBigSplashCallback. */
    level_c::splashCallback_c mSplashCallback;                     /* 0x778 */
    s32 mBubbleSystemID;                                           /* 0x780 */
    level_c::bubbleCallback_c mBubbleCallback;                     /* 0x784 */
    s32 mRippleSystemID;                                           /* 0x78c */
    level_c::fitWaterSimpleCallback_c mRippleCallback;             /* 0x790 */
    s32 mRedFlameSystemID;                                          /* 0x798 */
    level_c::simpleCallback_c mRedFlameCallback;                    /* 0x79c */
    s32 mBlueFlameSystemID;                                         /* 0x7a4 */
    level_c::simpleCallback_c mBlueFlameCallback;                   /* 0x7a8 */
    s32 mFlamethrowerRedASystemID;                                 /* 0x7b0 */
    level_c::scaleCallback_c mFlamethrowerRedACallback;            /* 0x7b4 */
    s32 mFlamethrowerRedBSystemID;                                 /* 0x7c0 */
    level_c::scaleCallback_c mFlamethrowerRedBCallback;            /* 0x7c4 */
    s32 mFlamethrowerBlueASystemID;                                /* 0x7d0 */
    level_c::scaleCallback_c mFlamethrowerBlueACallback;           /* 0x7d4 */
    s32 mFlamethrowerBlueBSystemID;                                /* 0x7e0 */
    level_c::scaleCallback_c mFlamethrowerBlueBCallback;           /* 0x7e4 */
    level_c::checkWaterCallback_c mCheckWaterCallback;             /* 0x7f0 */
    level_c::checkWaterRippleCallback_c mCheckWaterRippleCallback; /* 0x7f4 */
    level_c::fitWaterCallback_c mFitWaterCallback;                 /* 0x7f8 */
    level_c::fitWaterCallback_c mFitWaterRaisedCallback;           /* 0x800 */
    level_c::checkYoganCallback_c mCheckYoganCallback;             /* 0x808 */
    level_c::clipCallback_c mWeatherCallback;                      /* 0x810 */
    level_c::cleanParticleCallback_c mCleanParticleCallback;       /* 0x818 */

    dPa_c();
    ~dPa_c();
    void Initialise();
    void Update();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dPa_callback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::callback_c) == 0x4 ? 1 : -1];
typedef char dPa_simpleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::simpleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_scaleCallback_c_size_must_be_0xc[
    sizeof(dPa_c::level_c::scaleCallback_c) == 0xc ? 1 : -1];
typedef char dPa_edStarKiraCallback_c_size_must_be_0x310[
    sizeof(dPa_c::level_c::edStarKiraCallback_c) == 0x310 ? 1 : -1];
typedef char dPa_checkYoganCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::checkYoganCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_fitWaterSimpleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::fitWaterSimpleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_fitWaterCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::fitWaterCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_checkWaterCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::checkWaterCallback_c) == 0x4 ? 1 : -1];
typedef char dPa_bubbleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::bubbleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_splashCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::splashCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_clipCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::clipCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_level_c_size_must_be_0x748[
    sizeof(dPa_c::level_c) == 0x748 ? 1 : -1];
typedef char dPa_level_c_emitter_c_size_must_be_0x1c[
    sizeof(dPa_c::level_c::emitter_c) == 0x1c ? 1 : -1];
typedef char dPa_cleanParticleCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::cleanParticleCallback_c) == 0x4 ? 1 : -1];
typedef char dPa_checkWaterRippleCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::checkWaterRippleCallback_c) == 0x4 ? 1 : -1];
typedef char dPa_c_size_must_be_0x81c[sizeof(dPa_c) == 0x81c ? 1 : -1];
#endif

#endif
