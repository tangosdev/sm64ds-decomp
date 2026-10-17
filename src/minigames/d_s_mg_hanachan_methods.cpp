//cpp
/* Which Wiggler? Up to fifteen wigglers, one of them with the star.
 * Touch it before mTimeLeft runs out.
 *
 * This file is the whole ov006 unit 0x020ea280..0x020ede18: 61
 * functions in ROM order, from the destructor to Hana_LoadBackground.
 * The destructor is the key function, so the vtable and RTTI are emitted
 * here too. dScMgHanachan_c_classInit, just above at 0x020ede18, is still
 * its own file (src/minigames/d_s_mg_hanachan.cpp).
 *
 * The destructor pair and the ten functions from Wiggler_Destroy to
 * Wiggler_DrawTarget came from one-function files. Their bodies are kept
 * as they matched there. The two popup arrays are declared once; where a
 * function spelled a record differently, its tag is renamed per function
 * and the access casts. Wiggler_Destroy is the wiggler array's element
 * destructor and takes the element, so its declaration now says so.
 *
 * The nine helpers that take the scene as their first argument
 * (Phase_WaitTouch .. Phase_SetupRound, the phase and tap handlers) are
 * dScMgHanachan_c members, declared in dScMgHanachan_c.h; the address is the
 * method name, since no ROM spelling survives. Each body opens by re-viewing
 * `this` as the char * it used to take. The other func_ov006 helpers stay
 * free: the ones from Wiggler_DrawTarget to Hana_SpawnWigglers take one
 * 0x98-byte wiggler record (an element of mWiggler) or no object at all, the
 * popup helpers before them take no object, and Hana_StateNoneA,
 * Hana_StateNoneB and Hana_LoadBackground take nothing.
 *
 * leftovers:
 * - the sixteen popups at sHanaPopups are read through
 *   four record spellings (HanaPopup_DrawAll, HanaPopup_UpdateAll,
 *   HanaPopup_Spawn, HanaPopup_ClearAll).
 * - the popup at sHanaStarPopup is read through raw byte
 *   offsets, and StarPopup_Update reaches its +0xc byte through a
 *   separate symbol, sHanaStarPopups.
 * - Wiggler_DrawTarget repeats Wiggler_DrawField's segment
 *   walk with its own point type, Vec2_ea914, whose trivial destructor
 *   is part of the match.
 * - dScMgHanachan_c::Render, `Thing *base; &base[i]` and `slot++`
 *   on the 0x98 array, DIFF, 6 words. `char *arr += 0x98` stays.
 *   Behavior's `&base[i]` matches.
 * - BeginResult, `C *p = (C *)mWiggler; p->alive = 0; p++`
 *   and mHit / mPhaseTimer / mPhaseFn, DIFF. The 0x4000 splits stay.
 * - Wiggler_StateMoveA, Wiggler_StateWait, Wiggler_StateMoveB,
 *   Wiggler_UpdateSway, member access on C (flower, sub, seg, hold, state,
 *   bob), DIFF. The byte offsets stay.
 * - Wiggler_StepSegmentsA, Wiggler_StepSegmentsB, Wiggler_StepSegmentsC,
 *   Wiggler_MoveBounce, cstd::atan2(Fix12<int>, Fix12<int>), does not link.
 *   cstd::atan2(int, int) mangles Eii, not E5Fix12IiES1_.
 * - Wiggler_DrawField, OAM::Render(int, OamAttr*, int, int, int,
 *   int, int, int), does not typecheck (the sprite table is void*). The
 *   ROM symbol's scale parameter is Fix12<int>, which this tree cannot spell.
 * - Wiggler_Retarget and Wiggler_StartPathA, a state copy of two
 *   named ints, DIFF (ldr/str/ldr/str). int[2] block-moves, so ObjB and Obj stay.
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

typedef struct V2 { int x, y; } V2;
typedef struct { int x, y; } Vec2;
/* int[2], not two named ints: a named-int copy is ldr/str/ldr/str. */
typedef struct P2 { int v[2]; } P2;
typedef struct S { int a; int b; } S;
typedef struct { int v[2]; } P;
struct Pair { int v[2]; };
struct Words2 { int v[2]; };
/* E and Thing are never whole-struct-assigned, so they keep named ints. */
struct E { int a, b; };
typedef struct Thing
{
    int a;              /* 0x00 */
    int b;              /* 0x04 */
    char pad0[0x10];    /* 0x08 */
    V2 pos;             /* 0x18 */
    char pad1[0x78];    /* 0x20 */
} Thing;               /* 0x98 */

struct B72 {
    unsigned short f0 : 2;
    unsigned short f1 : 2;
    unsigned short f2 : 2;
    unsigned short f3 : 2;
};

/* One wiggler: five body segments. state/saved/sub are the two-word
   Itanium member pointers (function word, this-adjustment). ang[1] is the
   angle the body steps along. decoy is 0 on the starred wiggler until a
   miss clears it. */
struct C {
    V2 state;            /* 0x00 tick callback */
    V2 saved;            /* 0x08 */
    V2 sub;              /* 0x10 callback the pop states run */
    V2 seg[5];           /* 0x18 segment positions */
    int speed;           /* 0x40 */
    int speedTarget;     /* 0x44 */
    int flower;          /* 0x48 bloom, Fix12; 0x3000 when the flower shows */
    int stretch[5];      /* 0x4c extra gap per segment */
    int popCount;        /* 0x60 */
    int index;           /* 0x64 index in the scene array */
    int voice;           /* 0x68 sound handle */
    int voiceId;         /* 0x6c */
    int decoy;           /* 0x70 0 = this wiggler has the star */
    u16 color;           /* 0x74 four 2-bit body colors */
    s16 aim;             /* 0x76 */
    s16 timer;           /* 0x78 */
    s16 ang[5];          /* 0x7a */
    s16 bob;             /* 0x84 */
    s16 bobRate;         /* 0x86 */
    int round;           /* 0x88 */
    s16 tries;           /* 0x8c */
    s16 flash;           /* 0x8e */
    s16 hold;            /* 0x90 */
    u8 alive;            /* 0x92 */
    u8 busy;             /* 0x93 */
    u8 bloom;            /* 0x94 draw the flower */
    u8 face;             /* 0x95 head sprite */
    u8 pad96[2];
};
typedef char C_size_must_be_0x98[sizeof(struct C) == 0x98 ? 1 : -1];

/* Same wiggler, the fields Wiggler_Retarget writes. P2 is an int[2]
   so the state copies block-move; a two-named-int copy does not. */
struct ObjB {
    P2 state;                       /* 0x00 */
    P2 saved;                       /* 0x08 */
    P2 sub;                         /* 0x10 */
    char pad18[0x40 - 0x18];
    int speed;                      /* 0x40 */
    char pad44[0x70 - 0x44];
    int decoy;                      /* 0x70 */
    char pad74[0x78 - 0x74];
    short timer;                    /* 0x78 */
    char pad7a[0x8c - 0x7a];
    short tries;                    /* 0x8c */
    short pad8e;
    short hold;                     /* 0x90 */
    unsigned char pad92;
    unsigned char busy;             /* 0x93 */
};

/* The pointer-to-member-function view of the element used by
   Wiggler_Update, Wiggler_StateWait and dScMgHanachan_c::Behavior: the
   word at 0x10 (Behavior reaches the one at +0x4660 instead) is called through
   as a PMF, which is the only reason these need a class type at all. */
struct CB {
    char pad10[0x10];
    void (CB::*pmf)();  /* 0x10 */
};
typedef void (CB::*PMF)();

/* Wiggler_StateHold's own spelling of the same idiom, kept separate because
   it names the member `cb`. */
struct Self;
typedef void (Self::*Pmf)();
struct Self {
    char pad10[0x10];
    Pmf cb;
};

/* dScMgHanachan_c::InitResources reaches slot 18 polymorphically through this
   shim.  Slot 18 is a declared method on the class now, but the legacy source
   dispatched through a 19-slot shim and that is the dispatch the ROM matches.
   Nothing else in the TU uses it. */
struct SelfV {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int a);
    char pad[0xa4];
    int unkA8;
    int unkAC;
};

#define PAIR0(s) ((Pair *)(int)((char *)(s) + 0x4660))

/* ROM symbols this TU calls or reads. decl_common.h's declarations are not
   repeated. The C++ helpers must stay outside the extern "C" block so that
   they mangle. */
extern int ApproachLinear(int &, int, int);
extern void UpdateAngle(short &, short, int, short);
extern int ApproachLinear2(short &, short, short);
bool ApproachLinear(short &value, short target, short step);
namespace Sound { u32 PlayBank2_2D(u32 id); }
namespace G2S { char *GetBG3CharPtr(); }
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace cstd { int fdiv(int numerator, int denominator); }

extern "C" {
/* This TU's own functions. */
extern void Wiggler_DrawField(void *o_);
extern void Wiggler_Update(char *c);
extern void Wiggler_StateHold(char *self);
extern void Wiggler_StateMoveA(char *c);
extern void Wiggler_StateWait(char* c);
extern void Wiggler_StateMoveB(char *c);
extern void Wiggler_UpdateSway(char *thiz_);
extern void Wiggler_Move(char *c);
extern int Wiggler_HitTest(char* a, const V2* b);
extern int Wiggler_IsStarVisible(char* p);
extern void Wiggler_StopIfIdle(char* c);
extern void Wiggler_OnTouched(char *c);
extern void Wiggler_SetCruiseSpeed(char *p);
extern void Wiggler_Retarget(struct ObjB* c, int v);
extern int Wiggler_GetFrame(char *o, int);
extern int Wiggler_GetFlag(char* c, int n);
extern int Hana_PickUnused(int n, int mask);
extern void Hana_ChooseStarWiggler(int count);
extern int Hana_PickMoveKind(void);
extern int Wiggler_ClampToScreen(char *c);
extern void Wiggler_StepSegmentsA(struct C* c);
extern void Wiggler_StepSegmentsB(struct C* c);
extern void Wiggler_StepSegmentsC(struct C* c);
extern void Wiggler_CheckOffLeftA(C* self);
extern void Wiggler_StartPathA(Obj *self, int n);
extern void Wiggler_MoveBounce(char *c);
extern void Wiggler_StartBounce(char *c, int);
extern void Wiggler_CheckOffLeftB(C* self);
extern void Wiggler_StartPathB(char* self, int idx);
extern void Wiggler_TurnTowardAim(void *a);
extern void Wiggler_StartPathC(struct C* c, int param);
extern void Hana_SpawnWigglers(char *self, int arg1, int arg2);
extern void Hana_StateNoneA(void);
extern void Hana_StateNoneB(void);
extern void Hana_LoadBackground(void);

extern short data_02082214[];
extern void *sWigglerSprites[];
extern void *sHanaWigglerTargetSprites[];
extern int pmf_Wiggler_StateWait_2[];
extern void *sHanaWigglerFieldSprites[];
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int b, void* attr, int x, int y, int a, int c, int f, int g);
extern int pmf_Wiggler_TurnTowardAim_2[];
extern int pmf_Wiggler_TurnTowardAim_8[];
extern int pmf_Wiggler_TurnTowardAim_7;
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
extern P pmf_Wiggler_TurnTowardAim_3;
extern P pmf_Wiggler_StateWait_3;
extern P pmf_Wiggler_StateMoveA;
extern P pmf_Wiggler_StateHold_2;
extern Pair pmf_Wiggler_TurnTowardAim;
extern Pair pmf_Wiggler_StateMoveA_2;
extern Pair pmf_Wiggler_Update;
extern struct P2 pmf_Wiggler_TurnTowardAim_6;
extern struct P2 pmf_Wiggler_TurnTowardAim_5;
extern struct P2 pmf_Wiggler_MoveBounce;
extern struct P2 pmf_Wiggler_StateMoveB;
extern struct P2 pmf_Wiggler_StateWait;
extern int pmf_Wiggler_StateWait_4[];
extern int pmf_Wiggler_StateHold[];
extern short sHanaPickedCount;
extern unsigned short sHanaPickedIds[];
extern unsigned short sHanaUsedIds[];
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern Vec2i pmf_Wiggler_CheckOffLeftA;
extern struct Words2 pmf_Wiggler_MoveBounce_2;
extern int pmf_Wiggler_CheckOffLeftB[];
extern V2 pmf_Wiggler_TurnTowardAim_4;
extern void Mg_CommitRecords(void *);
extern void Mg_DrawScoreTally(void *a, int b, int c, int d);
extern int GetGameLanguage(void);
extern void DrawOamSprite(void *a, int b, int c, int d);
extern void Mg_DrawNumberRotated(int a, int b, void *c, int d, int e, int f, int g);
extern Pair pmf_dScMgHanachan_c_Phase_Play;
extern Pair pmf_dScMgHanachan_c_Phase_Result;
extern Pair pmf_Hana_StateNoneB;
extern Pair pmf_dScMgHanachan_c_Phase_WaitTouch;
extern Pair pmf_dScMgHanachan_c_Phase_WaitIntro_2;
extern int pmf_Hana_StateNoneA[];
extern struct Pair pmf_dScMgHanachan_c_Phase_WaitTouch_2;
extern struct Pair pmf_Hana_StateNoneB_2;
extern int pmf_dScMgHanachan_c_Phase_Result_2[];
extern int pmf_Wiggler_TurnTowardAim_9[];
extern int pmf_Wiggler_TurnTowardAim_10[];
extern void func_02012790(int a);
extern void StarPopup_Start(int x, int y);
extern void HanaPopup_Spawn(int x, int y);
extern struct Pair pmf_dScMgHanachan_c_Phase_Play_2;
extern s16 sHanaRoundPatterns[];
extern volatile int sHanaFieldWigglerCounts[];
extern Pair pmf_dScMgHanachan_c_Phase_WaitIntro;
extern u8 data_0209d45c;
extern u8 data_0209d454;

/* func_0203d5dc: Wiggler_HitTest typed both arguments `const Vector3*` and
   then stepped one of them by 8 bytes, so they are V2s; Phase_Play
   typed the first `int*`.  Pointer either way -- one spelling, a cast at the
   odd call site. */
extern int func_0203d5dc(const V2 *a, const V2 *b);
/* Four files spelled func_0203d704's arguments V2* and two spelled them int*;
   decl_common.h does not declare it, so V2* wins and the int* callers cast. */
extern void func_0203d704(V2 *o, V2 *a, V2 *b);
extern void Vec2_Sub(V2 *o, V2 *a, V2 *b);
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
    int x;       /* +0x0 */
    int y;       /* +0x4 */
    int z;       /* +0x8 */
    short w;     /* +0xc */
    unsigned char inuse;  /* +0xe */
    unsigned char f;      /* +0xf */
    unsigned char g;      /* +0x10 */
    unsigned char pad;    /* +0x11 */
    unsigned char pad2[2];/* +0x12 -> stride 0x14 */
};

/* The popup records: one at sHanaStarPopup and sixteen at
   sHanaPopups, 0x14 bytes each. The sixteen are declared with
   HanaPopup_Spawn's layout, which it needs to match; the other
   functions keep the spelling their own match used, through a cast. */
extern "C" {
/* local extern: only sibling minigame headers (dScMgAmida_c.h and four
   others) declare it; this TU includes none of them */
extern void NullDestructor_0203d47c(void);
extern char sHanaStarPopup[];
extern struct S_ea81c sHanaPopups[];
extern char sMgDigitSprites[];
}

// @symbol _ZN15dScMgHanachan_cD1Ev
// @symbol _ZN15dScMgHanachan_cD0Ev
/* Destroys the fifteen 0x98-byte wigglers at 0x4678 through
   __cxa_vec_cleanup, as dScMgBase_c's own destructor does for its array.
   The vptr store, the base destructor call and D0's delete are the
   compiler's. */
dScMgHanachan_c::~dScMgHanachan_c()
{
    __cxa_vec_cleanup((char *)this + 0x4678, 0xf, 0x98, (void *)Wiggler_Destroy);
}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgHanachan_c()
 * above, so this spells out what the deleting destructor does in terms of
 * it: the D1 body, called qualified so it is a direct call, then the
 * class-specific operator delete. mwccarm never defines _MSC_VER, so
 * nothing here reaches the cartridge object. */
extern "C" dScMgHanachan_c *_ZN15dScMgHanachan_cD0Ev(dScMgHanachan_c *thiz)
{
    thiz->dScMgHanachan_c::~dScMgHanachan_c();
    dScMgHanachan_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol Wiggler_Destroy
/* The element destructor for one wiggler: its five segment points at
   +0x18 go through __cxa_vec_cleanup. */
extern "C" void *Wiggler_Destroy(void *self)
{
    __cxa_vec_cleanup((char *)self + 0x18, 5, 8, (void *)NullDestructor_0203d47c);
    return self;
}

// @symbol StarPopup_Draw
/* Draws the single popup while its +0x10 flag is 1: a sprite from the
   language's table 8 pixels left of it and one from sMgDigitSprites
   8 pixels right. */
extern "C" void StarPopup_Draw(void)
{
    int x;
    int y;
    s32 r;
    int *new_var;
    new_var = (int *)sHanaStarPopup;
    if ((*(((unsigned char *)sHanaStarPopup) + 0x10)) != 1) {
        return;
    }
    x = ((int *)sHanaStarPopup)[0] >> 12;
    y = new_var[1] >> 12;
    r = GetGameLanguage();
    Mg_RenderSubSpriteMtx(*((void **)(((char *)sHanaTextSpritesByLanguage[r]) + 0x34)), x - 8, y,
                          0);
    Mg_RenderSubSpriteMtx(*((void **)(sMgDigitSprites + 0x64)), x + 8, y, 0);
}

// @symbol StarPopup_Update
/* Steps the single popup through three phases. Phase 0 moves it by its
   velocity until its timer runs out, phase 1 holds for 0x40 frames and then
   adds one to the scene's mHudScore (capped at 9999, raising
   mBestHudScore to match), and phase 2 counts the +0xf field down every
   eighth frame, stepping the int behind arg toward 0x32 each time. */
typedef struct {
    u8 t;
    u8 pad[0x13];
} Timer20;
extern "C" Timer20 sHanaStarPopups[];

extern "C" void StarPopup_Update(char *arg)
{
    int i;
    char *f = sHanaStarPopup;
    for (i = 0; i < 1; i++, f += 0x14) {
        u8 ph;
        if (*(u8 *)(f + 0xd) != 1)
            continue;
        if (*(int *)(f + 4) < 0x8000)
            *(int *)(f + 4) = 0x8000;
        ph = *(u8 *)(f + 0xe);
        if (ph == 0) {
            *(int *)(((int)f + 4)) += *(int *)(f + 8);
            *(int *)(((int)f + 8)) -= 0x100;
            if (*(u8 *)(f + 0xc) != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            *(u8 *)(f + 0xc) = 0x40;
            *(u8 *)(((int)f + 0xe)) += 1;
        } else if (ph == 1) {
            if (*(u8 *)(f + 0xc) != 0) {
                int v;
                sHanaStarPopups[i].t--;
                v = sHanaStarPopups[i].t;
                if (v < 0)
                    sHanaStarPopups[i].t = 0;
                return;
            }
            *(u8 *)(f + 0x10) = 0;
            *(u8 *)(((int)f + 0xe)) += 1;
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
            if (*(u8 *)(f + 0xf) != 0) {
                *(u8 *)(((int)f + 0xc)) += 1;
                if (*(u8 *)(f + 0xc) >= 8) {
                    *(u8 *)(f + 0xc) = 0;
                    *(u8 *)(((int)f + 0xf)) -= 1;
                    if (arg != 0)
                        ApproachLinear(*(int *)arg, 0x32, 1);
                }
            } else {
                *(u8 *)(f + 0xd) = 0;
            }
        }
    }
}

// @symbol StarPopup_Start
/* Starts the single popup at (x, y), x kept between 0x10000 and 0xf4000,
   unless it is already active. */
extern "C" void StarPopup_Start(int x, int y)
{
    char *g = sHanaStarPopup;
    if (*(unsigned char *)(g + 0xd) != 0) return;
    *(unsigned char *)(g + 0xd) = 1;
    if (x < 0x10000) x = 0x10000;
    else if (x > 0xf4000) x = 0xf4000;
    *(int *)(g) = x;
    *(int *)(g + 4) = y;
    *(unsigned char *)(g + 0xc) = 0x18;
    *(unsigned char *)(g + 0xe) = 0;
    *(unsigned char *)(g + 0x10) = 1;
    *(unsigned char *)(g + 0xf) = 5;
    *(int *)(g + 8) = -0x800;
}

// @symbol StarPopup_Clear
/* Clears the single popup's active and draw flags. */
extern "C" unsigned char StarPopup_Clear(void)
{
    ((unsigned char *)sHanaStarPopup)[0xd] = 0;
    ((unsigned char *)sHanaStarPopup)[0x10] = 0;
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

extern "C" void HanaPopup_DrawAll(void)
{
    void *a = 0, *b = 0, *c = 0;
    int i;
    for (i = 0; i < 16; i++) {
        Ent_ea670 *e = &((Ent_ea670 *)sHanaPopups)[i];
        if (e->flag != 0) {
            int sb = e->x >> 12;
            int r8 = e->y >> 12;
            int idx = GetGameLanguage();
            void *obj = sHanaTextSpritesByLanguage[idx];
            Mg_RenderSubSpriteMtx(*(void **)((char *)obj + 0x38), sb - 0x10, r8, a);
            Mg_RenderSubSpriteMtx(*(void **)(sMgDigitSprites + 0xa4), sb, r8, b);
            Mg_RenderSubSpriteMtx(*(void **)(sMgDigitSprites + 0xa0), sb + 0x10, r8, c);
        }
    }
}

// @symbol HanaPopup_UpdateAll
/* Steps the sixteen popups: y is kept at or below 0xb8000, phase 0 moves
   each by its velocity until its timer runs out, and phase 1 holds for 0x40
   frames and then clears it. */
typedef struct Ent71c {
    int _0;      /* 0x00 */
    int pos;     /* 0x04 */
    int vel;     /* 0x08 */
    u16 timer;   /* 0x0c */
    u8 active;   /* 0x0e */
    u8 fE;       /* 0x0f */
    u8 phase;    /* 0x10 */
    u8 pad[3];
} Ent71c;

extern "C" void HanaPopup_UpdateAll(void)
{
    int i;
    for (i = 0; i < 16; i++) {
        Ent71c *e = &((Ent71c *)sHanaPopups)[i];
        if (e->active) {
            if (e->pos > 0xb8000)
                e->pos = 0xb8000;
            if (e->phase == 0) {
                if (e->timer != 0) {
                    *(u16 *)(((int)e + 0xc)) -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                    *(int *)(((int)e + 4)) += e->vel;
                    *(int *)(((int)e + 8)) += 0x100;
                } else {
                    e->timer = 0x40;
                    *(u8 *)(((int)e + 0x10)) += 1;
                }
            } else {
                if (e->timer != 0) {
                    *(u16 *)(((int)e + 0xc)) -= 1;
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

extern "C" void HanaPopup_Spawn(int r0, int r1)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (sHanaPopups[i].inuse == 0) {
            sHanaPopups[i].inuse = 1;
            sHanaPopups[i].f = 1;
            sHanaPopups[i].w = 0x18;
            if (r0 < 0x14000) r0 = 0x14000;
            else if (r0 > 0xf0000) r0 = 0xf0000;
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
struct E6_ea8e0 { unsigned char pad[0xe]; unsigned char e; unsigned char f; unsigned char pad2[4]; };

extern "C" void HanaPopup_ClearAll(void)
{
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
   sprite by the head when +0x94 is set and Wiggler_IsStarVisible agrees. The
   trivial ~Vec2_ea914 and the strength-reduction bracket are both part of
   the match. */
struct Vec2_ea914 { s32 x; s32 y; ~Vec2_ea914() {} };

#pragma push
#pragma opt_strength_reduction off
extern "C" void Wiggler_DrawTarget(void *self_) {
    char *self = (char *)self_;
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
        pts[j].x = pts[j].x - (((s32 *)(self + j * 4))[0x13] + ((j + 1) << 13));
    }
    pts[0].x = pts[1].x;
    pts[0].y = pts[1].y;
    pts[0].x = pts[0].x - (*(s32 *)(self + 0x4c) + 0x2000);

    for (i = 0, A = 0, B = 0; i < 5; i++) {
        rot = -0x4000;
        va.y = 0x1000;
        va.x = 0;
        pos.x = pts[i].x;
        pos.y = pts[i].y;
        ang = (s16)(*(s16 *)(self + 0x84) - A);
        func_0203d388((int *)&va, -0x4000);
        if (*(u8 *)(self + 0x94) == 0) {
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
        res = Wiggler_GetFrame(self, i);
        px = pos.y >> 12;
        py = pos.x >> 12;
        a2i = Wiggler_GetFlag(self, i);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, sWigglerSprites[a2i], py, px, res, -1, 0x1000,
                                                (u16)rot);
        if (i == 0) {
            if (*(u8 *)(self + 0x94) != 0) {
                if (Wiggler_IsStarVisible(self) != 0) {
                    vb.x = 0x2000;
                    vb.y = -0x18000;
                    func_0203d388((int *)&vb, rot);
                    vb.x = vb.x + pos.x;
                    vb.y = vb.y + pos.y;
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                        0, sHanaWigglerTargetSprites[0], vb.x >> 12, vb.y >> 12, -1, -1,
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
    char* raw = (char*)arg;
    struct { int A, B, urot, va0x, res; } t;
    Vec2 vb0;
    struct { Vec2 vc0; } s3;
    struct { Vec2 pos, va, vb, vc, vout; } s;
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
        rot = ((short*)(raw + 0x7a))[i];
        s.pos.x = *(int*)(raw + i*8 + 0x18);
        s.pos.y = *(int*)(raw + i*8 + 0x1c);
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        ang = (short)(*(short*)(raw + 0x84) - t.A);
        func_0203d388(&s.va.x, rot);

        if (*(unsigned char*)(raw + 0x94) == 0) {
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
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            1,
            sWigglerSprites[(a2i = Wiggler_GetFlag(raw, i), (t.urot = (unsigned short)rot), a2i)],
            py, px, t.res, -1, 0x1000, t.urot);

        if (i == 0) {
            if (*(unsigned char*)(raw + 0x94) != 0 && *(int*)(raw + 0x48) > 0) {
                int fp2;
                int scale;
                fp2 = (0x3000 - *(int*)(raw + 0x48)) >> 12;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb.x, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                if (fp2 != 0)
                    scale = 0x1000;
                else
                    scale = func_02053200((data_02082214[((unsigned short)ang >> 4) * 2] >> 2) + 0xc00);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, sHanaWigglerTargetSprites[fp2],
                                                        s.vb.x >> 12, s.vb.y >> 12, -1, -1, scale,
                                                        t.urot);
            } else if (*(int*)raw == pmf_Wiggler_StateWait_2[0]) {
                if ((*(int*)(raw + 4) == pmf_Wiggler_StateWait_2[1] || *(int*)raw == 0) &&
                    *(short*)(raw + 0x90) > 0x1a && *(short*)(raw + 0x90) < 0x30) {
                    int idx = s3.vc0.x;
                    s.vc.x = s3.vc0.x;
                    s.vc.y = s3.vc0.y;
                    if (*(short*)(raw + 0x90) < 0x26) {
                        int d = (0x30 - *(short*)(raw + 0x90)) * 0x600;
                        int vy;
                        s.vc.y = (vy = *(volatile int*)((char*)&s - 4), (idx = 1) ? vy : vy) - d;
                    }
                    func_0203d388(&s.vc.x, rot);
                    s.vc.x += s.pos.x;
                    s.vc.y += s.pos.y;
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                        1, sHanaWigglerFieldSprites[idx], s.vc.x >> 12, s.vc.y >> 12, -1, -1,
                        0x1000, (unsigned short)(short)(rot & 0xf800));
                }
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}
#pragma pop
}


// @symbol Wiggler_Update
#pragma push
#pragma O3 /* the default schedule misses */
extern "C" void Wiggler_Update(char *c)
{
    C *w = (C *)c;
    CB *o = (CB *)c;
    (o->*(o->pmf))();
    if (w->face != 5) return;
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
// @symbol Wiggler_StateHold
extern "C" void Wiggler_StateHold(char *self) {
    C *w = (C *)self;
    if (ApproachLinear2(w->hold, 0, 1)) {
        w->bloom = 0;
        w->busy = 1;
        w->flash = 0x21;
        w->speedTarget = 0x1000;
        w->bobRate = 0x900;
        w->face = 6;
    } else {
        (((Self *)self)->*(((Self *)self)->cb))();
        if (w->hold == 0x30) {
            if (w->face != 4) w->face = 3;
        }
    }

    if (w->face != 6) return;

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

// @symbol Wiggler_StateMoveA
extern "C" {
void Wiggler_StateMoveA(char *raw)
{
    int va[2];
    int vb[2];
    short tgt;

    ApproachLinear(*(int *)(raw + 0x48), 0, 0x200);
    if (ApproachLinear2(*(short *)(raw + 0x78), 0, 1) != 0) {
        int *p10 = (int *)(((int)raw + 0x10));
        int *pd = (int *)(((int)pmf_Wiggler_TurnTowardAim_2));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(raw + 0x10) == 0)) {
            *(unsigned char *)(raw + 0x94) = 1;
            *(int *)(raw + 0x48) = 0x3000;
            *(unsigned char *)(raw + 0x93) = 0;
            *(unsigned char *)(raw + 0x95) = 0;
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
    tgt = *(short *)(raw + 0x76) - 0x1000;
    ApproachLinear(*(short *)(raw + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(raw + 0x7a));
    func_0203d704((V2 *)vb, (V2 *)(raw + 0x20), (V2 *)va);
    *(int *)(raw + 0x18) = vb[0];
    *(int *)(raw + 0x1c) = vb[1];
    {
        short cnt = *(short *)(raw + 0x78);
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            *(unsigned char *)(raw + 0x95) = 1;
        else
            *(unsigned char *)(raw + 0x95) = 0;
    }
}
}

// @symbol Wiggler_StateWait
extern "C" void Wiggler_StateWait(char* c)
{
    short* t = (short*)(((int)c + 0x90));
    short v = (short)(*t - 1);
    *t = v;
    if (*(short*)(c+0x90) == 0) {
        *(unsigned char*)(c+0x94) = 0;
        *(unsigned char*)(c+0x93) = 1;
        *(int*)(c+0x44) = 0x1000;
        *(short*)(c+0x86) = 0x900;
        *(unsigned char*)(c+0x95) = 4;
        *(short*)(c+0x8e) = 0x78;
        {
            int a = *(int*)(c+0x10);
            int b = *(int*)(c+0x14);
            *(int*)c = b ? a : a;
            *(int*)(c+4) = b;
        }
        return;
    }
    CB* o = (CB*)c;
    (o->*(o->pmf))();
    if (*(short*)(c+0x90) != 0x30) return;
    if (*(int*)(c+0x70) == 0)
        Sound::PlayBank2_2D(0x1ea);
    *(unsigned char*)(c+0x95) = 3;
}

// @symbol Wiggler_StateMoveB
extern "C" {
void Wiggler_StateMoveB(char *c)
{
    int va[2];
    int vb[2];
    short tgt;
    short cnt;

    ApproachLinear(*(int *)(c + 0x48), 0, 0x200);
    *(short *)(((int)c + 0x78)) -= 1;
    if (*(short *)(c + 0x78) == 0) {
        int *p10 = (int *)(((int)c + 0x10));
        int *pd = (int *)(((int)pmf_Wiggler_TurnTowardAim_8));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(c + 0x10) == 0)) {
            *(unsigned char *)(c + 0x94) = 1;
            *(int *)(c + 0x48) = 0x3000;
            *(unsigned char *)(c + 0x93) = 0;
            *(unsigned char *)(c + 0x95) = 0;
        } else {
            *(unsigned char *)(c + 0x94) = 0;
            *(unsigned char *)(c + 0x93) = 0;
            *(int *)(c + 0x44) = 0xa00;
            *(unsigned short *)(c + 0x86) = 0x600;
            *(unsigned char *)(c + 0x95) = 2;
            *(int *)(c + 0x68) = 0;
            *(int *)(c + 0x6c) = 0x1ec;
        }
        *(P2 *)c = *(P2 *)(c + 0x10);
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = *(short *)(c + 0x76) - 0x1000;
    ApproachLinear(*(short *)(c + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(c + 0x7a));
    func_0203d704((V2 *)vb, (V2 *)(c + 0x20), (V2 *)va);
    *(int *)(c + 0x18) = vb[0];
    *(int *)(c + 0x1c) = vb[1];
    cnt = *(short *)(c + 0x78);
    if ((cnt & 0x10) == 0 || cnt < 0x38)
        *(unsigned char *)(c + 0x95) = 1;
    else
        *(unsigned char *)(c + 0x95) = 0;
}
}

// @symbol Wiggler_UpdateSway
/* char* parameter to agree with include/decl_common.h -- see Wiggler_DrawField. */
extern "C" void Wiggler_UpdateSway(char *raw) {
    int *thiz = (int *)raw;
    int *g = &pmf_Wiggler_TurnTowardAim_7;
    int v0 = thiz[0];
    if (v0 == g[0] && (thiz[1] == g[1] || v0 == 0) && thiz[0x70/4] == 0) {
        short h = *(short*)((char*)thiz + 0x84);
        if (h < 0 && (short)(h + *(short*)((char*)thiz + 0x86)) > 0) {
            Sound::PlayBank2_2D(0x1e8);
        } else if (h > 0 && (short)(h + *(short*)((char*)thiz + 0x86)) < 0) {
            Sound::PlayBank2_2D(0x1e7);
        }
    }
    {
        short *p = (short*)((int)thiz + 0x84);
        *p = *p + *(short*)((char*)thiz + 0x86);
    }
}

// @symbol Wiggler_Move
extern "C" void Wiggler_Move(char *c)
{
    C *w = (C *)c;
    int v[2];
    int step;
    int v93;

    if (w->alive != 0) {
        step = 0x1000;
        if (w->speed < 0xa00) step = step >> 2;
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
        if (w->busy != 0) v93 = 0x1f4;
        w->voice = func_02012468(w->voice, 2,
            w->voiceId, 2, 0, v93, 0, 0);
    } else {
        w->voice = 0;
    }
    if (w->state.x != 0) {
        int off = w->state.y;
        char *base = c + (off >> 1);
        void (*fn)(char *);
        if (off & 1)
            fn = *(void (**)(char *))(*(char **)base + w->state.x);
        else
            fn = *(void (**)(char *))c;
        fn(base);
    }
    if (w->alive == 0) return;
    Wiggler_StepSegmentsA(w);
}

// @symbol Wiggler_HitTest
extern "C" {
int Wiggler_HitTest(char* raw, const V2* b){
  C *w = (C *)raw;
  int i;
  for (i = 0; i < 5; i++){
    if (func_0203d5dc(&w->seg[i], b) < 0x14000) return 1;
  }
  return 0;
}
}

// @symbol Wiggler_IsStarVisible
extern "C" {
int Wiggler_IsStarVisible(char* raw)
{
    if (*(unsigned char*)(raw + 0x93) == 0) {
        S* q = (S*)(long long)(raw + 8);
        int x = *(int*)raw;
        if (x == q->a && (*(int*)(raw + 4) == q->b || x == 0))
            return 1;
    }
    return 0;
}
}

// @symbol Wiggler_StopIfIdle
extern "C" {
void Wiggler_StopIfIdle(char* c){
  P* d = &pmf_Wiggler_TurnTowardAim_3;
  int c0 = *(int*)c;
  if(c0 == d->v[0] && (*(int*)(c+4) == d->v[1] || c0 == 0)){
    *(unsigned char*)(c+0x93) = 0;
    *(short*)(c+0x78) = 0x60;
    if(*(int*)(c+0x70) == 0){
      Sound::PlayBank2_2D(0x1e9);
    }
    *(P*)c = pmf_Wiggler_StateMoveA;
  }else{
    P* d2 = &pmf_Wiggler_StateWait_3;
    if(c0 != d2->v[0] || (*(int*)(c+4) != d2->v[1] && c0 != 0)){
      *(short*)(c+0x90) = 0x31;
      *(P*)(c+0x10) = *(P*)(c+8);
      Sound::PlayBank2_2D(0x1ed);
    }
    *(int*)(c+0x60) = 0;
    *(P*)c = pmf_Wiggler_StateHold_2;
  }
}
}

// @symbol Wiggler_OnTouched
extern "C" void Wiggler_OnTouched(char *c)
{
    Pair *g = &pmf_Wiggler_TurnTowardAim;
    Pair *p = (Pair *)c;
    if (p->v[0] == g->v[0] && (p->v[1] == g->v[1] || p->v[0] == 0)) {
        *(unsigned char *)(c + 0x93) = 0;
        *(unsigned short *)(c + 0x78) = 0x60;
        if (*(int *)(c + 0x70) == 0)
            Sound::PlayBank2_2D(0x1e9);
        *p = pmf_Wiggler_StateMoveA_2;
    } else {
        *(unsigned char *)(c + 0x95) = 5;
        *(int *)(c + 0x60) = 0;
        *(Pair *)(c + 0x10) = *(Pair *)(c + 8);
        *p = pmf_Wiggler_Update;
    }
}

// @symbol Wiggler_SetCruiseSpeed
extern "C" {
void Wiggler_SetCruiseSpeed(char *raw)
{
    C *w = (C *)raw;
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
void Wiggler_Retarget(struct ObjB* c, int v)
{
    short* h = (short*)(int)(&c->tries);
    struct P2* p = (struct P2*)(int)(&c->saved);
    struct P2* g = (struct P2*)(int)(&pmf_Wiggler_TurnTowardAim_6);

    (*h)++;
    if (p->v[0] == g->v[0] &&
        (p->v[1] == g->v[1] || c->saved.v[0] == 0) &&
        c->tries < 3) {
        c->busy = 0;
        c->timer = (RandomIntInternal(&data_0209e650) & 0x20) + 0x60;
        if (c->tries == 1) {
            c->sub = pmf_Wiggler_TurnTowardAim_5;
            c->saved = c->sub;
        } else {
            c->sub = pmf_Wiggler_MoveBounce;
            c->saved = c->sub;
        }
        if (c->decoy == 0) {
            Sound::PlayBank2_2D(0x1e9);
        }
        c->state = pmf_Wiggler_StateMoveB;
    } else {
        c->speed = 0;
        c->hold = (v >> 12) / 4 + 0x31;
        c->sub = c->saved;
        c->state = pmf_Wiggler_StateWait;
    }
}
}

// @symbol Wiggler_GetFrame
extern "C" {
/* Second parameter unused by the body -- Wiggler_DrawField passes an index
   this callee ignores; declaring it keeps the caller's argument setup. */
int Wiggler_GetFrame(char *raw, int){
  int r2 = 0;
  if (*(unsigned char*)(raw + 0x93) != 0){
    int r1 = *(short*)(raw + 0x8e);
    if (r1 > 0x20){ r2 = 1; goto end; }
    if (r1 > 0x10){ *(unsigned char*)(raw + 0x95) = 3; r2 = 5; goto end; }
    if (r1 > 0) r2 = 4;
    goto end;
  }
  {
    int *ref = pmf_Wiggler_StateWait_4;
    int ip = *(int*)raw;
    if (ip == ref[0]){
      if (*(int*)(raw + 4) == ref[1]) goto C;
      if (ip == 0) goto C;
    }
    ref = pmf_Wiggler_StateHold;
    if (ip != ref[0]) goto end0;
    if (*(int*)(raw + 4) == ref[1]) goto C;
    if (ip != 0) goto end0;
  }
C:
  {
    int v = *(short*)(raw + 0x90);
    if (v < 0x10){ r2 = 5; goto end; }
    if (v < 0x20) r2 = 4;
    else r2 = 0;
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
int Wiggler_GetFlag(char* raw, int n) {
    switch (n) {
    case 0:
        return *(unsigned char*)(raw + 0x95);
    case 1:
        return ((struct B72*)(raw + 0x74))->f0 + 7;
    case 2:
        return ((struct B72*)(raw + 0x74))->f1 + 7;
    case 3:
        return ((struct B72*)(raw + 0x74))->f2 + 7;
    default:
        return ((struct B72*)(raw + 0x74))->f3 + 7;
    }
}
}

// @symbol Hana_PickUnused
extern "C" {
int Hana_PickUnused(int n, int mask)
{
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
void Hana_ChooseStarWiggler(int count)
{
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
int Hana_PickMoveKind(void){
  int v = 3;
  int r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
  if (r < 0x400) v = 0;
  else if (r < 0x800) v = 1;
  else if (r < 0xc00) v = 2;
  return v;
}
}

// @symbol Wiggler_ClampToScreen
extern "C" {
int Wiggler_ClampToScreen(char *raw){
    C *w = (C *)raw;
    int r=0;
    int v=w->seg[1].x;
    if(v>=0xe8000){ r|=1; v=0xe8000; w->seg[1].x=v; r&=0xff; }
    else if(v<=0x18000){ r|=2; v=0x18000; w->seg[1].x=v; r&=0xff; }
    v=w->seg[1].y;
    if(v>=0xa8000){ r|=8; v=0xa8000; w->seg[1].y=v; r&=0xff; }
    else if(v<=0x18000){ r|=4; v=0x18000; w->seg[1].y=v; r&=0xff; }
    return r;
}
}

// @symbol Wiggler_StepSegmentsA
extern "C" {
void Wiggler_StepSegmentsA(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i;
    s16 na;
    int j;
    s16 d2;
    int cur;
    V2* pj;
    int zero;
    int step;
    int amt;
    V2* vb2;

    if (c->bloom != 0)
        return;

    UpdateAngle(c->ang[1], c->aim, 8, 0x100);

    i = 2;
    step = 2;
    zero = 0;
    amt = 0x100;

    for (; i < 5; i++) {
        j = i - 1;
        pj = &c->seg[j];
        Vec2_Sub(&d, &c->seg[i], pj);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, diff;
            func_0203d630((int *)&d, c->stretch[i] + i * 0x2000);
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->ang[j];
            d2 = zero;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            UpdateAngle(c->ang[i], na, step, amt);
            cur = c->ang[i];
            cur = na - cur;
            func_0203d388((int *)&d, (s16)(d2 - cur));
        }
        func_0203d704(&e, pj, &d);
        c->seg[i].x = e.x;
        c->seg[i].y = e.y;
    }

    {
        s16 dd = (s16)(c->aim - c->ang[1]);
        g.x = 0;
        g.y = -0x10000;
        UpdateAngle(c->ang[0], (s16)(c->ang[1] + dd), 8, 0x100);
    }
    func_0203d388((int *)&g, c->ang[0]);
    vb2 = c->seg;
    func_0203d704(&h, vb2 + 1, &g);
    c->seg[0].x = h.x;
    c->seg[0].y = h.y;
}
}

// @symbol Wiggler_StepSegmentsB
extern "C" {
void Wiggler_StepSegmentsB(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 4;
    V2* p = &c->seg[i];
    int m = 0x8000;
    s16 zero = 0;
    int j;

    for (; i >= 1; i--) {
        j = i - 1;
        Vec2_Sub(&d, p, &c->seg[j]);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            { int w = c->stretch[i]; func_0203d630((int *)&d, w + m); }
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->ang[j];
            d2 = zero;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            cur = c->ang[i];
            func_0203d388((int *)&d, (s16)(d2 - (na - cur)));
        }
        Vec2_Sub(&e, p, &d);
        c->seg[j].x = e.x;
        c->seg[j].y = e.y;
        p--;
        m -= 0x2000;
    }
    g.x = 0;
    g.y = -0x10000;
    func_0203d388((int *)&g, c->ang[0]);
    func_0203d704(&h, (V2*)(c->seg) + 1, &g);
    c->seg[0].x = h.x;
    c->seg[0].y = h.y;
}
}

// @symbol Wiggler_StepSegmentsC
extern "C" {
void Wiggler_StepSegmentsC(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 2;
    V2* p = &c->seg[i];
    int j;

    for (; i < 5; i++) {
        j = i - 1;
        Vec2_Sub(&d, p, &c->seg[j]);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            func_0203d630((int *)&d, (i * 2 + 0x10) << 12);
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->ang[j];
            d2 = 0;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            c->ang[i] = na;
            cur = c->ang[i];
            func_0203d388((int *)&d, (s16)(d2 - (na - cur)));
        }
        func_0203d704(&e, &c->seg[j], &d);
        c->seg[i].x = e.x;
        c->seg[i].y = e.y;
        p++;
    }
    g.x = 0;
    g.y = -0x10000;
    c->ang[0] = (s16)(c->ang[1] + (((c->ang[1] - c->ang[2]) << 14) >> 16));
    func_0203d388((int *)&g, c->ang[0]);
    func_0203d704(&h, (V2*)(c->seg) + 1, &g);
    c->seg[0].x = h.x;
    c->seg[0].y = h.y;
}
}

// @symbol Wiggler_CheckOffLeftA
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void Wiggler_CheckOffLeftA(C* self) {
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
}

// @symbol Wiggler_StartPathA
extern "C" {
void Wiggler_StartPathA(Obj *self, int n)
{
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

    Wiggler_StepSegmentsC((struct C *)self);
    self->voice = 0;
    self->voiceId = 0x1ec;
    *(struct Words2 *)&self->state = *(struct Words2 *)&pmf_Wiggler_CheckOffLeftA;
    *(struct Words2 *)&self->saved = *(struct Words2 *)&self->state;
}
}


// @symbol Wiggler_MoveBounce
extern "C" {
void Wiggler_MoveBounce(char *raw)
{
    int res = Wiggler_ClampToScreen(raw);
    if (res != 0) {
        int tx;
        int ty;
        int y;
        int x;
        s16 a;
        s16 d;
        x = *(int*)(raw + 0x20);
        y = *(int*)(raw + 0x24);
        tx = x;
        ty = y;
        if (res | 3) tx = 0x80000;
        if (res | 0xc) ty = 0x60000;
        a = _ZN4cstd5atan2E5Fix12IiES1_(tx - x, -(ty - y));
        d = (s16)(a - *(s16*)(raw + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(raw + 0x76) = (s16)(*(s16*)(raw + 0x7c) + d);
        return;
    }
    {
        int ra;
        int rb;
        s16 a;
        s16 d;
        *(s16*)(int)(raw + 0x78) -= 1;
        if (*(s16*)(raw + 0x78) >= 0) return;
        *(s16*)(raw + 0x78) = (s16)((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) * 0x1e) >> 12) + 0x3c);
        ra = RandomIntInternal(&data_0209e650);
        rb = RandomIntInternal(&data_0209e650);
        a = _ZN4cstd5atan2E5Fix12IiES1_(
            ((int)((unsigned int)(ra & 0x7fffffff) >> 19) * 0x100) - *(int*)(raw + 0x20),
            -(((int)((unsigned int)(rb & 0x7fffffff) >> 19) * 0xc0) - *(int*)(raw + 0x24)));
        d = (s16)(a - *(s16*)(raw + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(raw + 0x76) = (s16)(*(s16*)(raw + 0x7c) + d);
    }
}
}


// @symbol Wiggler_StartBounce
extern "C" {
void Wiggler_StartBounce(char *raw, int)
{
    int i;
    *(int *)(raw + 0x40) = 0xa00;
    *(int *)(raw + 0x44) = 0xa00;
    for (i = 0; i < 5; i++) {
        ((struct E *)(raw + 0x18))[i].a = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) << 8);
        ((struct E *)(raw + 0x18))[i].b = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xc0);
        ((short *)(raw + 0x7a))[i] = (short)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    }
    *(short *)(raw + 0x76) = (short)((*(short *)(raw + 0x7a) + *(short *)(raw + 0x7c)) >> 1);
    Wiggler_ClampToScreen(raw);
    Wiggler_StepSegmentsC((struct C *)raw);
    *(short *)(raw + 0x78) = 0x1e;
    *(int *)(raw + 0x68) = 0;
    *(int *)(raw + 0x6c) = 0x1ec;
    *(struct Words2 *)(raw + 0) = pmf_Wiggler_MoveBounce_2;
    *(struct Words2 *)(raw + 8) = *(struct Words2 *)(raw + 0);
}
}


// @symbol Wiggler_CheckOffLeftB
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void Wiggler_CheckOffLeftB(C* self) {
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
}


// @symbol Wiggler_StartPathB
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void Wiggler_StartPathB(char* raw, int idx)
{
    int n1;
    int A, B;
    int p;
    int valY;
    int pos[2];
    int i;
    int off;

    *(int*)(raw + 0x40) = 0xa00;
    *(int*)(raw + 0x44) = 0xa00;

    n1 = sHanaFieldWigglerCount - 1;
    A = cstd::fdiv(idx << 12, n1 << 12);
    B = cstd::fdiv((n1 - idx) << 12, n1 << 12);

    p = (n1 - 2) << 3;
    valY = FX_MUL(((0x28 - p) << 12), A) + FX_MUL(((p + 0x98) << 12), B);

    *(int*)((char*)pos + 4) = 0;
    *(int*)((char*)pos + 0) = 0;
    *(int*)((char*)pos + 4) = valY;

    if (RandomIntInternal(&data_0209e650) & 0x8000000) {
        *(short*)(raw + 0x76) = -0x4000;
        pos[0] = 0x80000;
        pos[0] += ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    } else {
        *(short*)(raw + 0x76) = 0x4000;
        pos[0] = 0x80000;
        pos[0] -= ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    }

    for (i = 0, off = 0; i < 5; i++) {
        *(int*)(raw + i * 8 + 0x18) = pos[0];
        *(int*)(raw + i * 8 + 0x1c) = pos[1];
        if (*(short*)(raw + 0x76) < 0)
            *(int*)(raw + i * 8 + 0x18) += off;
        else
            *(int*)(raw + i * 8 + 0x18) -= off;
        *(short*)(raw + i * 2 + 0x7a) = *(short*)(raw + 0x76);
        off += 0x20000;
    }

    Wiggler_StepSegmentsC((struct C *)raw);
    *(int*)(raw + 0x68) = 0;
    *(int*)(raw + 0x6c) = 0x1ec;
    *(struct Words2 *)(raw + 0) = *(struct Words2 *)pmf_Wiggler_CheckOffLeftB;
    *(struct Words2 *)(raw + 8) = *(struct Words2 *)(raw + 0);
}
#pragma pop
}


// @symbol Wiggler_TurnTowardAim
extern "C" {
void Wiggler_TurnTowardAim(void *arg)
{
  C *w = (C *) arg;
  ApproachLinear(w->ang[0], (short)(w->aim - 0x2000), 0x100);
}
}


// @symbol Wiggler_StartPathC
extern "C" {
void Wiggler_StartPathC(struct C* c, int param)
{
    V2 vec0;
    V2 vec1;
    V2 vec3;
    V2 vec2;
    V2 vec4;
    int m, r1, r2, k, i, j;

    c->bloom = 1;
    c->flower = 0x3000;
    c->face = 0;
    c->bobRate = 0x200;
    c->speed = 0;
    c->speedTarget = 0;

    m = sHanaFieldWigglerCount - 1;
    r1 = cstd::fdiv(param << 12, m << 12);
    r2 = cstd::fdiv((m - param) << 12, m << 12);
    k = (m - 2) * 8;

    vec0.y = 0;
    vec0.y = FX_MUL(((0x40 - k) << 12), r1) + FX_MUL(((k + 0x98) << 12), r2);
    vec0.x = 0;
    if (param & 1)
        vec0.x = 0xa8000;
    else
        vec0.x = 0x38000;

    c->aim = -0x8000;
    c->seg[0].x = *(int*)&vec0;
    c->seg[0].y = *((int*)&vec0 + 1);
    c->ang[0] = c->aim - 0x2000;

    for (i = 1; i < 5; i++) {
        j = i - 1;
        vec1.x = 0;
        vec1.y = (i * 2 + 0x10) << 12;
        func_0203d388((int *)&vec1, (s16)(c->ang[j] + 0x2000));
        c->ang[i] = c->ang[j] + 0x2000;
        func_0203d704(&vec2, &c->seg[j], &vec1);
        c->seg[i].x = vec2.x;
        c->seg[i].y = vec2.y;
    }

    Wiggler_StepSegmentsC(c);

    vec3.x = 0;
    vec3.y = -0x10000;
    c->ang[0] = c->aim - 0x2000;
    func_0203d388((int *)&vec3, c->ang[0]);
    func_0203d704(&vec4, &c->seg[1], &vec3);
    c->seg[0].x = vec4.x;
    c->seg[0].y = vec4.y;

    *(struct Words2 *)&c->state = *(struct Words2 *)&pmf_Wiggler_TurnTowardAim_4;
    *(struct Words2 *)&c->saved = *(struct Words2 *)&c->state;
}
}


// @symbol Hana_SpawnWigglers
extern "C" {
void Hana_SpawnWigglers(char *raw, int arg1, int arg2)
{
    int i;
    struct C *o = (struct C *)raw;

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
        Wiggler_StartPathC((struct C *)raw, arg1);
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
s32 dScMgHanachan_c::CleanupResources()
{
    Mg_CommitRecords(this);
    return 1;
}


// @symbol _ZN15dScMgHanachan_c6RenderEv
#pragma push
#pragma opt_common_subs off
s32 dScMgHanachan_c::Render()
{
    char *raw = (char *)this;
    int i;
    char *arr;

    Mg_DrawScoreTally(raw, 0xe0, 0x14, 1);
    HanaPopup_DrawAll();
    StarPopup_Draw();

    {
        Pair *p = PAIR0(raw);
        Pair *g = &pmf_dScMgHanachan_c_Phase_Play;
        if (p->v[0] == g->v[0]) {
            if (p->v[1] == g->v[1] || mPhaseFn == 0) {
                i = 0;
                if (sHanaFieldWigglerCount > 0) {
                    arr = raw + 0x4678;
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
        Pair *p = PAIR0(raw);
        Pair *g = &pmf_dScMgHanachan_c_Phase_Result;
        if (p->v[0] == g->v[0]) {
            if (p->v[1] == g->v[1] || mPhaseFn == 0) {
                int flag = mHit;
                if (flag != 0) {
                    if (mPhaseTimer < 0x5a) goto do_eac;
                }
                if (flag != 0) goto count_loop;
                if (mPhaseTimer < 0x8a) goto do_eac;
                goto count_loop;
            do_eac:
                Wiggler_DrawField(mTarget);
                goto tail;
            count_loop:
                i = 0;
                if (sHanaFieldWigglerCount > 0) {
                    arr = raw + 0x4678;
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
        Pair *p = PAIR0(raw);
        Pair *g = &pmf_Hana_StateNoneB;
        if ((p->v[0] == g->v[0] && (p->v[1] == g->v[1] || mPhaseFn == 0))
            || (p = PAIR0(raw), g = &pmf_dScMgHanachan_c_Phase_WaitTouch, (p->v[0] == g->v[0] && (p->v[1] == g->v[1] || mPhaseFn == 0)))) {
            Wiggler_DrawField(mTarget);
        }
    }

tail:
    Wiggler_DrawTarget(mTarget);

    {
        Pair *p = PAIR0(raw);
        Pair *g = &pmf_dScMgHanachan_c_Phase_WaitIntro_2;
        if (!(p->v[0] == g->v[0] && (p->v[1] == g->v[1] || mPhaseFn == 0))) {
            int idx = GetGameLanguage();
            void *e = sHanaTextSpritesByLanguage[idx];
            void *f = *(void **)((char *)e + 0xc);
            DrawOamSprite(f, 0x80, 0x10, 0);
            Mg_DrawNumberRotated(0x80, 0x28, (void *)mTimeLeft, 1, -1, 0x800, 0);
        }
    }

    return 1;
}
#pragma pop


// @symbol _ZN15dScMgHanachan_c8BehaviorEv
s32 dScMgHanachan_c::Behavior()
{

    sHanaSpinAngle = sHanaSpinAngle + 0x800;
    (((CB *)this)->*(*(PMF *)&mPhaseFn))();
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


// @symbol Hana_StateNoneA
extern "C" {
void Hana_StateNoneA(void)
{
}
}


// @symbol _ZN15dScMgHanachan_c15Phase_WaitTouchEv
void dScMgHanachan_c::Phase_WaitTouch()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    int idx;
    int b;
    int w0, w1;
    s->mPhaseTimer -= 1;
    if (s->mPhaseTimer != 0)
    {
        idx = gActivePlayerSlot;
        b = 0;
        if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    w0 = pmf_Hana_StateNoneA[0];
    w1 = pmf_Hana_StateNoneA[1];
    w0 = w1 ? w0 : w0;
    s->mPhaseFn = w0;
    s->mPhaseAdj = w1;
}


// @symbol _ZN15dScMgHanachan_c15BeginResultWaitEv
void dScMgHanachan_c::BeginResultWait()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    s->mPhaseTimer = 0xb4;
    *(Pair *)&s->mPhaseFn = pmf_dScMgHanachan_c_Phase_WaitTouch_2;
}


// @symbol Hana_StateNoneB
extern "C" {
void Hana_StateNoneB(void)
{
}
}


// @symbol _ZN15dScMgHanachan_c12SetPhaseIdleEv
void dScMgHanachan_c::SetPhaseIdle()
{
    char *raw = (char *)this;
    *(Pair *)&((dScMgHanachan_c *)raw)->mPhaseFn = pmf_Hana_StateNoneB_2;
}


// @symbol _ZN15dScMgHanachan_c12Phase_ResultEv
void dScMgHanachan_c::Phase_Result()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    int v;
    s->mPhaseTimer -= 1;
    v = s->mPhaseTimer;
    if (v == 0)
    {
        if (s->mHit != 0)
        {
            Mg_SetSharedState(MG_STATE_NEXT_ROUND);
            SetPhaseIdle();
        }
        else
        {
            ((C *)s->mTarget)->decoy = 1;
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            BeginResultWait();
        }
        s->mPromptEnabled = 0;
    }
    else if (s->mHit != 0)
    {
        if (v == 0x5a)
        {
            Wiggler_OnTouched((char *)s->mTarget);
        }
    }
    else
    {
        if (v == 0x8a)
        {
            Wiggler_StopIfIdle((char *)s->mTarget);
        }
    }
}


// @symbol _ZN15dScMgHanachan_c11BeginResultEv
void dScMgHanachan_c::BeginResult()
{
    char *raw = (char *)this;
    int i = 0;
    if (i < sHanaFieldWigglerCount) {
        char *p = raw;
        do {
            *(unsigned char *)(p + 0x4000 + 0x70a) = 0;
            i++;
            p += 0x98;
        } while (i < sHanaFieldWigglerCount);
    }
    if (mHit != 0)
        mPhaseTimer = 0xb4;
    else
        mPhaseTimer = 0xe4;
    *(struct Words2 *)(raw + 0x4000 + 0x660) = *(struct Words2 *)pmf_dScMgHanachan_c_Phase_Result_2;
}


// @symbol _ZN15dScMgHanachan_c10Phase_PlayEv
void dScMgHanachan_c::Phase_Play()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    V2 a;
    int vec[4];
    int b = 0;
    int idx = gActivePlayerSlot;

    if (gTouchHeld[idx * 4] != 0 && gTouchEdge[idx * 4] != 0)
        b = 1;

    if (b != 0)
    {
        Thing *p;
        V2 *src;
        a.x = gTouchX[(unsigned int)idx * 4] << 12;
        a.y = gTouchY[(unsigned int)idx * 4] << 12;
        p = (Thing *)s->mTarget;
        src = (V2 *)(int)((char *)p + 0x18);
        vec[0] = src->x;
        vec[1] = src->y;

        if (Wiggler_IsStarVisible((char *)s->mTarget) != 0 &&
            Wiggler_HitTest((char *)s->mTarget, &a) != 0)
        {
            s->mHit = 1;
            func_02012790(0x26);
            p = (Thing *)s->mTarget;
            {
            int *g = pmf_Wiggler_TurnTowardAim_9;
            if (p->a == g[0] &&
                (p->b == g[1] || p->a == 0))
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
                do
                {
                    if (Wiggler_IsStarVisible((char *)ptr) != 0 &&
                        Wiggler_HitTest((char *)ptr, &a) != 0)
                    {
                        found = i;
                        break;
                    }
                    i++;
                    ptr++;
                } while (i < sHanaFieldWigglerCount);
            }

            if (found != -1)
            {
                int j;
                Thing *ptr2;
                Thing *sel = (Thing *)s->mWiggler + found;
                {
                int *g2 = pmf_Wiggler_TurnTowardAim_10;
                if (sel->a == g2[0] &&
                    (sel->b == g2[1] || sel->a == 0))
                    Sound::PlayBank2_2D(0x1eb);
                else
                    Sound::PlayBank2_2D(0x1ed);
                }

                {
                    j = 0;
                    if (sHanaFieldWigglerCount > 0) {
                        ptr2 = (Thing *)s->mWiggler;
                        do
                        {
                            V2 *s2 = (V2 *)(int)((char *)ptr2 + 0x18);
                            vec[0] = s2->x;
                            vec[1] = s2->y;
                            Wiggler_Retarget((struct ObjB *)ptr2, func_0203d5dc((const V2 *)vec, &a));
                            j++;
                            ptr2++;
                        } while (j < sHanaFieldWigglerCount);
                    }
                }

                func_02012790(0xe);
                {
                    V2 *s3 = (V2 *)(int)((char *)sel + 0x18);
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
    if (ApproachLinear(s->mTimeLeft, 0, 1) != 0)
    {
        func_02012790(0xe);
        BeginResult();
    }

    if (s->mTimeLeft > 2)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
}


// @symbol _ZN15dScMgHanachan_c9BeginPlayEv
void dScMgHanachan_c::BeginPlay()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    s->mPhaseTimer = 0x4b0;
    *(Pair *)&s->mPhaseFn = pmf_dScMgHanachan_c_Phase_Play_2;
}


// @symbol _ZN15dScMgHanachan_c15Phase_WaitIntroEv
void dScMgHanachan_c::Phase_WaitIntro()
{
    char *raw = (char *)this;
    dScMgHanachan_c *s = (dScMgHanachan_c *)raw;
    if (ApproachLinear(s->mDelay, 0, 1) == 0) return;
    FreeGfxSlotsById(0x1d);
    if (s->mPromptBlinkCount == 0) {
        s->mPromptEnabled = 1;
        s->mPromptBlinkCount = 1;
        s->mPromptBlinkTimer = 0;
    }
    BeginPlay();
}


// @symbol _ZN15dScMgHanachan_c16Phase_SetupRoundEv
void dScMgHanachan_c::Phase_SetupRound()
{
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
        s->mTarget = (char *)((Thing *)s->mWiggler + k);
        ((C *)s->mTarget)->decoy = 0;
        s->mDelay = 0x3c;
        Mg_ShowBanner(0xd, 0x80, 0x50, 1, -1, 0xd);
        s->mTimeTick = 0x3c;
        s->mHit = 0;
        *(Pair *)&s->mPhaseFn = pmf_dScMgHanachan_c_Phase_WaitIntro;
    }
}


// @symbol _ZN15dScMgHanachan_c10StartRoundEi
void dScMgHanachan_c::StartRound(int state)
{
    char *raw = (char *)this;

    if (state == 1) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc field
           address into its own register (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the
           function grows a word; the cartridge re-issues ldr r1,[r4,#0xbc]. */
        mLevel = *(const unsigned int*)(raw + 0xbc) + 1;
        if (mLevel > 0x270e) mLevel = 0x270e;
    } else if (state == 0x12) {
        mLevel = 0;
        if (mLevel > 0x270e) mLevel = 0x270e;
        if (gMgScene != 0) gMgScene->mHudScore = 0;
        Mg_SubmitScore(mHudScore);
        mTimeLeft = 0x14;
    } else {
        int v = mLevel;
        if (gMgScene != 0) gMgScene->mHudScore = v;
        mTimeLeft = 0x14;
    }
    HanaPopup_ClearAll();
    StarPopup_Clear();
    Phase_SetupRound();
}


// @symbol _ZN15dScMgHanachan_c13InitResourcesEv
s32 dScMgHanachan_c::InitResources()
{
    void *a;
    void *b;
    SelfV *obj = (SelfV *)this;

    Hana_LoadBackground();
    a = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCG_BIN);
    b = LoadFile(ASSET_D_2D_MG_HANA_SAGASE_NCL_BIN);
    DecompressLZ16(a, (void *)0x6400000);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0xc0);
    GX::LoadOBJPltt(b, 0, 0xc0);
    Deallocate(a);
    Deallocate(b);
    obj->unkA8 = data_ov006_0213c954;
    obj->unkAC = obj->unkA8;
    Mg_SetTopScreenYOffset(0x20);
    obj->v18(-1);
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
void Hana_LoadBackground(void){
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
extern void* data_ov006_021374b8;
extern void* data_ov006_021374c4;
extern void* data_ov006_021374ac;
extern void* data_ov006_021374e8;
extern void* data_ov006_021374f4;
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

