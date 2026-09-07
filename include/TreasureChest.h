#ifndef TREASURECHEST_H
#define TREASURECHEST_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

/* The cartridge RTTI names this class daObjTbox_c. TreasureChest is the
 * readable compatibility spelling carried by every named virtual. The ROM's
 * __si_class_type_info record gives it one direct base, dActor_c at offset
 * zero, and its 31-slot vtable has the same extent as that base. Slots 0, 3,
 * 6, 9, 16 and 17 are the only overrides. `_ZTV13TreasureChest` and
 * `_ZTV11daObjTbox_c` are address aliases for the same table at 0x0211c50c.
 *
 * The state ownership is ROM-backed rather than inferred from adjacency. The
 * overlay initializer at 0x0211b65c copies six PMF constants into the table at
 * data_ov064_0211c98c in the exact pairs documented below. SetState indexes a
 * pair and invokes its first PMF; Behavior invokes the second. Descriptive
 * original names are absent, so the evidenced indices are retained.
 *
 * The factory allocates 0x178 bytes and constructs ModelAnim at 0x0d4 and
 * dCcAc_c at 0x138. D1 and D0 destroy those same members in reverse order,
 * independently pinning both member types, offsets and the total class size.
 */
struct TreasureChest : dActor_c {
    u32 unk_0d0;                         /* 0x0d0 -- not accessed by this TU */
    ModelAnim mModelAnim;                /* 0x0d4 */
    dCcAc_c mCylinder;                   /* 0x138 */
    s32 mState;                          /* 0x16c -- indexes the three PMF pairs */
    u16 mStateTimer;                     /* 0x170 -- opening/star countdown */
    u8 mOrder;                           /* 0x172 -- required chest order, 1-based */
    u8 mIsLastChest;                     /* 0x173 -- this chest will spawn the star */
    u8 mStarID;                          /* 0x174 -- 0xff means no star */
    s8 mTrackedStarID;                   /* 0x175 -- marker slot from TrackStar */
    u8 mPad176[0x2];                     /* 0x176 */

    typedef void (TreasureChest::*StateFunc)();

    virtual ~TreasureChest();            /* slots 16, 17 */

    virtual int InitResources();         /* slot  0 */
    virtual int CleanupResources();      /* slot  3 */
    virtual int Behavior();              /* slot  6 */
    virtual int Render();                /* slot  9 */

    void InitState0();
    void State0();
    void InitState1();
    void State1();
    void InitState2();
    void State2();

    void UpdateModelTransform();
    void SetState(s32 state);
    void CallStateBehavior();
};

typedef char TreasureChest_size_must_be_0x178[
    sizeof(TreasureChest) == 0x178 ? 1 : -1];

#endif
