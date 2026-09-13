/* daChScene_c, ov002 0x020b09b0..0x020b0f54 -- five virtual overrides, the
 * destructor pair, a helper and the factory, all in
 * src/game/actors/d_a_ch_scene.cpp. Offsets/widths are observed, not guessed.
 * Gaps are explicit padding. Field NAMES are placeholders - renaming cannot
 * change codegen.
 *
 * daChScene_c is a TRIGGER VOLUME, not a moving actor -- it never moves and
 * never draws (Render is `return 1`). So it inherits dActor_c's motion and scale
 * slots and uses them for something else entirely. The names below are dActor_c's,
 * kept deliberately so this header does not contradict include/dActor_c.h, but
 * what this class actually does with them is:
 *
 *   mScaleX  0x080   HALF-WIDTH of the trigger box. InitResources derives it
 *                    from the spawn param's low nibble, ((n+1) * 0x64000) >> 1,
 *                    and Behavior tests |local.x| <= it.
 *   mScaleY  0x084   HEIGHT of the box, from the next nibble, tested as
 *                    0 <= local.y <= it. Two levels (0x20, 0x22) override both
 *                    with fixed sizes.
 *   mScaleZ  0x088   NOT a size at all: last frame's local z of the player.
 *                    Behavior XORs it with this frame's and tests the sign, so
 *                    the door fires on the frame the player CROSSES its plane,
 *                    then stores the new value. It is the only one of the three
 *                    written every frame.
 *   mAngleX  0x08c   genuinely an angle -- fed to the matrix below. Also
 *                    doubles as a flag: zero means an untilted door, which gets
 *                    its box grown by 0x64000 in both axes and dropped 0x32000.
 *   mAngleY  0x08e   genuinely an angle.
 *   mAngleZ  0x090   NOT an angle: the exit band, 0/1/2, chosen from how high
 *                    up the box the player crossed.
 *   mHorzSpeed 0x098 NOT a speed: the pull-through depth. Non-zero puts the
 *                    door in its second mode, where each frame drags the player
 *                    0x20000 further along local -z until it clamps at
 *                    -0x300000.
 */
#ifndef DACHSCENE_C_H
#define DACHSCENE_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daChScene_c -- daChScene_c_classInit (was daChScene_c_Spawn),
 *       g_profile_CH_SCENE (was daChScene_c_SpawnInfo)
 */
#include "types.h"
#include "math/Matrix.h"

#ifdef __cplusplus
#include "dActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daChScene_c is a real dActor_c, like every other actor in this family
 * (daChRoom_c_Spawn's neighbor, daBar_c_classInit, and daCamTag_c_Spawn; the
 * bar factory's historical alias was InvisiblePole_Spawn) --
 * mParam/mPosX/mScaleX/mAngleX/mHorzSpeed are dActor_c's own fields
 * (param1 at 0x008 via fBase_c, the rest at 0x05c..0x098), inherited rather
 * than re-declared flat as they were before. The non-motion reuse those fields
 * get here is documented in the file comment above.
 */
struct daChScene_c : dActor_c {
    u8 pad_0d0[0x4];
    /* WORLD -> DOOR-LOCAL. InitResources builds translation, then rotation Y by
       -mAngleY and X by -mAngleX, then inverts in place -- so every Behavior
       test runs in the door's own frame and the box tests are plain axis
       comparisons. */
    Matrix4x3 mInvMat;            /* 0x0d4 */

    /* MEASURED -- INLINE ON PURPOSE, do not move this body out of line.
     * Out of line, mwcc emits D2, D0, D1; the ROM has D1 at ov002 0x020b09b0
     * then D0 at 0x020b09d4 and no D2 at all. Production isolation lays .text
     * into the spanning delink in EMISSION order, so the out-of-line spelling
     * fails tubuild linkcheck even when every function's bytes match and
     * objisolate calls the object clean -- objisolate checks one object's
     * relocations, never the order the linker will see. Inline, the emission
     * is D1, D0, no D2: the ROM's own order.
     *
     * Safe here only because this class is a LEAF. Its _ZTI is ov002
     * 0x02108674, and the only word in any extracted binary pointing at it is
     * 0x021086b0 -- the class's own vtable slot -- so no other class names it
     * as a base and no other TU's codegen moves with this. See the leaf proof
     * in src/game/actors/d_a_ch_scene.cpp. */
    virtual ~daChScene_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daChScene_c_size_must_be_0x104[sizeof(daChScene_c) == 0x104 ? 1 : -1];

#endif /* __cplusplus */

#endif
