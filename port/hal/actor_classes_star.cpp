// GATE 89: the POWER_STAR class (178, ov002), and GATE 90: STAR_CAMERA (177,
// ov002).
//
// Same law as hal/actor_classes_wf_enemy.cpp -- ROM slot order, __fastcall
// thunks that call QUALIFIED or forward to C-named free functions, unhosted
// slots trap by name -- and the THIRTY-ONE slot shape, because PowerStar is an
// Enemy subclass (through Actor), not the twenty-slot ActorBase shape.
//
// ---- POWER_STAR (178) -----------------------------------------------------
//
// _ZTV9PowerStar @ ov002 0x0210ab3c, 31 slots, RTTI daStar_c (aliased
// _ZTV8daStar_c at the same address). PowerStar_Spawn installs it directly
// (p[0] = (int)_ZTV9PowerStar) after _ZN5EnemyC2Ev, so the vtable is a HOST
// array this file declares and the registry fills -- the ov079 rule, a mounted
// vtable would hand the factory DS code addresses.
//
// The class overrides, read out of the ROM vtable (relocs.txt):
//   slot 0  InitResources  020eb63c  _ZN9PowerStar13InitResourcesEv (matched)
//   slot 3  CleanupResources 020eac18 _ZN9PowerStar16CleanupResourcesEv (matched)
//   slot 6  Behavior       020eb05c  _ZN9PowerStar8BehaviorEv -- HOST COPY
//                          (the PMF state dispatch, port/unmatched/PowerStar_States)
//   slot 9  Render         020eacf4  _ZN9PowerStar6RenderEv -- HOST COPY
//                          (ModelAnim slot-5 dispatch, port/unmatched/ModelAnim_Renders.cpp)
//   slot 16 D1             020e6c40  _ZN9PowerStarD1Ev (matched, C-named .c)
//   slot 17 D0             020e6c90  _ZN9PowerStarD0Ev (matched, C-named .c)
//   slot 18 OnYoshiTryEat  020e8ee8  func_ov002_020e8ee8 (matched)
//   slot 19 (egg)          020e8edc  func_ov002_020e8edc (matched)
// every other slot is an Actor/ActorBase base method, the ac31/we31 shared set.
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
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);          /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);               /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                 /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                /* slot 18 default */
int _ZN5Actor9Virtual50Ev(void *self);                     /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);  /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);      /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);      /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);          /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);          /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* the Enemy tier's own eight-entry death table, seated the first time an
   Enemy-family class registers (idempotent) */
void port_enemy_death_states_seat(void);
}

// ---- the trap --------------------------------------------------------------
static void star_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define STAR_TRAP(n) \
    static int __fastcall star_trap##n(void *s, void *) \
    { star_trap_report(s, n); return 0; }
STAR_TRAP(13) STAR_TRAP(14) STAR_TRAP(30)
#undef STAR_TRAP

// ---- the ten shared lifecycle halves plus Actor's tail ---------------------
static int __fastcall star_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall star_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall star_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall star_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall star_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall star_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall star_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall star_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall star_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall star_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall star_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall star_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall star_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall star_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall star_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall star_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall star_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall star_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall star_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

/* The shared half of a 31-slot Actor table: Actor's four Before/After pairs,
   ActorBase's OnHeapCreated/OnPendingDestroy, Virtual50 and the eight combat
   hooks, plus the traps at 13/14/30. Slot 12 is ActorBase::OnPendingDestroy;
   slots 18/19 are class overrides the caller writes. */
static void star31_fill_shared(void **vt)
{
    vt[1] = (void *)star_binit;
    vt[2] = (void *)star_ainit;
    vt[4] = (void *)star_bclean;
    vt[5] = (void *)star_aclean;
    vt[7] = (void *)star_bbeh;
    vt[8] = (void *)star_abeh;
    vt[10] = (void *)star_bren;
    vt[11] = (void *)star_aren;
    vt[12] = (void *)star_pdes;
    vt[13] = (void *)star_trap13;
    vt[14] = (void *)star_trap14;
    vt[15] = (void *)star_heap;
    vt[20] = (void *)star_v50;
    vt[21] = (void *)star_pounded;
    vt[22] = (void *)star_atk1;
    vt[23] = (void *)star_atk2;
    vt[24] = (void *)star_kicked;
    vt[25] = (void *)star_pushed;
    vt[26] = (void *)star_cannon;
    vt[27] = (void *)star_mega;
    vt[28] = (void *)star_under;
    vt[29] = (void *)star_trap30;   /* OnAimedAtWithEgg default -- overwritten below */
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// POWER_STAR (actor 178), _ZTV9PowerStar @ ov002 0x0210ab3c
// ============================================================================
extern "C" {
int _ZN9PowerStar13InitResourcesEv(void *self);   /* face: below */
int _ZN9PowerStar16CleanupResourcesEv(void *self); /* face: below */
int _ZN9PowerStar6RenderEv(void *self);            /* face: below */
int _ZN9PowerStar8BehaviorEv(void *self);          /* host copy */
int *_ZN9PowerStarD1Ev(int *self);
int *_ZN9PowerStarD0Ev(int *self);
int func_ov002_020e8ee8(void *self);               /* slot 18 override */
int func_ov002_020e8edc(void *self, void *player); /* slot 19 override */
void *_ZTV9PowerStar[31];
void port_power_star_states_seat(void);   /* port/unmatched/PowerStar_States */

/* the two Actor combat-tail slots PowerStar leaves as Actor's own bodies but
   the ROM table names explicitly at 29/30 (OnAimedAtWithEgg /
   OnAimedAtWithEggReturnVec). 29 forwards to Actor's; 30 returns a Vector3 by
   value, an ABI a thunk cannot bridge, so it TRAPS -- nothing aims a Yoshi egg
   at the star as Mario, same as every Enemy class in the port. */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);     /* slot 29 */
}
#pragma comment(linker, "/alternatename:__ZTV8daStar_c=__ZTV9PowerStar")

/* PowerStar's Cleanup/InitResources and the state helper func_ov002_020e8ef0
   declare a handful of data symbols as bare typed globals OUTSIDE extern "C", so
   MSVC C++-mangles the references while the ov002 mount / arm9 romdata / a hal
   file define them with C linkage. Alias each mangled reference onto the C
   symbol -- the same seam actor_faces_bob.cpp closes for data_ov002_0211092c.
   Not a src edit. The three 0x8016/0x801a/0x801b SharedFilePtrs and the 0x8018
   one PowerStar::CleanupResources releases: */
#pragma comment(linker, "/alternatename:?data_ov002_02110934@@3DA=_data_ov002_02110934")
#pragma comment(linker, "/alternatename:?data_ov002_02110944@@3DA=_data_ov002_02110944")
#pragma comment(linker, "/alternatename:?data_ov002_02110924@@3DA=_data_ov002_02110924")
#pragma comment(linker, "/alternatename:?data_ov002_02110964@@3DA=_data_ov002_02110964")
/* the four arm9/hal globals func_ov002_020e8ef0 (the coin-record state) reads: */
#pragma comment(linker, "/alternatename:?data_0209b454@@3IA=_data_0209b454")
#pragma comment(linker, "/alternatename:?data_0209f228@@3EA=_data_0209f228")
#pragma comment(linker, "/alternatename:?data_0209f2ac@@3EA=_data_0209f2ac")
#pragma comment(linker, "/alternatename:?data_0209f358@@3PAFA=_data_0209f358")

/* The VS-star obtained array. GiveVsStars writes data_0209f310[idx] as a
   signed-char array; NumVsStarsObtained reads data_0209f310 then walks the run
   from &data_0209f311, the ROM's own next byte (f310 is 1 byte, f311 the next
   3, then f314 begins a DIFFERENT symbol -- the level area table, hosted in
   camera_bridges.cpp). f310 and f311 must therefore be the first two bytes of
   ONE contiguous block. MSVC grouped sections put them in ADDRESS ORDER: two
   $NNNN contributions to one section, laid out by suffix, the same idiom
   romdata.py's CONTIG runs use. (This note used to say "adjacently"; order and
   adjacency are not the same claim and only the first one was true. See
   below.) A generous 8-byte head keeps room for the 4-byte ROM run
   (f310..f313) plus slack.

   THE ALIGNMENT WAS THE MISSING HALF, and the block was NOT adjacent. Measured
   by run rel0215 lane prop15 out of the release build's own maps, all three
   hosting targets, before any change:

       walk_window.map        _data_0209f310 00d280d4   _data_0209f311 00d280d8
       smoke_player.map       _data_0209f310 009fd0d0   _data_0209f311 009fd0d4
       walk_window_hires.map  _data_0209f310 015980d4   _data_0209f311 015980d8

   f311 four bytes behind f310, where the ROM puts it ONE. Grouped sections put
   contributions in suffix order, which is what the note above claims and is
   true; they do NOT make them adjacent, because each contribution still gets
   the linker's default four-byte alignment. hal/cxx_aliases.cpp's GXBANK macro
   already carries the other half -- "align(4) on every member would pad each
   one to four and put +0x18 at +0x30" -- and this pair was written without it.

   WHAT THAT COST, and it is a read/write SPLIT rather than a clobber: every
   writer indexes data_0209f310[i] (GiveVsStars, Stage::InitResources' p2 walk,
   func_ov002_020d94cc's drop test, the three HUD readers, and
   func_ov075_021165b0's explicit [0][1][2][3] scan), landing on host +0..+3.
   NumVsStarsObtained -- whose sum IS the VS win test, `== 5` -- starts at f310
   for player 0 and then walks from &data_0209f311, landing on +4..+6 for
   players 1, 2 and 3. Three bytes of inter-contribution padding that nothing
   writes sat between them. Player 0 read back correctly, players 1-3 always
   read zero, and nothing was corrupted, which is why no run ever faulted on it.

   align(1) on the member is the fix and align(4) on the head keeps the block's
   own placement deterministic, exactly the GXBANK arrangement. */
#pragma section(".dsstate$hvsstar0000", read, write)
#pragma section(".dsstate$hvsstar0001", read, write)
extern "C" {
__declspec(allocate(".dsstate$hvsstar0000")) __declspec(align(4))
signed char data_0209f310[1];
__declspec(allocate(".dsstate$hvsstar0001")) __declspec(align(1))
signed char data_0209f311[31];
}
/* Reads the layout back at run time the way port_gxbank_layout_check does, so
   a section trick that stops working fails loudly instead of quietly reading
   another byte. The link-time answer is port/tools/gxband_guard.py's 'vsstar'
   band, which checks every hosting target's map rather than the one code path
   that calls this. */
extern "C" int port_vsstar_layout_check(void)
{
    const long d = (long)(data_0209f311 - data_0209f310);
    if (d == 1)
        return 1;
    std::printf("  [vs] STAR BAND NOT CONTIGUOUS: f311 at f310+%ld (want 1) --"
                " NumVsStarsObtained reads bytes no writer touches for players"
                " 1..3\n", d);
    return 0;
}
/* func_ov002_020d94cc declares data_0209f310 as a bare `signed char` outside
   extern "C" (a second, C++-mangled spelling); alias it onto the C symbol. */
#pragma comment(linker, "/alternatename:?data_0209f310@@3CA=_data_0209f310")

/* func_ov002_020e6df8 (the VS-star spawn helper) declares the same two mount
   symbols with their real struct types (Anim2 / int*) OUTSIDE extern "C", a
   second MSVC mangling of names the ov002 mount defines with C linkage. Alias
   these too. data_ov002_02110944 is one of the SharedFilePtr/Anim2 pointers
   the sinit builds; data_ov002_0210aa0c is the Vector3 collision offset now in
   the ov002 mount. */
#pragma comment(linker, "/alternatename:?data_ov002_02110944@@3UAnim2@@A=_data_ov002_02110944")
#pragma comment(linker, "/alternatename:?data_ov002_0210aa0c@@3PAHA=_data_ov002_0210aa0c")

/* func_ov002_020e81e0 calls Particle::System::New with a (System*)-returning,
   Vector3-taking signature MSVC mangles differently from the two aliases
   cxx_aliases.cpp already carries. Point this third spelling at the same
   matched Itanium body. */
#pragma comment(linker, "/alternatename:?New@System@Particle@@SAPAU12@IIHHHPBUVector3@@PAUCallback@2@@Z=__ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE")

/* gate 90: _ZN5ActorC1Ev (the matched Actor base ctor StarCamera_Spawn calls)
   declares its engine globals as bare typed C++ names, which MSVC mangles while
   the port hosts them with C linkage (data_0208e3a4 here, the rest in
   actor_vtables.cpp / actor_registry.cpp). Alias each mangled reference onto its
   C symbol -- the same seam every gate uses for a .cpp that reaches a mount or
   hal global outside extern "C". */
#pragma comment(linker, "/alternatename:?data_0208e3a4@@3PAXA=_data_0208e3a4")
#pragma comment(linker, "/alternatename:?data_0209b468@@3PAXA=_data_0209b468")
#pragma comment(linker, "/alternatename:?data_0209b460@@3PAXA=_data_0209b460")
#pragma comment(linker, "/alternatename:?data_0209b45c@@3PAXA=_data_0209b45c")
#pragma comment(linker, "/alternatename:?data_0208e378@@3FA=_data_0208e378")
#pragma comment(linker, "/alternatename:?data_020a4bb8@@3PAXA=_data_020a4bb8")

static int __fastcall ps_init(void *s, void *)
{ return _ZN9PowerStar13InitResourcesEv(s); }
static int __fastcall ps_clean(void *s, void *)
{ return _ZN9PowerStar16CleanupResourcesEv(s); }
static int __fastcall ps_behavior(void *s, void *)
{ return _ZN9PowerStar8BehaviorEv(s); }
static int __fastcall ps_render(void *s, void *)
{ port_actor_render_probe("POWER_STAR", (char *)s + 0x30c);
  return _ZN9PowerStar6RenderEv(s); }
static int __fastcall ps_d1(void *s, void *)
{ return (int)(size_t)_ZN9PowerStarD1Ev((int *)s); }
static int __fastcall ps_d0(void *s, void *)
{ return (int)(size_t)_ZN9PowerStarD0Ev((int *)s); }
static int __fastcall ps_yoshi(void *s, void *)
{ return func_ov002_020e8ee8(s); }
/* Slot 19 is OnTurnIntoEgg(Player &player): the pushed player has to be
   declared so the thunk pops it, the way ps_yoshi's argument-free slot 18
   does not have to, and it has to be FORWARDED. func_ov002_020e8edc is the
   ROM's `ldr ip,[pc]; bx ip` veneer onto func_ov002_020e8e80(c, a), and a
   veneer is a tail jump on the host too, so whatever this pushes is what
   the body reads. Pushing only self left the body's `a` reading this
   thunk's own return address, which it stored at +0x438 and passed on to
   func_ov002_020e8ef0. */
static int __fastcall ps_s19(void *s, void *, void *player)
{ return func_ov002_020e8edc(s, player); }
static int __fastcall ps_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

extern "C" void hal_fill_power_star_vtable(void)
{
    void **vt = _ZTV9PowerStar;
    port_enemy_death_states_seat();
    port_power_star_states_seat();
    star31_fill_shared(vt);
    vt[0] = (void *)ps_init;
    vt[3] = (void *)ps_clean;
    vt[6] = (void *)ps_behavior;
    vt[9] = (void *)ps_render;
    vt[16] = (void *)ps_d1;
    vt[17] = (void *)ps_d0;
    vt[18] = (void *)ps_yoshi;
    vt[19] = (void *)ps_s19;
    vt[29] = (void *)ps_aimed;
}

// ---- method faces ----------------------------------------------------------
// The vtable's C-named references onto the matched lifecycle TUs. PowerStar's
// InitResources/CleanupResources/Render are real MSVC methods against
// include/PowerStar.h; D1/D0 and the two func_ov002_* overrides are already
// C-named free functions in their own TUs.
#include "PowerStar.h"
extern "C" {
int _ZN9PowerStar13InitResourcesEv(void *self)
{ return ((PowerStar *)self)->PowerStar::InitResources(); }
int _ZN9PowerStar16CleanupResourcesEv(void *self)
{ return ((PowerStar *)self)->PowerStar::CleanupResources(); }
/* PowerStar::Render is NOT faced here: it dispatches ModelAnim slot 5 through a
   local six-virtual shadow (sub.m5(&arg80), the ROM Render), which the host
   _ZTV9ModelAnim array numbers as Virtual18 (MSVC folds the two dtor slots into
   one). src/_ZN9PowerStar6RenderEv.cpp is dropped from slice_gate89.txt and
   _ZN9PowerStar6RenderEv is the host copy in port/unmatched/ModelAnim_Renders.cpp,
   the Whomp/Butterfly/Fish/QuestionBlock case. Measured as a c0000005 EXECUTION
   fault at actor+0x38 on the first drawn frame of a spawned star. */
}

// ============================================================================
// GATE 90: STAR_CAMERA (actor 177), the BASE Actor vtable _ZTV5Actor
// ============================================================================
//
// StarCamera_Spawn (src/StarCamera_Spawn.cpp) is `new(0xd4) Actor` then
// _ZN5ActorC1Ev -- it installs NO vtable of its own, so a spawned STAR_CAMERA
// dispatches every virtual through the base Actor vtable, _ZTV5Actor at arm9
// 0x0208e3a4 (aliased data_0208e3a4, 31 slots). The port kept that symbol as
// zeroed transient storage, so registering STAR_CAMERA as-is dispatched through
// nulls on the first frame.
//
// FILLING IT IS PROVABLY SAFE. _ZN5ActorC1Ev is the ONLY code that stores
// data_0208e3a4 as an object's vptr, and it does so transiently: it writes
// data_0208e4b8 (ActorDerived) then data_0208e3a4 (Actor), and every derived
// class's own Spawn overwrites word 0 with its class vtable immediately after
// the ctor returns. No virtual is dispatched between the ctor's store and the
// derived overwrite (the ctor calls its bases QUALIFIED, never virtual), so the
// contents of data_0208e3a4 are never read for any class but the one that
// LEAVES it installed. StarCamera_Spawn is the ONLY such TU in src/ (verified:
// it is the only file that calls _ZN5ActorC1Ev and installs no vtable after).
// So this fill changes the behaviour of exactly one class -- STAR_CAMERA --
// turning its null dispatches into the ROM's own Actor/ActorBase bodies, and is
// invisible to every other class's ctor.
//
// The 31 slots are the ROM's own _ZTV5Actor (relocs.txt, 0x0208e3a4..0x0208e41c):
// ActorBase's InitResources/CleanupResources/Behavior/Render at 0/3/6/9 (a bare
// Actor overrides none of them), Actor's Before/After hooks, ActorBase's
// Virtual34/38 at 13/14, OnHeapCreated at 15, Actor D1/D0 at 16/17, the Actor
// defaults for OnYoshiTryEat/OnTurnIntoEgg/Virtual50 at 18/19/20 and the ten
// combat hooks at 21..30. Slot 30 (OnAimedAtWithEggReturnVec) returns a Vector3
// by value, an ABI a thunk cannot bridge, so it TRAPS -- nothing aims a Yoshi
// egg at the star camera as Mario, the same trap every Enemy class carries.
// The base bodies are real MSVC methods against include/ActorBase.h and
// include/Actor.h, so the thunks call them QUALIFIED (never virtual), the same
// way hal/actor_classes_wf_enemy.cpp reaches Actor's methods. The Actor tail
// virtuals with C-named free-function TUs (OnYoshiTryEat/OnTurnIntoEgg/
// OnAimedAtWithEgg, D1/D0) are declared extern "C".
extern "C" {
int _ZN9ActorBase13InitResourcesEv(void);                /* slot 0, C-free */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);           /* slot 29, C-free */
int _ZN5Actor13OnYoshiTryEatEv(void *self);              /* slot 18, C-free */
void _ZN5Actor13OnTurnIntoEggER6Player(void);            /* slot 19, C-free veneer */
int *_ZN5ActorD1Ev(int *self);                           /* slot 16 */
int *_ZN5ActorD0Ev(int *self);                           /* slot 17 */
extern void *data_0208e3a4[];   /* _ZTV5Actor, storage in hal/actor_vtables.cpp */
}

static int __fastcall star_d1(void *s, void *)
{ return (int)(size_t)_ZN5ActorD1Ev((int *)s); }
static int __fastcall star_d0(void *s, void *)
{ return (int)(size_t)_ZN5ActorD0Ev((int *)s); }

static int __fastcall ab_init(void *s, void *)
{ (void)s; return _ZN9ActorBase13InitResourcesEv(); }
static int __fastcall ab_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall ab_behavior(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Behavior(); }
static int __fastcall ab_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall ac_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ac_egg(void *s, void *, void *)
{ _ZN5Actor13OnTurnIntoEggER6Player(); return 0; }
static int __fastcall ac_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* The base Actor vtable is filled slot by slot HERE rather than through
   star31_fill_shared. data_0208e3a4 is aliased to _ZTV5Actor and declared with
   a different type (scalar void*) in src/_ZN5ActorC1Ev.cpp than here (void*[31]);
   MSVC's optimiser exploited that cross-TU type disagreement to treat the
   through-parameter stores star31_fill_shared makes into it as dead and drop
   them (it did NOT for _ZTV9PowerStar, which no TU spells as a scalar). Filling
   every slot in this function's own body sidesteps the elimination. The layout
   is the ROM's 31-slot _ZTV5Actor. */
extern "C" void hal_fill_actor_base_vtable(void)
{
    void **vt = data_0208e3a4;
    vt[0] = (void *)ab_init;       /* ActorBase::InitResources (not overridden) */
    vt[1] = (void *)star_binit;    /* Actor::BeforeInitResources */
    vt[2] = (void *)star_ainit;    /* Actor::AfterInitResources */
    vt[3] = (void *)ab_clean;      /* ActorBase::CleanupResources */
    vt[4] = (void *)star_bclean;   /* Actor::BeforeCleanupResources */
    vt[5] = (void *)star_aclean;   /* ActorBase::AfterCleanupResources */
    vt[6] = (void *)ab_behavior;   /* ActorBase::Behavior */
    vt[7] = (void *)star_bbeh;     /* Actor::BeforeBehavior */
    vt[8] = (void *)star_abeh;     /* ActorBase::AfterBehavior */
    vt[9] = (void *)ab_render;     /* ActorBase::Render */
    vt[10] = (void *)star_bren;    /* Actor::BeforeRender */
    vt[11] = (void *)star_aren;    /* ActorBase::AfterRender */
    vt[12] = (void *)star_pdes;    /* ActorBase::OnPendingDestroy */
    /* slots 13/14 (ActorBase::Virtual34/38, the per-instance heap hooks) trap
       -- a bare STAR_CAMERA never dispatches them, the same treatment every
       Enemy class gives 13/14. If the trap ever fires, host them. */
    vt[13] = (void *)star_trap13;
    vt[14] = (void *)star_trap14;
    vt[15] = (void *)star_heap;    /* ActorBase::OnHeapCreated */
    vt[16] = (void *)star_d1;      /* Actor::~Actor (D1) */
    vt[17] = (void *)star_d0;      /* Actor::~Actor (D0) */
    vt[18] = (void *)ac_yoshi;     /* Actor::OnYoshiTryEat default */
    vt[19] = (void *)ac_egg;       /* Actor::OnTurnIntoEgg default */
    vt[20] = (void *)star_v50;     /* Actor::Virtual50 */
    vt[21] = (void *)star_pounded; /* the ten combat hooks */
    vt[22] = (void *)star_atk1;
    vt[23] = (void *)star_atk2;
    vt[24] = (void *)star_kicked;
    vt[25] = (void *)star_pushed;
    vt[26] = (void *)star_cannon;
    vt[27] = (void *)star_mega;
    vt[28] = (void *)star_under;
    vt[29] = (void *)ac_aimed;     /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;  /* OnAimedAtWithEggReturnVec: Vector3 by value */
}
