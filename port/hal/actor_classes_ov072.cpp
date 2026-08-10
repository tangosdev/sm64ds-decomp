// GATE 193: the actual SNOWMAN (272), ov072. Fresh per-symbol mount
// (port/ov072_syms.txt), the ov079/ov080/ov081 convention. A plain
// 31-slot Actor table (no Kill -- Actor-derived, not Platform-derived).
// Full slot derivation, the class-identity correction (id 272 is the
// UNNAMED daBgSnwmn_c block, NOT SnowmanBody or SnowmanHead) and the
// vtable-slot-count correction are documented in port/ov072_syms.txt's
// own header. Its vtable was read directly out of
// extracted/overlays/overlay_0072.bin (ROM-byte verified, not inferred
// from relocs alone).
//
// SNOWMAN'S OWN PMF-FREE SHAPE: no state-cell dispatch of its own -- its
// nine files are all plain calls, matching MotherPenguin's gate-191
// shape (a plain Actor with a real MSVC-synthesised D1 destructor and a
// D0 that stores its own table and never overwrites it).
//
// PREPARE SHORT-1 BUG: func_ov072_02120a44's InitResources calls
// TextureSequence::Prepare with the MotherPenguin (gate 191) two-argument
// shape; HOST COPY port/unmatched/Snowman_InitResources.cpp fixes it.
//
// SNOWMAN_HEAD (273) COUPLING: daBgSnwmn_c::InitResources spawns
// SnowmanHead (Actor::Spawn(0x111, ...)) unconditionally when star 5 is
// not yet collected, then marks itself for destruction the same frame.
// SnowmanHead stays UNREGISTERED this gate -- the Spawn call reaches the
// registry's own "not registered" skip path, not a crash (verified at
// runtime, see the commit message).
//
// BABY_PENGUIN (256) IS REGISTERED (babypenguin-crash lane). The crash
// that excluded it from gate 193 is ROOT-CAUSED and fixed: the matched
// src _ZN11BabyPenguin6RenderEv.cpp dispatched ModelAnim slot 5 through a
// local six-virtual ROM-order shadow, which the host _ZTV9ModelAnim
// numbers as Virtual18 (the documented ModelAnim slot-5 collision, the
// Whomp/Butterfly/Fish case) -- Virtual18's __fastcall trampoline pops
// TWO stack words where the shadow call pushed one, the skewed caller
// rets into its own spilled `self`, and the jump surfaces as a DEP
// execute-violation inside port_actor_process's frame (which is why it
// first read as a "shared actor-Process pipeline" bug -- the pipeline
// was the victim frame, not the culprit). Fix: the matched Render TU is
// dropped from slice_gate193.txt (byte-locked in src/ as proof) and
// _ZN11BabyPenguin6RenderEv is a HOST COPY in
// port/unmatched/ModelAnim_Renders.cpp with the dispatch spelled as the
// qualified ModelAnim::Render. The six-cell PMF state table, the
// func_ov072_02121d50 SHORT-1 fixes and both state host copies were
// verified INNOCENT during the hunt (all seat-checked valid at the
// moment of the crash). daBgSnwmn_c is not exposed: its +0xd4/+0x124
// members are plain 0x50-stride Models and _ZTV5Model[5] is dual-filled.
//
// FindWithActorID(0x101) MOTHER COUPLING (BabyPenguin::Behavior): looks
// up MOTHER_PENGUIN (id 257, hosted since gate 191) by actor id when in
// range and on-ground; null-guarded at the call site -- CLEAN even
// before 191, MOOT now that 191 hosts the mother. Documented here even
// though BABY_PENGUIN is unregistered this gate, since the fill and the
// PMF work are complete and this finding stands on its own.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "BabyPenguin.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int _ZN5Actor12BeforeRenderEv(void *self);
int _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
int _ZN5Actor16OnAimedAtWithEggEv(void *self);
void _ZN5ActorD2Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN12WithMeshClsnD1Ev(void *self);
extern void *data_020a0eac;
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
void port_actor_render_probe(const char *cls, void *model);
}

// ---- the shared 1..30 half, both 31-slot plain-Actor tables share it ----
static int __fastcall ov72_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov72_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov72_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov72_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov72_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov72_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov72_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov72_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov72_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov72_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov72_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov72_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov72_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov72_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov72_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov72_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov72_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov72_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov72_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov72_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov72_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov72_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30 -- every shared slot both
   31-slot plain-Actor tables share. The caller writes its own
   0/3/6/9/12/16/17 (and daBgSnwmn_c/BabyPenguin's slot 18/19 overrides
   afterward, where each has one). */
static void ov72_fill_shared(void **vt)
{
    vt[1]  = (void *)ov72_binit;
    vt[2]  = (void *)ov72_ainit;
    vt[4]  = (void *)ov72_bclean;
    vt[5]  = (void *)ov72_aclean;
    vt[7]  = (void *)ov72_bbeh;
    vt[8]  = (void *)ov72_abeh;
    vt[10] = (void *)ov72_bren;
    vt[11] = (void *)ov72_aren;
    vt[13] = (void *)ov72_heap;
    vt[14] = (void *)ov72_v50;
    vt[15] = (void *)ov72_pdes;
    vt[18] = (void *)ov72_yoshi;
    vt[19] = (void *)ov72_egg;
    vt[20] = (void *)ov72_pounded;
    vt[21] = (void *)ov72_atk1;
    vt[22] = (void *)ov72_atk2;
    vt[23] = (void *)ov72_kicked;
    vt[24] = (void *)ov72_pushed;
    vt[25] = (void *)ov72_cannon;
    vt[26] = (void *)ov72_mega;
    vt[27] = (void *)ov72_under;
    vt[28] = (void *)ov72_aimed;
    vt[29] = (void *)0;   /* unused past this point for these two classes */
    vt[30] = (void *)0;
}

// ============================================================================
// SNOWMAN (daBgSnwmn_c, 272), a plain Actor, 31 slots. Own overrides:
// 0 Init (HOST COPY, Prepare SHORT-1 fix), 3 Cleanup, 6 Behavior,
// 9 Render, 12 OnPendingDestroy (own empty body), 16 D1, 17 D0. No own
// slot 18+ override -- confirmed by the raw vtable dump (words 18..30 all
// match the shared arm9 defaults byte for byte).
// ============================================================================
extern "C" {
int func_ov072_02120a44(char *self);       /* slot 0, HOST COPY (Snowman_InitResources.cpp) */
int func_ov072_02120980(void);             /* slot 3, .c body takes void */
int func_ov072_02120a08(char *self);       /* slot 6 */
int func_ov072_021209c0(char *self);       /* slot 9 */
void func_ov072_021209bc(void);            /* slot 12, own, empty body */
int *func_ov072_02120824(int *self);       /* slot 16, D1 */
int *func_ov072_02120874(int *self);       /* slot 17, D0 */
void *func_ov072_02120c00(void);           /* installs the class's own (unnamed) vtable */
int _ZTV11daBgSnwmn_c[31];                 /* the class's own vtable, host array -- ALL matched src spells it by this mangled name directly, no alias needed */
}

static int __fastcall snm_init(void *s, void *)
{ return func_ov072_02120a44((char *)s); }
static int __fastcall snm_clean(void *s, void *)
{ (void)s; return func_ov072_02120980(); }
static int __fastcall snm_behavior(void *s, void *)
{ return func_ov072_02120a08((char *)s); }
static int __fastcall snm_render(void *s, void *)
{ port_actor_render_probe("SNOWMAN", (char *)s + 0xd4);
  return func_ov072_021209c0((char *)s); }
static int __fastcall snm_pdes(void *s, void *)
{ (void)s; func_ov072_021209bc(); return 0; }
static int __fastcall snm_d1(void *s, void *)
{ return (int)(size_t)func_ov072_02120824((int *)s); }
static int __fastcall snm_d0(void *s, void *)
{ return (int)(size_t)func_ov072_02120874((int *)s); }

extern "C" void hal_fill_snowman_vtable(void)
{
    void **vt = (void **)_ZTV11daBgSnwmn_c;
    ov72_fill_shared(vt);
    vt[0]  = (void *)snm_init;
    vt[3]  = (void *)snm_clean;
    vt[6]  = (void *)snm_behavior;
    vt[9]  = (void *)snm_render;
    vt[12] = (void *)snm_pdes;
    vt[16] = (void *)snm_d1;
    vt[17] = (void *)snm_d0;
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ============================================================================
// BABY_PENGUIN (daPgBby_c, 256), a plain Actor, 31 slots
// (_ZTV11BabyPenguin == _ZTV9daPgBby_c). Own overrides: 0 Init, 3 Cleanup,
// 6 Behavior, 9 Render, 12 OnPendingDestroy (own, empty body --
// func_ov072_02121db0), 16 D1 (real MSVC-synthesised empty destructor, no
// vtable-store -- chain spelled directly, the MotherPenguin/OneUpLogo
// shape), 17 D0 (stores its own table, never overwrites -- stays in
// slice), 18 own OnYoshiTryEat (returns 7, NOT the shared default),
// 19 own OnTurnIntoEgg (a tail-call veneer straight to
// ActorBase::MarkForDestruction). Six-cell PMF state table seated below
// before InitResources can dispatch through it.
// ============================================================================
extern "C" {
int _ZN11BabyPenguin13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN11BabyPenguin16CleanupResourcesEv(void);         /* slot 3, .c body takes void */
int _ZN11BabyPenguin8BehaviorEv(void *self);             /* slot 6, faced */
int _ZN11BabyPenguin6RenderEv(void *self);                /* slot 9, faced */
void _ZN11BabyPenguin16OnPendingDestroyEv(void);        /* slot 12, own, .c body takes void */
int *_ZN11BabyPenguinD0Ev(int *self);                    /* slot 17, spells its own table */
int func_ov072_02120cfc(void);                            /* slot 18, own OnYoshiTryEat, returns 7 */
void func_ov072_02121fa0(void);                           /* slot 19, own OnTurnIntoEgg */
void *BabyPenguin_Spawn(void);                             /* installs _ZTV11BabyPenguin */
int _ZTV11BabyPenguin[31];

/* the six-cell PMF state table: each cell is TWO 8-byte {fn,delta} pairs
   back to back (the .a=enter half, .b=tick half __sinit_ov072_02122414's
   own struct shows), 0x10 bytes/cell -- data_ov072_02122d6c[6]. ROM-byte
   verified against overlay_0072.bin (see this file's own header), every
   delta 0. */
struct PortBabyPenguinPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortBabyPenguinPair data_ov072_02122d6c[6];

/* the twelve real state-handler bodies the six cells' enter/tick halves
   point at -- all already matched src in the slice (func_ov072_021217ac
   is the ONE exception: its host copy, BabyPenguin_LandTransition.cpp,
   supplies this exact symbol name instead, replacing the func_ov072_02121d50
   SHORT-1 caller -- same address, same signature, transparent to the seat
   below). All twelve take one pointer, return int -- the uniform shape
   every state handler in this table shares. */
int func_ov072_02121c94(char *p);
int func_ov072_02121a84(void *c);
int func_ov072_02121a28(char *c);
int func_ov072_021218dc(char *self);
int func_ov072_02121890(char *c);
int func_ov072_021217ac(void *thiz);
int func_ov072_02121758(char *c);
int func_ov072_02121670(char *c);
int func_ov072_02121640(char *c);
int func_ov072_021214dc(void *thiz);
int func_ov072_02121368(char *self);
int func_ov072_021212c0(void *thiz);
}

/* {ROM address the sinit's own source cell carries, verified against
   overlay_0072.bin before the rewrite, host body -- the SoundObject/Cap/
   MrBlizzard seat shape: a mount pointing at the wrong bytes aborts
   instead of silently calling into garbage}. Order matches
   __sinit_ov072_02122414's own field order (NOT sorted by address). */
typedef int (*PortBpFn)(void *);
static const struct { unsigned enter_rom, tick_rom; PortBpFn enter_host, tick_host; }
g_bp_cells[6] = {
    {0x02121c94, 0x02121a84, (PortBpFn)func_ov072_02121c94, (PortBpFn)func_ov072_02121a84},
    {0x02121a28, 0x021218dc, (PortBpFn)func_ov072_02121a28, (PortBpFn)func_ov072_021218dc},
    {0x02121890, 0x021217ac, (PortBpFn)func_ov072_02121890, (PortBpFn)func_ov072_021217ac},
    {0x02121758, 0x02121670, (PortBpFn)func_ov072_02121758, (PortBpFn)func_ov072_02121670},
    {0x02121640, 0x021214dc, (PortBpFn)func_ov072_02121640, (PortBpFn)func_ov072_021214dc},
    {0x02121368, 0x021212c0, (PortBpFn)func_ov072_02121368, (PortBpFn)func_ov072_021212c0},
};

/* Seat the six cells with HOST function addresses, the MrBlizzard shape:
   verify the sinit copied the ROM's own {address,0} pairs (WRONG BYTES
   aborts instead of silently calling into garbage), then overwrite
   enter_fn/tick_fn with the host bodies' own addresses -- BabyPenguin_
   StateEnter.cpp / BabyPenguin_StateTick.cpp dispatch through these same
   fields as plain function pointers, so they must hold host code, not
   the ROM's DS addresses. */
extern "C" void port_baby_penguin_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 6; ++i) {
        PortBabyPenguinPair &cell = data_ov072_02122d6c[i];
        if (cell.enter_fn != g_bp_cells[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != g_bp_cells[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: BabyPenguin state cell %d: the sinit "
                         "left %08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell.enter_fn, cell.enter_delta, cell.tick_fn, cell.tick_delta,
                         g_bp_cells[i].enter_rom, g_bp_cells[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)g_bp_cells[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)g_bp_cells[i].tick_host;
    }
}

static int __fastcall bp_init(void *s, void *)
{ return _ZN11BabyPenguin13InitResourcesEv(s); }
static int __fastcall bp_clean(void *s, void *)
{ (void)s; return _ZN11BabyPenguin16CleanupResourcesEv(); }
static int __fastcall bp_behavior(void *s, void *)
{ return _ZN11BabyPenguin8BehaviorEv(s); }
static int __fastcall bp_render(void *s, void *)
{ port_actor_render_probe("BABY_PENGUIN", (char *)s + 0xd4);
  return _ZN11BabyPenguin6RenderEv(s); }
static int __fastcall bp_pdes(void *s, void *)
{ (void)s; _ZN11BabyPenguin16OnPendingDestroyEv(); return 0; }
/* D1: a real MSVC-synthesised empty destructor over a LOCAL shadow class
   (no vtable-store write) -- the MotherPenguin/OneUpLogo shape, not
   compiled. Chain spelled directly, HIGH ADDRESS FIRST (WithMeshClsn
   +0x194, MovingCylinderClsn +0x160, ShadowModel +0x138, ModelAnim
   +0xd4), then Actor's own D2. */
static int __fastcall bp_d1(void *s, void *)
{
    char *t = (char *)s;
    _ZN12WithMeshClsnD1Ev(t + 0x194);
    _ZN18MovingCylinderClsnD1Ev(t + 0x160);
    _ZN11ShadowModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall bp_d0(void *s, void *)
{ return (int)(size_t)_ZN11BabyPenguinD0Ev((int *)s); }
static int __fastcall bp_yoshi(void *s, void *)
{ (void)s; return func_ov072_02120cfc(); }
static int __fastcall bp_egg(void *s, void *)
{ (void)s; func_ov072_02121fa0(); return 0; }

extern "C" void hal_fill_baby_penguin_vtable(void)
{
    /* seat + verify the six PMF cells BEFORE InitResources can dispatch
       through them (func_ov072_02121d50/02121d18 dispatch the "enter"
       half from InitResources' own body). */
    port_baby_penguin_states_seat();
    void **vt = (void **)_ZTV11BabyPenguin;
    ov72_fill_shared(vt);
    vt[0]  = (void *)bp_init;
    vt[3]  = (void *)bp_clean;
    vt[6]  = (void *)bp_behavior;
    vt[9]  = (void *)bp_render;
    vt[12] = (void *)bp_pdes;
    vt[16] = (void *)bp_d1;
    vt[17] = (void *)bp_d0;
    vt[18] = (void *)bp_yoshi;   /* own OnYoshiTryEat, overrides the shared default */
    vt[19] = (void *)bp_egg;     /* own OnTurnIntoEgg, overrides the shared default */
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ---- method faces -----------------------------------------------------------
// _ZN11BabyPenguin13InitResourcesEv and _ZN11BabyPenguin8BehaviorEv are
// real MSVC methods against BabyPenguin.h (?InitResources@BabyPenguin@@...
// etc., not __ZN11BabyPenguin...) -- faced here, the IceSheet/OneUpLogo
// recipe. CleanupResources and OnPendingDestroy are plain C-linkage .c
// bodies taking (void) -- no face needed, declared extern "C" above and
// called directly.
// _ZN11BabyPenguin6RenderEv is NOT faced here: the matched Render TU
// dispatches ModelAnim slot 5 through a local six-virtual shadow (the ROM
// Render), which the host _ZTV9ModelAnim numbers as Virtual18. The TU is
// dropped from slice_gate193.txt and _ZN11BabyPenguin6RenderEv is the
// host copy in port/unmatched/ModelAnim_Renders.cpp, the Whomp/Butterfly/
// Fish/QuestionBlock case (see this file's own header).
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN11BabyPenguin13InitResourcesEv(void *self)
{ return ((BabyPenguin *)self)->BabyPenguin::InitResources(); }
#else
int _ZN11BabyPenguin13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN11BabyPenguin13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN11BabyPenguin8BehaviorEv(void *self)
{ return ((BabyPenguin *)self)->BabyPenguin::Behavior(); }
#else
int _ZN11BabyPenguin8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN11BabyPenguin8BehaviorEv */
#endif /* _WIN32 */
}
