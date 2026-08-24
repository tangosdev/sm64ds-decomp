/* The ball in the Smartball minigame -- one of eleven direct children of
 * cMgSmartball_object_c. Size 0x12c. The layout
 * below comes from this class's own RestoreInitial, which zeroes every field
 * exhaustively; see notes/minigame-provenance.md. */
#ifndef CMGSMARTBALL_BALL_C_H
#define CMGSMARTBALL_BALL_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_ball_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034;      /* 0x034 -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  unk_038;      /* 0x038 -- with unk_039/state3a/state3b, a small
                          zone-entry state group SaveSnapshot latches when
                          unk_034 has drained inside a trigger box */
    u8  unk_039;      /* 0x039 */
    u8  state3a;      /* 0x03a -- name from func_ov006_02112ad8.c's own
                          Obj cast of this same class; gates most of that
                          function's per-frame collision work */
    u8  state3b;      /* 0x03b -- ditto; set once unk_03c maxes out */
    s32 unk_03c;      /* 0x03c -- zone-dwell counter, capped at 0x12c,
                          reset to 0 outside the zone */
    s32 unk_040;      /* 0x040 -- countdown; func_ov006_02111dcc refuses to
                          restart it while positive, and its expiry clears
                          the base's unk_030 flag */

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
    s32 unk_0fc;      /* 0x0fc -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  unk_100;      /* 0x100 -- selects between func_ov006_021128fc and
                          func_ov006_02112ad8 in SaveSnapshot */
    u8  pad_101[0x3]; /* 0x101-0x103 -- alignment */
    s32 unk_104;      /* 0x104 -- zeroed here; func_ov006_021128fc (out of
                          scope) sets it from elapsed-frame lag */
    s32 unk_108;      /* 0x108 -- read-only in this class's own functions;
                          copied into mCurrent0 when unk_110 is set */
    s32 unk_10c;      /* 0x10c -- ditto, copied into mCurrent1 */
    u8  unk_110;      /* 0x110 -- flag that triggers the unk_108/unk_10c
                          position reset */
    u8  pad_111[0x3]; /* 0x111-0x113 -- alignment */
    s32 unk_114;      /* 0x114 -- with unk_118, a position pair read/written
                          as a V2 by SaveSnapshot/RestoreInitial */
    s32 unk_118;      /* 0x118 */
    s32 unk_11c;      /* 0x11c -- incremented while the position pair above
                          isn't refreshed; zeroed by RestoreInitial */
    u8  unk_120;      /* 0x120 -- write-only in this class's own functions */
    u8  unk_121;      /* 0x121 -- gates the SaveSnapshot/Update branches
                          RestoreInitial derives from the base's unk_02c and
                          the scene's entity count */
    u8  pad_122[0x2]; /* 0x122-0x123 -- alignment */
    s32 soundTimer;   /* 0x124 -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  soundPlayed;  /* 0x128 -- zeroed by RestoreInitial */
    u8  unk_129;      /* 0x129 -- flag SaveSnapshot latches once and
                          RestoreInitial clears */
    u8  pad_12a[0x2]; /* 0x12a-0x12b -- alignment */
};

typedef char cMgSmartball_ball_c_size_must_be_0x12c[sizeof(cMgSmartball_ball_c) == 0x12c ? 1 : -1];

#endif
