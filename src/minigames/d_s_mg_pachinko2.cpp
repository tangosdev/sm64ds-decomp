//cpp
/* Tamaire (scene 0x171, profile MG_TAMAIRE): dScMgPachinko2_c, the whole
 * class in one translation unit. 74 functions, .text 0x020ff420..0x0210428c:
 * the destructor pair, 67 members, StartRound, Render, Behavior,
 * InitResources and the factory.
 *
 * The destructor is declared first in the class header, so this TU owns the
 * key function and emits dScMgPachinko2_c's vtable and RTTI; the ROM's own
 * copies live in ov006 .data, and these are discarded at link time.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder. Each optimisation pragma sits in its
 * own push/pop bracket around the one member that needs it.
 *
 * The scene's three PMF state tables dispatch on this class: cups at
 * sPachinko2CupStateTable (5 states on the two 0x20 records at 0x5620), balls at
 * sPachinko2BallStateTable (8 states on mBalls[]), paddles at
 * sPachinko2PaddleStateTable (15 states on the three 0x40 records at 0x5260). The
 * descriptor records they are built from live at data_ov006_0213da*; the
 * runtime copies are made by __sinit_ov006_02131cd0.
 *
 * leftovers:
 * - The helpers still address the scene through byte offsets and the local
 *   view structs below. mBalls[0x40] is the typed name for 0x4660..0x565f,
 *   but only balls 0..0x2f are live balls: the tail overlaps three 0x40-byte
 *   paddle records at 0x5260, two 16 x 0x18 sprite records at 0x5320/0x54a0
 *   and the two 0x20-byte cup records at 0x5620. Naming those sub-records as
 *   members needs a union or punned views and is deferred; receivers stay as
 *   `char *c = (char *)this` locals because the offset arithmetic is what
 *   the matching bytes already prove.
 * - PlotSubBg2Pixels keeps an unused receiver: every call passes the
 *   scene in r0 and the body only uses (a1, a2, a3). Memberized with the
 *   others for the uniform call shape.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgPachinko2_c_AssetId {
    ASSET_D_2D_HEI_TOP_DOWN_NSC_BIN = 0x0005,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCG_BIN = 0x005c,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCL_BIN = 0x005d,
    ASSET_D_2D_MG_BG_PACHINKO_DOWN_NSC_BIN = 0x005e,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NCG_BIN = 0x005f,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NCL_BIN = 0x0060,
    ASSET_D_2D_MG_BG_PACHINKO_UP_NSC_BIN = 0x0061,
    ASSET_D_2D_MG_PACHINKO_NCG_BIN = 0x00e0,
    ASSET_D_2D_MG_PACHINKO_NCL_BIN = 0x00e1
};

#pragma defer_codegen off

#include "types.h"
#include "dScMgPachinko2_c.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

/* The pointer-to-member tables dispatch on this class. */
typedef void (dScMgPachinko2_c::*PMF)(int);
struct Entry { PMF pmf; };

/* CupSlot_DrawAll's view of the two 0x20-byte entries at 0x5620. */
struct E {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char pad0[0x0f];        /* 0x08 */
    unsigned char idx;      /* 0x17 */
    char pad1;              /* 0x18 */
    unsigned char flag;     /* 0x19 */
    char pad2[6];           /* 0x1a */
};

struct Obj {
    char pad[0x5620];
    struct E arr[2];
};

/* CupSlot_ResetAll's two slot entries at 0x563b. */
struct Slot {
    u8 active;   /* +0x00 (this+0x563b) */
    u8 count;    /* +0x01 */
    u8 id;       /* +0x02 */
    u8 timer;    /* +0x03 */
    char _pad[0x1c];
};

struct Obj4ec {
    char _pad[0x563b];
    struct Slot slots[2];  /* 0x563b */
};

/* Spark_UpdateAll's view of the 16 entries at 0x5330. */
typedef struct {
    char _pad0[0x5330];
    u16 timer;   /* +0x5330 */
    char _pad1[2];
    u8 f4;       /* +0x5334 */
    u8 stage;    /* +0x5335 */
    u8 active;   /* +0x5336 */
} View;

/* DrawTopRecord's view: the state word at 0x5660. */
struct Obj4c0 { char pad[0x5660]; int f; /* 0x5660 */ };

/* Paddle_DrawAll's three 0x40-byte records at 0x5260. */
struct E68c {
    int x;                          /* +0x00 (abs 0x5260) */
    int y;                          /* +0x04 (abs 0x5264) */
    unsigned char pad0[0x35 - 0x08];
    unsigned char flag;             /* +0x35 (abs 0x5295) */
    unsigned char pad1[0x40 - 0x36];
};

struct Obj68c {
    unsigned char pad[0x5260];
    struct E68c arr[3];
};

/* Paddle_StateSwing's view of the same records. */
struct Sub {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0c */ s32 unk0c;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1c */ s32 unk1c;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2c */ s32 unk2c;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
    /* 0x34 */ u16 unk34;
    /* 0x36 */ u8 unk36;
    /* 0x37 */ u8 unk37;
    /* 0x38 */ u8 unk38;
    /* 0x39 */ u8 unk39;
    /* 0x3a */ u8 unk3a;
    /* 0x3b */ u8 unk3b;
    /* 0x3c */ s32 unk3c;
};

struct Obje3c {
    /* 0x0000 */ u8 pad0[0x5260];
    /* 0x5260 */ struct Sub sub[2];
};

/* Paddle_UpdateAll's stride-only view of the records at 0x5260. */
struct Row { u8 d[0x40]; };

/* Ball_Bounce's view of the 0x40-byte balls at 0x4660. */
struct Entryd6c {
    u32 handle;    /* +0x00 (this+0x4660) */
    int a;         /* +0x04 = 0xa8000 */
    int b;         /* +0x08 = 0 */
    int vel;       /* +0x0c = -old/4 */
    char _pad10[0x26];
    u16 half;      /* +0x36 (this+0x4696) = 0 */
    char _pad38;
    u8 state;      /* +0x39 (this+0x4699) = 4 */
    char _pad3a[3];
    u8 flag;       /* +0x3d (this+0x469d) = 0 */
    char _pad3e[2];
};

struct Objd6c {
    char _pad[0x4660];
    struct Entryd6c entries[1];  /* 0x4660 */
};

extern "C" {

/* --- shared ov004, main and library helpers --- */
extern void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
extern void Mg_RenderSpriteBothScaled(int a, int b, int c, int d, int e, int f, int g);
extern void Mg_DrawBigNumber(int a, int b, int c, int d, int e, int f, int g);
extern int  Mg_GetSubmittedScore(void);
extern int  Mg_IsNewRecord(void);
extern int  Mg_GetTopRecord(void);
extern void Mg_DrawTopLeftCounter(void);
extern void Mg_DrawTopRightCounter(int, int);
extern void Mg_DrawTopCenterCounter(int a0);
extern void Mg_SubmitScore(int self);
extern void Mg_InitBrush(char *, char, int, int);
extern void Mg_SetTopScreenYOffset(int);
extern void Mg_SetSharedState(int);
extern void FreeGfxSlotsById(int n);
extern int  RandomIntInternal(int *seed);
extern int  Sound_PlayIfNotActive(int a, int b, int c, int d);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_02012718(int, int);
extern int  _ZN4cstd4sqrtEy(u64 x);
extern s16  _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern int  LoadFile(int handle);
extern void Deallocate(void*);
extern void DecompressLZ16(int src, void *dst);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  func_02054d88(void);
extern void func_02056314(void*, u32, u32);
extern void func_020562b4(const void*, u32, u32);
extern void func_020564f4(const void*, int, int);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern unsigned _ZN3G2S12GetBG2ScrPtrEv(void);
extern unsigned _ZN3G2S13GetBG3CharPtrEv(void);
extern void *_ZN11dScMgBase_cC2Ev(void*);

/* --- ROM data this TU reads --- */
extern int  data_0209d4b8;
extern u8   data_0209d45c;
extern u8   data_0209d454;
extern s16  data_02082214[];
extern int sPachinkoPipeSprites[];
extern int sPachinko2SparkSprites[];
extern int sPachinko2PaddleSprite;
extern int sPachinko2BallSprite;
extern int sPachinko2BallCountSprites[];
extern u8 sPachinko2CupHopRightStartFrames[];
extern u8 sPachinko2CupHopLeftStartFrames[];
extern unsigned char sPachinko2BallWalkOffSprites[];
extern unsigned char sPachinko2PaddleCountPerLanes[];
extern u8 sPachinko2CupHopLeftFrames[];
extern u8 sPachinko2CupHopRightFrames[];
extern unsigned char sPachinko2CupWaitFrames[];
extern unsigned char sPachinko2CupWaitTimes[];
extern int sPachinko2CupX[];
extern int sPachinko2CrossRowY[];
extern int sPachinko2PaddleFlyAwayVX[];
extern unsigned short sPachinko2CupWaitTimesAlt[];
extern s32 sPachinko2CupRightStops[];
extern s32 sPachinko2CupLeftStops[];
extern unsigned short *sPachinko2CupWaitFramesPerCup[];
extern int *sPachinko2PaddleDescendLimits[];
extern int *sPachinko2PaddleSweepLimits[];
extern int *sPachinko2PaddleGatherTargets[];
extern int  data_ov006_0213dbbc[];

/* The three pointer-to-member tables the helpers dispatch through. */
extern Entry sPachinko2CupStateTable[];
extern Entry sPachinko2BallStateTable[];
extern PMF sPachinko2PaddleStateTable[];

/* The ROM {code pointer, adjustment} records the three tables copy from
   (unlicensed .data), grouped by destination table in retail copy order. */
extern PMF pmf_dScMgPachinko2_c_Ball_StateWaitGrab;
extern PMF pmf_dScMgPachinko2_c_Ball_StateAim;
extern PMF pmf_dScMgPachinko2_c_Ball_StateFlying;
extern PMF pmf_dScMgPachinko2_c_Ball_StateTumble;
extern PMF pmf_dScMgPachinko2_c_Ball_StateHop;
extern PMF pmf_dScMgPachinko2_c_Ball_StateWalkOff;
extern PMF pmf_dScMgPachinko2_c_Ball_StateRemove;
extern PMF pmf_dScMgPachinko2_c_Ball_StateScored;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateSpawn;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateDescend;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateSweep;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateBoxLoop;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateOrbit;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateWeave;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateCross;
extern PMF pmf_dScMgPachinko2_c_Paddle_StatePause;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateKnockedDown;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateFallToGround;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateSwing;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateRiseHome;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateGather;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateDropBalls;
extern PMF pmf_dScMgPachinko2_c_Paddle_StateFlyAway;
extern PMF pmf_dScMgPachinko2_c_Cup_StateInit;
extern PMF pmf_dScMgPachinko2_c_Cup_StateWait;
extern PMF pmf_dScMgPachinko2_c_Cup_StateHopLeft;
extern PMF pmf_dScMgPachinko2_c_Cup_StateHopRight;
extern PMF pmf_dScMgPachinko2_c_Cup_StateDecide;

}  /* extern "C" */

// @symbol _ZN16dScMgPachinko2_cD1Ev
// @symbol _ZN16dScMgPachinko2_cD0Ev
/* Both variants come from this one definition: D1 stores this class's
   vtable and calls dScMgBase_c's D2; D0 adds dScMgBase_c::operator delete. */
dScMgPachinko2_c::~dScMgPachinko2_c()
{
}

// @symbol _ZN16dScMgPachinko2_c15CupSlot_DrawAllEv
void dScMgPachinko2_c::CupSlot_DrawAll()
{
    struct Obj *o = (struct Obj *)this;
    int i;
    for (i = 0; i < 2; i++) {
        if (o->arr[i].flag) {
            Mg_RenderSpriteBoth((void *)sPachinkoPipeSprites[o->arr[i].idx], o->arr[i].x >> 12,
                                o->arr[i].y >> 12, -1, 2);
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c16CupSlot_ResetAllEv
/* Resets two 0x20-byte slot entries: flag=1, id=0xff, two counters=0. */
void dScMgPachinko2_c::CupSlot_ResetAll()
{
    struct Obj4ec *self = (struct Obj4ec *)this;
    int i;

    for (i = 0; i < 2; i++) {
        self->slots[i].active = 1;
        self->slots[i].id = 0xff;
        self->slots[i].count = 0;
        self->slots[i].timer = 0;
    }
}

// @symbol _ZN16dScMgPachinko2_c14Cup_PickTargetEi
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgPachinko2_c::Cup_PickTarget(int k)
{
    char *c = (char *)this;
    int found = 0;
    int i = 0;
    int *px = (int *)(c + k * 32 + 0x5620);
    int *py = (int *)(c + k * 32 + 0x5624);
    for (; i < 0x30; i++) {
        int dx, dy;
        if (mBalls[i].active == 0)
            continue;
        if (mBalls[i].state < 3) continue;
        dx = (mBalls[i].x - *px) >> 12;
        dy = (mBalls[i].y - *py) >> 12;
        if (dx < -0x18) continue;
        if (dx > 0x18) continue;
        if (dy < 0) continue;
        if (dy <= 0x40) { found++; break; }
    }
    if (found == 0) return;
    if ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 2) >> 15) {
        mCups[k].state = 3;
        mCups[k].phase = 0;
        mCups[k].vx = 0x1800;
        mCups[k].frame = 0x10;
        mCups[k].animStep = 0;
        mCups[k].timer = 0;
        mCups[k].dir = 1;
    } else {
        mCups[k].state = 2;
        mCups[k].phase = 0;
        mCups[k].vx = -0x1800;
        mCups[k].frame = 0x10;
        mCups[k].animStep = 0;
        mCups[k].timer = 0;
        mCups[k].dir = 0;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c15Cup_StateDecideEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Cup_StateDecide(int n) {
    unsigned int r;
    int flag;

    flag = 0;
    r = (unsigned int)RandomIntInternal(&data_0209d4b8);
    if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
    {
        flag = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        if ((((r >> 16) & 0x7fff) << 2) >> 0xf)
        {
            mCups[n].state = 3;
            mCups[n].vx = 0x1800;
        }
        else
        {
            mCups[n].state = 2;
            mCups[n].vx = -0x1800;
        }
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        mCups[n].hops = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
    }
    else
    {
        mCups[n].state = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        mCups[n].hops = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
    }

    {
        unsigned int state = mCups[n].lastMove;
        if (state != 0xff)
        {
            if (flag == state)
            {
                mCups[n].repeats += 1;
                if (mCups[n].repeats >= 2)
                {
                    if (flag != 0)
                    {
                        mCups[n].state = 1;
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        mCups[n].hops = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
                        flag = 0;
                    }
                    else
                    {
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
                        {
                            mCups[n].state = 3;
                            mCups[n].vx = 0x1800;
                        }
                        else
                        {
                            mCups[n].state = 2;
                            mCups[n].vx = -0x1800;
                        }
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        mCups[n].hops = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
                        flag = 1;
                    }
                }
            }
            else
            {
                mCups[n].repeats = 0;
            }
        }
    }

    mCups[n].lastMove = (char)flag;
    mCups[n].phase = 0;
    mCups[n].frame = 0x10;
    mCups[n].animStep = 0;
    mCups[n].timer = 0;
    mCups[n].dir = 1;
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Cup_StateHopRightEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Cup_StateHopRight(int i) {
    int v;

    if (mCups[i].phase == 1) {
        mCups[i].timer++;
        if (mCups[i].timer < 4)
            return;
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 3) {
            mCups[i].animStep = 0;
            mCups[i].state = 2;
            if (mCups[i].armed != 0) {
                if (mCups[i].hops != 0) {
                    mCups[i].phase = 0;
                    mCups[i].hops--;
                    return;
                }
            }
            mCups[i].phase = 2;
            return;
        }
        mCups[i].frame = sPachinko2CupHopRightStartFrames[mCups[i].animStep];
        return;
    }

    mCups[i].timer++;
    if (mCups[i].timer >= 4) {
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 6)
            mCups[i].animStep = 0;
        mCups[i].frame = sPachinko2CupHopRightFrames[mCups[i].animStep];
    }

    mCups[i].x = mCups[i].x + mCups[i].vx;
    v = mCups[i].x >> 0xc;

    if (mCups[i].phase == 2) {
        if (v >= sPachinko2CupRightStops[i + 4]) {
            mCups[i].x = sPachinko2CupRightStops[i + 4] << 0xc;
            mCups[i].vx = 0;
            mCups[i].phase = 0;
            mCups[i].state = 0;
            return;
        }
    }

    if (v >= sPachinko2CupRightStops[i + 2]) {
        if (mCups[i].vx >= 0x800) {
            mCups[i].vx = mCups[i].vx - 0x80;
        }
    } else {
        mCups[i].vx = mCups[i].vx + 0x80;
    }

    if (v < sPachinko2CupRightStops[i])
        return;

    mCups[i].x = sPachinko2CupRightStops[i] << 0xc;
    mCups[i].vx = -0x1000;
    mCups[i].phase++;
    mCups[i].timer = 0;
    mCups[i].animStep = 0;
    mCups[i].frame = sPachinko2CupHopRightStartFrames[0];
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c16Cup_StateHopLeftEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Cup_StateHopLeft(int i) {
    int v;

    if (mCups[i].phase == 1) {
        mCups[i].timer++;
        if (mCups[i].timer < 4)
            return;
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 3) {
            mCups[i].animStep = 0;
            mCups[i].state = 3;
            if (mCups[i].armed != 0) {
                if (mCups[i].hops != 0) {
                    mCups[i].phase = 0;
                    mCups[i].hops--;
                    return;
                }
            }
            mCups[i].phase = 2;
            return;
        }
        mCups[i].frame = sPachinko2CupHopLeftStartFrames[mCups[i].animStep];
        return;
    }

    mCups[i].timer++;
    if (mCups[i].timer >= 4) {
        mCups[i].timer = 0;
        mCups[i].animStep++;
        if (mCups[i].animStep >= 6)
            mCups[i].animStep = 0;
        mCups[i].frame = sPachinko2CupHopLeftFrames[mCups[i].animStep];
    }

    mCups[i].x = mCups[i].x + mCups[i].vx;
    v = mCups[i].x >> 0xc;

    if (mCups[i].phase == 2) {
        if (v <= sPachinko2CupLeftStops[i + 4]) {
            mCups[i].x = sPachinko2CupLeftStops[i + 4] << 0xc;
            mCups[i].vx = 0;
            mCups[i].phase = 0;
            mCups[i].state = 0;
            return;
        }
    }

    if (v <= sPachinko2CupLeftStops[i + 2]) {
        if (mCups[i].vx <= -0x800) {
            mCups[i].vx = mCups[i].vx + 0x80;
        }
    } else {
        mCups[i].vx = mCups[i].vx - 0x80;
    }

    if (v > sPachinko2CupLeftStops[i])
        return;

    mCups[i].x = sPachinko2CupLeftStops[i] << 0xc;
    mCups[i].vx = 0x1000;
    mCups[i].phase++;
    mCups[i].timer = 0;
    mCups[i].animStep = 0;
    mCups[i].frame = sPachinko2CupHopLeftStartFrames[0];
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c13Cup_StateWaitEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Cup_StateWait(int k) {
    if (mCups[k].armed != 0) {
        mCups[k].timer++;
        if (mCups[k].timer >= sPachinko2CupWaitTimes[mCups[k].animStep]) {
            mCups[k].timer = 0;
            mCups[k].animStep++;
            if (mCups[k].animStep >= 8) {
                mCups[k].animStep = 0;
                if (mCups[k].hops == 0) {
                    mCups[k].state = 4;
                    mCups[k].frame = 0x10;
                    return;
                }
                mCups[k].hops--;
            }
        }
        mCups[k].frame = sPachinko2CupWaitFrames[mCups[k].animStep];
    } else {
        mCups[k].timer++;
        if ((unsigned int)mCups[k].timer >=
            (unsigned int)sPachinko2CupWaitTimesAlt[mCups[k].animStep]) {
            mCups[k].timer = 0;
            mCups[k].animStep++;
            if (mCups[k].animStep >= 7) {
                mCups[k].animStep = 0;
            }
            mCups[k].frame = sPachinko2CupWaitFramesPerCup[k][mCups[k].animStep];
        }
        Cup_PickTarget(k);
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c13Cup_StateInitEi
void dScMgPachinko2_c::Cup_StateInit(int i) {
    char *base = (char *)this;
  char *p;
  *((base + (i << 5)) + 0x5637) = 0x10;
  *((unsigned char *) ((base + (i << 5)) + 0x5638)) = 0;
  *((short *) ((base + (i << 5)) + 0x5630)) = 0;
  *((unsigned char *) ((base + (i << 5)) + 0x5639)) = 1;
  *((unsigned char *) ((base + (i << 5)) + 0x5635)) = 1;
}

// @symbol _ZN16dScMgPachinko2_c11Cup_DrawAllEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Cup_DrawAll()
{
    int i;
    for (i = 0; i < 2; i++) {
        char *b = (char *)this + (i << 5);
        if (*(unsigned char *)(b + 0x5634) != 0) {
            (this->*sPachinko2CupStateTable[*(unsigned char *)(b + 0x5635)].pmf)(i);
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c11Cup_ShowAllEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Cup_ShowAll()
{
    int i;
    for(i=0;i<2;i++){
        mCups[i].active = 1;
        mCups[i].visible = 0;
        mCups[i].state = 0;
        mCups[i].phase = 0;
        mCups[i].frame = 0;
        mCups[i].animStep = 0;
        mCups[i].dir = 0;
        mCups[i].x = sPachinko2CupX[i] << 12;
        mCups[i].y = 0x68000;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c11Cup_HideAllEv
void dScMgPachinko2_c::Cup_HideAll()
{
    unsigned char (*c)[0x20] = (unsigned char (*)[0x20])this;
    int i;
    for (i = 0; i < 2; i++) {
        c[i][0x5634] = 0;
        c[i][0x5639] = 0;
        c[i][0x563b] = 0;
    }
}

// @symbol _ZN16dScMgPachinko2_c16ChooseLaneConfigEv
void dScMgPachinko2_c::ChooseLaneConfig()
{
    unsigned int a, b;
    if ((int)mLevel != 0) {
        a = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
        if ((unsigned int)mLaneConfig == a) {
            b = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 0xf) + 1;
            a = a + b;
            if ((int)a >= 6) a -= 6;
        }
        mLaneConfig = a;
        return;
    }
    mLaneConfig = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
}

// @symbol _ZN16dScMgPachinko2_c13Popup_DrawAllEv
void dScMgPachinko2_c::Popup_DrawAll()
{
    char *c = (char *)this;
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x4b5) != 0){
            Mg_DrawBigNumber(
                *(int*)(c+0x5000+0x4a0) >> 12,
                *(int*)(c+0x5000+0x4a4) >> 12,
                *(unsigned char*)(c+0x5000+0x4b6),
                -1, -1, 0, 0);
        }
        c += 0x18;
    }
}

// @symbol _ZN16dScMgPachinko2_c15Popup_UpdateAllEv
void dScMgPachinko2_c::Popup_UpdateAll()
{
    char *p = (char *)this;
    int i;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(u8*)(p + 0x54b4) != 0) {
            if (*(u8*)(p + 0x54b7) == 0) {
                *(int*)(((int)p + 0x54a4)) += *(int*)(p + 0x54a8);
                *(int*)(((int)p + 0x54a8)) -= 0x200;
                if (*(u16*)(p + 0x54b0) == 0x30) {
                    *(u8*)(((int)p + 0x54b7)) += 1;
                }
            }
            if (*(u16*)(p + 0x54b0) != 0) {
                *(u16*)(((int)p + 0x54b0)) -= 1;
            } else {
                *(u8*)(p + 0x54b5) = 0;
                *(u8*)(p + 0x54b4) = 0;
                *(u16*)(p + 0x54b0) = 0;
            }
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c11Popup_SpawnEiii
void dScMgPachinko2_c::Popup_Spawn(int r1, int r2, int r3)
{
    char *c = (char *)this;
    int i;
    char *p = c;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(unsigned char *)(p + 0x54b4) == 0) {
            int off = i * 0x18;
            mScorePops[i].active = 1;
            mScorePops[i].visible = 1;
            if (r1 >= 0x80000)
                mScorePops[i].x = 0xa4000;
            else
                mScorePops[i].x = 0x5c000;
            {
                char *w = (char *)(int)(c + off);
                *(int *)(w + 0x54a4) = r2 + 0x48000;
                *(int *)(w + 0x54ac) = 0;
                *(int *)(w + 0x54a8) = 0;
                *(unsigned char *)(w + 0x54b6) = (unsigned char)r3;
                *(unsigned char *)(w + 0x54b7) = 0;
                *(short *)(w + 0x54b0) = 0x40;
            }
            return;
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c13Spark_DrawAllEv
void dScMgPachinko2_c::Spark_DrawAll()
{
    char *c = (char *)this;
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x334) != 0){
            Mg_RenderSpriteBoth(
                (void *)sPachinko2SparkSprites[*(unsigned char *)(c + 0x5000 + 0x335)],
                *(int *)(c + 0x5000 + 0x320) >> 12, *(int *)(c + 0x5000 + 0x324) >> 12, -1, -1);
        }
        c += 0x18;
    }
}

// @symbol _ZN16dScMgPachinko2_c15Spark_UpdateAllEv
/* For each of 16 active entries (stride 0x18 array at 0x5330): every 8
 * ticks bump a stage byte; after 4 stages clear the entry. */
void dScMgPachinko2_c::Spark_UpdateAll()
{
    char *c = (char *)this;
    int i;
    for (i = 0; i < 16; i++, c += 0x18) {
        View *v = (View*)c;
        if (v->active == 0)
            continue;
        (*(u16*)(c + 0x5330))++;
        if (v->timer < 8)
            continue;
        (*(u8*)(c + 0x5335))++;
        if (v->stage >= 4) {
            v->active = 0;
            v->stage = 0;
            v->f4 = 0;
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c11Spark_SpawnEii
void dScMgPachinko2_c::Spark_Spawn(int a2, int a3)
{
    char *c = (char *)this;
    int i = 0;
    char *p = c;
    do {
        if (*(unsigned char *)(p + 0x5336) == 0) {
            mHitFx[i].x = a2;
            mHitFx[i].y = a3;
            mHitFx[i].active = 1;
            mHitFx[i].timer = 0;
            mHitFx[i].unk_08 = 0;
            mHitFx[i].unk_0c = 0;
            mHitFx[i].visible = 1;
            mHitFx[i].frame = 0;
            return;
        }
        i++;
        p += 0x18;
    } while (i < 0x10);
}

// @symbol _ZN16dScMgPachinko2_c9DrawScoreEv
void dScMgPachinko2_c::DrawScore()
{
    if (mPhase < 2)
        return;
    Mg_DrawTopRightCounter(Mg_GetSubmittedScore(), 6);
}

// @symbol _ZN16dScMgPachinko2_c13DrawTopRecordEv
void dScMgPachinko2_c::DrawTopRecord()
{
    struct Obj4c0 *o = (struct Obj4c0 *)this;
    if (o->f < 2)
        return;
    Mg_GetTopRecord();
    Mg_DrawTopLeftCounter();
}

// @symbol _ZN16dScMgPachinko2_c13StartRoundEndEi
void dScMgPachinko2_c::StartRoundEnd(int a)
{
    char *dst;
    volatile unsigned short v;
    mPhase = 3;
    mEndTimer = 0x40;
    dst = (char *)_ZN3G2S13GetBG2CharPtrEv();
    v = 0;
    MultiStore16(v, dst, 0x6000);
    mResult = (unsigned char)a;
    Ball_ClearAll();
}

// @symbol _ZN16dScMgPachinko2_c14CheckRoundOverEv
void dScMgPachinko2_c::CheckRoundOver()
{
    char *s = (char *)this;
    int count;
    int i;

    if (mPaddles[0].ballsHeld && mPaddles[0].active) return;
    if (mPaddles[1].ballsHeld && mPaddles[1].active) return;
    if (mPaddles[2].ballsHeld && mPaddles[2].active) return;
    if (mPaddles[0].state != 0xd && mPaddles[0].active) return;
    if (mPaddles[1].state != 0xd && mPaddles[1].active) return;
    if (mPaddles[2].state != 0xd && mPaddles[2].active) return;

    count = 0;
    for (i = 0; i < 0x30; i++) {
        if (((u8 (*)[0x40])(s + 0x4698))[i][0]) {
            count++;
            break;
        }
    }
    if (count) return;

    if (Mg_IsNewRecord())
        StartRoundEnd(1);
    else
        StartRoundEnd(0);
    Paddle_HideAll();
}

// @symbol _ZN16dScMgPachinko2_c14Paddle_DrawAllEv
void dScMgPachinko2_c::Paddle_DrawAll()
{
    struct Obj68c *o = (struct Obj68c *)this;
    int i;
    for(i=0;i<3;i++){
        if(o->arr[i].flag){
            Mg_RenderSpriteBoth((void *)sPachinko2PaddleSprite, o->arr[i].x >> 12,
                                o->arr[i].y >> 12, -1, -1);
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c14Paddle_HideAllEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Paddle_HideAll()
{
    unsigned char *o = (unsigned char *)this;
  int i;
  for(i=0;i<3;i++){
    unsigned char* b = o + (i<<6);
    if(b[0x5000+0x294] != 0){
      b[0x5000+0x296] = 0xe;
      *(unsigned short*)(b + 0x5200 + 0x92) = 0x88;
    }
  }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c11Paddle_StopEi
void dScMgPachinko2_c::Paddle_Stop(int idx)
{
    if (mPaddles[idx].active == 0) return;
    mPaddles[idx].angle = 0;
    mPaddles[idx].speed = 0;
    mPaddles[idx].timer = 0x40;
    mPaddles[idx].state = 0xc;
}

// @symbol _ZN16dScMgPachinko2_c20Paddle_CheckBallHitsEi
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_CheckBallHits(int idx)
{
    char *c = (char *)this;
    int i;
    u8 (*arr)[0x40];
    u8 *slot;
    u8 st;
    int *px;
    int *py;
    int k1800;
    int kn1800;
    u8 *pcnt;
    int k2800;
    int kn2800;
    int k2000;
    int kn2000;
    int *pvx;
    u8 *row;
    u8 *pflag;
    u8 *base4k;
    int *p4664;
    int dx;
    int dy;
    int *p466c;
    int *pxrow;
    int z;

    arr = (u8 (*)[0x40])c;
    slot = arr[idx];
    st = slot[0x5296];
    if (st == 0)
        return;
    if (st >= 0xc)
        return;

    px = (int *)(slot + 0x5260);
    py = (int *)(slot + 0x5264);
    k1800 = 0x1800;
    kn1800 = -k1800;
    pcnt = slot + 0x5299;
    k2800 = 0x2800;
    kn2800 = -k2800;
    k2000 = 0x2000;
    kn2000 = -k2000;
    i = 0;
    pvx = (int *)(slot + 0x5268);
    z = i;

    do {
        row = arr[i];
        pflag = row + 0x4698;
        if (*pflag != 0) {
            base4k = row + 0x4000;
            if (base4k[0x699] >= 2) {
                p466c = (int *)(row + 0x466c);
                if (*p466c > 0x2000) {
                    dx = (*px - *(int *)(row + 0x4660)) >> 12;
                    p4664 = (int *)(row + 0x4664);
                    dy = (*py - *p4664) >> 12;
                    if (dx >= -0x30 && dx <= 0x30 && dy >= 0 && dy <= 8) {
                        if (dx >= -0x10 && dx <= 0x10) {
                            *pcnt = (u8)(*pcnt + 1);
                            row[0x469a] = (u8)z;
                            pxrow = (int *)(row + 0x4660);
                            *pflag = (u8)z;
                            Spark_Spawn(*pxrow, *p4664);
                            func_02012718(0x19a, *pxrow);
                            Paddle_Hit(idx);
                        } else if (dx >= -0x20 && dx <= 0x20) {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = *pvx - 0x1800;
                            else
                                *(int *)(arr[i] + 0x4668) = *pvx + 0x1800;
                            *p466c = kn2000;
                            Sound::PlayBank2_2D(0x19b);
                        } else {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = k1800;
                            else
                                *(int *)(arr[i] + 0x4668) = kn1800;
                            *p466c = kn2800;
                            Sound::PlayBank2_2D(0x19b);
                        }
                    }
                }
            }
        }
        i++;
    } while (i < 0x30);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c19Paddle_StateFlyAwayEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateFlyAway(int i) {
    int v;
    u8 state;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        if ((short)mPaddles[i].timer > 0)
            return;
        mPaddles[i].vy = 0;
        mPaddles[i].vx = sPachinko2PaddleFlyAwayVX[i];
        mPaddles[i].dir = i;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    mPaddles[i].vy -= 0x20;
    state = mPaddles[i].dir;
    if (state == 0) {
        if (mPaddles[i].vx <= 0xc00)
            mPaddles[i].vx += 0x80;
    } else if (state == 1) {
        if (mPaddles[i].vx >= -0xc00)
            mPaddles[i].vx -= 0x80;
    }

    v = mPaddles[i].y >> 12;
    if (v <= -0x120) {
        mPaddles[i].active = 0;
        mPaddles[i].visible = 0;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c21Paddle_StateDropBallsEi
void dScMgPachinko2_c::Paddle_StateDropBalls(int idx) {

    if (mPaddles[idx].timer != 0) {
        mPaddles[idx].timer -= 1;
        if ((short)mPaddles[idx].timer < 0) {
            mPaddles[idx].timer = 0;
        }
        return;
    }
    if (mPaddles[idx].ballsHeld != 0) {
        int x;
        int z;

        mPaddles[idx].ballsHeld -= 1;
        mPaddles[idx].timer = 0x30;
        x = mPaddles[idx].x >> 12;
        z = mPaddles[idx].y >> 12;
        Ball_HitAt(x, z, mPaddles[idx].ballsHeld & 1);
    }
}

// @symbol _ZN16dScMgPachinko2_c18Paddle_StateGatherEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateGather(int i) {
    int b = i << 6;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        if ((s16)mPaddles[i].timer < 0)
            mPaddles[i].timer = 0;
        return;
    }

    {
        int r4 = sPachinko2PaddleGatherTargets[mLaneConfig][i];
        int ax = 0x80 - (mPaddles[i].x >> 12);
        int y_raw = mPaddles[i].y;

        y_raw = ax ? y_raw : y_raw;
        mPaddles[i].angle = (s16)_ZN4cstd5atan2E5Fix12IiES1_(
            r4 - (y_raw >> 12), ax);

        {
            s16 tv = data_02082214[(((u16)mPaddles[i].angle >> 4) << 1) + 1];
            int spd = mPaddles[i].speed;
            mPaddles[i].x += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        {
            s16 tv = data_02082214[((u16)mPaddles[i].angle >> 4) << 1];
            int spd = mPaddles[i].speed;
            mPaddles[i].y += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        mPaddles[i].speed += 0x100;

        {
            int dx = (mPaddles[i].x >> 12) - 0x80;
            int dy = (mPaddles[i].y >> 12) - r4;
            if (dx < -3)
                return;
            if (dx > 3)
                return;
            if (dy < -3)
                return;
            if (dy > 3)
                return;
        }

        mPaddles[i].x = 0x80000;
        mPaddles[i].y = r4 << 12;
        mPaddles[i].state = 0xd;
        CupSlot_ResetAll();
        mPaddles[i].timer = 0x40;
        if (i & 1) {
            mPaddles[i].timer += 0x20;
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c20Paddle_StateRiseHomeEi
void dScMgPachinko2_c::Paddle_StateRiseHome(int idx) {
    char *base = (char *)this;
  char *r3 = base + (idx * 0x40);
  if ((*((unsigned short *) (r3 + 0x5292))) != 0)
  {
    unsigned short *h = (unsigned short *) ((base + 0x5292) + (idx * 0x40));
    *h = (*h) - 1;
    return;
  }
  char *q;
  *((int *) ((base + 0x5264) + (idx * 0x40))) = (*((int *) ((base + 0x5264) + (idx * 0x40)))) + (*((int *) ((r3 + 0x5000) + 0x26c)));
  *((int *) ((base + 0x526c) + (idx * 0x40))) = (*((int *) ((base + 0x526c) + (idx * 0x40)))) - 0x100;
  int a = *((int *) ((r3 + 0x5000) + 0x280));
  int b = *((int *) ((r3 + 0x5000) + 0x264));
  if (a < b)
  {
    return;
  }
  *((int *) ((r3 + 0x5000) + 0x264)) = a;
  *((unsigned char *) ((r3 + 0x5000) + (b = 0x296))) = *((unsigned char *) ((r3 + 0x5000) + 0x297));
  *((int *) ((r3 + 0x5000) + 0x268)) = *((int *) ((r3 + 0x5000) + 0x284));
  *((int *) ((r3 + 0x5000) + 0x26c)) = *((int *) ((r3 + 0x5000) + 0x288));
}

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateSwingEi
void dScMgPachinko2_c::Paddle_StateSwing(int idx) {
    struct Obje3c *obj = (struct Obje3c *)this;
    obj->sub[idx].unk00 += obj->sub[idx].unk08;

    if (obj->sub[idx].unk3a == 0) {
        obj->sub[idx].unk08 += 0x200;
        if (obj->sub[idx].unk08 >= 0x1800) {
            obj->sub[idx].unk3a = 1;
        }
    } else if (obj->sub[idx].unk3a == 1) {
        obj->sub[idx].unk08 -= 0x200;
        if (obj->sub[idx].unk08 <= -0x1800) {
            obj->sub[idx].unk3a = 0;
        }
    }

    if (obj->sub[idx].unk32 != 0) {
        obj->sub[idx].unk32--;
        return;
    }

    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 < -2) {
        return;
    }
    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 > 2) {
        return;
    }

    obj->sub[idx].unk32 = 0x10;
    obj->sub[idx].unk0c = 0;
    obj->sub[idx].unk36 = 0xb;
    obj->sub[idx].unk00 = obj->sub[idx].unk1c;
    obj->sub[idx].unk08 = 0;
}

// @symbol _ZN16dScMgPachinko2_c24Paddle_StateFallToGroundEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateFallToGround(int i) {
    int d;
    mPaddles[i].y += mPaddles[i].vy;
    d = mPaddles[i].y >> 12;
    if (mPaddles[i].vy <= 0x4000)
        mPaddles[i].vy += 0x200;
    if (d < 0xa0)
        return;
    mPaddles[i].y = 0xa0000;
    mPaddles[i].state = 0xa;
    mPaddles[i].vy = 0;
    mPaddles[i].timer = 0x40;
    if ((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 1) >> 15) {
        mPaddles[i].vx = 0x1000;
        mPaddles[i].swing = 0;
    } else {
        mPaddles[i].vx = -0x1000;
        mPaddles[i].swing = 1;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c23Paddle_StateKnockedDownEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateKnockedDown(int i) {
    int d;
    mPaddles[i].y += mPaddles[i].vy;
    d = (mPaddles[i].y - mPaddles[i].homeY) >> 12;
    if (d >= 0x40) {
        if (mPaddles[i].vy >= 0x1000)
            mPaddles[i].vy -= 0x100;
    } else if (mPaddles[i].vy <= 0x4000) {
        mPaddles[i].vy += 0x200;
    }
    if (d < 0x60) return;
    mPaddles[i].timer = 0x10;
    mPaddles[i].vy = 0;
    mPaddles[i].state = 0xb;
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c10Paddle_HitEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_Hit(int i)
{
    if ((int)mPaddles[i].state >= 8) return;

    mPaddles[i].hits += 1;

    if (mPaddles[i].hits < 4) return;

    mPaddles[i].homeX = mPaddles[i].x;
    mPaddles[i].homeY = mPaddles[i].y;
    mPaddles[i].savedState = mPaddles[i].state;
    mPaddles[i].savedVx = mPaddles[i].vx;
    mPaddles[i].savedVy = mPaddles[i].vy;
    mPaddles[i].vx = 0;
    mPaddles[i].vx = 0;
    mPaddles[i].vy = 0x2000;

    if (mPaddles[i].flip != 0)
        mPaddles[i].state = 9;
    else
        mPaddles[i].state = 8;

    mPaddles[i].hits = 0;

    mPaddles[i].flip ^= 1;
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StatePauseEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StatePause(int i) {
    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer -= 1;
        if ((short)mPaddles[i].timer < 0)
            mPaddles[i].timer = 0;
        return;
    }

    if (mLaneConfig == 3)
        mPaddles[i].state = 3;
    else
        mPaddles[i].state = 2;
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateCrossEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateCross(int i) {
    int v1;
    int v2;
    u8 state;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    v1 = mPaddles[i].x >> 12;
    state = mPaddles[i].step;
    v2 = mPaddles[i].y >> 12;

    if (state == 0) {
        if (v1 >= 0xe0) {
            mPaddles[i].timer = 0x40;
            mPaddles[i].step++;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            return;
        }
        if (v1 >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
            if (i == 0) {
                if (mPaddles[i].vy <= 0xc00)
                    mPaddles[i].vy += 0x10;
            } else {
                if (mPaddles[i].vy >= -0xc00)
                    mPaddles[i].vy -= 0x10;
            }
        } else {
            if (mPaddles[i].vx <= 0xc00)
                mPaddles[i].vx += 0x20;
            if (i == 0) {
                if (v2 != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy >= -0xc00)
                        mPaddles[i].vy -= 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            } else {
                if (v2 != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy <= 0xc00)
                        mPaddles[i].vy += 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            }
        }
    } else if (state == 1) {
        if (v1 <= 0x20) {
            mPaddles[i].timer = 0x40;
            mPaddles[i].step = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            return;
        }
        if (v1 <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
            if (i == 0) {
                if (mPaddles[i].vy <= 0xc00)
                    mPaddles[i].vy += 0x10;
            } else {
                if (mPaddles[i].vy >= -0xc00)
                    mPaddles[i].vy -= 0x10;
            }
        } else {
            if (mPaddles[i].vx >= -0xc00)
                mPaddles[i].vx -= 0x20;
            if (i == 0) {
                if (v2 != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy >= -0xc00)
                        mPaddles[i].vy -= 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            } else {
                if (v2 != sPachinko2CrossRowY[i]) {
                    if (mPaddles[i].vy <= 0xc00)
                        mPaddles[i].vy += 0x30;
                } else {
                    mPaddles[i].vy = 0;
                }
            }
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateWeaveEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateWeave(int i) {
    int v;
    u8 flag;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer -= 1;
        return;
    }

    mPaddles[i].x += mPaddles[i].vx;
    mPaddles[i].y += mPaddles[i].vy;
    flag = mPaddles[i].dir;
    v = mPaddles[i].x >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            if (mPaddles[i].vx <= 0x1000)
                mPaddles[i].vx += 0x80;
            if (mPaddles[i].step == 0) {
                if (mPaddles[i].vy <= 0x1000)
                    mPaddles[i].vy += 0xc;
            } else {
                if (mPaddles[i].vy >= -0x1000)
                    mPaddles[i].vy -= 0xc;
            }
        }
        if (v >= 0xe0) {
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].step ^= 1;
            mPaddles[i].timer = 0x40;
        }
    } else {
        if (v <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            if (mPaddles[i].vx >= -0x1000)
                mPaddles[i].vx -= 0x80;
            if (mPaddles[i].step == 0) {
                if (mPaddles[i].vy <= 0x1000)
                    mPaddles[i].vy += 0xc;
            } else {
                if (mPaddles[i].vy >= -0x1000)
                    mPaddles[i].vy -= 0xc;
            }
        }
        if (v <= 0x20) {
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].dir ^= 1;
            mPaddles[i].step ^= 1;
            mPaddles[i].timer = 0x40;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateOrbitEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateOrbit(int i) {
    char *c = (char *)this;
    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t0 = (long long)data_02082214[(angle >> 4) * 2 + 1] * 0x50000 + 0x800;
        mPaddles[i].x = (int)(t0 >> 12) + 0x80000;
    }

    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t1 = (long long)data_02082214[(angle >> 4) * 2] * 0x50000 + 0x800;
        mPaddles[i].y = (int)(t1 >> 12) - 0x80000;
    }

    if (mPaddles[i].step != 0) {
        *(unsigned short *)(c + 0x5290 + (i << 6)) -= 0x40;
    } else {
        *(unsigned short *)(c + 0x5290 + (i << 6)) += 0x40;
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c19Paddle_StateBoxLoopEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateBoxLoop(int i) {
    int v;
    u8 state;

    if (mPaddles[i].timer != 0) {
        mPaddles[i].timer--;
        return;
    }

    state = mPaddles[i].step;

    if (state == 0) {
        mPaddles[i].y += mPaddles[i].vy;
        v = mPaddles[i].y >> 12;
        if (v >= -0x60) {
            if (mPaddles[i].vy >= 0x400)
                mPaddles[i].vy -= 8;
        } else {
            if (mPaddles[i].vy <= 0x1000)
                mPaddles[i].vy += 0x80;
        }
        if (v >= -0x40) {
            mPaddles[i].y = -0x40000;
            mPaddles[i].vy = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
            return;
        }
    } else if (state == 1) {
        mPaddles[i].x += mPaddles[i].vx;
        v = mPaddles[i].x >> 12;
        if (v >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            if (mPaddles[i].vx <= 0x1000)
                mPaddles[i].vx += 0x80;
        }
        if (v >= 0xe0) {
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vy = 0;
            mPaddles[i].vx = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
            return;
        }
    } else if (state == 2) {
        mPaddles[i].y += mPaddles[i].vy;
        v = mPaddles[i].y >> 12;
        if (v <= -0xa0) {
            if (mPaddles[i].vy <= -0x400)
                mPaddles[i].vy += 8;
        } else {
            if (mPaddles[i].vy >= -0x1000)
                mPaddles[i].vy -= 0x80;
        }
        if (v <= -0xc0) {
            mPaddles[i].y = -0xc0000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step++;
        }
    } else {
        mPaddles[i].x += mPaddles[i].vx;
        v = mPaddles[i].x >> 12;
        if (v <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            if (mPaddles[i].vx >= -0x1000)
                mPaddles[i].vx -= 0x80;
        }
        if (v <= 0x20) {
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].vy = 0;
            mPaddles[i].timer = 0;
            mPaddles[i].step = 0;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateSweepEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateSweep(int i) {
    int v;
    u8 flag;

    mPaddles[i].x += mPaddles[i].vx;
    flag = mPaddles[i].dir;
    v = mPaddles[i].x >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (mPaddles[i].vx >= 0x400)
                mPaddles[i].vx -= 8;
        } else {
            int idx = mLaneConfig;
            if (mPaddles[i].vx <= sPachinko2PaddleSweepLimits[idx][i])
                mPaddles[i].vx += 0x80;
        }
        if (v >= 0xe0) {
            u32 a;
            mPaddles[i].x = 0xe0000;
            mPaddles[i].vx = 0;
            mPaddles[i].dir ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mPaddles[i].timer = (u16)(((a << 3) >> 15) * 3 + 0x40);
            mPaddles[i].state = 7;
        }
    } else {
        if (v <= 0x80) {
            if (mPaddles[i].vx <= -0x400)
                mPaddles[i].vx += 8;
        } else {
            int idx = mLaneConfig;
            if (mPaddles[i].vx >= -sPachinko2PaddleSweepLimits[idx][i])
                mPaddles[i].vx -= 0x80;
        }
        if (v <= 0x20) {
            u32 a;
            mPaddles[i].x = 0x20000;
            mPaddles[i].vx = 0;
            mPaddles[i].dir ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            mPaddles[i].timer = (u16)(((a << 3) >> 15) * 3 + 0x40);
            mPaddles[i].state = 7;
        }
    }

    mPaddles[i].sound = Sound_PlayIfNotActive(mPaddles[i].sound, 2, 0x198, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c19Paddle_StateDescendEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateDescend(int i) {
    char *p = (char *)this;
    int limit;

    mPaddles[i].y += mPaddles[i].vy;
    limit = sPachinko2PaddleDescendLimits[mLaneConfig][i];

    if ((mPaddles[i].y >> 12) >= limit) {
        mPaddles[i].y = limit << 12;
        mPaddles[i].vy = 0;

        if (i & 2) {
            if (mPaddles[i].vx > 0)
                mPaddles[i].vx = 0xc80;
            else
                mPaddles[i].vx = -0xc80;
        }

        if (mLaneConfig == 1) {
            mPaddles[i].state = 4;
            if (mPaddles[i].vx > 0)
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0x8000;
            else
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0;
        }

        if (mLaneConfig == 3) {
            mPaddles[i].state = 3;
            mPaddles[i].vx = 0;
            if (mPaddles[i].dir != 0) {
                mPaddles[i].vy = 0xc00;
                mPaddles[i].step = 0;
            } else {
                mPaddles[i].vy = -0xc00;
                mPaddles[i].step = 2;
            }
            return;
        }

        if (mLaneConfig == 4) {
            mPaddles[i].state = 5;
            if (i == 0)
                mPaddles[i].step = 0;
            else
                mPaddles[i].step = 1;
        }

        if (mLaneConfig == 5) {
            mPaddles[i].state = 6;
            if (mPaddles[i].dir != 0)
                mPaddles[i].step = 0;
            else
                mPaddles[i].step = 1;
            return;
        }
    } else {
        mPaddles[i].vy -= 0x20;
    }

    Paddle_StateSweep(i);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Paddle_StateSpawnEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Paddle_StateSpawn(int idx) {
    char *c = (char *)this;
    char *slot;
    char *ip;
    char *base;
    int bit;
    int r;
    int off;

    slot = c + (idx << 6);
    off = idx << 6;

    if (mPaddles[idx].timer != 0) {
        base = c + 0x5292;
        *(unsigned short *)(base + off) =
            (unsigned short)(*(unsigned short *)(base + off) - 1);

        if (*(short *)(slot + 0x5292) < 0)
            mPaddles[idx].timer = 0;

        return;
    }

    ip = slot + 0x5000;
    *(unsigned char *)(ip + 0x296) = 1;

    r = RandomIntInternal(&data_0209d4b8);
    bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);

    if (idx & 1)
        bit = mPaddles[0].dir ^ 1;

    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x298) = bit;

    if (bit != 0) {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = -0x10000;
        *(int *)(ip + 0x268) = 0x200;

        if (mLaneConfig == 1) {
            *(int *)(ip + 0x268) = 0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) = mPaddles[0].step;
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    } else {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = 0x110000;
        *(int *)(ip + 0x268) = -0x200;

        if (mLaneConfig == 1) {
            *(int *)(ip + 0x268) = -0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) = mPaddles[0].step;
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    }

    mPaddles[idx].y = -0xf8000;
    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x299) = 0;
    *(int *)(ip + 0x26c) = 0x2000;

    r = func_020126e8(*(int *)(ip + 0x260));
    mPaddles[idx].sound = func_02012468(
        mPaddles[idx].sound,
        2, 0x198, 4, 0, 0, r, 0);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c16Paddle_UpdateAllEv
void dScMgPachinko2_c::Paddle_UpdateAll()
{
    Row *rows = (Row *)this;
    for (int i = 0; i < 3; i++) {
        if (rows[i].d[0x5294]) {
            (this->*sPachinko2PaddleStateTable[rows[i].d[0x5296]])(i);
            Paddle_CheckBallHits(i);
        }
    }
}

// @symbol _ZN16dScMgPachinko2_c20Paddle_SpawnForLanesEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Paddle_SpawnForLanes()
{
    char *c = (char *)this;
    int n = sPachinko2PaddleCountPerLanes[mLaneConfig];
    int i = 0;
    if (n <= 0) return;
    do {
        char *b = c + i * 0x40;
        mPaddles[i].active = 1;
        mPaddles[i].state = 0;
        mPaddles[i].step = 0;
        *(short*)(b + 0x5292) = 0x20;
        mPaddles[i].sound = 0;
        *(int*)(b + 0x5274) = 0;
        *(int*)(b + 0x5278) = 0;
        mPaddles[i].hits = 0;
        mPaddles[i].flip = 0;
        i++;
    } while (i < n);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c15Paddle_ClearAllEv
void dScMgPachinko2_c::Paddle_ClearAll()
{
    char (*c)[64] = (char (*)[64])this;
  int i;
  for(i=0;i<3;i++){ c[i][0x5294]=0; c[i][0x5295]=0; }
}

// @symbol _ZN16dScMgPachinko2_c17CheckCountdownEndEv
void dScMgPachinko2_c::CheckCountdownEnd()
{
  volatile unsigned short v;
  char* dst;
  if (mCountdown != 0) return;
  Paddle_Stop(0);
  Paddle_Stop(1);
  Paddle_Stop(2);
  mPhase = 2;
  mEndTimer = 0x40;
  dst = (char*)_ZN3G2S13GetBG2CharPtrEv();
  v = 0;
  MultiStore16(v, dst, 0x6000);
  mResult = 0;
  Ball_ClearAll();
}

// @symbol _ZN16dScMgPachinko2_c13DrawCountdownEv
void dScMgPachinko2_c::DrawCountdown()
{
    if (mCountdownShown == 0) return;
    Mg_DrawTopCenterCounter(mCountdown);
}

// @symbol _ZN16dScMgPachinko2_c15UpdateCountdownEv
void dScMgPachinko2_c::UpdateCountdown()
{
    char *c = (char *)this;
    unsigned char *q;
    unsigned short *h;
    unsigned short t;
    if (mCountdown == 0) return;
    q = (unsigned char *)(((int)c + 0x5679));
    *q += 1;
    if (mCountdownTick < 0x3c) return;
    mCountdownTick = 0;
    h = &mCountdown;
    *h -= 1;
    t = mCountdown;
    if (t > 0xa)
        Sound::PlayBank2_2D(0xa8);
    else if (t > 3)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
    if (mCountdown == 0)
        mCountdownShown = 0;
}

// @symbol _ZN16dScMgPachinko2_c12ClearAimLineEv
void dScMgPachinko2_c::ClearAimLine()
{
    char *c = (char *)this;
    volatile unsigned short tmp;
    int n = mHeldBall;
    int a, b;
    char* base;

    if (n == 0) return;

    base = c + (n - 1) * 0x40;
    a = 0x80 - (*(int*)(base + 0x4660) >> 12);
    b = 0x20 - (*(int*)(base + 0x4664) >> 12);

    if (a < -6) return;
    if (a > 6) return;
    if (b < -6) return;
    if (b > 6) return;
    if (*(unsigned char*)(base + 0x4699) != 2) return;

    mHeldBall = 0;
    {
        char* dst = (char*)_ZN3G2S13GetBG2CharPtrEv();
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);
    }
}

// @symbol _ZN16dScMgPachinko2_c16PlotSubBg2PixelsEiii
#pragma push
#pragma opt_loop_invariants off
void dScMgPachinko2_c::PlotSubBg2Pixels(int a1, int a2, int a3)
{
    int i, j, x, lo, hi, col, row, yb, s;
    u32 *bp;
    s = a1;
    for (i = 0; i < 2; i++)
    {
        x = a2 - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = s - 1;
            a1 = yb + j;
            bp = (u32 *)_ZN3G2S13GetBG2CharPtrEv();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= a3 << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c11DrawAimLineEv
#define LA(p) (*(int *)((((int)(p)) + 0x5664)))
#define LB(p) (*(int *)((((unsigned int)(p)) + 0x5664)))
#define LC(p) (*(int *)(((int)((p) + 0x5664))))
#define LD(p) (*(int *)(((unsigned int)((p) + 0x5664))))
#define LE(p) (*(int *)(((long long)((int)(p) + 0x5664))))
#define LF(p) (*(int *)(((long long)((unsigned int)(p) + 0x5664))))
#define LG(p) (*(int *)(((unsigned long long)((int)(p) + 0x5664))))
#define LH(p) (*(int *)(((unsigned long long)((unsigned int)(p) + 0x5664))))
#pragma push
#pragma opt_loop_invariants off
#pragma opt_dead_assignments off
void dScMgPachinko2_c::DrawAimLine()
{
    char *c = (char *)this;
    int k;                       /* sp+0x00 */
    int tx;                      /* sp+0x04 */
    int ty;                      /* sp+0x08 */
    int *p1;                     /* sp+0x0c */
    int *p2;                     /* sp+0x10 */
    int idx;                     /* sp+0x14 */
    int v2[2];                   /* sp+0x18..0x1c */
    int x1;                      /* sp+0x20 */
    volatile unsigned short tmp; /* sp+0x24 */
    int cx;                      /* sb */
    int cy;                      /* r8 */
    int dx;                      /* r7 */
    int dy;                      /* fp */
    int adx;                     /* r6 */
    int ady;                     /* r5 */
    int layer;                   /* r4 */
    int n;

    n = mHeldBall;
    if (n == 0) return;
    idx = n - 1;
    if (mBalls[idx].x != *(int *)(c + idx * 0x40 + 0x4000 + 0x678)
     || mBalls[idx].y != *(int *)(c + idx * 0x40 + 0x4000 + 0x67c)) {
        char *dst = (char *)_ZN3G2S13GetBG2CharPtrEv();
        int i2;
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);

        i2 = (int)((long long)idx);
        p1 = (int *)(c + i2 * 0x40 + 0x4660);
        p2 = (int *)(c + i2 * 0x40 + 0x4664);
        k = 0;
        v2[0] = 0x6c;
        v2[1] = 0x22;
        layer = 2;
        x1 = 0x94;
        do {
            if (k == 0) {
                int v;
                cx = v2[0];
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx - 4;
            } else {
                int v;
                cx = x1;
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx + 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;

            if (adx >= ady) {
                mGuideStep = adx / 2;
            LX:
                if (dx == 0) {
                    PlotSubBg2Pixels(cx, cy, layer);
                } else if (dx > 0) {
                    cx++;
                    LA(c) += ady;
                    if (mGuideStep > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LB(c) -= adx;
                    }
                    PlotSubBg2Pixels(cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                } else {
                    cx--;
                    LC(c) += ady;
                    if (mGuideStep > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LD(c) -= adx;
                    }
                    PlotSubBg2Pixels(cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                }
            } else {
                mGuideStep = ady / 2;
            LY:
                if (dy == 0) {
                    PlotSubBg2Pixels(cx, cy, layer);
                } else if (dy > 0) {
                    cy++;
                    LE(c) += adx;
                    if (mGuideStep > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LF(c) -= ady;
                    }
                    PlotSubBg2Pixels(cx, cy, layer);
                    if (cy == ty) goto DONEK;
                    goto LY;
                } else {
                    cy--;
                    LG(c) += adx;
                    if (mGuideStep > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LH(c) -= ady;
                    }
                    PlotSubBg2Pixels(cx, cy, layer);
                    if (cy != ty) goto LY;
                }
            }
        DONEK:
            k++;
        } while (k < 2);
    }
    {
        int i3 = (int)((unsigned long long)idx);
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x678) = mBalls[i3].x;
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x67c) = mBalls[i3].y;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c10Ball_HitAtEiii
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_HitAt(int x, int z, int d)
{
    char *o = (char *)this;
    int i;
    for (i = 0; i < 0x30; i++) {
        char *q = o + i * 0x40;
        if (mBalls[i].active == 0) {
            mBalls[i].active = 1;
            mBalls[i].state = 7;
            *(u16 *)(q + 0x4696) = 0x40;
            mBalls[i].visible = 1;
            mBalls[i].sprite = 0;
            if (d != 0)
                *(u16 *)(q + 0x4694) = 0xd000;
            else
                *(u16 *)(q + 0x4694) = 0xb000;
            mBalls[i].x = x << 12;
            mBalls[i].y = (z - 0x12) << 12;
            mBalls[i].vx =
                FX_MUL(data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2 + 1], 0x2000);
            mBalls[i].vy =
                FX_MUL(data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2], 0x2000);
            *(int *)(o + i * 0x40 + 0x4684) = 0;
            Sound::PlayBank2_2D(0x199);
            return;
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c11Ball_BounceEi
/* Resets entry i of a 0x40-stride array at this+0x4660: reverses and
 * quarters its velocity, clears counters, and tail-calls func_02012718
 * (sound: id 0x19c) with the entry's handle. */
void dScMgPachinko2_c::Ball_Bounce(int i)
{
    struct Objd6c *self = (struct Objd6c *)this;
    self->entries[i].state = 4;
    self->entries[i].vel = -self->entries[i].vel >> 2;
    self->entries[i].b = 0;
    self->entries[i].half = 0;
    self->entries[i].flag = 0;
    self->entries[i].a = 0xa8000;
    func_02012718(0x19c, self->entries[i].handle);
}

// @symbol _ZN16dScMgPachinko2_c13Ball_ClearAllEv
void dScMgPachinko2_c::Ball_ClearAll()
{
    char (*c)[64] = (char (*)[64])this;
  int i;
  for(i=0;i<0x30;i++){ c[i][0x469a]=0; c[i][0x4698]=0; }
}

// @symbol _ZN16dScMgPachinko2_c12Ball_DrawAllEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Ball_DrawAll()
{
    char *p = (char *)this;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (mBalls[i].visible != 0) {
            int x = mBalls[i].x >> 12;
            unsigned char t = mBalls[i].sprite;
            int y = mBalls[i].y >> 12;
            if (t != 0) {
                Mg_RenderSpriteBoth((void *)sPachinko2BallCountSprites[t - 1], x, y, -1, -1);
            } else {
                Mg_RenderSpriteBothScaled(sPachinko2BallSprite, x, y, -1, -1, 0x1000,
                                          *(unsigned short *)(p + i * 64 + 0x4690));
            }
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c14Ball_UpdateAllEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::Ball_UpdateAll()
{
    int i;
    for (i = 0; i < 0x30; i++) {
        if (mBalls[i].active != 0) {
            unsigned char k = mBalls[i].state;
            (this->*sPachinko2BallStateTable[k].pmf)(i);
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c14StartCountdownEv
void dScMgPachinko2_c::StartCountdown()
{
    unsigned char *r0 = (unsigned char *)this;
    if (mCountdownArmed != 0) return;
    mCountdown = 0x1e;
    mCountdownTick = 0;
    mCountdownShown = 1;
    {
        unsigned char *p = (unsigned char *)(r0 + 0x5678);
        *p += 1;
    }
}

// @symbol _ZN16dScMgPachinko2_c18Ball_StateWaitGrabEi
void dScMgPachinko2_c::Ball_StateWaitGrab(int arg1) {
    char *base = (char *)this;
    u8 idx;
    int off;
    int has;
    char *e;
    int a, b;

    idx = gActivePlayerSlot;
    off = idx * 4;
    has = 0;
    if (gTouchHeld[off])
    {
        if (gTouchEdge[off])
            has = 1;
    }
    if (has != 0)
    {
        e = base + arg1 * 0x40 + 0x4000;
        a = (*(int *)(e + 0x660) >> 12) - gTouchX[idx * 4];
        b = (*(int *)(e + 0x664) >> 12) - gTouchY[idx * 4];
        *(u8 *)(e + 0x699) = 1;
        *(int *)(e + 0x670) = a << 12;
        *(int *)(e + 0x674) = b << 12;
        *(u8 *)(e + 0x69b) = 0;
    }
    DrawAimLine();
}

// @symbol _ZN16dScMgPachinko2_c13Ball_StateAimEi
/* dScMgPachinko2_c ball i under a live pen record: while the pen is down the
   ball follows the pen (x/y = pen + held offset, clamped to the field, offset
   re-derived); on release it aims the ball at (0x80, 0x20) and launches it.
   Touch record reads: gTouchX (pen x) is a volatile byte read and
   gTouchY (pen y) is read through a plain u8 pointer. Measured under
   2004/b56: both volatile 4 div (ldrb/add and literal/ldr order), both plain
   arrays 18 div (the y read sinks past the x store), both pointer reads 7 div
   (r3/r6 colouring); only this pairing reproduces the ROM's literal hoisting
   and load order at both sites. */
void dScMgPachinko2_c::Ball_StateAim(int i) {
    int idx = gActivePlayerSlot;
    int off = idx * 4;

    if (gTouchHeld[off] != 0) {
        int sx;
        int sy;
        int dx;
        int dy;
        int dist;
        int sq;
        int prev;
        int tx = ((volatile u8 *)gTouchX)[off];
        int ty = ((u8 *)gTouchY)[off];

        this->mBalls[i].x = this->mBalls[i].px + (tx << 12);
        this->mBalls[i].y = this->mBalls[i].py + (ty << 12);
        sx = this->mBalls[i].x >> 12;
        sy = this->mBalls[i].y >> 12;
        if (sx >= 0xf8) {
            this->mBalls[i].x = 0xf8000;
        }
        if (sx <= 8) {
            this->mBalls[i].x = 0x8000;
        }
        if (sy >= 0xb8) {
            this->mBalls[i].y = 0xb8000;
        }
        if (sy <= 8) {
            this->mBalls[i].y = 0x8000;
        }
        {
            int nx = (this->mBalls[i].x >> 12) - ((volatile u8 *)gTouchX)[gActivePlayerSlot * 4];
            int ny = (this->mBalls[i].y >> 12) - ((u8 *)gTouchY)[gActivePlayerSlot * 4];
            this->mBalls[i].px = nx << 12;
            this->mBalls[i].py = ny << 12;
        }

        dy = 0x80 - (this->mBalls[i].x >> 12);
        dx = 0x20 - (this->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        prev = this->mBalls[i].prevDist;
        this->mBalls[i].prevDist = sq;
        if (sq > prev) {
            if (sq > prev + 10) {
                this->mBalls[i].sound = Sound_PlayIfNotActive(this->mBalls[i].sound, 2, 0x17b, 0);
            } else {
                this->mBalls[i].sound = Sound_PlayIfNotActive(this->mBalls[i].sound, 2, 0x17c, 0);
            }
        }
    } else {
        int dy;
        int dx;
        int dist;
        int sq;

        this->mBalls[i].state = 2;
        dy = 0x80 - (this->mBalls[i].x >> 12);
        dx = 0x20 - (this->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        if (sq >= 0x10) {
            this->mBalls[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            this->mBalls[i].speed = _ZN4cstd4sqrtEy(dist) << 8;
            this->mBalls[i].speed += this->mBalls[i].speed >> 3;
            this->mBalls[i].speed += 0x1800;
            if (this->mBalls[i].speed >= 0x9400) {
                this->mBalls[i].speed = 0x9400;
            }
            this->mNextBallDelay = this->mBalls[i].speed >> 11;
            this->mNextBallDelay += this->mNextBallDelay >> 1;
            if (this->mNextBallDelay == 0) {
                this->mNextBallDelay = 1;
            }
            {
                s16 c = data_02082214[((u16)this->mBalls[i].angle >> 4) * 2 + 1];
                this->mBalls[i].vx = FX_MUL(c, this->mBalls[i].speed);
            }
            {
                s16 s = data_02082214[((u16)this->mBalls[i].angle >> 4) * 2];
                this->mBalls[i].vy = FX_MUL(s, this->mBalls[i].speed);
            }
            this->mBalls[i].timer = 0;
            this->mBalls[i].spin = this->mBalls[i].vx >> 2;
            if (sq >= 0x40) {
                Sound::PlayBank2_2D(0x17e);
            } else {
                Sound::PlayBank2_2D(0x17d);
            }
        } else {
            this->mBalls[i].x = 0x80000;
            this->mBalls[i].y = 0x28000;
            this->mBalls[i].state = 0;
        }
    }
    DrawAimLine();
}

// @symbol _ZN16dScMgPachinko2_c16Ball_StateFlyingEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_StateFlying(int i) {
    char *c = (char *)this;
    int idx;
    int velX, velY;
    int idx2;

    idx2 = ((unsigned short)mBalls[i].angle >> 4) * 2;

    if (data_02082214[idx2] <= 0) {
        {
            short tv = data_02082214[idx2 + 1];
            int spd = mBalls[i].speed;
            mBalls[i].x += FX_MUL(tv, spd);
        }

        idx = (unsigned short)mBalls[i].angle >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = mBalls[i].speed;
            mBalls[i].y += FX_MUL(tv, spd);
        }

        *(int*)(c + 0x4684 + i * 0x40) += 0x10;

        mBalls[i].speed -= *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        if (*(int*)(c + i * 0x40 + 0x4000 + 0x684) >= 0x600) {
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0x600;
        }

        if (mBalls[i].speed < 0) {
            mBalls[i].state = 3;
            mBalls[i].speed = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
            mBalls[i].vy = 0;
            mBalls[i].vx = 0;
        }
    } else {
        mBalls[i].x += mBalls[i].vx;
        mBalls[i].y += mBalls[i].vy;
        *(int*)(c + 0x4684 + i * 0x40) += 0x10;
        mBalls[i].vy += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        {
            int v = mBalls[i].vx;
            int nv;
            if (v > 0) {
                nv = v - 0x200;
                if (nv <= 0) nv = 0;
            } else if (v < 0) {
                nv = v + 0x200;
                if (nv >= 0) nv = 0;
            } else {
                mBalls[i].state = 3;
                mBalls[i].speed = 0;
                *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
                mBalls[i].vx = 0;
                return;
            }
            mBalls[i].vx = nv;
        }
    }

    {
        int rawX = mBalls[i].x;
        int rawY = mBalls[i].y;
        velY = rawY >> 12;
        *(unsigned short *)(c + 0x4690 + i * 0x40) += (unsigned short)mBalls[i].spin;
        velX = rawX >> 12;
    }

    if (velY <= -0x140 || velY >= 0xd0) {
        mBalls[i].state = 6;
    }
    if (velX >= 0x140 || velX <= -0x40) {
        mBalls[i].state = 6;
    }

    ClearAimLine();
    if (mHeldBall != 0) {
        DrawAimLine();
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c16Ball_StateTumbleEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_StateTumble(int i) {
    char *o = (char *)this;
    int v660, v664, t660;
    *(int *)((char *)(((int)o + 0x4660)) + i * 0x40) += mBalls[i].vx;
    *(int *)((char *)(((int)o + 0x4664)) + i * 0x40) += mBalls[i].vy;
    *(int *)((char *)(((int)o + 0x4684)) + i * 0x40) += 0x20;
    *(int *)((char *)(((int)o + 0x466c)) + i * 0x40) += *(int *)(o + i * 0x40 + 0x4684);
    *(u16 *)((char *)(((int)o + 0x4690)) + i * 0x40) += (u16)mBalls[i].spin;
    if (mBalls[i].vy >= 0x8000)
        mBalls[i].vy = 0x8000;
    t660 = mBalls[i].y;
    v660 = mBalls[i].x >> 12;
    v664 = t660 >> 12;
    if (v664 <= -0x140)
        mBalls[i].state = 6;
    if (v660 >= 0x140 || v660 <= -0x40)
        mBalls[i].state = 6;
    if (v664 >= 0xa8)
        Ball_Bounce(i);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c13Ball_StateHopEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_StateHop(int i) {
    char *c = (char *)this;
    int old;
    int v;
    int w;

    mBalls[i].y += mBalls[i].vy;
    old = mBalls[i].vy;
    mBalls[i].vy += 0x200;

    if (old < 0 && mBalls[i].vy >= 0) {
        *(u16 *)(c + i * 0x40 + 0x4600 + 0x90) = 0;
        v = mBalls[i].x >> 12;
        if (v > 0x80) {
            mBalls[i].facingRight = 1;
            mBalls[i].sprite = 5;
            mBalls[i].vx = 0x1000;
        } else if (v < 0x80) {
            mBalls[i].facingRight = 0;
            mBalls[i].sprite = 1;
            mBalls[i].vx = -0x1000;
        } else {
            unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            if (((r << 1) >> 15) != 0) {
                mBalls[i].facingRight = 1;
                mBalls[i].sprite = 5;
                mBalls[i].vx = 0x1000;
            } else {
                mBalls[i].facingRight = 0;
                mBalls[i].sprite = 1;
                mBalls[i].vx = -0x1000;
            }
        }
    }

    w = mBalls[i].y >> 12;
    if (w >= 0xa8) {
        mBalls[i].y = 0xa8000;
        mBalls[i].state = 5;
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c17Ball_StateWalkOffEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_StateWalkOff(int i) {
    char *c = (char *)this;
    *(unsigned short *)(c + 0x4696 + (i << 6)) += 1;

    if (*(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) >= 4) {
        *(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) = 0;
        mBalls[i].animStep += 1;
        if (mBalls[i].animStep >= 4)
            mBalls[i].animStep = 0;
    }

    {
        unsigned char idx = mBalls[i].animStep;
        mBalls[i].sprite = sPachinko2BallWalkOffSprites[idx];
        if (mBalls[i].facingRight != 0)
            mBalls[i].sprite += 4;
    }

    mBalls[i].x += mBalls[i].vx;

    {
        int val = mBalls[i].x;
        int scaled = val >> 12;
        if (scaled >= 0x110 || scaled <= -16) {
            mBalls[i].state = 6;
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c16Ball_StateRemoveEi
void dScMgPachinko2_c::Ball_StateRemove(int idx) {
    mBalls[idx].active = 0;
    mBalls[idx].visible = 0;
}

// @symbol _ZN16dScMgPachinko2_c16Ball_StateScoredEi
#pragma push
#pragma opt_common_subs off
void dScMgPachinko2_c::Ball_StateScored(int i) {
    char *c = (char *)this;
    mBalls[i].x += mBalls[i].vx;
    mBalls[i].y += mBalls[i].vy;
    *(int*)(c + 0x4684 + i * 0x40) += 0x10;
    mBalls[i].vy += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

    if (mBalls[i].vy >= 0x8000)
        mBalls[i].vy = 0x8000;

    if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) != 0)
    {
        *(unsigned short*)(c + 0x4696 + i * 0x40) -= 1;

        if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) == 0x20)
        {
            int r3 = Mg_GetSubmittedScore();
            int f660 = mBalls[i].x;
            int f664 = mBalls[i].y;
            Popup_Spawn(f660, f664, r3 + 1);
            Mg_SubmitScore(Mg_GetSubmittedScore() + 1);
        }
    }

    if ((mBalls[i].y >> 12) < 0xa8) return;

    Ball_Bounce(i);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c14UpdateNextBallEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::UpdateNextBall() {
    char *c = (char *)this;
    int i;
    unsigned short *q;
    if (mNextBallDelay == 0)
        return;
    q = &mNextBallDelay;
    *q = *q - 1;
    if ((short)mNextBallDelay > 0)
        return;
    mNextBallDelay = 0;
    for (i = 0; i < 0x30; i++) {
        char *b = c + (i << 6);
        if (mBalls[i].active == 0) {
            mBalls[i].active = 1;
            mBalls[i].visible = 1;
            mBalls[i].state = 0;
            *(int *)(b + 0x4684) = 0;
            mBalls[i].x = 0x80000;
            mBalls[i].y = 0x28000;
            mBalls[i].px = 0;
            mBalls[i].py = 0;
            mBalls[i].spawnClear = 0;
            mBalls[i].vx = 0;
            mBalls[i].vy = 0;
            mBalls[i].timer = 0;
            *(short *)(b + 0x4690) = 0;
            mBalls[i].spin = 0;
            mBalls[i].sprite = 0;
            mBalls[i].animStep = 0;
            mBalls[i].facingRight = 0;
            mBalls[i].sound = 0;
            mBalls[i].prevDist = 4;
            mHeldBall = (unsigned char)(i + 1);
            if (mBallDealt != 0) {
                Sound::PlayBank2_2D(0x19d);
                return;
            }
            mBallDealt += 1;
            return;
        }
    }
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c10ResetRoundEv
#pragma push
#pragma opt_strength_reduction off
void dScMgPachinko2_c::ResetRound()
{
    char *c = (char *)this;
    int i;
    int j;
    char *p;
    mPhase = 0;
    for (i = 0; i < 0x30; i++) {
        char *r = c + (i << 6);
        mBalls[i].x = 0;
        mBalls[i].y = 0;
        *(int *)(r + 0x4678) = 0;
        *(int *)(r + 0x467c) = 0;
        mBalls[i].speed = 0;
        *(int *)(r + 0x4684) = 0;
        *(unsigned short *)(r + 0x4694) = 0;
        *(unsigned short *)(r + 0x4696) = 0;
        mBalls[i].active = 0;
        mBalls[i].state = 0;
        mBalls[i].visible = 0;
    }
    p = c;
    for (j = 0; j < 0x10; j++) {
        *(unsigned char *)(p + 0x5336) = 0;
        *(unsigned char *)(p + 0x5334) = 0;
        p += 0x18;
    }
    mEndTimer = 0;
    unk_5672 = 0;
    mResult = 0;
    mCountdownArmed = 0;
    mCountdownTick = 0;
    mCountdownShown = 0;
    mBallDealt = 0;
    Mg_SubmitScore(0);
    Paddle_ClearAll();
    Cup_HideAll();
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c10StartRoundEi
/* Slot 18, overriding dScMgBase_c::StartRound(int). That name is a
   placeholder borrowed from dActor_c by slot index. */
void dScMgPachinko2_c::StartRound(int n)
{
    /* Reading through `ro` keeps mwcc from holding &mLevel in a register,
       which costs a word. */
    const dScMgPachinko2_c *ro = this;

    mPhase = 0;
    if (n == 0x10) {
        mLevel = ro->mLevel + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
    } else {
        mLevel = 0;
        /* dead clamp, kept: dropping it changes the code */
        if (mLevel > 0x270e) mLevel = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    ResetRound();
}

// @symbol _ZN16dScMgPachinko2_c6RenderEv
/* Slot 9. */
s32 dScMgPachinko2_c::Render()
{
    DrawTopRecord();
    DrawScore();
    DrawCountdown();
    Spark_DrawAll();
    Popup_DrawAll();
    Ball_DrawAll();
    Paddle_DrawAll();
    CupSlot_DrawAll();
    return 1;
}

// @symbol _ZN16dScMgPachinko2_c8BehaviorEv
/* Slot 6. mPhase selects which helpers run this frame. */
s32 dScMgPachinko2_c::Behavior()
{
    switch (mPhase) {
    case 0:
        ChooseLaneConfig();
        Paddle_SpawnForLanes();
        Cup_ShowAll();
        mNextBallDelay = 0x10;
        mPhase = 1;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        UpdateCountdown();
        StartCountdown();
        UpdateNextBall();
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Spark_UpdateAll();
        CheckCountdownEnd();
        break;
    case 2:
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Spark_UpdateAll();
        CheckRoundOver();
        Popup_UpdateAll();
        break;
    case 3:
        Ball_UpdateAll();
        Paddle_UpdateAll();
        Cup_DrawAll();
        Popup_UpdateAll();
        if (mEndTimer != 0) {
            mEndTimer--;
            if ((s16)mEndTimer <= 0) {
                Mg_SetSharedState(MG_STATE_RESULTS);
                mPromptEnabled = 0;
            }
        }
        break;
    }
    return 1;
}

// @symbol _ZN16dScMgPachinko2_c13InitResourcesEv
/* Slot 0. Loads both screens' backgrounds and OBJ graphics, then runs the
   three helpers and the two stores of Behavior's case 0. */
s32 dScMgPachinko2_c::InitResources()
{
    char *b;
    char *dst;
    /* volatile: the ROM stores each fill value to the stack and reloads it */
    volatile u16 spC;
    volatile u16 spE;
    int sp4;
    int sp8;
    int f;
    int n;
    int y;
    int x;

    data_0209d45c |= 8; /* main BG3 on */
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NCG_BIN);
    DecompressLZ16(f, (void *)func_02054d88());
    Deallocate((void *)f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NCL_BIN);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_UP_NSC_BIN);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    sp4 = LoadFile(ASSET_D_2D_MG_PACHINKO_NCG_BIN);
    sp8 = LoadFile(ASSET_D_2D_MG_PACHINKO_NCL_BIN);
    DecompressLZ16(sp4, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);

    data_0209d454 |= 0xd; /* sub BG0, BG2 and BG3 on */
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    b = (char *)_ZN3G2S13GetBG2CharPtrEv();
    spC = 0;
    MultiStore16(spC, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)_ZN3G2S12GetBG2ScrPtrEv() + x + y * 0x20);
            spE = n;
            MultiStore16(spE, dst, 2);
            n++;
        }
    }

    Mg_InitBrush((char *)this, 0, 2, 0);

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x208;

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCG_BIN);
    DecompressLZ16(f, (void *)_ZN3G2S13GetBG3CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NCL_BIN);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(ASSET_D_2D_MG_BG_PACHINKO_DOWN_NSC_BIN);
    func_020562b4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x608;

    f = LoadFile(ASSET_D_2D_HEI_TOP_DOWN_NSC_BIN);
    func_020564f4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    DecompressLZ16(sp4, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);
    Deallocate((void *)sp4);
    Deallocate((void *)sp8);

    ResetRound();
    ChooseLaneConfig();
    Paddle_SpawnForLanes();
    Cup_ShowAll();
    Mg_SetTopScreenYOffset(0x20);

    mRunDuringFade = 1;
    mNextBallDelay = 0x10;
    mPhase = 1;
    return 1;
}

// @symbol dScMgPachinko2_c_classInit
/* The MG_TAMAIRE factory, kept hand-built: fBase_c::operator new for
   0x567c bytes, the base constructor, then the vtable address point
   0x0213dbbc stored as data. The classInit spelling follows later EAD
   lineage; the historical alias is MgLakituLaunch_Spawn. */
extern "C" int *dScMgPachinko2_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(22140);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213dbbc;
    }
    return p;
}

/* The three state tables, defined in retail copy order: the 8-row ball
   table, the 15-row paddle table, then the 5-row cup table. mwcc emits
   __sinit_d_s_mg_pachinko2.cpp from these definitions, copying the ROM
   .data records. */
Entry sPachinko2BallStateTable[8] = {
    {pmf_dScMgPachinko2_c_Ball_StateWaitGrab}, {pmf_dScMgPachinko2_c_Ball_StateAim},
    {pmf_dScMgPachinko2_c_Ball_StateFlying},   {pmf_dScMgPachinko2_c_Ball_StateTumble},
    {pmf_dScMgPachinko2_c_Ball_StateHop},      {pmf_dScMgPachinko2_c_Ball_StateWalkOff},
    {pmf_dScMgPachinko2_c_Ball_StateRemove},   {pmf_dScMgPachinko2_c_Ball_StateScored},
};
PMF sPachinko2PaddleStateTable[15] = {
    pmf_dScMgPachinko2_c_Paddle_StateSpawn,       pmf_dScMgPachinko2_c_Paddle_StateDescend,
    pmf_dScMgPachinko2_c_Paddle_StateSweep,       pmf_dScMgPachinko2_c_Paddle_StateBoxLoop,
    pmf_dScMgPachinko2_c_Paddle_StateOrbit,       pmf_dScMgPachinko2_c_Paddle_StateWeave,
    pmf_dScMgPachinko2_c_Paddle_StateCross,       pmf_dScMgPachinko2_c_Paddle_StatePause,
    pmf_dScMgPachinko2_c_Paddle_StateKnockedDown, pmf_dScMgPachinko2_c_Paddle_StateFallToGround,
    pmf_dScMgPachinko2_c_Paddle_StateSwing,       pmf_dScMgPachinko2_c_Paddle_StateRiseHome,
    pmf_dScMgPachinko2_c_Paddle_StateGather,      pmf_dScMgPachinko2_c_Paddle_StateDropBalls,
    pmf_dScMgPachinko2_c_Paddle_StateFlyAway,
};
Entry sPachinko2CupStateTable[5] = {
    {pmf_dScMgPachinko2_c_Cup_StateInit},    {pmf_dScMgPachinko2_c_Cup_StateWait},
    {pmf_dScMgPachinko2_c_Cup_StateHopLeft}, {pmf_dScMgPachinko2_c_Cup_StateHopRight},
    {pmf_dScMgPachinko2_c_Cup_StateDecide},
};
