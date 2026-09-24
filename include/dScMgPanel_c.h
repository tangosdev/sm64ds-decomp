#ifndef DSCMGPANEL_C_H
#define DSCMGPANEL_C_H
#include "dScMgBase_c.h"

/* Puzzle Panic, MG_PANEL in the profile registry. A square grid of panels,
 * each showing one of two faces. The deal flips mFlips of them; touch a
 * panel to flip it and its neighbours back. mLives starts at 3.
 *
 * A confirmed leaf: no RTTI record in the cartridge names it as a base
 * (tools/rtti_extract.py). Six vtable slots are its own: 0, 6, 9, the 16/17
 * destructor pair, and 18, an override of dScMgBase_c's.
 *
 * The field names are readings of what the code does, not recovered
 * identifiers; the offsets, widths and array counts are ROM facts.
 *
 * The factory dScMgPanel_c_classInit is a reconstruction (historical alias
 * MgPuzzlePanelPuzzlePanic_Spawn); it installs this vtable for MG_PANEL.
 * Only the class name is a cartridge string.
 */
struct dScMgPanel_c : dScMgBase_c {
    virtual ~dScMgPanel_c();
    virtual s32 InitResources();         /* slot 0 */
    virtual s32 Behavior();              /* slot 6 */
    virtual s32 Render();                /* slot 9 */
    virtual void OnYoshiTryEat(int arg); /* slot 18 */

    /* Intro card slides in on mSlide, then holds. Scroll and the 64
       particles live in the pad above mState; the cpp names them. */
    s32 mSlide;              /* 0x4660 Fix12 */
    s32 mSlideY;             /* 0x4664 */
    s32 mSlideVel;           /* 0x4668 */
    u8  pad_466c[0x4];
    u16 mHold;               /* 0x4670 */
    u8  pad_4672[0x2];
    u8  mSlideOn;            /* 0x4674 */
    u8  mSlideStep;          /* 0x4675 */
    u8  mSlideFlag;          /* 0x4676 */
    u8  mSlideLeft;          /* 0x4677 */
    u8  pad_4678[0x630];
    s32 mState;              /* 0x4ca8 state-table index */
    s32 mIntro;              /* 0x4cac intro slides finished */
    s32 mDealt;              /* 0x4cb0 0 until the first board is chosen */
    s32 mFaceSet;            /* 0x4cb4 which face table, kept off the last one */
    s32 mCount;              /* 0x4cb8 panel count, mWidth * mWidth */
    s32 mWidth;              /* 0x4cbc grid width */
    s32 mFlips;              /* 0x4cc0 how many panels the deal flips */
    s32 mX[0x24];            /* 0x4cc4 panel x, Fix12 */
    s32 mY[0x24];            /* 0x4d54 panel y, Fix12 */
    u8  pad_4de4[0xe0];      /* 0x4de4 flash timers and the last five deals */
    s16 mDelay;              /* 0x4ec4 countdown; helpers also read it as u16 */
    u8  pad_4ec6[0x34];
    u8  mKind[0x24];         /* 0x4efa per-panel behavior index */
    u8  mFace[0x24];         /* 0x4f1e current face, flipped by the deal and by touches */
    u8  mGoal[0x24];         /* 0x4f42 solved pattern; set by the deal, compared
                                against mFace, drawn as the overlay sprite */
    u8  mVisible[0x24];      /* 0x4f66 drawn only while non-zero (02105ab4);
                                02106048 sets every entry at intro end */
    u8  pad_4f8a[0x24];
    u8  mFlipAt[0x30];       /* 0x4fae panels the deal flipped */
    u8  mFlipCount;          /* 0x4fde */
    u8  mClear;              /* 0x4fdf set once every mKind is back to 0 */
    u8  pad_4fe0[0x2];
    u8  mLives;              /* 0x4fe2 starts at 3; a miss takes one */
    u8  pad_4fe3[0x6];
    u8  mBusy;               /* 0x4fe9 a flip is still playing */
    u8  unk_4fea;            /* 0x4fea only ever written 0 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPanel_c_size_must_be_0x4fec[sizeof(dScMgPanel_c) == 0x4fec ? 1 : -1];
#endif

#endif
