#ifndef DADGR_C_H
#define DADGR_C_H

#include "types.h"
#include "dBgW_KcMbg.h"
#include "Model.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Spindel (actor 163 DONGURU). RTTI ov025:0x02113730 names `7daDgr_c`.
 * g_profile_DONGURU (ov025:0x0211373c) is this class's registry descriptor.
 * overlay_actors lists SPINDEL(163) on ov021 because the actor is used in HMC;
 * the class TU is ov025. GRINDEL(162) on ov025 is daDkk_c (DONKAKU), not this
 * class. No pretty name is coined.
 *
 * BASE: dBgActor_c, direct. The RTTI edge (base_key ov002:0x021089ec, offset 0)
 * agrees with the destructor: D1 stores exactly two vtables -- `_ZTV7daDgr_c`
 * then `_ZTV10dBgActor_c` -- one intermediate-free inlined base.
 *
 * SIZE IS 0x334 (820 decimal), THE LITERAL THE FACTORY PASSES TO operator new.
 * The factory is `return new daDgr_c()`: leaf `operator new(unsigned long)`
 * forwards `_ZN7fBase_cnwEj`, the implicit ctor calls `dBgActor_c()` and
 * stores `_ZTV7daDgr_c`. No dedicated `daDgr_c()` body exists in the ROM.
 *
 * MEMBERS. dBgActor_c ends at 0x320; daDgr_c adds 0x14 more bytes and destructs
 * none of them (func_ov025_021111a0 tears down only dBgActor_c's own Model and
 * dBgW_KcMbg, then chains to dActor_c -- nothing at 0x320 or later).
 * That means everything from 0x320 to 0x334 is POD, read from InitResources
 * (func_ov025_021117dc) and Behavior (func_ov025_021113f0):
 *
 *   0x320  mBasePosY       -- InitResources copies mPosY (dActor_c +0x60) here;
 *                              Behavior reads it back as the resting height a
 *                              sine offset is added to.
 *   0x324  mAngleXSpeed    -- the per-tick angular step, +-(0x400/n), added to
 *                              mAngleX every frame of a roll; Behavior only.
 *   0x326  mPhaseTimer     -- a phase counter Behavior increments every call and
 *                              compares against several thresholds.
 *   0x328  mRollStage      -- a stage counter, -1 sentinel, incremented up to
 *                              0x14 and read as `10 - value` to shape the speed.
 *   0x329  mRollDir        -- toggled with XOR 1 at the end of a roll; it picks
 *                              the sign of mAngleXSpeed and of the Z step.
 *   0x32a  --              -- never dereferenced by any function in the class;
 *                              2 bytes of padding closing the s8/u8 pair up to
 *                              the next s32.
 *   0x32c  mDustParticle1  -- a Particle::System::New() handle (effect 0x2d),
 *                              read back as that call's own first argument
 *                              (handle recycling); positioned 100 units along
 *                              the platform's facing.
 *   0x330  mDustParticle2  -- the same handle shape for the other end, 100
 *                              units the opposite way.
 *
 * 0x320 + 0x14 = 0x334, closing exactly on the factory's literal. Types are
 * proven from the accesses above, not guessed; the per-offset citations are in
 * notes/bgobject-provenance.md.
 *
 * SM64DS RTTI names the implementation daDgr_c. The reconstructed factory
 * daDgr_c_classInit (historical alias func_ov025_02111898) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_DONGURU
 * (historical alias data_ov025_0211373c) is its registry descriptor.
 */

struct daDgr_c : dBgActor_c {
    s32 mBasePosY;
    s16 mAngleXSpeed;
    u16 mPhaseTimer;
    s8  mRollStage;
    u8  mRollDir;
    u8  pad_32a[0x2];
    u32 mDustParticle1;
    u32 mDustParticle2;

    /* Leaf allocator until #2570 merges a shared fBase_c::operator new.
       `unsigned long` is size_t on this ABI; `unsigned int` is illegal. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* --- vtable, in ROM order. Do not reorder. ---
     * DECLARED INLINE, AND FIRST. This is measured against the cartridge, not
     * a style choice: ov025 puts D1 at 0x021111a0 and D0 at 0x021111e4, i.e.
     * D1 BELOW D0, and carries no D2 at all. mwccarm 2004/b56 emits exactly
     * that pair, in exactly that order, only for a destructor defined in the
     * class body; the out-of-line form emits D2, D0, D1 plus a homeless D2 and
     * fails linkcheck's ROM-ascending .text audit. The body is empty because
     * this class owns no member with a destructor of its own -- the compiler
     * emits the two vtable stores and dBgActor_c's Model / dBgW_KcMbg
     * teardowns from the base clause alone.
     *
     * (This header previously declared it out of line, reasoning by analogy
     * with include/BigBrickBlock.h. The analogy is wrong for this class: the
     * ROM's own D1/D0 addresses settle it, and the one-function shards could
     * not see the ordering because each held only one variant.)
     *
     * SIDE EFFECT, AND IT IS BENIGN: an inline destructor is skipped when the
     * key function is chosen, so the key function is now InitResources -- the
     * first non-inline virtual DECLARED below -- and no longer the destructor.
     * Both live in src/actors/daDgr_c.cpp, so the same eleven data symbols
     * (_ZTV7daDgr_c plus ten RTTI records) are emitted from the same TU either
     * way and the promotion's compiler_only_output stays at 12 rows; the
     * correction did not move the eligibility bracket. Declaring a new virtual
     * ABOVE InitResources would move the key function again. */
    virtual ~daDgr_c() {}

    /* --- overrides of inherited fBase_c slots. Each takes its base's index
     *     (see include/fBase_c.h for the full 32-slot table). --- */
    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */

    /* slot 31 (Kill) is NOT overridden: the vtable word at data_ov025_02113760
       + 0x7c is 0x020ee55c, identical to dBgActor_c's own slot 31 -- confirmed
       by reloc, not assumed. */
};

typedef char daDgr_c_size_must_be_0x334[sizeof(daDgr_c) == 0x334 ? 1 : -1];

#endif /* __cplusplus */

#endif
