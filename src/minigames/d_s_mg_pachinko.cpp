//cpp
/* Bob-omb Squad. The player aims a Bob-omb with the stylus and fires it
 * up the board; a shot that reaches a target knocks the balls loose.
 * 76 functions (.text 0x020fa75c..0x020ff420), the class's whole linker
 * unit, closing with its factory dScMgPachinko_c_classInit. Below it is the
 * pair-match card game (d_s_mg_m_carlo2.cpp); above it is the next class's
 * destructor.
 *
 * _ZTS15dScMgPachinko_c 0x0213d960. _ZTI15dScMgPachinko_c 0x0213d948.
 * _ZTV15dScMgPachinko_c 0x0213d9cc. The destructor is the key function:
 * D1 at 0x020fa75c, D0 at 0x020fa780, no D2.
 *
 * The 30 functions from 0x020fc8c0 up were folded in from one-function
 * sources. The ball drawer at 0x020fc8c0 was the last to match: its
 * products go through FX_Mul, under opt_prelinearize off. 0x020fcb4c,
 * 0x020fdaf0 and 0x020fe394 depend on spellings that look interchangeable
 * (declaration order, which of two locals is read first); each one is
 * noted at its function.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder.
 */

// leftovers:
// - The func_ and data_ helpers are unnamed in symbols.txt.
// - The effects (mFx, 0x5958), the flyer (mFlyer, 0x5bb0), the targets
//   (mTarget, 0x5bcc) and the background scroller (mBg, 0x5bfc) are typed
//   in the header. PachinkoFlyer_Draw, PachinkoFx_DrawAll, PachinkoBg_Stop,
//   PachinkoTarget_ShowAll/HideAll/DrawAll and the target count read them by
//   field (PachinkoFx_Spawn indexes mFx through the strided `PACHINKO(fx)->mFx[0]`
//   form; `mFx[i]` changes code). mFlyer and mBg are
//   one-element arrays because their handlers take an index; PachinkoBg_StateScroll
//   and PachinkoFlyer_CheckHits' head use them. Bg_Start and Target_Choose
//   read the fields through a `self` local: spelled on the cast `PACHINKO(raw)`
//   directly, each field folds to its own base and the shared raw + 0x5000
//   add is lost. Bg_StateRiseIn/SinkOut and Flyer_Cross/Pause/Leave mostly keep
//   REC_OFF forms; the other handlers
//   keep the offset forms their launder macros and saved row cursors tune,
//   and convert one function at a time. mShot holds 0x30 records,
//   ending exactly at mFx. The pipe fields behind the PIPE_* byte-offset
//   macros take a pre-scaled offset rather than an index.
// - Pachinko_SpawnWave, the Ball Fall/Pop/Dive/SpinAway states and
//   PachinkoBobOmb_StateFallAway index mBall[i]/mShot[i]. In StateAim,
//   StateDrift and the other BALL_OFF sites indexing is one word different, so
//   `p + i * 0x38` stays; the r5 cursor locals in the two table walkers are
//   named for the register they color.
// - Where a typed member moves code, the whole-scene offset is spelled
//   SHOT_OFF/BALL_OFF/REC_OFF(field): the same constant, checked against the
//   header. Split bases (`+ 0x4000 + 0x68f`) keep their literal halves.

#pragma defer_codegen off

#include "types.h"
#include "nitro/fx/fx.h"
#include "dScMgPachinko_c.h"

namespace GX {
void LoadBGPltt(const void *p, u32 a, u32 b);
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

namespace GXS {
void LoadBGPltt(const void *p, u32 a, u32 b);
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

/* Two helpers dispatch through the ov006 pointer-to-member tables, each on
   its own view of the scene. Both pointer-to-member types are formed while
   the view is still incomplete: the compiler picks the pointer-to-member
   layout from that, and completing the class first changes the code. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
/* C is the scene from mBg[0].active (0x5c0e) and mBg[0].state (0x5c0f). */
struct C {
    char pad[0x5c0e];
    unsigned char active;
    unsigned char state;
};

struct C2;
typedef void (C2::*PMF2)(int);
/* C2 is the scene from mFlyer[0].active (0x5bc6) to mFlyer[0].state (0x5bc8). */
struct C2 {
    char pad[0x5bc6];
    unsigned char active;
    char visible;
    unsigned char state;
};

/* PachinkoFlyer_StateDescend's typed view of mFlyer from its y (0x5bb4).
   `volatile` on y is needed: the ROM reloads it after the store. */
typedef struct Ent {
    volatile int y; /* 0x00 mFlyer[i].y (20.12) */
    int vx;         /* 0x04 mFlyer[i].vx */
    int vy;         /* 0x08 mFlyer[i].vy (20.12) */
    char _padC[0x10];
} Ent;

typedef struct Obj {
    char _pad0[0x5bb4]; /* 0x0000 */
    Ent entries[8];     /* 0x5bb4 mFlyer[0].y */
} Obj;

/* The {code pointer, adjustment} records PachinkoPipe_UpdateAll,
   PachinkoBall_UpdateAll and PachinkoBobOmb_UpdateAll walk. */
struct Ent7d0 { int a; int b; };

/* The sprite record Mg_RenderSpriteBothScaled draws; only handled by pointer here. */
struct OamAttr;

/* The 2x2 rotate-and-scale matrix PachinkoBall_DrawAll hands the sprite
   drawer. */
struct Matrix2x2 { int _00, _01, _10, _11; };

/* Launder macros: they make the compiler materialise a base address on its
   own instead of folding it into a neighbouring add, as the ROM does. */
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
/* PachinkoBall_StateDrift's two spellings of the same address: one through a
   signed and one through an unsigned integer. */
#define ATI(p, off) ((char *)(int)((char *)(p) + (off)))
#define ATU(p, off) ((char *)(unsigned int)((char *)(p) + (off)))
#define M1(a) (a)
#define M2(a) (a)

/* Pipe row accessors for PachinkoPipe_StateSink: mPipe[0]'s field, then the
   pre-scaled index b. */
#define PIPE_PHASE(c, b)  (*(u8*)((c) + REC_OFF(mPipe, phase) + (b)))
#define PIPE_TIMER(c, b)  (*(u16*)((c) + REC_OFF(mPipe, timer) + (b)))
#define PIPE_FRAME(c, b)  (*(u8*)((c) + REC_OFF(mPipe, frame) + (b)))
#define PIPE_SPRITE(c, b) (*(u8*)((c) + REC_OFF(mPipe, sprite) + (b)))
#define PIPE_STATE(c, b)  (*(u8*)((c) + REC_OFF(mPipe, state) + (b)))
#define PIPE_X(c, b)      (*(s32*)((c) + REC_OFF(mPipe, x) + (b)))
#define PIPE_VEL(c, b)    (*(s32*)((c) + REC_OFF(mPipe, vel) + (b)))

/* The scene seen through a char pointer, for the free helpers below. */
#define PACHINKO(p) ((dScMgPachinko_c *)(p))
/* One record, already stepped by i * 0x38; indexing the array or saving an
 * element pointer is one word different. */
#define SHOT_BASE ((int)&((dScMgPachinko_c *)0)->mShot)
#define BALL_BASE ((int)&((dScMgPachinko_c *)0)->mBall)
#define SHOT(p) ((dScMgPachinko_shot *)((p) + SHOT_BASE))
#define BALL(p) ((dScMgPachinko_ball *)((p) + BALL_BASE))
#define PIPE(p) ((dScMgPachinko_pipe *)((p) + (int)&((dScMgPachinko_c *)0)->mPipe))
/* Pachinko_DrawAimLine adds the field first, then the index. `&SHOT(p)->f`
 * shares one base and does not match. */
#define SHOT_OFF(member) (SHOT_BASE + (int)&((dScMgPachinko_shot *)0)->member)
#define BALL_OFF(member) (BALL_BASE + (int)&((dScMgPachinko_ball *)0)->member)
/* The same for the other arrays: a field of element 0, as a byte offset into
 * the scene, for the handlers that add their pre-scaled index after it. */
#define REC_OFF(arr, member) ((int)&((dScMgPachinko_c *)0)->arr[0].member)
/* A scalar scene field as a byte offset, for a read off the char pointer. */
#define SCENE_OFF(member) ((int)&((dScMgPachinko_c *)0)->member)

namespace G2 {
    void *GetBG2CharPtr();
    void *GetBG2ScrPtr();
}

namespace G2S {
    unsigned GetBG2CharPtr();
    unsigned GetBG2ScrPtr();
    unsigned GetBG3CharPtr();
}

namespace cstd {
s16 atan2(s32, s32);
int sqrt(unsigned long long val);
}

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

extern "C" {

/* --- shared ov004, main and library helpers --- */
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern void Mg_DrawBigNumber(int x, int y, int h, int a, int b, int c, int d);
extern int  Mg_GetSubmittedScore(void);
extern int  Mg_DrawTopRightCounter(int a, int b);
extern int  Mg_GetTopRecord(void);
extern int  Mg_DrawTopLeftCounter(int score);
extern void Mg_SubmitScore(int self);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void SetBg2Offset(int a, int b);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
struct OamAttr;
struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};
extern void Mg_RenderSpriteBothPalMtx(void *sprite, int x, int y, int a3, void *m);
extern int  Mg_RenderSpriteBothScaled(OamAttr *a, int b, int c, int d, int e, int f, int g);
extern void Mg_SetSharedState(int a);
extern void Mg_SetTopScreenYOffset(int a);
extern void Mg_InitBrush(char *a, char b, int c, int d);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern void func_02012718(int a, int b);
extern int  Sound_PlayIfNotActive(int handle, int player, int id, int unused);
extern s16 atan2_hold_d_s_mg_pachinko_1(int y, int x);
extern void FreeGfxSlotsById(int arg);
extern int  LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *p);
extern int  func_02054d88(void);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void func_020564f4(const void *src, int offset, int count);

/* --- ROM data this TU reads --- */
extern int   data_0209d4b8;
extern unsigned char data_0209d45c;
extern u8    data_0209d454;
extern s16   data_02082214[];
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8    gActivePlayerSlot;
// local extern: see above.
extern u8    gTouchHeld[][4];
// local extern: see above.
extern u8    gTouchEdge[][4];
// local extern: see above.
extern u8    gTouchX[][4];
// local extern: see above.
extern u8    gTouchY[][4];
extern void *sPachinkoDecoSprite;
extern unsigned char sPachinkoPipeRiseStartFrames[];
extern unsigned char sPachinkoPipeSinkStartFrames[];
extern unsigned char sPachinkoPipeRiseFrames[];
extern unsigned char sPachinkoPipeSinkFrames[];
extern int   data_ov006_0212eb24[];
extern int sPachinkoPipeKinds[];
extern u8 sPachinkoPipeIdleFrames[];
extern u8 sPachinkoPipeIdleTimes[];
extern s32   data_ov006_0212eb44[];
extern int sPachinkoPipeRiseStops[];
extern s32 sPachinkoPipeSinkStops[];
extern int sPachinkoTargetX[];
extern int sPachinkoFxSpritesA[];
extern int sPachinkoFxSpritesB[];
extern int sPachinkoPipeSprites[];
extern int sPachinkoTargetSprites[];
extern int sPachinkoFlyerSprites[];
extern int sPachinkoWaveColumnX[];
extern int sPachinkoPopSfx[];
extern int sPachinkoBallSprites[];
extern int sPachinkoBobOmbSprites;
extern u16 sPachinkoPopScores[];
extern u8 sPachinkoWaveSlots[];

/* The two pointer-to-member tables the helpers dispatch through, and the
   record tables PachinkoPipe_UpdateAll, PachinkoBobOmb_UpdateAll and
   PachinkoBall_UpdateAll walk. Static initialisers fill them from {code
   pointer, adjustment} records in ov006 .data. */
extern Entry sPachinkoBgStateTable[];
extern Ent7d0 sPachinkoPipeStateTable[];
extern Ent7d0 sPachinkoBobOmbStateTable[];
extern PMF2 sPachinkoFlyerStateTable[];
extern Ent7d0 sPachinkoBallStateTable[];

/* The ROM {code pointer, adjustment} records the five tables copy from
   (unlicensed .data), grouped by destination table in retail copy order. */
extern Ent7d0 pmf_PachinkoBobOmb_StateWaitGrab;
extern Ent7d0 pmf_PachinkoBobOmb_StateAim;
extern Ent7d0 pmf_PachinkoBobOmb_StateFlying;
extern Ent7d0 pmf_PachinkoBobOmb_StateFallAway;
extern Ent7d0 pmf_PachinkoBobOmb_StateRemove;
extern Ent7d0 pmf_PachinkoBall_StateWaiting;
extern Ent7d0 pmf_PachinkoBall_StateFall;
extern Ent7d0 pmf_PachinkoBall_StateDrift;
extern Ent7d0 pmf_PachinkoBall_StateDive;
extern Ent7d0 pmf_PachinkoBall_StateGlide;
extern Ent7d0 pmf_PachinkoBall_StateSpinAway;
extern Ent7d0 pmf_PachinkoBall_StatePop;
extern Ent7d0 pmf_PachinkoPipe_StatePick;
extern Ent7d0 pmf_PachinkoPipe_StateRise;
extern Ent7d0 pmf_PachinkoPipe_StateSink;
extern Ent7d0 pmf_PachinkoPipe_StateIdle;
extern PMF2 pmf_PachinkoFlyer_StateWaiting;
extern PMF2 pmf_PachinkoFlyer_StateDescend;
extern PMF2 pmf_PachinkoFlyer_StateCross;
extern PMF2 pmf_PachinkoFlyer_StatePause;
extern PMF2 pmf_PachinkoFlyer_StateLeave;
extern PMF pmf_PachinkoBg_StateWaiting;
extern PMF pmf_PachinkoBg_StateRiseIn;
extern PMF pmf_PachinkoBg_StateScroll;
extern PMF pmf_PachinkoBg_StateSinkOut;

/* This TU's own members, forward-declared for the callers below them. */
extern void PachinkoDeco_DrawAll(char *thiz);
extern void PachinkoDeco_UpdateAll(char *self);
extern void PachinkoDeco_ShowAll(char *p);
extern void PachinkoDeco_HideAll(char *p);
extern void PachinkoBg_StateSinkOut(char *p, int i);
extern void PachinkoBg_StateScroll(char *c, int i);
extern void PachinkoBg_StateRiseIn(char *c, int i);
extern void PachinkoBg_StateWaiting(char *c, int idx);
extern void PachinkoBg_Update(C *c);
extern void PachinkoBg_Start(char *c);
extern void PachinkoBg_Stop(char *p);
extern void PachinkoTarget_DrawAll(char *base);
extern void PachinkoTarget_ShowAll(u8 *c);
extern void PachinkoTarget_HideAll(char *p);
extern void PachinkoFlyer_Draw(char *c);
extern void PachinkoFlyer_CheckHits(char *c, int idx);
extern void PachinkoFlyer_StateLeave(char *c, int i);
extern void PachinkoFlyer_StatePause(char *c, int i);
extern void PachinkoFlyer_StateCross(char *p, int i);
extern void PachinkoFlyer_StateDescend(Obj *self, int i);
extern void PachinkoFlyer_StateWaiting(char *c, int idx);
extern void PachinkoFlyer_Update(C2 *c);
extern void PachinkoTarget_Choose(char *obj);
extern void PachinkoFx_DrawAll(void *base);
extern void PachinkoFx_UpdateAll(char *thiz);
extern void PachinkoFx_Spawn(char *c, int a2, int a3, int a4, int a5, int a6);
extern void Pachinko_CheckTargetsLeft(char *c);
extern void Pachinko_DrawScore(void *);
extern void Pachinko_DrawTopRecord(void *scene);
extern void PachinkoPopup_DrawAll(char *base);
extern void PachinkoPopup_UpdateAll(char *c);
extern void PachinkoPopup_SpawnScore(char *c, int idx, unsigned short val);
extern void Pachinko_ClearAimLine(char *c);
extern void Pachinko_PlotSubBg2Pixels(void *a0, int a1, int a2, int a3);
extern void Pachinko_DrawAimLine(void *arg0);
extern void PachinkoPipe_DrawAll(char *base);
extern void PachinkoPipe_SetAll(char *base, int val);
extern int PachinkoPipe_StateIdle(char *self, int idx);
extern void PachinkoPipe_StateSink(char *c, int i);
extern void PachinkoPipe_StateRise(char *c, int i);
extern void PachinkoPipe_StatePick(char *thiz, int n);
extern void PachinkoPipe_UpdateAll(char *c);
extern void PachinkoPipe_ResetAll(unsigned char *c);
extern void PachinkoBall_DrawAll(char *scene);
extern void PachinkoBall_CountNearby(char *base, int i);
extern void PachinkoBall_CheckPipes(char *c, int idx);
extern void PachinkoBall_AimAtTarget(char *base, int i);
extern void PachinkoBall_CheckFall(char *thiz, int idx);
extern void PachinkoBall_StatePop(char *c, int i);
extern void PachinkoBall_StateSpinAway(char *c, int i);
extern void PachinkoBall_StateGlide(char *self, int idx);
extern void PachinkoBall_StateDive(char *c, int i);
extern void PachinkoBall_StateDrift(char *o, int i);
extern void PachinkoBall_StateFall(char *o, int i);
extern void PachinkoBall_StateWaiting(char *base, int idx);
extern void PachinkoBall_UpdateAll(char *c);
extern void PachinkoBall_CheckHitByBobOmb(char *base, int i);
extern void Pachinko_SpawnWave(dScMgPachinko_c *self);
extern void PachinkoBobOmb_ClearAll(char *p);
extern void PachinkoBobOmb_DrawAll(char *thiz);
extern void PachinkoBobOmb_UpdateAll(char *c);
extern void Pachinko_CountLaunch(char *c);
extern void PachinkoBobOmb_StateWaitGrab(char *self, int i);
extern void PachinkoBobOmb_StateAim(dScMgPachinko_c *self, int i);
extern void PachinkoBobOmb_StateFlying(char *c, int idx);
extern void PachinkoBobOmb_StateFallAway(char *base, int i);
extern void PachinkoBobOmb_StateRemove(char *p, int idx);
extern void Pachinko_UpdateNextBall(char *o);
extern void Pachinko_ResetRound(char *self);

}  /* extern "C" */

// @symbol _ZN15dScMgPachinko_cD1Ev
// @symbol _ZN15dScMgPachinko_cD0Ev
/* Both D1 and D0 come from this one definition. */
dScMgPachinko_c::~dScMgPachinko_c()
{
}

extern "C" {  /* .c-derived members: C linkage for everything below */

// @symbol PachinkoDeco_DrawAll
void PachinkoDeco_DrawAll(char* row)
{
    for (int i = 0; i < 3; i++) {
        if (PACHINKO(row)->mRow[0].visible != 0) {
            OAM::Render(false, (OamAttr *)sPachinkoDecoSprite, PACHINKO(row)->mRow[0].x >> 12,
                        PACHINKO(row)->mRow[0].y >> 12, -1, -1, 0x1000, 0x1000,
                        (unsigned short)PACHINKO(row)->mRow[0].angle, -1);
        }
        row += 0x18;
    }
}

// @symbol PachinkoDeco_UpdateAll
void PachinkoDeco_UpdateAll(char *raw)
{
    int i;
    char *row;

    row = raw;
    for (i = 0; i < 3; i++) {
        *(s32 *)(row + REC_OFF(mRow, x)) =
            (data_ov006_0212eb44[i] << 12) - *(s32 *)(raw + REC_OFF(mBg, x));
        *(s32 *)(row + REC_OFF(mRow, y)) = -*(s32 *)(raw + REC_OFF(mBg, y));
        if (*(u8 *)(row + REC_OFF(mRow, dir)) != 0) {
            *(u16 *)AT(row, REC_OFF(mRow, angle)) -= 0x40;
            if (*(s16 *)(row + REC_OFF(mRow, angle)) <= -0x1000) {
                *(u16 *)(row + REC_OFF(mRow, angle)) = 0xf000;
                *(u8 *)(row + REC_OFF(mRow, dir)) = 0;
            }
        } else {
            *(u16 *)AT(row, REC_OFF(mRow, angle)) += 0x40;
            if (*(s16 *)(row + REC_OFF(mRow, angle)) >= 0x1000) {
                *(s16 *)(row + REC_OFF(mRow, angle)) = 0x1000;
                *(u8 *)AT(row, REC_OFF(mRow, dir)) += 1;
            }
        }
        row += 0x18;
    }
}

// @symbol PachinkoDeco_ShowAll
void PachinkoDeco_ShowAll(char *row) {
    int i;
    char *raw = row;
    for (i = 0; i < 3; i++, row += 0x18) {
        if (*(unsigned char *)(row + REC_OFF(mRow, active)) == 0) {
            *(unsigned char *)(row + REC_OFF(mRow, active)) = 1;
            PACHINKO(row)->mRow[0].visible = 1;
            PACHINKO(row)->mRow[0].x =
                (data_ov006_0212eb44[i] << 12) - *(int *)(raw + REC_OFF(mBg, x));
            PACHINKO(row)->mRow[0].y = -*(int *)(raw + REC_OFF(mBg, y));
            *(short *)(row + 0x4e6a) = 0; /* mRow[0].unk12: nothing reads it */
            PACHINKO(row)->mRow[0].angle = 0;
            PACHINKO(row)->mRow[0].dir = 0;
        }
    }
}

// @symbol PachinkoDeco_HideAll
void PachinkoDeco_HideAll(char *row)
{
    int i;
    for (i = 0; i < 3; i++) {
        PACHINKO(row)->mRow[0].active = 0;
        PACHINKO(row)->mRow[0].visible = 0;
        row += 0x18;
    }
}

// @symbol PachinkoBg_StateSinkOut
void PachinkoBg_StateSinkOut(char *raw, int i) {
    int off = i * 0x14;
    int y;
    *(int *)(raw + REC_OFF(mBg, x) + off) += *(int *)(raw + REC_OFF(mBg, vx) + off);
    *(int *)(raw + REC_OFF(mBg, y) + off) += *(int *)(raw + REC_OFF(mBg, vy) + off);
    if (*(int *)(raw + REC_OFF(mBg, vx) + off) <= -0x1000)
        *(int *)(raw + REC_OFF(mBg, vx) + off) += 0x40;
    if (*(int *)(raw + REC_OFF(mBg, vy) + off) >= 0x3000)
        *(int *)(raw + REC_OFF(mBg, vy) + off) += 0x100;
    y = *(int *)(raw + REC_OFF(mBg, y) + off) >> 12;
    SetBg2Offset(*(int *)(raw + REC_OFF(mBg, x) + off) >> 12, y);
    if (y < 0x40) return;
    *(short *)(raw + REC_OFF(mBg, timer) + off) = 0x258;
    *(unsigned char *)(raw + REC_OFF(mBg, state) + off) = 0;
    data_0209d45c &= ~4;
    PachinkoDeco_HideAll(raw);
}

// @symbol PachinkoBg_StateScroll
void PachinkoBg_StateScroll(char *raw, int i) {
    int* vel = &PACHINKO(raw)->mBg[i].vx;
    int* pos = &PACHINKO(raw)->mBg[i].x;
    int x;

    *pos = *pos + *vel;

    if (*vel <= -0x2000) {
        *vel = *vel - 0x100;
    }

    x = *pos >> 12;
    SetBg2Offset(x, PACHINKO(raw)->mBg[i].y >> 12);

    if (x > -0x90) return;

    PACHINKO(raw)->mBg[i].vy = 0x800;
    PACHINKO(raw)->mBg[i].state = 3;
}

// @symbol PachinkoBg_StateRiseIn
void PachinkoBg_StateRiseIn(char *raw, int i) {
    int off = i * 0x14;
    int* velX;
    int* velY;
    int* posX;
    int* posY;
    int y;
    int x;

    *(int *)(raw + REC_OFF(mBg, x) + off) += *(int *)(raw + REC_OFF(mBg, vx) + off);
    *(int *)(raw + REC_OFF(mBg, y) + off) += *(int *)(raw + REC_OFF(mBg, vy) + off);
    velX = (int *)(raw + REC_OFF(mBg, vx) + off);
    posX = (int *)(raw + REC_OFF(mBg, x) + off);
    posY = (int *)(raw + REC_OFF(mBg, y) + off);
    velY = (int *)(raw + REC_OFF(mBg, vy) + off);
    if (*velX <= -0x2000) *velX -= 0x100;
    if (*velY <= -0xc00) *velY += 0x40;
    y = *posY >> 12;
    x = *posX >> 12;
    if (y <= 0) {
        y = 0;
        *posY = 0;
        *velY = 0;
        *(unsigned char *)(raw + off + REC_OFF(mBg, state)) = 2;
    }
    SetBg2Offset(x, y);
}

// @symbol PachinkoBg_StateWaiting
void PachinkoBg_StateWaiting(char *raw, int idx) {
    int off;
    unsigned short delay;
    char *p_row;
    int neg;
    int b;
    int a;
    int lo;
    int hi;
    unsigned char flag;

    if (PACHINKO(raw)->mPhase == 2)
        return;

    off = idx * 0x14;
    delay = *(unsigned short *)((raw + REC_OFF(mBg, timer)) + off);
    if (delay != 0) {
        *(unsigned short *)((raw + REC_OFF(mBg, timer)) + off) = delay - 1;
        return;
    }

    flag = data_0209d45c;
    p_row = raw + off;
    neg = 0x1000;
    data_0209d45c = flag | 4;
    *(int *)((raw + REC_OFF(mBg, x)) + off) = 0x100000;
    *(int *)((raw + REC_OFF(mBg, y)) + off) = 0x40000;
    neg = -neg;
    /* Mix (raw+off) and p_row so mwccarm colors r4 and r5 like the ROM. */
    PACHINKO(raw + off)->mBg[0].vx = neg;
    PACHINKO(p_row)->mBg[0].vy = neg;
    a = *(int *)((raw + REC_OFF(mBg, y)) + off);
    b = *(int *)((raw + REC_OFF(mBg, x)) + off);
    lo = b >> 12;
    hi = a >> 12;
    PACHINKO(raw + off)->mBg[0].state = 1;
    *(volatile int *)0x4000018 = (0x1ff & lo) | (0x1ff0000 & (hi << 16));
    PachinkoDeco_ShowAll(raw);
}

// @symbol PachinkoBg_Update
/* Dispatches through the ov006 pointer-to-member table at 0x02142604. */
void PachinkoBg_Update(C* c) {
    if (c->active) {
        (c->*(sPachinkoBgStateTable[c->state].pmf))(0);
    }
    PachinkoDeco_UpdateAll((char *)c);
}

// @symbol PachinkoBg_Start
void PachinkoBg_Start(char *raw){
    dScMgPachinko_c *self = PACHINKO(raw);
    self->mBg[0].active = 1;
    self->mBg[0].x = 0x100000;
    self->mBg[0].y = 0x40000;
    self->mBg[0].vx = 0;
    self->mBg[0].vy = 0;
    self->mBg[0].state = 0;
    self->mBg[0].timer = 0;
    *(volatile int *)0x4000018 =
        (0x1ff & (self->mBg[0].x >> 12)) | (0x1ff0000 & ((self->mBg[0].y >> 12) << 16));
}

// @symbol PachinkoBg_Stop
void PachinkoBg_Stop(char *raw)
{
    data_0209d45c &= ~4;
    PACHINKO(raw)->mBg[0].active = 0;
}

// @symbol PachinkoTarget_DrawAll
void PachinkoTarget_DrawAll(char *raw)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (PACHINKO(raw)->mTarget[i].visible != 0) {
            Mg_RenderSpriteBoth((void *)sPachinkoTargetSprites[PACHINKO(raw)->mTarget[i].sprite],
                                PACHINKO(raw)->mTarget[i].x >> 0xc,
                                PACHINKO(raw)->mTarget[i].y >> 0xc, -1, 1);
        }
    }
}

// @symbol PachinkoTarget_ShowAll
void PachinkoTarget_ShowAll(u8 *raw)
{
    int i;
    for (i = 0; i < 4; i++) {
        PACHINKO(raw)->mTarget[i].active = 1;
        PACHINKO(raw)->mTarget[i].visible = 1;
        PACHINKO(raw)->mTarget[i].x = sPachinkoTargetX[i] << 12;
        PACHINKO(raw)->mTarget[i].y = 0xa0000;
        PACHINKO(raw)->mTarget[i].sprite = 0;
    }
}

// @symbol PachinkoTarget_HideAll
void PachinkoTarget_HideAll(char *raw)
{
    int i;
    for (i = 0; i < 4; i++) {
        PACHINKO(raw)->mTarget[i].active = 0;
        PACHINKO(raw)->mTarget[i].visible = 0;
    }
}

// @symbol PachinkoFlyer_Draw
void PachinkoFlyer_Draw(char* raw){
  if (PACHINKO(raw)->mFlyer[0].visible == 0) return;
  int x = PACHINKO(raw)->mFlyer[0].x;
  int y = PACHINKO(raw)->mFlyer[0].y;
  Mg_RenderSpriteBoth((void *)sPachinkoFlyerSprites[PACHINKO(raw)->mFlyer[0].facing], x >> 0xc,
                      y >> 0xc, -1, -1);
}

#pragma push
#pragma opt_common_subs off
// @symbol PachinkoFlyer_CheckHits
void PachinkoFlyer_CheckHits(char *raw, int idx)
{
    int i;
    int j;
    char *shot;
    char *ball;
    int dx;
    int dy;
    u8 state;
    int n;

    if (PACHINKO(raw)->mFlyer[idx].state == 0)
        return;

    shot = raw;
    for (i = 0; i < 0x30; i++, shot += 0x38) {
        if (SHOT(shot)->active == 0)
            continue;
        if (SHOT(shot)->state < 2)
            continue;
        dx = (PACHINKO(raw)->mFlyer[idx].x - SHOT(shot)->x) >> 12;
        dy = (PACHINKO(raw)->mFlyer[idx].y - SHOT(shot)->y) >> 12;
        if (cstd::sqrt(dx * dx + dy * dy) > 0x18)
            continue;

        /* The tail re-derives the flyer through n: spelling these as
           mFlyer[idx] changes the code. */
        n = idx * 0x1c;
        PACHINKO(raw)->mHitPause = 0x20;
        SHOT(raw + i * 0x38)->active = 0;
        SHOT(raw + i * 0x38)->visible = 0;
        PachinkoFx_Spawn(raw, PACHINKO(raw + n)->mFlyer[0].x, PACHINKO(raw + n)->mFlyer[0].y, 2, 0,
                         0);
        ball = raw;
        for (j = 0; j < 0x1e; j++, ball += 0x38) {
            if (BALL(ball)->active == 0)
                continue;
            state = BALL(ball)->state;
            if (state == 0)
                continue;
            if (state < 5) {
                BALL(ball)->state = 6;
                BALL(ball)->timer = 0x10;
            }
        }
        PACHINKO(raw + n)->mFlyer[0].visible = 0;
        PACHINKO(raw + n)->mFlyer[0].active = 0;
        PACHINKO(raw)->mFlyerOut = 0;
        Sound::PlayBank2_2D(0x18a);
        return;
    }
}
#pragma pop

// @symbol PachinkoFlyer_StateLeave
void PachinkoFlyer_StateLeave(char *raw, int i) {
    int n = i * 0x1c;
    int *pos = (int *)(raw + REC_OFF(mFlyer, x));
    int *sound = (int *)(raw + REC_OFF(mFlyer, sound));
    *(int *)((char *)sound + n) = func_02012468(*(int *)((char *)sound + n), 2, 0x189, 4, 0, 0,
                                                func_020126e8(*(int *)((char *)pos + n)), 0);
    *(int *)((char *)pos + n) = *(int *)((char *)pos + n) + PACHINKO(raw + n)->mFlyer[0].vx;
    {
        int v = *(int *)((char *)pos + n) >> 12;
        if (v >= 0x110)
            goto clear;
        if (v > -0x10)
            return;
    }
clear:
    PACHINKO(raw + n)->mFlyer[0].active = 0;
    PACHINKO(raw + n)->mFlyer[0].visible = 0;
    PACHINKO(raw)->mFlyerOut = 0;
}

// @symbol PachinkoFlyer_StatePause
void PachinkoFlyer_StatePause(char *raw, int i) {
    short *delay = (short *)(raw + REC_OFF(mFlyer, delay) + i * 0x1c);
    if(*(unsigned short*)delay != 0){
        *delay = *(unsigned short*)delay - 1;
        if(*delay < 0) *delay = 0;
        return;
    }
    PACHINKO(raw + i * 0x1c)->mFlyer[0].state = 2;
    *(unsigned char *)(raw + REC_OFF(mFlyer, passes) + i * 0x1c) =
        *(unsigned char *)(raw + REC_OFF(mFlyer, passes) + i * 0x1c) - 1;
}

// @symbol PachinkoFlyer_StateCross
void PachinkoFlyer_StateCross(char *raw, int i) {
    int n = i * 0x1c;
    int x;
    u8 flag;
    *((int *)((raw + REC_OFF(mFlyer, sound)) + n)) =
        func_02012468(*((int *)((raw + REC_OFF(mFlyer, sound)) + n)), 2, 0x189, 4, 0, 0,
                      func_020126e8(*((int *)((raw + REC_OFF(mFlyer, x)) + n))), 0);
    *((int *)((raw + REC_OFF(mFlyer, x)) + n)) += *((int *)((raw + REC_OFF(mFlyer, vx)) + n));
    flag = *((u8 *)((raw + REC_OFF(mFlyer, facing)) + n));
    x = (*((int *)((raw + REC_OFF(mFlyer, x)) + n))) >> 12;
    if (flag != 0) {
        if (x >= 0xc0)
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) -= 0x80;
        else if ((*((int *)((raw + REC_OFF(mFlyer, vx)) + n))) <= 0x3000)
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) += 0x200;
        if (x >= 0xf0) {
            u32 a;
            if ((*((u8 *)((raw + REC_OFF(mFlyer, passes)) + n))) == 0) {
                *((u8 *)((raw + REC_OFF(mFlyer, state)) + n)) = 4;
                return;
            }
            *((int *)((raw + REC_OFF(mFlyer, x)) + n)) = 0xf0000;
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) = 0;
            *((u8 *)((raw + REC_OFF(mFlyer, facing)) + n)) ^= 1;
            a = (((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
            *((u16 *)((raw + REC_OFF(mFlyer, delay)) + n)) = (u16)((((a << 3) >> 15) * 2) + 0x10);
            *((u8 *)((raw + REC_OFF(mFlyer, state)) + n)) = 3;
            return;
        }
    } else {
        if (x <= 0x40)
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) += 0x80;
        else if ((*((int *)((raw + REC_OFF(mFlyer, vx)) + n))) >= (-0x3000))
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) -= 0x200;
        if (x <= 0x10) {
            u32 a;
            if ((*((u8 *)((raw + REC_OFF(mFlyer, passes)) + n))) == 0) {
                *((u8 *)((raw + REC_OFF(mFlyer, state)) + n)) = 4;
                return;
            }
            *((int *)((raw + REC_OFF(mFlyer, x)) + n)) = 0x10000;
            *((int *)((raw + REC_OFF(mFlyer, vx)) + n)) = 0;
            *((u8 *)((raw + REC_OFF(mFlyer, facing)) + n)) ^= 1;
            a = (((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
            *((u16 *)((raw + REC_OFF(mFlyer, delay)) + n)) = (u16)((((a << 3) >> 15) * 2) + 0x10);
            *((u8 *)((raw + REC_OFF(mFlyer, state)) + n)) = 3;
            return;
        }
    }
}

// @symbol PachinkoFlyer_StateDescend
/* Moves flyer i down: y += vy. While y is still below -0xc0 it slows
   (vy -= 0x60); once it reaches -0xc0 it snaps to -0xc0000 and stops.
   Then hands on to PachinkoFlyer_StateCross. */
void PachinkoFlyer_StateDescend(Obj *self, int i) {
    self->entries[i].y += self->entries[i].vy;
    if (self->entries[i].y >> 12 >= -0xc0) {
        self->entries[i].y = -0xc0000;
        self->entries[i].vy = 0;
    } else {
        self->entries[i].vy -= 0x60;
    }
    PachinkoFlyer_StateCross((char *)self, i);
}

#pragma push
#pragma opt_common_subs off
// @symbol PachinkoFlyer_StateWaiting
void PachinkoFlyer_StateWaiting(char *raw, int idx) {
    int off;
    short* delay;
    unsigned int side;
    off = idx * 0x1c;
    delay = (short *)&PACHINKO(raw)->mFlyer[idx].delay;
    if (*(unsigned short*)delay != 0) {
        *delay = *(unsigned short*)delay - 1;
        if (*delay < 0) *delay = 0;
        return;
    }
    Sound::PlayBank2_2D(0x188);
    PACHINKO(raw + off)->mFlyer[0].state = 1;
    side = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1;
    side = side >> 0xf;
    PACHINKO(raw + off)->mFlyer[0].facing = side;
    if (side != 0) {
        PACHINKO(raw + off)->mFlyer[0].visible = 1;
        PACHINKO(raw + off)->mFlyer[0].x = -0x10000;
        PACHINKO(raw + off)->mFlyer[0].vx = 0x400;
    } else {
        PACHINKO(raw + off)->mFlyer[0].visible = 1;
        PACHINKO(raw + off)->mFlyer[0].x = 0x110000;
        PACHINKO(raw + off)->mFlyer[0].vx = -0x400;
    }
    PACHINKO(raw + off)->mFlyer[0].y = -0xf8000;
    PACHINKO(raw + off)->mFlyer[0].passes =
        ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 0xf) + 2;
    PACHINKO(raw + off)->mFlyer[0].vy = 0x2000;
}
#pragma pop

// @symbol PachinkoFlyer_Update
/* Dispatches through the ov006 pointer-to-member table at 0x0214266c. */
void PachinkoFlyer_Update(C2 *c){
    if (c->active == 0)
        return;
    (c->*sPachinkoFlyerStateTable[c->state])(0);
    PachinkoFlyer_CheckHits((char *)c, 0);
}

// @symbol PachinkoTarget_Choose
void PachinkoTarget_Choose(char *raw) {
    int targets;
    int i;
    unsigned int rand;
    char *target;
    dScMgPachinko_c *self = PACHINKO(raw);
    if (self->mFlyerOut != 0) {
        return;
    }
    self->mFlyer[0].active = 1;
    self->mFlyer[0].state = 0;
    self->mFlyer[0].sound = 0;
    rand = (((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
    self->mFlyer[0].delay = (short)((((rand << 5) >> 15) << 3) + 0x200);
    targets = 0;
    for (i = 0; i < 4; i++) {
        if (self->mTarget[i].active != 0) {
            targets++;
        }
    }

    if (targets == 1) {
        rand = (((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
        self->mFlyer[0].delay = (short)((((rand << 5) >> 15) << 2) + 0x200);
    }
    self->mFlyerOut++;
}

// @symbol PachinkoFx_DrawAll
void PachinkoFx_DrawAll(void* raw) {
    int i;
    for (i = 0; i < 30; i++) {
        if (PACHINKO(raw)->mFx[i].visible) {
            int type = PACHINKO(raw)->mFx[i].kind;
            int x = PACHINKO(raw)->mFx[i].x >> 12;
            int y = PACHINKO(raw)->mFx[i].y >> 12;
            int idx = PACHINKO(raw)->mFx[i].frame;
            if (type != 2) {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesA[idx], x, y, -1, -1);
            } else {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesB[idx], x, y, -1, -1);
            }
        }
    }
}

// @symbol PachinkoFx_UpdateAll
/* The distinct launder macros force independent RMW base materialization
   (add plus pool).  A plain *(fx + 0x5960) for the compare and zero reloads
   folds to add #0x5900 + #0x60 and loses the ROM's shape. */
void PachinkoFx_UpdateAll(char *raw)
{
    int i;
    char *fx = raw;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(fx + REC_OFF(mFx, active)) != 0) {
            u16 *timer = (u16 *)(int)M1((u32)fx + REC_OFF(mFx, timer));
            int lastFrame;
            u8 target;
            (*timer)++;
            if (*(u16 *)(fx + REC_OFF(mFx, timer)) >= 6) {
                *(u16 *)(fx + REC_OFF(mFx, timer)) = 0;
                (*(u8 *)(int)M2((u32)fx + REC_OFF(mFx, frame)))++;
                lastFrame = 3;
                if (*(u8 *)(fx + REC_OFF(mFx, kind)) == 2)
                    lastFrame = 5;
                target = *(u8 *)(fx + REC_OFF(mFx, target));
                if (target != 0) {
                    if (lastFrame - 2 == *(u8 *)(fx + REC_OFF(mFx, frame))) {
                        PACHINKO(raw)->mTarget[target - 1].active = 0;
                        PACHINKO(raw)->mTarget[target - 1].sprite = 1;
                    }
                }
                if (*(u8 *)(fx + REC_OFF(mFx, frame)) >= lastFrame) {
                    *(u8 *)(fx + REC_OFF(mFx, visible)) = 0;
                    *(u8 *)(fx + REC_OFF(mFx, active)) = 0;
                    if (*(u16 *)(fx + REC_OFF(mFx, points)) != 0) {
                        PachinkoPopup_SpawnScore(raw, i, *(u16 *)(fx + REC_OFF(mFx, points)));
                    }
                }
            }
        }
        fx = fx + 0x14;
    }
}

// @symbol PachinkoFx_Spawn
void PachinkoFx_Spawn(char *raw, int x, int y, int kind, int points, int target)
{
    int i = 0;
    char *scan = raw;
    do {
        if (PACHINKO(scan)->mFx[0].active == 0) {
            char *fx = raw + i * 0x14;
            PACHINKO(fx)->mFx[0].x = x;
            PACHINKO(fx)->mFx[0].y = y;
            PACHINKO(fx)->mFx[0].active = 1;
            PACHINKO(fx)->mFx[0].visible = 1;
            PACHINKO(fx)->mFx[0].timer = 0;
            PACHINKO(fx)->mFx[0].frame = 0;
            PACHINKO(fx)->mFx[0].kind = kind;
            PACHINKO(fx)->mFx[0].points = points;
            PACHINKO(fx)->mFx[0].target = target;
            return;
        }
        i++;
        scan += 0x14;
    } while (i < 0x1e);
}

// @symbol Pachinko_CheckTargetsLeft
void Pachinko_CheckTargetsLeft(char *raw) {
    volatile unsigned short zero;
    int count = 0;
    int i = 0;
    char *target = raw;
    do {
        if (PACHINKO(target)->mTarget[0].active != 0) {
            count++;
            break;
        }
        i++;
        target += 0xc;
    } while (i < 4);
    if (count != 0)
        return;
    PACHINKO(raw)->mPhase = 2;
    PACHINKO(raw)->mEndTimer = 0x40;
    {
        unsigned chars = G2S::GetBG2CharPtr();
        zero = 0;
        MultiStore16(zero, (char *)chars, 0x6000);
    }
    *(unsigned char *)(raw + 0x5c30) = 0;
    PachinkoBobOmb_ClearAll(raw);
    PachinkoPipe_SetAll(raw, 1);
}

// @symbol Pachinko_DrawScore
/* Render hands it the scene, which it does not read. */
void Pachinko_DrawScore(void *){
  Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
}

// @symbol Pachinko_DrawTopRecord
/* Draws the high score: the stored best comes back from Mg_GetTopRecord
   as a full word and goes straight to the HUD's number drawer. Render hands
   every drawer the scene; this one does not read it. */
void Pachinko_DrawTopRecord(void *)
{
    Mg_DrawTopLeftCounter(Mg_GetTopRecord());
}

// @symbol PachinkoPopup_DrawAll
void PachinkoPopup_DrawAll(char *raw)
{
    int i;
    char *popup = raw;
    for (i = 0; i < 0x1e; i++) {
        if (PACHINKO(popup)->mPopup[0].timer != 0) {
            Mg_DrawBigNumber(PACHINKO(popup)->mPopup[0].x >> 0xc,
                             PACHINKO(popup)->mPopup[0].y >> 0xc, PACHINKO(popup)->mPopup[0].points,
                             -1, -1, 0, 0);
        }
        popup += 0xc;
    }
}

// @symbol PachinkoPopup_UpdateAll
void PachinkoPopup_UpdateAll(char *popup) {
    int i;
    for (i = 0; i < 0x1e; i++) {
        unsigned short v = PACHINKO(popup)->mPopup[0].timer;
        if (v != 0) {
            unsigned short *timer =
                (unsigned short *)(((long long)((int)(popup + REC_OFF(mPopup, timer)))));
            *timer = (*timer) - 1;
            /* mPopup[0].timer, signed: the member spelling shares the laundered
               address above instead of a fresh + 0x4c00 base. */
            if ((*((short *)((popup + 0x4c00) + 0xfa))) < 0) {
                *((short *)((popup + 0x4c00) + 0xfa)) = 0;
            }
        }
        popup += 0xc;
    }
}

// @symbol PachinkoPopup_SpawnScore
void PachinkoPopup_SpawnScore(char *raw, int idx, unsigned short points)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    char *popup = raw + 0x1d * sizeof(dScMgPachinko_popup);
    for (i = 0x1d; i >= 0; popup -= 0xc, i--) {
        if (PACHINKO(popup)->mPopup[0].timer != 0)
            continue;
        self->mPopup[i].x = self->mFx[idx].x;
        self->mPopup[i].y = self->mFx[idx].y;
        self->mPopup[i].timer = 0x30;
        self->mPopup[i].points = points;
        Mg_SubmitScore(self->mPopup[i].points + Mg_GetSubmittedScore());
        if ((unsigned int)Mg_GetSubmittedScore() < 0xbb8) return;
        if (PACHINKO(raw)->mPromptEnabled != 0)
            PACHINKO(raw)->mPromptEnabled = 0;
        return;
    }
}

// @symbol Pachinko_ClearAimLine
void Pachinko_ClearAimLine(char* raw)
{
    volatile unsigned short zero;
    /* By offset: PACHINKO(raw)->mAimShot is 8 bytes longer. */
    int shotNo = *(unsigned char *)(raw + SCENE_OFF(mAimShot));
    int dx, dy;
    char* shot;

    if (shotNo == 0) return;

    shot = raw + (shotNo - 1) * 0x38;
    dx = 0x80 - (SHOT(shot)->x >> 12);
    dy = 0x20 - (SHOT(shot)->y >> 12);

    if (dx < -6) return;
    if (dx > 6) return;
    if (dy < -6) return;
    if (dy > 6) return;
    if (SHOT(shot)->state != 2) return;

    PACHINKO(raw)->mAimShot = 0;
    {
        char* dst = (char*)G2S::GetBG2CharPtr();
        zero = 0;
        MultiStore16(zero, dst, 0x6000);
    }
}

#pragma push
#pragma opt_loop_invariants off
// @symbol Pachinko_PlotSubBg2Pixels
/* Sets a 2x2 block of pixels in the sub BG2 character data. The first
   parameter is never read. */
void Pachinko_PlotSubBg2Pixels(void *a0, int a1, int px, int colour)
{
    int i, j, x, lo, hi, col, row, yb, py;
    u32 *bp;
    py = a1;
    for (i = 0; i < 2; i++)
    {
        x = px - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = py - 1;
            a1 = yb + j;
            bp = (u32 *)G2S::GetBG2CharPtr();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= colour << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

#pragma push
#pragma opt_common_subs off
#pragma opt_loop_invariants off
// @symbol Pachinko_DrawAimLine
void Pachinko_DrawAimLine(void *arg0)
{
    char *a = (char *)arg0;
    int i;
    int tx;
    int ty;
    int *sx;
    int *sy;
    int *pf00;
    int *pf18;
    int idx;
    int sX0;
    int sX1;
    int sY;
    volatile u16 fill;
    int off;
    int py;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    char *buf;
    int n;

    n = PACHINKO(a)->mAimShot;
    if (n == 0)
        return;
    idx = n - 1;
    off = idx * 0x38;
    pf18 = (int *)(a + SHOT_OFF(prevX) + off);
    pf00 = (int *)(a + SHOT_OFF(x) + off);
    if (*pf00 != *pf18 || SHOT(a + off)->y != SHOT(a + off)->prevY) {
        buf = (char *)G2S::GetBG2CharPtr();
        fill = 0;
        MultiStore16(fill, buf, 0x6000);
        sx = (int *)(a + off + SHOT_OFF(x));
        sy = (int *)(a + off + SHOT_OFF(y));
        sX0 = 0x6c;
        sX1 = 0x94;
        sY = 0x22;
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = sX0;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx -= 4;
            } else {
                cx = sX1;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                PACHINKO(a)->mLineErr = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        PACHINKO(a)->mLineErr += ady;
                        if (PACHINKO(a)->mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            PACHINKO(a)->mLineErr -= adx;
                        }
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        PACHINKO(a)->mLineErr += ady;
                        if (PACHINKO(a)->mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            PACHINKO(a)->mLineErr -= adx;
                        }
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                PACHINKO(a)->mLineErr = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        PACHINKO(a)->mLineErr += adx;
                        if (PACHINKO(a)->mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            PACHINKO(a)->mLineErr -= ady;
                        }
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        PACHINKO(a)->mLineErr += adx;
                        if (PACHINKO(a)->mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            PACHINKO(a)->mLineErr -= ady;
                        }
                        Pachinko_PlotSubBg2Pixels(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    *pf18 = *pf00;
    SHOT(a + idx * 0x38)->prevY = SHOT(a + idx * 0x38)->y;
}
#pragma pop

// @symbol PachinkoPipe_DrawAll
void PachinkoPipe_DrawAll(char *raw)
{
    int i;
    char *row = raw;
    for (i = 0; i < 2; i++) {
        if (PIPE(row)->visible != 0) {
            Mg_RenderSpriteBoth((void *)sPachinkoPipeSprites[PIPE(row)->sprite],
                                PIPE(row)->x >> 0xc, PIPE(row)->y >> 0xc, -1, 2);
        }
        row += 0x1c;
    }
}

// @symbol PachinkoPipe_SetAll
void PachinkoPipe_SetAll(char *row, int phase) {
    int i;
    for (i = 0; i < 2; i++) {
        if (PIPE(row)->active) {
            PIPE(row)->state = 3;
            PIPE(row)->laps = 0;
            PIPE(row)->phase = (unsigned char)phase;
        }
        row += 0x1c;
    }
}

// @symbol PachinkoPipe_StateIdle
int PachinkoPipe_StateIdle(char *raw, int idx) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    u16 *counter = &self->mPipe[idx].timer;
    u8 *frame = &self->mPipe[idx].frame;
    u16 c = *counter;
    *counter = c + 1;
    u8 oldFrame = *frame;
    if (*counter >= sPachinkoPipeIdleTimes[oldFrame]) {
        *counter = 0;
        *frame = *frame + 1;
        if (*frame >= 8) {
            *frame = 0;
            self->mPipe[idx].laps = self->mPipe[idx].laps + 1;
        }
        self->mPipe[idx].sprite = sPachinkoPipeIdleFrames[*frame];
    }
    {
        if (self->mPipe[idx].phase != 0) {
            self->mPipe[idx].laps = 0;
            return 0;
        }
    }
    {
        u8 c3 = self->mPipe[idx].laps;
        if (c3 >= 2) {
            c3 = 0;
            self->mPipe[idx].state = c3;
            self->mPipe[idx].laps = c3;
        }
        return c3;
    }
}

// @symbol PachinkoPipe_StateSink
void PachinkoPipe_StateSink(char *raw, int i) {
    int off = i * 0x1c;
    int x;

    if (PIPE_PHASE(raw, off) != 0) {
        PIPE_TIMER(raw, off)++;
        if (PIPE_TIMER(raw, off) < 4)
            return;
        PIPE_TIMER(raw, off) = 0;
        PIPE_FRAME(raw, off)++;
        if (PIPE_FRAME(raw, off) >= 3) {
            PIPE_FRAME(raw, off) = 0;
            PIPE_STATE(raw, off) = 1;
            PIPE_PHASE(raw, off) = 0;
            return;
        }
        PIPE_SPRITE(raw, off) = sPachinkoPipeSinkStartFrames[PIPE_FRAME(raw, off)];
        return;
    }

    PIPE_TIMER(raw, off)++;
    if (PIPE_TIMER(raw, off) >= 4) {
        PIPE_TIMER(raw, off) = 0;
        PIPE_FRAME(raw, off)++;
        if (PIPE_FRAME(raw, off) >= 6) {
            PIPE_FRAME(raw, off) = 0;
        }
        PIPE_SPRITE(raw, off) = sPachinkoPipeSinkFrames[PIPE_FRAME(raw, off)];
    }

    PIPE_X(raw, off) += PIPE_VEL(raw, off);
    x = PIPE_X(raw, off) >> 0xc;
    if (x >= sPachinkoPipeSinkStops[i + 2]) {
        if (PIPE_VEL(raw, off) >= 0x800)
            PIPE_VEL(raw, off) -= 0x80;
    } else {
        PIPE_VEL(raw, off) += 0x80;
    }
    if (x < sPachinkoPipeSinkStops[i])
        return;

    PIPE_X(raw, off) = sPachinkoPipeSinkStops[i] << 0xc;
    PIPE_VEL(raw, off) = -0x1000;
    PIPE_PHASE(raw, off)++;
    PIPE_TIMER(raw, off) = 0;
    PIPE_FRAME(raw, off) = 0;
    PIPE_SPRITE(raw, off) = sPachinkoPipeSinkStartFrames[0];
}

// @symbol PachinkoPipe_StateRise
void PachinkoPipe_StateRise(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int off = i * 0x1c;
    int x;

    if (self->mPipe[i].phase != 0) {
        (self->mPipe[i].timer)++;
        if (self->mPipe[i].timer < 4) {
            return;
        }
        self->mPipe[i].timer = 0;
        (self->mPipe[i].frame)++;
        if (self->mPipe[i].frame >= 3) {
            self->mPipe[i].frame = 0;
            self->mPipe[i].state = 2;
            self->mPipe[i].phase = 0;
            return;
        }
        self->mPipe[i].sprite = sPachinkoPipeRiseStartFrames[self->mPipe[i].frame];
        return;
    }

    (self->mPipe[i].timer)++;
    if (self->mPipe[i].timer >= 4) {
        self->mPipe[i].timer = 0;
        (self->mPipe[i].frame)++;
        if (self->mPipe[i].frame >= 6) {
            self->mPipe[i].frame = 0;
        }
        self->mPipe[i].sprite = sPachinkoPipeRiseFrames[self->mPipe[i].frame];
    }

    self->mPipe[i].x = self->mPipe[i].x + self->mPipe[i].vel;
    x = self->mPipe[i].x >> 0xc;
    if (x <= sPachinkoPipeRiseStops[i + 2]) {
        if (self->mPipe[i].vel <= -0x800) {
            self->mPipe[i].vel = self->mPipe[i].vel + 0x80;
        }
    } else {
        self->mPipe[i].vel = self->mPipe[i].vel - 0x80;
    }

    if (x > sPachinkoPipeRiseStops[i]) {
        return;
    }
    self->mPipe[i].x = sPachinkoPipeRiseStops[i] << 0xc;
    self->mPipe[i].vel = 0x1000;
    (self->mPipe[i].phase)++;
    self->mPipe[i].timer = 0;
    self->mPipe[i].frame = 0;
    self->mPipe[i].sprite = sPachinkoPipeRiseStartFrames[0];
}

#pragma push
#pragma opt_common_subs off
// @symbol PachinkoPipe_StatePick
/* Resets one row of the ov006 table (rows are 0x1c bytes apart) after
   rolling a random kind from sPachinkoPipeKinds.  The row base `raw + off`
   is spelled twice on purpose: the ROM computes it once for the phase
   store and again, right after the kind reload and before the predicated
   if-else, for the tail stores.  With common-subexpression elimination on,
   mwccarm folds the second into the first and sinks the add below the
   if-else, which recolours the whole tail. */
void PachinkoPipe_StatePick(char *raw, int n) {
    int rnd = RandomIntInternal(&data_0209d4b8);
    int off = n * 0x1c;
    unsigned int rand = ((unsigned)rnd >> 16) & 0x7fff;
    unsigned int idx = (rand << 1) >> 0xf;
    int kind = sPachinkoPipeKinds[idx];
    unsigned char *state = (unsigned char *)(raw + REC_OFF(mPipe, state));
    char *q;
    state[off] = (unsigned char)kind;
    PIPE(raw + off)->phase = 0;
    q = raw + off;
    if (state[off] == 2) {
        PIPE(q)->sinking = 1;
        PIPE(q)->vel = 0x1000;
    } else {
        PIPE(q)->sinking = 0;
        PIPE(q)->vel = -0x1000;
    }
    PIPE(raw + off)->timer = 0;
    PIPE(raw + off)->frame = 0;
    PIPE(raw + off)->visible = 1;
}
#pragma pop

// @symbol PachinkoPipe_UpdateAll
/* Walks the 8-byte {code pointer, adjustment} records at 0x02142624; an odd
   adjustment selects the virtual spelling through the receiver's own vtable. */
void PachinkoPipe_UpdateAll(char *raw) {
    int i = 0;
    char *row = raw;
    do {
        if (PIPE(row)->active != 0) {
            int idx = PIPE(row)->state;
            Ent7d0 *rec = &sPachinkoPipeStateTable[idx];
            int adj = rec->b;
            char *obj = raw + (adj >> 1);
            int fn;
            if (adj & 1) {
                fn = *(int *)(*(int *)obj + rec->a);
            } else {
                fn = rec->a;
            }
            ((void (*)(void *, int))fn)(obj, i);
        }
        i++;
        row += 0x1c;
    } while (i < 2);
}

// @symbol PachinkoPipe_ResetAll
void PachinkoPipe_ResetAll(unsigned char *row) {
    int i;
    for (i = 0; i < 2; i++, row += 0x1c) {
        PIPE(row)->active = 1;
        PIPE(row)->visible = 0;
        PIPE(row)->state = 0;
        PIPE(row)->phase = 0;
        PIPE(row)->sprite = 0;
        PIPE(row)->timer = 0;
        PIPE(row)->x = data_ov006_0212eb24[i] << 12;
        PIPE(row)->y = 0x68000;
        PIPE(row)->vel = 0;
        *(int *)(row + 0x4eac) = 0;
        PIPE(row)->sinking = 0;
        PIPE(row)->laps = 0;
    }
}

#pragma push
#pragma opt_prelinearize off
static inline int FX_Mul(int v1, int v2)
{
    s64 t = (s64)v1 * v2;
    return (int)((t + 0x800) >> 12);
}

// @symbol PachinkoBall_DrawAll
/* Draws the 30 balls. A ball with `visible` set is drawn with the sprite
 * `sprite` picks, at its position plus offX/offY, through a 2x2
 * rotate-and-scale matrix: `rotation` is the angle (>> 4 indexes the shared
 * sine/cosine table) and the 20.12 `scale` is the scale. The products go through FX_Mul; spelled out in the body
 * they put the scale in the first smull's Rm instead of the table value. With
 * opt_prelinearize on, every callee-saved register in the loop rotates by one
 * place. */
void PachinkoBall_DrawAll(char *scene)
{
    int i;
    char *ball = scene;
    for (i = 0; i < 30; i++, ball += 0x38) {
        if (BALL(ball)->visible != 0) {
            int idx = ((int)BALL(ball)->rotation >> 4) << 1;
            int scale = BALL(ball)->scale;
            int sprite = BALL(ball)->sprite;
            Matrix2x2 m;
            int cosv = FX_Mul(data_02082214[idx + 1], scale);
            int sinv = FX_Mul(data_02082214[idx], scale);
            int x = (BALL(ball)->x + BALL(ball)->offX) >> 12;
            int y = (BALL(ball)->y + BALL(ball)->offY) >> 12;
            m._00 = cosv;
            m._01 = sinv;
            m._10 = -sinv;
            m._11 = cosv;
            Mg_RenderSpriteBothPalMtx((void *)sPachinkoBallSprites[sprite], x, y, -1, &m);
        }
    }
}
#pragma pop

// @symbol PachinkoBall_CountNearby
void PachinkoBall_CountNearby(char *base, int i)
{
    int x, y, cnt;

    dScMgPachinko_c *self = PACHINKO(base);
    if (self->mBall[i].state == 5)
        return;
    x = self->mBall[i].x >> 12;
    y = self->mBall[i].y >> 12;
    cnt = 0;
    if (y >= 0xd0) cnt++;
    if (x >= 0x120 || x <= -0x20) cnt++;
    if (cnt != 0) {
        self->mBall[i].active = 0;
        self->mBall[i].visible = 0;
    }
}

#pragma push
#pragma opt_common_subs off
// @symbol PachinkoBall_CheckPipes
void PachinkoBall_CheckPipes(char *c, int idx)
{
    int i;
    char *p;
    int dx;
    int dy;
    int n;

    if (BALL(c + idx * 0x38)->state == 5)
        return;

    p = c;
    for (i = 0; i < 4; i++, p += 0xc) {
        if (PACHINKO(p)->mTarget[0].active == 0)
            continue;

        dx = (PACHINKO(p)->mTarget[0].x - *(int *)(c + idx * 0x38 + BALL_OFF(x))) >> 12;
        dy = (PACHINKO(p)->mTarget[0].y - *(int *)(c + idx * 0x38 + BALL_OFF(y))) >> 12;
        if (dx < -0x10) continue;
        if (dx > 0x18) continue;
        if (dy < -0x10) continue;
        if (dy > 0x18) continue;

        n = idx * 0x38;
        BALL(c + n)->active = 0;
        BALL(c + n)->visible = 0;
        PachinkoFx_Spawn(c, *(int *)(c + i * 0xc + REC_OFF(mTarget, x)),
                         *(int *)(c + i * 0xc + REC_OFF(mTarget, y)), 2, 0, i + 1);
        PachinkoPipe_SetAll(c, 0);
        func_02012718(0x18c, BALL(c + n)->x);
        return;
    }
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol PachinkoBall_AimAtTarget
/* Re-aims ball `i` at a randomly chosen active target. Bails out if no
 * mTarget slot is active, or if the ball has sunk below y=0. Takes the length
 * of the ball's current velocity vector, scales it to 9/10, picks a random
 * active mTarget, and rewrites the velocity as that speed pointed along
 * atan2 towards the chosen target, then fires a sound whose pan comes from
 * the ball's x.
 *
 * The two velocity-component base offsets must be introduced in the order
 * below: the constant pool emits 0x4668 before 0x466c, and the declaration
 * order is what fixes it. */
void PachinkoBall_AimAtTarget(char *base, int i) {
    int idx;
    int n;
    int factor;
    int dist;
    int *pY;
    int *pVx;
    int *pVy;
    char *p;
    char *angBase;
    s16 angle;
    int count = 0, j;
    char *q;
    for (j = 0, q = base; j < 4; j++, q += 0xc) {
        if (PACHINKO(q)->mTarget[0].active != 0) {
            count++;
            break;
        }
    }
    if (!count)
        return;
    n = i * 0x38;
    {
        char *yBase = base + BALL_OFF(y);
        int y = *(int *)(yBase + n);
        pY = (int *)(yBase + n);
        if ((y >> 12) < 0)
            return;
    }
    {
        char *velXBase = base + BALL_OFF(velX);
        char *velYBase = base + BALL_OFF(velY);
        int a = *(int *)(velXBase + n);
        s64 distSq = (s64)a * a;
        int b = *(int *)(velYBase + n);
        pVx = (int *)(velXBase + n);
        distSq += (s64)b * b;
        pVy = (int *)(velYBase + n);
        dist = (int)cstd::sqrt((u64)distSq);
    }
    factor = dist * 9 / 10;
    idx = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 4 >> 15;
    for (;;) {
        int t = idx * 0xc;
        p = base + t;
        if (PACHINKO(p)->mTarget[0].active != 0)
            break;
        idx = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 4 >> 15;
    }
    {
        char *xBase = base + BALL_OFF(x);
        int targetY = PACHINKO(p)->mTarget[0].y;
        int yval = *pY;
        int targetX = PACHINKO(p)->mTarget[0].x;
        int entX = *(int *)(xBase + n);
        angBase = base + BALL_OFF(angle);
        angle = cstd::atan2((targetY - yval) >> 12, (targetX - entX) >> 12);
        *(u16 *)(angBase + n) = (u16)angle;
        u16 angU = *(u16 *)(angBase + n);
        int angIdx = (angU >> 4) << 1;
        {
            s16 cosv = data_02082214[angIdx + 1];
            *pVx = FX_MUL(cosv, factor);
        }
        {
            u16 angU2 = *(u16 *)(angBase + n);
            s16 sinv = data_02082214[(angU2 >> 4) << 1];
            *pVy = FX_MUL(sinv, factor);
        }
        int fieldByte = BALL(base + n)->level;
        int div40 = (fieldByte * 0x180) / 40;
        int e8ret = func_020126e8(*(int *)(xBase + n));
        func_020126ac(0x186, 6, 0, div40 - 0x80, e8ret);
        BALL(base + n)->state = 4;
        BALL(base + n)->target = (u8)idx;
    }
}
#pragma pop

// @symbol PachinkoBall_CheckFall
void PachinkoBall_CheckFall(char *thiz, int idx) {
    unsigned char *flag = (unsigned char *)(thiz + BALL_OFF(fallPlayed)) + idx * 0x38;
    char *e;
    if (*flag != 0) return;
    e = thiz + idx * 0x38;
    if ((BALL(e)->y >> 0xc) < -0xf2)
        return;
    *flag = *flag + 1;
    func_02012718(0x184, BALL(e)->x);
}

// @symbol PachinkoBall_StatePop
void PachinkoBall_StatePop(char *c, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    int k = i * 0x38;
    unsigned short *cnt = &self->mBall[i].timer;
    if (*cnt != 0) {
        *cnt = *cnt - 1;
        if (*cnt != 0)
            return;
        PachinkoFx_Spawn(c, self->mBall[i].x, self->mBall[i].y, 2, sPachinkoPopScores[0], 0);
        func_02012718(0x18b, self->mBall[i].x);
        return;
    }
    /* active, visible: BALL() drops the + 0x4000 base and grows it. */
    *(unsigned char *)((c + k) + 0x4000 + 0x68c) = 0;
    *(unsigned char *)((c + k) + 0x4000 + 0x68d) = 0;
}

// @symbol PachinkoBall_StateSpinAway
void PachinkoBall_StateSpinAway(char *c, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    int n = i * 0x38;
    int x;
    int y;
    int f;
    short sv;

    self->mBall[i].x += self->mBall[i].velX;
    self->mBall[i].y += self->mBall[i].velY;
    self->mBall[i].rotation += 0x800;
    if (self->mBall[i].spinRadius <= 0x30000) {
        self->mBall[i].spinRadius += 0x800;
    }
    sv = data_02082214[(self->mBall[i].rotation >> 4) * 2 + 1];
    self->mBall[i].offX = FX_MUL(sv, self->mBall[i].spinRadius);
    sv = data_02082214[(self->mBall[i].rotation >> 4) * 2];
    self->mBall[i].offY = FX_MUL(sv, self->mBall[i].spinRadius);

    x = (self->mBall[i].x + self->mBall[i].offX) >> 12;
    y = (self->mBall[i].y + self->mBall[i].offY) >> 12;
    self->mBall[i].sound = Sound_PlayIfNotActive(self->mBall[i].sound, 2, 0x187, 0);
    f = 0;
    if (x >= 0x130 || x <= -0x30) {
        f++;
    }
    if (y >= 0xf0 || y <= -0x110) {
        f++;
    }
    if (f != 0) {
        self->mBall[i].active = 0;
        self->mBall[i].visible = 0;
    }
}

// @symbol PachinkoBall_StateGlide
void PachinkoBall_StateGlide(char *self, int idx) {
    char *new_var;
    *((int *)((self + BALL_OFF(x)) + (idx * 0x38))) =
        (*((int *)((self + BALL_OFF(x)) + (idx * 0x38)))) +
        (*((int *)((self + (idx * 0x38)) + BALL_OFF(velX))));
    new_var = self;
    *((int *)((new_var + BALL_OFF(y)) + (idx * 0x38))) =
        (*((int *)((new_var + BALL_OFF(y)) + (idx * 0x38)))) +
        (*((int *)((self + (idx * 0x38)) + BALL_OFF(velY))));
    {
        int raw = *((u16 *)((new_var + ((idx * 0x38) & 0xFFFFFFFF)) + BALL_OFF(angle)));
        u16 ip = (u16)(raw - 0x4000);
        short tgt = (short)ip;
        short *cur = (short *)((self + BALL_OFF(rotation)) + (idx * 0x38));
        if ((*cur) > tgt) {
            *cur = (*cur) - 0x100;
            if ((*cur) <= ((short)ip)) {
                *cur = ip & 0xFFFFFFFFFFFFFFFF;
            }
        } else if ((*cur) < ((short)ip)) {
            *cur = (*cur) + 0x100;
            raw = ip & 0xFFFFFFFFFFFFFFFF;
            if ((*cur) >= ((short)ip)) {
                *cur = raw;
            }
        }
    }
    PachinkoBall_CheckHitByBobOmb(new_var, idx);
    PachinkoBall_CheckPipes(self, idx);
    PachinkoBall_CountNearby(self, idx);
}

// @symbol PachinkoBall_StateDive
void PachinkoBall_StateDive(char *c, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    unsigned short t;
    int lim;

    if (self->mBall[i].timer != 0)
    {
        (self->mBall[i].timer)--;
        if (self->mBall[i].timer == 0)
        {
            func_02012718(0x185, self->mBall[i].x);
        }
    }

    self->mBall[i].x += self->mBall[i].velX;
    self->mBall[i].y += self->mBall[i].velY;

    t = self->mWaveLevel;
    if (t > 12)
    {
        lim = 0xd80 + ((t - 12) << 7);
    }
    else
    {
        lim = 0x600 + t * 0xa0;
    }

    if (self->mBall[i].velY <= lim) {
        self->mBall[i].velY += t * 4 + 8;
    }

    PachinkoBall_CheckHitByBobOmb(c, i);
    PachinkoBall_CheckPipes(c, i);
    PachinkoBall_CountNearby(c, i);

    self->mBall[i].angle = cstd::atan2(self->mBall[i].velY, self->mBall[i].velX);
    self->mBall[i].rotation = self->mBall[i].angle - 0x4000;

    PachinkoBall_AimAtTarget(c, i);
}

// @symbol PachinkoBall_StateDrift
void PachinkoBall_StateDrift(char *o, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)o;
    int idx;
    PachinkoBall_CheckFall(o, i);
    idx = i * 0x38;
    if (*(unsigned char *)(o + BALL_OFF(started) + idx) == 0) {
        *(unsigned char *)(o + BALL_OFF(started) + idx) += 1;
        if (self->mWaveLevel > 0xc) {
            *(int *)(ATI(o, idx) + BALL_OFF(velY)) =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf)
                 << 7) +
                0xb80 + ((self->mWaveLevel - 0xc) << 7);
        } else {
            *(int *)(ATU(o, idx) + BALL_OFF(velY)) =
                self->mWaveLevel * 0xa0 +
                (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf)
                  << 7) +
                 0x400);
        }
        if ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) != 0)
        {
            *(int *)(o + idx + BALL_OFF(velX)) = (self->mWaveLevel << 7) + 0x600;
            *(unsigned char *)(o + idx + BALL_OFF(driftDir)) = 0;
        }
        else
        {
            *(int *)(o + idx + BALL_OFF(velX)) = -((self->mWaveLevel << 7) + 0x600);
            *(unsigned char *)(o + idx + BALL_OFF(driftDir)) = 0;
        }
        *(unsigned short *)((ATI(o, 0) + idx) + BALL_OFF(timer)) =
            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf)
             << 3) +
            0x20;
    } else {
        int v12;
        int *vel;
        *(int *)(o + BALL_OFF(y) + idx) += *(int *)(o + idx + BALL_OFF(velY));
        vel = (int *)(o + BALL_OFF(velX) + idx);
        *(int *)(o + BALL_OFF(x) + idx) += *(int *)(o + BALL_OFF(velX) + idx);
        v12 = *(int *)(o + BALL_OFF(x) + idx) >> 12;
        if (v12 > 0xf0)
        {
            u8 d;
            *vel = -*vel;
            d = *(unsigned char *)(o + BALL_OFF(driftDir) + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 2;
                else
                    *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 1;
            }
            *(int *)(o + BALL_OFF(x) + idx) = 0xf0000;
        }
        if (v12 < 0x10)
        {
            u8 d;
            *(int *)(o + BALL_OFF(velX) + idx) = -*(int *)(o + BALL_OFF(velX) + idx);
            d = *(unsigned char *)(o + BALL_OFF(driftDir) + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 2;
                else
                    *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 1;
            }
            *(int *)(o + BALL_OFF(x) + idx) = 0x10000;
        }
        PachinkoBall_CheckHitByBobOmb(o, i);
        PachinkoBall_CheckPipes(o, i);
        PachinkoBall_CountNearby(o, i);
        {
            unsigned short t = *(unsigned short *)(o + BALL_OFF(timer) + idx);
            if (t != 0)
            {
                *(unsigned short *)(o + BALL_OFF(timer) + idx) = t - 1;
                if (*vel > 0)
                {
                    *(unsigned short *)(o + BALL_OFF(rotation) + idx) -= 0x80;
                    if (*(short *)(o + BALL_OFF(rotation) + idx) <= -0x1800)
                        *(unsigned short *)(o + BALL_OFF(rotation) + idx) = 0xe800;
                }
                else if (*vel < 0)
                {
                    *(unsigned short *)(o + BALL_OFF(rotation) + idx) += 0x80;
                    if (*(short *)(o + BALL_OFF(rotation) + idx) >= 0x1800)
                        *(short *)(o + BALL_OFF(rotation) + idx) = 0x1800;
                }
                if (*(short *)(o + BALL_OFF(timer) + idx) < 0)
                    *(short *)(o + BALL_OFF(timer) + idx) = 0;
                return;
            }
            {
                u8 st = *(unsigned char *)(o + BALL_OFF(driftDir) + idx);
                if (st == 0)
                {
                    int v = *vel;
                    if (v > 0)
                    {
                        *vel = v - 0x20;
                        *(unsigned short *)(o + BALL_OFF(rotation) + idx) += 0x80;
                        if (*(short *)(o + BALL_OFF(rotation) + idx) >= 0)
                            *(short *)(o + BALL_OFF(rotation) + idx) = 0;
                        if (*vel <= 0)
                        {
                            *(unsigned char *)(o + BALL_OFF(driftDir) + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                  << 1) >>
                                 0xf) +
                                1;
                            *(unsigned short *)(o + BALL_OFF(timer) + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                    0x7fff)
                                   << 4) >>
                                  0xf)
                                 << 2);
                            *(short *)(o + BALL_OFF(rotation) + idx) = 0;
                        }
                    }
                    else if (v < 0)
                    {
                        *vel = v + 0x20;
                        *(unsigned short *)(o + BALL_OFF(rotation) + idx) -= 0x80;
                        if (*(short *)(o + BALL_OFF(rotation) + idx) < 0)
                            *(short *)(o + BALL_OFF(rotation) + idx) = 0;
                        if (*vel >= 0)
                        {
                            *(unsigned char *)(o + BALL_OFF(driftDir) + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                                  << 1) >>
                                 0xf) +
                                1;
                            *(unsigned short *)(o + BALL_OFF(timer) + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) &
                                    0x7fff)
                                   << 4) >>
                                  0xf)
                                 << 2);
                            *(short *)(o + BALL_OFF(rotation) + idx) = 0;
                        }
                    }
                }
                else if (st == 1)
                {
                    *vel += 0x20;
                    *(unsigned short *)(o + BALL_OFF(rotation) + idx) -= 0x80;
                    if (*(short *)(o + BALL_OFF(rotation) + idx) <= -0x1800)
                        *(unsigned short *)(o + BALL_OFF(rotation) + idx) = 0xe800;
                    if (*vel >= (self->mWaveLevel << 7) + 0x600) {
                        *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 0;
                        *vel = (self->mWaveLevel << 7) + 0x600;
                        *(unsigned short *)(o + BALL_OFF(timer) + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                               << 5) >>
                              0xf)
                             << 3) +
                            0x10;
                    }
                }
                else
                {
                    *vel -= 0x20;
                    *(unsigned short *)(o + BALL_OFF(rotation) + idx) += 0x80;
                    if (*(short *)(o + BALL_OFF(rotation) + idx) >= 0x1800)
                        *(short *)(o + BALL_OFF(rotation) + idx) = 0x1800;
                    if (*vel <= -((self->mWaveLevel << 7) + 0x600)) {
                        *(unsigned char *)(o + BALL_OFF(driftDir) + idx) = 0;
                        *vel = -((self->mWaveLevel << 7) + 0x600);
                        *(unsigned short *)(o + BALL_OFF(timer) + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
                               << 5) >>
                              0xf)
                             << 3) +
                            0x10;
                    }
                }
            }
        }
    }
    PachinkoBall_AimAtTarget(o, i);
}

// @symbol PachinkoBall_StateFall
void PachinkoBall_StateFall(char *o, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)o;
    if (self->mBall[i].started == 0) {
        self->mBall[i].started += 1;
        if (self->mWaveLevel > 0xc) {
            self->mBall[i].velY =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf)
                 << 7) +
                0xb80 + ((self->mWaveLevel - 0xc) << 7);
        } else {
            self->mBall[i].velY =
                self->mWaveLevel * 0xa0 +
                (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf)
                  << 7) +
                 0x400);
        }
    } else {
        self->mBall[i].y += self->mBall[i].velY;
    }
    PachinkoBall_CheckFall(o, i);
    PachinkoBall_CheckHitByBobOmb(o, i);
    PachinkoBall_CheckPipes(o, i);
    PachinkoBall_CountNearby(o, i);
    PachinkoBall_AimAtTarget(o, i);
}

// @symbol PachinkoBall_StateWaiting
void PachinkoBall_StateWaiting(char *base, int idx) {
    int off = idx * 0x38;
    unsigned short *timer = (unsigned short *)(base + BALL_OFF(timer) + off);
    unsigned int r;
    char *o;
    if (*timer != 0) {
        *timer = *timer - 1;
        return;
    }
    r = RandomIntInternal(&data_0209d4b8);
    o = base + off;
    BALL(o)->state = ((((r >> 16) & 0x7fff) << 1) >> 0xf) + 1;
    BALL(o)->started = 0;
    BALL(o)->visible = 1;
    BALL(o)->sprite = 0;
    BALL(o)->rotation = 0;
    BALL(o)->spinRadius = 0;
    BALL(o)->offX = 0;
    BALL(o)->offY = 0;
    BALL(o)->scale = 0x1000;
    BALL(o)->fallPlayed = 0;
}

// @symbol PachinkoBall_UpdateAll
void PachinkoBall_UpdateAll(char *c) {
    int i = 0;
    char *r5 = c;
    do {
        if (BALL(r5)->active != 0) {
            int idx = BALL(r5)->state;
            Ent7d0 *e = &sPachinkoBallStateTable[idx];
            int adj = e->b;
            char *obj = c + (adj >> 1);
            int fn;
            if (adj & 1) {
                fn = *(int *)(*(int *)obj + e->a);
            } else {
                fn = e->a;
            }
            ((void (*)(void *, int))fn)(obj, i);
        }
        i++;
        r5 += 0x38;
    } while (i < 30);
}

/* Finds the first live entry within 0x18 of
 * the caller's entry and launch it.
 *
 * Walks the 0x30 records of stride 0x38 that begin at self+0x4000. A record is
 * a candidate when its flag byte at +0x4f0c is non-zero and its state byte at
 * +0x4f0d is at least 2. For each candidate the planar distance from record
 * `i`'s position (self + i*0x38 + 0x4660 for x, +0x4664 for z) is taken in
 * whole units (>> 12) and compared against 0x18 through cstd::sqrt of the
 * 64-bit sum of squares. The first record inside that radius wins: the scan
 * writes its heading into self+0x4684+i*0x38, derives a sine/cosine pair from
 * data_02082214 at 0x2000 scale plus a zero velocity pair into the record at
 * self+i*0x38+0x4000+0x668..0x674, plays the sound whose id comes from
 * sPachinkoPopSfx indexed by the hit record's counter (clamped at 5),
 * spawns the effect through PachinkoFx_Spawn, bumps that counter, and
 * returns. With no hit the loop runs out and the function returns having
 * written nothing.
 *
 * Two spellings are load-bearing for the register colouring in the guard
 * block, and only TOGETHER -- each one alone scores worse than either does
 * apart (16 and 11 against 7):
 *
 *   1. The four record fields in the guard block are reached off `walk` with
 *      the +0x4000 already folded into the constant (0x4f0c, 0x4f0d, 0x4ed8,
 *      0x4edc). Introducing a `char *w = walk + 0x4000` local and using
 *      w+0xf0c... is the same address arithmetic but creates a named web that
 *      takes r2 away from the loaded x.
 *   2. The z difference is computed BEFORE the x difference. mwccarm schedules
 *      the z term first either way, but the source order decides which of the
 *      two competing values reaches the allocator first, and with it whether
 *      the record base lands in r2 and the loaded x in r3 (the cartridge) or
 *      the other way round.
 */

// @symbol PachinkoBall_CheckHitByBobOmb
void PachinkoBall_CheckHitByBobOmb(char *base, int i)
{
    int n = i * 0x38;
    char *o = base;
    int j = 0;
    char *walk = o;
    int *xp = (int *)((o + n) + BALL_OFF(x));
    int *zp = (int *)((o + n) + BALL_OFF(y));
    do {
        if (SHOT(walk)->active == 0)
            goto next;
        if (SHOT(walk)->state < 2)
            goto next;
        {
            int dy = (*zp - SHOT(walk)->y) >> 12;
            int dx = (*xp - SHOT(walk)->x) >> 12;
            s64 distsq = (s64)dx * dx + (s64)dy * dy;
            if (cstd::sqrt((unsigned long long)distsq) > 0x18)
                goto next;
        }
        {
            int m = j * 0x38;
            int five = 5;
            char *ov = (char *)o;
            unsigned char *comboBase = (unsigned char *)(o + SHOT_OFF(combo));
            char *bn = ov + n;
            char *bm = ov + m;
            u16 ang;
            s16 s;
            s16 c;
            int vel;
            int cnt;
            int cc;
            char *rotationBase;
            char *spinRadiusBase;
            char *xBase;
            unsigned char *cntp;
            int round = 0x800;

            BALL(bn)->state = (u8)five;
            {
                char *r = bm + 0x4f00; /* split base: + 8 is the angle */
                ang = *(u16 *)(r + 8);
                r = o + BALL_OFF(rotation);
                rotationBase = r;
            }
            *(u16 *)(rotationBase + n) = ang;
            ang = *(u16 *)(rotationBase + n);

            s = data_02082214[((ang >> 4) << 1) + 1];
            {
                unsigned char *lr = comboBase;
                spinRadiusBase = o + BALL_OFF(spinRadius);
                BALL(bn)->velX = (int)((((s64)s * 0x2000) + round) >> 12);
                ang = *(u16 *)(rotationBase + n);
                c = data_02082214[(ang >> 4) << 1];
                BALL(bn)->velY = (int)((((s64)c * 0x2000) + round) >> 12);
                *(int *)(spinRadiusBase + n) = 0;
                ang = *(u16 *)(rotationBase + n);
                vel = *(int *)(spinRadiusBase + n);
                s = data_02082214[((ang >> 4) << 1) + 1];
                BALL(bn)->offX = (int)((((s64)s * (s64)vel) + round) >> 12);
                ang = *(u16 *)(rotationBase + n);
                vel = *(int *)(spinRadiusBase + n);
                c = data_02082214[(ang >> 4) << 1];
                BALL(bn)->offY = (int)((((s64)c * (s64)vel) + round) >> 12);
                cnt = *(u8 *)(lr + m);
                cc = cnt;
                if (cc >= 5)
                    cc = five;
                xBase = o + BALL_OFF(x);
                func_02012718(sPachinkoPopSfx[cc], *(int *)(xBase + n));
                {
                    char *q = (char *)o + n;
                    PachinkoFx_Spawn(o, *(int *)(xBase + n), BALL(q)->y, 2,
                                     (int)sPachinkoPopScores[cc], 0);
                }
                cntp = comboBase + m;
                *cntp = (u8)(*cntp + 1);
            }
            return;
        }
    next:
        j += 1;
        walk += 0x38;
    } while (j < 0x30);
}

// @symbol Pachinko_SpawnWave
void Pachinko_SpawnWave(dScMgPachinko_c *self)
{
    int i;
    int ones;
    int tens;

    if (self->mWaveTimer != 0) {
        self->mWaveTimer--;
        if ((s16)self->mWaveTimer < 0)
            self->mWaveTimer = 0;
        return;
    }
    self->mWaveTimer = 0x50;
    for (i = 0; i < 0x1e; i++) {
        int ra;
        int rb;
        if (self->mBall[i].active != 0) continue;
        self->mBall[i].active = 1;
        ra = RandomIntInternal(&data_0209d4b8);
        rb = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].x = (sPachinkoWaveColumnX[(((u32)rb >> 16) & 0x7fff) * 5 >> 15] +
                            ((((u32)ra >> 16) & 0x7fff) * 4 >> 15) * 8)
                           << 12;
        self->mBall[i].y = -0x100000;
        self->mBall[i].velX = 0;
        self->mBall[i].velY = 0;
        self->mBall[i].state = 0;
        self->mBall[i].started = 0;
        ra = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].timer = ((((u32)ra >> 16) & 0x7fff) * 8 >> 15) << 4;
        self->mBall[i].rotation = 0;
        self->mBall[i].sound = 0;
        self->mBall[i].level = self->mWaveLevel;
        self->mWaveSpawns++;
        if (self->mWaveLevel == 0)
            break;
        {
            int r = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
            int lvl = self->mWaveLevel >> 1;
            if (lvl >= 4) lvl = 4;
            if (sPachinkoWaveSlots[r + lvl * 4] == 0)
                break;
        }
        if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) {
            self->mBall[i].x = 0x100000;
            self->mBall[i].angle = 0xc000 - (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9);
        } else {
            self->mBall[i].x = 0;
            self->mBall[i].angle = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9) + 0xc000;
        }
        self->mBall[i].spinRadius = 0;
        self->mBall[i].offX = 0;
        self->mBall[i].offY = 0;
        self->mBall[i].rotation = self->mBall[i].angle - 0x4000;
        self->mBall[i].y = -0x60000;
        self->mBall[i].velX = FX_MUL(data_02082214[(self->mBall[i].angle >> 4) * 2 + 1], 0xe80);
        self->mBall[i].velY = FX_MUL(data_02082214[(self->mBall[i].angle >> 4) * 2], 0xe80);
        self->mBall[i].state = 3;
        self->mBall[i].visible = 1;
        self->mBall[i].sprite = 0;
        self->mBall[i].scale = 0x1000;
        self->mBall[i].timer = 0x10;
        break;
    }
    ones = self->mWaveSpawns;
    for (tens = 0; ones >= 0xa; tens++) ones -= 0xa;
    if (tens != 0 && ones == 0)
        self->mWaveLevel++;
    if (self->mWaveLevel > 0x28)
        self->mWaveLevel = 0x27;
    self->mWaveTimer -= self->mWaveLevel << 2;
    if ((s16)self->mWaveTimer <= 0x20)
        self->mWaveTimer = 0x20;
}

// @symbol PachinkoBobOmb_ClearAll
void PachinkoBobOmb_ClearAll(char *p)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        SHOT(p)->active = 0;
        SHOT(p)->visible = 0;
        p += 0x38;
    }
}

// @symbol PachinkoBobOmb_DrawAll
void PachinkoBobOmb_DrawAll(char *thiz) {
    int i;
    for (i = 0; i < 0x30; i++, thiz += 0x38) {
        if (SHOT(thiz)->visible == 0)
            continue;
        Mg_RenderSpriteBothScaled((OamAttr *)sPachinkoBobOmbSprites, SHOT(thiz)->x >> 0xc,
                                  SHOT(thiz)->y >> 0xc, -1, -1, 0x1000, 0);
    }
}

// @symbol PachinkoBobOmb_UpdateAll
void PachinkoBobOmb_UpdateAll(char *c) {
    int i = 0;
    char *r5 = c;
    do {
        if (SHOT(r5)->active != 0) {
            int idx = SHOT(r5)->state;
            Ent7d0 *e = &sPachinkoBobOmbStateTable[idx];
            int adj = e->b;
            char *obj = c + (adj >> 1);
            int fn;
            if (adj & 1) {
                fn = *(int *)(*(int *)obj + e->a);
            } else {
                fn = e->a;
            }
            ((void (*)(void *, int))fn)(obj, i);
        }
        i++;
        r5 += 0x38;
    } while (i < 48);
}

// @symbol Pachinko_CountLaunch
void Pachinko_CountLaunch(char *c)
{
    dScMgPachinko_c *self = PACHINKO(c);
    if (self->mPlayStarted == 0)
        self->mPlayStarted += 1;
}

// @symbol PachinkoBobOmb_StateWaitGrab
void PachinkoBobOmb_StateWaitGrab(char *self, int i) {
    unsigned int idx = gActivePlayerSlot;
    int flag = 0;
    char *p;
    int dx, dy;

    if (gTouchHeld[idx][0] != 0) {
        flag = gTouchEdge[idx][0] != 0;
    }
    if (flag != 0) {
        p = self + i * 0x38;
        dx = (SHOT(p)->x >> 12) - gTouchX[idx][0];
        dy = (SHOT(p)->y >> 12) - gTouchY[idx][0];
        SHOT(p)->state = 1;
        SHOT(p)->offX = dx << 12;
        SHOT(p)->offY = dy << 12;
        SHOT(p)->combo = 0;
    }
    Pachinko_DrawAimLine(self);
}

#pragma push
#pragma opt_propagation off
// @symbol PachinkoBobOmb_StateAim
/* One ball's stylus frame. While the stylus is down the ball follows the touch
 * point through its grab offset, clamped into the 8..0xf8 x 8..0xb8 box, the
 * offset is re-derived, and the drag-speed sound is re-armed at 0x17b or 0x17c
 * depending on whether the distance from the launcher grew by more than ten.
 * On release the launch fires: the pull-back vector becomes an angle at 0x4f08
 * and a speed at 0x4ef8 (the square root scaled by 1.953, clamped to 0xa000),
 * that speed drives the sine table into the velocity pair at 0x4ee0/0x4ee4, and
 * the ball is re-seated at 0x80000, 0x28000 if the pull was too short.
 *
 * Four spellings only move registers, and each was measured:
 * - dy is taken before dx in both distance blocks;
 * - the second sine read goes through a named `const s16 *` (non-const changes
 *   the size; doing the same to the first read is worse), which keeps the two
 *   reads of data_02082214 from sharing one table base;
 * - the last y clamp stores through `py`, assigned between the two x clamps;
 * - the first velocity word's table index is a block-scope local, not inline
 *   and not at function scope.
 * `opt_propagation off` was measured against deleting it outright.
 */
void PachinkoBobOmb_StateAim(dScMgPachinko_c *self, int i) {
    char *c = (char *)self;
    s32 cx;
    s32 *py;
    s32 cy;
    u32 idx = gActivePlayerSlot;

    if (gTouchHeld[idx][0] != 0) {
        int off = i * 0x38;
        s32 vy, vx;

        cx = gTouchX[idx][0];
        cy = gTouchY[idx][0];
        *(s32 *)(c + SHOT_OFF(x) + off) = (cx << 12) + *(s32 *)(c + SHOT_OFF(offX) + off);
        *(s32 *)(c + SHOT_OFF(y) + off) = (cy << 12) + *(s32 *)(c + SHOT_OFF(offY) + off);

        vy = *(s32 *)(c + SHOT_OFF(y) + off) >> 12;
        vx = *(s32 *)(c + SHOT_OFF(x) + off) >> 12;
        if (vx >= 0xf8)
            *(s32 *)(c + SHOT_OFF(x) + off) = 0xf8000;
        py = (s32 *)(c + SHOT_OFF(y) + off);
        if (vx <= 8)
            *(s32 *)(c + SHOT_OFF(x) + off) = 0x8000;
        if (vy >= 0xb8)
            *py = 0xb8000;
        if (vy <= 8)
            *py = 0x8000;

        {
            s32 nx = (*(s32 *)(c + SHOT_OFF(x) + off) >> 12) - gTouchX[gActivePlayerSlot][0];
            s32 ny = (*(s32 *)(c + SHOT_OFF(y) + off) >> 12) - gTouchY[gActivePlayerSlot][0];

            *(s32 *)(c + SHOT_OFF(offX) + off) = nx << 12;
            *(s32 *)(c + SHOT_OFF(offY) + off) = ny << 12;
        }

        {
            s32 dy = 0x80 - (*(s32 *)(c + SHOT_OFF(x) + off) >> 12);
            s32 dx = 0x20 - (*(s32 *)(c + SHOT_OFF(y) + off) >> 12);
            s32 dist = cstd::sqrt((u64)(s64)(dy * dy + dx * dx));
            s32 prev = *(s32 *)(c + SHOT_OFF(dist) + off);

            *(s32 *)(c + SHOT_OFF(dist) + off) = dist;
            if (dist > prev) {
                if (dist > prev + 10)
                    *(s32 *)(c + SHOT_OFF(sound) + off) =
                        Sound_PlayIfNotActive(*(s32 *)(c + SHOT_OFF(sound) + off), 2, 0x17b, 0);
                else
                    *(s32 *)(c + SHOT_OFF(sound) + off) =
                        Sound_PlayIfNotActive(*(s32 *)(c + SHOT_OFF(sound) + off), 2, 0x17c, 0);
            }
        }
    } else {
        int off = i * 0x38;
        s32 dy, dx, dist;

        *(u8 *)(c + SHOT_OFF(state) + off) = 2;
        dy = 0x80 - (*(s32 *)(c + SHOT_OFF(x) + off) >> 12);
        dx = 0x20 - (*(s32 *)(c + SHOT_OFF(y) + off) >> 12);
        dist = cstd::sqrt((u64)(s64)(dy * dy + dx * dx));
        if (dist >= 0x10) {
            {
                s16 ang = cstd::atan2(dx, dy);
                char *p = c + off;

                *(s16 *)(p + SHOT_OFF(angle)) = ang;
            }
            *(s32 *)(c + SHOT_OFF(speed) + off) = cstd::sqrt((u64)(s64)(dy * dy + dx * dx)) << 8;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            *(s32 *)(c + SHOT_OFF(speed) + off) += *(s32 *)(c + SHOT_OFF(speed) + off) >> 2;
            if (*(s32 *)(c + SHOT_OFF(speed) + off) >= 0xa000)
                *(s32 *)(c + SHOT_OFF(speed) + off) = 0xa000;
            self->mNextBallDelay = (u16)(*(s32 *)(c + SHOT_OFF(speed) + off) >> 10);
            if (self->mNextBallDelay == 0)
                self->mNextBallDelay = 1;
            *(u8 *)(c + 0x5c31) = 0;
            {
                int ai = (*(u16 *)(c + SHOT_OFF(angle) + off) >> 4) * 2 + 1;
                *(s32 *)(c + SHOT_OFF(velX) + off) =
                    FX_MUL(data_02082214[ai], *(s32 *)(c + SHOT_OFF(speed) + off));
            }
            {
                const s16 *sp = &data_02082214[(*(u16 *)(c + SHOT_OFF(angle) + off) >> 4) * 2];
                *(s32 *)(c + SHOT_OFF(velY) + off) =
                    FX_MUL(*sp, *(s32 *)(c + SHOT_OFF(speed) + off));
            }
            *(s16 *)(c + SHOT_OFF(timer) + off) = 0;
            if (dist >= 0x40)
                Sound::PlayBank2_2D(0x17e);
            else
                Sound::PlayBank2_2D(0x17d);
        } else {
            *(s32 *)(c + SHOT_OFF(x) + off) = 0x80000;
            *(s32 *)(c + SHOT_OFF(y) + off) = 0x28000;
            *(u8 *)(c + SHOT_OFF(state) + off) = 0;
        }
    }
    Pachinko_DrawAimLine(self);
}
#pragma pop

// @symbol PachinkoBobOmb_StateFlying
void PachinkoBobOmb_StateFlying(char *c, int idx) {
    int n = idx * 0x38;
    int old;
    int e, b;

    *(int *)(c + SHOT_OFF(x) + n) =
        *(int *)(c + SHOT_OFF(x) + n) + *(int *)(c + SHOT_OFF(velX) + n);
    *(int *)(c + SHOT_OFF(y) + n) =
        *(int *)(c + SHOT_OFF(y) + n) + *(int *)(c + SHOT_OFF(velY) + n);
    /* a typed mShot[idx]. access here changes the code. */
    *(int *)(c + SHOT_OFF(gravity) + n) = *(int *)(c + SHOT_OFF(gravity) + n) + 0x10;
    old = *(int *)(c + SHOT_OFF(velY) + n);
    *(int *)(c + SHOT_OFF(velY) + n) = old + *(int *)(c + SHOT_OFF(gravity) + n);
    if (*(int *)(c + SHOT_OFF(velY) + n) >= 0x8000)
        *(int *)(c + SHOT_OFF(velY) + n) = 0x8000;

    *(s16 *)(c + SHOT_OFF(angle) + n) =
        cstd::atan2(*(int *)(c + SHOT_OFF(velY) + n) >> 12, *(int *)(c + SHOT_OFF(velX) + n) >> 12);

    if (old < 0) {
        if (*(int *)(c + SHOT_OFF(velY) + n) >= 0)
            *(s16 *)(c + SHOT_OFF(timer) + n) = 0x40;
    }

    if (*(u16 *)(c + SHOT_OFF(timer) + n) != 0) {
        *(volatile u16 *)(c + SHOT_OFF(timer) + n) = *(volatile u16 *)(c + SHOT_OFF(timer) + n) - 1;
        if (*(s16 *)(c + SHOT_OFF(timer) + n) <= 0)
            *(u8 *)(c + SHOT_OFF(state) + n) = 4;
    }

    b = *(int *)(c + SHOT_OFF(x) + n) >> 12;
    e = *(int *)(c + SHOT_OFF(y) + n) >> 12;
    if (e <= -0x120)
        *(u8 *)(c + SHOT_OFF(state) + n) = 4;
    if (e >= 0xd0)
        *(u8 *)(c + SHOT_OFF(state) + n) = 4;
    if (b >= 0x140 || b <= -0x40)
        *(u8 *)(c + SHOT_OFF(state) + n) = 4;

    Pachinko_ClearAimLine(c);
    if (PACHINKO(c)->mAimShot != 0)
        Pachinko_DrawAimLine(c);
}

// @symbol PachinkoBobOmb_StateFallAway
void PachinkoBobOmb_StateFallAway(char *base, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)base;
    int n = i * 0x38;
    s16 t;
    u16 v;
    self->mShot[i].speed = self->mShot[i].speed + self->mShot[i].gravity;
    self->mShot[i].gravity = self->mShot[i].gravity + 0x10;
    self->mShot[i].x =
        self->mShot[i].x + FX_MUL(data_02082214[((self->mShot[i].angle >> 4) << 1) + 1], 0x800);
    t = data_02082214[(self->mShot[i].angle >> 4) << 1];
    self->mShot[i].y = self->mShot[i].y + FX_MUL(t, self->mShot[i].speed);
    v = self->mShot[i].timer;
    if (v != 0) {
        self->mShot[i].timer = v - 1;
        if ((s16)self->mShot[i].timer < 0)
            self->mShot[i].timer = 0;
    } else {
        self->mShot[i].state = 4;
    }
}

// @symbol PachinkoBobOmb_StateRemove
void PachinkoBobOmb_StateRemove(char *p, int idx) {
    PACHINKO(p)->mShot[idx].active = 0;
    PACHINKO(p)->mShot[idx].visible = 0;
}

// @symbol Pachinko_UpdateNextBall
void Pachinko_UpdateNextBall(char *o) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)o;
    int i;
    char *p;

    if (self->mNextBallDelay == 0)
        return;
    self->mNextBallDelay -= 1;
    if ((short)self->mNextBallDelay > 0)
        return;
    self->mNextBallDelay = 0;

    p = o;
    for (i = 0; i < 0x30; i++) {
        if (SHOT(p)->active == 0) {
            p = o + i * 0x38;
            SHOT(p)->active = 1;
            SHOT(p)->visible = 1;
            SHOT(p)->state = 0;
            SHOT(p)->gravity = 0;
            SHOT(p)->x = 0x80000;
            SHOT(p)->y = 0x28000;
            SHOT(p)->offX = 0;
            SHOT(p)->offY = 0;
            SHOT(p)->combo = 0;
            SHOT(p)->velX = 0;
            SHOT(p)->velY = 0;
            self->mShot[i].timer = 0;
            SHOT(p)->sound = 0;
            SHOT(p)->dist = 4;
            self->mAimShot = (unsigned char)(i + 1);
            if (self->mBallsServed != 0) {
                Sound::PlayBank2_2D(0x18d);
                return;
            }
            self->mBallsServed += 1;
            return;
        }
        p += 0x38;
    }
}

// @symbol Pachinko_ResetRound
void Pachinko_ResetRound(char *self)
{
    dScMgPachinko_c *scene = (dScMgPachinko_c *)self;
    int i, j, k, l, m, n;
    char *r, *s, *t;

    scene->mPhase = 0;

    for (i = 0; i < 30; i++) {
        scene->mPopup[i].x = 0;
        scene->mPopup[i].y = 0;
        scene->mPopup[i].points = 0;
        scene->mPopup[i].timer = 0;
    }

    for (j = 0; j < 3; j++) {
        scene->mRow[j].active = 0;
        scene->mRow[j].visible = 0;
    }

    /* r, s and t walk `this` one record per pass, so their [0] is element k,
       l or m; indexing the arrays directly grows the function by 16 bytes.
       The two raw pipe stores hit pads nothing reads. */
    r = self;
    for (k = 0; k < 2; k++) {
        ((dScMgPachinko_c *)r)->mPipe[0].x = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].y = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].vel = 0;
        *(int *)(r + 0x4eac) = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].timer = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].sprite = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].state = 0;
        *(char *)(r + 0x4eb2) = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].active = 0;
        ((dScMgPachinko_c *)r)->mPipe[0].visible = 0;
        r += 0x1c;
    }

    s = self;
    for (l = 0; l < 0x30; l++) {
        ((dScMgPachinko_c *)s)->mShot[0].x = 0;
        ((dScMgPachinko_c *)s)->mShot[0].y = 0;
        ((dScMgPachinko_c *)s)->mShot[0].prevX = 0;
        ((dScMgPachinko_c *)s)->mShot[0].prevY = 0;
        ((dScMgPachinko_c *)s)->mShot[0].speed = 0;
        ((dScMgPachinko_c *)s)->mShot[0].gravity = 0;
        ((dScMgPachinko_c *)s)->mShot[0].angle = 0;
        ((dScMgPachinko_c *)s)->mShot[0].timer = 0;
        ((dScMgPachinko_c *)s)->mShot[0].active = 0;
        ((dScMgPachinko_c *)s)->mShot[0].state = 0;
        ((dScMgPachinko_c *)s)->mShot[0].visible = 0;
        s += 0x38;
    }

    t = self;
    for (m = 0; m < 30; m++) {
        ((dScMgPachinko_c *)t)->mBall[0].active = 0;
        ((dScMgPachinko_c *)t)->mBall[0].visible = 0;
        ((dScMgPachinko_c *)t)->mBall[0].state = 0;
        t += 0x38;
    }

    for (n = 0; n < 30; n++) {
        scene->mFx[n].active = 0;
        scene->mFx[n].visible = 0;
    }

    scene->mFlyer[0].active = 0;
    scene->mFlyer[0].state = 0;
    scene->mFlyer[0].visible = 0;
    scene->mEndTimer = 0;
    /* 0x5c1e, 0x5c22 and 0x5c30 stay raw header pads: nothing in this
       file reads them; 0x5c30 is also cleared at time up. */
    *(short *)(self + 0x5c22) = 0;
    *(short *)(self + 0x5c1e) = 0;
    *(char *)(self + 0x5c30) = 0;
    scene->unk_5c31 = 0;
    scene->mPlayStarted = 0;
    scene->mWaveTimer = 0;
    scene->mFlyerOut = 0;
    scene->mWaveSpawns = 0;
    scene->mWaveLevel = 0;
    scene->mHitPause = 0;
    scene->mBallsServed = 0;

    Mg_SubmitScore(0);
    PachinkoTarget_HideAll(self);
    PachinkoBg_Stop(self);
    PachinkoBg_Start(self);
}

}  /* extern "C" */

// @symbol _ZN15dScMgPachinko_c10StartRoundEi
/* Vtable slot 18, an override of
   dScMgBase_c::StartRound(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgPachinko_c::StartRound(int n)
{
    /* The score increment reads mLevel through a const view of `this`.
       Without it mwcc CSEs the +0xbc field address into its own register
       (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the function grows a
       word; the cartridge re-issues ldr r1,[r4,#0xbc] / str r1,[r4,#0xbc].
       Same lever as dScMgPachinko2_c::StartRound. */
    const dScMgPachinko_c *ro = this;

    mPhase = 0;
    if (n == 9) {
        mLevel = ro->mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
    } else {
        mLevel = 0;
        if (mLevel > 0x270e) mLevel = 0x270e;
    }
    PachinkoBg_Stop((char *)this);
    PachinkoBg_Start((char *)this);
}

// @symbol _ZN15dScMgPachinko_c6RenderEv
/* Vtable slot 9. */
s32 dScMgPachinko_c::Render()
{
    Pachinko_DrawTopRecord(this);
    Pachinko_DrawScore(this);
    PachinkoFx_DrawAll(this);
    PachinkoPopup_DrawAll((char *)this);
    PachinkoBobOmb_DrawAll((char *)this);
    PachinkoFlyer_Draw((char *)this);
    PachinkoBall_DrawAll((char *)this);
    PachinkoTarget_DrawAll((char *)this);
    PachinkoPipe_DrawAll((char *)this);
    PachinkoDeco_DrawAll((char *)this);
    return 1;
}

// @symbol _ZN15dScMgPachinko_c8BehaviorEv
/* Vtable slot 6. */
s32 dScMgPachinko_c::Behavior()
{
    char *c = (char *)this;

    switch (mPhase) {
    case 0:
        FreeGfxSlotsById(0x1d);
        Pachinko_ResetRound(c);
        PachinkoPipe_ResetAll((u8 *)c);
        PachinkoTarget_ShowAll((u8 *)c);
        mPhase = 1;
        mNextBallDelay = 0x10;
        mWaveTimer = 0x60;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (mHitPause != 0) {
            PachinkoFx_UpdateAll(c);
            mHitPause--;
        } else {
            Pachinko_SpawnWave(this);
            Pachinko_CountLaunch(c);
            Pachinko_UpdateNextBall(c);
            PachinkoBobOmb_UpdateAll(c);
            PachinkoBall_UpdateAll(c);
            PachinkoTarget_Choose(c);
            PachinkoFlyer_Update((C2 *)c);
            PachinkoPipe_UpdateAll(c);
            PachinkoPopup_UpdateAll(c);
            Pachinko_CheckTargetsLeft(c);
            PachinkoFx_UpdateAll(c);
        }
        break;
    case 2:
        if (mEndTimer != 0) {
            mEndTimer--;
            if ((s16)mEndTimer <= 0) {
                Mg_SetSharedState(MG_STATE_RESULTS);
                mPromptEnabled = 0;
            }
        }
        PachinkoFx_UpdateAll(c);
        PachinkoBall_UpdateAll(c);
        PachinkoFlyer_Update((C2 *)c);
        PachinkoPipe_UpdateAll(c);
        PachinkoPopup_UpdateAll(c);
        break;
    }
    PachinkoBg_Update((C *)c);
    return 1;
}

// @symbol _ZN15dScMgPachinko_c13InitResourcesEv
/* Vtable slot 0. */
s32 dScMgPachinko_c::InitResources()
{
    char *c = (char *)this;
    char *b;
    char *dst;
    volatile u16 fillScr;
    volatile u16 fillZero;
    volatile u16 fillIdx;
    int objChar;
    int objPltt;
    int f;
    int n;
    int y;
    int x;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x5f);
    DecompressLZ16((void *)f, (void *)func_02054d88());
    Deallocate((void *)f);

    f = LoadFile(0x60);
    GX::LoadBGPltt((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x61);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5418;

    f = LoadFile(0x46);
    DecompressLZ16((void *)f, G2::GetBG2CharPtr());
    Deallocate((void *)f);

    b = (char *)G2::GetBG2ScrPtr();
    fillScr = 0x3073;
    MultiStore16(fillScr, b, 0x1000);

    f = LoadFile(6);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    objChar = LoadFile(0xe0);
    objPltt = LoadFile(0xe1);
    DecompressLZ16((void *)objChar, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)objPltt, 0, 0x100);

    data_0209d454 |= 0xd;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    Mg_InitBrush(c, 0, 2, 0);

    b = (char *)G2S::GetBG2CharPtr();
    fillZero = 0;
    MultiStore16(fillZero, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)G2S::GetBG2ScrPtr() + x + y * 0x20);
            fillIdx = n;
            MultiStore16(fillIdx, dst, 2);
            n++;
        }
    }

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x208;

    f = LoadFile(0x5c);
    DecompressLZ16((void *)f, (void *)G2S::GetBG3CharPtr());
    Deallocate((void *)f);

    f = LoadFile(0x5d);
    GXS::LoadBGPltt((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x5e);
    func_020562b4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x608;

    f = LoadFile(5);
    func_020564f4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    DecompressLZ16((void *)objChar, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)objPltt, 0, 0x100);
    Deallocate((void *)objChar);
    Deallocate((void *)objPltt);

    Pachinko_ResetRound(c);
    PachinkoPipe_ResetAll((u8 *)c);
    PachinkoTarget_ShowAll((u8 *)c);
    Mg_SetTopScreenYOffset(0x20);

    mRunDuringFade = 1;
    mNextBallDelay = 0x10;
    mWaveTimer = 0x60;
    mPhase = 1;
    mRunDuringFade = 1;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgPachinko_c through RTTI,
 * allocation size, vtable identity, and the MG_PACHINKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgBobOmbSquad_Spawn. */
// @symbol dScMgPachinko_c_classInit
extern "C" void *dScMgPachinko_c_classInit()
{
    return new dScMgPachinko_c;
}

/* The five state/record tables, defined in retail copy order: the 5-row
   and 7-row Ent7d0 tables, the 4-row Ent7d0 table, the 5-row PMF2 table,
   then the 4-row Entry table. mwcc emits __sinit_d_s_mg_pachinko.cpp from
   these definitions, copying the ROM .data records. */
Ent7d0 sPachinkoBobOmbStateTable[5] = {
    pmf_PachinkoBobOmb_StateWaitGrab, pmf_PachinkoBobOmb_StateAim,
    pmf_PachinkoBobOmb_StateFlying,   pmf_PachinkoBobOmb_StateFallAway,
    pmf_PachinkoBobOmb_StateRemove,
};
Ent7d0 sPachinkoBallStateTable[7] = {
    pmf_PachinkoBall_StateWaiting, pmf_PachinkoBall_StateFall,  pmf_PachinkoBall_StateDrift,
    pmf_PachinkoBall_StateDive,    pmf_PachinkoBall_StateGlide, pmf_PachinkoBall_StateSpinAway,
    pmf_PachinkoBall_StatePop,
};
Ent7d0 sPachinkoPipeStateTable[4] = {
    pmf_PachinkoPipe_StatePick,
    pmf_PachinkoPipe_StateRise,
    pmf_PachinkoPipe_StateSink,
    pmf_PachinkoPipe_StateIdle,
};
PMF2 sPachinkoFlyerStateTable[5] = {
    pmf_PachinkoFlyer_StateWaiting, pmf_PachinkoFlyer_StateDescend, pmf_PachinkoFlyer_StateCross,
    pmf_PachinkoFlyer_StatePause,   pmf_PachinkoFlyer_StateLeave,
};
Entry sPachinkoBgStateTable[4] = {
    {pmf_PachinkoBg_StateWaiting},
    {pmf_PachinkoBg_StateRiseIn},
    {pmf_PachinkoBg_StateScroll},
    {pmf_PachinkoBg_StateSinkOut},
};
