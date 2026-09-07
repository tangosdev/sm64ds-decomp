// GATE 232 (run link100, lane SPAWN2) -- ov010's TRAP (actor 36) gets its OWN
// vtable back. daObjC1_Trap_c, the castle interior's traps, level 2.
// ============================================================================
//
// WHAT THIS FILE CORRECTS. hal/actor_classes.cpp's gate-41 block carries
//
//     #pragma comment(linker, "/alternatename:__ZTV14daObjC1_Trap_c=__ZTV4Trap")
//
// on the reading that ONE table wears three names: config's _ZTV4Trap,
// LIGHT_BEAM's RTTI _ZTV15daObjC1Hikari_c at the same address, and TRAP's own
// RTTI _ZTV14daObjC1_Trap_c. The ROM says TWO tables of TWO classes, and it says
// it four independent ways:
//
//   1. THE RTTI NAME STRINGS. Each table's vtable-4 word relocates to that
//      class's typeinfo record, whose word[1] points at the Itanium name:
//        0x02112ae4 -> 0x02112aa0 -> name at 0x02112aac = "14daObjC1_Trap_c"
//        0x02112ba8 -> 0x02112b64 -> name at 0x02112b70 = "15daObjC1Hikari_c"
//   2. THE FACTORIES' OWN LITERAL POOLS. Trap_Spawn (0x02111998, the factory
//      Trap_SpawnInfo's word[0] relocates to, whose +4 halfword reads 36) loads
//      0x02112ae4 at pool word 0x021119cc. LightBeam_Spawn (0x02111dd0, id 37)
//      loads 0x02112ba8 at 0x02111e0c.
//   3. EVERY LOAD IN THE OVERLAY. -> 0x02112ae4 comes from 0x021111e4 (this
//      class's D1), 0x02111240 (its D0) and 0x021119cc (its factory).
//      -> 0x02112ba8 comes from 0x02111a04 and 0x02111a4c (Hikari's own D0) and
//      0x02111e0c (LightBeam_Spawn). The two sets do not intersect.
//   4. THE SLOTS THEMSELVES. 0x02112ae4 slot 0 is func_ov010_02111654;
//      0x02112ba8 slot 0 is _ZN4Trap13InitResourcesEv. Different bodies.
//
// So the alias sent TRAP's factory to LIGHT_BEAM's table, and until this gate a
// TRAP -- 944 bytes, Platform-derived, Model at +0x320 -- ran LIGHT_BEAM's
// InitResources, Behavior and Render, which are written for a 364-byte
// Actor-derived object with its Model at +0xd4. hal/actor_classes.cpp already
// knew something was wrong without knowing what: its tr_d1 discriminates on the
// actor id at +0xc and runs the ROM destructor chain only for 0x25, and carries
// a "LOUD LEAK" note saying TRAP's Model at +0x320 is never torn down. Both are
// symptoms of the mis-binding, not of a ROM ambiguity. That file is not this
// lane's and is NOT edited: the fix is entirely in the registry rows
// (hal/actor_classes.inc), the mount (port/ov010_syms.txt) and this new file.
// hal_fill_trap_vtable keeps filling _ZTV4Trap and simply moves to the row that
// really owns it, id 37 LIGHT_BEAM.
//
// FOLLOW-UP OWED to whoever owns hal/actor_classes.cpp: with 36 off that table,
// only LIGHT_BEAM reaches it, so tr_d1's id==0x25 discriminator and the LOUD
// LEAK note are both dead and slot 16 can go back to a straight forward.
//
// THE TABLE. data_ov010_02112ae4, 32 slots. The reloc run covers
// [0x02112ae4, 0x02112b64): slot 31 is 0x020ee55c, the shared Platform::Kill
// every 32-slot Platform table in this tree carries, and the word at +0x80 is
// 0x0209a764, which is the NEXT class's typeinfo record and not a slot. dsd's
// data_ov010_02112b08 is a symbol it landed INSIDE this table at +0x24, which is
// why lane STUBADJ's census keys two of these bodies as "02112b08 slot 8" and
// "slot 0" -- those are the array's slots 17 and 9.
//
// THE SIX OWN SLOTS, each the destination of the reloc at vtable + 4*N with a
// kind:function(arm,size=..) record at exactly that address, none interior:
//
//   slot 0   0x02112ae4 -> 0x02111654  func_ov010_02111654  0x1f8  InitResources
//   slot 3   0x02112af0 -> 0x02111554  func_ov010_02111554  0x54   CleanupResources
//   slot 6   0x02112afc -> 0x021115e0  func_ov010_021115e0  0x74   Behavior
//   slot 9   0x02112b08 -> 0x021115a8  func_ov010_021115a8  0x38   Render
//   slot 16  0x02112b24 -> 0x021111a0  func_ov010_021111a0  0x4c   D1
//   slot 17  0x02112b28 -> 0x021111ec  func_ov010_021111ec  0x60   D0
//
// Five of the six carry the "recovered from vtable slot identity" marker and
// lane STUBADJ ruled all five REAL_DECOMP against the ROM (match.py 2004/b56
// strict-reloc MATCH, 0 WRONG-DEST); the D1 never carried one. THE SLOT CAME
// FROM THE RELOC, NEVER FROM THE COMMENT: src/func_ov010_021111ec.c is headed
// "daObjC1_Trap_c::OnYoshiTryEat" and is the slot-17 deleting destructor, which
// its own last line (Memory::Deallocate) settles.
//
// SLOT 9 IS THE SHADOW-DISPATCH SHAPE and it is safe here for the reason
// hal/actor_classes_ov043.cpp gives for DIAMOND_LIFT's byte-identical body: the
// object it dispatches slot 5 of lives at +0x320 and InitResources builds it
// with Model::LoadFile into ModelBase::SetFile while both destructors call
// _ZN5ModelD1Ev on +0x320, so it is a plain Model and hal/cxxname_bridge.cpp's
// dual-filled _ZTV5Model[5] is Model::Render.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dsstate_seg.h"
#include "Actor.h"
#include "ActorBase.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of this table IS
   the arm9 base body 0x020100dc (checked at 0x02112b5c in ov010/relocs.txt),
   and the three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

extern "C" {
/* the shared arm9 lifecycle halves. This table's non-own words are the SAME 26
   addresses ov029's and ov043's Platform tables carry, checked one by one:
   1=0x02011268 2=0x02011244 4=0x02011220 5=0x02011214 7=0x02010fd4
   8=0x02010fc8 10=0x02010f78 11=0x02010f6c 12=0x02043ac0 13=0x0204357c
   14=0x0204349c 15=0x02043494 18..29=0x02010160..0x02010124 30=0x020100dc
   31=0x020ee55c. */
int  _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int  _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int  _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
/* slots 4, 5, 8, 11, 12 and 15 are reached through the C++ class rather than an
   Itanium C name, because those five have no C-linkage spelling in this tree --
   hal/actor_classes_ov029.cpp and hal/actor_classes_ov043.cpp both do the same. */
int  _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int  _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);
void hal_fill_platform_vtable(void);            /* hal/actor_classes.cpp */

/* the six own bodies, addresses above */
int  func_ov010_02111654(char *self);   /* slot 0  */
int  func_ov010_02111554(char *self);   /* slot 3  */
int  func_ov010_021115e0(void *self);   /* slot 6  */
int  func_ov010_021115a8(char *self);   /* slot 9  */
int *func_ov010_021111a0(int *self);    /* slot 16 */
int *func_ov010_021111ec(int *self);    /* slot 17 */

/* the five state bodies the Behavior dispatcher reaches through the pointer-to-
   member cells. All plain cdecl C in src/, none linked before this gate. */
void func_ov010_02111320(char *c);          /* state 0 */
void func_ov010_021112b4(char *c);          /* state 1 */
void func_ov010_02111284(int c);            /* state 2 */
void func_ov010_0211125c(unsigned char *c); /* state 3 */
void func_ov010_0211124c(char *c);          /* state 4 */

/* the five MOUNTED 8-byte {fn, delta} source records __sinit_ov010_0211203c
   copies. The mount emits each as `u8 data_ov010_02112a__[8]` holding the ROM's
   own DS word, which is what the seat below verifies against. */
extern unsigned char data_ov010_02112a78[];
extern unsigned char data_ov010_02112a80[];
extern unsigned char data_ov010_02112a88[];
extern unsigned char data_ov010_02112a90[];
extern unsigned char data_ov010_02112a98[];

/* THE HOST TABLE. `int` and C linkage because include/decl_common.h declares
   the placeholder the per-source -D renames as `extern int _ZTV14daObjC1_Trap_c[]`,
   and src/Trap_Spawn.c spells it the same way. Inside DSSTATE because it is a
   hosted DS name and dsstate_guard requires every one of those to sit in the
   captured segment. port/ov010_syms.txt gives the span up with this gate: the
   two rows data_ov010_02112ae4:0x24 and data_ov010_02112b08:0x5c come out and
   the generator leaves a zero-filled pk010_gap_* in their place, which is how
   every other hosted table in the tree is handled. */
DSSTATE_BEGIN
int data_ov010_02112ae4[32];
DSSTATE_END
}

/* THE DISPATCH CELLS, and why they are declared exactly like this.
   src/func_ov010_021115e0.cpp -- the matched Behavior -- opens with
       struct C; typedef void (C::*PMF)();
       extern PMF data_ov010_02112d28[];
   and calls (c->*data_ov010_02112d28[state])(). That declaration sits OUTSIDE
   extern "C", so MSVC decorates it with the pointer-to-member type and the
   symbol the linker wants is the mangled one, which no C-named mount object can
   satisfy. Rather than guess a decoration off an LNK2019, port/CMakeLists.txt
   puts a per-source -D on that ONE file renaming the array to the name below,
   and this file DEFINES it with the identical two declarations. Same type, same
   flags, so both translation units mangle to the same symbol and it resolves
   with no alias at all.

   THE WIDTH IS THE ROM'S, and it is asserted rather than assumed: the tree
   compiles every C++ TU with /vmg /vmm (port/CMakeLists.txt), which the measured
   table there records as eight bytes for a forward-declared class's PMF with
   word 0 the code address and word 1 the this-adjust -- the ROM's own record
   shape. If that ever stops holding, the static_assert fails the build instead
   of letting the dispatcher stride off the end of a five-cell table. */
struct C;
typedef void (C::*PMF)();
DSSTATE_BEGIN
PMF port_trap36_states[5];
DSSTATE_END
static_assert(sizeof(PMF) == 8,
              "ov010 TRAP's state cells are the ROM's 8-byte {fn, delta}; "
              "/vmg /vmm must be on for this table to be walkable");

/* THE TWO SharedFilePtr NAMES slot 3 spells are dsd cross-overlay misnames.
   src/func_ov010_02111554.cpp says data_ov025_02112d08 and data_ov024_02112d00;
   the ROM's own loads inside that body, at 0x021115a0 and 0x021115a4, are
   data_ov010_02112d08 and data_ov010_02112d00, both mounted.
   AN /alternatename ONTO THE dsd SPELLINGS WOULD BE DEFEATED AND SILENT:
   data_ov025_02112d08 is really defined -- ov025's own mount emits it and it is
   in walk_window.map, with hal/actor_classes_ov025.cpp already binding its
   mangled CLPS_Block spelling onto it -- so the body would Release ov025's
   CLPS_Block instead of this class's file pointer. And data_ov024_02112d00
   cannot take a -D straight onto the mount name either: decl_common.h declares
   it `extern char data_ov024_02112d00[]` while declaring
   `extern void *data_ov010_02112d00` as a SCALAR, which is a C2040. So both
   take the PATH_LIFT remedy: a per-source -D to a private name this seat owns,
   bound onto the mounted object here. */
extern "C" {
unsigned char port_ov010_trap_modelfile[];  /* -> data_ov010_02112d08 */
unsigned char port_ov010_trap_clsnfile[];   /* -> data_ov010_02112d00 */
}
#pragma comment(linker, "/alternatename:_port_ov010_trap_modelfile=_data_ov010_02112d08")
#pragma comment(linker, "/alternatename:_port_ov010_trap_clsnfile=_data_ov010_02112d00")

// ---- the trap ---------------------------------------------------------------
// 13/14 are ActorBase::Virtual34/38(u32,u32) -- two arguments past `this`, which
// no int __fastcall(void*, void*) thunk can serve. Trapped by name, the shape
// every sibling fill in this tree uses, never seated wrong.
static void ov10_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov010 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov010 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV10_TRAP(n) \
    static int __fastcall ov10_trap##n(void *s, void *) \
    { ov10_trap_report(s, n); return 0; }
OV10_TRAP(13) OV10_TRAP(14)
#undef OV10_TRAP

// ---- the shared half, slots 1..30 -------------------------------------------
static int  __fastcall ov10_binit(void *s, void *)  { return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov10_ainit(void *s, void *, unsigned a) { _ZN5Actor18AfterInitResourcesEj(s, a); }
static int  __fastcall ov10_bclean(void *s, void *) { return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov10_aclean(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int  __fastcall ov10_bbeh(void *s, void *)   { return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov10_abeh(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int  __fastcall ov10_bren(void *s, void *)   { return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov10_aren(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int  __fastcall ov10_pdes(void *s, void *)   { ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int  __fastcall ov10_heap(void *s, void *)   { return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int  __fastcall ov10_yoshi(void *s, void *)  { return _ZN5Actor13OnYoshiTryEatEv(s); }
static int  __fastcall ov10_turn_egg(void *s, void *, void *p) { _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int  __fastcall ov10_v50(void *s, void *)    { return _ZN5Actor9Virtual50Ev(s); }
static int  __fastcall ov10_pounded(void *s, void *, void *o) { _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int  __fastcall ov10_atk1(void *s, void *, void *o) { _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int  __fastcall ov10_atk2(void *s, void *, void *o) { _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int  __fastcall ov10_kicked(void *s, void *, void *o) { _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int  __fastcall ov10_pushed(void *s, void *, void *o) { _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int  __fastcall ov10_cannon(void *s, void *, void *o) { _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int  __fastcall ov10_mega(void *s, void *, void *p) { _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int  __fastcall ov10_under(void *s, void *, void *o) { _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int  __fastcall ov10_egg(void *s, void *)    { return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int  __fastcall ov10_kill(void *s, void *)   { _ZN8Platform4KillEv(s); return 0; }

// ---- the state faces --------------------------------------------------------
//
// WHY A FACE AND NOT THE BODY'S OWN ADDRESS. The cell is dereferenced by a real
// C++ `(c->*cell)()`, so MSVC emits a __thiscall through it: `this` in ECX,
// no stack arguments, `this += delta` first. The five matched bodies are plain
// cdecl C taking the pointer as their one stack argument, so they cannot be
// called that way. `int __fastcall f(void *, void *)` is the port's standing
// answer to exactly that call shape -- it reads ECX as its first parameter,
// EDX is the dead second one, and it pops zero stack bytes, which is what a
// zero-argument __thiscall call site expects. Identical to the contract every
// vtable face in port/hal already keeps; the only new thing here is that the
// word is reached through a member-pointer cell instead of a table slot.
static void __fastcall t36_state0(void *s, void *) { func_ov010_02111320((char *)s); }
static void __fastcall t36_state1(void *s, void *) { func_ov010_021112b4((char *)s); }
static void __fastcall t36_state2(void *s, void *) { func_ov010_02111284((int)(size_t)s); }
static void __fastcall t36_state3(void *s, void *) { func_ov010_0211125c((unsigned char *)s); }
static void __fastcall t36_state4(void *s, void *) { func_ov010_0211124c((char *)s); }

// ---- the states seat --------------------------------------------------------
//
// __sinit_ov010_0211203c (already linked, called from hal/actor_overlays.cpp)
// copies five 8-byte records out of the mount into the ROM's bss cell. The mount
// emits those records with the ROM's own DS code addresses in word 0, so the
// copy would hand the dispatcher DS pointers. This seat reads the SAME five
// records, refuses to proceed unless each one still holds exactly the address
// the ROM's relocation names, and writes the host face into the host cell array
// the dispatcher now reads. The verify is the point: a mount pointing at the
// wrong storage would otherwise feed the state machine garbage in silence, the
// hal/actor_classes_ov026.cpp rule.
struct RomPmf { unsigned fn; unsigned adj; };
typedef void(__fastcall *T36Face)(void *, void *);
static const struct {
    const unsigned char *src; unsigned rom; T36Face face; const char *what;
} g_t36_states[5] = {
    {data_ov010_02112a78, 0x02111320u, t36_state0, "state 0 (0x02112a78)"},
    {data_ov010_02112a88, 0x021112b4u, t36_state1, "state 1 (0x02112a88)"},
    {data_ov010_02112a80, 0x02111284u, t36_state2, "state 2 (0x02112a80)"},
    {data_ov010_02112a98, 0x0211125cu, t36_state3, "state 3 (0x02112a98)"},
    {data_ov010_02112a90, 0x0211124cu, t36_state4, "state 4 (0x02112a90)"},
};
/* the ROM's own copy order, read straight off src/__sinit_ov010_0211203c.c:
   v0 <- 0x02112a78, v1 <- 0x02112a88, v2 <- 0x02112a80, v3 <- 0x02112a98,
   v4 <- 0x02112a90. The table above is in cell order, not address order, for
   that reason. */

DSSTATE_BEGIN
static int g_t36_states_seated;
DSSTATE_END

extern "C" void port_trap36_states_seat(void)
{
    if (g_t36_states_seated)
        return;
    g_t36_states_seated = 1;
    for (int i = 0; i < 5; ++i) {
        RomPmf rec;
        std::memcpy(&rec, g_t36_states[i].src, sizeof rec);
        if (rec.fn != g_t36_states[i].rom || rec.adj != 0) {
            std::fprintf(stderr,
                         "FATAL: ov010 TRAP PMF source %s: the mount holds "
                         "%08x/%u, the ROM's own record says %08x/0 -- WRONG "
                         "BYTES\n", g_t36_states[i].what, rec.fn, rec.adj,
                         g_t36_states[i].rom);
            std::abort();
        }
        RomPmf out;
        out.fn = (unsigned)(size_t)g_t36_states[i].face;
        out.adj = 0;
        std::memcpy(&port_trap36_states[i], &out, sizeof out);
    }
}

// ---- TRAP (id 36) -- table 0x02112ae4 ---------------------------------------
static int __fastcall t36_init(void *s, void *)  { return func_ov010_02111654((char *)s); }
static int __fastcall t36_clean(void *s, void *) { return func_ov010_02111554((char *)s); }
static int __fastcall t36_beh(void *s, void *)   { return func_ov010_021115e0(s); }
static int __fastcall t36_ren(void *s, void *)
{ port_actor_render_probe("TRAP", (char *)s + 0x320);
  return func_ov010_021115a8((char *)s); }
static int __fastcall t36_d1(void *s, void *)    { return (int)(size_t)func_ov010_021111a0((int *)s); }
static int __fastcall t36_d0(void *s, void *)    { return (int)(size_t)func_ov010_021111ec((int *)s); }

extern "C" void hal_fill_ov010_trap36_vtable(void)
{
    /* TRAP is Platform-derived (Trap_Spawn runs Platform's C2 and builds a
       Model at +0x320), so the base table has to be live before the first
       instance destructs through it -- the SIGN_POST shape gate 41 already used
       for this class. */
    hal_fill_platform_vtable();
    port_trap36_states_seat();

    void **vt = (void **)data_ov010_02112ae4;
    vt[0]  = (void *)t36_init;
    vt[1]  = (void *)ov10_binit;
    vt[2]  = (void *)ov10_ainit;
    vt[3]  = (void *)t36_clean;
    vt[4]  = (void *)ov10_bclean;
    vt[5]  = (void *)ov10_aclean;
    vt[6]  = (void *)t36_beh;
    vt[7]  = (void *)ov10_bbeh;
    vt[8]  = (void *)ov10_abeh;
    vt[9]  = (void *)t36_ren;
    vt[10] = (void *)ov10_bren;
    vt[11] = (void *)ov10_aren;
    vt[12] = (void *)ov10_pdes;
    vt[13] = (void *)ov10_trap13;
    vt[14] = (void *)ov10_trap14;
    vt[15] = (void *)ov10_heap;
    vt[16] = (void *)t36_d1;
    vt[17] = (void *)t36_d0;
    vt[18] = (void *)ov10_yoshi;
    vt[19] = (void *)ov10_turn_egg;
    vt[20] = (void *)ov10_v50;
    vt[21] = (void *)ov10_pounded;
    vt[22] = (void *)ov10_atk1;
    vt[23] = (void *)ov10_atk2;
    vt[24] = (void *)ov10_kicked;
    vt[25] = (void *)ov10_pushed;
    vt[26] = (void *)ov10_cannon;
    vt[27] = (void *)ov10_mega;
    vt[28] = (void *)ov10_under;
    vt[29] = (void *)ov10_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)ov10_kill;
}
