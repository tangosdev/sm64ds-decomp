/* dScMg3DEsp_c, scene 0x185 (profile MG_3DESP). The ROM's RTTI names the
 * class and its one base, dScMgSingle3DBase_c. The factory
 * dScMg3DEsp_c_classInit, historically MgPsycheOut_Spawn, allocates 0x5558
 * bytes.
 *
 * The factory and the destructor fix the members after the base:
 *   - two Models at 0x4f38 and 0x4f88, 0x50 bytes each
 *   - a dMg3DEspModel_c at 0x4fd8, 0x21c bytes (mEspModel)
 *   - a dExtAnmTexSRT_c at 0x51f4, 0x14 bytes
 *   - this class's own fields from 0x5208 to 0x5558
 * They are typed members: the factory is `return new dScMg3DEsp_c;`, whose
 * implicit constructor builds them in declaration order, and the destructor
 * tears them down in reverse.
 *
 * The destructor is the key function, defined out of line in
 * src/minigames/d_mg3d_esp_anim_set.cpp, which emits both D1 and D0.
 * dScMgBase_c's operator delete serves D0.
 */
#ifndef DSCMG3DESP_C_H
#define DSCMG3DESP_C_H
#include "dScMgSingle3DBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dMg3DEspModel_c.h"
#include "dExtAnmTexSRT_c.h"

struct dScMg3DEsp_c : dScMgSingle3DBase_c {
    /* One of the five cards, 0x18 bytes. DealCards places it and
       CardTouch/CardSeek fly it to the reveal spot at (0x80, 0x30). */
    struct Card {
        s32 mX;       /* 0x00, Fix12 screen position */
        s32 mY;       /* 0x04 */
        s32 mSpeed;   /* 0x08, CardSeek adds 0x300 a frame; CardTouch zeroes it */
        u16 mTimer;   /* 0x0c, CardSeek waits it out; CardTouch sets 0x20 */
        u16 mAngle;   /* 0x0e, heading to the reveal spot (CardTouch's atan2) */
        u8 mActive;   /* 0x10, UpdateCards runs the card while set */
        u8 mState;    /* 0x11, s3DEspCardTable index: 0 CardExit, 1 touch, 2 seek */
        u8 mVisible;  /* 0x12, RenderCards draws the card while set */
        u8 mFace;     /* 0x13, sprite index; matched against mTarget */
        u8 mPriority; /* 0x14, OAM priority; DealCards sets 1, CardExit drops it to 0 */
        u8 pad_15[3]; /* 0x15 */
    };

    /* One of the three emitters SpawnRows copies a tapped card's spot to,
       0x14 bytes. */
    struct Row {
        s32 mX;       /* 0x00 */
        s32 mY;       /* 0x04 */
        s32 mScale;   /* 0x08, Fix12 sprite scale; 0x1400 at spawn, then decays */
        u16 mTimer;   /* 0x0c, RowWait's countdown */
        u8 pad_0e[2]; /* 0x0e */
        u8 mActive;   /* 0x10, UpdateRows runs the row while set */
        u8 mState;    /* 0x11, s3DEspRowTable index: 0 wait, 1 decay, 2 fade */
        u8 mVisible;  /* 0x12, RenderRows draws the row while set */
        u8 mPriority; /* 0x13, OAM priority; SpawnRows sets 0 */
    };

    /* One of the twenty falling slots, 0x20 bytes. */
    struct Slot {
        s32 mX;        /* 0x00, Fix12 screen position */
        s32 mY;        /* 0x04 */
        s32 mVelX;     /* 0x08, added to mX each frame by the type handlers */
        s32 mVelY;     /* 0x0c, added to mY; SlotFall pulls it down by mGravity */
        u16 mLife;     /* 0x10, SlotFall's countdown; at 0 the slot waits again */
        u16 mDelay;    /* 0x12, frames the rise/kick handlers hold before pushing */
        u16 mPushTime; /* 0x14, frames they push mVelX for */
        u16 mWait;     /* 0x16, SlotWait's countdown before a relaunch */
        u8 mActive;    /* 0x18, UpdateSlots runs the slot while set */
        u8 mAccel;     /* 0x19, 0..7; the push step is (mAccel << 4) + 0x10 */
        u8 mGravity;   /* 0x1a, 0..7; SlotFall's pull is (mGravity << 3) + 0x10 */
        u8 mFrame;     /* 0x1b, sprite index; 7 - min(mLife >> 3, 7) */
        u8 mVisible;   /* 0x1c, RenderSlots draws the slot while set */
        u8 mState;     /* 0x1d, s3DEspSlotFallTable index: 0 wait, 1 fall */
        u8 mType;      /* 0x1e, s3DEspSlotRiseTable index: 0 setup, 1 rise,
                          2 kick, 3 dampen */
        u8 pad_1f;     /* 0x1f */
    };

    virtual ~dScMg3DEsp_c();
    virtual void StartRound(int arg);               /* slot 18 */
    virtual void OnNewRecord();                          /* slot 20 */

    dExtSimpleModel_c mModel1;           /* 0x4f38 */
    dExtSimpleModel_c mModel2;           /* 0x4f88 */
    dMg3DEspModel_c mEspModel;           /* 0x4fd8 */
    dExtAnmTexSRT_c mTextureTransformer; /* 0x51f4 */
    Card mCards[5];                /* 0x5208 */
    Row mRows[3];                  /* 0x5280 */
    Slot mSlots[20];               /* 0x52bc */
    s32 mRoundState;               /* 0x553c, the state index Behavior dispatches on */
    s32 mPhase;                    /* 0x5540, the play sub-state index */
    s32 mDifficulty;               /* 0x5544 */
    u16 mTimer;                    /* 0x5548, the round clock */
    u16 mFadeTick;                 /* 0x554a, 4-tick divider for the blend ramp */
    u16 mIdleTimer;                /* 0x554c */
    u8  mFade;                     /* 0x554e, blend level 0..0x10 */
    u8  mTouched;                  /* 0x554f, set when a card is tapped */
    u8  mCorrect;                  /* 0x5550, the pick matched the target */
    u8  mTarget;                   /* 0x5551, index of the target card */
    u8  mFlashMode;                /* 0x5552, picks the flash handler */
    u8  mFlashStep;                /* 0x5553 */
    u8  mFlashBeats;               /* 0x5554 */
    u8  mDone;                     /* 0x5555 */
    u8  pad_5556[0x2];             /* 0x5556 */

    /* Overrides of fBase_c's slots 0, 3, 6 and 9, defined out of line. The
       destructor stays the key function, so only its files emit the vtable. */
    s32 InitResources();      /* slot 0 */
    s32 CleanupResources();   /* slot 3 */
    s32 Behavior();           /* slot 6 */
    s32 Render();             /* slot 9 */

    /* The round states s3DEspRoundTable[mRoundState] dispatches. */
    void Wait();                  /* inter-round timer */
    void Play();                  /* dispatch the phase and the flash */
    void Results();               /* reveal, score and restart */

    /* The play phases s3DEspPhaseTable[mPhase] dispatches. */
    void Deal();                  /* difficulty, intro anim, deal the cards */
    void Fade();                  /* blend in */
    void Await();                 /* run the cards until a tap lands */
    void Resolve();               /* run cards and rows while scoring */

    /* The flash handlers s3DEspFlashTable[mFlashMode] picks. */
    void UpdateFlash();
    void FlashOnce();
    void FlashSync();
    void FlashQuick();

    /* The five cards and their per-card states. */
    void UpdateCards();
    void CardSeek(int i);         /* fall toward the target zone and land */
    void CardTouch(int i);        /* stylus hit test */
    void CardExit(int i);         /* waits for mFade to reach 0x10, then sets state 1 */
    void DealCards();
    void ClearCards();
    void RenderCards();
    void SpawnRows(int i);        /* copy card i's landing spot to the emitters */

    /* The three emitters and their per-row states. */
    void UpdateRows();
    void RowWait(int i);          /* countdown, then charge */
    void RowDecay(int i);         /* shrink, then fade */
    void RowFade(int i);          /* blend out and die */
    void RenderRows();

    /* The twenty slots and their per-slot states and type handlers. */
    void UpdateSlots();
    void SlotWait(int i);         /* countdown, then launch */
    void SlotFall(int i);         /* fall, run the type handler, land */
    void SlotSetup(int i);        /* type 0: re-roll speeds and timers */
    void SlotRise(int i);         /* type 1: float up and settle */
    void SlotKick(int i);         /* type 2: gravity with random jolts */
    void SlotDampen(int i);       /* type 3: ease velocity to zero */
    void ResetSlots();
    void RenderSlots();

    void SetDifficulty();         /* read the score, pick the round's pace */
    void ResetRound();            /* clear cards, timers and flags */
    void DrawBanner();            /* the end-of-round marker */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMg3DEsp_c_size_must_be_0x5558[sizeof(dScMg3DEsp_c) == 0x5558 ? 1 : -1];
typedef char dScMg3DEsp_c_Card_size_must_be_0x18[sizeof(dScMg3DEsp_c::Card) == 0x18 ? 1 : -1];
typedef char dScMg3DEsp_c_Row_size_must_be_0x14[sizeof(dScMg3DEsp_c::Row) == 0x14 ? 1 : -1];
typedef char dScMg3DEsp_c_Slot_size_must_be_0x20[sizeof(dScMg3DEsp_c::Slot) == 0x20 ? 1 : -1];
#endif

#endif
