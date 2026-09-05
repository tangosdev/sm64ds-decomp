// RUN LINKW WAVE 6 (lane w6-D): PATH_LIFT (31) AND THE PathLift BASE BRANCH.
//
// The first class in this port with a real THREE-LEVEL hierarchy that is
// SPLIT ACROSS OVERLAYS: the leaf lives in ov100, its base lives in ov002,
// and the base adds a virtual of its own on top of Platform's shape. Every
// row in the map below was re-derived from raw images, relocs and RTTI in
// this lane; nothing is carried. Same law as hal/actor_classes_ov045.cpp
// (the wave-4 exemplar) and hal/actor_classes_ov065.cpp (wave 5).
//
// ---- THE HIERARCHY MAP -----------------------------------------------------
//
//   ActorBase -> Actor -> Platform          _ZTV8Platform    0x0210ae38, 32
//                            |               (ov002; already a host array in
//                            |                hal/actor_vtables.cpp)
//                            v
//                     PathLiftActor_c       data_ov002_0210af70, 33 slots
//                            |               (ov002; RTTI name string
//                            |                0x0210af18 "16dPathLiftActor_c",
//                            |                i.e. the class is dPathLiftActor_c
//                            |                -- the 16 is the Itanium length
//                            |                prefix and dPathLiftActor_c is
//                            |                exactly 16 characters, which is
//                            |                the check the lane's
//                            |                "16PathLiftActor_c" reading fails
//                            |                (that name is 15). Config PRs
//                            |                would inherit the wrong class
//                            |                name from the old spelling,
//                            |                typeinfo record 0x0210af0c whose
//                            |                base word is Platform's
//                            |                0x021089ec; vtable[-1] at
//                            |                0x0210af6c -> that record)
//                            v
//                     daObjPathLift_c       data_ov100_0214857c, 33 slots
//                                            (ov100; typeinfo 0x02148538 =
//                                             {0x0209a764, name 0x02148544
//                                             "15daObjPathLift_c", BASE
//                                             0x0210af0c -- the RTTI chain
//                                             PROVES the derivation);
//                                             vtable[-1] 0x02148578)
//
// PathLiftActor_c EXTENDS Platform's table by exactly one slot: slot 32 is
// _ZN8PathLift9AfterClsnEv (0x020eff18). daObjPathLift_c inherits that slot
// UNCHANGED -- the word at 0x021485fc is a reloc into ov002, not ov100. That
// is the "cross-class inherited slot" this lane was sent to find, and a fill
// that only wrote ov100's own 0/3/6/9/16/17 would leave it null.
//
// ---- BOTH WIDTHS ARE 33, EACH PINNED TWICE ---------------------------------
//   data_ov002_0210af70  last reloc 0x0210aff0 (slot 32); next dsd symbol
//                        data_ov002_0210aff4 = base + 0x84 = 33 words exactly
//   data_ov100_0214857c  last reloc 0x021485fc (slot 32); next dsd symbol
//                        data_ov100_02148600 (bss) = base + 0x84, 33 exactly
// The wf header's lesson ("31 because Platform WAS THE BUG") one slot further
// out: a [32] array here would read past the end on every AfterClsn.
//
// ---- WHO OVERRIDES WHAT (both tables read off their own reloc runs) --------
//   slot  PathLiftActor_c (ov002)          daObjPathLift_c (ov100)
//    0    ActorBase::InitResources         func_ov100_021471e0
//    3    ActorBase::CleanupResources      func_ov100_02147054
//    6    ActorBase::Behavior              func_ov100_021470f4
//    9    ActorBase::Render                func_ov100_021470a4
//   16    _ZN8PathLiftD2Ev  0x020ef390     func_ov100_02146d7c
//   17    _ZN8PathLiftD0Ev  0x020ef320     func_ov100_02146dec
//   31    Platform::Kill    0x020ee55c     Platform::Kill (same word)
//   32    PathLift::AfterClsn 0x020eff18   0x020eff18 (INHERITED, same word)
// Everything else in both tables is the shared Actor/ActorBase half, word for
// word identical between the two.
//
// PathLiftActor_c is a PURE INTERMEDIATE. Its table is stored by exactly EIGHT
// sites in the whole ROM (config relocs, `to:0x0210af70` -- counted again at
// the wave-6 close, where the lane's "six" was an arithmetic slip against its
// own list): its own two destructors (ov002 0x020ef380, 0x020ef3e0), ov100's
// three (0x02146de0 D1, 0x02146e60 D0, 0x02147394 the factory) and ov036's
// three (0x021121bc, 0x0211223c, 0x021125a0) -- and
// NO SpawnInfo anywhere names it. Nothing is ever live as one: at all eight
// sites the vptr passes through the base table inside a window with no
// virtual dispatch on `this` (the factory's window runs the Model[3] array
// ctor and PathPtrC1, both on interior addresses; the destructors' window
// runs __destroy_arr with an explicit function pointer). Its slots 0/3/6/9
// -- the four ActorBase defaults an abstract intermediate never uses -- trap
// here rather than pull four unreferenced engine TUs.
//
// ---- THE FACTORY IS BASE-THEN-OWN, AND THE RECOVERED SOURCE SPELLS IT RIGHT
// PathLift_Spawn 0x02147328 (0x7c): ActorBase::new(0x4b4), Platform::Platform,
// vptr = data_ov002_0210af70 (BASE, transient), Model[3] @ +0x320 stride 0x50,
// PathPtr @ +0x430, vptr = data_ov100_0214857c (OWN, FINAL),
// ShadowModel @ +0x450. The FINAL store lands on the leaf table by a real
// name, so unlike the ov045 factory-F / ShutterHmc shape there is NO factory
// wrapper here and no p[0] reseat. Registering the class is enough.
//
// ---- THE STATE MACHINE: THREE STATES, TWO PMF HALVES, SEATED ---------------
// The base drives WAIT/PATH/FALL through data_ov002_0210af2c: three 20-byte
// records of { Pair enter; Pair tick; const char *name; } where Pair is the
// standard 8-byte mwcc {fn, delta<<1|isVirtual}. The record width is not
// inferred -- both dispatchers literally `mov r0, #0x14`. The names are ROM
// strings: 0x0210aec0 "WAIT", 0x0210aec8 "PATH", 0x0210aeb8 "FALL".
// __sinit_ov002_021071f4 (matched, ALREADY linked via slice_gate10.txt:432)
// copies six source Pairs into it, so the table holds DS CODE ADDRESSES at
// runtime today. port_pathlift_states_seat rewrites the six words with the
// host bodies, verify-then-rewrite, before any fill can dispatch them --
// the gate-200 PushBlock order exactly.
//
// ---- THE TRAP THAT WOULD HAVE LINKED ANOTHER OVERLAY'S CODE ----------------
// src/_ZN8PathLift17BaseInitResourcesEv.c is NOT this class's base init. That
// name is an OV006 export (overlays/ov006/symbols.txt:887, size 0x118) and
// ov006 shares the 0x020efaf0 window with ov002, whose body there is
// func_ov002_020efaf0 (size 0xec -- the sizes alone settle that they are two
// different bodies). ov100's Init calls the address through a reloc dsd
// annotates `module:overlays(2,6)`; ov002 is the overlay resident with ov100,
// so the body is ov002's. Slicing the named TU would have handed every
// PathLift ov006's MgBounceAndPounce constructor. The w5-B EyerokD0 class,
// caught by re-deriving instead of trusting the name.
//
// ---- REACH, MEASURED (baseline censuses at b8ff3d0b7, FAULTS_FATAL) --------
//   level 13 HMC   31 x1  (with 262 x1, 318 x3 -> 183 spawned / 5 skipped)
//   level 15 LLL   31 x2  (with 84 x1, 318 x3  ->  50 spawned / 6 skipped)
// No other mounted level places id 31. The rest of the family is PARKED with
// its evidence, per the ov019 rule (a registry row for a class no bootable
// level spawns is the measured gate-6 mistake):
//   id 175 SQUARE_PATH_LIFT      ov052, NOT mounted. PathLift by NAME ONLY:
//          zero relocs from ov052 into any base body, 32-slot own table
//          (_ZTV14SquarePathLift 0x021125f0, slot 32 zero in ROM), derives
//          Platform directly. Would need NO base machinery.
//   ids 123/125/126/127/129/130/132 (ov036, NOT mounted). FLYING_CARPET 130
//          IS a real derived class: own 33-slot table 0x02113f9c, slot 32
//          reloc-pinned to the same 0x020eff18, and its Behavior/Init call
//          BaseBehavior/the base init. It is the second customer for
//          everything this lane lands.
//   ids 144/157 (ov091, MOUNTED, _ZTV13daLinelift2_c) are NOT
//          PathLift-derived: 32-slot table, no base-body relocs. This lane's
//          finding still holds and was re-derived on the ROM by run rel0215
//          wave 3 lane w3-f2, which SEATED both ids -- so "144 is in level
//          37's skip residue" is retired (level 37's skip list is empty now)
//          and the fill is not gate 73's: the two share a class of their own,
//          _ZTV22RotatingUpDownPlatform at ov091 0x02134ec0, which is a
//          DIFFERENT table from gate 73's Utm one at 0x02134c5c. See
//          hal/actor_classes_ov091.cpp.
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

#include "Actor.h"
#include "ActorBase.h"
#include "PathLift.h"

extern "C" {
/* the arm9 shared half; every address read off this lane's two reloc runs */
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
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26  */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27  */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28  */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* 29  */
void _ZN8Platform4KillEv(void *self);                              /* 31  */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);

/* the state seat (port/unmatched/PathLift_StateDispatch.cpp) */
void port_pathlift_states_seat(void);

/* ---- the branch's own bodies (port/slice_w6d.txt) ---- */
/* ov002, the base */
int _ZN8PathLiftD2Ev(void *self);                  /* base slot 16 */
int _ZN8PathLiftD0Ev(void *self);                  /* base slot 17 */
void func_ov002_020efa54(void *self, int state);   /* HOST COPY, installer */
void _ZN8PathLift12BaseBehaviorEv(void *self);     /* HOST COPY, tick      */
void func_ov002_020efaf0(void *self);              /* the REAL base init   */
void func_ov002_020efc74(void *self);              /* Render's base half   */
void func_ov002_020efcf4(void *self);              /* Behavior's base half */
/* ov100, the leaf */
int func_ov100_021471e0(void *self);               /* slot 0  */
int func_ov100_02147054(void *self);               /* slot 3  */
int func_ov100_021470f4(void *self);               /* slot 6  */
int func_ov100_021470a4(void *self);               /* slot 9  */
int func_ov100_02146d7c(void *self);               /* slot 16 */
void *func_ov100_02146dec(void *self);             /* slot 17 */
void *PathLift_Spawn(void);

/* THE TWO TABLES ARE HOST STORAGE. data_ov100_0214857c is already excluded
   from ov100's per-symbol mount BY ADDRESS (port/ov100_syms.txt's header
   explains why the "_ZTV*" rule could not catch it); data_ov002_0210af70 was
   never in ov002_syms.txt at all, so hosting it here is the whole change on
   the ov002 side. */
DSSTATE_BEGIN
void *data_ov002_0210af70[33];
void *data_ov100_0214857c[33];
DSSTATE_END
}

/* Both tables are named by TU declarations that sit OUTSIDE extern "C" --
   src/PathLift_Spawn.cpp spells `extern void* data_ov002_0210af70;` and
   `extern void* data_ov100_0214857c;` as plain C++ objects and takes their
   ADDRESS -- so MSVC mangles the type into the name. Bind the decorated
   spellings onto the one C-named array each (the ?data_020a0e68@@3UMtx43@@A
   precedent; both are free-standing data, so the alias is legal and the
   alternatename guard has a real definition on the RHS). */
#pragma comment(linker, "/alternatename:?data_ov002_0210af70@@3PAXA=_data_ov002_0210af70")
#pragma comment(linker, "/alternatename:?data_ov100_0214857c@@3PAXA=_data_ov100_0214857c")
/* ov100's Init declares the base init as plain `func_020efaf0` (no ov002
   prefix) inside extern "C". Nothing else in the game defines that name --
   it is the recovered source's own shorthand, not a window race -- so a
   C-name alias onto the ov002 body is the right bridge, and it is a free
   function (@@YA-class) so the alias is legal. */
#pragma comment(linker, "/alternatename:_func_020efaf0=_func_ov002_020efaf0")
/* Two more decorated-data spellings, same rule: the recovered TUs declare
   mounted storage outside extern "C" and MSVC mangles the type in. Both
   addresses are ov002's own mount rows (data_ov002_0210d9f0 has been mounted
   since gate 40; data_ov002_0210af00 is one of this lane's three new rows). */
#pragma comment(linker, "/alternatename:?data_ov002_0210d9f0@@3HA=_data_ov002_0210d9f0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9f0@@3UPathStuff@@A=_data_ov002_0210d9f0")
#pragma comment(linker, "/alternatename:?data_ov002_0210af00@@3UVector3@@A=_data_ov002_0210af00")
/* Vec3_Dist is a FREE function (@@YA), so the one-liner alias is legal here
   -- the U10 rule. The recovered Behavior spells its arguments const. */
#pragma comment(linker, "/alternatename:?Vec3_Dist@@YAHPBUVector3@@0@Z=_Vec3_Dist")
/* MeshColliderBase::Enable with a void return over void*; the linked body is
   the int-return / Actor* spelling. Same __thiscall body, eax ignored -- the
   GetFloorResult precedent, and the same alias hal/actor_classes_ov065.cpp
   already writes for Dorrie's Init with a different argument spelling. */
#pragma comment(linker, "/alternatename:?Enable@MeshColliderBase@@QAEXPAX@Z=?Enable@MeshColliderBase@@QAEHPAUActor@@@Z")
/* THE THREE SHARED-HEADER ROLE NAMES, the CannonHatch disease's third case.
   src/func_ov100_02147054.c releases its SharedFilePtrs as G0/G1/G2, and
   _G0/_G1 are single global names hal/cxx_aliases.cpp has already bound to
   the GAME HEAP pointer and to SignPost's KCL -- two of the three would have
   linked SILENTLY onto the wrong storage, only G2 errored. The CMake block
   beside the w5-B R1/R2 renames redirects that one TU onto the three private
   names below; each binds here onto the object the ROM's own literal pool
   names, in release order. */
#pragma comment(linker, "/alternatename:_port_pathlift_file0=_data_ov002_0210d9f0")
#pragma comment(linker, "/alternatename:_port_pathlift_file1=_data_ov100_02148a54")
#pragma comment(linker, "/alternatename:_port_pathlift_file2=_data_ov100_02148a5c")

// ---- three faces, all __thiscall onto @@YA bodies --------------------------
// NOT alias-eligible: hal/cxx_aliases.cpp:1059-1060 already establishes that
// Platform::UpdateClsnPosAndRot and Platform::IsClsnInRange link as CDECL
// free functions taking `self` explicitly, and PathLift::BaseBehavior is this
// lane's own C-named host copy. An /alternatename from a @@QAE LHS onto any
// of them would hand the body a `this` that never was one (the
// Model::LoadAndSetFile comment in hal/method_faces.cpp spells out why).
// `Platform2` / `Platform3` are the recovered Behavior's own local struct
// names for the same object, not real classes. The third face
// (?BaseBehavior@PathLift@@QAEXXZ) sits beside its host body in
// port/unmatched/PathLift_StateDispatch.cpp -- this file includes PathLift.h
// for the AfterClsn thunk and PathLift.h does not declare BaseBehavior.
extern "C" {
int _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}
struct Platform2 { void UpdateClsnPosAndRot(); };
struct Platform3 { int IsClsnInRange(int a, int b); };
void Platform2::UpdateClsnPosAndRot()
{ _ZN8Platform19UpdateClsnPosAndRotEv(this); }
int Platform3::IsClsnInRange(int a, int b)
{ return _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(this, a, b); }

// ---- the traps -------------------------------------------------------------
static void pl_trap_report(void *self, int slot, const char *which)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: %s vtable slot %d is not hosted (actor id %u %s)\n",
                 which, slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted %s vtable slot %d on id %u %s",
                    which, slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define PL_TRAP(n) \
    static int __fastcall pl_trap##n(void *s, void *) \
    { pl_trap_report(s, n, "path-lift"); return 0; }
PL_TRAP(13) PL_TRAP(14)
#undef PL_TRAP
/* the four ActorBase defaults the pure-intermediate base table carries and
   no live object can ever reach (see the header's six-site argument) */
#define PL_BASE_TRAP(n) \
    static int __fastcall pl_base_trap##n(void *s, void *) \
    { pl_trap_report(s, n, "PathLiftActor_c-base"); return 0; }
PL_BASE_TRAP(0) PL_BASE_TRAP(3) PL_BASE_TRAP(6) PL_BASE_TRAP(9)
#undef PL_BASE_TRAP

// ---- the shared half, slots 1..30 ------------------------------------------
// Read off BOTH reloc runs; the two tables agree word for word outside each
// class's own overrides, and agree with ov045/ov065's readings of the same
// arm9 addresses. THE POINTER IS VOLATILE ON PURPOSE: the gate-200
// elided-stores bug (hal/actor_classes_ov002g200.cpp).
static int __fastcall pl_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall pl_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall pl_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall pl_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall pl_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall pl_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall pl_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall pl_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall pl_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall pl_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall pl_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall pl_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall pl_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall pl_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall pl_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall pl_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall pl_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall pl_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall pl_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall pl_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall pl_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall pl_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall pl_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }
/* slot 32: the extra virtual PathLiftActor_c adds. BOTH tables carry the
   SAME word (0x020eff18), so both get the same thunk.

   THE THIRD PARAMETER IS THE POP, NOT A VALUE. The dispatch site is
   src/func_ov002_020eff90.cpp, `b->m(x)` over a 33-virtual class, which MSVC
   emits as thiscall with the argument PUSHED and no caller cleanup:

       mov  ecx,dword ptr [ebp+0Ch]      ; this
       push dword ptr [ebp+10h]          ; x -- the colliding Actor
       mov  eax,dword ptr [ecx]
       call dword ptr [eax+00000080h]    ; slot 32
       pop  ebp                          ; NO add esp,4: the callee pops
       ret

   The two-parameter shape emitted a bare `ret` and popped nothing, so `pop ebp`
   took the pushed Actor into EBP and `ret` took the saved EBP -- a stack
   address -- as the return address. That is report 7447e46c: level 46, walker
   PLAYER, `access 00000008 at 00e7f1bc`, `eip 00e7f1bc esp 00e7f1a4` (delta
   0x18), and `ebp 30039f38` equal to the dump's own `player` line, which is the
   un-popped argument sitting where the frame pointer belongs. It reached the
   veneer through the collider callback at MeshColliderBase+0x1c, which
   func_02038234 calls cdecl (`call ebx` / `add esp,0Ch`), and the same fault
   stands in that reporter's quarantine ledger on 0.2.8.2 as well.

   PathLift::AfterClsn takes VOID in the ROM (_ZN8PathLift9AfterClsnEv,
   0x020eff18, size 0x78): the argument rides in r1 on ARM and costs nothing
   there. On the host somebody still has to clean it, and that is the callee.
   The flw_reset spelling (bf9e332a5). */
static int __fastcall pl_after_clsn(void *s, void *, void * /*clsnActor, ridethrough*/)
{ ((PathLift *)s)->PathLift::AfterClsn(); return 0; }

static void pl_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)pl_binit;
    vt[2]  = (void *)pl_ainit;
    vt[4]  = (void *)pl_bclean;
    vt[5]  = (void *)pl_aclean;
    vt[7]  = (void *)pl_bbeh;
    vt[8]  = (void *)pl_abeh;
    vt[10] = (void *)pl_bren;
    vt[11] = (void *)pl_aren;
    vt[12] = (void *)pl_pdes;
    vt[13] = (void *)pl_trap13;      /* ActorBase Virtual34, the wf reading */
    vt[14] = (void *)pl_trap14;      /* ActorBase Virtual38                 */
    vt[15] = (void *)pl_heap;
    vt[18] = (void *)pl_yoshi;
    vt[19] = (void *)pl_turn_egg;
    vt[20] = (void *)pl_v50;
    vt[21] = (void *)pl_pounded;
    vt[22] = (void *)pl_atk1;
    vt[23] = (void *)pl_atk2;
    vt[24] = (void *)pl_kicked;
    vt[25] = (void *)pl_pushed;
    vt[26] = (void *)pl_cannon;
    vt[27] = (void *)pl_mega;
    vt[28] = (void *)pl_under;
    vt[29] = (void *)pl_egg;
    vt[30] = (void *)port_actor_s30_base;      /* returns a Vector3 by value; the SRET
                                        contract is unproved -- scuttlebug   */
    vt[31] = (void *)pl_kill;        /* Platform::Kill, both tables          */
    vt[32] = (void *)pl_after_clsn;  /* the INHERITED extra virtual          */
}

// ---- the base table, filled once -------------------------------------------
// PathLiftActor_c's own table. Slots 16/17 are its own destructors; 0/3/6/9
// are the ActorBase defaults an intermediate never dispatches (traps, see the
// header); everything else is the shared half.
static int __fastcall plb_d2(void *s, void *)
{ return _ZN8PathLiftD2Ev(s); }
static int __fastcall plb_d0(void *s, void *)
{ return _ZN8PathLiftD0Ev(s); }

static void pl_bringup(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    /* the ov002 sinit chain has already run by the time any registry fill is
       called (the gate-51 house order), so the six dispatch words hold DS
       code addresses right now; seat them before anything can dispatch. */
    port_pathlift_states_seat();
    void *volatile *vt = (void *volatile *)data_ov002_0210af70;
    pl_fill_shared(vt);
    vt[0]  = (void *)pl_base_trap0;
    vt[3]  = (void *)pl_base_trap3;
    vt[6]  = (void *)pl_base_trap6;
    vt[9]  = (void *)pl_base_trap9;
    vt[16] = (void *)plb_d2;
    vt[17] = (void *)plb_d0;
}

// ============================================================================
// PATH_LIFT (31) -- daObjPathLift_c, table 0x0214857c, 33 slots
// ============================================================================
//
// A 0x4b4-byte platform that rides a PathPtr: Model at +0xd4,
// MovingMeshCollider at +0x124, a THREE-element Model array at +0x320 (stride
// 0x50, the ends and the middle of the lift), PathPtr at +0x430,
// ShadowModel at +0x450, the state index at +0x44c. One on level 13, two on
// level 15.
static int __fastcall pl_init(void *s, void *)
{ return func_ov100_021471e0(s); }
static int __fastcall pl_clean(void *s, void *)
{ return func_ov100_02147054(s); }
static int __fastcall pl_behavior(void *s, void *)
{ return func_ov100_021470f4(s); }
static int __fastcall pl_render(void *s, void *)
{ port_actor_render_probe("PATH_LIFT", (char *)s + 0xd4);
  return func_ov100_021470a4(s); }
static int __fastcall pl_d1(void *s, void *)
{ return func_ov100_02146d7c(s); }
static int __fastcall pl_d0(void *s, void *)
{ return (int)(size_t)func_ov100_02146dec(s); }

extern "C" void hal_fill_path_lift_vtable(void)
{
    pl_bringup();
    void *volatile *vt = (void *volatile *)data_ov100_0214857c;
    pl_fill_shared(vt);
    vt[0]  = (void *)pl_init;
    vt[3]  = (void *)pl_clean;
    vt[6]  = (void *)pl_behavior;
    vt[9]  = (void *)pl_render;
    vt[16] = (void *)pl_d1;
    vt[17] = (void *)pl_d0;
}
