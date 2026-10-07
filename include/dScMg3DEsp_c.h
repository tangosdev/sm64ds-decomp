/* dScMg3DEsp_c, scene 0x185 (profile MG_3DESP). The ROM's RTTI names the
 * class and its one base, dScMgSingle3DBase_c. The factory
 * dScMg3DEsp_c_classInit, historically MgPsycheOut_Spawn, allocates 0x5558
 * bytes.
 *
 * The factory and the destructor fix the members after the base:
 *   - two Models at 0x4f38 and 0x4f88, 0x50 bytes each
 *   - a dMg3DEspModel_c at 0x4fd8, 0x21c bytes; unk_51e4 is its unk_20c
 *   - a TextureTransformer at 0x51f4, 0x14 bytes
 *   - this class's own fields from 0x5208 to 0x5558
 * The factory constructs the four in declaration order and the destructor
 * tears them down in reverse, so they could become typed members. They stay
 * raw storage until the factory and the destructor change together.
 *
 * The destructor is the key function, defined out of line in
 * src/actors/dMg3DEspAnimSet_c.cpp, which emits both D1 and D0.
 * dScMgBase_c's operator delete serves D0.
 */
#ifndef DSCMG3DESP_C_H
#define DSCMG3DESP_C_H
#include "dScMgSingle3DBase_c.h"

struct dScMg3DEsp_c : dScMgSingle3DBase_c {
    virtual ~dScMg3DEsp_c();
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */

    u8  mModel1[0x50];             /* 0x4f38, a Model */
    u8  mModel2[0x50];             /* 0x4f88, a Model */
    u8  pad_4fd8[0x20c];           /* 0x4fd8, a dMg3DEspModel_c up to 0x51f4 */
    s32 unk_51e4;                  /* 0x51e4 */
    u8  pad_51e8[0xc];             /* 0x51e8 */
    u8  mTextureTransformer[0x14]; /* 0x51f4, a TextureTransformer */
    u8  mCards[0x78];              /* 0x5208, five 0x18-byte cards */
    u8  mRows[0x3c];               /* 0x5280, three 0x14-byte emitters */
    u8  mSlots[0x280];             /* 0x52bc, twenty 0x20-byte slots */
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

    /* The round states data_ov006_02141f2c[mRoundState] dispatches. */
    void Wait();                  /* inter-round timer */
    void Play();                  /* dispatch the phase and the flash */
    void Results();               /* reveal, score and restart */

    /* The play phases data_ov006_02141fac[mPhase] dispatches. */
    void Deal();                  /* difficulty, intro anim, deal the cards */
    void Fade();                  /* blend in */
    void Await();                 /* run the cards until a tap lands */
    void Resolve();               /* run cards and rows while scoring */

    /* The flash handlers data_ov006_02141f44[mFlashMode] picks. */
    void UpdateFlash();
    void FlashOnce();
    void FlashSync();
    void FlashQuick();

    /* The five cards and their per-card states. */
    void UpdateCards();
    void CardSeek(int i);         /* fall toward the target zone and land */
    void CardTouch(int i);        /* stylus hit test */
    void CardExit(int i);         /* clear once the fade ramps */
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
#endif

#endif
