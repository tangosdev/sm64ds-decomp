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
 * Wiggler_DrawTarget came from one-function files. Their bodies are kept
 * as they matched there. The two popup arrays are declared once; where a
 * function spelled a record differently, its tag is renamed per function
 * and the access casts. The wiggler array's element destructor
 * (formerly Wiggler_Destroy) is Wiggler's own inline destructor.
 *
 * The nine helpers that take the scene as their first argument
 * (Phase_WaitTouch .. Phase_SetupRound, the phase and tap handlers) are
 * dScMgHanachan_c members, declared in dScMgHanachan_c.h; their descriptive
 * names are reconstructed. Each body opens by re-viewing
 * `this` as the char * it used to take. The other func_ov006 helpers stay
 * free: the ones from Wiggler_DrawTarget to Hana_SpawnWigglers take one
 * 0x98-byte wiggler record (an element of mWiggler) or no object at all, the
 * popup helpers before them and Hana_LoadBackground take no object.
 * The nine callback targets on Wiggler and two empty scene phases are real
 * methods; the other Wiggler helpers retain their existing free-function ABI.
 * Native member-pointer literals and the typed spawn profile now provide the
 * complete 0x0213c95c..0x0213ca7c data range. This reconstruction assignment
 * does not establish the original translation-unit boundary.
 *
 * leftovers:
 * - the sixteen popups at sHanaPopups are read through
 *   four record spellings (HanaPopup_DrawAll, HanaPopup_UpdateAll,
 *   HanaPopup_Spawn, HanaPopup_ClearAll).
 * - the popup at sHanaStarPopup is read through the file-local
 *   HanaStarPopup view, and StarPopup_Update reaches its timer byte
 *   through a separate symbol, sHanaStarPopups.
 * - Wiggler_DrawTarget repeats Wiggler_DrawField's segment
 *   walk with its own point type, Vec2_ea914, whose trivial destructor
 *   is part of the match.
 * - dScMgHanachan_c::Render, `Thing *base; &base[i]` and `slot++`
 *   on the 0x98 array, DIFF, 6 words. `char *arr += 0x98` stays.
 *   Behavior's `&base[i]` matches.
 * - BeginResult, `Wiggler *p = (Wiggler *)mWiggler; p->alive = 0; p++`,
 *   DIFF. The loop steps a char * by 0x98 and clears mWiggler[0].alive
 *   through it.
 * - Wiggler_StateMoveA, Wiggler_StateWait, Wiggler_StateMoveB read their
 *   fields through a `Wiggler *w = this` local; dropping the old char *
 *   local instead renumbers the pinned @NNN labels. Wiggler_UpdateSway reads its
 *   fields through a typed Wiggler local; its state test keeps the (Wiggler *)raw
 *   spelling (thiz->state renumbers the pinned @NNN labels).
 * - Wiggler_StepSegmentsA, Wiggler_StepSegmentsB, Wiggler_StepSegmentsC,
 *   Wiggler_MoveBounce, cstd::atan2(Fix12<int>, Fix12<int>), does not link.
 *   cstd::atan2(int, int) mangles Eii, not E5Fix12IiES1_.
 * - Wiggler_DrawField's seg reads and Wiggler_StartPathB's seg / ang loop
 *   step the base pointer (`((Wiggler *)(raw + i * 8))->seg[0]`); direct
 *   seg[i] / ang[i] renumbers six pinned @NNN labels in each.
 *   Wiggler_GetFlag reads the four 2-bit colours in color (+0x74) through
 *   the B72 bitfield view.
 * - Wiggler_DrawField, OAM::Render(int, OamAttr*, int, int, int,
 *   int, int, int), does not typecheck (the sprite table is void*). The
 *   ROM symbol's scale parameter is Fix12<int>, which this tree cannot spell.
 * - Wiggler_StartPathA still uses the private Obj view for its scalar fields.
 *   Its callback assignments now use the real Wiggler member-pointer fields.
 */

/* .text in source order (ROM-ascending; do not reorder) and per-function
   opt pragmas bind: 5x opt_strength_reduction off, one O3, Render
   opt_common_subs off. Without it the 49-function version matched 44/49
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
#include "private/ov006_ec4dc_obj.h"
#include "dScMgHanachan_c.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"
#include "Sound.h"
#include "math/Approach.h"

struct OamAttr;
struct OAM {
    static OamAttr *Render(bool, OamAttr *, int, int, int, int, int, int);
};

extern "C" void *dScMgHanachan_c_classInit();
extern "C" BaseSpawnInfo<void> g_profile_MG_HANACHAN = {dScMgHanachan_c_classInit, 0x182, 0x182};

typedef dScMgHanachan_c::Point V2;
typedef struct {
    int x, y;
} Vec2;
/* Thing is never whole-struct-assigned, so it keeps named ints. */
typedef struct Thing {
    int a;           /* 0x00 */
    int b;           /* 0x04 */
    char pad0[0x10]; /* 0x08 */
    V2 pos;          /* 0x18 */
    char pad1[0x78]; /* 0x20 */
} Thing;             /* 0x98 */

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
   0xa8/0xac as dScMgBase_c's mCredits/mCreditsShown, which is the same
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
    int unkA8;
    int unkAC;
};


/* ROM symbols this TU calls or reads. decl_common.h's declarations are not
   repeated. The C++ helpers must stay outside the extern "C" block so that
   they mangle. */
namespace Sound {
u32 PlayBank2_2D(u32 id);
}
namespace G2S {
char *GetBG3CharPtr();
}
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace cstd {
s16 atan2(s32, s32);
int fdiv(int numerator, int denominator);
}

extern "C" {
/* This TU's own functions. */
extern void Wiggler_DrawField(void *o_);
extern void Wiggler_UpdateSway(char *thiz_);
extern void Wiggler_Move(char *c);
extern int Wiggler_HitTest(char *a, const V2 *b);
extern int Wiggler_IsStarVisible(char *p);
extern void Wiggler_StopIfIdle(char *c);
extern void Wiggler_OnTouched(char *c);
extern void Wiggler_SetCruiseSpeed(char *p);
extern void Wiggler_Retarget(Wiggler *c, int v);
extern int Wiggler_GetFrame(char *o, int);
extern int Wiggler_GetFlag(char *c, int n);
extern int Hana_PickUnused(int n, int mask);
extern void Hana_ChooseStarWiggler(int count);
extern int Hana_PickMoveKind(void);
extern int Wiggler_ClampToScreen(char *c);
extern void Wiggler_StepSegmentsA(Wiggler *c);
extern void Wiggler_StepSegmentsB(Wiggler *c);
extern void Wiggler_StepSegmentsC(Wiggler *c);
extern void Wiggler_StartPathA(Obj *self, int n);
extern void Wiggler_StartBounce(char *c, int);
extern void Wiggler_StartPathB(char *self, int idx);
extern void Wiggler_StartPathC(Wiggler *c, int param);
extern void Hana_SpawnWigglers(char *self, int arg1, int arg2);
extern void Hana_LoadBackground(void);

extern short data_02082214[];
extern void *sWigglerSprites[];
extern void *sHanaWigglerTargetSprites[];
extern void *sHanaWigglerFieldSprites[];
extern void OamRender8_d_s_mg_hanachan(int b, void *attr, int x, int y, int a, int c, int f, int g);

extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
extern short sHanaPickedCount;
extern unsigned short sHanaPickedIds[];
extern unsigned short sHanaUsedIds[];
extern s16 atan2_hold_d_s_mg_hanachan_methods_1(int y, int x);
extern void Mg_CommitRecords(void *);
extern void Mg_DrawScoreTally(void *a, int b, int c, int d);
extern int GetGameLanguage(void);
extern void DrawOamSprite(void *a, int b, int c, int d);
extern void Mg_DrawNumberRotated(int a, int b, void *c, int d, int e, int f, int g);
extern void StarPopup_Start(int x, int y);
extern void HanaPopup_Spawn(int x, int y);
extern s16 sHanaRoundPatterns[];
extern volatile int sHanaFieldWigglerCounts[];
extern u8 data_0209d45c;
extern u8 data_0209d454;

/* These helpers read or write two scalar words. Cast the plain Point
   records at the C ABI boundary rather than giving the externs new types. */
/* Scalar pair ABI agrees with the arm9 definitions; V2 only names this TU workspace. */
extern int func_0203d5dc(const int *a, const int *b);
extern void func_0203d704(int *o, int *a, int *b);
extern void Vec2_Sub(int *o, int *a, int *b);
extern void func_0203d680(Vec2 *out, const Vec2 *in, int scale);
extern int RandomIntInternal(int *seed);
/* data_0209e650 is the RNG state: one file spelled it int[] and passed it bare,
   the rest spelled it int and passed &.  Scalar wins; the address is the same. */
extern int data_0209e650;
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

struct S_ea81c {
    int x;                 /* +0x0 */
    int y;                 /* +0x4 */
    int z;                 /* +0x8 */
    short w;               /* +0xc */
    unsigned char inuse;   /* +0xe */
    unsigned char f;       /* +0xf */
    unsigned char g;       /* +0x10 */
    unsigned char pad;     /* +0x11 */
    unsigned char pad2[2]; /* +0x12 -> stride 0x14 */
};

/* The popup records: one at sHanaStarPopup and sixteen at
   sHanaPopups, 0x14 bytes each. The sixteen are declared with
   HanaPopup_Spawn's layout, which it needs to match; the other
   functions keep the spelling their own match used, through a cast. */
extern "C" {
extern char sHanaStarPopup[];
extern struct S_ea81c sHanaPopups[];
extern char sMgDigitSprites[];
}

// @symbol _ZN15dScMgHanachan_cD1Ev
// @symbol _ZN15dScMgHanachan_cD0Ev
/* Empty: the compiler destroys the fifteen 0x98-byte wigglers at 0x4678
   through __cxa_vec_cleanup with Wiggler's destructor, as dScMgBase_c's own
   destructor does for its array. The vptr store, the base destructor call
   and D0's delete are the compiler's too. */
dScMgHanachan_c::~dScMgHanachan_c() {}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgHanachan_c()
 * above, so this spells out what the deleting destructor does in terms of
 * it: the D1 body, called qualified so it is a direct call, then the
 * class-specific operator delete. mwccarm never defines _MSC_VER, so
 * nothing here reaches the cartridge object. */
extern "C" dScMgHanachan_c *_ZN15dScMgHanachan_cD0Ev(dScMgHanachan_c *thiz) {
    thiz->dScMgHanachan_c::~dScMgHanachan_c();
    dScMgHanachan_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol _ZN15dScMgHanachan_c7WigglerD1Ev
/* Wiggler's empty inline destructor (dScMgHanachan_c.h), emitted here after
   the class destructor that first uses it: the compiler tears the five
   Vector2 segment points at +0x18 down through __cxa_vec_cleanup. */

/* The single popup at sHanaStarPopup, as StarPopup_Start, StarPopup_Update,
   StarPopup_Draw and StarPopup_Clear use it. Its byte fields differ from the
   sixteen popups' S_ea81c layout. */
struct HanaStarPopup {
    int x;      /* 0x00 Fix12; clamped to 0x10000..0xf4000 at start */
    int y;      /* 0x04 Fix12; kept at or above 0x8000 */
    int vy;     /* 0x08 added to y in phase 0, -0x800 at start, minus 0x100 a frame */
    u8 timer;   /* 0x0c 0x18 at start, 0x40 for the hold; counts to 8 in phase 2 */
    u8 active;  /* 0x0d Update skips the popup while clear */
    u8 phase;   /* 0x0e 0 move, 1 hold and score, 2 count down */
    u8 ticks;   /* 0x0f 5 at start; one per eight frames in phase 2 */
    u8 visible; /* 0x10 Draw draws only while this is 1 */
    u8 pad[3];
};
typedef char HanaStarPopup_size_must_be_0x14[sizeof(HanaStarPopup) == 0x14 ? 1 : -1];

// @symbol StarPopup_Draw
/* Draws the single popup while its visible flag is 1: a sprite from the
   language's table 8 pixels left of it and one from sMgDigitSprites
   8 pixels right. */
extern "C" void StarPopup_Draw(void) {
    int x;
    int y;
    s32 r;
    int *new_var;
    new_var = (int *)sHanaStarPopup;
    if (((HanaStarPopup *)sHanaStarPopup)->visible != 1) {
        return;
    }
    x = ((HanaStarPopup *)sHanaStarPopup)->x >> 12;
    y = ((HanaStarPopup *)new_var)->y >> 12;
    r = GetGameLanguage();
    Mg_RenderSubSpriteMtx(((void **)sHanaTextSpritesByLanguage[r])[13], x - 8, y, 0);
    Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[25], x + 8, y, 0);
}

// @symbol StarPopup_Update
/* Steps the single popup through three phases. Phase 0 moves it by its
   velocity until its timer runs out, phase 1 holds for 0x40 frames and then
   adds one to the scene's mHudScore (capped at 9999, raising
   mBestHudScore to match), and phase 2 counts ticks down every eighth
   frame, stepping the int behind arg toward 0x32 each time. The
   compound assignments view the record through (int)f; the plain spelling
   reuses the field address and changes the object. */
typedef struct {
    u8 t;
    u8 pad[0x13];
} Timer20;
extern "C" Timer20 sHanaStarPopups[];

extern "C" void StarPopup_Update(char *arg) {
    int i;
    char *f = sHanaStarPopup;
    for (i = 0; i < 1; i++, f += 0x14) {
        u8 ph;
        if (((HanaStarPopup *)f)->active != 1)
            continue;
        if (((HanaStarPopup *)f)->y < 0x8000)
            ((HanaStarPopup *)f)->y = 0x8000;
        ph = ((HanaStarPopup *)f)->phase;
        if (ph == 0) {
            ((HanaStarPopup *)(int)f)->y += ((HanaStarPopup *)f)->vy;
            ((HanaStarPopup *)(int)f)->vy -= 0x100;
            if (((HanaStarPopup *)f)->timer != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            ((HanaStarPopup *)f)->timer = 0x40;
            ((HanaStarPopup *)f)->phase += 1;
        } else if (ph == 1) {
            if (((HanaStarPopup *)f)->timer != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            ((HanaStarPopup *)f)->visible = 0;
            ((HanaStarPopup *)f)->phase += 1;
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
            if (((HanaStarPopup *)f)->ticks != 0) {
                ((HanaStarPopup *)(int)f)->timer += 1;
                if (((HanaStarPopup *)f)->timer >= 8) {
                    ((HanaStarPopup *)f)->timer = 0;
                    ((HanaStarPopup *)(int)f)->ticks -= 1;
                    if (arg != 0)
                        ApproachLinear(*(int *)arg, 0x32, 1);
                }
            } else {
                ((HanaStarPopup *)f)->active = 0;
            }
        }
    }
}

// @symbol StarPopup_Start
/* Starts the single popup at (x, y), x kept between 0x10000 and 0xf4000,
   unless it is already active. */
extern "C" void StarPopup_Start(int x, int y) {
    char *g = sHanaStarPopup;
    if (((HanaStarPopup *)g)->active != 0)
        return;
    ((HanaStarPopup *)g)->active = 1;
    if (x < 0x10000)
        x = 0x10000;
    else if (x > 0xf4000)
        x = 0xf4000;
    ((HanaStarPopup *)g)->x = x;
    ((HanaStarPopup *)g)->y = y;
    ((HanaStarPopup *)g)->timer = 0x18;
    ((HanaStarPopup *)g)->phase = 0;
    ((HanaStarPopup *)g)->visible = 1;
    ((HanaStarPopup *)g)->ticks = 5;
    ((HanaStarPopup *)g)->vy = -0x800;
}

// @symbol StarPopup_Clear
/* Clears the single popup's active and draw flags. */
extern "C" unsigned char StarPopup_Clear(void) {
    ((HanaStarPopup *)sHanaStarPopup)->active = 0;
    ((HanaStarPopup *)sHanaStarPopup)->visible = 0;
    return 0;
}

// @symbol HanaPopup_DrawAll
/* Draws each of the sixteen popups whose +0xb flag is set, as three
   sprites 0x10 pixels apart. */
typedef struct {
    int x;
    int y;
    unsigned char pad0[7];
    unsigned char flag;
    int pad1;
} Ent_ea670;

extern "C" void HanaPopup_DrawAll(void) {
    void *a = 0, *b = 0, *c = 0;
    int i;
    for (i = 0; i < 16; i++) {
        Ent_ea670 *e = &((Ent_ea670 *)sHanaPopups)[i];
        if (e->flag != 0) {
            int sb = e->x >> 12;
            int r8 = e->y >> 12;
            int idx = GetGameLanguage();
            void *obj = sHanaTextSpritesByLanguage[idx];
            Mg_RenderSubSpriteMtx(((void **)obj)[14], sb - 0x10, r8, a);
            Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[41], sb, r8, b);
            Mg_RenderSubSpriteMtx(((void **)sMgDigitSprites)[40], sb + 0x10, r8, c);
        }
    }
}

// @symbol HanaPopup_UpdateAll
/* Steps the sixteen popups: y is kept at or below 0xb8000, phase 0 moves
   each by its velocity until its timer runs out, and phase 1 holds for 0x40
   frames and then clears it. The compound assignments go through (int)e,
   as StarPopup_Update's do. */
typedef struct Ent71c {
    int _0;    /* 0x00 */
    int pos;   /* 0x04 */
    int vel;   /* 0x08 */
    u16 timer; /* 0x0c */
    u8 active; /* 0x0e */
    u8 fE;     /* 0x0f */
    u8 phase;  /* 0x10 */
    u8 pad[3];
} Ent71c;

extern "C" void HanaPopup_UpdateAll(void) {
    int i;
    for (i = 0; i < 16; i++) {
        Ent71c *e = &((Ent71c *)sHanaPopups)[i];
        if (e->active) {
            if (e->pos > 0xb8000)
                e->pos = 0xb8000;
            if (e->phase == 0) {
                if (e->timer != 0) {
                    ((Ent71c *)(int)e)->timer -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                    ((Ent71c *)(int)e)->pos += e->vel;
                    ((Ent71c *)(int)e)->vel += 0x100;
                } else {
                    e->timer = 0x40;
                    ((Ent71c *)(int)e)->phase += 1;
                }
            } else {
                if (e->timer != 0) {
                    ((Ent71c *)(int)e)->timer -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                } else {
                    e->timer = 0;
                    e->active = 0;
                    e->fE = 0;
                }
            }
        }
    }
}

// @symbol HanaPopup_Spawn
/* Starts a popup in the first free one of the sixteen slots at (r0, r1),
   r0 kept between 0x14000 and 0xf0000. */

extern "C" void HanaPopup_Spawn(int r0, int r1) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (sHanaPopups[i].inuse == 0) {
            sHanaPopups[i].inuse = 1;
            sHanaPopups[i].f = 1;
            sHanaPopups[i].w = 0x18;
            if (r0 < 0x14000)
                r0 = 0x14000;
            else if (r0 > 0xf0000)
                r0 = 0xf0000;
            sHanaPopups[i].x = r0;
            sHanaPopups[i].y = r1;
            sHanaPopups[i].z = 0x800;
            sHanaPopups[i].g = 0;
            return;
        }
    }
}

// @symbol HanaPopup_ClearAll
/* Clears the +0xe and +0xf flags of every one of the sixteen popups whose
   +0xe flag is already clear. */
struct E6_ea8e0 {
    unsigned char pad[0xe];
    unsigned char e;
    unsigned char f;
    unsigned char pad2[4];
};

extern "C" void HanaPopup_ClearAll(void) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (((struct E6_ea8e0 *)sHanaPopups)[i].e == 0) {
            ((struct E6_ea8e0 *)sHanaPopups)[i].e = 0;
            ((struct E6_ea8e0 *)sHanaPopups)[i].f = 0;
        }
    }
}

// @symbol Wiggler_DrawTarget
/* Draws a wiggler's five segments from the tail forward, then one more
   sprite by the head when bloom is set and Wiggler_IsStarVisible agrees. The
   trivial ~Vec2_ea914 and the strength-reduction bracket are both part of
   the match. */
struct Vec2_ea914 {
    s32 x;
    s32 y;
    ~Vec2_ea914() {}
};

#pragma push
#pragma opt_strength_reduction off
extern "C" void Wiggler_DrawTarget(void *self_) {
    Wiggler *self = (Wiggler *)self_;
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

    Vec2_ea914 pos;
    Vec2_ea914 va;
    Vec2_ea914 vb;
    Vec2_ea914 vout;
    Vec2_ea914 pts[5];

    pts[4].x = 0xb4000;
    pts[4].y = 0x80000;
    for (j = 3; j >= 1; j--) {
        pts[j].x = pts[j + 1].x;
        pts[j].y = pts[j + 1].y;
        pts[j].x = pts[j].x - (self->stretch[j] + ((j + 1) << 13));
    }
    pts[0].x = pts[1].x;
    pts[0].y = pts[1].y;
    pts[0].x = pts[0].x - (self->stretch[0] + 0x2000);

    for (i = 0, A = 0, B = 0; i < 5; i++) {
        rot = -0x4000;
        va.y = 0x1000;
        va.x = 0;
        pos.x = pts[i].x;
        pos.y = pts[i].y;
        ang = (s16)(self->bob - A);
        func_0203d388((int *)&va, -0x4000);
        if (self->bloom == 0) {
            if (i == 0) {
                rot += data_02082214[((u16)ang >> 4) * 2] >> 1;
            } else {
                s32 v = data_02082214[((u16)ang >> 4) * 2];
                rot += (s16)(((s64)v * (0x1800 - B) + 0x800) >> 12);
            }
        }
        func_0203d680((Vec2 *)&vout, (const Vec2 *)&va, data_02082214[((u16)(ang * 2) >> 4) * 2]);
        pos.x = pos.x + vout.x;
        pos.y = pos.y + vout.y;
        res = Wiggler_GetFrame((char *)self, i);
        px = pos.y >> 12;
        py = pos.x >> 12;
        a2i = Wiggler_GetFlag((char *)self, i);
        OAM::Render(0, (OamAttr *)sWigglerSprites[a2i], py, px, res, -1, 0x1000, (u16)rot);
        if (i == 0) {
            if (self->bloom != 0) {
                if (Wiggler_IsStarVisible((char *)self) != 0) {
                    vb.x = 0x2000;
                    vb.y = -0x18000;
                    func_0203d388((int *)&vb, rot);
                    vb.x = vb.x + pos.x;
                    vb.y = vb.y + pos.y;
                    OAM::Render(
                        0, (OamAttr *)sHanaWigglerTargetSprites[0], vb.x >> 12, vb.y >> 12, -1, -1,
                        func_02053200((data_02082214[((u16)ang >> 4) * 2] >> 2) + 0xc00), (u16)rot);
                }
            }
        }
        A = A + 0x1800;
        B = B + 0x200;
    }
}
#pragma pop

// @symbol Wiggler_DrawField
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void Wiggler_DrawField(void *arg) {
    char *raw = (char *)arg;
    struct {
        int A, B, urot, va0x, res;
    } t;
    Vec2 vb0;
    struct {
        Vec2 vc0;
    } s3;
    struct {
        Vec2 pos, va, vb, vc, vout;
    } s;
    int px, py;
    int a2i;
    int i;
    short rot, ang;

    i = 0;
    vb0.y = -0x18000;
    s3.vc0.y = -0x14000;
    t.A = 0;
    t.B = 0;
    vb0.x = 0x2000;
    s3.vc0.x = 0;
    t.va0x = 0;
    for (; i < 5; i++) {
        rot = ((Wiggler *)raw)->ang[i];
        /* Stepped base: the direct seg[i].x/.y spelling renumbers six @NNN labels. */
        s.pos.x = ((Wiggler *)(raw + i * 8))->seg[0].x;
        s.pos.y = ((Wiggler *)(raw + i * 8))->seg[0].y;
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        ang = (short)(((Wiggler *)raw)->bob - t.A);
        func_0203d388(&s.va.x, rot);

        if (((Wiggler *)raw)->bloom == 0) {
            if (i == 0) {
                rot = (short)(rot + (data_02082214[((unsigned short)ang >> 4) * 2] >> 1));
            } else {
                int sinv = data_02082214[((unsigned short)ang >> 4) * 2];
                rot = (short)(rot + (short)(((long long)sinv * (0x1800 - t.B) + 0x800) >> 12));
            }
        }

        if (sHanaFieldWigglerCount >= 6) {
            rot = (short)(rot & 0xfc00);
        }
        func_0203d680(&s.vout, &s.va, data_02082214[((unsigned short)(ang << 1) >> 4) * 2]);
        s.pos.x += s.vout.x;
        s.pos.y += s.vout.y;

        t.res = Wiggler_GetFrame(raw, i);
        px = s.pos.y >> 12;
        py = s.pos.x >> 12;
        OAM::Render(1,
                    (OamAttr *)sWigglerSprites[(a2i = Wiggler_GetFlag(raw, i),
                                                (t.urot = (unsigned short)rot), a2i)],
                    py, px, t.res, -1, 0x1000, t.urot);

        if (i == 0) {
            if (((Wiggler *)raw)->bloom != 0 && ((Wiggler *)raw)->flower > 0) {
                int fp2;
                int scale;
                fp2 = (0x3000 - ((Wiggler *)raw)->flower) >> 12;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb.x, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                if (fp2 != 0)
                    scale = 0x1000;
                else
                    scale =
                        func_02053200((data_02082214[((unsigned short)ang >> 4) * 2] >> 2) + 0xc00);
                OAM::Render(1, (OamAttr *)sHanaWigglerTargetSprites[fp2], s.vb.x >> 12,
                            s.vb.y >> 12, -1, -1, scale, t.urot);
            } else if (((Wiggler *)raw)->state == &Wiggler::StateWait) {
                if (((Wiggler *)raw)->hold > 0x1a && ((Wiggler *)raw)->hold < 0x30) {
                    int idx = s3.vc0.x;
                    s.vc.x = s3.vc0.x;
                    s.vc.y = s3.vc0.y;
                    if (((Wiggler *)raw)->hold < 0x26) {
                        int d = (0x30 - ((Wiggler *)raw)->hold) * 0x600;
                        int vy;
                        s.vc.y = (vy = *(volatile int *)((char *)&s - 4), (idx = 1) ? vy : vy) - d;
                    }
                    func_0203d388(&s.vc.x, rot);
                    s.vc.x += s.pos.x;
                    s.vc.y += s.pos.y;
                    OAM::Render(1, (OamAttr *)sHanaWigglerFieldSprites[idx], s.vc.x >> 12,
                                s.vc.y >> 12, -1, -1, 0x1000,
                                (unsigned short)(short)(rot & 0xf800));
                }
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}
#pragma pop
}

// @symbol _ZN15dScMgHanachan_c7Wiggler6UpdateEv
#pragma push
#pragma O3 /* the default schedule misses */
void dScMgHanachan_c::Wiggler::Update() {
    Wiggler *w = this;
    (this->*sub)();
    if (w->face != 5)
        return;
    w->popCount += 1;
    {
        int i = 0;
        int max = 0x200;
        int *p = w->stretch;
        do {
            if (w->popCount > (5 - i) * 3)
                Math_Function_0203b0fc(p, 0x9800, 0x400, max);
            i++;
            max += 0x140;
            p++;
        } while (i < 5);
    }
    Wiggler_StepSegmentsB(w);
}
#pragma pop
// @symbol _ZN15dScMgHanachan_c7Wiggler9StateHoldEv
void dScMgHanachan_c::Wiggler::StateHold() {
    Wiggler *w = this;
    if (ApproachLinear2(w->hold, 0, 1)) {
        w->bloom = 0;
        w->busy = 1;
        w->flash = 0x21;
        w->speedTarget = 0x1000;
        w->bobRate = 0x900;
        w->face = 6;
    } else {
        (this->*sub)();
        if (w->hold == 0x30) {
            if (w->face != 4)
                w->face = 3;
        }
    }

    if (w->face != 6)
        return;

    w->popCount += 1;

    int i = 0;
    int scale = 0x200;
    int *q = w->stretch;
    do {
        if (w->popCount > (5 - i) * 3) {
            Math_Function_0203b0fc(q, 0x9800, 0x400, scale);
        }
        scale += 0x140;
        q += 1;
        i++;
    } while (i < 5);

    Wiggler_StepSegmentsB(w);
}

// @symbol _ZN15dScMgHanachan_c7Wiggler10StateMoveAEv
void dScMgHanachan_c::Wiggler::StateMoveA() {
    Wiggler *w = this;
    int va[2];
    int vb[2];
    short tgt;

    ApproachLinear(w->flower, 0, 0x200);
    if (ApproachLinear2(w->timer, 0, 1) != 0) {
        if (sub == &Wiggler::TurnTowardAim) {
            w->bloom = 1;
            w->flower = 0x3000;
            w->busy = 0;
            w->face = 0;
        }
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = w->aim - 0x1000;
    ApproachLinear(w->ang[0], tgt, 0x400);
    func_0203d388(va, w->ang[0]);
    func_0203d704((int *)vb, (int *)&w->seg[1], (int *)va);
    w->seg[0].x = vb[0];
    w->seg[0].y = vb[1];
    {
        short cnt = w->timer;
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            w->face = 1;
        else
            w->face = 0;
    }
}

// @symbol _ZN15dScMgHanachan_c7Wiggler9StateWaitEv
void dScMgHanachan_c::Wiggler::StateWait() {
    Wiggler *w = this;
    short *t = &w->hold;
    short v = (short)(*t - 1);
    *t = v;
    if (w->hold == 0) {
        w->bloom = 0;
        w->busy = 1;
        w->speedTarget = 0x1000;
        w->bobRate = 0x900;
        w->face = 4;
        w->flash = 0x78;
        {
            state = sub;
        }
        return;
    }
    (this->*sub)();
    if (w->hold != 0x30)
        return;
    if (w->decoy == 0)
        Sound::PlayBank2_2D(0x1ea);
    w->face = 3;
}

// @symbol _ZN15dScMgHanachan_c7Wiggler10StateMoveBEv
void dScMgHanachan_c::Wiggler::StateMoveB() {
    Wiggler *w = this;
    int va[2];
    int vb[2];
    short tgt;
    short cnt;

    ApproachLinear(w->flower, 0, 0x200);
    w->timer -= 1;
    if (w->timer == 0) {
        if (sub == &Wiggler::TurnTowardAim) {
            w->bloom = 1;
            w->flower = 0x3000;
            w->busy = 0;
            w->face = 0;
        } else {
            w->bloom = 0;
            w->busy = 0;
            w->speedTarget = 0xa00;
            w->bobRate = 0x600;
            w->face = 2;
            w->voice = 0;
            w->voiceId = 0x1ec;
        }
        state = sub;
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = w->aim - 0x1000;
    ApproachLinear(w->ang[0], tgt, 0x400);
    func_0203d388(va, w->ang[0]);
    func_0203d704((int *)vb, (int *)&w->seg[1], (int *)va);
    w->seg[0].x = vb[0];
    w->seg[0].y = vb[1];
    cnt = w->timer;
    if ((cnt & 0x10) == 0 || cnt < 0x38)
        w->face = 1;
    else
        w->face = 0;
}

// @symbol Wiggler_UpdateSway
/* char* parameter to agree with include/decl_common.h -- see Wiggler_DrawField. */
extern "C" void Wiggler_UpdateSway(char *raw) {
    Wiggler *thiz = (Wiggler *)raw;
    /* thiz->state here renumbers the TU's pinned @NNN member-pointer labels. */
    if (((Wiggler *)raw)->state == &Wiggler::TurnTowardAim && thiz->decoy == 0) {
        short h = thiz->bob;
        if (h < 0 && (short)(h + thiz->bobRate) > 0) {
            Sound::PlayBank2_2D(0x1e8);
        } else if (h > 0 && (short)(h + thiz->bobRate) < 0) {
            Sound::PlayBank2_2D(0x1e7);
        }
    }
    thiz->bob += thiz->bobRate;
}

// @symbol Wiggler_Move
extern "C" void Wiggler_Move(char *c) {
    Wiggler *w = (Wiggler *)c;
    int v[2];
    int step;
    int v93;

    if (w->alive != 0) {
        step = 0x1000;
        if (w->speed < 0xa00)
            step = step >> 2;
        ApproachLinear(w->speed, w->speedTarget, step);
        v[1] = -w->speed;
        v[0] = 0;
        func_0203d388(v, w->ang[1]);
        w->seg[1].x += v[0];
        w->seg[1].y += v[1];
        if (w->busy != 0) {
            /* long long cast: a plain w->flash -= 1 reloads the field. */
            *(short *)(long long)(int)&w->flash = *(short *)(long long)(int)&w->flash - 1;
            if (w->flash == 0) {
                Wiggler_SetCruiseSpeed(c);
            }
        }
    }
    if (w->voiceId != 0 && w->decoy == 0) {
        v93 = 0;
        if (w->busy != 0)
            v93 = 0x1f4;
        w->voice = func_02012468(w->voice, 2, w->voiceId, 2, 0, v93, 0, 0);
    } else {
        w->voice = 0;
    }
    if (w->state)
        (w->*w->state)();
    if (w->alive == 0)
        return;
    Wiggler_StepSegmentsA(w);
}

// @symbol Wiggler_HitTest
extern "C" {
int Wiggler_HitTest(char *raw, const V2 *touchPoint) {
    Wiggler *wiggler = (Wiggler *)raw;
    int segmentIndex;
    for (segmentIndex = 0; segmentIndex < 5; segmentIndex++) {
        if (func_0203d5dc((const int *)(&wiggler->seg[segmentIndex]), (const int *)touchPoint) <
            0x14000)
            return 1;
    }
    return 0;
}
}

// @symbol Wiggler_IsStarVisible
extern "C" {
int Wiggler_IsStarVisible(char *raw) {
    if (((Wiggler *)raw)->busy == 0) {
        Wiggler *w = (Wiggler *)raw;
        if (w->state == w->saved)
            return 1;
    }
    return 0;
}
}

// @symbol Wiggler_StopIfIdle
extern "C" {
void Wiggler_StopIfIdle(char *c) {
    Wiggler *w = (Wiggler *)c;
    if (w->state == &Wiggler::TurnTowardAim) {
        w->busy = 0;
        w->timer = 0x60;
        if (w->decoy == 0) {
            Sound::PlayBank2_2D(0x1e9);
        }
        w->state = &Wiggler::StateMoveA;
    } else {
        if (w->state != &Wiggler::StateWait) {
            w->hold = 0x31;
            w->sub = w->saved;
            Sound::PlayBank2_2D(0x1ed);
        }
        w->popCount = 0;
        w->state = &Wiggler::StateHold;
    }
}
}

// @symbol Wiggler_OnTouched
extern "C" void Wiggler_OnTouched(char *c) {
    Wiggler *w = (Wiggler *)c;
    if (w->state == &Wiggler::TurnTowardAim) {
        w->busy = 0;
        w->timer = 0x60;
        if (w->decoy == 0)
            Sound::PlayBank2_2D(0x1e9);
        w->state = &Wiggler::StateMoveA;
    } else {
        w->face = 5;
        w->popCount = 0;
        w->sub = w->saved;
        w->state = &Wiggler::Update;
    }
}

// @symbol Wiggler_SetCruiseSpeed
extern "C" {
void Wiggler_SetCruiseSpeed(char *raw) {
    Wiggler *w = (Wiggler *)raw;
    w->speedTarget = 0xa00;
    w->bobRate = 0x600;
    w->flash = 0;
    w->busy = 0;
    w->bloom = 0;
    w->face = 2;
}
}

// @symbol Wiggler_Retarget
extern "C" {
void Wiggler_Retarget(Wiggler *c, int v) {
    short *h = (short *)(int)(&c->tries);

    (*h)++;
    if (c->saved == &Wiggler::TurnTowardAim && c->tries < 3) {
        c->busy = 0;
        c->timer = (RandomIntInternal(&data_0209e650) & 0x20) + 0x60;
        if (c->tries == 1) {
            c->sub = &Wiggler::TurnTowardAim;
            c->saved = c->sub;
        } else {
            c->sub = &Wiggler::MoveBounce;
            c->saved = c->sub;
        }
        if (c->decoy == 0) {
            Sound::PlayBank2_2D(0x1e9);
        }
        c->state = &Wiggler::StateMoveB;
    } else {
        c->speed = 0;
        c->hold = (v >> 12) / 4 + 0x31;
        c->sub = c->saved;
        c->state = &Wiggler::StateWait;
    }
}
}

// @symbol Wiggler_GetFrame
extern "C" {
/* Second parameter unused by the body -- Wiggler_DrawField passes an index
   this callee ignores; declaring it keeps the caller's argument setup. */
int Wiggler_GetFrame(char *raw, int) {
    int r2 = 0;
    if (((Wiggler *)raw)->busy != 0) {
        int r1 = ((Wiggler *)raw)->flash;
        if (r1 > 0x20) {
            r2 = 1;
            goto end;
        }
        if (r1 > 0x10) {
            ((Wiggler *)raw)->face = 3;
            r2 = 5;
            goto end;
        }
        if (r1 > 0)
            r2 = 4;
        goto end;
    }
    {
        Wiggler *w = (Wiggler *)raw;
        if (w->state == &Wiggler::StateWait)
            goto C;
        if (w->state != &Wiggler::StateHold)
            goto end0;
    }
C: {
    int v = ((Wiggler *)raw)->hold;
    if (v < 0x10) {
        r2 = 5;
        goto end;
    }
    if (v < 0x20)
        r2 = 4;
    else
        r2 = 0;
    goto end;
}
end0:
    r2 = 0;
end:
    return r2;
}
}

// @symbol Wiggler_GetFlag
extern "C" {
int Wiggler_GetFlag(char *raw, int n) {
    switch (n) {
    case 0:
        return ((Wiggler *)raw)->face;
    case 1:
        return ((struct B72 *)&((Wiggler *)raw)->color)->f0 + 7;
    case 2:
        return ((struct B72 *)&((Wiggler *)raw)->color)->f1 + 7;
    case 3:
        return ((struct B72 *)&((Wiggler *)raw)->color)->f2 + 7;
    default:
        return ((struct B72 *)&((Wiggler *)raw)->color)->f3 + 7;
    }
}
}

// @symbol Hana_PickUnused
extern "C" {
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

    v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
    if (found)
        v = sHanaPickedLow | (unsigned short)(v & 0xf0);

    for (i = 0; i < n; i++) {
        if (v == sHanaUsedIds[i]) {
            v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
            if (found)
                v = sHanaPickedLow | (unsigned short)(v & 0xf0);
            i = -1;
        }
    }

    sHanaUsedIds[n] = v;
    return v;
}
}

// @symbol Hana_ChooseStarWiggler
extern "C" {
void Hana_ChooseStarWiggler(int count) {
    int i, j;
    sHanaPickedLow = (short)(RandomIntInternal(&data_0209e650) & 0xf);
    sHanaPickedCount = (short)count;
    for (i = 0; i < count; i++) {
        int r = RandomIntInternal(&data_0209e650);
        sHanaPickedIds[i] = (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) *
                                                   sHanaFieldWigglerCount) >>
                                             0xc);
        j = 0;
        while (j < i) {
            if (sHanaPickedIds[j] == sHanaPickedIds[i]) {
                r = RandomIntInternal(&data_0209e650);
                sHanaPickedIds[i] =
                    (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) *
                                           sHanaFieldWigglerCount) >>
                                     0xc);
                j = 0;
            } else {
                j++;
            }
        }
    }
}
}

// @symbol Hana_PickMoveKind
extern "C" {
int Hana_PickMoveKind(void) {
    int v = 3;
    int r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
    if (r < 0x400)
        v = 0;
    else if (r < 0x800)
        v = 1;
    else if (r < 0xc00)
        v = 2;
    return v;
}
}

// @symbol Wiggler_ClampToScreen
extern "C" {
int Wiggler_ClampToScreen(char *raw) {
    Wiggler *wiggler = (Wiggler *)raw;
    int boundaryMask = 0;
    int coordinate = wiggler->seg[1].x;
    if (coordinate >= 0xe8000) {
        boundaryMask |= 1;
        coordinate = 0xe8000;
        wiggler->seg[1].x = coordinate;
        boundaryMask &= 0xff;
    } else if (coordinate <= 0x18000) {
        boundaryMask |= 2;
        coordinate = 0x18000;
        wiggler->seg[1].x = coordinate;
        boundaryMask &= 0xff;
    }
    coordinate = wiggler->seg[1].y;
    if (coordinate >= 0xa8000) {
        boundaryMask |= 8;
        coordinate = 0xa8000;
        wiggler->seg[1].y = coordinate;
        boundaryMask &= 0xff;
    } else if (coordinate <= 0x18000) {
        boundaryMask |= 4;
        coordinate = 0x18000;
        wiggler->seg[1].y = coordinate;
        boundaryMask &= 0xff;
    }
    return boundaryMask;
}
}

// @symbol Wiggler_StepSegmentsA
extern "C" {
void Wiggler_StepSegmentsA(Wiggler *wiggler) {
    V2 segmentOffset;
    V2 headOffset;
    V2 segmentPosition;
    V2 headPosition;
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

    if (wiggler->bloom != 0)
        return;

    UpdateAngle(wiggler->ang[1], wiggler->aim, 8, 0x100);

    segmentIndex = 2;
    step = 2;
    zero = 0;
    angleStep = 0x100;

    for (; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        previousSegment = &wiggler->seg[previousIndex];
        Vec2_Sub((int *)&segmentOffset, (int *)(&wiggler->seg[segmentIndex]),
                 (int *)previousSegment);
        if (func_0203d434((int *)&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, angleDifference;
            func_0203d630((int *)&segmentOffset,
                          wiggler->stretch[segmentIndex] + segmentIndex * 0x2000);
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = wiggler->ang[previousIndex];
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
            UpdateAngle(wiggler->ang[segmentIndex], targetAngle, step, angleStep);
            angleCorrection = wiggler->ang[segmentIndex];
            angleCorrection = targetAngle - angleCorrection;
            func_0203d388((int *)&segmentOffset, (s16)(bendExcess - angleCorrection));
        }
        func_0203d704((int *)&segmentPosition, (int *)previousSegment, (int *)&segmentOffset);
        wiggler->seg[segmentIndex].x = segmentPosition.x;
        wiggler->seg[segmentIndex].y = segmentPosition.y;
    }

    {
        s16 dd = (s16)(wiggler->aim - wiggler->ang[1]);
        headOffset.x = 0;
        headOffset.y = -0x10000;
        UpdateAngle(wiggler->ang[0], (s16)(wiggler->ang[1] + dd), 8, 0x100);
    }
    func_0203d388((int *)&headOffset, wiggler->ang[0]);
    segments = wiggler->seg;
    func_0203d704((int *)&headPosition, (int *)(segments + 1), (int *)&headOffset);
    wiggler->seg[0].x = headPosition.x;
    wiggler->seg[0].y = headPosition.y;
}
}

// @symbol Wiggler_StepSegmentsB
extern "C" {
void Wiggler_StepSegmentsB(Wiggler *wiggler) {
    V2 segmentOffset;
    V2 headOffset;
    V2 segmentPosition;
    V2 headPosition;
    int segmentIndex = 4;
    Vector2 *segment = &wiggler->seg[segmentIndex];
    int baseSpacing = 0x8000;
    s16 zero = 0;
    int previousIndex;

    for (; segmentIndex >= 1; segmentIndex--) {
        previousIndex = segmentIndex - 1;
        Vec2_Sub((int *)&segmentOffset, (int *)segment, (int *)(&wiggler->seg[previousIndex]));
        if (func_0203d434((int *)&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, targetAngle, bendExcess, angleDifference, currentAngle;
            {
                int extraSpacing = wiggler->stretch[segmentIndex];
                func_0203d630((int *)&segmentOffset, extraSpacing + baseSpacing);
            }
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = wiggler->ang[previousIndex];
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
            currentAngle = wiggler->ang[segmentIndex];
            func_0203d388((int *)&segmentOffset, (s16)(bendExcess - (targetAngle - currentAngle)));
        }
        Vec2_Sub((int *)&segmentPosition, (int *)segment, (int *)&segmentOffset);
        wiggler->seg[previousIndex].x = segmentPosition.x;
        wiggler->seg[previousIndex].y = segmentPosition.y;
        segment--;
        baseSpacing -= 0x2000;
    }
    headOffset.x = 0;
    headOffset.y = -0x10000;
    func_0203d388((int *)&headOffset, wiggler->ang[0]);
    func_0203d704((int *)&headPosition, (int *)((Vector2 *)(wiggler->seg) + 1), (int *)&headOffset);
    wiggler->seg[0].x = headPosition.x;
    wiggler->seg[0].y = headPosition.y;
}
}

// @symbol Wiggler_StepSegmentsC
extern "C" {
void Wiggler_StepSegmentsC(Wiggler *wiggler) {
    V2 segmentOffset;
    V2 headOffset;
    V2 segmentPosition;
    V2 headPosition;
    int segmentIndex = 2;
    Vector2 *segment = &wiggler->seg[segmentIndex];
    int previousIndex;

    for (; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        Vec2_Sub((int *)&segmentOffset, (int *)segment, (int *)(&wiggler->seg[previousIndex]));
        if (func_0203d434((int *)&segmentOffset) != 0) {
            s16 offsetAngle, previousAngle, targetAngle, bendExcess, angleDifference, currentAngle;
            func_0203d630((int *)&segmentOffset, (segmentIndex * 2 + 0x10) << 12);
            offsetAngle = cstd::atan2(segmentOffset.x, segmentOffset.y);
            previousAngle = wiggler->ang[previousIndex];
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
            wiggler->ang[segmentIndex] = targetAngle;
            currentAngle = wiggler->ang[segmentIndex];
            func_0203d388((int *)&segmentOffset, (s16)(bendExcess - (targetAngle - currentAngle)));
        }
        func_0203d704((int *)&segmentPosition, (int *)(&wiggler->seg[previousIndex]),
                      (int *)&segmentOffset);
        wiggler->seg[segmentIndex].x = segmentPosition.x;
        wiggler->seg[segmentIndex].y = segmentPosition.y;
        segment++;
    }
    headOffset.x = 0;
    headOffset.y = -0x10000;
    wiggler->ang[0] = (s16)(wiggler->ang[1] + (((wiggler->ang[1] - wiggler->ang[2]) << 14) >> 16));
    func_0203d388((int *)&headOffset, wiggler->ang[0]);
    func_0203d704((int *)&headPosition, (int *)((Vector2 *)(wiggler->seg) + 1), (int *)&headOffset);
    wiggler->seg[0].x = headPosition.x;
    wiggler->seg[0].y = headPosition.y;
}
}

// @symbol _ZN15dScMgHanachan_c7Wiggler13CheckOffLeftAEv
#pragma push
#pragma opt_strength_reduction off
void dScMgHanachan_c::Wiggler::CheckOffLeftA() {
    Wiggler *self = (Wiggler *)this;
    int i;
    s16 v = self->aim;
    if (v < 0 && self->seg[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            self->seg[i].x += 0x170000;
        return;
    }
    if (v > 0 && self->seg[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            self->seg[i].x -= 0x170000;
    }
}
#pragma pop

// @symbol Wiggler_StartPathA
extern "C" {
void Wiggler_StartPathA(Obj *self, int n) {
    int b = n % 3;
    int a = n / 3;
    int rA, rB, rC, rD;
    int g1;
    int cm, dm;

    self->speed = 0xa00;
    self->speedTarget = 0xa00;
    g1 = sHanaFieldWigglerCount / 3 - 1;

    rA = cstd::fdiv(a << 12, g1 << 12);
    rB = cstd::fdiv((g1 - a) << 12, g1 << 12);
    rC = cstd::fdiv(b << 12, 0x2000);
    rD = cstd::fdiv((2 - b) << 12, 0x2000);

    if (a & 1) {
        self->aim = -0x4000;
        self->bob = -0x8000;
        cm = 0x8000;
        dm = 0xfe000;
    } else {
        self->aim = 0x4000;
        self->bob = 0;
        cm = 0x100000;
        dm = 0xa000;
    }

    {
        Vec2i base;
        int i = 0;
        int delta;

        base.x = 0;
        base.y = 0;
        delta = 0;
        base.x = FX_MUL(cm, rC) + FX_MUL(dm, rD);
        base.y = (int)((((long long)rA << 16) + 0x800) >> 12) + FX_MUL(rB, 0xb0000);

        for (; i < 5; i++) {
            int bx = *(volatile int *)&base.x;
            self->seg[i].x = bx;
            self->seg[i].y = *(volatile int *)&base.y;
            if (self->aim < 0)
                self->seg[i].x += delta;
            else
                self->seg[i].x -= delta;
            self->ang[i] = self->aim;
            delta += 0x20000;
        }
    }

    Wiggler_StepSegmentsC((Wiggler *)self);
    self->voice = 0;
    self->voiceId = 0x1ec;
    ((Wiggler *)self)->state = &Wiggler::CheckOffLeftA;
    ((Wiggler *)self)->saved = ((Wiggler *)self)->state;
}
}

// @symbol _ZN15dScMgHanachan_c7Wiggler10MoveBounceEv
void dScMgHanachan_c::Wiggler::MoveBounce() {
    char *raw = (char *)this;
    int boundaryMask = Wiggler_ClampToScreen(raw);
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
        timer =
            (s16)((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) *
                    0x1e) >>
                   12) +
                  0x3c);
        randomX = RandomIntInternal(&data_0209e650);
        randomY = RandomIntInternal(&data_0209e650);
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

// @symbol Wiggler_StartBounce
extern "C" {
void Wiggler_StartBounce(char *raw, int) {
    int i;
    ((Wiggler *)raw)->speed = 0xa00;
    ((Wiggler *)raw)->speedTarget = 0xa00;
    for (i = 0; i < 5; i++) {
        ((Wiggler *)raw)->seg[i].x =
            (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) << 8);
        ((Wiggler *)raw)->seg[i].y =
            (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xc0);
        ((Wiggler *)raw)->ang[i] = (short)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    }
    ((Wiggler *)raw)->aim = (short)((((Wiggler *)raw)->ang[0] + ((Wiggler *)raw)->ang[1]) >> 1);
    Wiggler_ClampToScreen(raw);
    Wiggler_StepSegmentsC((Wiggler *)raw);
    ((Wiggler *)raw)->timer = 0x1e;
    ((Wiggler *)raw)->voice = 0;
    ((Wiggler *)raw)->voiceId = 0x1ec;
    ((Wiggler *)raw)->state = &Wiggler::MoveBounce;
    ((Wiggler *)raw)->saved = ((Wiggler *)raw)->state;
}
}

// @symbol _ZN15dScMgHanachan_c7Wiggler13CheckOffLeftBEv
#pragma push
#pragma opt_strength_reduction off
void dScMgHanachan_c::Wiggler::CheckOffLeftB() {
    Wiggler *self = (Wiggler *)this;
    int i;
    s16 v = self->aim;
    if (v < 0 && self->seg[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            self->seg[i].x += 0x170000;
        return;
    }
    if (v > 0 && self->seg[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            self->seg[i].x -= 0x170000;
    }
}
#pragma pop

// @symbol Wiggler_StartPathB
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void Wiggler_StartPathB(char *raw, int idx) {
    int n1;
    int A, B;
    int p;
    int valY;
    int pos[2];
    int i;
    int off;

    ((Wiggler *)raw)->speed = 0xa00;
    ((Wiggler *)raw)->speedTarget = 0xa00;

    n1 = sHanaFieldWigglerCount - 1;
    A = cstd::fdiv(idx << 12, n1 << 12);
    B = cstd::fdiv((n1 - idx) << 12, n1 << 12);

    p = (n1 - 2) << 3;
    valY = FX_MUL(((0x28 - p) << 12), A) + FX_MUL(((p + 0x98) << 12), B);

    *(int *)((char *)pos + 4) = 0;
    *(int *)((char *)pos + 0) = 0;
    *(int *)((char *)pos + 4) = valY;

    if (RandomIntInternal(&data_0209e650) & 0x8000000) {
        ((Wiggler *)raw)->aim = -0x4000;
        pos[0] = 0x80000;
        pos[0] += ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    } else {
        ((Wiggler *)raw)->aim = 0x4000;
        pos[0] = 0x80000;
        pos[0] -= ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    }

    /* seg and ang through a stepped base: direct seg[i] / ang[i] renumbers six @NNN labels. */
    for (i = 0, off = 0; i < 5; i++) {
        ((Wiggler *)(raw + i * 8))->seg[0].x = pos[0];
        ((Wiggler *)(raw + i * 8))->seg[0].y = pos[1];
        if (((Wiggler *)raw)->aim < 0)
            ((Wiggler *)(raw + i * 8))->seg[0].x += off;
        else
            ((Wiggler *)(raw + i * 8))->seg[0].x -= off;
        ((Wiggler *)(raw + i * 2))->ang[0] = ((Wiggler *)raw)->aim;
        off += 0x20000;
    }

    Wiggler_StepSegmentsC((Wiggler *)raw);
    ((Wiggler *)raw)->voice = 0;
    ((Wiggler *)raw)->voiceId = 0x1ec;
    ((Wiggler *)raw)->state = &Wiggler::CheckOffLeftB;
    ((Wiggler *)raw)->saved = ((Wiggler *)raw)->state;
}
#pragma pop
}

// @symbol _ZN15dScMgHanachan_c7Wiggler13TurnTowardAimEv
void dScMgHanachan_c::Wiggler::TurnTowardAim() {
    Wiggler *w = this;
    ApproachLinear(w->ang[0], (short)(w->aim - 0x2000), 0x100);
}

// @symbol Wiggler_StartPathC
extern "C" {
void Wiggler_StartPathC(Wiggler *wiggler, int wigglerIndex) {
    V2 initialHead;
    V2 segmentOffset;
    V2 headOffset;
    V2 segmentPosition;
    V2 headPosition;
    int lastIndex, indexWeight, reverseWeight, spacingOffset, segmentIndex, previousIndex;

    wiggler->bloom = 1;
    wiggler->flower = 0x3000;
    wiggler->face = 0;
    wiggler->bobRate = 0x200;
    wiggler->speed = 0;
    wiggler->speedTarget = 0;

    lastIndex = sHanaFieldWigglerCount - 1;
    indexWeight = cstd::fdiv(wigglerIndex << 12, lastIndex << 12);
    reverseWeight = cstd::fdiv((lastIndex - wigglerIndex) << 12, lastIndex << 12);
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

    wiggler->aim = -0x8000;
    wiggler->seg[0].x = *(int *)&initialHead;
    wiggler->seg[0].y = *((int *)&initialHead + 1);
    wiggler->ang[0] = wiggler->aim - 0x2000;

    for (segmentIndex = 1; segmentIndex < 5; segmentIndex++) {
        previousIndex = segmentIndex - 1;
        segmentOffset.x = 0;
        segmentOffset.y = (segmentIndex * 2 + 0x10) << 12;
        func_0203d388((int *)&segmentOffset, (s16)(wiggler->ang[previousIndex] + 0x2000));
        wiggler->ang[segmentIndex] = wiggler->ang[previousIndex] + 0x2000;
        func_0203d704((int *)&segmentPosition, (int *)(&wiggler->seg[previousIndex]),
                      (int *)&segmentOffset);
        wiggler->seg[segmentIndex].x = segmentPosition.x;
        wiggler->seg[segmentIndex].y = segmentPosition.y;
    }

    Wiggler_StepSegmentsC(wiggler);

    headOffset.x = 0;
    headOffset.y = -0x10000;
    wiggler->ang[0] = wiggler->aim - 0x2000;
    func_0203d388((int *)&headOffset, wiggler->ang[0]);
    func_0203d704((int *)&headPosition, (int *)(&wiggler->seg[1]), (int *)&headOffset);
    wiggler->seg[0].x = headPosition.x;
    wiggler->seg[0].y = headPosition.y;

    wiggler->state = &Wiggler::TurnTowardAim;
    wiggler->saved = wiggler->state;
}
}

// @symbol Hana_SpawnWigglers
extern "C" {
void Hana_SpawnWigglers(char *raw, int arg1, int arg2) {
    int i;
    Wiggler *o = (Wiggler *)raw;

    o->voice = 0;
    o->voiceId = 0;
    o->decoy = 1;
    o->index = arg1;
    o->tries = 0;
    o->bob = (short)(RandomIntInternal(&data_0209e650) & 0xfff0);
    o->bobRate = 0x600;
    o->busy = 0;
    o->bloom = 0;
    o->face = 2;
    o->round = arg2;
    o->alive = 1;

    for (i = 0; i < 5; i++)
        o->stretch[i] = 0x10000;

    o->color = (short)Hana_PickUnused(arg1, 0xff);

    switch (sHanaRoundPattern) {
    case 0:
        Wiggler_StartPathC((Wiggler *)raw, arg1);
        return;
    case 1:
        Wiggler_StartPathB(raw, arg1);
        return;
    case 2:
        Wiggler_StartBounce(raw, arg1);
        return;
    case 3:
    default:
        Wiggler_StartPathA((Obj *)raw, arg1);
        return;
    }
}
}

// @symbol _ZN15dScMgHanachan_c16CleanupResourcesEv
s32 dScMgHanachan_c::CleanupResources() {
    Mg_CommitRecords(this);
    return 1;
}

// @symbol _ZN15dScMgHanachan_c6RenderEv
#pragma push
#pragma opt_common_subs off
s32 dScMgHanachan_c::Render() {
    char *raw = (char *)this;
    int i;
    char *arr;

    Mg_DrawScoreTally(raw, 0xe0, 0x14, 1);
    HanaPopup_DrawAll();
    StarPopup_Draw();

    {
        {
            if (mPhase == &dScMgHanachan_c::Phase_Play) {
                i = 0;
                if (sHanaFieldWigglerCount > 0) {
                    arr = (char *)mWiggler;
                    do {
                        Wiggler_DrawField(arr);
                        i++;
                        arr += 0x98;
                    } while (i < sHanaFieldWigglerCount);
                }
                goto tail;
            }
        }
    }

    {
        {
            if (mPhase == &dScMgHanachan_c::Phase_Result) {
                int flag = mHit;
                if (flag != 0) {
                    if (mPhaseTimer < 0x5a)
                        goto do_eac;
                }
                if (flag != 0)
                    goto count_loop;
                if (mPhaseTimer < 0x8a)
                    goto do_eac;
                goto count_loop;
            do_eac:
                Wiggler_DrawField(mTarget);
                goto tail;
            count_loop:
                i = 0;
                if (sHanaFieldWigglerCount > 0) {
                    arr = (char *)mWiggler;
                    do {
                        Wiggler_DrawField(arr);
                        i++;
                        arr += 0x98;
                    } while (i < sHanaFieldWigglerCount);
                }
                goto tail;
            }
        }
    }

    {
        if (mPhase == &dScMgHanachan_c::Phase_NoneB ||
            mPhase == &dScMgHanachan_c::Phase_WaitTouch) {
            Wiggler_DrawField(mTarget);
        }
    }

tail:
    Wiggler_DrawTarget(mTarget);

    {
        if (mPhase != &dScMgHanachan_c::Phase_WaitIntro) {
            int idx = GetGameLanguage();
            void *e = sHanaTextSpritesByLanguage[idx];
            void *f = ((void **)e)[3];
            DrawOamSprite(f, 0x80, 0x10, 0);
            Mg_DrawNumberRotated(0x80, 0x28, (void *)mTimeLeft, 1, -1, 0x800, 0);
        }
    }

    return 1;
}
#pragma pop

// @symbol _ZN15dScMgHanachan_c8BehaviorEv
s32 dScMgHanachan_c::Behavior() {

    sHanaSpinAngle = sHanaSpinAngle + 0x800;
    (this->*mPhase)();
    int i = 0;
    if (sHanaFieldWigglerCount > 0) {
        Thing *base = (Thing *)mWiggler;
        do {
            Wiggler_Move((char *)&base[i]);
            i++;
        } while (i < sHanaFieldWigglerCount);
    }
    int j = 0;
    if (sHanaFieldWigglerCount > 0) {
        Thing *base = (Thing *)mWiggler;
        do {
            Wiggler_UpdateSway((char *)&base[j]);
            j++;
        } while (j < sHanaFieldWigglerCount);
    }
    HanaPopup_UpdateAll();
    StarPopup_Update((char *)&mTimeLeft);
    return 1;
}

// @symbol _ZN15dScMgHanachan_c11Phase_NoneAEv
void dScMgHanachan_c::Phase_NoneA() {}

// @symbol _ZN15dScMgHanachan_c15Phase_WaitTouchEv
void dScMgHanachan_c::Phase_WaitTouch() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    int idx;
    int b;
    s->mPhaseTimer -= 1;
    if (s->mPhaseTimer != 0) {
        idx = gActivePlayerSlot;
        b = 0;
        if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    s->mPhase = &dScMgHanachan_c::Phase_NoneA;
}

// @symbol _ZN15dScMgHanachan_c15BeginResultWaitEv
void dScMgHanachan_c::BeginResultWait() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    s->mPhaseTimer = 0xb4;
    s->mPhase = &dScMgHanachan_c::Phase_WaitTouch;
}

// @symbol _ZN15dScMgHanachan_c11Phase_NoneBEv
void dScMgHanachan_c::Phase_NoneB() {}

// @symbol _ZN15dScMgHanachan_c12SetPhaseIdleEv
void dScMgHanachan_c::SetPhaseIdle() {
    mPhase = &dScMgHanachan_c::Phase_NoneB;
}

// @symbol _ZN15dScMgHanachan_c12Phase_ResultEv
void dScMgHanachan_c::Phase_Result() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    int v;
    s->mPhaseTimer -= 1;
    v = s->mPhaseTimer;
    if (v == 0) {
        if (s->mHit != 0) {
            Mg_SetSharedState(MG_STATE_NEXT_ROUND);
            SetPhaseIdle();
        } else {
            ((Wiggler *)s->mTarget)->decoy = 1;
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            BeginResultWait();
        }
        s->mPromptEnabled = 0;
    } else if (s->mHit != 0) {
        if (v == 0x5a) {
            Wiggler_OnTouched((char *)s->mTarget);
        }
    } else {
        if (v == 0x8a) {
            Wiggler_StopIfIdle((char *)s->mTarget);
        }
    }
}

// @symbol _ZN15dScMgHanachan_c11BeginResultEv
void dScMgHanachan_c::BeginResult() {
    char *raw = (char *)this;
    int i = 0;
    if (i < sHanaFieldWigglerCount) {
        char *p = raw;
        do {
            ((dScMgHanachan_c *)p)->mWiggler[0].alive = 0;
            i++;
            p += 0x98;
        } while (i < sHanaFieldWigglerCount);
    }
    if (mHit != 0)
        mPhaseTimer = 0xb4;
    else
        mPhaseTimer = 0xe4;
    mPhase = &dScMgHanachan_c::Phase_Result;
}

// @symbol _ZN15dScMgHanachan_c10Phase_PlayEv
void dScMgHanachan_c::Phase_Play() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    V2 a;
    int vec[4];
    int b = 0;
    int idx = gActivePlayerSlot;

    if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0)
        b = 1;

    if (b != 0) {
        Thing *p;
        Vector2 *src;
        a.x = gTouchX[(unsigned int)idx * 4] << 12;
        a.y = gTouchY[(unsigned int)idx * 4] << 12;
        p = (Thing *)s->mTarget;
        src = ((Wiggler *)p)->seg;
        vec[0] = src->x;
        vec[1] = src->y;

        if (Wiggler_IsStarVisible((char *)s->mTarget) != 0 &&
            Wiggler_HitTest((char *)s->mTarget, &a) != 0) {
            s->mHit = 1;
            func_02012790(0x26);
            p = (Thing *)s->mTarget;
            {
                if (((Wiggler *)p)->state == &Wiggler::TurnTowardAim)
                    Sound::PlayBank2_2D(0x1ef);
                else
                    Sound::PlayBank2_2D(0x1ee);
                if (sHanaRoundPattern == 0)
                    vec[0] -= 0x28000;
                StarPopup_Start(vec[0], vec[1]);
                BeginResult();
                return;
            }
        }

        {
            int found = -1;
            int i = 0;
            Thing *ptr;

            if (sHanaFieldWigglerCount > 0) {
                ptr = (Thing *)s->mWiggler;
                do {
                    if (Wiggler_IsStarVisible((char *)ptr) != 0 &&
                        Wiggler_HitTest((char *)ptr, &a) != 0) {
                        found = i;
                        break;
                    }
                    i++;
                    ptr++;
                } while (i < sHanaFieldWigglerCount);
            }

            if (found != -1) {
                int j;
                Thing *ptr2;
                Thing *sel = (Thing *)s->mWiggler + found;
                {
                    if (((Wiggler *)sel)->state == &Wiggler::TurnTowardAim)
                        Sound::PlayBank2_2D(0x1eb);
                    else
                        Sound::PlayBank2_2D(0x1ed);
                }

                {
                    j = 0;
                    if (sHanaFieldWigglerCount > 0) {
                        ptr2 = (Thing *)s->mWiggler;
                        do {
                            V2 *s2 = (V2 *)(int)&ptr2->pos;
                            vec[0] = s2->x;
                            vec[1] = s2->y;
                            Wiggler_Retarget(
                                (Wiggler *)ptr2,
                                func_0203d5dc((const int *)((const V2 *)vec), (const int *)&a));
                            j++;
                            ptr2++;
                        } while (j < sHanaFieldWigglerCount);
                    }
                }

                func_02012790(0xe);
                {
                    Vector2 *s3 = ((Wiggler *)sel)->seg;
                    vec[2] = s3->x;
                    vec[3] = s3->y;
                }
                if (sHanaRoundPattern == 0)
                    vec[2] -= 0x28000;
                HanaPopup_Spawn(vec[2], vec[3]);

                {
                    int t = s->mTimeLeft - 10;
                    if (t < 0)
                        t = 0;
                    s->mTimeLeft = t;
                }
            }
        }
    }

    if (ApproachLinear(s->mTimeTick, 0, 1) == 0)
        return;

    s->mTimeTick = 0x3c;
    if (ApproachLinear(s->mTimeLeft, 0, 1) != 0) {
        func_02012790(0xe);
        BeginResult();
    }

    if (s->mTimeLeft > 2)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
}

// @symbol _ZN15dScMgHanachan_c9BeginPlayEv
void dScMgHanachan_c::BeginPlay() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    s->mPhaseTimer = 0x4b0;
    s->mPhase = &dScMgHanachan_c::Phase_Play;
}

// @symbol _ZN15dScMgHanachan_c15Phase_WaitIntroEv
void dScMgHanachan_c::Phase_WaitIntro() {
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    if (ApproachLinear(s->mDelay, 0, 1) == 0)
        return;
    FreeGfxSlotsById(0x1d);
    if (s->mPromptBlinkCount == 0) {
        s->mPromptEnabled = 1;
        s->mPromptBlinkCount = 1;
        s->mPromptBlinkTimer = 0;
    }
    BeginPlay();
}

// @symbol _ZN15dScMgHanachan_c16Phase_SetupRoundEv
void dScMgHanachan_c::Phase_SetupRound() {
    dScMgHanachan_c *self = this;
    char *raw = (char *)this;
    if ((s32)self->mLevel < 0xa) {
        int bc = (s32)self->mLevel;
        sHanaRoundPattern = sHanaRoundPatterns[bc];
        sHanaFieldWigglerCount = sHanaFieldWigglerCounts[bc];
    } else {
        int ret = Hana_PickMoveKind();
        sHanaRoundPattern = ret;
        if (ret == 3) {
            sHanaFieldWigglerCount = 0xf;
        } else {
            unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
            sHanaFieldWigglerCount = ((int)(rnd >> 19) * 3 >> 12) + 3;
        }
    }

    if (self->mLevel < 0xa)
        Hana_ChooseStarWiggler(2);
    else
        Hana_ChooseStarWiggler(3);

    {
        int i = 0;
        if (sHanaFieldWigglerCount > 0) {
            Thing *p = (Thing *)((dScMgHanachan_c *)raw)->mWiggler;
            do {
                Hana_SpawnWigglers((char *)p, i, (s32)self->mLevel);
                i++;
                p++;
            } while (i < sHanaFieldWigglerCount);
        }
    }

    {
        unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
        int k = (int)(rnd >> 19) * sHanaFieldWigglerCount >> 12;
        dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
        s->mTarget = (Wiggler *)((Thing *)s->mWiggler + k);
        ((Wiggler *)s->mTarget)->decoy = 0;
        s->mDelay = 0x3c;
        Mg_ShowBanner(0xd, 0x80, 0x50, 1, -1, 0xd);
        s->mTimeTick = 0x3c;
        s->mHit = 0;
        s->mPhase = &dScMgHanachan_c::Phase_WaitIntro;
    }
}

// @symbol _ZN15dScMgHanachan_c10StartRoundEi
void dScMgHanachan_c::StartRound(int state) {
    if (state == 1) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc field
           address into its own register (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the
           function grows a word; the cartridge re-issues ldr r1,[r4,#0xbc]. */
        mLevel = *(const u32 *)&mLevel + 1;
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

// @symbol _ZN15dScMgHanachan_c13InitResourcesEv
s32 dScMgHanachan_c::InitResources() {
    void *a;
    void *b;

    Hana_LoadBackground();
    a = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCG_BIN);
    b = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCL_BIN);
    DecompressLZ16(a, (void *)0x6400000);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0xc0);
    GX::LoadOBJPltt(b, 0, 0xc0);
    Deallocate(a);
    Deallocate(b);
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

// @symbol Hana_LoadBackground
extern "C" {
void Hana_LoadBackground(void) {
    void *r6 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NCG_BIN);
    void *r5 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NSC_BIN);
    void *r4 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE1_NCL_BIN);
    {
        volatile unsigned short *m = (volatile unsigned short *)0x400000e;
        *m = (*m & ~3) | 1;
        *m = (*m & 0x43) | 0x614;
    }
    DecompressLZ16(r6, (void *)func_02054d88());
    func_02056314(r5, 0, 0x800);
    GX::LoadBGPltt((const void *)r4, 0x60, 0x1a0);
    data_0209d45c |= 8;
    Deallocate(r6);
    Deallocate(r5);
    Deallocate(r4);

    r6 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NCG_BIN);
    r5 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NSC_BIN);
    r4 = LoadFile(ASSET_D_2D_MG_BG_HANA_SAGASE2_NCL_BIN);
    {
        volatile unsigned short *m = (volatile unsigned short *)0x400100e;
        *m = (*m & ~3) | 2;
        *m = (*m & 0x43) | 0x614;
    }
    DecompressLZ16(r6, G2S::GetBG3CharPtr());
    func_020562b4(r5, 0, 0x800);
    GXS::LoadBGPltt((const void *)r4, 0x60, 0x1a0);
    data_0209d454 |= 8;
    Deallocate(r6);
    Deallocate(r5);
    Deallocate(r4);
}

/* The OamAttr* table OAM::Render indexes: the five scalar attributes, the
 * two sHanaBackgroundPartsB entries, then every third pointer out of the
 * vu32* ring sHanaBackgroundParts. Non-constant initializers, so the
 * compiler emits __sinit_d_s_mg_hanachan_methods.cpp to copy them at load. */
extern void *data_ov006_021374b8;
extern void *data_ov006_021374c4;
extern void *data_ov006_021374ac;
extern void *data_ov006_021374e8;
extern void *data_ov006_021374f4;
extern void *sHanaBackgroundPartsB[];

void *sWigglerSprites[11] = {
    data_ov006_021374b8,
    data_ov006_021374c4,
    data_ov006_021374ac,
    data_ov006_021374e8,
    data_ov006_021374f4,
    sHanaBackgroundPartsB[0],
    sHanaBackgroundPartsB[1],
    (void *)sHanaBackgroundParts[0],
    (void *)sHanaBackgroundParts[3],
    (void *)sHanaBackgroundParts[6],
    (void *)sHanaBackgroundParts[9],
};
}
