/* The ball in the Smartball minigame -- one of eleven direct children of
 * cMgSmartball_object_c. Size 0x12c. The layout
 * below comes from this class's own RestoreInitial, which zeroes every field
 * exhaustively.
 *
 * FIELD NAMES, and the evidence behind each (including the two out-of-scope
 * helpers that lend several of them), are tabulated in
 * notes/smartball-provenance.md. ONE field keeps an unk_ name on purpose:
 * unk_039 has no reader anywhere in the tree, so there is nothing to name it
 * from.
 *
 * TWO FIELDS THAT USED TO SAY THAT ARE NAMED NOW, and the correction is worth
 * recording because it is the same mistake twice: both readers live OUTSIDE
 * this class, reaching the ball through the manager's table rather than
 * through `this`, so a search of this class's own four functions found
 * nothing and concluded there was nothing. mCollisionCooldown (0x0fc) is read
 * and written by func_ov006_02115830, the ball-vs-ball collision resolver;
 * mPipeUsed (0x120) by cMgSmartball_dokan_c::SaveSnapshot. Neither spells the
 * field by name -- both cast a raw offset off a table entry -- which is why
 * grepping for the field name found nothing either. */
#ifndef CMGSMARTBALL_BALL_C_H
#define CMGSMARTBALL_BALL_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_ball_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 mZoneCooldown;   /* 0x034 -- countdown; SaveSnapshot decrements it while
                             >0 and refuses to re-latch mUpperWallSolid until it
                             has drained. Zeroed by RestoreInitial. */
    u8  mUpperWallSolid; /* 0x038 -- latched to 1 by SaveSnapshot when the ball
                             sits in the box x [0xe8000,0xf0000) z
                             [0x78000,0xa8000) with mZoneCooldown drained. Its
                             one reader is func_ov006_021126b4, the
                             out-of-bounds predicate: while this is set, that
                             function reports z >= 0x78000 in the x band
                             [0xd8000,0xe0000) as blocked -- the boundary the
                             ball just came through is solid behind it. */
    u8  unk_039;      /* 0x039 -- cleared alongside mUpperWallSolid and by
                          RestoreInitial; NOTHING IN THE TREE READS IT. No
                          evidence, so no name. */
    u8  state3a;      /* 0x03a -- name from func_ov006_02112ad8.c's own
                          Obj cast of this same class; gates most of that
                          function's per-frame collision work */
    u8  state3b;      /* 0x03b -- ditto; set once mZoneDwell maxes out */
    s32 mZoneDwell;      /* 0x03c -- frames spent inside the box x [0x8000,
                             0xd8000) z [0x74000,0x7c000); counts up, sets
                             state3b past 0x1e, saturates at 0x12c, and resets
                             to 0 the moment the ball leaves. */
    s32 mExpireTimer;    /* 0x040 -- despawn countdown. func_ov006_02111dcc arms
                             it (and refuses to re-arm while it is positive);
                             SaveSnapshot ages it and, on expiry, clears the
                             base's mIsActive -- after which every SaveSnapshot
                             and Update returns immediately. */

    /* hitX/hitZ per func_ov006_02112ad8, but nothing in this class reads
       them -- unmodelled, not unread. */
    u8  pad_044[0x8]; /* 0x044-0x04b */

    u8  hit[0x20];      /* 0x04c */
    u8  hitA[0x20];     /* 0x06c */
    u8  hitB[0x20];     /* 0x08c */
    u8  hitC[0x20];     /* 0x0ac */
    u8  anyHit;          /* 0x0cc */
    u8  specialHit;       /* 0x0cd -- SaveSnapshot bails out early while set */
    u8  nearby[0x19];    /* 0x0ce */

    u8  pad_0e7[0x11]; /* 0x0e7-0x0f7 -- genuine gap; RestoreInitial's
                          exhaustive zero pass skips straight over it */

    s32 targetIndex;  /* 0x0f8 -- set to -1 by RestoreInitial */
    s32 mCollisionCooldown; /* 0x0fc -- frames before this ball may exchange
                             velocity with another one again. SaveSnapshot ages
                             it while >0 and RestoreInitial zeroes it, but the
                             evidence is func_ov006_02115830, the ball-vs-ball
                             resolver: given two balls it returns without
                             touching either velocity when EITHER one's counter
                             is still positive, and it sets BOTH counters to 3
                             on the way out of either path. Three frames of
                             deafness after a hit, on both participants. */
    u8  mIsWaiting;      /* 0x100 -- 1 while this ball is still queued behind the
                             one in play. RestoreInitial sets it; SaveSnapshot
                             clears it once the base's mIndex (this ball's own
                             slot index) equals the manager's current index at
                             mgr+0x4664 and mInPlay is up. While set,
                             SaveSnapshot runs func_ov006_021128fc, which lines
                             the ball up behind its predecessor in the
                             manager's 13-entry table at mgr+0x4688; once clear
                             it runs the live physics path
                             func_ov006_02112ad8 instead. */
    u8  pad_101[0x3]; /* 0x101-0x103 -- alignment */
    s32 mQueueGap;       /* 0x104 -- (this ball's slot index - the manager's
                             current index) * 0xf, written twice by
                             func_ov006_021128fc while queued and zeroed by
                             RestoreInitial. Nothing in the tree reads it back;
                             the name records the arithmetic, which is
                             unambiguous, not a use. */
    s32 mFrozenPos0;     /* 0x108 -- the position the ball is pinned to. Written
                             only by func_ov006_02111d4c, which copies mCurrent
                             here and raises mIsFrozen. */
    s32 mFrozenPos1;     /* 0x10c -- ditto, the z half. */
    u8  mIsFrozen;       /* 0x110 -- while set, SaveSnapshot forces mCurrent back
                             to mFrozenPos every frame, zeroes the base's
                             velocity pair and returns before any physics.
                             Raised by func_ov006_02111d4c (which also sets
                             state3b); only RestoreInitial ever clears it. */
    u8  pad_111[0x3]; /* 0x111-0x113 -- alignment */
    s32 mLastPos0;       /* 0x114 -- the last position at which the ball counted
                             as moving. SaveSnapshot refreshes the pair from
                             mCurrent whenever the ball is in bounds and has
                             travelled 0x8000 or more from it. */
    s32 mLastPos1;       /* 0x118 -- the z half. */
    s32 mStuckFrames;    /* 0x11c -- frames since that refresh: incremented when
                             the ball is out of bounds or has not moved 0x8000,
                             reset to 0 on a refresh, zeroed by RestoreInitial.
                             func_ov006_02111df4 reports the ball as needing a
                             rescue once it reaches 0x78 (120 frames). */
    u8  mPipeUsed;       /* 0x120 -- one-shot: this ball has already been taken
                             by the pipe. cMgSmartball_dokan_c::SaveSnapshot
                             sweeps the manager's ball table, skips any ball
                             with this set, and -- for one that is in the mouth
                             of the pipe (within 0x8000 in x, between -0x40000
                             and -0x38000 in z of the pipe) -- warps it, scores
                             it, and sets this. RestoreInitial zeroes it, which
                             is what makes it once per ball per round. */
    u8  mInPlay;         /* 0x121 -- 1 while this ball is a live participant.
                             Update draws nothing while it is 0;
                             func_ov006_02111df4 reports a ball with it clear as
                             finished; func_ov006_021128fc uses it on both this
                             ball and its predecessor to decide who to line up
                             behind; func_ov006_02111e7c raises it.
                             RestoreInitial clears it only for slot indices
                             >= 9 on the second board. */
    u8  pad_122[0x2]; /* 0x122-0x123 -- alignment */
    s32 soundTimer;   /* 0x124 -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  soundPlayed;  /* 0x128 -- zeroed by RestoreInitial */
    u8  mExitGateOpen;   /* 0x129 -- one-shot latch. SaveSnapshot raises it once
                             the ball has crossed the plane through the corner
                             (0xd8000, -0x80000) by more than the base's
                             mRadius. Until then func_ov006_021126b4 reports
                             z < -0x80000 as out of bounds, so the ball cannot
                             leave that way; func_ov006_021122e0 likewise gates
                             its bottom-right exit test on it. */
    u8  pad_12a[0x2]; /* 0x12a-0x12b -- alignment */
};

typedef char cMgSmartball_ball_c_size_must_be_0x12c[sizeof(cMgSmartball_ball_c) == 0x12c ? 1 : -1];

#endif
