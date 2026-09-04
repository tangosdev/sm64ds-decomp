/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class daChScene_c: 4 matched functions, 12 evidenced fields.
 * (That generator line is a 2026-08 snapshot and is left as written for
 * provenance. The class as shipped owns nine functions in ov002
 * 0x020b09b0..0x020b0f54 -- five virtual overrides, the destructor pair,
 * a helper and the factory -- all in src/game/actors/d_a_ch_scene.cpp.)
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED 2026-08-09 from all five methods. Do not regenerate over this.
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
#include "types.h"
#include "math/Matrix.h"

#ifdef __cplusplus
#include "dActor_c.h"

/* daChScene_c is a real dActor_c, like every other actor in this family
 * (daChRoom_c_Spawn's neighbor, daBar_c_classInit, and daCamTag_c_Spawn; the
 * bar factory's historical alias was InvisiblePole_Spawn) --
 * mParam/mPosX/mScaleX/mAngleX/mHorzSpeed above are dActor_c's own fields
 * (param1 at 0x008 via fBase_c, the rest at 0x05c..0x098), inherited rather
 * than re-declared flat as they were before. Only the offsets and inherited
 * NAMES survive the fix; the non-motion reuse those fields get here is
 * unchanged and still documented on include/dActor_c.h's own field list plus
 * the notes below, which describe what THIS class does with them:
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
};

typedef char daChScene_c_size_must_be_0x104[sizeof(daChScene_c) == 0x104 ? 1 : -1];

#else

/* Flat layout for the C translation units, which can express neither the
   base class nor the virtual functions -- see include/dActor_c.h's own
   #else branch for the same split. mParam/mPosX/mScaleX/mAngleX/mHorzSpeed
   below are dActor_c's own fields, re-listed here only because C cannot
   inherit them; their non-motion reuse as this class's trigger-box state is
   documented in the struct above. */
struct daChScene_c {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x6];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x38];
    Matrix4x3 mInvMat;            /* 0x0d4 */
};

typedef char daChScene_c_size_must_be_0x104_c[sizeof(struct daChScene_c) == 0x104 ? 1 : -1];

#endif /* __cplusplus */

#endif
