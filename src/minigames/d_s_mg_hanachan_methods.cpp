//cpp
/* Which Wiggler? Up to fifteen wigglers, one of them with the star.
 * Touch it before mTimeLeft runs out.
 *
 * This production TU owns ov006 0x020ea280..0x020ede18: 61
 * functions in ROM order, from the destructor to Hana_LoadBackground.
 * The destructor is the key function, so the vtable and RTTI are emitted
 * here too. dScMgHanachan_c_classInit, just above at 0x020ede18, is still
 * its own file (src/minigames/d_s_mg_hanachan.cpp).
 *
 * The destructor pair and the ten functions from Wiggler's destructor to
 * Wiggler::DrawTarget came from one-function files. Their bodies are kept
 * as they matched there. The two popup arrays are declared once; where a
 * function spelled a record differently, its tag is renamed per function
 * and the access casts. The wiggler array's element destructor
 * (formerly Wiggler_Destroy) is Wiggler's own inline destructor.
 *
 * The phase and tap handlers (Phase_WaitTouch .. Phase_SetupRound) are
 * dScMgHanachan_c members. Everything that takes one 0x98-byte wiggler
 * record, an element of mWiggler, is a dScMgHanachan_c::Wiggler member:
 * the nine callback targets, the two draws, the segment and path steps,
 * Spawn and the small queries the scene asks of one wiggler. All of them
 * are declared in dScMgHanachan_c.h, and their descriptive names are
 * reconstructed. The popup helpers, the three Hana_ pickers and
 * Hana_LoadBackground take no object and stay free.
 * Native member-pointer literals and the typed spawn profile now provide the
 * complete 0x0213c95c..0x0213ca7c data range. This reconstruction assignment
 * does not establish the original translation-unit boundary.
 *
 * leftovers:
 * - the popup at sHanaStarPopup is declared as the file-local
 *   HanaStarPopup record, and StarPopup_Update reaches its timer byte
 *   through a separate symbol, sHanaStarPopups.
 * - Wiggler::DrawTarget repeats Wiggler::DrawField's segment
 *   walk with Vector2 points, whose declared destructor is part of
 *   the match.
 * - Wiggler::GetFlag reads the four 2-bit colours in color (+0x74) through
 *   the B72 bitfield view.
 * - Wiggler::DrawField: OAM::Render(int, OamAttr*, int, int, int,
 *   int, int, int) does not typecheck (the sprite table is void*).
 */

/* .text in source order (ROM-ascending; do not reorder) and per-function
   pragmas bind: one O3, on Wiggler::Update. Without it the 49-function version matched 44/49
   and failed the order check. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgHanachan_c_AssetId {
    ASSET_D_2D_MG_BG_HANA_SAGASE1_NCG_BIN = 0x003b,
    ASSET_D_2D_MG_BG_HANA_SAGASE1_NCL_BIN = 0x003c,
    ASSET_D_2D_MG_BG_HANA_SAGASE1_NSC_BIN = 0x003d,
    ASSET_D_2D_MG_BG_HANA_SAGASE2_NCG_BIN = 0x003e,
    ASSET_D_2D_MG_BG_HANA_SAGASE2_NCL_BIN = 0x003f,
    ASSET_D_2D_MG_BG_HANA_SAGASE2_NSC_BIN = 0x0040,
    ASSET_D_2D_MG_HANA_SAGASE_NCG_BIN = 0x00cb,
    ASSET_D_2D_MG_HANA_SAGASE_NCL_BIN = 0x00cc
};

#pragma defer_codegen off

#include "types.h"
#include "Vector2.h"
#include "OAM.h"
#include "OamAttr.h"
#include "nitro/hw/registers.h"
#include "dScMgHanachan_c.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "SpawnInfo.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/fx/fx_cp.h"
#include "Vector2Math.h"

extern int data_ov006_0213c954;
extern int sHanaFieldWigglerCount;

void *dScMgHanachan_c_classInit();
extern "C" BaseSpawnInfo<void> g_profile_MG_HANACHAN = {dScMgHanachan_c_classInit, 0x182, 0x182};

/* Kept (#821 r72): DrawField's vb0 and s3.vc0 as Vector2 give 0x3d8 against
   the ROM's 0x3e0; this plain pair keeps the frame. Re-measured 2026-11-19:
   vb0 as Vector2 still differs by 22 words, while the grouped points in `s`
   are Vector2 with identical bytes. */
typedef struct {
    int x, y;
} Vec2;

struct B72 {
    unsigned short f0 : 2;
    unsigned short f1 : 2;
    unsigned short f2 : 2;
    unsigned short f3 : 2;
};

/* The shared record types the three native member-function pointers before
   the segment coordinates. ang[1] is the movement heading; decoy is cleared
   when this wiggler becomes the target and set after the result. */
typedef dScMgHanachan_c::Wiggler Wiggler;

/* Nothing uses this shim: InitResources reaches slot 18 as StartRound and
   0xa8/0xac as dScMgBase_c's mCredits/mCreditsShown (the names its two
   fields carry here), which is the same
   dispatch and the same bytes.  The DECLARATION still has to stay.  mwccarm
   numbers the TU's compiler-local symbols in declaration order, and the 35
   native pointer-to-member constants this file emits land on the @NNNN names
   that dScMgHanachan_c.json's defined_symbol_renames rows are keyed to.
   Dropping the struct shifts every one of them down by one, which silently
   renames the wrong constants. Recovering that is matching work, not a
   rewrite, so the type stays until those rows are re-derived. */
struct SelfV {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int a);
    char pad[0xa4];
    int mCredits;
    int mCreditsShown;
};

/* ROM symbols this TU calls or reads. decl_common.h's declarations are not
   repeated. The C++ helpers must stay outside the extern "C" block so that
   they mangle. */
namespace Sound {
u32 PlayBank2_2D(u32 id);
}
namespace cstd {
s16 atan2(s32, s32);
}

extern "C" {
/* This TU's own functions. */

extern OamAttr *sWigglerSprites[];
OamAttr data_ov006_021375b8[1] = {{0x00f0, 0x81f0, 0x3104, 0xffff}};
OamAttr data_ov006_021375b0[1] = {{0x00f8, 0x41f8, 0x305e, 0xffff}};
OamAttr data_ov006_021375a8[1] = {{0x00f8, 0x41f8, 0x301e, 0xffff}};
OamAttr *sHanaWigglerTargetSprites[] = {data_ov006_021375b8, data_ov006_021375b0,
                                        data_ov006_021375a8};
OamAttr data_ov006_02137598[1] = {{0x40f8, 0x81f0, 0x314a, 0xffff}};
OamAttr data_ov006_02137590[1] = {{0x40f8, 0x81f0, 0x310a, 0xffff}};
OamAttr *sHanaWigglerFieldSprites[] = {data_ov006_02137598, data_ov006_02137590};

/* The per-language text sprites, in GetGameLanguage order. */
extern OamAttr *sMgTextSpritesEnglish[];
extern OamAttr *sMgTextSpritesSpanish[];
extern OamAttr *sMgTextSpritesFrench[];
extern OamAttr *sMgTextSpritesGerman[];
extern OamAttr *sMgTextSpritesItalian[];
OamAttr **sHanaTextSpritesByLanguage[5] = {
    sMgTextSpritesEnglish, sMgTextSpritesSpanish, sMgTextSpritesFrench,
    sMgTextSpritesGerman,  sMgTextSpritesItalian,
};

extern short sHanaPickedCount;
extern unsigned short sHanaPickedIds[];
extern unsigned short sHanaUsedIds[];
extern void Mg_CommitRecords(void);
extern int GetGameLanguage(void);
extern void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);
extern void Mg_DrawNumberRotated(int a, int b, int c, int d, int e, int f, u16 g);
extern const s16 sHanaRoundPatterns[10] = {0, 0, 0, 1, 1, 2, 2, 2, 3, 3};
extern const int sHanaFieldWigglerCounts[10] = {3, 4, 5, 4, 6, 3, 4, 5, 15, 15};
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;

/* sHanaPickedLow is only read where it is unsigned and only written as a
   halfword, so the unsigned spelling costs the writer nothing. */
extern unsigned short sHanaPickedLow;
/* sHanaRoundPattern: Phase_SetupRound spelled it volatile and only ever
   stores to it (never re-reads), so the plain spelling emits the same strh. */
extern s16 sHanaRoundPattern;
/* LoadFile returns the buffer; one file typed it int and casts at each use. */
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void FreeGfxSlotsById(int x);
}
extern int Hana_PickUnused(int n, int mask);
extern void Hana_ChooseStarWiggler(int count);
extern int Hana_PickMoveKind(void);
extern void Hana_LoadBackground(void);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void StarPopup_Start(int x, int y);
extern void HanaPopup_Spawn(int x, int y);

/* One of the sixteen popups at sHanaPopups, 0x14 bytes each. */
struct HanaPopup {
    int x;                 /* +0x00 */
    int y;                 /* +0x04 */
    int velY;              /* +0x08 */
    unsigned short timer;  /* +0x0c */
    unsigned char active;  /* +0x0e */
    unsigned char visible; /* +0x0f */
    unsigned char phase;   /* +0x10 */
    unsigned char pad[3];
};

extern "C" {
extern struct HanaPopup sHanaPopups[];
extern OamAttr *sMgDigitSprites[];
}

/* Empty: the compiler destroys the fifteen 0x98-byte wigglers at 0x4678
   through __cxa_vec_cleanup with Wiggler's destructor, as dScMgBase_c's own
   destructor does for its array. The vptr store, the base destructor call
   and D0's delete are the compiler's too. */
dScMgHanachan_c::~dScMgHanachan_c() {}

/* Wiggler's empty inline destructor (dScMgHanachan_c.h), emitted here after
   the class destructor that first uses it: the compiler tears the five
   Vector2 segment points at +0x18 down through __cxa_vec_cleanup. */

/* The single popup at sHanaStarPopup, as StarPopup_Start, StarPopup_Update,
   StarPopup_Draw and StarPopup_Clear use it. Its byte fields differ from the
   sixteen popups' HanaPopup layout. */
struct HanaStarPopup {
    int x;      /* 0x00 fx32; clamped to 0x10000..0xf4000 at start */
    int y;      /* 0x04 fx32; kept at or above 0x8000 */
    int vy;     /* 0x08 added to y in phase 0, -0x800 at start, minus 0x100 a frame */
    u8 timer;   /* 0x0c 0x18 at start, 0x40 for the hold; counts to 8 in phase 2 */
    u8 active;  /* 0x0d Update skips the popup while clear */
    u8 phase;   /* 0x0e 0 move, 1 hold and score, 2 count down */
    u8 ticks;   /* 0x0f 5 at start; one per eight frames in phase 2 */
    u8 visible; /* 0x10 Draw draws only while this is 1 */
    u8 pad[3];
};
typedef char HanaStarPopup_size_must_be_0x14[sizeof(HanaStarPopup) == 0x14 ? 1 : -1];
extern "C" HanaStarPopup sHanaStarPopup[];

/* Draws the single popup while its visible flag is 1: a sprite from the
   language's table 8 pixels left of it and one from sMgDigitSprites
   8 pixels right. */
void StarPopup_Draw(void) {
    int x;
    int y;
    s32 r;
    HanaStarPopup *popup = sHanaStarPopup;
    if (popup->visible != 1) {
        return;
    }
    x = popup->x >> 12;
    y = popup->y >> 12;
    r = GetGameLanguage();
    Mg_RenderSubSpriteMtx(sHanaTextSpritesByLanguage[r][13], x - 8, y, 0);
    Mg_RenderSubSpriteMtx(sMgDigitSprites[25], x + 8, y, 0);
}

/* Steps the single popup through three phases. Phase 0 moves it by its
   velocity until its timer runs out, phase 1 holds for 0x40 frames and then
   adds one to the scene's mHudScore (capped at 9999, raising
   mBestHudScore to match), and phase 2 counts ticks down every eighth
   frame, stepping the int behind timeLeft (the scene's mTimeLeft) toward
   0x32 each time. The compound assignments view the record through (int)f;
   the plain spelling reuses the field address and changes the object. */
typedef struct {
    u8 t;
    u8 pad[0x13];
} Timer20;
extern "C" Timer20 sHanaStarPopups[];

void StarPopup_Update(int *timeLeft) {
    int i;
    HanaStarPopup *f = sHanaStarPopup;
    for (i = 0; i < 1; i++, f++) {
        u8 ph;
        if (f->active != 1)
            continue;
        if (f->y < 0x8000)
            f->y = 0x8000;
        ph = f->phase;
        if (ph == 0) {
            f->y += f->vy;
            f->vy -= 0x100;
            if (f->timer != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            f->timer = 64;
            f->phase += 1;
        } else if (ph == 1) {
            if (f->timer != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            f->visible = 0;
            f->phase += 1;
            {
                dScMgBase_c *g = gMgScene;
                if (g != 0) {
                    if (g->mHudScore < 9999)
                        g->mHudScore += 1;
                    if (g->mHudScore > g->mBestHudScore)
                        g->mBestHudScore = g->mHudScore;
                }
            }
            Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
        } else {
            if (f->ticks != 0) {
                f->timer += 1;
                if (f->timer >= 8) {
                    f->timer = 0;
                    f->ticks -= 1;
                    if (timeLeft != 0)
                        ApproachLinear(*timeLeft, 0x32, 1);
                }
            } else {
                f->active = 0;
            }
        }
    }
}

/* Starts the single popup at (x, y), x kept between 0x10000 and 0xf4000,
   unless it is already active. */
void StarPopup_Start(int x, int y) {
    HanaStarPopup *g = sHanaStarPopup;
    if (g->active != 0)
        return;
    g->active = 1;
    if (x < 0x10000)
        x = 0x10000;
    else if (x > 0xf4000)
        x = 0xf4000;
    g->x = x;
    g->y = y;
    g->timer = 24;
    g->phase = 0;
    g->visible = 1;
    g->ticks = 5;
    g->vy = -0x800;
}

/* Clears the single popup's active and draw flags. */
unsigned char StarPopup_Clear(void) {
    sHanaStarPopup->active = 0;
    sHanaStarPopup->visible = 0;
    return 0;
}

/* Draws each of the sixteen visible popups as three sprites 0x10 pixels
   apart. */
void HanaPopup_DrawAll(void) {
    Matrix2x2 *a = 0, *b = 0, *c = 0;
    int i;
    for (i = 0; i < 16; i++) {
        HanaPopup *e = &sHanaPopups[i];
        if (e->visible != 0) {
            int sb = e->x >> 12;
            int r8 = e->y >> 12;
            int idx = GetGameLanguage();
            OamAttr **obj = sHanaTextSpritesByLanguage[idx];
            Mg_RenderSubSpriteMtx(obj[14], sb - 0x10, r8, a);
            Mg_RenderSubSpriteMtx(sMgDigitSprites[41], sb, r8, b);
            Mg_RenderSubSpriteMtx(sMgDigitSprites[40], sb + 0x10, r8, c);
        }
    }
}

/* Steps the sixteen popups: y is kept at or below 0xb8000, phase 0 moves
   each by its velocity until its timer runs out, and phase 1 holds for 0x40
   frames and then clears it. The compound assignments go through (int)e,
   as StarPopup_Update's do. */
void HanaPopup_UpdateAll(void) {
    int i;
    for (i = 0; i < 16; i++) {
        HanaPopup *e = &sHanaPopups[i];
        if (e->active) {
            if (e->y > 0xb8000)
                e->y = 0xb8000;
            if (e->phase == 0) {
                if (e->timer != 0) {
                    ((HanaPopup *)(int)e)->timer -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                    ((HanaPopup *)(int)e)->y += e->velY;
                    ((HanaPopup *)(int)e)->velY += 0x100;
                } else {
                    e->timer = 64;
                    ((HanaPopup *)(int)e)->phase += 1;
                }
            } else {
                if (e->timer != 0) {
                    ((HanaPopup *)(int)e)->timer -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                } else {
                    e->timer = 0;
                    e->active = 0;
                    e->visible = 0;
                }
            }
        }
    }
}

/* Starts a popup in the first free one of the sixteen slots at (r0, r1),
   r0 kept between 0x14000 and 0xf0000. */

void HanaPopup_Spawn(int r0, int r1) {
    int i;
    for (i = 0; i < 16; i++) {
        if (sHanaPopups[i].active == 0) {
            sHanaPopups[i].active = 1;
            sHanaPopups[i].visible = 1;
            sHanaPopups[i].timer = 0x18;
            if (r0 < 0x14000)
                r0 = 0x14000;
            else if (r0 > 0xf0000)
                r0 = 0xf0000;
            sHanaPopups[i].x = r0;
            sHanaPopups[i].y = r1;
            sHanaPopups[i].velY = 0x800;
            sHanaPopups[i].phase = 0;
            return;
        }
    }
}

/* Clears the active and visible flags of every one of the sixteen popups
   that is already inactive. */
void HanaPopup_ClearAll(void) {
    int i;
    for (i = 0; i < 16; i++) {
        if (sHanaPopups[i].active == 0) {
            sHanaPopups[i].active = 0;
            sHanaPopups[i].visible = 0;
        }
    }
}

/* Draws a wiggler's five segments from the tail forward, then one more
   sprite by the head when bloom is set and IsStarVisible agrees. The
   Vector2's declared destructor and the strength-reduction bracket are
   both part of the match. */
void dScMgHanachan_c::Wiggler::DrawTarget() {
    s32 i;
    s16 rot;
    s16 ang;
    s32 A;
    s32 B;
    s32 a2i;
    s32 px;
    s32 py;
    s32 res;
    s32 j;

    Vector2 pos;
    Vector2 va;
    Vector2 vb;
    Vector2 vout;
    Vector2 pts[5];

    pts[4].x = 0xb4000;
    pts[4].y = 0x80000;
    for (j = 3; j >= 1; j--) {
        pts[j].x = pts[j + 1].x;
        pts[j].y = pts[j + 1].y;
        pts[j].x = pts[j].x - (stretch[j] + ((j + 1) << 13));
    }
    pts[0].x = pts[1].x;
    pts[0].y = pts[1].y;
    pts[0].x = pts[0].x - (stretch[0] + 0x2000);

    for (i = 0, A = 0, B = 0; i < 5; i++) {
        rot = -0x4000;
        va.y = 0x1000;
        va.x = 0;
        pos.x = pts[i].x;
        pos.y = pts[i].y;
        ang = (s16)(bob - A);
        func_0203d388(&va, -0x4000);
        if (bloom == 0) {
            if (i == 0) {
                rot += FX_SinIdx((u16)ang >> 4) >> 1;
            } else {
                s32 v = FX_SinIdx((u16)ang >> 4);
                rot += (s16)FX_MUL(v, 0x1800 - B);
            }
        }
        func_0203d680(&vout, &va, FX_SinIdx((u16)(ang * 2) >> 4));
        pos.x = pos.x + vout.x;
        pos.y = pos.y + vout.y;
        res = GetFrame(i);
        px = pos.y >> 12;
        py = pos.x >> 12;
        a2i = GetFlag(i);
        OAM::Render(0, sWigglerSprites[a2i], py, px, res, -1, 0x1000, (u16)rot);
        if (i == 0) {
            if (bloom != 0) {
                if (IsStarVisible() != 0) {
                    vb.x = 0x2000;
                    vb.y = -0x18000;
                    func_0203d388(&vb, rot);
                    vb.x = vb.x + pos.x;
                    vb.y = vb.y + pos.y;
                    OAM::Render(0, sHanaWigglerTargetSprites[0], vb.x >> 12, vb.y >> 12, -1, -1,
                                FX_Inv((FX_SinIdx((u16)ang >> 4) >> 2) + 0xc00), (u16)rot);
                }
            }
        }
        A = A + 0x1800;
        B = B + 0x200;
    }
}

void dScMgHanachan_c::Wiggler::DrawField() {
    /* The three anonymous groupings set the stack layout: the ROM spills A,
       B, (u16)rot, the hoisted zero, res and py, and keeps px in a register.
       Measured 2026-11-16 (#821 r22) against 0x3e0 bytes: plain Vector2
       locals give 0x3dc, the same locals unnested 0x3d8, DrawTarget's
       destructor-only point shape 0x3e4, and all 72 declaration orders of A/B with
       the points as Vector2 miss the size. Inside `s` the points are Vector2
       (identical bytes; func_0203d388 takes a Vector2 *), at the cost of an
       empty local-class destructor the TU manifest discards. */
    struct {
        int A, B, urot, va0x, res;
    } t;
    Vec2 vb0;
    struct {
        Vec2 vc0;
    } s3;
    struct {
        Vector2 pos, va, vb, vc, vout;
    } s;
    int px, py;
    int a2i;
    int i;
    short rot, sway;

    i = 0;
    vb0.y = -0x18000;
    s3.vc0.y = -0x14000;
    t.A = 0;
    t.B = 0;
    vb0.x = 0x2000;
    s3.vc0.x = 0;
    t.va0x = 0;
    for (; i < 5; i++) {
        rot = ang[i];
        s.pos.x = seg[i].x;
        s.pos.y = seg[i].y;
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        sway = (short)(bob - t.A);
        func_0203d388(&s.va, rot);

        if (bloom == 0) {
            if (i == 0) {
                rot = (short)(rot + (FX_SinIdx((unsigned short)sway >> 4) >> 1));
            } else {
                int sinv = FX_SinIdx((unsigned short)sway >> 4);
                rot = (short)(rot + (short)FX_MUL(sinv, 0x1800 - t.B));
            }
        }

        if (sHanaFieldWigglerCount >= 6) {
            rot = (short)(rot & 0xfc00);
        }
        func_0203d680(&s.vout, &s.va, FX_SinIdx((unsigned short)(sway << 1) >> 4));
        s.pos.x += s.vout.x;
        s.pos.y += s.vout.y;

        t.res = GetFrame(i);
        px = s.pos.y >> 12;
        py = s.pos.x >> 12;
        OAM::Render(1, sWigglerSprites[(a2i = GetFlag(i), (t.urot = (unsigned short)rot), a2i)], py,
                    px, t.res, -1, 0x1000, t.urot);

        if (i == 0) {
            if (bloom != 0 && flower > 0) {
                int fp2;
                int scale;
                fp2 = (0x3000 - flower) >> 12;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                if (fp2 != 0)
                    scale = 0x1000;
                else
                    scale = FX_Inv((FX_SinIdx((unsigned short)sway >> 4) >> 2) + 0xc00);
                OAM::Render(1, sHanaWigglerTargetSprites[fp2], s.vb.x >> 12, s.vb.y >> 12, -1, -1,
                            scale, t.urot);
            } else if (state == &Wiggler::StateWait) {
                if (hold > 0x1a && hold < 0x30) {
                    int idx = s3.vc0.x;
                    s.vc.x = s3.vc0.x;
                    s.vc.y = s3.vc0.y;
                    if (hold < 0x26) {
                        idx = 1;
                        s.vc.y -= (0x30 - hold) * 0x600;
                    }
                    func_0203d388(&s.vc, rot);
                    s.vc.x += s.pos.x;
                    s.vc.y += s.pos.y;
                    OAM::Render(1, sHanaWigglerFieldSprites[idx], s.vc.x >> 12, s.vc.y >> 12, -1,
                                -1, 0x1000, (unsigned short)(short)(rot & 0xf800));
                }
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}

#pragma push
#pragma O3 /* the default schedule misses */
void dScMgHanachan_c::Wiggler::Update() {
    (this->*sub)();
    if (face != 5)
        return;
    popCount += 1;
    {
        int i = 0;
        int max = 0x200;
        int *p = stretch;
        do {
            if (popCount > (5 - i) * 3)
                Math_Function_0203b0fc(p, 0x9800, 0x400, max);
            i++;
            max += 0x140;
            p++;
        } while (i < 5);
    }
    StepSegmentsB();
}
#pragma pop
void dScMgHanachan_c::Wiggler::StateHold() {
    if (ApproachLinear2(hold, 0, 1)) {
        bloom = 0;
        busy = 1;
        flash = 0x21;
        speedTarget = 0x1000;
        bobRate = 0x900;
        face = 6;
    } else {
        (this->*sub)();
        if (hold == 0x30) {
            if (face != 4)
                face = 3;
        }
    }

    if (face != 6)
        return;

    popCount += 1;

    int i = 0;
    int scale = 0x200;
    int *q = stretch;
    do {
        if (popCount > (5 - i) * 3) {
            Math_Function_0203b0fc(q, 0x9800, 0x400, scale);
        }
        scale += 0x140;
        q += 1;
        i++;
    } while (i < 5);

    StepSegmentsB();
}

void dScMgHanachan_c::Wiggler::StateMoveA() {
    Vector2 va;
    Vector2 vb;
    short tgt;

    ApproachLinear(flower, 0, 0x200);
    if (ApproachLinear2(timer, 0, 1) != 0) {
        if (sub == &Wiggler::TurnTowardAim) {
            bloom = 1;
            flower = 0x3000;
            busy = 0;
            face = 0;
        }
        return;
    }
    va.x = 0;
    va.y = -0x10000;
    tgt = aim - 0x1000;
    ApproachLinear(ang[0], tgt, 0x400);
    func_0203d388(&va, ang[0]);
    func_0203d704(&vb, &seg[1], &va);
    seg[0].x = vb.x;
    seg[0].y = vb.y;
    {
        short cnt = timer;
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            face = 1;
        else
            face = 0;
    }
}

void dScMgHanachan_c::Wiggler::StateWait() {
    short *t = &hold;
    short v = (short)(*t - 1);
    *t = v;
    if (hold == 0) {
        bloom = 0;
        busy = 1;
        speedTarget = 0x1000;
        bobRate = 0x900;
        face = 4;
        flash = 0x78;
        {
            state = sub;
        }
        return;
    }
    (this->*sub)();
    if (hold != 0x30)
        return;
    if (decoy == 0)
        Sound::PlayBank2_2D(0x1ea);
    face = 3;
}

void dScMgHanachan_c::Wiggler::StateMoveB() {
    Vector2 va;
    Vector2 vb;
    short tgt;
    short cnt;

    ApproachLinear(flower, 0, 0x200);
    timer -= 1;
    if (timer == 0) {
        if (sub == &Wiggler::TurnTowardAim) {
            bloom = 1;
            flower = 0x3000;
            busy = 0;
            face = 0;
        } else {
            bloom = 0;
            busy = 0;
            speedTarget = 0xa00;
            bobRate = 0x600;
            face = 2;
            voice = 0;
            voiceId = 0x1ec;
        }
        state = sub;
        return;
    }
    va.x = 0;
    va.y = -0x10000;
    tgt = aim - 0x1000;
    ApproachLinear(ang[0], tgt, 0x400);
    func_0203d388(&va, ang[0]);
    func_0203d704(&vb, &seg[1], &va);
    seg[0].x = vb.x;
    seg[0].y = vb.y;
    cnt = timer;
    if ((cnt & 0x10) == 0 || cnt < 0x38)
        face = 1;
    else
        face = 0;
}

void dScMgHanachan_c::Wiggler::UpdateSway() {
    if (state == &Wiggler::TurnTowardAim && decoy == 0) {
        short h = bob;
        if (h < 0 && (short)(h + bobRate) > 0) {
            Sound::PlayBank2_2D(0x1e8);
        } else if (h > 0 && (short)(h + bobRate) < 0) {
            Sound::PlayBank2_2D(0x1e7);
        }
    }
    bob += bobRate;
}

void dScMgHanachan_c::Wiggler::Move() {
    Vector2 v;
    int step;
    int v93;

    if (alive != 0) {
        step = 0x1000;
        if (speed < 0xa00)
            step = step >> 2;
        ApproachLinear(speed, speedTarget, step);
        v.y = -speed;
        v.x = 0;
        func_0203d388(&v, ang[1]);
        seg[1].x += v.x;
        seg[1].y += v.y;
        if (busy != 0) {
            flash--;
            if (flash == 0) {
                SetCruiseSpeed();
            }
        }
    }
    if (voiceId != 0 && decoy == 0) {
        v93 = 0;
        if (busy != 0)
            v93 = 0x1f4;
        voice = func_02012468(voice, 2, voiceId, 2, 0, v93, 0, 0);
    } else {
        voice = 0;
    }
    if (state)
        (this->*state)();
    if (alive == 0)
        return;
    StepSegmentsA();
}

int dScMgHanachan_c::Wiggler::HitTest(const Vector2 *touchPoint) {
    int segmentIndex;
    for (segmentIndex = 0; segmentIndex < 5; segmentIndex++) {
        if (func_0203d5dc(&seg[segmentIndex], touchPoint) < 0x14000)
            return 1;
    }
    return 0;
}

int dScMgHanachan_c::Wiggler::IsStarVisible() {
    if (busy == 0) {
        if (state == saved)
            return 1;
    }
    return 0;
}

void dScMgHanachan_c::Wiggler::StopIfIdle() {
    if (state == &Wiggler::TurnTowardAim) {
        busy = 0;
        timer = 96;
        if (decoy == 0) {
            Sound::PlayBank2_2D(0x1e9);
        }
        state = &Wiggler::StateMoveA;
    } else {
        if (state != &Wiggler::StateWait) {
            hold = 0x31;
            sub = saved;
            Sound::PlayBank2_2D(0x1ed);
        }
        popCount = 0;
        state = &Wiggler::StateHold;
    }
}

void dScMgHanachan_c::Wiggler::OnTouched() {
    if (state == &Wiggler::TurnTowardAim) {
        busy = 0;
        timer = 96;
        if (decoy == 0)
            Sound::PlayBank2_2D(0x1e9);
        state = &Wiggler::StateMoveA;
    } else {
        face = 5;
        popCount = 0;
        sub = saved;
        state = &Wiggler::Update;
    }
}

void dScMgHanachan_c::Wiggler::SetCruiseSpeed() {
    speedTarget = 0xa00;
    bobRate = 0x600;
    flash = 0;
    busy = 0;
    bloom = 0;
    face = 2;
}

void dScMgHanachan_c::Wiggler::Retarget(int v) {
    tries++;
    if (saved == &Wiggler::TurnTowardAim && tries < 3) {
        busy = 0;
        timer = (RandomIntInternal(&data_0209e650.value) & 0x20) + 0x60;
        if (tries == 1) {
            sub = &Wiggler::TurnTowardAim;
            saved = sub;
        } else {
            sub = &Wiggler::MoveBounce;
            saved = sub;
        }
        if (decoy == 0) {
            Sound::PlayBank2_2D(0x1e9);
        }
        state = &Wiggler::StateMoveB;
    } else {
        speed = 0;
        hold = (v >> 12) / 4 + 0x31;
        sub = saved;
        state = &Wiggler::StateWait;
    }
}

/* Second parameter unused by the body -- DrawField passes an index
   this callee ignores; declaring it keeps the caller's argument setup. */
int dScMgHanachan_c::Wiggler::GetFrame(int) {
    int frame = 0;
    if (busy != 0) {
        if (flash > 0x20) {
            frame = 1;
        } else if (flash > 0x10) {
            face = 3;
            frame = 5;
        } else if (flash > 0) {
            frame = 4;
        }
    } else if (state == &Wiggler::StateWait || state == &Wiggler::StateHold) {
        if (hold < 0x10)
            frame = 5;
        else if (hold < 0x20)
            frame = 4;
        else
            frame = 0;
    } else {
        frame = 0;
    }
    return frame;
}

int dScMgHanachan_c::Wiggler::GetFlag(int n) {
    switch (n) {
    case 0:
        return face;
    case 1:
        return ((struct B72 *)&color)->f0 + 7;
    case 2:
        return ((struct B72 *)&color)->f1 + 7;
    case 3:
        return ((struct B72 *)&color)->f2 + 7;
    default:
        return ((struct B72 *)&color)->f3 + 7;
    }
}

int Hana_PickUnused(int n, int mask) {
    int v;
    int i;
    int found = 0;

    for (i = 0; i < sHanaPickedCount; i++) {
        if (n == sHanaPickedIds[i]) {
            found = 1;
            break;
        }
    }

    v = (unsigned short)(mask & RandomIntInternal(&data_0209e650.value));
    if (found)
        v = sHanaPickedLow | (unsigned short)(v & 0xf0);

    for (i = 0; i < n; i++) {
        if (v == sHanaUsedIds[i]) {
            v = (unsigned short)(mask & RandomIntInternal(&data_0209e650.value));
            if (found)
                v = sHanaPickedLow | (unsigned short)(v & 0xf0);
            i = -1;
        }
    }

    sHanaUsedIds[n] = v;
    return v;
}

void Hana_ChooseStarWiggler(int count) {
    int i, j;
    fx32 r;
    sHanaPickedLow = (short)(RandomIntInternal(&data_0209e650.value) & 0xf);
    sHanaPickedCount = (short)count;
    for (i = 0; i < count; i++) {
        r = RAND_FX();
        sHanaPickedIds[i] = (unsigned short)(r * sHanaFieldWigglerCount >> 12);
        j = 0;
        while (j < i) {
            if (sHanaPickedIds[j] == sHanaPickedIds[i]) {
                r = RAND_FX();
                sHanaPickedIds[i] = (unsigned short)(r * sHanaFieldWigglerCount >> 12);
                j = 0;
            } else {
                j++;
            }
        }
    }
}

int Hana_PickMoveKind(void) {
    int v = 3;
    fx32 r = RAND_FX();
    if (r < 0x400)
        v = 0;
    else if (r < 0x800)
        v = 1;
    else if (r < 0xc00)
        v = 2;
    return v;
}

int dScMgHanachan_c::Wiggler::ClampToScreen() {
    int boundaryMask = 0;
    int coordinate = seg[1].x;
    if (coordinate >= 0xe8000) {
        boundaryMask |= 1;
        coordinate = 0xe8000;
        seg[1].x = coordinate;
        boundaryMask &= 0xff;
    } else if (coordinate <= 0x18000) {
        boundaryMask |= 2;
        coordinate = 0x18000;
        seg[1].x = coordinate;
        boundaryMask &= 0xff;
    }
    coordinate = seg[1].y;
    if (coordinate >= 0xa8000) {
        boundaryMask |= 8;
        coordinate = 0xa8000;
        seg[1].y = coordinate;
        boundaryMask &= 0xff;
    } else if (coordinate <= 0x18000) {
        boundaryMask |= 4;
        coordinate = 0x18000;
        seg[1].y = coordinate;
        boundaryMask &= 0xff;
    }
    return boundaryMask;
}

void dScMgHanachan_c::Wiggler::StepSegmentsA() {
    Vector2 segmentOffset;
    Vector2 headOffset;
    Vector2 segmentPosition;
    Vector2 headPosition;
    int segmentIndex;
    s16 targetAngle;
    int previousIndex;
    s16 bendExcess;
    int angleCorrection;
    Vector2 *previousSegment;
    int zero;
    int step;
    int angleStep;
    Vector2 *segments;

    if (bloom != 0)
        return;

    UpdateAngle(ang[1], aim, 8, 0x100);

    segmentIndex = 2;
    step = 2;
    zero = 0;
    angleStep = 0x100;

    for (; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        previousSegment = &seg[previousIndex];
        Vec2_Sub(&segmentOffset, &seg[segmentIndex], previousSegment);
        if (func_0203d434(&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, angleDifference;
            func_0203d630(&segmentOffset, stretch[segmentIndex] + segmentIndex * 0x2000);
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = ang[previousIndex];
            bendExcess = zero;
            targetAngle = (s16)(-offsetAngle);
            angleDifference = (s16)(previousAngle - (s16)(-offsetAngle));
            if (angleDifference > 0x2000) {
                bendExcess = (s16)(angleDifference - 0x2000);
                targetAngle = (s16)(previousAngle - 0x2000);
            } else if (angleDifference < -0x2000) {
                bendExcess = (s16)(angleDifference + 0x2000);
                targetAngle = (s16)(previousAngle + 0x2000);
            }
            UpdateAngle(ang[segmentIndex], targetAngle, step, angleStep);
            angleCorrection = ang[segmentIndex];
            angleCorrection = targetAngle - angleCorrection;
            func_0203d388(&segmentOffset, (s16)(bendExcess - angleCorrection));
        }
        func_0203d704(&segmentPosition, previousSegment, &segmentOffset);
        seg[segmentIndex].x = segmentPosition.x;
        seg[segmentIndex].y = segmentPosition.y;
    }

    {
        s16 dd = (s16)(aim - ang[1]);
        headOffset.x = 0;
        headOffset.y = -0x10000;
        UpdateAngle(ang[0], (s16)(ang[1] + dd), 8, 0x100);
    }
    func_0203d388(&headOffset, ang[0]);
    segments = seg;
    func_0203d704(&headPosition, segments + 1, &headOffset);
    seg[0].x = headPosition.x;
    seg[0].y = headPosition.y;
}

void dScMgHanachan_c::Wiggler::StepSegmentsB() {
    Vector2 segmentOffset;
    Vector2 headOffset;
    Vector2 segmentPosition;
    Vector2 headPosition;
    int segmentIndex = 4;
    Vector2 *segment = &seg[segmentIndex];
    int baseSpacing = 0x8000;
    int previousIndex;

    for (; segmentIndex >= 1; segmentIndex--) {
        previousIndex = segmentIndex - 1;
        Vec2_Sub(&segmentOffset, segment, &seg[previousIndex]);
        if (func_0203d434(&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, targetAngle, bendExcess, angleDifference, currentAngle;
            {
                int extraSpacing = stretch[segmentIndex];
                func_0203d630(&segmentOffset, extraSpacing + baseSpacing);
            }
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = ang[previousIndex];
            bendExcess = 0;
            targetAngle = (s16)(-offsetAngle);
            angleDifference = (s16)(previousAngle - (s16)(-offsetAngle));
            if (angleDifference > 0x2000) {
                bendExcess = (s16)(angleDifference - 0x2000);
                targetAngle = (s16)(previousAngle - 0x2000);
            } else if (angleDifference < -0x2000) {
                bendExcess = (s16)(angleDifference + 0x2000);
                targetAngle = (s16)(previousAngle + 0x2000);
            }
            currentAngle = ang[segmentIndex];
            func_0203d388(&segmentOffset, (s16)(bendExcess - (targetAngle - currentAngle)));
        }
        Vec2_Sub(&segmentPosition, segment, &segmentOffset);
        seg[previousIndex].x = segmentPosition.x;
        seg[previousIndex].y = segmentPosition.y;
        segment--;
        baseSpacing -= 0x2000;
    }
    headOffset.x = 0;
    headOffset.y = -0x10000;
    func_0203d388(&headOffset, ang[0]);
    /* The (Vector2 *) spelling of the array decay is load-bearing: `&seg[1]`
       and `seg + 1` both fold the head address differently (-1 instruction,
       12 words moved, measured). */
    func_0203d704(&headPosition, (Vector2 *)(seg) + 1, &headOffset);
    seg[0].x = headPosition.x;
    seg[0].y = headPosition.y;
}

void dScMgHanachan_c::Wiggler::StepSegmentsC() {
    Vector2 segmentOffset;
    Vector2 headOffset;
    Vector2 segmentPosition;
    Vector2 headPosition;
    int segmentIndex = 2;
    Vector2 *segment = &seg[segmentIndex];
    int previousIndex;

    for (; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        Vec2_Sub(&segmentOffset, segment, &seg[previousIndex]);
        if (func_0203d434(&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, targetAngle, bendExcess, angleDifference, currentAngle;
            func_0203d630(&segmentOffset, (segmentIndex * 2 + 0x10) << 12);
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = ang[previousIndex];
            bendExcess = 0;
            targetAngle = (s16)(-offsetAngle);
            angleDifference = (s16)(previousAngle - (s16)(-offsetAngle));
            if (angleDifference > 0x2000) {
                bendExcess = (s16)(angleDifference - 0x2000);
                targetAngle = (s16)(previousAngle - 0x2000);
            } else if (angleDifference < -0x2000) {
                bendExcess = (s16)(angleDifference + 0x2000);
                targetAngle = (s16)(previousAngle + 0x2000);
            }
            ang[segmentIndex] = targetAngle;
            currentAngle = ang[segmentIndex];
            func_0203d388(&segmentOffset, (s16)(bendExcess - (targetAngle - currentAngle)));
        }
        func_0203d704(&segmentPosition, &seg[previousIndex], &segmentOffset);
        seg[segmentIndex].x = segmentPosition.x;
        seg[segmentIndex].y = segmentPosition.y;
        segment++;
    }
    headOffset.x = 0;
    headOffset.y = -0x10000;
    ang[0] = (s16)(ang[1] + (((ang[1] - ang[2]) << 14) >> 16));
    func_0203d388(&headOffset, ang[0]);
    func_0203d704(&headPosition, (Vector2 *)(seg) + 1, &headOffset);
    seg[0].x = headPosition.x;
    seg[0].y = headPosition.y;
}

void dScMgHanachan_c::Wiggler::CheckOffLeftA() {
    int i;
    s16 v = aim;
    if (v < 0 && seg[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            seg[i].x += 0x170000;
        return;
    }
    if (v > 0 && seg[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            seg[i].x -= 0x170000;
    }
}

void dScMgHanachan_c::Wiggler::StartPathA(int n) {
    int b = n % 3;
    int a = n / 3;
    int rA, rB, rC, rD;
    int g1;
    int cm, dm;
    Vector2 base;

    speed = 0xa00;
    speedTarget = 0xa00;
    g1 = sHanaFieldWigglerCount / 3 - 1;

    rA = FX_Div(a << 12, g1 << 12);
    rB = FX_Div((g1 - a) << 12, g1 << 12);
    rC = FX_Div(b << 12, 0x2000);
    rD = FX_Div((2 - b) << 12, 0x2000);

    if (a & 1) {
        aim = -0x4000;
        bob = -0x8000;
        cm = 0x8000;
        dm = 0xfe000;
    } else {
        aim = 0x4000;
        bob = 0;
        cm = 0x100000;
        dm = 0xa000;
    }

    {
        int i = 0;
        int delta;

        base.x = 0;
        base.y = 0;
        delta = 0;
        base.x = FX_MUL(cm, rC) + FX_MUL(dm, rD);
        base.y = (int)((((long long)rA << 16) + 0x800) >> 12) + FX_MUL(rB, 0xb0000);

        for (; i < 5; i++) {
            seg[i].x = base.x;
            seg[i].y = base.y;
            if (aim < 0)
                seg[i].x += delta;
            else
                seg[i].x -= delta;
            ang[i] = aim;
            delta += 0x20000;
        }
    }

    StepSegmentsC();
    voice = 0;
    voiceId = 0x1ec;
    state = &Wiggler::CheckOffLeftA;
    saved = state;
}

void dScMgHanachan_c::Wiggler::MoveBounce() {
    int boundaryMask = ClampToScreen();
    if (boundaryMask != 0) {
        int targetX;
        int targetY;
        int y;
        int x;
        s16 targetAngle;
        s16 angleDifference;
        x = seg[1].x;
        y = seg[1].y;
        targetX = x;
        targetY = y;
        /* Keep the matched OR expressions: any boundary hit targets the centre. */
        if (boundaryMask | 3)
            targetX = 0x80000;
        if (boundaryMask | 0xc)
            targetY = 0x60000;
        targetAngle = cstd::atan2(targetX - x, -(targetY - y));
        angleDifference = (s16)(targetAngle - ang[0]);
        if (angleDifference > 0x2000)
            angleDifference = 0x2000;
        else if (angleDifference < -0x2000)
            angleDifference = -0x2000;
        aim = (s16)(ang[1] + angleDifference);
        return;
    }
    {
        int randomX;
        int randomY;
        s16 targetAngle;
        s16 angleDifference;
        timer -= 1;
        if (timer >= 0)
            return;
        timer = (s16)(RAND_FX_SCALE(0x1e) + 0x3c);
        randomX = RandomIntInternal(&data_0209e650.value);
        randomY = RandomIntInternal(&data_0209e650.value);
        targetAngle =
            cstd::atan2(((int)((unsigned int)(randomX & 0x7fffffff) >> 19) * 0x100) - seg[1].x,
                        -(((int)((unsigned int)(randomY & 0x7fffffff) >> 19) * 0xc0) - seg[1].y));
        angleDifference = (s16)(targetAngle - ang[0]);
        if (angleDifference > 0x2000)
            angleDifference = 0x2000;
        else if (angleDifference < -0x2000)
            angleDifference = -0x2000;
        aim = (s16)(ang[1] + angleDifference);
    }
}

void dScMgHanachan_c::Wiggler::StartBounce(int) {
    int i;
    speed = 0xa00;
    speedTarget = 0xa00;
    for (i = 0; i < 5; i++) {
        seg[i].x = RAND_FX() << 8;
        seg[i].y = RAND_FX() * 0xc0;
        ang[i] = (short)((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10);
    }
    aim = (short)((ang[0] + ang[1]) >> 1);
    ClampToScreen();
    StepSegmentsC();
    timer = 30;
    voice = 0;
    voiceId = 0x1ec;
    state = &Wiggler::MoveBounce;
    saved = state;
}

void dScMgHanachan_c::Wiggler::CheckOffLeftB() {
    int i;
    s16 v = aim;
    if (v < 0 && seg[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            seg[i].x += 0x170000;
        return;
    }
    if (v > 0 && seg[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            seg[i].x -= 0x170000;
    }
}

void dScMgHanachan_c::Wiggler::StartPathB(int idx) {
    int n1;
    int A, B;
    int p;
    int valY;
    Vector2 pos;
    int i;
    int off;

    speed = 0xa00;
    speedTarget = 0xa00;

    n1 = sHanaFieldWigglerCount - 1;
    A = FX_Div(idx << 12, n1 << 12);
    B = FX_Div((n1 - idx) << 12, n1 << 12);

    p = (n1 - 2) << 3;
    valY = FX_MUL(((0x28 - p) << 12), A) + FX_MUL(((p + 0x98) << 12), B);

    pos.y = 0;
    pos.x = 0;
    pos.y = valY;

    if (RandomIntInternal(&data_0209e650.value) & 0x8000000) {
        aim = -0x4000;
        pos.x = 0x80000;
        pos.x += RAND_FX() << 7;
    } else {
        aim = 0x4000;
        pos.x = 0x80000;
        pos.x -= RAND_FX() << 7;
    }

    for (i = 0, off = 0; i < 5; i++) {
        seg[i].x = pos.x;
        seg[i].y = pos.y;
        if (aim < 0)
            seg[i].x += off;
        else
            seg[i].x -= off;
        ang[i] = aim;
        off += 0x20000;
    }

    StepSegmentsC();
    voice = 0;
    voiceId = 0x1ec;
    state = &Wiggler::CheckOffLeftB;
    saved = state;
}

void dScMgHanachan_c::Wiggler::TurnTowardAim() {
    ApproachLinear(ang[0], (short)(aim - 0x2000), 0x100);
}

void dScMgHanachan_c::Wiggler::StartPathC(int wigglerIndex) {
    Vector2 initialHead;
    Vector2 segmentOffset;
    Vector2 headOffset;
    Vector2 segmentPosition;
    Vector2 headPosition;
    int lastIndex, indexWeight, reverseWeight, spacingOffset, segmentIndex, previousIndex;

    bloom = 1;
    flower = 0x3000;
    face = 0;
    bobRate = 0x200;
    speed = 0;
    speedTarget = 0;

    lastIndex = sHanaFieldWigglerCount - 1;
    indexWeight = FX_Div(wigglerIndex << 12, lastIndex << 12);
    reverseWeight = FX_Div((lastIndex - wigglerIndex) << 12, lastIndex << 12);
    spacingOffset = (lastIndex - 2) * 8;

    initialHead.y = 0;
    initialHead.y =
        (int)(((long long)((0x40 - spacingOffset) << 12) * indexWeight + 0x800) >> 12) +
        (int)(((long long)((spacingOffset + 0x98) << 12) * reverseWeight + 0x800) >> 12);
    initialHead.x = 0;
    if (wigglerIndex & 1)
        initialHead.x = 0xa8000;
    else
        initialHead.x = 0x38000;

    aim = -0x8000;
    seg[0] = initialHead;
    ang[0] = aim - 0x2000;

    for (segmentIndex = 1; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        segmentOffset.x = 0;
        segmentOffset.y = (segmentIndex * 2 + 0x10) << 12;
        func_0203d388(&segmentOffset, (s16)(ang[previousIndex] + 0x2000));
        ang[segmentIndex] = ang[previousIndex] + 0x2000;
        func_0203d704(&segmentPosition, &seg[previousIndex], &segmentOffset);
        seg[segmentIndex].x = segmentPosition.x;
        seg[segmentIndex].y = segmentPosition.y;
    }

    StepSegmentsC();

    headOffset.x = 0;
    headOffset.y = -0x10000;
    ang[0] = aim - 0x2000;
    func_0203d388(&headOffset, ang[0]);
    func_0203d704(&headPosition, &seg[1], &headOffset);
    seg[0].x = headPosition.x;
    seg[0].y = headPosition.y;

    state = &Wiggler::TurnTowardAim;
    saved = state;
}

void dScMgHanachan_c::Wiggler::Spawn(int slot, int roundNo) {
    int i;

    voice = 0;
    voiceId = 0;
    decoy = 1;
    index = slot;
    tries = 0;
    bob = (short)(RandomIntInternal(&data_0209e650.value) & 0xfff0);
    bobRate = 0x600;
    busy = 0;
    bloom = 0;
    face = 2;
    round = roundNo;
    alive = 1;

    for (i = 0; i < 5; i++)
        stretch[i] = 0x10000;

    color = (short)Hana_PickUnused(slot, 0xff);

    switch (sHanaRoundPattern) {
    case 0:
        StartPathC(slot);
        return;
    case 1:
        StartPathB(slot);
        return;
    case 2:
        StartBounce(slot);
        return;
    case 3:
    default:
        StartPathA(slot);
        return;
    }
}

s32 dScMgHanachan_c::CleanupResources() {
    Mg_CommitRecords();
    return 1;
}

s32 dScMgHanachan_c::Render() {
    DrawScoreTally(0xe0, 0x14, 1);
    HanaPopup_DrawAll();
    StarPopup_Draw();

    if (mPhase == &dScMgHanachan_c::Phase_Play) {
        for (int i = 0; i < sHanaFieldWigglerCount; i++)
            mWiggler[i].DrawField();
    } else if (mPhase == &dScMgHanachan_c::Phase_Result) {
        if ((mHit != 0 && mPhaseTimer < 90) || (mHit == 0 && mPhaseTimer < 138)) {
            mTarget->DrawField();
        } else {
            for (int i = 0; i < sHanaFieldWigglerCount; i++)
                mWiggler[i].DrawField();
        }
    } else if (mPhase == &dScMgHanachan_c::Phase_NoneB ||
               mPhase == &dScMgHanachan_c::Phase_WaitTouch) {
        mTarget->DrawField();
    }

    mTarget->DrawTarget();

    if (mPhase != &dScMgHanachan_c::Phase_WaitIntro) {
        int idx = GetGameLanguage();
        OamAttr **e = sHanaTextSpritesByLanguage[idx];
        OamAttr *f = e[3];
        DrawOamSprite(f, 128, 16, 0);
        Mg_DrawNumberRotated(0x80, 0x28, mTimeLeft, 1, -1, 0x800, 0);
    }

    return 1;
}

s32 dScMgHanachan_c::Behavior() {

    sHanaSpinAngle = sHanaSpinAngle + 0x800;
    (this->*mPhase)();
    int i = 0;
    if (sHanaFieldWigglerCount > 0) {
        do {
            mWiggler[i].Move();
            i++;
        } while (i < sHanaFieldWigglerCount);
    }
    int j = 0;
    if (sHanaFieldWigglerCount > 0) {
        do {
            mWiggler[j].UpdateSway();
            j++;
        } while (j < sHanaFieldWigglerCount);
    }
    HanaPopup_UpdateAll();
    StarPopup_Update(&mTimeLeft);
    return 1;
}

void dScMgHanachan_c::Phase_NoneA() {}

void dScMgHanachan_c::Phase_WaitTouch() {
    int idx;
    int b;
    mPhaseTimer -= 1;
    if (mPhaseTimer != 0) {
        idx = gActivePlayerSlot;
        b = 0;
        if (gTouchInfo[idx].held != 0 && gTouchInfo[idx].edge != 0)
            b = 1;
        if (b == 0)
            return;
    }
    mPhase = &dScMgHanachan_c::Phase_NoneA;
}

void dScMgHanachan_c::BeginResultWait() {
    mPhaseTimer = 180;
    mPhase = &dScMgHanachan_c::Phase_WaitTouch;
}

void dScMgHanachan_c::Phase_NoneB() {}

void dScMgHanachan_c::SetPhaseIdle() {
    mPhase = &dScMgHanachan_c::Phase_NoneB;
}

void dScMgHanachan_c::Phase_Result() {
    int v;
    mPhaseTimer -= 1;
    v = mPhaseTimer;
    if (v == 0) {
        if (mHit != 0) {
            Mg_SetSharedState(MG_STATE_NEXT_ROUND);
            SetPhaseIdle();
        } else {
            mTarget->decoy = 1;
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            BeginResultWait();
        }
        mPromptEnabled = 0;
    } else if (mHit != 0) {
        if (v == 0x5a) {
            mTarget->OnTouched();
        }
    } else {
        if (v == 0x8a) {
            mTarget->StopIfIdle();
        }
    }
}

void dScMgHanachan_c::BeginResult() {
    int i = 0;
    if (i < sHanaFieldWigglerCount) {
        do {
            mWiggler[i].alive = 0;
            i++;
        } while (i < sHanaFieldWigglerCount);
    }
    if (mHit != 0)
        mPhaseTimer = 180;
    else
        mPhaseTimer = 228;
    mPhase = &dScMgHanachan_c::Phase_Result;
}

void dScMgHanachan_c::Phase_Play() {
    Vector2 a;
    Vector2 vec;
    Vector2 popup;
    int b = 0;
    int idx = gActivePlayerSlot;

    if (gTouchInfo[idx].held != 0 && gTouchInfo[idx].edge != 0)
        b = 1;

    if (b != 0) {
        Wiggler *p;
        Vector2 *src;
        a.x = gTouchInfo[idx].x << 12;
        a.y = gTouchInfo[idx].y << 12;
        p = mTarget;
        src = p->seg;
        vec.x = src->x;
        vec.y = src->y;

        if (mTarget->IsStarVisible() != 0 && mTarget->HitTest(&a) != 0) {
            mHit = 1;
            func_02012790(0x26);
            p = mTarget;
            {
                if (p->state == &Wiggler::TurnTowardAim)
                    Sound::PlayBank2_2D(0x1ef);
                else
                    Sound::PlayBank2_2D(0x1ee);
                if (sHanaRoundPattern == 0)
                    vec.x -= 0x28000;
                StarPopup_Start(vec.x, vec.y);
                BeginResult();
                return;
            }
        }

        {
            int found = -1;
            int i = 0;
            Wiggler *ptr;

            if (sHanaFieldWigglerCount > 0) {
                ptr = mWiggler;
                do {
                    if (ptr->IsStarVisible() != 0 && ptr->HitTest(&a) != 0) {
                        found = i;
                        break;
                    }
                    i++;
                    ptr++;
                } while (i < sHanaFieldWigglerCount);
            }

            if (found != -1) {
                int j;
                Wiggler *ptr2;
                Wiggler *sel = mWiggler + found;
                {
                    if (sel->state == &Wiggler::TurnTowardAim)
                        Sound::PlayBank2_2D(0x1eb);
                    else
                        Sound::PlayBank2_2D(0x1ed);
                }

                {
                    j = 0;
                    if (sHanaFieldWigglerCount > 0) {
                        ptr2 = mWiggler;
                        do {
                            Vector2 *s2 = ptr2->seg;
                            vec.x = s2->x;
                            vec.y = s2->y;
                            ptr2->Retarget(func_0203d5dc(&vec, &a));
                            j++;
                            ptr2++;
                        } while (j < sHanaFieldWigglerCount);
                    }
                }

                func_02012790(0xe);
                {
                    Vector2 *s3 = sel->seg;
                    popup.x = s3->x;
                    popup.y = s3->y;
                }
                if (sHanaRoundPattern == 0)
                    popup.x -= 0x28000;
                HanaPopup_Spawn(popup.x, popup.y);

                {
                    int t = mTimeLeft - 10;
                    if (t < 0)
                        t = 0;
                    mTimeLeft = t;
                }
            }
        }
    }

    if (ApproachLinear(mTimeTick, 0, 1) == 0)
        return;

    mTimeTick = 0x3c;
    if (ApproachLinear(mTimeLeft, 0, 1) != 0) {
        func_02012790(0xe);
        BeginResult();
    }

    if (mTimeLeft > 2)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
}

void dScMgHanachan_c::BeginPlay() {
    mPhaseTimer = 1200;
    mPhase = &dScMgHanachan_c::Phase_Play;
}

void dScMgHanachan_c::Phase_WaitIntro() {
    if (ApproachLinear(mDelay, 0, 1) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    BeginPlay();
}

void dScMgHanachan_c::Phase_SetupRound() {
    if ((s32)mLevel < 0xa) {
        int bc = mLevel;
        sHanaRoundPattern = sHanaRoundPatterns[bc];
        sHanaFieldWigglerCount = sHanaFieldWigglerCounts[bc];
    } else {
        int ret = Hana_PickMoveKind();
        sHanaRoundPattern = ret;
        if (ret == 3) {
            sHanaFieldWigglerCount = 0xf;
        } else {
            unsigned int rnd = RandomIntInternal(&data_0209e650.value) & 0x7fffffff;
            sHanaFieldWigglerCount = ((int)(rnd >> 19) * 3 >> 12) + 3;
        }
    }

    if (mLevel < 0xa)
        Hana_ChooseStarWiggler(2);
    else
        Hana_ChooseStarWiggler(3);

    {
        int i = 0;
        if (sHanaFieldWigglerCount > 0) {
            Wiggler *p = mWiggler;
            do {
                p->Spawn(i, mLevel);
                i++;
                p++;
            } while (i < sHanaFieldWigglerCount);
        }
    }

    {
        unsigned int rnd = RandomIntInternal(&data_0209e650.value) & 0x7fffffff;
        int k = (int)(rnd >> 19) * sHanaFieldWigglerCount >> 12;
        mTarget = mWiggler + k;
        mTarget->decoy = 0;
        mDelay = 60;
        Mg_ShowBanner(0xd, 0x80, 0x50, 1, -1, 0xd);
        mTimeTick = 0x3c;
        mHit = 0;
        mPhase = &dScMgHanachan_c::Phase_WaitIntro;
    }
}

/* StartRound steps the level with a read through this accessor, and the
   cartridge re-issues ldr r1,[r4,#0xbc] for it. A plain `mLevel + 1` lets
   mwccarm keep the +0xbc field address in its own register (add r2,r4,#0xbc /
   ldr [r2] / str [r2]), a word longer. */
inline u32 dScMgBase_c::GetLevel() const {
    return mLevel;
}

void dScMgHanachan_c::StartRound(int state) {
    if (state == 1) {
        mLevel = GetLevel() + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else if (state == 0x12) {
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
        if (gMgScene != 0)
            gMgScene->mHudScore = 0;
        Mg_SubmitScore(mHudScore);
        mTimeLeft = 0x14;
    } else {
        int v = mLevel;
        if (gMgScene != 0)
            gMgScene->mHudScore = v;
        mTimeLeft = 0x14;
    }
    HanaPopup_ClearAll();
    StarPopup_Clear();
    Phase_SetupRound();
}

s32 dScMgHanachan_c::InitResources() {
    void *ncg;
    void *ncl;

    Hana_LoadBackground();
    ncg = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCG_BIN);
    ncl = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCL_BIN);
    DecompressLZ16(ncg, G2_GetOBJCharPtr());
    DecompressLZ16(ncg, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(ncl, 0, 0xc0);
    GX_LoadOBJPltt(ncl, 0, 0xc0);
    Deallocate(ncg);
    Deallocate(ncl);
    mCredits = data_ov006_0213c954;
    mCreditsShown = mCredits;
    Mg_SetTopScreenYOffset(0x20);
    StartRound(-1);
    *sHanaBackgroundParts[0] = (*sHanaBackgroundParts[0] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[1] = (*sHanaBackgroundParts[1] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[2] = (*sHanaBackgroundParts[2] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[3] = (*sHanaBackgroundParts[3] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[4] = (*sHanaBackgroundParts[4] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[5] = (*sHanaBackgroundParts[5] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[6] = (*sHanaBackgroundParts[6] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[7] = (*sHanaBackgroundParts[7] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[8] = (*sHanaBackgroundParts[8] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[9] = (*sHanaBackgroundParts[9] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[10] = (*sHanaBackgroundParts[10] & 0xc1fffcff) | 0x100;
    *sHanaBackgroundParts[11] = (*sHanaBackgroundParts[11] & 0xc1fffcff) | 0x100;
    return 1;
}

extern "C" {
void Hana_LoadBackground(void) {
    void *ncg = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NCG_BIN);
    void *nsc = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NSC_BIN);
    void *ncl = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NCL_BIN);
    G2_SetBG3Priority(1);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                         GX_BG_CHARBASE_0x14000);
    DecompressLZ16(ncg, (void *)G2_GetBG3CharPtr());
    GX_LoadBG3Scr(nsc, 0, 0x800);
    GX_LoadBGPltt(ncl, 0x60, 0x1a0);
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    Deallocate(ncg);
    Deallocate(nsc);
    Deallocate(ncl);

    ncg = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NCG_BIN);
    nsc = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NSC_BIN);
    ncl = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NCL_BIN);
    G2S_SetBG3Priority(2);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x14000);
    DecompressLZ16(ncg, G2S_GetBG3CharPtr());
    GXS_LoadBG3Scr(nsc, 0, 0x800);
    GXS_LoadBGPltt(ncl, 0x60, 0x1a0);
    gSubScreenLayers |= 8;
    Deallocate(ncg);
    Deallocate(nsc);
    Deallocate(ncl);
}

/* The OamAttr* table OAM::Render indexes: the five scalar attributes, the
 * two sHanaBackgroundPartsB entries, then every third pointer out of the
 * vu32* ring sHanaBackgroundParts. Non-constant initializers, so the
 * compiler emits __sinit_d_s_mg_hanachan_methods.cpp to copy them at load. */
OamAttr data_ov006_021374bc[1] = {{0x80e0, 0xc1f0, 0x0018, 0xffff}};
OamAttr *data_ov006_021374b8 = data_ov006_021374bc;
OamAttr data_ov006_021374c8[1] = {{0x80e0, 0xc1f0, 0x0100, 0xffff}};
OamAttr *data_ov006_021374c4 = data_ov006_021374c8;
OamAttr data_ov006_021374b0[1] = {{0x80e0, 0xc1f0, 0x0000, 0xffff}};
OamAttr *data_ov006_021374ac = data_ov006_021374b0;
extern OamAttr *data_ov006_021374e8;
OamAttr data_ov006_021374f8[1] = {{0x80e0, 0xc1f0, 0x1006, 0xffff}};
OamAttr *data_ov006_021374f4 = data_ov006_021374f8;
OamAttr data_ov006_021375d4[1] = {{0x80e8, 0xc1f0, 0x010e, 0xffff}};
OamAttr data_ov006_021375cc[1] = {{0x80e8, 0xc1f0, 0x1112, 0xffff}};
OamAttr *sHanaBackgroundPartsB[] = {data_ov006_021375d4, data_ov006_021375cc};

OamAttr *sWigglerSprites[11] = {
    data_ov006_021374b8,
    data_ov006_021374c4,
    data_ov006_021374ac,
    data_ov006_021374e8,
    data_ov006_021374f4,
    sHanaBackgroundPartsB[0],
    sHanaBackgroundPartsB[1],
    (OamAttr *)sHanaBackgroundParts[0],
    (OamAttr *)sHanaBackgroundParts[3],
    (OamAttr *)sHanaBackgroundParts[6],
    (OamAttr *)sHanaBackgroundParts[9],
};
}
