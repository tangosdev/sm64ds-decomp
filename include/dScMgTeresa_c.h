#ifndef DSCMGTERESA_C_H
#define DSCMGTERESA_C_H
#include "dScMgBase_c.h"

/* dScMgTeresa_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (StartRound),
   20 (OnNewRecord) and 34 (PaintSquare). These overrides are declared members.
   Slot 20 is declared here because dScMgBase_c
   declares it, so leaving it out here would put the BASE's body in this
   class's slot 20 where the cartridge holds its own.  The body is a bare
   FreeGfxSlotsById(8) thunk that never reads `this`, which costs nothing --
   `this` arrives in r0 and is immediately overwritten with 8 either way, so
   the bytes are the same whether it is spelled as a member or not.
   "OnNewRecord" is a coined name; see include/dScMgBase_c.h.  Slot 34 IS that multi-argument virtual, and it is
   declared and reconstructed tree-wide now -- `PaintSquare(int, int, int, int)`,
   the family's pixel brush.  The guess recorded here, "draws a dMeter_c
   digit/glyph", was right: this class's override stamps a shape by testing
   sTeresaPaintMask[row] one bit per column, so it paints a stipple pattern
   rather than a solid square, and it really does barely touch `this` -- the
   object pointer arrives and is never read, because unlike the base it always
   draws into sub BG0 instead of consulting the layer index at +0x6c.  Fields below
   dScMgBase_c's own 0x4660 are INHERITED, not this class's own -- accessed
   via raw offsets on a char* cast of `this` (0xb4 touched here, already
   dScMgBase_c's own).
 *
 * SM64DS RTTI names the implementation dScMgTeresa_c. The reconstructed factory
 * dScMgTeresa_c_classInit (historical alias MgHideAndBooSeek_Spawn) installs this class's
 * cartridge vtable for the MG_TERESA registry profile.
 */
struct dScMgTeresa_c : dScMgBase_c {
    /* One of the 16 small Boos (0x24 bytes; the big Boo is a separate
       record at 0x4bcc). Meanings come from DrawBoos,
       SpawnBoos, AnimateBoo and the Boo_State handlers; unk_ bytes are
       written but not yet understood. */
    struct SmallBoo {
        s32 mX;          /* 0x00 -- 20.12 screen x */
        s32 mY;          /* 0x04 -- 20.12 screen y */
        s32 mSpeed;      /* 0x08 -- 0xc00, adjusted by mKind */
        u16 mHeading;    /* 0x0c -- angle; the sine table's sign picks mAnim */
        u16 mTimer;      /* 0x0e */
        u16 mAnimTimer;  /* 0x10 -- counts to 0xf, then advances mFrame */
        u16 unk_12;      /* 0x12 */
        u16 mDelay;      /* 0x14 */
        u8  mFade;       /* 0x16 -- nonzero draws semi-transparent */
        u8  mInPlay;     /* 0x17 */
        u8  mState;      /* 0x18 */
        u8  unk_19;      /* 0x19 */
        u8  mVisible;    /* 0x1a */
        u8  mPriority;   /* 0x1b -- OAM priority */
        u8  mFound;      /* 0x1c -- set when a touch test finds this Boo */
        u8  mFrame;      /* 0x1d */
        u8  mAnim;       /* 0x1e -- facing: 1 when the heading's sine is >= 0 */
        u8  mKind;       /* 0x1f -- per-level table value; 8 is the slow kind */
        u8  mFoundOrder; /* 0x20 -- the found counter (+0x4c21) when found */
        u8 mPlaySettleSound; /* 0x21 -- the one Boo of a settling group that plays sound 0x1f1 */
        u8  pad_22[0x2];
    };

    /* The "!" over a Boo the touch test just found (Exclaim_Spawn). */
    struct Exclaim {
        s32 x;           /* 0x00 -- 20.12 */
        s32 y;           /* 0x04 -- 20.12; spawned 0x18 px above the Boo */
        u8  timer;       /* 0x08 -- counts down from 0x60 */
        u8  active;      /* 0x09 */
        u8  visible;     /* 0x0a */
        u8  pad0b;
    };

    /* The five-frame puff of smoke where a caught Boo vanishes (seen in
       play; VanishPuff_Spawn). */
    struct VanishPuff {
        s32 x;           /* 0x00 -- 20.12 */
        s32 y;           /* 0x04 -- 20.12 */
        u16 timer;       /* 0x08 -- the frame steps every 4 */
        u16 pad0a;
        u8 frame;        /* 0x0c -- 0..4, then the puff ends */
        u8  active;      /* 0x0d */
        u8  visible;     /* 0x0e */
        u8  pad0f;
    };

    /* The digit that rises off a found Boo: 1 for the first found, 2 for the
       second (seen in play; FoundNumber_Spawn). */
    struct FoundNumber {
        s32 x;           /* 0x00 -- 20.12 */
        s32 y;           /* 0x04 -- 20.12 */
        s32 vy;          /* 0x08 */
        s32 timer;       /* 0x0c -- seeded from the Boo's mFoundOrder */
        u8  active;      /* 0x10 */
        u8  visible;     /* 0x11 */
        u8 sprite;       /* 0x12 -- the Boo's mFoundOrder; draws digit sprite + 1 */
        u8  state;       /* 0x13 */
    };

    /* The reveal sprite (Reveal_Draw, Reveal_StateStart..4). Its handlers take
       an index and dispatch on `state` through sTeresaRevealStateTable. */
    struct Reveal {
        s32 x;           /* 0x00 -- 20.12 */
        s32 y;           /* 0x04 -- 20.12 */
        s32 vx;          /* 0x08 -- 20.12; Reveal_StateSlideIn slides x by it */
        s32 vy;          /* 0x0c -- also moves mMarker */
        u16 angle;       /* 0x10 -- OAM::Render rotation */
        u16 frameTimer;  /* 0x12 -- the frame steps every 8 */
        u16 timer;       /* 0x14 */
        u16 pad16;
        u8  sprite;      /* 0x18 */
        u8  visible;     /* 0x19 */
        u8  state;       /* 0x1a */
        u8  unk_1b;      /* 0x1b */
        u8  active;      /* 0x1c -- dispatch runs only while set */
        u8  frame;       /* 0x1d */
        u8 phase;        /* 0x1e -- Reveal_StateDipAndRise: 0 rising, 1 falling */
        u8  pad1f;
    };

    /* The marker drawn with the reveal (Marker_Draw/Show/Hide). */
    struct Marker {
        s32 x;           /* 0x00 -- 20.12 */
        s32 y;           /* 0x04 -- 20.12 */
        u8  active;      /* 0x08 */
        u8  visible;     /* 0x09 */
        u8  pad0a[0x2];
    };

    virtual ~dScMgTeresa_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void StartRound(int arg);               /* slot 18 */
    virtual void OnNewRecord();                          /* slot 20 */
    virtual void PaintSquare(int cx, int cy, int colour, int size); /* slot 34 */

    /* scene helpers, in ROM order */
    void BigBoo_ShowResult();
    void BigBoo_SetDone();
    void BigBoo_Draw();
    void BigBoo_StateResult(int idx);
    void BigBoo_StateGone();
    void BigBoo_StateRemove(int index);
    void BigBoo_StateExit(int i);
    void BigBoo_StateOffscreen(int idx);
    void BigBoo_StateFlyRight(int i);
    void BigBoo_StateFlyLeft(int i);
    void BigBoo_StateHover(int i);
    void BigBoo_StateDropIn(int i);
    void BigBoo_Update();
    void BigBoo_Start();
    void BigBoo_Hide();
    void StepBg3Scroll();
    void Marker_Draw();
    void Marker_Update();
    void Marker_Show();
    void Marker_Hide();
    void Reveal_Draw();
    void Reveal_StateFinish(int idx);
    void Reveal_StateDipAndRise(int i);
    void Reveal_StateAnimate(int i);
    void Reveal_StateSlideIn(int i);
    void Reveal_StateStart(int i);
    void Reveal_Update();
    void Reveal_Start();
    void Reveal_Hide();
    void FoundNumber_DrawAll();
    void FoundNumber_ClearAll();
    int FoundNumber_CountBusy();
    void FoundNumber_UpdateAll();
    void FoundNumber_Spawn(int i);
    void FoundNumber_SpawnForFound();
    void VanishPuff_DrawAll();
    void VanishPuff_UpdateAll();
    void VanishPuff_Spawn(int param);
    void DrawCountdown();
    void UpdateCountdown();
    void StartCountdown();
    void Exclaim_DrawAll();
    void Exclaim_UpdateAll();
    void Exclaim_Spawn(int idx);
    void CheckRevealDone();
    void CheckRoundEnd();
    void DrawBoos();
    void AreBoosSettled();
    void TouchTestBoo(int idx);
    void CountRubbedTiles(int idx);
    void MoveBoo(int i);
    void Boo_StateFadeAway(int i);
    void Boo_StateFadeIn(int i);
    void Boo_StateFound(int idx);
    void Boo_StateHiddenTimed(int idx);
    void Boo_StateHidden(int i);
    void Boo_StateFadeOut(int i);
    void BounceBooOffWalls(int i);
    void StepBoo(int i);
    void StartBoosHiding();
    void Boo_StateFly(int i);
    void Boo_StateAppear(int idx);
    void AnimateBoo(int i);
    void UpdateBoos();
    void SpawnBoos();
    void UpdateRubbing();
    void ReadRubTouch();
    void ResetRound();
    void Scene_StateResult();
    void Scene_StateRoundEnd();
    void Scene_StatePlay();
    void Scene_StateReveal();
    void Scene_StateInit();

    SmallBoo   mBoos[16];        /* 0x4660 */
    Exclaim    mExclaims[16];    /* 0x48a0 */
    VanishPuff mVanishPuffs[16]; /* 0x4960 */
    FoundNumber mFoundNumbers[16]; /* 0x4a60 */
    /* A one-element array: the Reveal_State handlers take an index and
       step it by 0x20, and the dispatcher passes 0. [inference from codegen] */
    Reveal     mReveal[1];       /* 0x4ba0 */
    Marker     mMarker;          /* 0x4bc0 */
    /* The big Boo, 0x1c bytes; d_s_mg_teresa.cpp's `struct Boo` (BOO()) is
       its typed view. */
    u8  pad_4bcc[0x1c];
    s32 mSceneState; /* 0x4be8 -- state index for Behavior's pmf dispatch */
    u8  pad_4bec[0x2a];
    s16 unk_4c16;            /* 0x4c16 */
    u8  pad_4c18[0x9];
    u8  mFoundCount;         /* 0x4c21 -- Boos found this round; stamped into mFoundOrder */
    u8  mLastSettledBoo;     /* 0x4c22 -- the Boo AreBoosSettled settled last */
    u8 mBooPattern;          /* 0x4c23 -- spawn pattern: mLevel, or 10..13 from level 15 */
    /* trailing extent the ROM's `new dScMgTeresa_c` literal proves; see tools/opnew_sizes.py */
    u8 mBg3FlashStep;        /* 0x4c24 -- StepBg3Scroll's flash step, 0..8 */
    u8  mBooCount;           /* 0x4c25 -- Boos SpawnBoos placed */
    u8  mFirstFoundBoo;      /* 0x4c26 -- 0xff until the first find */
    u8 mRubLocked;           /* 0x4c27 -- set at time up; UpdateRubbing stops */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgTeresa_c_Reveal_size_must_be_0x20[sizeof(dScMgTeresa_c::Reveal) == 0x20 ? 1 : -1];
typedef char dScMgTeresa_c_Marker_size_must_be_0xc[sizeof(dScMgTeresa_c::Marker) == 0xc ? 1 : -1];
typedef char dScMgTeresa_c_Exclaim_size_must_be_0xc[sizeof(dScMgTeresa_c::Exclaim) == 0xc ? 1 : -1];
typedef char
    dScMgTeresa_c_VanishPuff_size_must_be_0x10[sizeof(dScMgTeresa_c::VanishPuff) == 0x10 ? 1 : -1];
typedef char dScMgTeresa_c_FoundNumber_size_must_be_0x14[sizeof(dScMgTeresa_c::FoundNumber) == 0x14
                                                             ? 1
                                                             : -1];
typedef char dScMgTeresa_c_SmallBoo_size_must_be_0x24[sizeof(dScMgTeresa_c::SmallBoo) == 0x24 ? 1 : -1];
typedef char dScMgTeresa_c_size_must_be_0x4c28[sizeof(struct dScMgTeresa_c) == 0x4c28 ? 1 : -1];
#endif

#endif
