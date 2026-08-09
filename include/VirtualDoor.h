/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class VirtualDoor: 4 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED 2026-08-09 from all five methods. Do not regenerate over this.
 *
 * VirtualDoor is a TRIGGER VOLUME, not a moving actor -- it never moves and
 * never draws (Render is `return 1`). So it inherits Actor's motion and scale
 * slots and uses them for something else entirely. The names below are Actor's,
 * kept deliberately so this header does not contradict include/Actor.h, but
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
#ifndef VIRTUALDOOR_H
#define VIRTUALDOOR_H
#include "types.h"
#include "math/Matrix.h"

struct VirtualDoor {
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
    /* WORLD -> DOOR-LOCAL. InitResources builds translation, then rotation Y by
       -mAngleY and X by -mAngleX, then inverts in place -- so every Behavior
       test runs in the door's own frame and the box tests are plain axis
       comparisons. Was declared `u8 unk_0d4`; it is 48 bytes and the struct
       grows from 0xd5 to 0x104 accordingly. */
    Matrix4x3 mInvMat;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
