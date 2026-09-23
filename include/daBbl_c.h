#ifndef DABBL_C_H
#define DABBL_C_H

#include "types.h"

/* The Lava Bubble (Podoboo) of the lava levels, registry profile BUBBLE.
 *
 * WHAT THE CARTRIDGE PROVES:
 *   _ZTS  ov064 0x0211beb0  the literal "7daBbl_c", nine bytes padded to
 *                           twelve -- exactly the length the mangled spelling
 *                           of this very name takes, so the RTTI name is the
 *                           ROM's own and nothing here is coined
 *   _ZTI  ov064 0x0211bebc  __si_class_type_info: one direct base,
 *                           _ZTI12dEnemyBase_c (ov002 0x021081c0), at offset
 *                           zero
 *   _ZTV  ov064 0x0211beec  the address point (storage 0x0211bee4, typeinfo
 *                           word at 0x0211bee8); 31 slots, the same extent as
 *                           dActor_c, with 0, 3, 6, 9, 12, 16, 17 and 18 the
 *                           only overrides
 *   profile ov064 0x0211bec8  g_profile_BUBBLE, whose factory word is
 *                           daBbl_c_classInit at 0x02118b10
 *   size  0x31c             the literal daBbl_c_classInit hands operator new
 *
 * TWO INDEPENDENT WITNESSES agree on the layout: the class's own destructor
 * `_ZN7daBbl_cD1Ev` destroys each member, and `daBbl_c_classInit` constructs
 * the same types at the same offsets before storing `_ZTV7daBbl_c`. Everything
 * this header used to restate below 0x110 belongs to dEnemyBase_c and dActor_c
 * and is inherited now.
 *
 * The members close on each other, which is what makes the layout a reading
 * rather than a guess:
 *
 *     0x110 dCcAc_c      0x34   -> 0x144
 *     0x144 dBgCh_Actr   0x1bc  -> 0x300
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_128 = dCcAc_c.flags
 *   - unk_130 = dCcAc_c.hitFlags
 *   - unk_134 = dCcAc_c.otherOwner
 *
 * SM64DS RTTI names the implementation daBbl_c. The reconstructed factory
 * daBbl_c_classInit (historical alias LavaBubble_Spawn) constructs it for the
 * BUBBLE registry profile.
 */

#include "dEnemyBase_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* The actor heap's own allocator. daBbl_c_classInit's first call is
 * fBase_c::operator new with the literal 0x31c, not the global operator new;
 * declaring it here, and the leaf `operator new` below, is what lets the
 * factory be written as a plain `new daBbl_c()` and still emit that call.
 * Spelt exactly as include/decl_common.h spells it. */
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daBbl_c : dEnemyBase_c {
    /* The bubble runs a two-entry state table. Each entry is an enter hook the
     * transition calls once and an execute hook Behavior calls every frame,
     * both pointer-to-member -- the cartridge stores them as the eight-byte
     * {ptr, adjustment} pairs at ov064 0x0211be90..0x0211beb0, which is what
     * fixes one entry's size at eight and mExecute's offset within it at
     * eight. The two tables themselves live in this overlay's .bss at
     * 0x0211c7b8 and 0x0211c7c8 and are filled by the module's own static
     * initializer; this translation unit consumes them and does not own them. */
    typedef int (daBbl_c::*StateFn)();

    struct State {
        StateFn mEnter;
        StateFn mExecute;
    };

    dCcAc_c    mdCcAc_c;       /* 0x110 */
    dBgCh_Actr mWithMeshClsn;  /* 0x144 */
    State     *mState;         /* 0x300 -- the table this bubble is running */
    s32        mSpawnPosX;     /* 0x304 -- where InitResources found the actor */
    s32        mSpawnPosY;     /* 0x308 */
    s32        mSpawnPosZ;     /* 0x30c */
    u8         mJumps;         /* 0x310 -- clear: the fixed flame, which only
                                *          hurts; set: the jumping bubble, which
                                *          falls under gravity and is hittable */
    u8         pad_311[0x3];
    s32        mFireParticle;  /* 0x314 -- both handles are rolled forward every
                                *          frame the bubble is airborne */
    s32        mSmokeParticle; /* 0x318 */

    /* Inline empty body on purpose. From an inline destructor mwccarm emits D1
     * and then D0 -- the cartridge's own order at 0x021185c0 and 0x021185f8 --
     * and no leaf D2. Written out of line in the translation unit instead, the
     * same two bodies come out D0-before-D1 and the isolation step rejects the
     * object. Every instruction in both is compiler-generated: this class's
     * vptr store, then dBgCh_Actr at 0x144 and dCcAc_c at 0x110 in reverse
     * construction order, then the dEnemyBase_c base; D0 additionally returns
     * the object to the actor heap through dEnemyBase_c's inline
     * operator delete. */
    virtual ~daBbl_c() {}            /* slots 16, 17 */

    virtual int  InitResources();    /* slot  0 */
    virtual int  CleanupResources(); /* slot  3 */
    virtual int  Behavior();         /* slot  6 */
    virtual int  Render();           /* slot  9 */
    virtual void OnPendingDestroy(); /* slot 12 */
    virtual s32  OnYoshiTryEat();    /* slot 18 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBbl_c_size_must_be_0x31c[sizeof(daBbl_c) == 0x31c ? 1 : -1];
#endif

#endif /* DABBL_C_H */
