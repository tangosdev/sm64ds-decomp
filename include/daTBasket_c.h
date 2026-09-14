#ifndef DATBASKET_C_H
#define DATBASKET_C_H

#include "types.h"

/* daTBasket_c -- the Boo cage (T_BASKET), ov063.
 *
 * Big Boo's Haunt furniture: the bouncing cage that plays the secret chime
 * while airborne and lets a grounded player jump inside it
 * (Player::JumpIntoBooCage). The class name is the ROM's own: _ZTS11daTBasket_c
 * at ov063:0x0211e750, with _ZTI11daTBasket_c reaching _ZTI12dEnemyBase_c, so
 * the direct base is dEnemyBase_c. The member shape is da1up_c's exactly --
 * same four types at the same four offsets -- which is what a shared
 * spawn-and-hold idiom looks like; the classes differ only in total size.
 *
 * LAYOUT IS READ TWICE, not once. daTBasket_c_classInit (0x0211c4d0) allocates
 * 0x380, chains dEnemyBase_c's constructor, stores this vtable, and constructs
 * dCcAc_c at +0x110, dBgCh_Actr at +0x144, Model at +0x300 and ShadowModel at
 * +0x350; _ZN11daTBasket_cD1Ev (0x02115fc4) destroys the same four at the same
 * offsets in exactly the reverse order. It closes: 0x350 + sizeof(ShadowModel)
 * 0x28 = 0x378, and the tail fields reach the 0x380 the ROM allocates.
 *
 * SPLIT, AND THAT IS LAYOUT, NOT TASTE. The class owns seven functions and the
 * ROM interleaves five of them with daTrs_c/daTrsIcon_c (Cleanup at 0x0211ae1c
 * sits between a helper and daTrs_c::CleanupResources; Render, Behavior,
 * InitResources and the factory each sit between other classes' members), so
 * no single file can own the run: the link places one .text section per
 * object. The true original TU is the whole Boo family; this class keeps one
 * file per function until that merge. The destructor pair IS contiguous
 * (D1 0x02115fc4, D0 0x0211600c) and stays two shards of one definition for
 * the same reason: merging them needs manifest licensing for the homeless D2
 * and the vague-linkage passengers, which a split TU does not have.
 *
 * deslop leftovers:
 * - (Vector3 *)&mPosX for the JumpIntoBooCage argument: dActor_c has no Pos()
 *   on this branch yet, so the leaf pun stays (S18; the header owns the fix).
 * - dCcAc_c::Init / dBgCh_Actr::Init stay TU-local mangled scalars: the
 *   header member forms take Fix12<int> BY VALUE but the tree's Fix12 has no
 *   int constructor (measured: neither a bare 0x64000 nor Fix12<int>(0x64000)
 *   converts), so only the scalar spelling reproduces the ROM's raw-word
 *   passing; dBgCh_Actr::Init's header additionally takes Fix12i, which
 *   mangles as i while the ROM is Fix12<int>.
 * - Sound::PlaySecretSound / Particle::System::New stay TU-local mangled: no
 *   shared header declares them yet (d_a_wanwan / da1up precedent).
 * - func_ov063_021169c4 is this class's own shadow helper (writes
 *   mModel.mat4x3, drops mShadowModel; Behavior is its only caller) but is
 *   enrolled as its own shard and is not contiguous with this TU, so the call
 *   stays by ROM label.
 * - data_ov063_0211edec is the shared model file both this class and daTrs_c
 *   load and release; the sinit owns it, this TU only externs it.
 * - Factory stays hand-rolled (typed C-ABI): `return new` reproduces the
 *   bytes but links the vptr slot +8 past the ROM; see d_a_t_basket.cpp.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daTBasket_c : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    Model                        mModel;                /* 0x300 */
    ShadowModel                  mShadowModel;          /* 0x350 */
    s32                          mParticleID;               /* 0x378 */
    s16                          mSoundTimer;               /* 0x37c */
    u8                           mMuteSecretSound;      /* 0x37e -- nonzero skips Sound::PlaySecretSound */
    u8  pad_37f[0x1];

    /* --- vtable ---
     * Overrides of fBase_c virtuals, so each takes the base's slot regardless of
     * the order declared here; the ROM's _ZTV11daTBasket_c @ 0x0211e930 puts
     * ov063 code in exactly these five and inherits every other entry.
     * The destructor is declared FIRST and stays OUT OF LINE: it is this
     * class's key function, so the TU that defines it emits the vtable. */
    virtual ~daTBasket_c();          /* slots 16 (D1), 17 (D0) */
    virtual s32 InitResources();     /* slot  0 -- ov063:0x0211c35c */
    virtual s32 CleanupResources();  /* slot  3 -- ov063:0x0211ae1c */
    virtual s32 Behavior();          /* slot  6 -- ov063:0x0211b888 */
    virtual s32 Render();            /* slot  9 -- ov063:0x0211b078 */

    /* Leaf adapter until fBase_c::operator new(unsigned long) lands (#2570).
       `return new daTBasket_c()` then routes through the retail allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daTBasket_c_size_must_be_0x380[sizeof(daTBasket_c) == 0x380 ? 1 : -1];
#endif

#endif /* DATBASKET_C_H */
