// RUN REL0215 WAVE 2 (lane cast-sweep2): OV032'S THREE CLASSES (level 24,
// Tiny-Huge Island's BIG side). Overlay 1 of 4 in this lane.
//
// Level 24 spawns 83 actors (86 on the second census) and skips 7 across four
// ids on tip 3b4863d9a. FOUR of the seven are this overlay's own cast; the
// other three are id 232 (FlyGuy), whose both-routes owner is ov070.
//
//   id   name         x on L24  factory              table                width
//   106  HUGE_COVER    1        HugeCover_Spawn      0x021138e0 (unnamed)  32
//   107  HUGE_WATER    1        HugeWater_Spawn      0x021139a4            32
//   228  BUBBA         2        func_ov032_0211244c  0x02113824 (unnamed)  31
//
// ---- THE CAMPAIGN PLAN SAYS TWO CLASSES; THE ROM SAYS THREE ---------------
//
// PLAN.md's inventory row for ov032 reads "HugeCover, HugeWater", which is the
// set dsd named. The third class is the bulk of the overlay: id 228, factory
// func_ov032_0211244c, RTTI 12daBakubaku_c -- the big fish, which
// tools/actor_names.py already resolves to BUBBA. Both attribution routes put
// it here: its SpawnInfo record 0x02113800's word[0] relocates to
// module:overlay(32) func_ov032_0211244c, and word[1]'s low halfword reads 228,
// the id the arm9 spawn table slot 0x02090bf4 was reached from. Run over every
// candidate overlay of every id, each of 106 / 107 / 228 has EXACTLY ONE
// both-routes owner and it is ov032. (tools/actor_names.py's own report line
// "AMBIG actor 228 BUBBA: ['ov027','ov032','ov034'] -> ov027" picks ov027 by a
// tie-break; the two routes disagree with that tie-break.)
//
// ---- THE DSD VTABLE LABELS ARE ON THE WRONG CLASS -------------------------
//
// dsd names ONE vtable address here and gives it TWO names, both 0x021139a4:
// _ZTV9HugeCover and _ZTV14daObjTdWater_c. Three independent routes say that
// address is the WATER's and the cover's own table is unnamed:
//   the factories   HugeCover_Spawn (0x02112668) allocates 800 and installs
//                   0x021138e0; HugeWater_Spawn (0x021128b8) allocates 820,
//                   installs 0x021139a4 and constructs a TextureTransformer at
//                   this+0x320; func_ov032_0211244c allocates 1080, runs
//                   Enemy::Enemy and installs 0x02113824.
//   the RTTI        0x02113824 -> "12daBakubaku_c"
//                   0x021138e0 -> "13daObjTdFuta_c" (futa = lid/cover)
//                   0x021139a4 -> "14daObjTdWater_c"
//   the records     0x021138bc (id 106) -> HugeCover_Spawn
//                   0x02113980 (id 107) -> HugeWater_Spawn
//                   0x02113800 (id 228) -> func_ov032_0211244c
// So every _ZN9HugeCover* method body in src/ is HUGE_WATER's. The rows below
// are wired by address; the dsd spellings survive only as host array names,
// which is what lets the matched TUs link unchanged. Same shift ov033 carries
// one overlay along, and ov035 before it. Derivation: port/ov032_syms.txt.
//
// ---- WIDTHS: 31 / 32 / 32, AND EVERY DSD LANDING IS SHORT -----------------
//
// BUBBA is Enemy-derived, so its table is THIRTY-ONE slots and ends on slot 30
// (Actor::OnAimedAtWithEggReturnVec) with no Platform::Kill; the two Platforms
// are 32 and close on 0x020ee55c. dsd's next-symbol landings read TWO, FOUR and
// THREE. Every slot was verified against config/arm9/overlays/ov032/relocs.txt
// before a word was replaced.
//
// BUBBA carries EIGHT own slots, not six: 12 OnPendingDestroy is an EMPTY body
// (func_ov032_02112160) that overrides ActorBase's, and 29 OnAimedAtWithEgg is
// func_ov032_02112444 (`return 0xa0000`). Letting the shared half write the
// defaults into either would have changed the answer with no fault and no log
// line, so the fill writes both explicitly.
//
// ---- T1: ALL THREE TABLES STAY ROM-SHAPED ---------------------------------
//
// No class here derives from Model or ModelAnim -- the two Platforms HOLD a
// Model at +0xd4 (and id 107 a TextureTransformer at +0x320), and BUBBA holds a
// ModelAnim at +0x34c and a ShadowModel at +0x3b4 -- so the default applies:
// ROM-shaped, [16] D1, [17] D0. HUGE_COVER's Render is the plain-Model slot-5
// bare-call shadow over +0xd4 and rides FROM SRC (the Tree/ov013 case;
// _ZTV5Model[5] is dual-filled in hal/cxxname_bridge.cpp). BUBBA's Render is
// the ModelAnim slot-5 COLLISION and is the host copy in
// port/unmatched/Bubba_Render.cpp.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov033/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern, behind
// one DSSTATE-bracketed done-guard. It runs the pack check, the syms patch,
// hal_fill_platform_vtable (both Platform destructors install ov002 0x0210ae38
// as the base vptr on the way out), hal_fill_enemy_base_vtable (BUBBA's
// destructors tail into the Enemy base D2 func_ov002_020aed18) and then all
// THREE ov032 sinits in ROM order -- all of them; no class here is without
// reach.
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */
void hal_fill_enemy_base_vtable(void);            /* hal/actor_classes.cpp */

/* the generated ov032 per-symbol mount (build/port/host-src/ov032_syms.c) */
void port_ov032_pack_check(void);
void port_ov032_syms_patch(void);
/* all three sinits, in ROM order: BUBBA's files and its five state cells,
   then id 106's two files, then id 107's two. */
void __sinit_ov032_02112c10(void);
void __sinit_ov032_02112dbc(void);
void __sinit_ov032_02112e28(void);

/* ---- id 228 BUBBA -------------------------------------------------------
   dsd left this class unnamed, so all of it is func_ov032_*. Its Behavior,
   Render and state-enter setter are HOST COPIES (port/unmatched/Bubba_*.cpp);
   everything else rides from src. */
int func_ov032_021122dc(char *self);       /* slot 0,  InitResources */
int func_ov032_02112124(void);             /* slot 3,  CleanupResources */
int func_ov032_021121b4(char *self);       /* slot 6,  Behavior  HOST COPY */
int func_ov032_02112164(void *self);       /* slot 9,  Render    HOST COPY */
void func_ov032_02112160(void);            /* slot 12, OnPendingDestroy, empty */
int *func_ov032_021111a0(int *self);       /* slot 16, D1 */
int *func_ov032_021111f0(int *self);       /* slot 17, D0 */
int func_ov032_02112444(void);             /* slot 29, OnAimedAtWithEgg */
int *func_ov032_0211244c(void);            /* id 228's factory */

/* BUBBA's ten state handlers, five {enter, tick} pairs */
int func_ov032_02111f9c(char *c);
int func_ov032_02111e24(char *c);
int func_ov032_02111dd8(char *c);
int func_ov032_02111d7c(void *c);
int func_ov032_02111d58(char *c);
int func_ov032_02111b9c(unsigned char *c);
int func_ov032_02111b50(char *c);
int func_ov032_02111830(char *c);
int func_ov032_02111814(char *c);
int func_ov032_02111620(void *c);

/* ---- id 106 HUGE_COVER --------------------------------------------------
   dsd left this class unnamed too, so its six lifecycle slots are all
   func_ov032_*. Their "recovered from vtable slot identity" markers are NAME
   recoveries over real decompiled bodies (T5, adjudicated in
   port/tools/inferred_stub_adjudicated.txt); none is a stub. */
int func_ov032_021125d4(char *self);       /* slot 0,  InitResources */
int func_ov032_02112544(void *self);       /* slot 3,  CleanupResources */
int func_ov032_021125b0(void *self);       /* slot 6,  Behavior */
int func_ov032_02112588(void *self);       /* slot 9,  Render */
int *func_ov032_021124a8(int *self);       /* slot 16, D1 */
int *func_ov032_021124ec(int *self);       /* slot 17, D0 */
int *HugeCover_Spawn(void);                /* id 106 */

/* ---- id 107 HUGE_WATER --------------------------------------------------
   these are the ones dsd spelled _ZN9HugeCover*, and they belong to the WATER
   class. The four method bodies are real MSVC members against
   include/HugeCover.h and are faced below. */
int *_ZN9HugeCoverD1Ev(int *self);         /* slot 16 */
int *_ZN9HugeCoverD0Ev(int *self);         /* slot 17 */
int *HugeWater_Spawn(void);                /* id 107 */

/* the three host vtables, all excluded from the mount. The names are dsd's for
   the addresses, NOT for the classes -- see this file's header. */
DSSTATE_BEGIN
int data_ov032_02113824[31];   /* 0x02113824, id 228 BUBBA */
int data_ov032_021138e0[32];   /* 0x021138e0, id 106 HUGE_COVER */
void *_ZTV9HugeCover[32];      /* 0x021139a4, id 107 HUGE_WATER */
DSSTATE_END
}

/* The two RTTI spellings src/ uses for tables dsd named differently or not at
   all. Both LHS are DECLARED in include/decl_common.h (lines 541 and 573) and
   DEFINED nowhere else in the link, so neither alias can be defeated and
   alternatename_guard stays clean.
     _ZTV13daObjTdFuta_c   src/func_ov032_021124a8.c, id 106's D1
     _ZTV14daObjTdWater_c  src/_ZN9HugeCoverD1Ev.c and D0Ev.c, id 107's pair --
                           the SAME address src/HugeWater_Spawn.c spells
                           _ZTV9HugeCover, which is the storage below. */
#pragma comment(linker, "/alternatename:__ZTV13daObjTdFuta_c=_data_ov032_021138e0")
#pragma comment(linker, "/alternatename:__ZTV14daObjTdWater_c=__ZTV9HugeCover")

/* THE C++-LINKAGE DATA SPELLINGS. Six of this overlay's TUs are //cpp files
   whose `extern <type> data_ovNNN_...;` declarations sit at file scope OUTSIDE
   decl_common.h's `extern "C"` block, so MSVC mangles each one with the type
   that TU chose. The mount emits ONE C-named array per symbol, so every
   mangled spelling is bound onto it -- the ov030 / ov065 / ov100pl /
   scuttlebug recipe (hal/actor_classes_ov030.cpp:290). Two TUs spell
   data_ov032_02113afc with DIFFERENT types (int* and struct D113afc), which is
   why it appears twice; both are the same eight bytes. Every LHS here is a
   mangled name defined nowhere in the link, so no alias can be defeated and
   alternatename_guard stays clean. Read off the linker's own LNK2019 lines
   rather than derived by hand. */
#pragma comment(linker, "/alternatename:?data_ov032_02113abc@@3PAHA=_data_ov032_02113abc")
#pragma comment(linker, "/alternatename:?data_ov032_02113a7c@@3PAHA=_data_ov032_02113a7c")
#pragma comment(linker, "/alternatename:?data_ov032_02113a50@@3PAPAXA=_data_ov032_02113a50")
#pragma comment(linker, "/alternatename:?data_ov032_02113a8c@@3PAXA=_data_ov032_02113a8c")
#pragma comment(linker, "/alternatename:?data_ov032_02113afc@@3PAHA=_data_ov032_02113afc")
#pragma comment(linker, "/alternatename:?data_ov032_02113afc@@3UD113afc@@A=_data_ov032_02113afc")
#pragma comment(linker, "/alternatename:?data_ov032_02113af4@@3USharedFilePtr@@A=_data_ov032_02113af4")
#pragma comment(linker, "/alternatename:?data_ov032_02112f64@@3UBTA_File@@A=_data_ov032_02112f64")
#pragma comment(linker, "/alternatename:?data_ov032_02112fb8@@3UCLPS_Block@@A=_data_ov032_02112fb8")
/* data_020a0e68 is arm9's scratch Matrix4x3, already bound under six other
   mangled spellings across the port (cxx_aliases.cpp:1796/1805,
   actor_vtables.cpp:243, actor_classes_scuttlebug.cpp:151). BUBBA's
   func_ov032_02111620 spells it `extern int`, a seventh. */
#pragma comment(linker, "/alternatename:?data_020a0e68@@3HA=_data_020a0e68")

/* The four bodies src defines as real C++ methods against include/HugeCover.h,
   faced here -- the ov013/ov024/ov025/ov033/ov035 recipe. All four serve id
   107, not id 106. */
#include "HugeCover.h"
extern "C" {
int _ZN9HugeCover13InitResourcesEv(void *self)
{ return ((HugeCover *)self)->HugeCover::InitResources(); }
int _ZN9HugeCover16CleanupResourcesEv(void *self)
{ return ((HugeCover *)self)->HugeCover::CleanupResources(); }
int _ZN9HugeCover6RenderEv(void *self)
{ return ((HugeCover *)self)->HugeCover::Render(); }
int _ZN9HugeCover8BehaviorEv(void *self)
{ return ((HugeCover *)self)->HugeCover::Behavior(); }
}

// ---- the trap --------------------------------------------------------------
static void ov32_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov032 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov032 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV32_TRAP(n) \
    static int __fastcall ov32_trap##n(void *s, void *) \
    { ov32_trap_report(s, n); return 0; }
OV32_TRAP(13) OV32_TRAP(14)
#undef OV32_TRAP

static int __fastcall ov32_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov32_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov32_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov32_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov32_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov32_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov32_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov32_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov32_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov32_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov32_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov32_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov32_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov32_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov32_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov32_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov32_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov32_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov32_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov32_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov32_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov32_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov32_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half of all three tables. Each caller writes its own 0/3/6/9/16/17
   afterwards, BUBBA also writes 12 and 29, and the two Platforms also write 31.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   in hal/actor_classes_ov002g200.cpp. */
static void ov32_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov32_binit;
    vt[2]  = (void *)ov32_ainit;
    vt[4]  = (void *)ov32_bclean;
    vt[5]  = (void *)ov32_aclean;
    vt[7]  = (void *)ov32_bbeh;
    vt[8]  = (void *)ov32_abeh;
    vt[10] = (void *)ov32_bren;
    vt[11] = (void *)ov32_aren;
    vt[12] = (void *)ov32_pdes;
    vt[13] = (void *)ov32_trap13;
    vt[14] = (void *)ov32_trap14;
    vt[15] = (void *)ov32_heap;
    vt[18] = (void *)ov32_yoshi;
    vt[19] = (void *)ov32_turn_egg;
    vt[20] = (void *)ov32_v50;
    vt[21] = (void *)ov32_pounded;
    vt[22] = (void *)ov32_atk1;
    vt[23] = (void *)ov32_atk2;
    vt[24] = (void *)ov32_kicked;
    vt[25] = (void *)ov32_pushed;
    vt[26] = (void *)ov32_cannon;
    vt[27] = (void *)ov32_mega;
    vt[28] = (void *)ov32_under;
    vt[29] = (void *)ov32_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// BUBBA'S FIVE-CELL PMF STATE SEAT
// ============================================================================
//
// __sinit_ov032_02112c10 copies ten 8-byte mwcc {function, delta} pairs out of
// .data into five 16-byte .bss cells, {lo = enter, hi = tick}. The pairs and the
// cells are all in this lane's mount (port/ov032_syms.txt), so the sinit's own
// bytes are the ROM's own bytes and the DS addresses below are what it writes.
//
// The rewrite is the Ukiki (ov030) / BabyPenguin (ov072) / MrBlizzard shape: the
// ROM column is VERIFIED against the mounted bytes first, so a mount pointing at
// the wrong bytes ABORTS instead of silently calling into DS memory. Order is
// the sinit's own cell order, not sorted by address.
extern "C" {
struct PortBubbaCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortBubbaCell data_ov032_02113a8c;   /* cell 0 */
extern PortBubbaCell data_ov032_02113a9c;   /* cell 1 */
extern PortBubbaCell data_ov032_02113aac;   /* cell 2 */
extern PortBubbaCell data_ov032_02113abc;   /* cell 3 */
extern PortBubbaCell data_ov032_02113a7c;   /* cell 4 */
}

typedef int (*PortBubbaFn)(void *);

static const struct {
    PortBubbaCell *cell;
    unsigned enter_rom, tick_rom;
    PortBubbaFn enter_host, tick_host;
} g_bubba_cells[5] = {
    { &data_ov032_02113a8c, 0x02111f9c, 0x02111e24,
      (PortBubbaFn)func_ov032_02111f9c, (PortBubbaFn)func_ov032_02111e24 },
    { &data_ov032_02113a9c, 0x02111dd8, 0x02111d7c,
      (PortBubbaFn)func_ov032_02111dd8, (PortBubbaFn)func_ov032_02111d7c },
    { &data_ov032_02113aac, 0x02111d58, 0x02111b9c,
      (PortBubbaFn)func_ov032_02111d58, (PortBubbaFn)func_ov032_02111b9c },
    { &data_ov032_02113abc, 0x02111b50, 0x02111830,
      (PortBubbaFn)func_ov032_02111b50, (PortBubbaFn)func_ov032_02111830 },
    { &data_ov032_02113a7c, 0x02111814, 0x02111620,
      (PortBubbaFn)func_ov032_02111814, (PortBubbaFn)func_ov032_02111620 },
};

extern "C" void port_bubba_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 5; ++i) {
        PortBubbaCell &cell = *g_bubba_cells[i].cell;
        if (cell.enter_fn != g_bubba_cells[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != g_bubba_cells[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: BUBBA state cell %d: the sinit left "
                         "%08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell.enter_fn, cell.enter_delta,
                         cell.tick_fn, cell.tick_delta,
                         g_bubba_cells[i].enter_rom, g_bubba_cells[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)g_bubba_cells[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)g_bubba_cells[i].tick_host;
    }
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov32_bringup_done;
DSSTATE_END

extern "C" void port_ov32_bringup(void)
{
    if (g_ov32_bringup_done)
        return;
    g_ov32_bringup_done = 1;
    port_ov032_pack_check();
    port_ov032_syms_patch();
    hal_fill_platform_vtable();
    hal_fill_enemy_base_vtable();
    __sinit_ov032_02112c10();
    __sinit_ov032_02112dbc();
    __sinit_ov032_02112e28();
    /* Seat and verify the five PMF cells BEFORE anything can dispatch through
       them: func_ov032_021122dc (BUBBA's InitResources) installs cell 0 through
       func_ov032_02111ff4, which tail-calls its enter half on the same frame. */
    port_bubba_states_seat();
}

// ============================================================================
// BUBBA (228) -- table 0x02113824, RTTI 12daBakubaku_c, 31 slots.
// ============================================================================
//
// The big fish in the huge island's pond. A 1080-byte Enemy: two
// MovingCylinderClsnWithPos at +0x110 and +0x150, a WithMeshClsn at +0x190, a
// ModelAnim at +0x34c, a ShadowModel at +0x3b4 and the state cell pointer at
// +0x3b0. InitResources loads model 661 and animations 662/663, sizes both
// cylinders out of the two Vector3s at 0x021137cc / 0x021137d8, and enters
// state cell 0x02113a8c.
static int __fastcall bb_init(void *s, void *)
{ return func_ov032_021122dc((char *)s); }
static int __fastcall bb_clean(void *s, void *)
{ (void)s; return func_ov032_02112124(); }
static int __fastcall bb_behavior(void *s, void *)
{ return func_ov032_021121b4((char *)s); }        /* HOST COPY */
static int __fastcall bb_render(void *s, void *)
{ port_actor_render_probe("BUBBA", (char *)s + 0x34c);
  return func_ov032_02112164(s); }                /* HOST COPY */
/* Slot 12, the OVERRIDE: an empty body where ActorBase does real work. */
static int __fastcall bb_pdes(void *s, void *)
{ (void)s; func_ov032_02112160(); return 0; }
static int __fastcall bb_d1(void *s, void *)
{ return (int)(size_t)func_ov032_021111a0((int *)s); }
static int __fastcall bb_d0(void *s, void *)
{ return (int)(size_t)func_ov032_021111f0((int *)s); }
/* Slot 29, the OVERRIDE: returns 0xa0000 where Actor returns its own default. */
static int __fastcall bb_aimed(void *s, void *)
{ (void)s; return func_ov032_02112444(); }

extern "C" void hal_fill_bubba_vtable(void)
{
    port_ov32_bringup();
    void *volatile *vt = (void *volatile *)data_ov032_02113824;
    ov32_fill_shared(vt);
    vt[0]  = (void *)bb_init;
    vt[3]  = (void *)bb_clean;
    vt[6]  = (void *)bb_behavior;
    vt[9]  = (void *)bb_render;
    /* AFTER the shared fill, which writes the ActorBase/Actor defaults here. */
    vt[12] = (void *)bb_pdes;
    vt[16] = (void *)bb_d1;
    vt[17] = (void *)bb_d0;
    vt[29] = (void *)bb_aimed;
    /* no slot 31: an Enemy is a plain Actor, 31 slots total, ends at 30. */
}

// ============================================================================
// HUGE_COVER (106) -- table 0x021138e0, RTTI 13daObjTdFuta_c, 32 slots.
// ============================================================================
//
// The lid over the huge island's well, the big-side twin of ov033's TINY_COVER.
// An 800-byte Platform: Model at +0xd4, MovingMeshCollider at +0x124, collider
// transform source at +0x2ec. InitResources loads model 1717 and KCL 1718,
// hands the collider CLPS 0x02112f98 at fix 0x199, and RETURNS
// Event::GetBit(0xe) == 0 -- the same event bit ov033's pair shares, so once the
// well is open the class declines to initialise.
static int __fastcall hc_init(void *s, void *)
{ return func_ov032_021125d4((char *)s); }
static int __fastcall hc_clean(void *s, void *)
{ return func_ov032_02112544(s); }
static int __fastcall hc_behavior(void *s, void *)
{ return func_ov032_021125b0(s); }
static int __fastcall hc_render(void *s, void *)
{ port_actor_render_probe("HUGE_COVER", (char *)s + 0xd4);
  return func_ov032_02112588(s); }
static int __fastcall hc_d1(void *s, void *)
{ return (int)(size_t)func_ov032_021124a8((int *)s); }
static int __fastcall hc_d0(void *s, void *)
{ return (int)(size_t)func_ov032_021124ec((int *)s); }

extern "C" void hal_fill_huge_cover_vtable(void)
{
    port_ov32_bringup();
    void *volatile *vt = (void *volatile *)data_ov032_021138e0;
    ov32_fill_shared(vt);
    vt[0]  = (void *)hc_init;
    vt[3]  = (void *)hc_clean;
    vt[6]  = (void *)hc_behavior;
    vt[9]  = (void *)hc_render;
    vt[16] = (void *)hc_d1;
    vt[17] = (void *)hc_d0;
    vt[31] = (void *)ov32_kill;
}

// ============================================================================
// HUGE_WATER (107) -- table 0x021139a4 (dsd _ZTV9HugeCover AND
// _ZTV14daObjTdWater_c), RTTI 14daObjTdWater_c, 32 slots.
// ============================================================================
//
// The huge island's water surface. An 820-byte Platform: Model at +0xd4,
// MovingMeshCollider at +0x124, and a TextureTransformer at +0x320 its factory
// constructs and both destructors tear down FIRST. InitResources loads model
// 1719 with LOD 0x14, prepares the scrolling BTA out of the six-word file
// descriptor 0x02112f64, loads KCL 1720, hands the collider CLPS 0x02112fb8 and
// enables it against itself.
static int __fastcall hw_init(void *s, void *)
{ return _ZN9HugeCover13InitResourcesEv(s); }
static int __fastcall hw_clean(void *s, void *)
{ return _ZN9HugeCover16CleanupResourcesEv(s); }
static int __fastcall hw_behavior(void *s, void *)
{ return _ZN9HugeCover8BehaviorEv(s); }
static int __fastcall hw_render(void *s, void *)
{ port_actor_render_probe("HUGE_WATER", (char *)s + 0xd4);
  return _ZN9HugeCover6RenderEv(s); }
static int __fastcall hw_d1(void *s, void *)
{ return (int)(size_t)_ZN9HugeCoverD1Ev((int *)s); }
static int __fastcall hw_d0(void *s, void *)
{ return (int)(size_t)_ZN9HugeCoverD0Ev((int *)s); }

extern "C" void hal_fill_huge_water_vtable(void)
{
    port_ov32_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9HugeCover;
    ov32_fill_shared(vt);
    vt[0]  = (void *)hw_init;
    vt[3]  = (void *)hw_clean;
    vt[6]  = (void *)hw_behavior;
    vt[9]  = (void *)hw_render;
    vt[16] = (void *)hw_d1;
    vt[17] = (void *)hw_d0;
    vt[31] = (void *)ov32_kill;
}
