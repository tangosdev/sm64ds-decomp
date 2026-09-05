// run rel0215 wave 1, lane cast-ov026: ov026's WHOLE CAST, level 18 (Dire Dire
// Docks, data/stage/water_land). Five classes, five vtables, 49 of the
// overlay's 54 matched TUs. Slice: port/slice_ov026cast.txt. Mount:
// port/ov026_syms.txt (a wave-20 derivation that had never been built; this
// lane wired it and re-verified every width from the ROM before doing so).
//
// ============================ THE FIVE TABLES ===============================
// Every word below was read out of extracted/overlays/overlay_0026.bin -- the
// raw image, never extracted/dsd/arm9_overlays/ (trap T4) -- with the targets
// resolved through config/arm9/symbols.txt and config/arm9/overlays/ov002/
// symbols.txt. The image is 0x2d00 bytes and loads at 0x021111a0, which is
// .text start in delinks.txt and reads e92d4010 (push {r4,lr}) at four known
// entry points.
//
//   slot  89 polelift   91 shutter   90 submarine  246 whirlpool 247 suction
//     0   02111598      021117d8     021119c0      021120ec      021123c8
//     1   Actor::BeforeInitResources  (0x02011268)  -- all five
//     2   Actor::AfterInitResources(u32) (0x02011244) -- all five
//     3   021112e4      02111764     02111954      02111fa4      02112328
//     4   Actor::BeforeCleanupResources  (0x02011220) -- all five
//     5   Actor::AfterCleanupResources(u32) (0x02011214) -- all five
//     6   02111330      021117d0     ActorBase::   0211200c      0211233c
//                                    Behavior
//                                    (0x02043b24)
//     7   Actor::BeforeBehavior (0x02010fd4) -- all five
//     8   Actor::AfterBehavior(u32) (0x02010fc8) -- all five
//     9   02111308      021117a8     02111998      02111fd8      02112334
//    10   Actor::BeforeRender (0x02010f78) -- all five
//    11   Actor::AfterRender(u32) (0x02010f6c) -- all five
//    12   ActorBase::OnPendingDestroy (0x02043ac0) on 89/91/90;
//         own bodies 02111fd4 on 246 and 02112330 on 247
//    13   ActorBase::Virtual34(u32,u32) (0x0204357c) -- all five
//    14   ActorBase::Virtual38(u32,u32) (0x0204349c) -- all five
//    15   ActorBase::OnHeapCreated (0x02043494) -- all five
//    16   021111a0      021116c8     021118b8      02111aa0      021121fc  D1
//    17   021111e0      0211170c     021118fc      02111ad8      02112234  D0
//    18..30  the shared Actor tail, byte-identical in all five tables:
//         02010160 02010154 0201014c 02010148 02010144 02010140 0201013c
//         02010138 02010134 02010130 0201012c 02010124 020100dc
//    31   --            020ee55c     020ee55c     --            --
//                       Platform::Kill (0x020ee55c, ov002)
//
// WIDTHS: 31 / 32 / 32 / 31 / 31. The wave-20 derivation predicted exactly
// these and this lane confirmed every one from the raw image. Two of them are
// over-read by the typeinfo-terminator route and the reason is a POINTER-TO-
// MEMBER SOURCE TABLE packed behind the vtable -- and this lane found that
// wave 20 attributed those tables to the WRONG CLASS:
//
//   * 0x02113cec..0x02113d0c, four {function,0} pairs, sits behind the
//     SUBMARINE's table -- but its four targets 02111d4c / 02111cb4 /
//     02111b24 / 02111ed8 all land inside the WHIRLPOOL's own body range
//     (02111aa0..021121bc), and __sinit_ov026_02112c94 (the whirlpool's own
//     sinit, the one that constructs its model 0x4a9 and anim 0x4a8) is what
//     copies them. They are the WHIRLPOOL's file-scope statics, emitted ahead
//     of the whirlpool's own typeinfo/name/SpawnInfo/vtable group.
//   * 0x02113dd0..0x02113de0, two pairs, sits behind the WHIRLPOOL's table and
//     targets 021122b0 / 021122cc, inside WATERSUCTION's range, copied by
//     __sinit_ov026_02112d68.
//
// The widths are unaffected -- the pairs are not vtable either way -- but who
// owns them decides which class's seat verifies them, so it is corrected here
// and in port/ov026_syms.txt.
//
// ============================ THE CLASS-IDENTITY SWAP =======================
// dsd's METHOD NAMES on this overlay are class-swapped for one class. Every
// `_ZN9Submarine*` TU is the WHIRLPOOL's (18daWater_Tatumaki_c, id 246):
// table 0x02113d54 carries _ZN9Submarine13InitResourcesEv at slot 0,
// _ZN9SubmarineD1Ev at 16 and _ZN9SubmarineD0Ev at 17, and its own vtable[-1]
// __si_class_type_info record's name string reads "18daWater_Tatumaki_c".
// The real Submarine's table is 0x02113c6c ("18daObjWlSubmarine_c"), whose own
// bodies dsd left unnamed. config/arm9/overlays/ov026/symbols.txt:201 puts
// _ZTV9Submarine on 0x02113d54 and THAT ROW IS THE WRONG ONE; the same file's
// row 202 (_ZTV18daWater_Tatumaki_c, same address) is right. 0x02113e24's two
// labels (_ZTV12WaterSuction, _ZTV17daWater_Suikomi_c) are the same class under
// two spellings and both are right.
//
// Consequence for this file: _ZN9Submarine13InitResourcesEv is filled into the
// WHIRLPOOL's slot 0, not the Submarine's, and the Submarine's slot 0 is
// func_ov026_021119c0. Getting that backwards would put a whirlpool's
// ModelAnim/TextureTransformer setup on a Platform-shaped 800-byte object.
//
// ============================ HOST STORAGE ==================================
// All five vtables are HOST ARRAYS (the ov015/ov016/ov022/ov045/ov080 rule: a
// mounted vtable hands a factory DS code addresses) and every dsd symbol inside
// each span is excluded from port/ov026_syms.txt with it. The matched sources
// spell three of the five under TWO names each; the second spelling is routed
// to the host array with a per-source -D in port/CMakeLists.txt rather than an
// /alternatename, because a -D cannot be defeated by a later mount (the
// ov071/ov073 R3 failure).
//
// ============================ THE PLACEHOLDER TRAP ==========================
// Five of the 49 TUs spell shared ROLE NAMES (VT0/VT1/G0/G1) that
// hal/cxx_aliases.cpp has already bound globally: bare G0 -> data_020a0eac (the
// GAME HEAP) and bare G1 -> data_ov002_0210e05c (SignPost's KCL). For the two
// CleanupResources bodies here those bindings are LIVE WRONG-OBJECT BUGS that
// link silently -- the CannonHatch/MetalNet/PathLift shape. Each is routed
// per-source; the two whose target is also declared in include/decl_common.h
// with an incompatible type go through a private name this lane owns, bound
// below with a C-name /alternatename whose LHS exists nowhere else.
//
// ============================ WRONG-OVERLAY CROSSINGS =======================
// ov026 shares the level load window at 0x021111a0 with ov014/ov015/ov016/
// ov020/ov021/ov022/ov029/ov032/ov034/ov036/ov045/ov053, and dsd raced the
// name on FOUR references inside ov026's own sources. All four are routed with
// per-source -D renames in port/CMakeLists.txt; one of them was already a live
// hazard in the shipping binary:
//
//   src/func_ov026_021122b0.c calls `__sinit_ov045_02112280` where ov026's own
//   func_ov026_02112280 is meant -- and __sinit_ov045_02112280 IS ALREADY
//   DEFINED in walk_window, by ov013_syms.c.obj (a mounted DATA object at that
//   shared address). An /alternatename would have been defeated silently and
//   the whirlpool's... no: WaterSuction's tick handler would have jumped into
//   mounted data. Measured in the baseline map before this lane changed
//   anything.
//
// The other three are undefined today and renamed anyway for the same reason:
//   src/func_ov026_02111598.cpp  data_ov032_02113a9c -> ov026's own
//   src/func_ov026_02111598.cpp  func_ov053_021112a4 -> ov026's own
//   src/__sinit_ov026_02112d68.c data_ov036_02113f58 -> ov026's own bss cell
//
// ============================ WHAT IS AND IS NOT TRAPPED ====================
// Slots 13, 14 and 30 are trapped by name rather than seated, the ccm/ov045/
// ov072 convention: 13/14 are ActorBase::Virtual34/Virtual38, two u32 arguments
// each, whose solid-heap bodies are not linked into the port at all, and 30 is
// ActorBase-side SRET whose hidden return pointer no thunk shape here models.
// Every other slot in every one of the five tables is seated.

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
#include "Submarine.h"
#include "WaterSuction.h"

// ---- the two private names the G0/G1 routing needs -------------------------
// include/decl_common.h:244-247 declares data_ov026_02113ebc / _02113eb4 /
// _02113edc / _02113ee4 as `char[]` while the two CleanupResources TUs declare
// their placeholder as `extern int G0[];`, so `-DG0=data_ov026_02113ebc` is a
// C2371 redeclaration (the ov006 Mg3DEsp measurement). The PathLift/Coffin
// remedy instead: rename to a name this lane owns and bind it here. Each LHS is
// undefined everywhere else in the link, so the alias cannot be defeated.
#pragma comment(linker, "/alternatename:_port_ov026_shutter_bmd=_data_ov026_02113ebc")
#pragma comment(linker, "/alternatename:_port_ov026_shutter_kcl=_data_ov026_02113eb4")
#pragma comment(linker, "/alternatename:_port_ov026_submarine_bmd=_data_ov026_02113ee4")
#pragma comment(linker, "/alternatename:_port_ov026_submarine_kcl=_data_ov026_02113edc")
// And the polelift's own collision vector, for the same reason one address
// over: decl_common.h declares data_ov026_02113a9c as `Vector3` while
// src/func_ov026_02111598.cpp declares its (wrong-overlay-spelled) reference as
// a local `struct V3`.
#pragma comment(linker, "/alternatename:_port_ov026_polelift_clsn=_data_ov026_02113a9c")

// ---- four C++-MANGLED REFERENCES, routed onto their C definitions ----------
// Measured from the link, not predicted: these four were walk_window's entire
// unresolved list once the slice compiled, and the linker printed both
// spellings of the three data ones itself ("Hint on symbols that are defined
// and could potentially match: _data_ov026_02113f0c").
//
// src/_ZN9Submarine13InitResourcesEv.cpp declares its three mount references
// OUTSIDE its own `extern "C"` block -- `extern struct FilePtr4
// data_ov026_02113f0c;` and friends -- so MSVC emits C++-decorated names for
// storage the mount defines with C linkage. The cxx_aliases.cpp idiom: alias
// the decorated spelling onto the C one, one indirection, no source edit. The
// alias cannot be defeated: nothing else in the link defines the decorated
// name, and the C name it points at is the mount's own array.
#pragma comment(linker, "/alternatename:?data_ov026_02113f0c@@3UFilePtr4@@A=_data_ov026_02113f0c")
#pragma comment(linker, "/alternatename:?data_ov026_02113f04@@3UFilePtr4@@A=_data_ov026_02113f04")
#pragma comment(linker, "/alternatename:?data_ov026_02112f40@@3UBTA_File@@A=_data_ov026_02112f40")
//
// src/func_ov026_02111d4c.cpp (the whirlpool's state-0 tick) declares a local
// shadow `struct Actor { Player *ClosestPlayer(); int HorzAngleToCPlayer(); };`
// -- and gets the RETURN TYPE wrong: the real method returns s16
// (include/Actor.h:189, defined as `short Actor::HorzAngleToCPlayer()` in
// hal/reverse_bridges.cpp:162), so the shadow's `int` mangles to ...@@QAEHXZ
// where the definition is ...@@QAEFXZ. Both decorated spellings were read out
// of the built objects, not guessed. Routed with the actor_classes_montymole
// remedy (its ClosestPlayer row is the same shape).
// SAFE ON THE WIDTH: the ROM body returns the angle in the low halfword and
// this caller's only use of it is `*(s16 *)(player + 0x8e) = HorzAngle...() +
// 0x8000`, which truncates to s16 regardless of what the upper half of eax
// holds. The other two shadow methods in that file (Actor::ClosestPlayer,
// Player::EnterWhirlpool) declare compatible shapes and resolve on their own.
#pragma comment(linker, "/alternatename:?HorzAngleToCPlayer@Actor@@QAEHXZ=?HorzAngleToCPlayer@Actor@@QAEFXZ")

extern "C" {

// ---- the shared arm9 halves -------------------------------------------------
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

/* the generated ov026 per-symbol mount (build/host-src/ov026_syms.c) */
void port_ov026_pack_check(void);
void port_ov026_syms_patch(void);

// ---- the five vtables, host arrays -----------------------------------------
int _ZTV17daObjWlPolelift_c[31];      /* vtspan: data_ov026_02113ae0 */
int _ZTV21daObjWlKoopaShutter_c[32];  /* vtspan: data_ov026_02113ba4 */
int _ZTV18daObjWlSubmarine_c[32];     /* vtspan: data_ov026_02113c6c */
int _ZTV18daWater_Tatumaki_c[31];     /* vtspan: _ZTV18daWater_Tatumaki_c */
int _ZTV12WaterSuction[31];           /* vtspan: _ZTV12WaterSuction */

// ---- the class bodies -------------------------------------------------------
/* id 89, daObjWlPolelift_c -- all seven flat C/C++ bodies, no faces needed */
int func_ov026_02111598(char *self);       /* slot 0  InitResources */
int func_ov026_021112e4(void);             /* slot 3  CleanupResources, (void) */
int func_ov026_02111330(char *self);       /* slot 6  Behavior */
int func_ov026_02111308(void *self);       /* slot 9  Render (plain Model shadow) */
int *func_ov026_021111a0(int *self);       /* slot 16 D1 */
int *func_ov026_021111e0(int *self);       /* slot 17 D0 */
void *func_ov026_02111678(void);           /* factory, installs the table */

/* id 91, daObjWlKoopaShutter_c */
int func_ov026_021117d8(char *self);       /* slot 0  */
int func_ov026_02111764(void *self);       /* slot 3  */
int func_ov026_021117d0(void);             /* slot 6  Behavior, (void) */
int func_ov026_021117a8(void *self);       /* slot 9  Render (plain Model shadow) */
int *func_ov026_021116c8(int *self);       /* slot 16 D1 */
int *func_ov026_0211170c(int *self);       /* slot 17 D0 */
void *BowserShutter_Spawn(void);
extern unsigned char BowserShutter_SpawnInfo[];   /* ov026 0x02113b80 */

/* id 90, daObjWlSubmarine_c -- NO own Behavior */
int func_ov026_021119c0(char *self);       /* slot 0  */
int func_ov026_02111954(void *self);       /* slot 3  */
int func_ov026_02111998(void *self);       /* slot 9  Render (plain Model shadow) */
int *func_ov026_021118b8(int *self);       /* slot 16 D1 */
int *func_ov026_021118fc(int *self);       /* slot 17 D0 */
void *Submarine_Spawn(void);
extern unsigned char Submarine_SpawnInfo[];       /* ov026 0x02113c48 */

/* id 246, daWater_Tatumaki_c -- the `_ZN9Submarine*` names are ITS methods */
int _ZN9Submarine13InitResourcesEv(void *self);    /* slot 0, FACED below     */
int _ZN9Submarine16CleanupResourcesEv(void);       /* slot 3, .c body, (void) */
int _ZN9Submarine8BehaviorEv(void *self);          /* slot 6, HOST COPY       */
int _ZN9Submarine6RenderEv(void *self);            /* slot 9, HOST COPY       */
void _ZN9Submarine16OnPendingDestroyEv(void);      /* slot 12, own, empty     */
int *_ZN9SubmarineD1Ev(int *self);                 /* slot 16 D1              */
int *_ZN9SubmarineD0Ev(int *self);                 /* slot 17 D0              */
void *Whirlpool_Spawn(void);
extern unsigned char Whirlpool_SpawnInfo[];        /* ov026 0x02113d30 */

/* id 247, daWater_Suikomi_c */
int _ZN12WaterSuction13InitResourcesEv(void *self); /* slot 0, FACED below     */
int _ZN12WaterSuction16CleanupResourcesEv(void);    /* slot 3, .c body, (void) */
int _ZN12WaterSuction8BehaviorEv(void *self);       /* slot 6, HOST COPY       */
int _ZN12WaterSuction6RenderEv(void);               /* slot 9, .c body, (void) */
void _ZN12WaterSuction16OnPendingDestroyEv(void);   /* slot 12, own, empty     */
int *_ZN12WaterSuctionD1Ev(int *self);              /* slot 16 D1              */
int *_ZN12WaterSuctionD0Ev(int *self);              /* slot 17 D0              */
void *WaterSuction_Spawn(void);
extern unsigned char WaterSuction_SpawnInfo[];      /* ov026 0x02113e00 */

/* THE SIX PMF SOURCE PAIRS, the Scuttlebug/ov070 treatment: seated with host
   bodies BEFORE the sinits copy them into bss, so the mounted romdata never
   holds a DS code address the dispatchers could reach and the destination
   cells are right by construction rather than by a second pass.
   Each is an Itanium {function, adjustment} record, 8 bytes, adjustment 0 --
   ROM-byte verified against overlay_0026.bin before the rewrite. */
struct PortOv026Pmf { unsigned fn, adj; };
extern PortOv026Pmf data_ov026_02113cec;   /* whirlpool state 0 tick  02111d4c */
extern PortOv026Pmf data_ov026_02113cf4;   /* whirlpool state 1 enter 02111cb4 */
extern PortOv026Pmf data_ov026_02113cfc;   /* whirlpool state 1 tick  02111b24 */
extern PortOv026Pmf data_ov026_02113d04;   /* whirlpool state 0 enter 02111ed8 */
extern PortOv026Pmf data_ov026_02113dd0;   /* suction   state 0 tick  021122b0 */
extern PortOv026Pmf data_ov026_02113dd8;   /* suction   state 0 enter 021122cc */

/* the six handler bodies those pairs point at -- all matched src in the slice */
int func_ov026_02111ed8(void);             /* whirlpool state 0 enter (returns 1) */
int func_ov026_02111d4c(char *self);       /* whirlpool state 0 tick  */
int func_ov026_02111cb4(char *self);       /* whirlpool state 1 enter */
int func_ov026_02111b24(char *self);       /* whirlpool state 1 tick  */
int func_ov026_021122cc(void);             /* suction state 0 enter (returns 1) */
int func_ov026_021122b0(void *self);       /* suction state 0 tick    */

}  // extern "C"

// ---- the traps --------------------------------------------------------------
static void ov26_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov026 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov026 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV26_TRAP(n) \
    static int __fastcall ov26_trap##n(void *s, void *) \
    { ov26_trap_report(s, n); return 0; }
OV26_TRAP(13) OV26_TRAP(14)
#undef OV26_TRAP

// ---- the shared 1..30 half --------------------------------------------------
static int __fastcall ov26_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov26_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov26_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov26_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov26_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov26_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov26_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov26_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov26_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov26_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov26_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov26_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov26_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov26_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov26_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov26_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov26_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov26_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov26_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov26_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov26_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov26_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail -- 91 and 90 take it unchanged, 89/246/247 have
   no slot 31 at all. */
static int __fastcall ov26_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- every word the five
   tables share. The caller writes its own 0/3/6/9/16/17, its own 12 where it
   has one, and its own 31.
//
   SLOT NUMBERING IS THE ROM'S, from the five reloc spans quoted in this file's
   header, not from the order the bodies happen to be declared in.
//
   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug (repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp): MSVC 19.44 x86 /O2 can
   delete a static filler's stores when it is called with several distinct
   extern-array arguments. This helper has five callers. */
static void ov26_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov26_binit;     /* Actor::BeforeInitResources    */
    vt[2]  = (void *)ov26_ainit;     /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)ov26_bclean;    /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov26_aclean;    /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ov26_bbeh;      /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov26_abeh;      /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)ov26_bren;      /* Actor::BeforeRender */
    vt[11] = (void *)ov26_aren;      /* Actor::AfterRender(u32) */
    vt[12] = (void *)ov26_pdes;      /* ActorBase::OnPendingDestroy; 246/247
                                        override with their own after this */
    vt[13] = (void *)ov26_trap13;    /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov26_trap14;    /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov26_heap;      /* ActorBase::OnHeapCreated */
    vt[18] = (void *)ov26_yoshi;     /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov26_turn_egg;  /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov26_v50;       /* Actor::Virtual50 */
    vt[21] = (void *)ov26_pounded;   /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov26_atk1;      /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov26_atk2;      /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov26_kicked;    /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov26_pushed;    /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov26_cannon;    /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov26_mega;      /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov26_under;     /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov26_aimed;     /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;    /* Actor::OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// THE SIX PMF SOURCE PAIRS -- seated before the sinits copy them.
// ============================================================================
// The two dispatchers (Whirlpool_StateDispatch.cpp, WaterSuction_StateDispatch.cpp
// and the two Behavior host copies) read these cells as PLAIN FUNCTION POINTERS
// at +0 of each 8-byte record, so the records have to hold HOST code, not the
// ROM's DS addresses. Seating the SOURCE rather than the destination is the
// ov063/ov070 order: it runs before __sinit_ov026_02112c94 / _02112d68 copy the
// pairs into bss, so both ends are right after one pass, and the mounted
// romdata stops carrying reachable DS code pointers (trap T2).
//
// WRONG BYTES ABORTS. A mount pointing at the wrong storage would otherwise
// hand the state machine garbage silently.
typedef int (*PortOv026Fn)(void *);
static const struct {
    PortOv026Pmf *cell; unsigned rom; PortOv026Fn host; const char *what;
} g_ov026_pmf[6] = {
    {&data_ov026_02113d04, 0x02111ed8, (PortOv026Fn)func_ov026_02111ed8,
     "whirlpool state 0 enter"},
    {&data_ov026_02113cec, 0x02111d4c, (PortOv026Fn)func_ov026_02111d4c,
     "whirlpool state 0 tick"},
    {&data_ov026_02113cf4, 0x02111cb4, (PortOv026Fn)func_ov026_02111cb4,
     "whirlpool state 1 enter"},
    {&data_ov026_02113cfc, 0x02111b24, (PortOv026Fn)func_ov026_02111b24,
     "whirlpool state 1 tick"},
    {&data_ov026_02113dd8, 0x021122cc, (PortOv026Fn)func_ov026_021122cc,
     "watersuction state 0 enter"},
    {&data_ov026_02113dd0, 0x021122b0, (PortOv026Fn)func_ov026_021122b0,
     "watersuction state 0 tick"},
};

extern "C" void port_ov026_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 6; ++i) {
        PortOv026Pmf &c = *g_ov026_pmf[i].cell;
        if (c.fn != g_ov026_pmf[i].rom || c.adj != 0) {
            std::fprintf(stderr, "FATAL: ov026 PMF source pair %d (%s): the "
                         "mount holds %08x/%u, the ROM's own record says "
                         "%08x/0 -- WRONG BYTES\n", i, g_ov026_pmf[i].what,
                         c.fn, c.adj, g_ov026_pmf[i].rom);
            std::abort();
        }
        c.fn = (unsigned)(size_t)g_ov026_pmf[i].host;
    }
}

// ============================================================================
// id 89 -- daObjWlPolelift_c, 31 slots, plain Actor.
// Own overrides: 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 16 D1, 17 D0.
// NO own slot 12 (its table carries ActorBase::OnPendingDestroy) and no
// slot 31 -- Actor-derived, the virtual list ends at 30.
// ============================================================================
static int __fastcall pl_init(void *s, void *)
{ return func_ov026_02111598((char *)s); }
static int __fastcall pl_clean(void *s, void *)
{ (void)s; return func_ov026_021112e4(); }
static int __fastcall pl_behavior(void *s, void *)
{ return func_ov026_02111330((char *)s); }
static int __fastcall pl_render(void *s, void *)
{ port_actor_render_probe("POLE_LIFT_DDD", (char *)s + 0xd4);
  return func_ov026_02111308(s); }
static int __fastcall pl_d1(void *s, void *)
{ return (int)(size_t)func_ov026_021111a0((int *)s); }
static int __fastcall pl_d0(void *s, void *)
{ return (int)(size_t)func_ov026_021111e0((int *)s); }

extern "C" void hal_fill_ov026_polelift_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV17daObjWlPolelift_c;
    ov26_fill_shared(vt);
    vt[0]  = (void *)pl_init;
    vt[3]  = (void *)pl_clean;
    vt[6]  = (void *)pl_behavior;
    vt[9]  = (void *)pl_render;
    vt[16] = (void *)pl_d1;
    vt[17] = (void *)pl_d0;
    /* no slot 31: plain Actor, 31 slots total, ends here */
}

// ============================================================================
// id 91 -- daObjWlKoopaShutter_c / BowserShutter, 32 slots, Platform-derived.
// Own overrides: 0, 3, 6, 9, 16, 17. Slot 31 is Platform::Kill unchanged.
// ============================================================================
static int __fastcall ks_init(void *s, void *)
{ return func_ov026_021117d8((char *)s); }
static int __fastcall ks_clean(void *s, void *)
{ return func_ov026_02111764(s); }
static int __fastcall ks_behavior(void *s, void *)
{ (void)s; return func_ov026_021117d0(); }
static int __fastcall ks_render(void *s, void *)
{ port_actor_render_probe("BOWSER_SHUTTER", (char *)s + 0xd4);
  return func_ov026_021117a8(s); }
static int __fastcall ks_d1(void *s, void *)
{ return (int)(size_t)func_ov026_021116c8((int *)s); }
static int __fastcall ks_d0(void *s, void *)
{ return (int)(size_t)func_ov026_0211170c((int *)s); }

extern "C" void hal_fill_ov026_bowser_shutter_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV21daObjWlKoopaShutter_c;
    ov26_fill_shared(vt);
    vt[0]  = (void *)ks_init;
    vt[3]  = (void *)ks_clean;
    vt[6]  = (void *)ks_behavior;
    vt[9]  = (void *)ks_render;
    vt[16] = (void *)ks_d1;
    vt[17] = (void *)ks_d0;
    vt[31] = (void *)ov26_kill;      /* Platform::Kill */
}

// ============================================================================
// id 90 -- daObjWlSubmarine_c / Submarine, 32 slots, Platform-derived.
// Own overrides: 0, 3, 9, 16, 17. NO own Behavior -- slot 6 in the ROM table is
// ActorBase::Behavior (0x02043b24), and the shared fill does not write 6, so
// this fill writes the ActorBase default explicitly rather than leaving a hole.
// Slot 31 is Platform::Kill.
// ============================================================================
static int __fastcall sb_init(void *s, void *)
{ return func_ov026_021119c0((char *)s); }
static int __fastcall sb_clean(void *s, void *)
{ return func_ov026_02111954(s); }
static int __fastcall sb_behavior(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Behavior(); }
static int __fastcall sb_render(void *s, void *)
{ port_actor_render_probe("SUBMARINE", (char *)s + 0xd4);
  return func_ov026_02111998(s); }
static int __fastcall sb_d1(void *s, void *)
{ return (int)(size_t)func_ov026_021118b8((int *)s); }
static int __fastcall sb_d0(void *s, void *)
{ return (int)(size_t)func_ov026_021118fc((int *)s); }

extern "C" void hal_fill_ov026_submarine_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV18daObjWlSubmarine_c;
    ov26_fill_shared(vt);
    vt[0]  = (void *)sb_init;
    vt[3]  = (void *)sb_clean;
    vt[6]  = (void *)sb_behavior;    /* ActorBase::Behavior, the ROM's own word */
    vt[9]  = (void *)sb_render;
    vt[16] = (void *)sb_d1;
    vt[17] = (void *)sb_d0;
    vt[31] = (void *)ov26_kill;      /* Platform::Kill */
}

// ============================================================================
// id 246 -- daWater_Tatumaki_c / Whirlpool, 31 slots, Enemy-derived.
// Own overrides: 0, 3, 6, 9, 12 (own empty body), 16, 17. No slot 31.
// Slot 6 (Behavior) and slot 9 (Render) are HOST COPIES; see this file's
// header and port/slice_ov026cast.txt.
// ============================================================================
static int __fastcall wp_init(void *s, void *)
{ return _ZN9Submarine13InitResourcesEv(s); }
static int __fastcall wp_clean(void *s, void *)
{ (void)s; return _ZN9Submarine16CleanupResourcesEv(); }
static int __fastcall wp_behavior(void *s, void *)
{ return _ZN9Submarine8BehaviorEv(s); }
static int __fastcall wp_render(void *s, void *)
{ port_actor_render_probe("WHIRLPOOL", (char *)s + 0x114);
  return _ZN9Submarine6RenderEv(s); }
static int __fastcall wp_pdes(void *s, void *)
{ (void)s; _ZN9Submarine16OnPendingDestroyEv(); return 0; }
static int __fastcall wp_d1(void *s, void *)
{ return (int)(size_t)_ZN9SubmarineD1Ev((int *)s); }
static int __fastcall wp_d0(void *s, void *)
{ return (int)(size_t)_ZN9SubmarineD0Ev((int *)s); }

extern "C" void hal_fill_ov026_whirlpool_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV18daWater_Tatumaki_c;
    ov26_fill_shared(vt);
    vt[0]  = (void *)wp_init;
    vt[3]  = (void *)wp_clean;
    vt[6]  = (void *)wp_behavior;
    vt[9]  = (void *)wp_render;
    vt[12] = (void *)wp_pdes;        /* own, empty; overrides the shared default */
    vt[16] = (void *)wp_d1;
    vt[17] = (void *)wp_d0;
    /* no slot 31: Enemy-derived, the virtual list ends at Actor's 30 */
}

// ============================================================================
// id 247 -- daWater_Suikomi_c / WaterSuction, 31 slots, Enemy-derived.
// Own overrides: 0, 3, 6, 9, 12, 16, 17. No slot 31.
// NOT PLACED ON LEVEL 18. It is registered so the overlay's cast is whole and
// its sinit's work is reachable; its proof is no-regression, not a census move.
// ============================================================================
static int __fastcall ws_init(void *s, void *)
{ return _ZN12WaterSuction13InitResourcesEv(s); }
static int __fastcall ws_clean(void *s, void *)
{ (void)s; return _ZN12WaterSuction16CleanupResourcesEv(); }
static int __fastcall ws_behavior(void *s, void *)
{ return _ZN12WaterSuction8BehaviorEv(s); }
static int __fastcall ws_render(void *s, void *)
{ (void)s; return _ZN12WaterSuction6RenderEv(); }
static int __fastcall ws_pdes(void *s, void *)
{ (void)s; _ZN12WaterSuction16OnPendingDestroyEv(); return 0; }
static int __fastcall ws_d1(void *s, void *)
{ return (int)(size_t)_ZN12WaterSuctionD1Ev((int *)s); }
static int __fastcall ws_d0(void *s, void *)
{ return (int)(size_t)_ZN12WaterSuctionD0Ev((int *)s); }

extern "C" void hal_fill_ov026_water_suction_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV12WaterSuction;
    ov26_fill_shared(vt);
    vt[0]  = (void *)ws_init;
    vt[3]  = (void *)ws_clean;
    vt[6]  = (void *)ws_behavior;
    vt[9]  = (void *)ws_render;
    vt[12] = (void *)ws_pdes;
    vt[16] = (void *)ws_d1;
    vt[17] = (void *)ws_d0;
    /* no slot 31: Enemy-derived, the virtual list ends at Actor's 30 */
}

// ---- method faces -----------------------------------------------------------
// _ZN9Submarine13InitResourcesEv.cpp and _ZN12WaterSuction13InitResourcesEv.cpp
// are real MSVC methods against Submarine.h / WaterSuction.h, so their compiled
// names are ?InitResources@Submarine@@... and not the Itanium spelling the
// vtable fills above call. Faced here, the IceSheet/OneUpLogo/BabyPenguin
// recipe. Everything else in this cast is a plain C-linkage body and is called
// directly.
extern "C" {
int _ZN9Submarine13InitResourcesEv(void *self)
{ return ((Submarine *)self)->Submarine::InitResources(); }
int _ZN12WaterSuction13InitResourcesEv(void *self)
{ return ((WaterSuction *)self)->WaterSuction::InitResources(); }
}
