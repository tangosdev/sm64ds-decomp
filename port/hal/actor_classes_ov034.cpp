// RUN REL0215 WAVE 2 (lane cast-sweep2): OV034'S ONE CLASS (level 26, Tiny-Huge
// Island's CAVE). Overlay 2 of 4 in this lane.
//
// Level 26 spawns 35 actors and skips exactly ONE on tip 3b4863d9a, and that
// one is this class -- so this seat takes level 26 to zero skipped.
//
//   id   name      x on L26  factory        table          width
//   248  WIGGLER    1        Wiggler_Spawn  _ZTV7Wiggler    31
//
// ---- NO IDENTITY SHIFT, AND THE RTTI SAYS SO ------------------------------
//
// Unlike ov032, ov033 and ov035, dsd's names in this overlay are on the right
// class and its inferred vtable width is the right 31. _ZTV7Wiggler is
// 0x021144bc, its typeinfo at vtable[-1] is 0x0211445c and the name string at
// 0x02114478 reads "12daHanachan_c" -- Hanachan is the ROM's own word for the
// Wiggler. Wiggler_Spawn installs 0x021144bc and every _ZN7Wiggler* method sits
// in that table's own slots. Checked against the image, not assumed.
//
// ---- THE MISSING BODY LANDED (run rel0215 lane prop18) ---------------------
//
// _ZN7Wiggler8BehaviorEv (0x02112b5c, 0x6e0 bytes, slot 6) used to be the ONE
// symbol in this overlay with neither a delink block nor a src file (36
// function symbols, 35 with a body). Main matched it -- 36e8cbcce, PR #2008 --
// and lane prop18 propagated src/_ZN7Wiggler8BehaviorEv.c here BY ADDRESS,
// VERBATIM, blob 70a58eb36714d32803108c480fc7a9c9737a288d.
//
// WHAT IT IS, HONESTLY: a DECLARED NONMATCHING DRAFT at div 20 of 440 words.
// Its own banner says so and it is deliberately not enrolled in
// config/arm9/overlays/ov034/delinks.txt. All twenty residual words are
// register/scheduling routing -- sixteen are pure instruction REORDER, where
// our word appears in the cartridge a few words away with the registers
// already right, and the other four are the setup block's 0x1000
// materialisation and one address association. There is no opcode, immediate
// or condition difference anywhere in the body, which is why it is admissible
// as the port's running code while it is not yet admissible as a decomp match.
// Re-measured in this tree at mwccarm 2004/b56 with strict relocs against
// extracted/overlays/overlay_0034.bin at base 0x021111a0: size 0x6e0 exact,
// 20 MISMATCH words, the same twenty.
//
// IT SEATS INTO A PLAIN VTABLE SLOT, AND THAT WAS VERIFIED RATHER THAN
// ASSUMED. The only reference to 0x02112b5c in ov034's relocations is
// `from:0x021144d4 kind:load`, and 0x021144d4 is _ZTV7Wiggler (0x021144bc)
// plus 0x18 -- SLOT 6. No arm_call, no pointer-to-member record, no data-table
// word. So there is no member-pointer dispatch to reconcile here and no
// __fastcall(self, edx) thunk in play: the wg_behavior wrapper below already
// carries MSVC's thiscall across, exactly as it did for the face, and the body
// is a plain cdecl `int (void *)`. (A global `to:0x02112b5c` sweep also hits
// ov025's relocs. That one is data_ov025_02112b5c kind:data(any): ov025 and
// ov034 share the level-overlay base, so it is a different module's own object
// at the same address, not this function.)
//
// WHAT CHANGES AT RUN TIME: the Wiggler was spawning, initialising its five
// segments, rendering and holding state DEMOWAIT forever, because Behavior is
// the only caller of the state machine's tick half. It now runs the ROM's own
// frame -- the tick dispatcher (func_ov034_02112604), the yaw approach toward
// +0x8d8, the JustHitGround landing dust, the footstep sounds, Animation::
// Advance on all five segments, Actor::UpdatePos, Enemy::UpdateWMClsn, the
// segment-follow with its per-segment ground raycast (func_ov034_02112874),
// the damage particle, and the ten collision cylinders cleared and
// re-registered every frame.
//
// ---- THE ELEVEN-STATE PMF TABLE -------------------------------------------
//
// data_ov034_02114538 is eleven TWENTY-byte records: {enter pmf 8, tick pmf 8,
// name pointer 4}. The pmfs are zero in .data and filled by
// __sinit_ov034_021138ec out of twenty-two 8-byte source pairs. The ROM's own
// state names come out of the +0x10 pointers and are quoted in the seat table
// below. Both dispatch sites are host copies
// (port/unmatched/Wiggler_State{Enter,Tick}.cpp) because MSVC's PMF
// representation for an incomplete class is not the ROM's {function, delta}
// pair. The seat VERIFIES all twenty-two ROM words before rewriting the
// function words with host addresses -- the Ukiki / BabyPenguin / MrBlizzard
// shape: a mount pointing at the wrong bytes ABORTS instead of calling into DS
// memory.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED ---------------------------------------
//
// The Wiggler does not derive from Model or ModelAnim -- it HOLDS five
// ModelAnims at +0x110 (stride 0x64) as an array member -- so the default
// applies: ROM-shaped, [16] D1, [17] D0, and no slot 31 because an Enemy is a
// plain Actor and not a Platform. Its Render is the ModelAnim slot-5 COLLISION
// and is the host copy in port/unmatched/Wiggler_Render.cpp.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov033/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern, behind
// one DSSTATE-bracketed done-guard. It runs the pack check, the syms patch,
// hal_fill_enemy_base_vtable (both destructors tail into the Enemy base D2
// func_ov002_020aed18) and then the overlay's single sinit.
#include <cstdio>
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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_enemy_base_vtable(void);            /* hal/actor_classes.cpp */

/* the generated ov034 per-symbol mount (build/port/host-src/ov034_syms.c) */
void port_ov034_pack_check(void);
void port_ov034_syms_patch(void);
/* the overlay's single sinit: thirty-five SharedFilePtrs, thirty-five
   destructor chain nodes and the twenty-two state pmfs. */
void __sinit_ov034_021138ec(void);

/* what port_factory_wiggler spells by hand (src/Wiggler_Spawn.c is held out --
   it rides the ROM's r0 through into Enemy::C2, the ChiefChilly shape) */
void *_ZN9ActorBasenwEj(unsigned int sz);
void _ZN5EnemyC2Ev(void *self);
int func_020733a8(void *p, int n, int stride, void *ctor, void *dtor);
int _ZN12WithMeshClsnC1Ev(void *p);
int _ZN25MovingCylinderClsnWithPosC1Ev(void *p);
int _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
int func_0203d73c(void *p);
int func_02011508(void *p);
int func_0203d384(void *p);
int func_020072c0(void *p);
int _ZN15TextureSequenceC1Ev(void *p);
int _ZN15TextureSequenceD1Ev(void *p);
int _ZN15MaterialChangerC1Ev(void *p);
int _ZN15MaterialChangerD1Ev(void *p);
int _ZN9ModelAnimC1Ev(void *p);
int _ZN9ModelAnimD1Ev(void *p);

/* the class's own bodies */
int *_ZN7WigglerD1Ev(void *self);                 /* slot 16 */
int *_ZN7WigglerD0Ev(void *self);                 /* slot 17 */
int _ZN7Wiggler16CleanupResourcesEv(void);        /* slot 3, a plain .c body */
int _ZN7Wiggler6RenderEv(void *self);             /* slot 9,  HOST COPY */
/* slot 6, propagated from main 36e8cbcce by lane prop18. Its own source
   spells the receiver `void *` and returns int; that is the shape here. */
int _ZN7Wiggler8BehaviorEv(void *self);           /* slot 6,  src/ */

/* the twenty-two state handlers, {enter, tick} per state */
void func_ov034_02112484(void *c); void func_ov034_02112348(void *c);
void func_ov034_02112330(void *c); void func_ov034_02112284(void *c);
void func_ov034_02112270(void *c); void func_ov034_021120ac(void *c);
void func_ov034_02112020(void *c); void func_ov034_02111e68(void *c);
void func_ov034_02111e4c(void *c); void func_ov034_02111c48(void *c);
void func_ov034_02111bb0(void *c); void func_ov034_02111a64(void *c);
void func_ov034_02111a0c(void *c); void func_ov034_021119ac(void *c);
void func_ov034_02111974(void *c); void func_ov034_02111788(void *c);
void func_ov034_02111720(void *c); void func_ov034_021115cc(void *c);
void func_ov034_021115c0(void *c); void func_ov034_02111588(void *c);
void func_ov034_02111520(void *c); void func_ov034_021113d4(void *c);

/* the host vtable, excluded from the mount */
DSSTATE_BEGIN
void *_ZTV7Wiggler[31];        /* 0x021144bc, id 248 WIGGLER */
DSSTATE_END
}

/* THE C++-LINKAGE SPELLINGS. Four of this overlay's //cpp TUs declare their
   data at file scope OUTSIDE decl_common.h's `extern "C"` block, and one
   declares two arm9 FUNCTIONS the same way, so MSVC mangles each with the type
   that TU chose. The mount emits ONE C-named array per symbol and the port
   defines the two functions under their C names, so every mangled spelling is
   bound onto the real one -- the ov030 / ov032 / ov065 / scuttlebug recipe.
   Every LHS here is a mangled name defined nowhere in the link, so no alias
   can be defeated and alternatename_guard stays clean. Read off the linker's
   own LNK2019 lines, not derived by hand. */
#pragma comment(linker, "/alternatename:?data_ov034_021138c4@@3PAHA=_data_ov034_021138c4")
#pragma comment(linker, "/alternatename:?data_ov034_02114488@@3HA=_data_ov034_02114488")
#pragma comment(linker, "/alternatename:?data_ov034_0211389c@@3PAPAPAXA=_data_ov034_0211389c")
#pragma comment(linker, "/alternatename:?data_ov034_02113860@@3PAPAPAXA=_data_ov034_02113860")
#pragma comment(linker, "/alternatename:?_ZN5Actor10PoofDustAtERK7Vector3@@YAXPAXPBUVector3@@@Z=__ZN5Actor10PoofDustAtERK7Vector3")
#pragma comment(linker, "/alternatename:?_ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j@@YAXPAXPACIPBUVector3@@I@Z=__ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j")

/* THE ALIAS THIS MOUNT DEFEATED, RE-ROUTED (T6). ov014, ov015, ov021, ov022 and
   ov034 share the level-overlay base 0x021111a0, and ov034's dsd export won the
   naming race at 0x02114538, so src/_ZN14KnockDownPlank13InitResourcesEv.cpp:30
   (ov015's KNOCK_DOWN_PLANK, LINKED via port/slice_gate59.txt) reaches OV015's
   own collider-file table under ov034's spelling. hal/actor_classes_wf.cpp
   routed that with an /alternatename, and an /alternatename is defeated the
   moment its LHS is DEFINED -- which is what this lane's mount does:
   data_ov034_02114538 is the WIGGLER'S ELEVEN-ENTRY STATE TABLE. Unfixed,
   KNOCK_DOWN_PLANK on level 7 would load its collider file out of the Wiggler's
   state machine. alternatename_guard's post-link map check caught it.
   The reader now compiles with -Ddata_ov034_02114538=port_ov015_kdp_clsn_files
   (a PRIVATE name, because decl_common.h declares both real spellings with
   different basic types and a direct -D is a C2371), and the private name is
   bound here. It is declared nowhere and defined nowhere else, so this alias
   cannot itself be defeated. */
extern "C" char port_ov015_kdp_clsn_files[];
#pragma comment(linker, "/alternatename:_port_ov015_kdp_clsn_files=_data_ov015_02114538")

/* ---- THE NAMING DELTA BETWEEN THE TWO LINES, BRIDGED BY ADDRESS ----------
   run rel0215 lane prop18. src/_ZN7Wiggler8BehaviorEv.c is taken VERBATIM off
   the decomp line and that line has taken a C++ rename this branch has not, so
   six of the names the body spells have no symbol here. They are the SAME SIX
   FUNCTIONS, and the join is the ROM ADDRESS, which is the only thing both
   lines agree on. Every address below is one of the thirty-seven relocations
   the ROM records inside 0x02112b5c..0x0211323c:

     0x02010c30  body `_ZN5Actor9UpdatePosEP5dCc_c`
              -> here `_ZN5Actor9UpdatePosEP12CylinderClsn`
     0x02014ff0  body `_ZN5dCc_c6UpdateEv`
              -> here `_ZN12CylinderClsn6UpdateEv`
     0x02015024  body `_ZN5dCc_c5ClearEv`
              -> here `_ZN12CylinderClsn5ClearEv`
     0x020356e8  body `_ZNK10dBgCh_Actr10IsOnGroundEv`
              -> here `_ZNK12WithMeshClsn10IsOnGroundEv`
     0x0203571c  body `_ZNK10dBgCh_Actr13JustHitGroundEv`
              -> here `_ZNK12WithMeshClsn13JustHitGroundEv`
     0x020aebf8  body `_ZN5Enemy12UpdateWMClsnER10dBgCh_Actrj`   (ov002)
              -> here `_ZN5Enemy12UpdateWMClsnER12WithMeshClsnj`

   Bridged rather than renamed at the source so the two lines reconcile without
   a conflict, which is this tree's standing rule for a propagated body, and
   the rule prop17 followed for ov074's three.

   THE BODY'S EXTERN SET IS A MIX OF THE TWO LINES' SPELLINGS, and that is
   worth writing down because it explains why nobody upstream noticed. It also
   spells `_ZN5Actor11LandingDustEb` (0x0200fc0c) and
   `_ZN9ActorBase18MarkForDestructionEv` (0x02043824), which are THIS
   branch's spellings and do NOT exist on main -- main has
   `_ZN8dActor_c11LandingDustEb` and `_ZN7fBase_c18MarkForDestructionEv`. The
   file is a declared draft that is not enrolled in delinks.txt, so nothing
   links it on the decomp side and the mixture is invisible there. Here it
   means only these six need a bridge; those two resolve directly.

   THE CALLING CONVENTIONS AGREE, CHECKED AGAINST THE DEFINITIONS RATHER THAN
   ASSUMED, because a pure rename is only safe if both sides are the same ABI.
   All six targets are cdecl C names taking the receiver as their FIRST STACK
   ARGUMENT, which is exactly how the propagated body calls them:
     src/_ZN5Actor9UpdatePosEP12CylinderClsn.c        `(struct Actor *, struct CylinderClsn *)`
     src/_ZNK12WithMeshClsn10IsOnGroundEv.c           `(const struct WithMeshClsn *)`
     src/_ZNK12WithMeshClsn13JustHitGroundEv.c        `(const struct WithMeshClsn *)`
     src/_ZN5Enemy12UpdateWMClsnER12WithMeshClsnj.cpp `(struct Enemy *, struct WithMeshClsn *, u32)`
   The two CylinderClsn ones are REAL C++ METHODS in src (`CylinderClsn::Clear`
   and `::Update`, which MSVC mangles and calls __thiscall), so the C name the
   ROM uses is not theirs -- it is the cdecl FACE in hal/method_faces.cpp:125
   and :127, which takes `void *self` and forwards. That is the symbol these
   two aliases land on, and it is the same one twenty-odd other port bodies
   already call with the receiver pushed.

   Every LHS below is DEFINED NOWHERE in this branch -- that is the whole
   reason the bridge exists -- so no alias here can be defeated by a real
   definition, and port/tools/alternatename_guard.py fails the build post-link
   if that ever stops being true. */
#pragma comment(linker, "/alternatename:__ZN5Actor9UpdatePosEP5dCc_c=__ZN5Actor9UpdatePosEP12CylinderClsn")
#pragma comment(linker, "/alternatename:__ZN5dCc_c6UpdateEv=__ZN12CylinderClsn6UpdateEv")
#pragma comment(linker, "/alternatename:__ZN5dCc_c5ClearEv=__ZN12CylinderClsn5ClearEv")
#pragma comment(linker, "/alternatename:__ZNK10dBgCh_Actr10IsOnGroundEv=__ZNK12WithMeshClsn10IsOnGroundEv")
#pragma comment(linker, "/alternatename:__ZNK10dBgCh_Actr13JustHitGroundEv=__ZNK12WithMeshClsn13JustHitGroundEv")
#pragma comment(linker, "/alternatename:__ZN5Enemy12UpdateWMClsnER10dBgCh_Actrj=__ZN5Enemy12UpdateWMClsnER12WithMeshClsnj")

/* The one body src defines as a real C++ method against include/Wiggler.h,
   faced here -- the ov013/ov024/ov025/ov032/ov033/ov035 recipe. Render is NOT
   faced: it is the host copy in port/unmatched/Wiggler_Render.cpp, which
   exports the Itanium name directly. */
#include "Wiggler.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
extern "C" {
int _ZN7Wiggler13InitResourcesEv(void *self)
{ return ((Wiggler *)self)->Wiggler::InitResources(); }
}

// ============================================================================
// THE FACTORY, SPELLED BY HAND -- the ChiefChilly r0 ride-through
// ============================================================================
/* src/Wiggler_Spawn.c is HELD OUT of the slice. It calls `func_020aed98();`
   with NO ARGUMENT because in the ROM the object operator new just returned is
   still in r0 when Enemy::C2 is entered:
       021136b0 bl 0x02043444   ActorBase::operator new(0x8e8)
       021136b4 movs r4, r0
       021136bc bl 0x020aed98   module:overlays(2,7) = ov002 _ZN5EnemyC2Ev
   mwcc reproduced that, so the TU is byte-faithful and unusable under cdecl --
   and the name it spells, func_020aed98, is an arm9-style spelling of an ov002
   symbol that exists nowhere in the link. Exactly the shape
   port/slice_w12.txt holds src/ChiefChilly_Spawn.cpp out for, and this is that
   lane's remedy: the ROM's own sequence with the receiver spelled, every
   offset, count and stride read from the disassembly at 0x021136a4. The
   allocation size 0x8e8 is the ROM's own literal pool word at 0x021137d4. */
extern "C" void *port_factory_wiggler(void)
{
    char *c = (char *)_ZN9ActorBasenwEj(0x8e8);
    if (c) {
        _ZN5EnemyC2Ev(c);                     /* ROM: entered with r0 = c */
        *(void **)c = (void *)_ZTV7Wiggler;
        func_020733a8(c + 0x110, 5, 0x64, (void *)&_ZN9ModelAnimC1Ev,
                      (void *)&_ZN9ModelAnimD1Ev);
        func_020733a8(c + 0x304, 5, 0x14, (void *)&_ZN15MaterialChangerC1Ev,
                      (void *)&_ZN15MaterialChangerD1Ev);
        func_020733a8(c + 0x368, 5, 0x14, (void *)&_ZN15TextureSequenceC1Ev,
                      (void *)&_ZN15TextureSequenceD1Ev);
        func_020733a8(c + 0x3cc, 5, 0xc, (void *)&func_0203d384,
                      (void *)&func_020072c0);
        func_020733a8(c + 0x408, 5, 0xc, (void *)&func_0203d384,
                      (void *)&func_020072c0);
        func_020733a8(c + 0x444, 5, 6, (void *)&func_0203d73c,
                      (void *)&func_02011508);
        func_020733a8(c + 0x478, 5, 0x40,
                      (void *)&_ZN25MovingCylinderClsnWithPosC1Ev,
                      (void *)&_ZN25MovingCylinderClsnWithPosD1Ev);
        func_020733a8(c + 0x5b8, 5, 0x40,
                      (void *)&_ZN25MovingCylinderClsnWithPosC1Ev,
                      (void *)&_ZN25MovingCylinderClsnWithPosD1Ev);
        _ZN12WithMeshClsnC1Ev(c + 0x708);
    }
    return c;
}

// ---- the trap and the ONE missing body ------------------------------------
static void ov34_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov034 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov034 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV34_TRAP(n) \
    static int __fastcall ov34_trap##n(void *s, void *) \
    { ov34_trap_report(s, n); return 0; }
OV34_TRAP(13) OV34_TRAP(14)
#undef OV34_TRAP

/* THE FACE THAT USED TO BE HERE IS GONE. run rel0215 lane prop18.
   `ov34_missing_behavior` was the L2_TRAP-model loud face for
   _ZN7Wiggler8BehaviorEv -- both streams, flushed, with a hit counter, and
   deliberately NOT the port_actor_slot_decline quarantine hook, because
   decline hard-aborts under SM64DS_FAULTS_FATAL and slot 6 runs every frame.
   It returned 1, the ROM body's only return (0x021131dc `mov r0, #1`), so the
   Wiggler stayed alive and inert.
   It is DELETED rather than left wired-but-unreached, and so is its
   `port_ov034_missing_behavior_hits()` accessor, which had no reader anywhere
   in the tree. A FACE IS A TENANT, NOT A FIXTURE -- the same retirement
   prop17 made for port_ov074_state0_tick_face when main matched that body.
   The three ov34_trapNN slots below are a different thing and STAY: they are
   vtable slots the ROM's own table does not fill for this class.
   ov034 now has NO missing body: 36 of 36 function symbols have source. */

static int __fastcall ov34_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov34_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov34_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov34_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov34_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov34_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov34_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov34_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov34_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov34_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov34_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov34_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov34_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov34_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov34_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov34_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov34_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov34_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov34_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov34_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov34_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov34_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ============================================================================
// THE ELEVEN-STATE PMF SEAT
// ============================================================================
/* {the ROM address the sinit's own source pair carries, the host body}. Order
   is the table's own index order, and the ROM column is verified against the
   mounted bytes before the rewrite -- a mount pointing at the wrong bytes
   ABORTS instead of silently calling into DS memory. The names are the ROM's
   own, read out of each entry's +0x10 pointer. */
extern "C" {
struct PortWigglerEntry { unsigned enter_fn, enter_delta, tick_fn, tick_delta, name; };
extern PortWigglerEntry data_ov034_02114538[11];
}
typedef void (*PortWigglerFn)(void *);

static const struct {
    const char *name;
    unsigned enter_rom, tick_rom;
    PortWigglerFn enter_host, tick_host;
} g_wiggler_states[11] = {
    { "DEMOWAIT",     0x02112484, 0x02112348, func_ov034_02112484, func_ov034_02112348 },
    { "DEMOCALL",     0x02112330, 0x02112284, func_ov034_02112330, func_ov034_02112284 },
    { "MOVE_STOP",    0x02112270, 0x021120ac, func_ov034_02112270, func_ov034_021120ac },
    { "MOVE_START",   0x02112020, 0x02111e68, func_ov034_02112020, func_ov034_02111e68 },
    { "MOVE",         0x02111e4c, 0x02111c48, func_ov034_02111e4c, func_ov034_02111c48 },
    { "DAMAGE",       0x02111bb0, 0x02111a64, func_ov034_02111bb0, func_ov034_02111a64 },
    { "DAMAGETALK",   0x02111a0c, 0x021119ac, func_ov034_02111a0c, func_ov034_021119ac },
    { "DEAD",         0x02111974, 0x02111788, func_ov034_02111974, func_ov034_02111788 },
    { "DAMAGE_STOP",  0x02111720, 0x021115cc, func_ov034_02111720, func_ov034_021115cc },
    { "DAMAGE_WAIT",  0x021115c0, 0x02111588, func_ov034_021115c0, func_ov034_02111588 },
    { "DAMAGE_START", 0x02111520, 0x021113d4, func_ov034_02111520, func_ov034_021113d4 },
};

extern "C" void port_wiggler_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 11; ++i) {
        PortWigglerEntry &e = data_ov034_02114538[i];
        if (e.enter_fn != g_wiggler_states[i].enter_rom || e.enter_delta != 0 ||
            e.tick_fn != g_wiggler_states[i].tick_rom || e.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: Wiggler state %d (%s): the sinit left "
                         "%08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         g_wiggler_states[i].name,
                         e.enter_fn, e.enter_delta, e.tick_fn, e.tick_delta,
                         g_wiggler_states[i].enter_rom,
                         g_wiggler_states[i].tick_rom);
            std::abort();
        }
        e.enter_fn = (unsigned)(size_t)g_wiggler_states[i].enter_host;
        e.tick_fn = (unsigned)(size_t)g_wiggler_states[i].tick_host;
    }
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov34_bringup_done;
DSSTATE_END

extern "C" void port_ov34_bringup(void)
{
    if (g_ov34_bringup_done)
        return;
    g_ov34_bringup_done = 1;
    port_ov034_pack_check();
    port_ov034_syms_patch();
    hal_fill_enemy_base_vtable();
    __sinit_ov034_021138ec();
    /* Seat and verify the eleven entries BEFORE anything can dispatch through
       them: Wiggler::InitResources enters state 0 through
       func_ov034_021125b8, which tail-calls its enter half on the same frame. */
    port_wiggler_states_seat();
}

// ============================================================================
// WIGGLER (248) -- _ZTV7Wiggler 0x021144bc, RTTI 12daHanachan_c, 31 slots.
// ============================================================================
//
// The cave's caterpillar. A 2280-byte Enemy built out of FIVE of everything:
// five ModelAnims at +0x110 (stride 0x64), five MaterialChangers at +0x304,
// five TextureSequences at +0x368, five 12-byte records at +0x3cc and +0x408,
// five 6-byte at +0x444 and five MovingCylinderClsnWithPos at +0x478 and
// +0x5b8 -- the head and four body segments -- plus a WithMeshClsn at +0x708.
// The state index is at +0x8c4.
static int __fastcall wg_init(void *s, void *)
{ return _ZN7Wiggler13InitResourcesEv(s); }
static int __fastcall wg_clean(void *s, void *)
{ (void)s; return _ZN7Wiggler16CleanupResourcesEv(); }
/* Slot 6, the propagated body, seated bare. The wrapper is the same
   __fastcall(self, dummy) shape every other slot in this table uses -- MSVC
   calls a vtable entry __thiscall with the receiver in ecx and the dummy edx
   soaks the second register argument -- and the callee is plain cdecl. There
   is nothing extra to do here because the vtable slot is this function's ONLY
   reference in the ROM (relocs.txt: one `kind:load` from 0x021144d4, no
   code-pointer record), so the seat is the whole of its coverage. */
static int __fastcall wg_behavior(void *s, void *)
{ return _ZN7Wiggler8BehaviorEv(s); }
static int __fastcall wg_render(void *s, void *)
{ port_actor_render_probe("WIGGLER", (char *)s + 0x110);
  return _ZN7Wiggler6RenderEv(s); }          /* HOST COPY */
static int __fastcall wg_d1(void *s, void *)
{ return (int)(size_t)_ZN7WigglerD1Ev(s); }
static int __fastcall wg_d0(void *s, void *)
{ return (int)(size_t)_ZN7WigglerD0Ev(s); }

extern "C" void hal_fill_wiggler_vtable(void)
{
    port_ov34_bringup();
    void *volatile *vt = (void *volatile *)_ZTV7Wiggler;
    vt[1]  = (void *)ov34_binit;
    vt[2]  = (void *)ov34_ainit;
    vt[4]  = (void *)ov34_bclean;
    vt[5]  = (void *)ov34_aclean;
    vt[7]  = (void *)ov34_bbeh;
    vt[8]  = (void *)ov34_abeh;
    vt[10] = (void *)ov34_bren;
    vt[11] = (void *)ov34_aren;
    vt[12] = (void *)ov34_pdes;
    vt[13] = (void *)ov34_trap13;
    vt[14] = (void *)ov34_trap14;
    vt[15] = (void *)ov34_heap;
    vt[18] = (void *)ov34_yoshi;
    vt[19] = (void *)ov34_turn_egg;
    vt[20] = (void *)ov34_v50;
    vt[21] = (void *)ov34_pounded;
    vt[22] = (void *)ov34_atk1;
    vt[23] = (void *)ov34_atk2;
    vt[24] = (void *)ov34_kicked;
    vt[25] = (void *)ov34_pushed;
    vt[26] = (void *)ov34_cannon;
    vt[27] = (void *)ov34_mega;
    vt[28] = (void *)ov34_under;
    vt[29] = (void *)ov34_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[0]  = (void *)wg_init;
    vt[3]  = (void *)wg_clean;
    vt[6]  = (void *)wg_behavior;
    vt[9]  = (void *)wg_render;
    vt[16] = (void *)wg_d1;
    vt[17] = (void *)wg_d0;
    /* no slot 31: an Enemy is a plain Actor, 31 slots total, ends at 30. */
}
