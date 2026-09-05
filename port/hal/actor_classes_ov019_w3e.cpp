// RUN rel0215 WAVE 3 (lane w3-e): ov019's RACING PENGUIN (259), level 11's own
// overlay (Cool Cool Mountain's slide). Gate 142/143 seated ICE_SLIDE_MANAGER
// (356) out of this overlay and left everything else; this file seats the
// other class and closes ov019 to three named held-out TUs.
//
// ---- THIS IS LINKAGE, NOT CENSUS, AND SAYING SO IS THE POINT -------------
//
// 259 is on NO mounted level's object list. Level 11's baseline on cons
// cc10acd4e is 84 spawned, ZERO skipped, and nothing in ov019 calls
// Actor::Spawn(259) -- swept, not assumed. So this seat moves the linkage count
// and leaves the census exactly where it was. It is done anyway because both
// attribution routes name the class, its closure is complete, and an unseated
// class whose overlay IS mounted is a trap waiting for the first roster that
// reaches it (the ov022 VOLCANO_FIRE case in this same run: wave 8 read the
// placement list, missed the run-time caller, and left a live id declined).
//
// ---- THE ID, ALL THREE ROUTES --------------------------------------------
//
//   id   SpawnInfo   word[0] spawnFunc            idhalf
//   259  0x021132ec  0x021125bc RacingPenguin_Spawn  259
// Route 2, the Spawn's own vtable store: 0x02113310. Route 3, the RTTI at
// vtable[-1]: 0x0211330c -> 0x021132d0 -> "10daPgRcer_c" (penguin racer),
// base typeinfo arm9 0x0208e390. dsd's _ZTV10daPgRcer_c and _ZTV13RacingPenguin
// are two names for that one address and there is no identity shift.
//
// ---- THE WIDTH: THIRTY-ONE, AN ACTOR AND NOT A PLATFORM ------------------
//
// Read out of extracted/overlays/overlay_0019.bin at 0x02113310. The run ends
//   slot 29  0x0201012c    slot 30  0x020100dc Actor::OnAimedAtWithEggReturnVec
// and the next word is 0x0209a764, the FOLLOWING class's Itanium typeinfo
// record (IceSlideManager's), not a slot 31. Second pin: there is no
// 0x020ee55c Platform::Kill anywhere in the run, which is what every
// Platform-derived table in this port ends on. Same reading gate 143 recorded
// for IceSlideManager next door, and the same 31.
//
//   own slots  0 021123d4 InitResources   3 021122f8 CleanupResources
//              6 02112394 Behavior        9 02112360 Render
//             12 0211235c OnPendingDestroy -- an OVERRIDE, where every other
//                table in this file's family holds arm9's 0x02043ac0
//             16 021111a0 D1              17 021111f0 D0
//
// ---- THE STATE MACHINE: SIX STATES, TWELVE POINTER-TO-MEMBER RECORDS -----
//
// __sinit_ov019_021127a4 copies TWELVE 8-byte {fn, delta} pairs from .data
// (0x02113270..0x021132d0) into the .bss array at 0x0211356c, and
// func_ov019_021122dc sets the actor's +0x370 to `data_ov019_0211356c +
// (state << 4)` -- sixteen bytes per state, so SIX states of TWO records.
// func_ov019_021122a4 calls record 0 of the current state and
// func_ov019_02112268 calls record 1 (`c->pp + 1`).
//
// THE TWELVE FUNCTION WORDS ARE RE-SEATED AT THE SOURCE, NOT AT THE
// DESTINATION, and the choice is deliberate. Overwriting the bss copy after
// the sinit has run (the CrazedCrate recipe) reaches the same final state, but
// leaves twelve mounted .data words holding DS CODE ADDRESSES that no host TU
// names -- which is exactly what port/tools/ptr_audit.py reports, because it
// reads the emitted C rather than the run-time result. Seating the SOURCES
// before the sinit (the Painting recipe) answers the audit and means the DS
// addresses are never live. The ROM deltas are all zero and are written back
// as zero.
//
// Both dispatchers are compiled with /vmg /vmm (port/CMakeLists.txt), the R9
// wave-18 ruling: that pair of flags gives MSVC the ROM's own 8-byte
// {fn, delta} record, so `pp + 1` strides eight onto record 1's function word
// instead of onto the adj word of record 0. THE FLAGS FIX THE STRIDE AND NOT
// THE RECEIVER -- a pointer-to-member call is __thiscall -- so the twelve
// words are seated with __fastcall thunks onto the cdecl bodies.
//
// ---- THREE HELD-OUT TUs, EACH ON A RULING THE TREE ALREADY MADE ---------
//
// src/_ZN13RacingPenguinD1Ev.cpp (slot 16) is a real MSVC-synthesised
// destructor over a shadow class: MSVC mangles it ??1RacingPenguin@@UAE@XZ,
// never the Itanium name the slot needs. rp_d1 below spells the chain HIGH
// ADDRESS FIRST from that file's own member list, which is also the order its
// D0 runs: WithMeshClsn +0x1a8, MovingCylinderClsn +0x174, ShadowModel +0x14c,
// TextureSequence +0x138, ModelAnim +0xd4, then Actor::D2 and no Deallocate.
//
// src/_ZN13RacingPenguin6RenderEv.cpp (slot 9) dispatches its ModelAnim's ROM
// slot 5 through a six-virtual local shadow, which hal/cxxname_bridge.cpp says
// in writing its _ZTV9ModelAnim cannot serve. Host copy in
// port/unmatched/RacingPenguin_Render.cpp.
//
// src/_ZN15IceSlideManagerD1Ev.cpp is the same synthesised-destructor shape
// and stays where gate 143 left it (ism_d1 in hal/actor_classes_ccm.cpp).
//
// ---- LANE OWNERSHIP ------------------------------------------------------
//
// hal/actor_classes_ccm.cpp has no mount bring-up: gate 142's ov019 mount is
// three symbols and its one sinit runs from hal/actor_overlays.cpp. This lane
// does not touch that file either. The pack check and syms patch it needs are
// already run there; what is NOT is __sinit_ov019_021127a4, which gate 142
// deliberately left off ("RacingPenguin's, not spawned by level 11"). It runs
// here, from this fill, under a DSSTATE-bracketed guard for the reason
// hal/level_boot.cpp's g_level_mounted records: what the pass writes lives in
// .dsstate, so the guard has to roll back with it.

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
#include "dtor_faces_cpp.h"

#include "Actor.h"
#include "ActorBase.h"
#include "RacingPenguin.h"

extern "C" {
/* the arm9 shared half, read off this table's own reloc run */
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
void _ZN5ActorD2Ev(void *self);

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* the class's own bodies, in the linkage form src actually gives them --
   InitResources and Behavior are real C++ methods (RacingPenguin.h declares
   them), CleanupResources / OnPendingDestroy / D0 are extern "C", and
   CleanupResources and OnPendingDestroy take NO receiver on the ROM either. */
int _ZN13RacingPenguin16CleanupResourcesEv(void);       /* slot 3  */
int _ZN13RacingPenguin6RenderEv(char *self);            /* slot 9, host copy */
void _ZN13RacingPenguin16OnPendingDestroyEv(void);      /* slot 12 */
int *_ZN13RacingPenguinD0Ev(int *self);                 /* slot 17 */
void *RacingPenguin_Spawn(void);

/* the member destructors rp_d1 spells out */
void _ZN12WithMeshClsnD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);

/* this class's static initialiser, which gate 142 left unrun */
void __sinit_ov019_021127a4(void);

/* the twelve pointer-to-member state bodies, in the ROM's own pair order --
   read out of overlay_0019.bin at 0x02113270, deltas all zero */
int func_ov019_02111754(char *self);   /* pair 0x02113270 */
int func_ov019_02111f54(char *self);   /* pair 0x02113278 */
int func_ov019_02112168(char *self);   /* pair 0x02113280 */
int func_ov019_02111904(char *self);   /* pair 0x02113288 */
int func_ov019_02111dec(char *self);   /* pair 0x02113290 */
int func_ov019_021117a8(char *self);   /* pair 0x02113298 */
int func_ov019_0211197c(char *self);   /* pair 0x021132a0 */
int func_ov019_0211213c(char *self);   /* pair 0x021132a8 */
int func_ov019_02111fec(char *self);   /* pair 0x021132b0 */
int func_ov019_021121f8(char *self);   /* pair 0x021132b8 */
int func_ov019_02111d58(char *self);   /* pair 0x021132c0 */
int func_ov019_02111558(char *self);   /* pair 0x021132c8 */
/* the twelve .data source pairs themselves, named here so the fill can seat
   their function words -- and so ptr_audit sees a host TU that names them */
extern unsigned char data_ov019_02113270[];
extern unsigned char data_ov019_02113278[];
extern unsigned char data_ov019_02113280[];
extern unsigned char data_ov019_02113288[];
extern unsigned char data_ov019_02113290[];
extern unsigned char data_ov019_02113298[];
extern unsigned char data_ov019_021132a0[];
extern unsigned char data_ov019_021132a8[];
extern unsigned char data_ov019_021132b0[];
extern unsigned char data_ov019_021132b8[];
extern unsigned char data_ov019_021132c0[];
extern unsigned char data_ov019_021132c8[];

/* The table the ROM factory installs. `int` and C linkage to match the
   `extern int _ZTV13RacingPenguin[]` in decl_common.h that RacingPenguin_Spawn
   sees, exactly as hal/actor_classes_ccm.cpp declares IceSlideManager's. */
DSSTATE_BEGIN
int _ZTV13RacingPenguin[31];
DSSTATE_END
}

/* 0x02113310's other dsd spelling, which the deleting destructor restores the
   table by. Both names are excluded from the mount, so this LHS is undefined
   everywhere and the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:__ZTV10daPgRcer_c=__ZTV13RacingPenguin")
/* THE ONE CODE CROSS-NAME IN THIS OVERLAY, and it is an /alternatename rather
   than a -D for a measured reason. src/_ZN13RacingPenguin8BehaviorEv.cpp ends
   in `p__sinit_ov031_02111434(this)`, dsd's .ctor-entry spelling of an address
   ov019's own relocs resolve to itself:
     from:0x021123c4 kind:arm_call to:0x021114ec module:overlay(19)
   0x021114ec is func_ov019_021114ec, on this lane's slice. A per-source -D
   would be the campaign default, and here it does not compile: both names are
   declared in include/decl_common.h with DIFFERENT parameter types (line 2668
   `void func_ov019_021114ec(void*)`, line 2964
   `void p__sinit_ov031_02111434(char*)`), so the rewrite is a redefinition --
   the error C2371 shape the W4 block in port/CMakeLists.txt records. The alias
   is safe here for the reason a -D is normally preferred over one: nothing in
   this link DEFINES p__sinit_ov031_02111434. ov031's mount deliberately leaves
   its ".p__sinit_ov031_02111434" .ctor entry out (port/ov031_syms.txt:89),
   checked in the map and not assumed, so there is no defeat to worry about. */
#pragma comment(linker, "/alternatename:_p__sinit_ov031_02111434=_func_ov019_021114ec")

// ---- the trap --------------------------------------------------------------
static void rp_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov019 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov019 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define RP_TRAP(n) \
    static int __fastcall rp_trap##n(void *s, void *) \
    { rp_trap_report(s, n); return 0; }
RP_TRAP(13) RP_TRAP(14)
#undef RP_TRAP

static int __fastcall rp_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall rp_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall rp_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall rp_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall rp_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall rp_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall rp_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall rp_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall rp_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall rp_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall rp_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall rp_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall rp_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall rp_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall rp_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall rp_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall rp_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall rp_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall rp_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall rp_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall rp_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- the class's own slots -------------------------------------------------
static int __fastcall rp_init(void *s, void *)
{ return ((RacingPenguin *)s)->RacingPenguin::InitResources(); }
/* slot 3: the ROM body takes no receiver either -- it Releases twelve file
   pointers out of statics and returns 1 -- so the thunk drops `this`. */
static int __fastcall rp_clean(void *s, void *)
{ (void)s; return _ZN13RacingPenguin16CleanupResourcesEv(); }
static int __fastcall rp_behavior(void *s, void *)
{ return ((RacingPenguin *)s)->RacingPenguin::Behavior(); }
static int __fastcall rp_render(void *s, void *)
{ port_actor_render_probe("RACING_PENGUIN", (char *)s + 0xd4);
  return _ZN13RacingPenguin6RenderEv((char *)s); }
/* slot 12: an empty override, and empty on the ROM too (a bare `bx lr`). */
static int __fastcall rp_pdes(void *s, void *)
{ (void)s; _ZN13RacingPenguin16OnPendingDestroyEv(); return 0; }
/* slot 16, the held-out D1 -- see this file's header for why the matched TU
   cannot carry the Itanium name, and for where this chain comes from. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (rp_d1) spelled the same chain by hand. */
static int __fastcall rp_d0(void *s, void *)
{ return (int)(size_t)_ZN13RacingPenguinD0Ev((int *)s); }

// ---- the twelve state records ----------------------------------------------
#define RP_STATE(n, body) \
    static int __fastcall rp_state##n(void *s, void *) \
    { return body((char *)s); }
RP_STATE(0,  func_ov019_02111754)
RP_STATE(1,  func_ov019_02111f54)
RP_STATE(2,  func_ov019_02112168)
RP_STATE(3,  func_ov019_02111904)
RP_STATE(4,  func_ov019_02111dec)
RP_STATE(5,  func_ov019_021117a8)
RP_STATE(6,  func_ov019_0211197c)
RP_STATE(7,  func_ov019_0211213c)
RP_STATE(8,  func_ov019_02111fec)
RP_STATE(9,  func_ov019_021121f8)
RP_STATE(10, func_ov019_02111d58)
RP_STATE(11, func_ov019_02111558)
#undef RP_STATE

// ---- the bring-up ----------------------------------------------------------
DSSTATE_BEGIN
static int g_rp_bringup_done;
DSSTATE_END

static void rp_bringup(void)
{
    if (g_rp_bringup_done)
        return;
    g_rp_bringup_done = 1;
    /* SEAT THE SOURCES FIRST, then run the sinit that copies them. The
       destination array at 0x0211356c therefore never holds a DS address, and
       the twelve mounted .data pairs are named by this TU, which is what
       ptr_audit asks. Deltas are ROM-zero and are written back as zero. */
    {
        static unsigned char *const src[12] = {
            data_ov019_02113270, data_ov019_02113278, data_ov019_02113280,
            data_ov019_02113288, data_ov019_02113290, data_ov019_02113298,
            data_ov019_021132a0, data_ov019_021132a8, data_ov019_021132b0,
            data_ov019_021132b8, data_ov019_021132c0, data_ov019_021132c8,
        };
        static void *const seat[12] = {
            (void *)rp_state0, (void *)rp_state1, (void *)rp_state2,
            (void *)rp_state3, (void *)rp_state4, (void *)rp_state5,
            (void *)rp_state6, (void *)rp_state7, (void *)rp_state8,
            (void *)rp_state9, (void *)rp_state10, (void *)rp_state11,
        };
        int i;
        for (i = 0; i < 12; ++i) {
            *(void *volatile *)(src[i]) = seat[i];
            *(int volatile *)(src[i] + 4) = 0;
        }
    }
    __sinit_ov019_021127a4();
}

extern "C" void hal_fill_racing_penguin_vtable(void)
{
    rp_bringup();
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro and dumpbin proof in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV13RacingPenguin;
    vt[0]  = (void *)rp_init;
    vt[1]  = (void *)rp_binit;
    vt[2]  = (void *)rp_ainit;
    vt[3]  = (void *)rp_clean;
    vt[4]  = (void *)rp_bclean;
    vt[5]  = (void *)rp_aclean;
    vt[6]  = (void *)rp_behavior;
    vt[7]  = (void *)rp_bbeh;
    vt[8]  = (void *)rp_abeh;
    vt[9]  = (void *)rp_render;
    vt[10] = (void *)rp_bren;
    vt[11] = (void *)rp_aren;
    vt[12] = (void *)rp_pdes;
    vt[13] = (void *)rp_trap13;
    vt[14] = (void *)rp_trap14;
    vt[15] = (void *)rp_heap;
    vt[16] = (void *)hal_cppd1_RacingPenguin;
    vt[17] = (void *)rp_d0;
    vt[18] = (void *)rp_yoshi;
    vt[19] = (void *)rp_turn_egg;
    vt[20] = (void *)rp_v50;
    vt[21] = (void *)rp_pounded;
    vt[22] = (void *)rp_atk1;
    vt[23] = (void *)rp_atk2;
    vt[24] = (void *)rp_kicked;
    vt[25] = (void *)rp_pushed;
    vt[26] = (void *)rp_cannon;
    vt[27] = (void *)rp_mega;
    vt[28] = (void *)rp_under;
    vt[29] = (void *)rp_egg;
    /* Slot 30 declines: its ROM body returns a Vector3 by value and the sret
       contract is unproved -- the same reading gate 143 took next door. */
    vt[30] = (void *)port_actor_s30_base;
}
