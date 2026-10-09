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
/* The banner that drops in at the start of a round (func_ov006_02104a84
   and its state table data_ov006_021427ec). */
struct dScMgPanel_Banner {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 */
    u16 timer;        /* +0x08 */
    u16 hold;         /* +0x0a */
    u8  active;       /* +0x0c */
    u8  visible;      /* +0x0d */
    u8  state;        /* +0x0e */
    u8  pad_0f;
};

/* Sub-screen BG0 shake after a miss (func_ov006_02104920/02104a10);
   state indexes data_ov006_021427bc. */
struct dScMgPanel_Shake {
    s32 unk_00;
    s32 offset;       /* +0x04 -- SetSubBg0Offset y */
    u16 timer;        /* +0x08 */
    u8  active;       /* +0x0a */
    u8  state;        /* +0x0b */
};

/* BG2 offset on both screens. func_ov006_02104580 runs it: phase 0 shakes
   x by +-2.0 for 0x3c frames and drops particles (func_ov006_0210446c) at
   both side edges, then y accelerates up to 512.0. func_ov006_02104870 arms
   it; func_ov006_021048b0 clears `on` and the offsets. */
struct dScMgPanel_Scroll {
    s32 x;            /* +0x00 */
    s32 y;            /* +0x04 */
    s32 z;            /* +0x08 */
    s32 vel;          /* +0x0c */
    u8  on;           /* +0x10 */
    u8  phase;        /* +0x11 */
    u8  timer;        /* +0x12 */
    u8  pad_13;
};

/* One of the 64 debris particles (func_ov006_02104354 steps them). */
struct dScMgPanel_Particle {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 */
    s32 vx;           /* +0x08 */
    s32 vy;           /* +0x0c */
    u8  active;       /* +0x10 */
    u8  stuck;        /* +0x11 -- set: no gravity */
    u8  kind;         /* +0x12 -- sprite, data_ov006_0213def0 */
    u8  frame;        /* +0x13 -- five frames, then the slot frees */
    u8  tick;         /* +0x14 */
    u8  visible;      /* +0x15 */
    u8  drift;        /* +0x16 -- set by the spawn: fixed or random */
    u8  pad_17;
};

#ifndef SM64DS_PLATFORM_PC
typedef char dScMgPanel_Banner_size_must_be_0x10[sizeof(dScMgPanel_Banner) == 0x10 ? 1 : -1];
typedef char dScMgPanel_Shake_size_must_be_0xc[sizeof(dScMgPanel_Shake) == 0xc ? 1 : -1];
typedef char dScMgPanel_Scroll_size_must_be_0x14[sizeof(dScMgPanel_Scroll) == 0x14 ? 1 : -1];
typedef char dScMgPanel_Particle_size_must_be_0x18[sizeof(dScMgPanel_Particle) == 0x18 ? 1 : -1];
#endif

struct dScMgPanel_c : dScMgBase_c {
    virtual ~dScMgPanel_c();
    virtual s32 InitResources();         /* slot 0 */
    virtual s32 Behavior();              /* slot 6 */
    virtual s32 Render();                /* slot 9 */
    virtual void OnYoshiTryEat(int arg); /* slot 18 */

    /* Intro card slides in on mSlide, then holds. */
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
    dScMgPanel_Banner   mBanner;            /* 0x4678 */
    dScMgPanel_Shake    mShake[1];          /* 0x4688 */
    dScMgPanel_Scroll   mScroll;            /* 0x4694 */
    dScMgPanel_Particle mParticles[0x40];   /* 0x46a8 */
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

    /* Helpers the state tables and the vtable bodies call. The address is the
       method name; no ROM spelling survives. */
    void func_ov006_021042e8();
    void func_ov006_02104354();
    void func_ov006_0210446c(int x, int y, int mode);
    void func_ov006_02104558();
    void func_ov006_02104580();
    void func_ov006_02104870();
    void func_ov006_021048b0();
    void func_ov006_021048e4();
    void func_ov006_02104920(int index);
    void func_ov006_02104a10(int index);
    void func_ov006_02104ac4();
    void func_ov006_02104b24();
    void func_ov006_02104b4c();
    void func_ov006_02104b5c();
    void func_ov006_02104bb0();
    void func_ov006_02104c08();
    void func_ov006_02104c60();
    void func_ov006_02104cfc(int i);
    void func_ov006_02104d44();
    void func_ov006_02104d94();
    void func_ov006_02104e70();
    void func_ov006_02104e80();
    void func_ov006_02104ea8();
    void func_ov006_02104eb8();
    void func_ov006_02104ecc();
    void func_ov006_02104fb4();
    void func_ov006_0210500c();
    void func_ov006_0210508c();
    void func_ov006_021050bc();
    void func_ov006_02105118();
    void func_ov006_02105134();
    void func_ov006_021051dc();
    void func_ov006_021053a8();
    void func_ov006_02105670();
    void func_ov006_02105730();
    void func_ov006_021057f0();
    void func_ov006_02105854();
    void func_ov006_02105ab4();
    void func_ov006_02105c1c();
    void func_ov006_02105c88();
    void func_ov006_02105d20();
    void func_ov006_02105de4();
    void func_ov006_02106048();
    void func_ov006_02106080(int index);
    void func_ov006_02106168();
    void func_ov006_021063a0();
    int func_ov006_02106664();
    void func_ov006_02106758();
    void func_ov006_021067a4();
    void func_ov006_021068d8();
    void func_ov006_02106910(int index);
    void func_ov006_02106a08(int idx);
    void func_ov006_02106aa8(int index);
    void func_ov006_02106bac(int index);
    void func_ov006_02106bc0();
    void func_ov006_02106ca4();
    void func_ov006_02106eb8();
    void func_ov006_02106f44();
    void func_ov006_02106fdc();
    void func_ov006_0210709c();
    void func_ov006_0210713c();
    void func_ov006_021071d4();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPanel_c_size_must_be_0x4fec[sizeof(dScMgPanel_c) == 0x4fec ? 1 : -1];
#endif

#endif
