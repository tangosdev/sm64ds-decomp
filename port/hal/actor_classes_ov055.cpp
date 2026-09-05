// RUN REL0215 WAVE 2 (lane cast-sweep2): OV055'S ONE CLASS (level 47, Luigi's
// key arena). Overlay 4 of 4 in this lane.
//
// Level 47 spawns 18 actors and skips exactly ONE on tip 3b4863d9a, and that
// one is this class -- so this seat takes level 47 to zero skipped.
//
//   id   name          x on L47  factory            table              width
//   195  MIRROR_LUIGI   1        MirrorLuigi_Spawn  _ZTV11MirrorLuigi   31
//
// ---- NO IDENTITY SHIFT, AND THE RTTI SAYS SO ------------------------------
//
// dsd's names here are on the right class. _ZTV11MirrorLuigi is 0x02111ae0, its
// typeinfo at vtable[-1] is 0x02111ab0 and the name string at 0x02111aa4 reads
// "9daLuigi_c". Its base typeinfo is 0x0208e390, which is ACTOR's -- this is a
// direct Actor subclass, not a Platform, which is why the table is 31 slots and
// 0x02111b5c holds a literal ZERO rather than Platform::Kill (the ov024
// PYRAMID_TAG / ov058 RECROOMCUPBOARD shape). dsd's next-symbol landing reads
// FIVE.
//
// SEVEN own slots, one more than usual: 12 OnPendingDestroy is a four-byte body
// (0x02111354) that overrides ActorBase's. Letting the shared half write the
// default there would have changed the answer with no fault and no log line.
//
// ---- THE ONE-CELL POINTER-TO-MEMBER MACHINE -------------------------------
//
// The smallest instance in the tree of the machine ov030, ov032 and ov034 all
// carry: ONE sixteen-byte cell at data_ov055_02111b70, built by
// __sinit_ov055_021118d4 out of two 8-byte {function, delta} pairs.
//   ENTER  data_ov055_02111a94 -> func_ov055_021112bc, delta 0  (`return 1`)
//   TICK   data_ov055_02111a9c -> func_ov055_02111288, delta 0
// MirrorLuigi::InitResources installs the cell through func_ov055_021112c4
// (host copy) and that tail-calls the enter half; MirrorLuigi::Behavior (host
// copy) dispatches the tick half at cell+8 every frame with the player pointer.
// The tick is the class: it copies the player's position with X negated and the
// Y angle negated, which is what makes this a MIRROR Luigi. The seat VERIFIES
// all four ROM words in the mounted cell before rewriting the two function
// words with host addresses -- the Ukiki / BabyPenguin / MrBlizzard shape.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED ---------------------------------------
//
// MirrorLuigi does not derive from Model or ModelAnim -- it HOLDS a ModelAnim
// at +0xd4, a Model at +0x138, a ShadowModel at +0x188 and two
// TextureSequences at +0x1b0 -- so the default applies: ROM-shaped, [16] D1,
// [17] D0, no slot 31. Render's `((Sub *)&mModel)->m5(0)` dispatches slot 5
// over the PLAIN MODEL at +0x138 (ROM 0x021114cc), and _ZTV5Model[5] is
// dual-filled in hal/cxxname_bridge.cpp, so it is the Tree/ov013 case and NOT
// the ModelAnim slot-5 collision -- Render rides from src unchanged. The
// ModelAnim at +0xd4 is drawn through a direct Model::Render call.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov032/ov033/ov034/ov035/ov045/ov047/ov052/ov056/ov073
// lane-ownership pattern, behind one DSSTATE-bracketed done-guard: the pack
// check, the syms patch, the overlay's single sinit, then the cell seat.
// hal_fill_platform_vtable is NOT called -- nothing here is a Platform.
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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* the generated ov055 per-symbol mount (build/port/host-src/ov055_syms.c) */
void port_ov055_pack_check(void);
void port_ov055_syms_patch(void);
/* the overlay's single sinit: it builds the sixteen-byte state cell and
   nothing else. */
void __sinit_ov055_021118d4(void);

/* what ml_d1 spells by hand (src/_ZN11MirrorLuigiD1Ev.cpp is held out -- it is
   the //cpp real-destructor shape, the ov024 PyramidTag case) */
int __destroy_arr(void *base, int n, int stride, void *dtor);
void _ZN15TextureSequenceD1Ev(void *p);
void _ZN11ShadowModelD1Ev(void *p);
void _ZN5ModelD1Ev(void *p);
void _ZN9ModelAnimD1Ev(void *p);
void _ZN5ActorD2Ev(void *p);

/* the class's own bodies */
void *MirrorLuigi_Spawn(void);                       /* id 195 */
int *_ZN11MirrorLuigiD0Ev(void *self);               /* slot 17 */
int _ZN11MirrorLuigi16CleanupResourcesEv(void);      /* slot 3,  a plain .c body */
void _ZN11MirrorLuigi16OnPendingDestroyEv(void);     /* slot 12, a plain .c body */
int _ZN11MirrorLuigi8BehaviorEv(void *self);         /* slot 6,  HOST COPY */

/* the two state halves the seat rewrites into the cell */
int func_ov055_021112bc(void);                       /* ENTER */
int func_ov055_02111288(void *dst, void *src);       /* TICK  */

/* the host vtable, excluded from the mount */
DSSTATE_BEGIN
void *_ZTV11MirrorLuigi[31];   /* 0x02111ae0, id 195 MIRROR_LUIGI */
DSSTATE_END
}

/* THE C++-LINKAGE DATA SPELLINGS. MirrorLuigi::InitResources is a //cpp TU
   whose `extern void *data_ov002_...;` declarations sit at file scope OUTSIDE
   decl_common.h's `extern "C"` block, so MSVC mangles each with the type that
   TU chose. ov002's mount emits ONE C-named array per symbol, so every mangled
   spelling is bound onto it -- the ov030 / ov032 / ov034 / ov047 recipe. Every
   LHS is a mangled name defined nowhere in the link, so no alias can be
   defeated. Read off the linker's own LNK2019 lines, not derived by hand. */
#pragma comment(linker, "/alternatename:?data_ov002_0210e8d0@@3PAXA=_data_ov002_0210e8d0")
#pragma comment(linker, "/alternatename:?data_ov002_0210eb20@@3PAXA=_data_ov002_0210eb20")
#pragma comment(linker, "/alternatename:?data_ov002_0210ebb8@@3PAXA=_data_ov002_0210ebb8")
#pragma comment(linker, "/alternatename:?data_ov002_0210ebd8@@3PAXA=_data_ov002_0210ebd8")

/* FIVE FRIENDLY SPELLINGS OF ARM9 METHODS, and this is the whole of ov055's
   measured closure gap. MirrorLuigi::InitResources calls the arm9 model and
   texture-sequence methods by their Class_Method names, which
   include/decl_common.h declares (lines 408, 1930, 1960, 1961, 2966) and
   nothing defines. Each ROM arm_call resolves to the Itanium symbol on the
   right, read out of config/arm9/overlays/ov055/relocs.txt:
     0x02015ebc  _ZN11ShadowModel12InitCylinderEv
     0x02016748  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj
     0x0201597c  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File
     0x020159ac  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj
     0x0201794c  _ZN9Animation8LoadFileER13SharedFilePtr
   ALL FIVE MATCHED TUs ARE ALREADY ON A SLICE (port/slice_gate7.txt and
   port/slice_gate10.txt) and were being compiled and then dropped: nothing in
   the link referenced them, so /OPT:REF stripped all five bodies and the map
   held zero of the five symbols. Binding the friendly name is what makes them
   reachable, so this lane's closure gap costs no slice line -- five aliases and
   five bodies that stop being dead. The linkage COUNT does not move for them
   (linkage.py keys on the object, and the objects were already in the link), so
   this is a correctness gain rather than a yield one. The same shape as
   `/alternatename:_func_02012664=__ZN5Sound9PlayBank3EjRK7Vector3` in
   hal/actor_classes_wf.cpp. */
#pragma comment(linker, "/alternatename:_ShadowModel_InitCylinder=__ZN11ShadowModel12InitCylinderEv")
#pragma comment(linker, "/alternatename:_ModelAnim_SetAnim=__ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj")
#pragma comment(linker, "/alternatename:_TextureSequence_Prepare=__ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File")
#pragma comment(linker, "/alternatename:_TextureSequence_SetFile=__ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj")
#pragma comment(linker, "/alternatename:_Animation_LoadFile=__ZN9Animation8LoadFileER13SharedFilePtr")

/* The two bodies src defines as real C++ methods against include/MirrorLuigi.h,
   faced here -- the ov013/ov024/ov025/ov032/ov033/ov034/ov035/ov047 recipe.
   Behavior is the THIRD such method and is NOT faced: it is the host copy in
   port/unmatched/MirrorLuigi_Behavior.cpp, which exports the Itanium name
   directly. */
#include "MirrorLuigi.h"
extern "C" {
int _ZN11MirrorLuigi13InitResourcesEv(void *self)
{ return ((MirrorLuigi *)self)->MirrorLuigi::InitResources(); }
int _ZN11MirrorLuigi6RenderEv(void *self)
{ return ((MirrorLuigi *)self)->MirrorLuigi::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov55_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov055 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov055 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV55_TRAP(n) \
    static int __fastcall ov55_trap##n(void *s, void *) \
    { ov55_trap_report(s, n); return 0; }
OV55_TRAP(13) OV55_TRAP(14)
#undef OV55_TRAP

static int __fastcall ov55_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov55_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov55_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov55_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov55_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov55_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov55_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov55_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov55_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov55_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov55_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov55_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov55_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov55_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov55_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov55_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov55_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov55_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov55_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov55_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov55_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ============================================================================
// THE ONE-CELL PMF SEAT
// ============================================================================
/* {the ROM address the sinit's own source pair carries, the host body}. The ROM
   column is verified against the mounted bytes before the rewrite: a mount
   pointing at the wrong bytes ABORTS instead of silently calling into DS
   memory. */
extern "C" {
struct PortMirrorLuigiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortMirrorLuigiCell data_ov055_02111b70;
}

extern "C" void port_mirrorluigi_state_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    PortMirrorLuigiCell &cell = data_ov055_02111b70;
    if (cell.enter_fn != 0x021112bc || cell.enter_delta != 0 ||
        cell.tick_fn != 0x02111288 || cell.tick_delta != 0) {
        std::fprintf(stderr, "FATAL: MirrorLuigi state cell: the sinit left "
                     "%08x/%u %08x/%u, the ROM's own records say "
                     "021112bc/0 02111288/0 -- WRONG BYTES\n",
                     cell.enter_fn, cell.enter_delta,
                     cell.tick_fn, cell.tick_delta);
        std::abort();
    }
    cell.enter_fn = (unsigned)(size_t)&func_ov055_021112bc;
    cell.tick_fn = (unsigned)(size_t)&func_ov055_02111288;
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov55_bringup_done;
DSSTATE_END

extern "C" void port_ov55_bringup(void)
{
    if (g_ov55_bringup_done)
        return;
    g_ov55_bringup_done = 1;
    port_ov055_pack_check();
    port_ov055_syms_patch();
    __sinit_ov055_021118d4();
    /* Seat and verify the cell BEFORE anything can dispatch through it:
       MirrorLuigi::InitResources installs it through func_ov055_021112c4,
       which tail-calls its enter half on the same frame. */
    port_mirrorluigi_state_seat();
}

// ============================================================================
// MIRROR_LUIGI (195) -- _ZTV11MirrorLuigi 0x02111ae0, RTTI 9daLuigi_c, 31 slots
// ============================================================================
//
// The mirror image of the player in Luigi's key course. A 524-byte Actor:
// ModelAnim at +0xd4, Model at +0x138, ShadowModel at +0x188, two
// TextureSequences at +0x1b0, the state cell pointer at +0x1d8 and a
// Matrix4x3 at +0x1dc.
//
// ---- SLOT 16, THE HAND-SPELLED D1 -----------------------------------------
// src/_ZN11MirrorLuigiD1Ev.cpp is the //cpp REAL-DESTRUCTOR shape, the one
// port/slice_sweep1_ov024.txt holds src/_ZN10PyramidTagD1Ev.cpp out for: it
// declares its own local struct ShadowModel / Model / ModelAnim / Actor and
// calls `((ShadowModel *)(c + 0x188))->~ShadowModel();` and three more like it,
// so MSVC emits THISCALL calls to ??1ShadowModel@@QAE@XZ and friends -- names
// the port defines only as CDECL C bodies. Aliasing across that is a
// receiver-shape mismatch, not a spelling one. It is also the ONE ov055
// function with no delink block.
// The thunk below is a statement-for-statement transcription of the ROM body at
// 0x021111a0 (0x58 bytes), disassembled from
// extracted/overlays/overlay_0055.bin: install the own table, __destroy_arr
// over the TWO TextureSequences at +0x1b0 (stride 0x14) with
// TextureSequence::~D1, then ShadowModel at +0x188, Model at +0x138, ModelAnim
// at +0xd4 and Actor::D2 -- member sub-objects destroyed high address first,
// and no Memory::Deallocate, because the caller of slot 16
// (ActorBase::AfterCleanupResources) deallocates itself.
static int __fastcall ml_init(void *s, void *)
{ return _ZN11MirrorLuigi13InitResourcesEv(s); }
static int __fastcall ml_clean(void *s, void *)
{ (void)s; return _ZN11MirrorLuigi16CleanupResourcesEv(); }
static int __fastcall ml_behavior(void *s, void *)
{ return _ZN11MirrorLuigi8BehaviorEv(s); }        /* HOST COPY */
static int __fastcall ml_render(void *s, void *)
{ port_actor_render_probe("MIRROR_LUIGI", (char *)s + 0x138);
  return _ZN11MirrorLuigi6RenderEv(s); }
static int __fastcall ml_pdes(void *s, void *)
{ (void)s; _ZN11MirrorLuigi16OnPendingDestroyEv(); return 0; }
static int __fastcall ml_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV11MirrorLuigi;
    __destroy_arr(t + 0x1b0, 2, 0x14, (void *)&_ZN15TextureSequenceD1Ev);
    _ZN11ShadowModelD1Ev(t + 0x188);
    _ZN5ModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall ml_d0(void *s, void *)
{ return (int)(size_t)_ZN11MirrorLuigiD0Ev(s); }

extern "C" void hal_fill_mirror_luigi_vtable(void)
{
    port_ov55_bringup();
    void *volatile *vt = (void *volatile *)_ZTV11MirrorLuigi;
    vt[1]  = (void *)ov55_binit;
    vt[2]  = (void *)ov55_ainit;
    vt[4]  = (void *)ov55_bclean;
    vt[5]  = (void *)ov55_aclean;
    vt[7]  = (void *)ov55_bbeh;
    vt[8]  = (void *)ov55_abeh;
    vt[10] = (void *)ov55_bren;
    vt[11] = (void *)ov55_aren;
    vt[13] = (void *)ov55_trap13;
    vt[14] = (void *)ov55_trap14;
    vt[15] = (void *)ov55_heap;
    vt[18] = (void *)ov55_yoshi;
    vt[19] = (void *)ov55_turn_egg;
    vt[20] = (void *)ov55_v50;
    vt[21] = (void *)ov55_pounded;
    vt[22] = (void *)ov55_atk1;
    vt[23] = (void *)ov55_atk2;
    vt[24] = (void *)ov55_kicked;
    vt[25] = (void *)ov55_pushed;
    vt[26] = (void *)ov55_cannon;
    vt[27] = (void *)ov55_mega;
    vt[28] = (void *)ov55_under;
    vt[29] = (void *)ov55_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[0]  = (void *)ml_init;
    vt[3]  = (void *)ml_clean;
    vt[6]  = (void *)ml_behavior;
    vt[9]  = (void *)ml_render;
    vt[12] = (void *)ml_pdes;   /* own body, overrides ActorBase's default */
    vt[16] = (void *)ml_d1;
    vt[17] = (void *)ml_d0;
    /* no slot 31: a plain Actor, 31 slots total, ends at 30. */
}
