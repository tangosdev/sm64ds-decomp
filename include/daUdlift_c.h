/* The up/down lift in BBH, HMC and RR. All three factories allocate 0x34c
 * bytes, call dBgActor_c::dBgActor_c() and install _ZTV10daUdlift_c.
 *
 * The destructor proves the hierarchy: it swaps the vptr from daUdlift_c to
 * dBgActor_c, destroys dBgActor_c's dBgW_KcMbg and Model members and chains
 * to dActor_c. In C++ that is all implied by the base, so the source
 * destructor is empty.
 *
 * InitResources reads the inherited mPrevAngleX and mPrevAngleZ as unsigned
 * shaft lengths rather than angles:
 *     mBottomY = mTopY - (mPrevAngleX << 12)
 *     mTopY    = mPosY + (mPrevAngleZ << 12)   (mMode 2 only)
 *
 * mMode picks what the lift does at either end:
 *     0  follow the player (BBH, HMC)
 *     1  stop at the top; at the bottom stop in state 4 and climb back once
 *        re-armed (RR)
 *     2  raise mTopY by mPrevAngleZ and stop at the bottom in state 3
 * InitResources only ever stores 0 or 1, so mode 2 is dead code in the
 * shipped game; it is reproduced because the cartridge has it.
 *
 * The factory and profile names (daUdlift_c_classInit_UDLIFT_TERESA at
 * 0x021367fc, its descriptor g_profile_UDLIFT_TERESA at 0x021375cc) are
 * reconstructed, not recovered SM64DS symbols.
 */
#ifndef DAUDLIFT_C_H
#define DAUDLIFT_C_H
#include "types.h"
#include "dBgActor_c.h"

struct daUdlift_c : dBgActor_c {
    /* dBgActor_c's last member ends at 0x31e. Keep its two bytes of tail
       padding so the first word owned by this class starts at 0x320. */
    u8 pad_31e[0x2];
    Player *mRider;                 /* 0x320 */
    Player *mClosestPlayer;         /* 0x324 */
    s32 mVariant;                   /* 0x328 */
    s32 mState;                     /* 0x32c */
    s32 mPlayerPosY;                /* 0x330 */
    s32 mTopY;                      /* 0x334 */
    s32 mBottomY;                   /* 0x338 */
    s32 mMiddleY;                   /* 0x33c */
    u32 mSoundHandle;               /* 0x340 */
    u16 mStateTimer;                /* 0x344 */
    u8 mIsAtBottom;                 /* 0x346 */
    u8 mIsArmed;                    /* 0x347 */
    u8 mIsRidden;                   /* 0x348 */
    u8 mMode;                       /* 0x349 */

    /* Inline and empty, so InitResources is the key function and its TU
       emits the vtable and RTTI. Written out of line, mwccarm emits D0 ahead
       of D1 (the cartridge has D1 first) plus a D2 with no ROM home. Keep
       the brace on this line: tools/check_header_offsets.py needs it. */
    virtual ~daUdlift_c() {}        /* slots 16 (D1), 17 (D0) */
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* The states Behavior runs out of its table, indexed by mState. */
    void StateWait();               /* 0 */
    void StateMoveUp();             /* 1 */
    void StateMoveDown();           /* 2 */
    void StateStop();               /* 3 */
    void StateStopAtBottom();       /* 4 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daUdlift_c_size_must_be_0x34c[sizeof(struct daUdlift_c) == 0x34c ? 1 : -1];
#endif

#endif
