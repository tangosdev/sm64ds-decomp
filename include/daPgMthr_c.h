/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * The ROM RTTI calls this class daPgMthr_c and identifies dActor_c as its base:
 * overlay_0018.bin holds the type-name string `10daPgMthr_c` at 0x02113988, and
 * the type_info record at 0x0211397c points its base word at 0x0208e390, which
 * config/arm9/symbols.txt names _ZTI8dActor_c. The class name here is the
 * cartridge's own spelling, so the compiler-emitted _ZTS reproduces the ROM
 * string exactly rather than a coined one.
 *
 * The destructor is the first virtual declared, and so this class's key
 * function; ov018/daPgMthr_c defines it, and every other override, so mwccarm
 * homes _ZTI/_ZTS/_ZTV in that translation unit.
 *
 * sizeof is independently pinned by daPgMthr_c_classInit allocating 0x38c.
 * Field names for the unknown tail are placeholders. */
#ifndef DAPGMTHR_C_H
#define DAPGMTHR_C_H
#include "types.h"
#include "dExtAnmModel_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

#include "dActor_c.h"

struct daPgMthr_c : dActor_c {
    u8  pad_0d0[0x4];
    dExtAnmModel_c mAnmModel;         /* 0x0d4 */
    dExtAnmTexPat_c mTextureSequence; /* 0x138 */
    dExtShadowModel_c mShadowModel;        /* 0x14c */
    dCcAc_c mdCcAc_c;                /* 0x174 */
    dBgCh_Actr mWithMeshClsn;        /* 0x1a8 */
    s32 mHomePosX;                   /* 0x364 */
    s32 mHomePosY;                   /* 0x368 */
    s32 mHomePosZ;                   /* 0x36c */
    /* ChangeState writes data_ov018_02113c4c + (i<<4) here; CallStateEnter
       and CallStateUpdate call through it as a PMF pair. */
    void *mState;                    /* 0x370 */
    /* ClosestPlayer / FindWithID result. StartTalk, ShowMessage,
       GetTalkState, DropActor take it as daPly_c. */
    daPly_c *mPlayer; /* 0x374 */
    /* FindNewToucher: last FindWithID hit, so a new PLAYER (0xbf) is noticed. */
    dActor_c *mLastPlayer;           /* 0x378 */
    /* The three state enters write their index 0/1/2; no reader in this TU. */
    s32 unk_37c;                     /* 0x37c */
    /* TurnHeadToward approaches these, UpdateModelAndShadow applies them. */
    s16 mLookAngX;                   /* 0x380 */
    s16 mLookAngY;                   /* 0x382 */
    s16 mMessageId;                  /* 0x384 -- 0xac / 0xad / 0xae from BeginTalk */
    u8  mGaveStar;                   /* 0x386 -- StateTalkUpdate sets when spawning POWER_STAR */
    u8  mHoldingBaby;                /* 0x387 -- BeginTalk, held actorID 0x100 (BABY_PENGUIN) */
    u8  mTalkStep;                   /* 0x388 -- StateTalkUpdate 0..3 */
    u8  mTalkTimer;                  /* 0x389 -- StateTalkEnter arms 0x3c; DecIfAbove0_Byte */
    u8  pad_38a[2];

    /* --- vtable overrides. Slots are inherited from fBase_c/dActor_c. --- */
    /* Defined here, in the class body, and not out of line in the .cpp: the
     * cartridge keeps D1 (0x02111848) BELOW D0 (0x02111898), and an out-of-line
     * member definition makes mwccarm emit the D2/D1/D0 group in the order
     * D0-then-D1, which the whole-range link refuses (`licensed .text functions
     * are not emitted in ROM address order`). Written in the class body the
     * group comes out ROM-ascending, and no D2 is emitted at all. The body is
     * genuinely empty -- every store and call in the two ROM bodies is base and
     * member destruction the compiler generates. Slots 16 (D1), 17 (D0). */
    virtual ~daPgMthr_c() {}
    virtual int InitResources();                 /* slot 0 */
    virtual int CleanupResources();              /* slot 3 */
    virtual int Behavior();                      /* slot 6 */
    virtual int Render();                        /* slot 9 */
    virtual void OnPendingDestroy();             /* slot 12 */

    /* Coined names. The state trio and the helpers it shares with the baby
       penguin follow daPgBby_c (ChangeState, CallStateEnter/Update,
       UpdateFloorAndWalls, UpdateModelAndShadow); the stand, talk and
       follow states are the N64 tuxies_mother_act_0/1/2 (tuxie.inc.c). */
    daPly_c *FindNewToucher();
    void BeginTalk(void *found, char *heldRaw);
    void TurnHeadToward(char *b);
    void CheckFollow();
    void UpdateFloorAndWalls(dBgCh_Actr *);
    void UpdateModelAndShadow();
    int StateFollowUpdate();
    int StateFollowEnter();
    int StateTalkUpdate();
    int StateTalkEnter();
    int StateStandUpdate();
    int StateStandEnter();
    void CallStateUpdate();
    void CallStateEnter();
    void ChangeState(int i);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char MotherPenguin_size_must_be_0x38c[sizeof(daPgMthr_c) == 0x38c ? 1 : -1];
#endif

#endif
