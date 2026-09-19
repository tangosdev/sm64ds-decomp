#ifndef DAOBJTBOX_C_H
#define DAOBJTBOX_C_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* The cartridge RTTI names this class daObjTbox_c, and that is now the
 * spelling every named virtual carries; the coined `TreasureChest` alias
 * this header used to gloss it with is gone. The ROM's
 * __si_class_type_info record gives it one direct base, dActor_c at offset
 * zero, and its 31-slot vtable has the same extent as that base. Slots 0, 3,
 * 6, 9, 16 and 17 are the only overrides. `_ZTV11daObjTbox_c` is that table,
 * at 0x0211c50c.
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
struct daObjTbox_c : dActor_c {
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

    typedef void (daObjTbox_c::*StateFunc)();

    /* Inline empty body on purpose. From an inline destructor mwccarm emits
     * D1 and then D0 -- the cartridge's own order at 0x0211a200 and
     * 0x0211a238 -- and no leaf D2. Written out of line in the translation
     * unit instead, the same two bodies come out D0-before-D1 and the
     * isolation step rejects the object. Every instruction in both is
     * compiler-generated: the vptr store, then dCcAc_c at 0x138 and ModelAnim
     * at 0x0d4 in reverse construction order, then the dActor_c base; D0 also
     * returns the object to the actor heap. */
    virtual ~daObjTbox_c() {}          /* slots 16, 17 */

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

    /* The factory's allocation goes through the actor heap, not the global
     * operator new: daObjTbox_c_classInit's first call is fBase_c's own
     * operator new with the literal 0x178. Spelling it here as a leaf
     * operator new is what lets the factory be written as a plain
     * `new daObjTbox_c()` and still emit that call. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjTbox_c_size_must_be_0x178[
    sizeof(daObjTbox_c) == 0x178 ? 1 : -1];
#endif

#endif
