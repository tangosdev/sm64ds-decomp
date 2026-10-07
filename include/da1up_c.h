#ifndef DA1UP_C_H
#define DA1UP_C_H

/* THE CLASS NAME IS THE CARTRIDGE'S. ov002 holds the length-prefixed Itanium
 * type-name string `7da1up_c\0` at 0x02108370, and _ZTI7da1up_c at 0x0210837c
 * is the matching __si_class_type_info whose +8 word reaches _ZTI12dEnemyBase_c
 * at 0x021081c0. The project used to call this class OneUpMushroom; that word
 * is absent from every executable image in every encoding tested, so it was a
 * coined name and is gone.
 *
 * RECONSTRUCTED IDENTIFIERS include the member names and signatures, plus
 * the two factories da1up_c_classInit_ONEUPKINOKO / _SCALEUP_KINOKO and the two
 * registry descriptors g_profile_ONEUPKINOKO / _SCALEUP_KINOKO. ONEUPKINOKO and
 * SCALEUP_KINOKO themselves are NOT coined -- they are ROM-attested ASCII in
 * arm9's actor debug-name table at 0x020901e8 and 0x02090658, profile ids 276
 * and 277 -- but the `classInit` and `g_profile_` affixes are later EAD lineage,
 * not SM64DS symbols. Every FIELD NAME below is a project invention too.
 * Constructor/destructor calls support the owned-subobject types and offsets;
 * the tail fields and exact base extent remain inferences as the fact file
 * records. The RTTI class string does not recover method or parameter names.
 */

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout: the
 * class's own destructor `_ZN7da1up_cD1Ev` destroys each member, and
 * `da1up_c_classInit_ONEUPKINOKO` constructs the same types at the same offsets before
 * storing `_ZTV7da1up_c`. Everything this header used to restate below 0x110
 * belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr               0x1bc   -> 0x300
 *     0x300 Model                      0x50    -> 0x350
 *     0x350 dExtShadowModel_c                0x28    -> 0x378
 *
 * SIZE IS THE ROM'S OWN: `da1up_c_classInit_ONEUPKINOKO` calls `fBase_c::operator new(920)`
 * -- 0x398 -- and stores this class's vtable, so that literal IS this
 * class's sizeof.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dExtShadowModel_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* mMushroomType, intended range 0..13 (Behavior indexes the array without a
   bounds check), is picked from the low nibble of param1 in
   InitResources and indexes the 14-element dispatch array Behavior calls. The
   enumerators name what each dispatched handler DOES, not what the original
   source called it -- no original name survives in the image. Each is spelled
   out at its handler in src/actors/da1up_c.cpp.
   Groups the code makes visible:
     5 / 7      stay hidden until mUnlockCount reaches 0, then pop out;
     6 / 8      invisible and stationary (mShown stays 0); touching them
                decrements a waiting 5 / 7
                (func_ov002_020af684);
     11 / 12    spinning variants of 6 / 8 that also give a coin and a heal;
     9          spawns one type 5 and two type 11 around its spawn point. */
enum da1up_MushroomType {
    MUSHROOM_POP_OUT_DRIFT        = 0,  /* arc out, drift at 2 units a frame, blink, vanish */
    MUSHROOM_POP_OUT_FLEE         = 1,  /* arc out, then walk away from the player */
    MUSHROOM_WAIT_THEN_ACCELERATE = 2,  /* wait for the player, then speed up along mPrevAngleY */
    MUSHROOM_STATIONARY           = 3,  /* no movement; just waits to be touched */
    MUSHROOM_WAIT_THEN_FACE_AWAY  = 4,  /* wait for the player, hop, then face away from him; no horizontal speed is set here */
    MUSHROOM_HIDDEN_FLEE          = 5,  /* hidden, then arc out and walk away */
    MUSHROOM_TRIGGER_FOR_5        = 6,  /* invisible and stationary; touching it unlocks a type 5 */
    MUSHROOM_HIDDEN_CHASE         = 7,  /* hidden, then arc out and steer toward the player */
    MUSHROOM_TRIGGER_FOR_7        = 8,  /* invisible and stationary; touching it unlocks a type 7 */
    MUSHROOM_SPAWNER              = 9,  /* spawns three mushrooms, then removes itself */
    MUSHROOM_RISE_THEN_POP_OUT    = 10, /* rises 100 units, then continues as type 0 */
    MUSHROOM_SPIN_TRIGGER_FOR_5   = 11, /* spinning 6, plus a coin and a heal */
    MUSHROOM_SPIN_TRIGGER_FOR_7   = 12, /* spinning 8, plus a coin and a heal */
    MUSHROOM_FALL_THEN_WAIT       = 13  /* falls to the ground, then waits to be touched */
};

struct da1up_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    dExtShadowModel_c                  mShadowModel;          /* 0x350 */
    /* mPos as InitResources found it (the three words are copied from
       mPosX/Y/Z). Type 10 rises to 100 units above mSpawnPosY; type 9 spawns
       its three mushrooms relative to it. */
    s32                          mSpawnPosX;            /* 0x378 */
    s32                          mSpawnPosY;            /* 0x37c */
    s32                          mSpawnPosZ;            /* 0x380 */
    s32                          mMushroomType;         /* 0x384 -- param1 & 0xf, see da1up_MushroomType */
    /* Step within the current behaviour, 0..3. Handlers 0, 1, 2, 4, 5, 7 and 13
       switch on it; the others never read it.
       Behavior zeroes mStateTimer and mStateFrames whenever a handler changes it. */
    s32                          mState;                /* 0x388 */
    /* 0x38c is live and distinct from dEnemyBase_c::mStateTimer at 0x100.
       Naming it mStateTimer shadowed the base field. Behavior advances it and
       clears it exactly as it does mStateTimer, and func_ov002_020af7cc sets
       both to 0xffff; it is the counter func_ov002_020af248 reads for the
       blink-then-vanish countdown. */
    u16                          mStateFrames;          /* 0x38c */
    /* 0x38e: nonzero lets Render and the drop shadow run. func_ov002_020af218
       stores IsPlayerInRange there each frame; types 5 and 7 hold it at 0 while
       hidden; type 10 forces it to 1. 0x38f: the blink phase, written by
       func_ov002_020af248 during the final 40 frames and otherwise left at 1
       (InitResources sets it). Render needs both. */
    u8                           mShown;                /* 0x38e */
    u8                           mBlinkOn;              /* 0x38f */
    /* param1 >> 4 & 0xf. Types 5 and 7 stay hidden in state 0 while it is
       nonzero; func_ov002_020af684 decrements it. Type 9 spawns a type 5 with
       it set to 2 (spawn param 0x25) next to two type 11. */
    s32                          mUnlockCount;          /* 0x390 */
    /* The handle func_ov002_020aeee4 hands Particle::System::New and stores
       its result back into, so one effect is carried from frame to frame. */
    u32                          mParticleID;           /* 0x394 */

    /* --- vtable ---
       Nine own overrides, and _ZTV7da1up_c at 0x021083c8 is what says which:
       its 31 slots relocate to this run at slot 0 (0x020b01c0), 3 (0x020affe8),
       6 (0x020b00e8), 9 (0x020b0070), 12 (0x020b006c), 16/17 (the destructor
       pair) and 18/19. Behavior, CleanupResources, InitResources,
       OnPendingDestroy and Render already overrode inherited virtuals in the
       previous header, despite its misleading non-virtual comment. Repeating
       the virtual keyword makes their role explicit; it does not turn a
       non-virtual function into an override or repair five vtable slots.

       The destructor stays OUT OF LINE and declared FIRST, so it is this
       class's key function: the ROM puts D1 at 0x020aee40 below D0 at
       0x020aee88 with no D2, and out-of-line plus `#pragma defer_codegen off`
       is the form that reproduces that order. */
    virtual ~da1up_c();

    virtual s32   InitResources();               /* slot  0 */
    virtual s32   CleanupResources();            /* slot  3 */
    virtual s32   Behavior();                    /* slot  6 */
    virtual s32   Render();                      /* slot  9 */
    virtual void  OnPendingDestroy();            /* slot 12 */
    virtual s32   OnYoshiTryEat();               /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */

    /* The fourteen mMushroomType handlers Behavior dispatches through the
       pointer-to-member table data_ov002_0210dc00 (indices in comments) plus
       the shared helpers they call. func_ov002_020aefa4 stays a free function:
       its void return is load-bearing (see the file's header comment). */
    void func_ov002_020aeee4();
    void func_ov002_020aefb8();
    void func_ov002_020af0c0();
    int  func_ov002_020af1dc();
    int  func_ov002_020af218(int range);
    int  func_ov002_020af248(int n);
    void func_ov002_020af3a8();
    void func_ov002_020af474();
    void func_ov002_020af4ec();
    void func_ov002_020af684(int target, Player *player);
    void func_ov002_020af724();   /* dispatch 13 */
    void func_ov002_020af7cc();   /* dispatch 10 */
    void func_ov002_020af838();   /* dispatch  9 */
    void func_ov002_020af908();   /* dispatch 12 */
    void func_ov002_020af924();   /* dispatch  8 */
    void func_ov002_020af950();   /* dispatch  7 */
    void func_ov002_020afa50();   /* dispatch 11 */
    void func_ov002_020afa6c();   /* dispatch  6 */
    void func_ov002_020afa98();   /* dispatch  5 */
    void func_ov002_020afbb4();   /* dispatch  4 */
    int  func_ov002_020afc44();   /* dispatch  3 */
    void func_ov002_020afc68();
    void func_ov002_020afd10();   /* dispatch  2 */
    void func_ov002_020afde4();
    void func_ov002_020afe4c();   /* dispatch  1 */
    void func_ov002_020aff10();   /* dispatch  0 */

    /* Leaf allocator until fBase_c::operator new is a real method (#2570).
       unsigned long, not unsigned int: that is the C++ new signature mwccarm
       2004/b56 emits for `new da1up_c()`. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char da1up_c_size_must_be_0x398[sizeof(da1up_c) == 0x398 ? 1 : -1];
#endif

#endif /* DA1UP_C_H */
