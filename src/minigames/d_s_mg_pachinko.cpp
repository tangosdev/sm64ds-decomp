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
// - The scene is still reached through free functions taking a char pointer;
//   each opens with `dScMgPachinko_c *self = (dScMgPachinko_c *)raw` and
//   indexes the record arrays by element. The state tables are pointer-to-
//   member tables in the ROM ({code, adjustment} pairs), so the handlers were
//   members of the scene; only PachinkoFlyer_Update dispatches as one so far.
// - PachinkoBall_AimAtTarget, PachinkoBall_CheckHitByBobOmb and
//   PachinkoBobOmb_StateAim keep the field-base addressing above; natural
//   indexing misses in each (AimAtTarget's note gives the sizes; 21 words of
//   register colouring in CheckHitByBobOmb and 88 in StateAim, which also
//   keeps `opt_propagation off`, measured 2026-11-07).
// - The BG2 character fills (Pachinko_ClearAimLine, _CheckTargetsLeft,
//   _DrawAimLine, InitResources) go through a volatile zero; in
//   Pachinko_ClearAimLine a by-reference fill wrapper is 7 words off
//   (measured 2026-11-06).
// - The scene bytes 0x5c1e, 0x5c22 and 0x5c30 are header pads that only
//   Pachinko_ResetRound and time up clear, so they stay raw.

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx.h"
#include "dScMgPachinko_c.h"
#include "LevelRng.h"

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

/* The {code pointer, adjustment} records PachinkoPipe_UpdateAll,
   PachinkoBall_UpdateAll and PachinkoBobOmb_UpdateAll walk. */
struct Ent7d0 { int a; int b; };

/* The sprite record Mg_RenderSpriteBothScaled draws; only handled by pointer here. */
struct OamAttr;

/* The 2x2 rotate-and-scale matrix PachinkoBall_DrawAll hands the sprite
   drawer. */
#include "Matrix2x2.h"

/* The three functions that keep the ROM's field-base addressing
 * (PachinkoBall_AimAtTarget, PachinkoBall_CheckHitByBobOmb,
 * PachinkoBobOmb_StateAim) reach a record field as the scene plus the
 * field's whole-scene offset, then add the pre-scaled index i * 0x38;
 * self->mBall[i].f folds the field into the index instead and does not match
 * there. SHOT/BALL view one record already stepped by i * 0x38. */
#define SHOT_BASE ((int)&((dScMgPachinko_c *)0)->mShot)
#define BALL_BASE ((int)&((dScMgPachinko_c *)0)->mBall)
#define SHOT(p) ((dScMgPachinko_shot *)((p) + SHOT_BASE))
#define BALL(p) ((dScMgPachinko_ball *)((p) + BALL_BASE))
#define SHOT_OFF(member) (SHOT_BASE + (int)&((dScMgPachinko_shot *)0)->member)
#define BALL_OFF(member) (BALL_BASE + (int)&((dScMgPachinko_ball *)0)->member)

/* A random integer in [0, n): the top 15 bits of the shared generator, scaled. */
#define PACHINKO_RAND(n) (((((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n)) >> 15)

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
extern void Mg_DrawTopRightCounter(int a, int b);
extern int  Mg_GetTopRecord(void);
extern void Mg_DrawTopLeftCounter(int score);
extern void Mg_SubmitScore(int self);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void SetBg2Offset(int a, int b);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
struct OamAttr;
#include "OAM.h"
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
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *p);
extern int  func_02054d88(void);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void func_020564f4(const void *src, int offset, int count);

/* --- ROM data this TU reads --- */
extern unsigned char gMainScreenLayers;
extern u8 gSubScreenLayers;
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
extern void PachinkoFlyer_StateDescend(dScMgPachinko_c *self, int i);
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
void PachinkoDeco_DrawAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    for (int i = 0; i < 3; i++) {
        if (self->mRow[i].visible != 0) {
            OAM::Render(false, (OamAttr *)sPachinkoDecoSprite, self->mRow[i].x >> 12,
                        self->mRow[i].y >> 12, -1, -1, 0x1000, 0x1000,
                        self->mRow[i].angle, -1);
        }
    }
}

// @symbol PachinkoDeco_UpdateAll
void PachinkoDeco_UpdateAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;

    for (i = 0; i < 3; i++) {
        self->mRow[i].x = (data_ov006_0212eb44[i] << 12) - self->mBg[0].x;
        self->mRow[i].y = -self->mBg[0].y;
        if (self->mRow[i].dir != 0) {
            self->mRow[i].angle -= 0x40;
            if ((s16)self->mRow[i].angle <= -0x1000) {
                self->mRow[i].angle = -0x1000;
                self->mRow[i].dir = 0;
            }
        } else {
            self->mRow[i].angle += 0x40;
            if ((s16)self->mRow[i].angle >= 0x1000) {
                self->mRow[i].angle = 0x1000;
                self->mRow[i].dir++;
            }
        }
    }
}

// @symbol PachinkoDeco_ShowAll
void PachinkoDeco_ShowAll(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 3; i++) {
        if (self->mRow[i].active == 0) {
            self->mRow[i].active = 1;
            self->mRow[i].visible = 1;
            self->mRow[i].x = (data_ov006_0212eb44[i] << 12) - self->mBg[0].x;
            self->mRow[i].y = -self->mBg[0].y;
            self->mRow[i].unk12 = 0;
            self->mRow[i].angle = 0;
            self->mRow[i].dir = 0;
        }
    }
}

// @symbol PachinkoDeco_HideAll
void PachinkoDeco_HideAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 3; i++) {
        self->mRow[i].active = 0;
        self->mRow[i].visible = 0;
    }
}

// @symbol PachinkoBg_StateSinkOut
void PachinkoBg_StateSinkOut(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int y;
    self->mBg[i].x += self->mBg[i].vx;
    self->mBg[i].y += self->mBg[i].vy;
    if (self->mBg[i].vx <= -0x1000)
        self->mBg[i].vx += 0x40;
    if (self->mBg[i].vy >= 0x3000)
        self->mBg[i].vy += 0x100;
    y = self->mBg[i].y >> 12;
    SetBg2Offset(self->mBg[i].x >> 12, y);
    if (y < 0x40) return;
    self->mBg[i].timer = 0x258;
    self->mBg[i].state = 0;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    PachinkoDeco_HideAll(raw);
}

// @symbol PachinkoBg_StateScroll
void PachinkoBg_StateScroll(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int x;

    self->mBg[i].x += self->mBg[i].vx;
    if (self->mBg[i].vx <= -0x2000)
        self->mBg[i].vx -= 0x100;

    x = self->mBg[i].x >> 12;
    SetBg2Offset(x, self->mBg[i].y >> 12);

    if (x > -0x90) return;

    self->mBg[i].vy = 0x800;
    self->mBg[i].state = 3;
}

// @symbol PachinkoBg_StateRiseIn
void PachinkoBg_StateRiseIn(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int y;
    int x;

    self->mBg[i].x += self->mBg[i].vx;
    self->mBg[i].y += self->mBg[i].vy;
    if (self->mBg[i].vx <= -0x2000) self->mBg[i].vx -= 0x100;
    if (self->mBg[i].vy <= -0xc00) self->mBg[i].vy += 0x40;
    x = self->mBg[i].x >> 12;
    y = self->mBg[i].y >> 12;
    if (y <= 0) {
        y = 0;
        self->mBg[i].y = 0;
        self->mBg[i].vy = 0;
        self->mBg[i].state = 2;
    }
    SetBg2Offset(x, y);
}

// @symbol PachinkoBg_StateWaiting
void PachinkoBg_StateWaiting(char *raw, int idx) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int x, y;

    if (self->mPhase == 2)
        return;
    if (self->mBg[idx].timer != 0) {
        self->mBg[idx].timer--;
        return;
    }
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    self->mBg[idx].x = 0x100000;
    self->mBg[idx].y = 0x40000;
    self->mBg[idx].vx = -0x1000;
    self->mBg[idx].vy = -0x1000;
    x = self->mBg[idx].x >> 12;
    y = self->mBg[idx].y >> 12;
    self->mBg[idx].state = 1;
    *(volatile int *)0x4000018 = (0x1ff & x) | (0x1ff0000 & (y << 16));
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    self->mBg[0].active = 0;
}

// @symbol PachinkoTarget_DrawAll
void PachinkoTarget_DrawAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 4; i++) {
        if (self->mTarget[i].visible != 0) {
            Mg_RenderSpriteBoth((void *)sPachinkoTargetSprites[self->mTarget[i].sprite],
                                self->mTarget[i].x >> 0xc,
                                self->mTarget[i].y >> 0xc, -1, 1);
        }
    }
}

// @symbol PachinkoTarget_ShowAll
void PachinkoTarget_ShowAll(u8 *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 4; i++) {
        self->mTarget[i].active = 1;
        self->mTarget[i].visible = 1;
        self->mTarget[i].x = sPachinkoTargetX[i] << 12;
        self->mTarget[i].y = 0xa0000;
        self->mTarget[i].sprite = 0;
    }
}

// @symbol PachinkoTarget_HideAll
void PachinkoTarget_HideAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 4; i++) {
        self->mTarget[i].active = 0;
        self->mTarget[i].visible = 0;
    }
}

// @symbol PachinkoFlyer_Draw
void PachinkoFlyer_Draw(char* raw){
  dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
  if (self->mFlyer[0].visible == 0) return;
  int x = self->mFlyer[0].x;
  int y = self->mFlyer[0].y;
  Mg_RenderSpriteBoth((void *)sPachinkoFlyerSprites[self->mFlyer[0].facing], x >> 0xc,
                      y >> 0xc, -1, -1);
}

// @symbol PachinkoFlyer_CheckHits
void PachinkoFlyer_CheckHits(char *raw, int idx)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    int j;
    int dx;
    int dy;
    u8 state;

    if (self->mFlyer[idx].state == 0)
        return;

    for (i = 0; i < 0x30; i++) {
        if (self->mShot[i].active == 0)
            continue;
        if (self->mShot[i].state < 2)
            continue;
        dx = (self->mFlyer[idx].x - self->mShot[i].x) >> 12;
        dy = (self->mFlyer[idx].y - self->mShot[i].y) >> 12;
        if (cstd::sqrt(dx * dx + dy * dy) > 0x18)
            continue;

        self->mHitPause = 0x20;
        self->mShot[i].active = 0;
        self->mShot[i].visible = 0;
        PachinkoFx_Spawn(raw, self->mFlyer[idx].x, self->mFlyer[idx].y, 2, 0, 0);
        for (j = 0; j < 0x1e; j++) {
            if (self->mBall[j].active == 0)
                continue;
            state = self->mBall[j].state;
            if (state == 0)
                continue;
            if (state < 5) {
                self->mBall[j].state = 6;
                self->mBall[j].timer = 0x10;
            }
        }
        self->mFlyer[idx].visible = 0;
        self->mFlyer[idx].active = 0;
        self->mFlyerOut = 0;
        Sound::PlayBank2_2D(0x18a);
        return;
    }
}

// @symbol PachinkoFlyer_StateLeave
void PachinkoFlyer_StateLeave(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    self->mFlyer[i].sound = func_02012468(self->mFlyer[i].sound, 2, 0x189, 4, 0, 0,
                                          func_020126e8(self->mFlyer[i].x), 0);
    self->mFlyer[i].x += self->mFlyer[i].vx;
    if ((self->mFlyer[i].x >> 12) >= 0x110 || (self->mFlyer[i].x >> 12) <= -0x10) {
        self->mFlyer[i].active = 0;
        self->mFlyer[i].visible = 0;
        self->mFlyerOut = 0;
    }
}

// @symbol PachinkoFlyer_StatePause
void PachinkoFlyer_StatePause(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    if (self->mFlyer[i].delay != 0) {
        self->mFlyer[i].delay--;
        if ((s16)self->mFlyer[i].delay < 0)
            self->mFlyer[i].delay = 0;
        return;
    }
    self->mFlyer[i].state = 2;
    self->mFlyer[i].passes--;
}

// @symbol PachinkoFlyer_StateCross
void PachinkoFlyer_StateCross(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int x;
    u8 flag;
    self->mFlyer[i].sound = func_02012468(self->mFlyer[i].sound, 2, 0x189, 4, 0, 0,
                                          func_020126e8(self->mFlyer[i].x), 0);
    self->mFlyer[i].x += self->mFlyer[i].vx;
    flag = self->mFlyer[i].facing;
    x = self->mFlyer[i].x >> 12;
    if (flag != 0) {
        if (x >= 0xc0)
            self->mFlyer[i].vx -= 0x80;
        else if (self->mFlyer[i].vx <= 0x3000)
            self->mFlyer[i].vx += 0x200;
        if (x >= 0xf0) {
            if (self->mFlyer[i].passes == 0) {
                self->mFlyer[i].state = 4;
                return;
            }
            self->mFlyer[i].x = 0xf0000;
            self->mFlyer[i].vx = 0;
            self->mFlyer[i].facing ^= 1;
            self->mFlyer[i].delay = PACHINKO_RAND(8) * 2 + 0x10;
            self->mFlyer[i].state = 3;
            return;
        }
    } else {
        if (x <= 0x40)
            self->mFlyer[i].vx += 0x80;
        else if (self->mFlyer[i].vx >= -0x3000)
            self->mFlyer[i].vx -= 0x200;
        if (x <= 0x10) {
            if (self->mFlyer[i].passes == 0) {
                self->mFlyer[i].state = 4;
                return;
            }
            self->mFlyer[i].x = 0x10000;
            self->mFlyer[i].vx = 0;
            self->mFlyer[i].facing ^= 1;
            self->mFlyer[i].delay = PACHINKO_RAND(8) * 2 + 0x10;
            self->mFlyer[i].state = 3;
            return;
        }
    }
}

// @symbol PachinkoFlyer_StateDescend
/* Moves flyer i down: y += vy. While y is still below -0xc0 it slows
   (vy -= 0x60); once it reaches -0xc0 it snaps to -0xc0000 and stops.
   Then hands on to PachinkoFlyer_StateCross. */
void PachinkoFlyer_StateDescend(dScMgPachinko_c *self, int i) {
    self->mFlyer[i].y += self->mFlyer[i].vy;
    if (self->mFlyer[i].y >> 12 >= -0xc0) {
        self->mFlyer[i].y = -0xc0000;
        self->mFlyer[i].vy = 0;
    } else {
        self->mFlyer[i].vy -= 0x60;
    }
    PachinkoFlyer_StateCross((char *)self, i);
}

// @symbol PachinkoFlyer_StateWaiting
void PachinkoFlyer_StateWaiting(char *raw, int idx) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    unsigned int side;
    if (self->mFlyer[idx].delay != 0) {
        self->mFlyer[idx].delay--;
        if ((s16)self->mFlyer[idx].delay < 0)
            self->mFlyer[idx].delay = 0;
        return;
    }
    Sound::PlayBank2_2D(0x188);
    self->mFlyer[idx].state = 1;
    side = PACHINKO_RAND(2);
    self->mFlyer[idx].facing = side;
    if (side != 0) {
        self->mFlyer[idx].visible = 1;
        self->mFlyer[idx].x = -0x10000;
        self->mFlyer[idx].vx = 0x400;
    } else {
        self->mFlyer[idx].visible = 1;
        self->mFlyer[idx].x = 0x110000;
        self->mFlyer[idx].vx = -0x400;
    }
    self->mFlyer[idx].y = -0xf8000;
    self->mFlyer[idx].passes = PACHINKO_RAND(3) + 2;
    self->mFlyer[idx].vy = 0x2000;
}

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
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    if (self->mFlyerOut != 0) {
        return;
    }
    self->mFlyer[0].active = 1;
    self->mFlyer[0].state = 0;
    self->mFlyer[0].sound = 0;
    self->mFlyer[0].delay = (PACHINKO_RAND(0x20) << 3) + 0x200;
    targets = 0;
    for (i = 0; i < 4; i++) {
        if (self->mTarget[i].active != 0) {
            targets++;
        }
    }

    if (targets == 1) {
        self->mFlyer[0].delay = (PACHINKO_RAND(0x20) << 2) + 0x200;
    }
    self->mFlyerOut++;
}

// @symbol PachinkoFx_DrawAll
void PachinkoFx_DrawAll(void* raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 30; i++) {
        if (self->mFx[i].visible) {
            int type = self->mFx[i].kind;
            int x = self->mFx[i].x >> 12;
            int y = self->mFx[i].y >> 12;
            int idx = self->mFx[i].frame;
            if (type != 2) {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesA[idx], x, y, -1, -1);
            } else {
                Mg_RenderSpriteBoth((void *)sPachinkoFxSpritesB[idx], x, y, -1, -1);
            }
        }
    }
}

// @symbol PachinkoFx_UpdateAll
void PachinkoFx_UpdateAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    int lastFrame;
    u8 target;

    for (i = 0; i < 0x1e; i++) {
        if (self->mFx[i].active == 0)
            continue;
        self->mFx[i].timer++;
        if (self->mFx[i].timer < 6)
            continue;
        self->mFx[i].timer = 0;
        self->mFx[i].frame++;
        lastFrame = 3;
        if (self->mFx[i].kind == 2)
            lastFrame = 5;
        target = self->mFx[i].target;
        if (target != 0 && lastFrame - 2 == self->mFx[i].frame) {
            self->mTarget[target - 1].active = 0;
            self->mTarget[target - 1].sprite = 1;
        }
        if (self->mFx[i].frame >= lastFrame) {
            self->mFx[i].visible = 0;
            self->mFx[i].active = 0;
            if (self->mFx[i].points != 0)
                PachinkoPopup_SpawnScore(raw, i, self->mFx[i].points);
        }
    }
}

// @symbol PachinkoFx_Spawn
void PachinkoFx_Spawn(char *raw, int x, int y, int kind, int points, int target)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (self->mFx[i].active == 0) {
            self->mFx[i].x = x;
            self->mFx[i].y = y;
            self->mFx[i].active = 1;
            self->mFx[i].visible = 1;
            self->mFx[i].timer = 0;
            self->mFx[i].frame = 0;
            self->mFx[i].kind = kind;
            self->mFx[i].points = points;
            self->mFx[i].target = target;
            return;
        }
    }
}

// @symbol Pachinko_CheckTargetsLeft
void Pachinko_CheckTargetsLeft(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    volatile unsigned short zero;
    int count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (self->mTarget[i].active != 0) {
            count++;
            break;
        }
    }
    if (count != 0)
        return;
    self->mPhase = 2;
    self->mEndTimer = 0x40;
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (self->mPopup[i].timer != 0)
            Mg_DrawBigNumber(self->mPopup[i].x >> 12, self->mPopup[i].y >> 12,
                             self->mPopup[i].points, -1, -1, 0, 0);
    }
}

// @symbol PachinkoPopup_UpdateAll
void PachinkoPopup_UpdateAll(char *popup) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)popup;
    int i;
    for (i = 0; i < 0x1e; i++) {
        if (self->mPopup[i].timer != 0) {
            self->mPopup[i].timer--;
            if ((s16)self->mPopup[i].timer < 0)
                self->mPopup[i].timer = 0;
        }
    }
}

// @symbol PachinkoPopup_SpawnScore
void PachinkoPopup_SpawnScore(char *raw, int idx, unsigned short points)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0x1d; i >= 0; i--) {
        if (self->mPopup[i].timer != 0)
            continue;
        self->mPopup[i].x = self->mFx[idx].x;
        self->mPopup[i].y = self->mFx[idx].y;
        self->mPopup[i].timer = 0x30;
        self->mPopup[i].points = points;
        Mg_SubmitScore(self->mPopup[i].points + Mg_GetSubmittedScore());
        if ((unsigned int)Mg_GetSubmittedScore() < 0xbb8) return;
        if (self->mPromptEnabled != 0)
            self->mPromptEnabled = 0;
        return;
    }
}

// @symbol Pachinko_ClearAimLine
void Pachinko_ClearAimLine(char* raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    volatile unsigned short zero;
    int shotNo = self->mAimShot;
    int dx, dy;

    if (shotNo == 0) return;

    dx = 0x80 - (self->mShot[shotNo - 1].x >> 12);
    dy = 0x20 - (self->mShot[shotNo - 1].y >> 12);

    if (dx < -6) return;
    if (dx > 6) return;
    if (dy < -6) return;
    if (dy > 6) return;
    if (self->mShot[shotNo - 1].state != 2) return;

    self->mAimShot = 0;
    {
        char *dst = (char *)G2S::GetBG2CharPtr();
        zero = 0;
        MultiStore16(zero, dst, 0x6000);
    }
}

#pragma push
#pragma opt_loop_invariants off
// @symbol Pachinko_PlotSubBg2Pixels
/* Sets a 2x2 block of pixels in the sub BG2 character data. The first
   parameter is never read.
   opt_loop_invariants stays off: the ROM keeps j on the stack and every spelling without these staged locals is a word short (0x7c of 0x80). */
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

// @symbol Pachinko_DrawAimLine
void Pachinko_DrawAimLine(void *arg0)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)arg0;
    int i;
    int idx;
    int tx;
    int ty;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    volatile u16 fill;

    if (self->mAimShot == 0)
        return;
    idx = self->mAimShot - 1;
    if (self->mShot[idx].x != self->mShot[idx].prevX ||
        self->mShot[idx].y != self->mShot[idx].prevY) {
        char *buf = (char *)G2S::GetBG2CharPtr();
        fill = 0;
        MultiStore16(fill, buf, 0x6000);
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = 0x6c;
                tx = self->mShot[idx].x >> 12;
                cy = 0x22;
                ty = self->mShot[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx -= 4;
            } else {
                cx = 0x94;
                tx = self->mShot[idx].x >> 12;
                cy = 0x22;
                ty = self->mShot[idx].y >> 12;
                if (ty >= 0x22)
                    ty += 4;
                else
                    ty -= 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                self->mLineErr = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        self->mLineErr += ady;
                        if (self->mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            self->mLineErr -= adx;
                        }
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        self->mLineErr += ady;
                        if (self->mLineErr > adx) {
                            if (dy >= 0) cy++; else cy--;
                            self->mLineErr -= adx;
                        }
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                self->mLineErr = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        self->mLineErr += adx;
                        if (self->mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            self->mLineErr -= ady;
                        }
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        self->mLineErr += adx;
                        if (self->mLineErr > ady) {
                            if (dx >= 0) cx++; else cx--;
                            self->mLineErr -= ady;
                        }
                        Pachinko_PlotSubBg2Pixels(arg0, cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    self->mShot[idx].prevX = self->mShot[idx].x;
    self->mShot[idx].prevY = self->mShot[idx].y;
}

// @symbol PachinkoPipe_DrawAll
void PachinkoPipe_DrawAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 2; i++) {
        if (self->mPipe[i].visible != 0)
            Mg_RenderSpriteBoth((void *)sPachinkoPipeSprites[self->mPipe[i].sprite],
                                self->mPipe[i].x >> 12, self->mPipe[i].y >> 12, -1, 2);
    }
}

// @symbol PachinkoPipe_SetAll
void PachinkoPipe_SetAll(char *raw, int phase) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 2; i++) {
        if (self->mPipe[i].active) {
            self->mPipe[i].state = 3;
            self->mPipe[i].laps = 0;
            self->mPipe[i].phase = phase;
        }
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int x;

    if (self->mPipe[i].phase != 0) {
        self->mPipe[i].timer++;
        if (self->mPipe[i].timer < 4)
            return;
        self->mPipe[i].timer = 0;
        self->mPipe[i].frame++;
        if (self->mPipe[i].frame >= 3) {
            self->mPipe[i].frame = 0;
            self->mPipe[i].state = 1;
            self->mPipe[i].phase = 0;
            return;
        }
        self->mPipe[i].sprite = sPachinkoPipeSinkStartFrames[self->mPipe[i].frame];
        return;
    }

    self->mPipe[i].timer++;
    if (self->mPipe[i].timer >= 4) {
        self->mPipe[i].timer = 0;
        self->mPipe[i].frame++;
        if (self->mPipe[i].frame >= 6)
            self->mPipe[i].frame = 0;
        self->mPipe[i].sprite = sPachinkoPipeSinkFrames[self->mPipe[i].frame];
    }

    self->mPipe[i].x += self->mPipe[i].vel;
    x = self->mPipe[i].x >> 12;
    if (x >= sPachinkoPipeSinkStops[i + 2]) {
        if (self->mPipe[i].vel >= 0x800)
            self->mPipe[i].vel -= 0x80;
    } else {
        self->mPipe[i].vel += 0x80;
    }
    if (x < sPachinkoPipeSinkStops[i])
        return;

    self->mPipe[i].x = sPachinkoPipeSinkStops[i] << 12;
    self->mPipe[i].vel = -0x1000;
    self->mPipe[i].phase++;
    self->mPipe[i].timer = 0;
    self->mPipe[i].frame = 0;
    self->mPipe[i].sprite = sPachinkoPipeSinkStartFrames[0];
}

// @symbol PachinkoPipe_StateRise
void PachinkoPipe_StateRise(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
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

// @symbol PachinkoPipe_StatePick
/* Rolls a random kind for pipe n from sPachinkoPipeKinds and resets it to
   rise (or, for kind 2, sink). */
void PachinkoPipe_StatePick(char *raw, int n) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int rnd = RandomIntInternal(&data_0209d4b8.value);
    int kind = sPachinkoPipeKinds[((((unsigned)rnd >> 16) & 0x7fff) * 2) >> 15];
    self->mPipe[n].state = kind;
    self->mPipe[n].phase = 0;
    if (self->mPipe[n].state == 2) {
        self->mPipe[n].sinking = 1;
        self->mPipe[n].vel = 0x1000;
    } else {
        self->mPipe[n].sinking = 0;
        self->mPipe[n].vel = -0x1000;
    }
    self->mPipe[n].timer = 0;
    self->mPipe[n].frame = 0;
    self->mPipe[n].visible = 1;
}

// @symbol PachinkoPipe_UpdateAll
/* Walks the 8-byte {code pointer, adjustment} records at 0x02142624; an odd
   adjustment selects the virtual spelling through the receiver's own vtable. */
void PachinkoPipe_UpdateAll(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 2; i++) {
        if (self->mPipe[i].active != 0) {
            Ent7d0 *rec = &sPachinkoPipeStateTable[self->mPipe[i].state];
            int adj = rec->b;
            char *obj = raw + (adj >> 1);
            int fn;
            if (adj & 1)
                fn = *(int *)(*(int *)obj + rec->a);
            else
                fn = rec->a;
            ((void (*)(void *, int))fn)(obj, i);
        }
    }
}

// @symbol PachinkoPipe_ResetAll
void PachinkoPipe_ResetAll(unsigned char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 2; i++) {
        self->mPipe[i].active = 1;
        self->mPipe[i].visible = 0;
        self->mPipe[i].state = 0;
        self->mPipe[i].phase = 0;
        self->mPipe[i].sprite = 0;
        self->mPipe[i].timer = 0;
        self->mPipe[i].x = data_ov006_0212eb24[i] << 12;
        self->mPipe[i].y = 0x68000;
        self->mPipe[i].vel = 0;
        self->mPipe[i].velY = 0;
        self->mPipe[i].sinking = 0;
        self->mPipe[i].laps = 0;
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)scene;
    int i;
    for (i = 0; i < 30; i++) {
        if (self->mBall[i].visible != 0) {
            int idx = (self->mBall[i].rotation >> 4) << 1;
            int scale = self->mBall[i].scale;
            int sprite = self->mBall[i].sprite;
            Matrix2x2 m;
            int cosv = FX_Mul(data_02082214[idx + 1], scale);
            int sinv = FX_Mul(data_02082214[idx], scale);
            int x = (self->mBall[i].x + self->mBall[i].offX) >> 12;
            int y = (self->mBall[i].y + self->mBall[i].offY) >> 12;
            m.m00 = cosv;
            m.m01 = sinv;
            m.m10 = -sinv;
            m.m11 = cosv;
            Mg_RenderSpriteBothPalMtx((void *)sPachinkoBallSprites[sprite], x, y, -1, &m);
        }
    }
}
#pragma pop

// @symbol PachinkoBall_CountNearby
void PachinkoBall_CountNearby(char *base, int i)
{
    int x, y, cnt;

    dScMgPachinko_c *self = (dScMgPachinko_c *)base;
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

// @symbol PachinkoBall_CheckPipes
void PachinkoBall_CheckPipes(char *c, int idx)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    int i;
    int dx;
    int dy;

    if (self->mBall[idx].state == 5)
        return;

    for (i = 0; i < 4; i++) {
        if (self->mTarget[i].active == 0)
            continue;

        dx = (self->mTarget[i].x - self->mBall[idx].x) >> 12;
        dy = (self->mTarget[i].y - self->mBall[idx].y) >> 12;
        if (dx < -0x10 || dx > 0x18 || dy < -0x10 || dy > 0x18)
            continue;

        self->mBall[idx].active = 0;
        self->mBall[idx].visible = 0;
        PachinkoFx_Spawn(c, self->mTarget[i].x, self->mTarget[i].y, 2, 0, i + 1);
        PachinkoPipe_SetAll(c, 0);
        func_02012718(0x18c, self->mBall[idx].x);
        return;
    }
}

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
 * order is what fixes it.
 * opt_common_subs stays off: typed mBall[i] spellings land at 0x1fc-0x260 of 0x240 with or without it, so the field-base forms remain. */
void PachinkoBall_AimAtTarget(char *base, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)base;
    int idx;
    int n;
    int factor;
    int dist;
    int *pY;
    int *pVx;
    int *pVy;
    char *angBase;
    s16 angle;
    int count = 0, j;
    for (j = 0; j < 4; j++) {
        if (self->mTarget[j].active != 0) {
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
    idx = PACHINKO_RAND(4);
    for (;;) {
        if (self->mTarget[idx].active != 0)
            break;
        idx = PACHINKO_RAND(4);
    }
    {
        char *xBase = base + BALL_OFF(x);
        int targetY = self->mTarget[idx].y;
        int yval = *pY;
        int targetX = self->mTarget[idx].x;
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)thiz;
    if (self->mBall[idx].fallPlayed != 0) return;
    if ((self->mBall[idx].y >> 0xc) < -0xf2)
        return;
    self->mBall[idx].fallPlayed++;
    func_02012718(0x184, self->mBall[idx].x);
}

// @symbol PachinkoBall_StatePop
void PachinkoBall_StatePop(char *c, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    if (self->mBall[i].timer != 0) {
        self->mBall[i].timer--;
        if (self->mBall[i].timer != 0)
            return;
        PachinkoFx_Spawn(c, self->mBall[i].x, self->mBall[i].y, 2, sPachinkoPopScores[0], 0);
        func_02012718(0x18b, self->mBall[i].x);
        return;
    }
    self->mBall[i].active = 0;
    self->mBall[i].visible = 0;
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
void PachinkoBall_StateGlide(char *raw, int idx) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    u16 target;

    self->mBall[idx].x += self->mBall[idx].velX;
    self->mBall[idx].y += self->mBall[idx].velY;
    target = self->mBall[idx].angle - 0x4000;
    if ((s16)self->mBall[idx].rotation > (s16)target) {
        self->mBall[idx].rotation -= 0x100;
        if ((s16)self->mBall[idx].rotation <= (s16)target)
            self->mBall[idx].rotation = target;
    } else if ((s16)self->mBall[idx].rotation < (s16)target) {
        self->mBall[idx].rotation += 0x100;
        if ((s16)self->mBall[idx].rotation >= (s16)target)
            self->mBall[idx].rotation = target;
    }
    PachinkoBall_CheckHitByBobOmb(raw, idx);
    PachinkoBall_CheckPipes(raw, idx);
    PachinkoBall_CountNearby(raw, idx);
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
    PachinkoBall_CheckFall(o, i);
    if (self->mBall[i].started == 0) {
        self->mBall[i].started++;
        if (self->mWaveLevel > 0xc)
            self->mBall[i].velY = ((PACHINKO_RAND(8)) << 7) + 0xb80 + ((self->mWaveLevel - 0xc) << 7);
        else
            self->mBall[i].velY = self->mWaveLevel * 0xa0 + (((PACHINKO_RAND(8)) << 7) + 0x400);
        if (PACHINKO_RAND(2) != 0) {
            self->mBall[i].velX = (self->mWaveLevel << 7) + 0x600;
            self->mBall[i].driftDir = 0;
        } else {
            self->mBall[i].velX = -((self->mWaveLevel << 7) + 0x600);
            self->mBall[i].driftDir = 0;
        }
        self->mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x20;
    } else {
        int x;
        self->mBall[i].y += self->mBall[i].velY;
        self->mBall[i].x += self->mBall[i].velX;
        x = self->mBall[i].x >> 12;
        if (x > 0xf0) {
            self->mBall[i].velX = -self->mBall[i].velX;
            if (self->mBall[i].driftDir != 0) {
                if (self->mBall[i].driftDir == 1)
                    self->mBall[i].driftDir = 2;
                else
                    self->mBall[i].driftDir = 1;
            }
            self->mBall[i].x = 0xf0000;
        }
        if (x < 0x10) {
            self->mBall[i].velX = -self->mBall[i].velX;
            if (self->mBall[i].driftDir != 0) {
                if (self->mBall[i].driftDir == 1)
                    self->mBall[i].driftDir = 2;
                else
                    self->mBall[i].driftDir = 1;
            }
            self->mBall[i].x = 0x10000;
        }
        PachinkoBall_CheckHitByBobOmb(o, i);
        PachinkoBall_CheckPipes(o, i);
        PachinkoBall_CountNearby(o, i);
        if (self->mBall[i].timer != 0) {
            self->mBall[i].timer--;
            if (self->mBall[i].velX > 0) {
                self->mBall[i].rotation -= 0x80;
                if ((s16)self->mBall[i].rotation <= -0x1800)
                    self->mBall[i].rotation = -0x1800;
            } else if (self->mBall[i].velX < 0) {
                self->mBall[i].rotation += 0x80;
                if ((s16)self->mBall[i].rotation >= 0x1800)
                    self->mBall[i].rotation = 0x1800;
            }
            if ((s16)self->mBall[i].timer < 0)
                self->mBall[i].timer = 0;
            return;
        }
        if (self->mBall[i].driftDir == 0) {
            if (self->mBall[i].velX > 0) {
                self->mBall[i].velX -= 0x20;
                self->mBall[i].rotation += 0x80;
                if ((s16)self->mBall[i].rotation >= 0)
                    self->mBall[i].rotation = 0;
                if (self->mBall[i].velX <= 0) {
                    self->mBall[i].driftDir = PACHINKO_RAND(2) + 1;
                    self->mBall[i].timer = PACHINKO_RAND(0x10) << 2;
                    self->mBall[i].rotation = 0;
                }
            } else if (self->mBall[i].velX < 0) {
                self->mBall[i].velX += 0x20;
                self->mBall[i].rotation -= 0x80;
                if ((s16)self->mBall[i].rotation < 0)
                    self->mBall[i].rotation = 0;
                if (self->mBall[i].velX >= 0) {
                    self->mBall[i].driftDir = PACHINKO_RAND(2) + 1;
                    self->mBall[i].timer = PACHINKO_RAND(0x10) << 2;
                    self->mBall[i].rotation = 0;
                }
            }
        } else if (self->mBall[i].driftDir == 1) {
            self->mBall[i].velX += 0x20;
            self->mBall[i].rotation -= 0x80;
            if ((s16)self->mBall[i].rotation <= -0x1800)
                self->mBall[i].rotation = -0x1800;
            if (self->mBall[i].velX >= (self->mWaveLevel << 7) + 0x600) {
                self->mBall[i].driftDir = 0;
                self->mBall[i].velX = (self->mWaveLevel << 7) + 0x600;
                self->mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x10;
            }
        } else {
            self->mBall[i].velX -= 0x20;
            self->mBall[i].rotation += 0x80;
            if ((s16)self->mBall[i].rotation >= 0x1800)
                self->mBall[i].rotation = 0x1800;
            if (self->mBall[i].velX <= -((self->mWaveLevel << 7) + 0x600)) {
                self->mBall[i].driftDir = 0;
                self->mBall[i].velX = -((self->mWaveLevel << 7) + 0x600);
                self->mBall[i].timer = (PACHINKO_RAND(0x20) << 3) + 0x10;
            }
        }
    }
    PachinkoBall_AimAtTarget(o, i);
}

// @symbol PachinkoBall_StateFall
void PachinkoBall_StateFall(char *o, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)o;
    if (self->mBall[i].started == 0) {
        self->mBall[i].started++;
        if (self->mWaveLevel > 0xc) {
            self->mBall[i].velY = (PACHINKO_RAND(8) << 7) + 0xb80 + ((self->mWaveLevel - 0xc) << 7);
        } else {
            self->mBall[i].velY = self->mWaveLevel * 0xa0 + ((PACHINKO_RAND(8) << 7) + 0x400);
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)base;
    if (self->mBall[idx].timer != 0) {
        self->mBall[idx].timer--;
        return;
    }
    self->mBall[idx].state = PACHINKO_RAND(2) + 1;
    self->mBall[idx].started = 0;
    self->mBall[idx].visible = 1;
    self->mBall[idx].sprite = 0;
    self->mBall[idx].rotation = 0;
    self->mBall[idx].spinRadius = 0;
    self->mBall[idx].offX = 0;
    self->mBall[idx].offY = 0;
    self->mBall[idx].scale = 0x1000;
    self->mBall[idx].fallPlayed = 0;
}

// @symbol PachinkoBall_UpdateAll
void PachinkoBall_UpdateAll(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 30; i++) {
        if (self->mBall[i].active != 0) {
            Ent7d0 *rec = &sPachinkoBallStateTable[self->mBall[i].state];
            int adj = rec->b;
            char *obj = raw + (adj >> 1);
            int fn;
            if (adj & 1)
                fn = *(int *)(*(int *)obj + rec->a);
            else
                fn = rec->a;
            ((void (*)(void *, int))fn)(obj, i);
        }
    }
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
        ra = RandomIntInternal(&data_0209d4b8.value);
        rb = RandomIntInternal(&data_0209d4b8.value);
        self->mBall[i].x = (sPachinkoWaveColumnX[(((u32)rb >> 16) & 0x7fff) * 5 >> 15] +
                            ((((u32)ra >> 16) & 0x7fff) * 4 >> 15) * 8)
                           << 12;
        self->mBall[i].y = -0x100000;
        self->mBall[i].velX = 0;
        self->mBall[i].velY = 0;
        self->mBall[i].state = 0;
        self->mBall[i].started = 0;
        ra = RandomIntInternal(&data_0209d4b8.value);
        self->mBall[i].timer = ((((u32)ra >> 16) & 0x7fff) * 8 >> 15) << 4;
        self->mBall[i].rotation = 0;
        self->mBall[i].sound = 0;
        self->mBall[i].level = self->mWaveLevel;
        self->mWaveSpawns++;
        if (self->mWaveLevel == 0)
            break;
        {
            int r = PACHINKO_RAND(16);
            int lvl = self->mWaveLevel >> 1;
            if (lvl >= 4) lvl = 4;
            if (sPachinkoWaveSlots[r + lvl * 4] == 0)
                break;
        }
        if (PACHINKO_RAND(2)) {
            self->mBall[i].x = 0x100000;
            self->mBall[i].angle = 0xc000 - ((PACHINKO_RAND(12) + 1) << 9);
        } else {
            self->mBall[i].x = 0;
            self->mBall[i].angle = ((PACHINKO_RAND(12) + 1) << 9) + 0xc000;
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
void PachinkoBobOmb_ClearAll(char *raw)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 0x30; i++) {
        self->mShot[i].active = 0;
        self->mShot[i].visible = 0;
    }
}

// @symbol PachinkoBobOmb_DrawAll
void PachinkoBobOmb_DrawAll(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (self->mShot[i].visible == 0)
            continue;
        Mg_RenderSpriteBothScaled((OamAttr *)sPachinkoBobOmbSprites, self->mShot[i].x >> 12,
                                  self->mShot[i].y >> 12, -1, -1, 0x1000, 0);
    }
}

// @symbol PachinkoBobOmb_UpdateAll
void PachinkoBobOmb_UpdateAll(char *raw) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int i;
    for (i = 0; i < 48; i++) {
        if (self->mShot[i].active != 0) {
            Ent7d0 *rec = &sPachinkoBobOmbStateTable[self->mShot[i].state];
            int adj = rec->b;
            char *obj = raw + (adj >> 1);
            int fn;
            if (adj & 1)
                fn = *(int *)(*(int *)obj + rec->a);
            else
                fn = rec->a;
            ((void (*)(void *, int))fn)(obj, i);
        }
    }
}

// @symbol Pachinko_CountLaunch
void Pachinko_CountLaunch(char *c)
{
    dScMgPachinko_c *self = (dScMgPachinko_c *)c;
    if (self->mPlayStarted == 0)
        self->mPlayStarted += 1;
}

// @symbol PachinkoBobOmb_StateWaitGrab
void PachinkoBobOmb_StateWaitGrab(char *raw, int i) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    unsigned int slot = gActivePlayerSlot;
    int grabbed = 0;
    int dx, dy;

    if (gTouchHeld[slot][0] != 0)
        grabbed = gTouchEdge[slot][0] != 0;
    if (grabbed != 0) {
        dx = (self->mShot[i].x >> 12) - gTouchX[slot][0];
        dy = (self->mShot[i].y >> 12) - gTouchY[slot][0];
        self->mShot[i].state = 1;
        self->mShot[i].offX = dx << 12;
        self->mShot[i].offY = dy << 12;
        self->mShot[i].combo = 0;
    }
    Pachinko_DrawAimLine(raw);
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
 * opt_propagation stays: a typed mShot[i] rewrite without it is the right size but schedules the touch loads and clamps differently (88 words).
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
            s32 dist = cstd::sqrt((dy * dy + dx * dx));
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
        dist = cstd::sqrt((dy * dy + dx * dx));
        if (dist >= 0x10) {
            {
                s16 ang = cstd::atan2(dx, dy);
                char *p = c + off;

                *(s16 *)(p + SHOT_OFF(angle)) = ang;
            }
            *(s32 *)(c + SHOT_OFF(speed) + off) = cstd::sqrt((dy * dy + dx * dx)) << 8;
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
void PachinkoBobOmb_StateFlying(char *raw, int idx) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)raw;
    int old;
    int x, y;

    self->mShot[idx].x += self->mShot[idx].velX;
    self->mShot[idx].y += self->mShot[idx].velY;
    self->mShot[idx].gravity += 0x10;
    old = self->mShot[idx].velY;
    self->mShot[idx].velY = old + self->mShot[idx].gravity;
    if (self->mShot[idx].velY >= 0x8000)
        self->mShot[idx].velY = 0x8000;

    self->mShot[idx].angle = cstd::atan2(self->mShot[idx].velY >> 12, self->mShot[idx].velX >> 12);

    if (old < 0 && self->mShot[idx].velY >= 0)
        self->mShot[idx].timer = 0x40;

    if (self->mShot[idx].timer != 0) {
        self->mShot[idx].timer--;
        if ((s16)self->mShot[idx].timer <= 0)
            self->mShot[idx].state = 4;
    }

    x = self->mShot[idx].x >> 12;
    y = self->mShot[idx].y >> 12;
    if (y <= -0x120)
        self->mShot[idx].state = 4;
    if (y >= 0xd0)
        self->mShot[idx].state = 4;
    if (x >= 0x140 || x <= -0x40)
        self->mShot[idx].state = 4;

    Pachinko_ClearAimLine(raw);
    if (self->mAimShot != 0)
        Pachinko_DrawAimLine(raw);
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
    dScMgPachinko_c *self = (dScMgPachinko_c *)p;
    self->mShot[idx].active = 0;
    self->mShot[idx].visible = 0;
}

// @symbol Pachinko_UpdateNextBall
void Pachinko_UpdateNextBall(char *o) {
    dScMgPachinko_c *self = (dScMgPachinko_c *)o;
    int i;

    if (self->mNextBallDelay == 0)
        return;
    self->mNextBallDelay--;
    if ((s16)self->mNextBallDelay > 0)
        return;
    self->mNextBallDelay = 0;

    for (i = 0; i < 0x30; i++) {
        if (self->mShot[i].active == 0) {
            self->mShot[i].active = 1;
            self->mShot[i].visible = 1;
            self->mShot[i].state = 0;
            self->mShot[i].gravity = 0;
            self->mShot[i].x = 0x80000;
            self->mShot[i].y = 0x28000;
            self->mShot[i].offX = 0;
            self->mShot[i].offY = 0;
            self->mShot[i].combo = 0;
            self->mShot[i].velX = 0;
            self->mShot[i].velY = 0;
            self->mShot[i].timer = 0;
            self->mShot[i].sound = 0;
            self->mShot[i].dist = 4;
            self->mAimShot = i + 1;
            if (self->mBallsServed != 0) {
                Sound::PlayBank2_2D(0x18d);
                return;
            }
            self->mBallsServed++;
            return;
        }
    }
}

// @symbol Pachinko_ResetRound
void Pachinko_ResetRound(char *self)
{
    dScMgPachinko_c *scene = (dScMgPachinko_c *)self;
    int i, j, k, l, m, n;

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

    for (k = 0; k < 2; k++) {
        scene->mPipe[k].x = 0;
        scene->mPipe[k].y = 0;
        scene->mPipe[k].vel = 0;
        scene->mPipe[k].velY = 0;
        scene->mPipe[k].timer = 0;
        scene->mPipe[k].sprite = 0;
        scene->mPipe[k].state = 0;
        scene->mPipe[k].pad12 = 0; /* a byte nothing else touches */
        scene->mPipe[k].active = 0;
        scene->mPipe[k].visible = 0;
    }

    for (l = 0; l < 0x30; l++) {
        scene->mShot[l].x = 0;
        scene->mShot[l].y = 0;
        scene->mShot[l].prevX = 0;
        scene->mShot[l].prevY = 0;
        scene->mShot[l].speed = 0;
        scene->mShot[l].gravity = 0;
        scene->mShot[l].angle = 0;
        scene->mShot[l].timer = 0;
        scene->mShot[l].active = 0;
        scene->mShot[l].state = 0;
        scene->mShot[l].visible = 0;
    }

    for (m = 0; m < 30; m++) {
        scene->mBall[m].active = 0;
        scene->mBall[m].visible = 0;
        scene->mBall[m].state = 0;
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
    void *objChar;
    void *objPltt;
    void *f;
    int n;
    int y;
    int x;

    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x5f);
    DecompressLZ16(f, (void *)func_02054d88());
    Deallocate(f);

    f = LoadFile(0x60);
    GX::LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x61);
    func_02056314(f, 0, 0x800);
    Deallocate(f);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5418;

    f = LoadFile(0x46);
    DecompressLZ16(f, G2::GetBG2CharPtr());
    Deallocate(f);

    b = (char *)G2::GetBG2ScrPtr();
    fillScr = 0x3073;
    MultiStore16(fillScr, b, 0x1000);

    f = LoadFile(6);
    func_020563d4(f, 0, 0x800);
    Deallocate(f);

    objChar = LoadFile(0xe0);
    objPltt = LoadFile(0xe1);
    DecompressLZ16(objChar, (void *)0x6400000);
    GX::LoadOBJPltt(objPltt, 0, 0x100);

    gSubScreenLayers |= 0xd;
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
    DecompressLZ16(f, (void *)G2S::GetBG3CharPtr());
    Deallocate(f);

    f = LoadFile(0x5d);
    GXS::LoadBGPltt(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x5e);
    func_020562b4(f, 0, 0x800);
    Deallocate(f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x608;

    f = LoadFile(5);
    func_020564f4(f, 0, 0x800);
    Deallocate(f);

    DecompressLZ16(objChar, (void *)0x6600000);
    GXS::LoadOBJPltt(objPltt, 0, 0x100);
    Deallocate(objChar);
    Deallocate(objPltt);

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
