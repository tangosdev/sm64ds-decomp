// GATE 50: the castle interior's PAINTING (daPicGate_c, actor 307), ov080.
//
// Peach's portrait gate: six on level 2's object table, all printed as skipped
// (- 307 x6) before this gate. NOT PeachPainting (38, ov010) -- that one is
// blocked on ov052/RollingRock. This is a different class in a different
// overlay, and it has no cross-overlay spawn: its InitResources reads its own
// tuning tables, allocates a Memory buffer and calls Actor::SetRanges and
// Actor::ClosestPlayer, nothing more.
//
// ---- THIRTY-ONE SLOTS ------------------------------------------------------
//
// daPicGate_c is an Actor subclass, so its vtable is the 0x7c-byte, 31-slot
// shape the Bob-omb enemies carry, not the 20-slot ActorBase one. Read the
// unnamed table dsd left as _ZTV8Painting with its relocations applied
// and it is Actor's own layout: slots 1/2/4/5/7/8/10/11 the Before/After
// halves, 13/14 ActorBase::Virtual34/38, 15 OnHeapCreated, 18 OnYoshiTryEat,
// 19 OnTurnIntoEgg, 20 Virtual50, 21..28 the eight combat hooks, 29
// OnAimedAtWithEgg and 30 OnAimedAtWithEggReturnVec. The painting overrides
// NONE of the tail -- 18 through 30 are all the inherited Actor defaults -- so
// its own slots are only 0/3/6/9/12/16/17, and pt_fill_shared writes the rest.
//
// The vtable is HOST STORAGE the registry fills, declared here as
// _ZTV8Painting so daPicGate_c_classInit's store of it (self->vtable = it)
// resolves to this array rather than the mounted ROM bytes -- ov080_syms.txt
// excludes it from the mount for exactly that reason.
//
// SLOTS 13/14 KEEP THE ENEMY TRAP, the gate-33 reading: they want
// ActorBase::Virtual34/Virtual38, which are matched but sit above the solid
// heap allocator the port has not built. Nothing dispatches them at a painting
// on level 2's boot; a frame that did aborts by name.
//
// SLOT 30 TRAPS: Actor::OnAimedAtWithEggReturnVec returns a Vector3 by value,
// the sret-in-r0 ABI no thunk bridges. Nothing aims a Yoshi egg at a painting
// while the character is Mario. Slot 17 is the class's own D0 (the deleting
// destructor _ZN8PaintingD0Ev, which stores the vtable, runs Actor::~Actor
// and deallocates) -- kept live because the ROM's teardown does call it,
// unlike the world-file classes whose slot 17 traps.
#include "port_d16.h"

#include <cstdio>
#include <cstring>
#include "ntr/gx.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include <cstdlib>

#include "dActor_c.h"
#include "fBase_c.h"

extern "C" {
/* the shared lifecycle halves, the same functions every gate writes */
int _ZN8dActor_c19BeforeInitResourcesEv(void *self);          /* slot 1  */
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN8dActor_c14BeforeBehaviorEv(void *self);               /* slot 7  */
int _ZN8dActor_c12BeforeRenderEv(void *self);                 /* slot 10 */
int _ZN8dActor_c13OnYoshiTryEatEv(void *self);                /* slot 18 */
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN8dActor_c9Virtual50Ev(void *self);                     /* slot 20 */
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);  /* slot 21 */
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);      /* slot 22 */
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);      /* slot 23 */
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);          /* slot 24 */
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);          /* slot 25 */
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */
void _ZN8dActor_c16OnAimedAtWithEggEv(void *self);            /* slot 29 */

extern int data_02099f24[];               /* the frame phase */
extern unsigned char data_020a4b4c;       /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);

/* the painting's seven own methods, C faces in their own TUs */
int _ZN8Painting13InitResourcesEv(void *self);      /* slot 0  InitResources */
int _ZN8Painting16CleanupResourcesEv(void *self);      /* slot 3  CleanupResources */
int _ZN8Painting8BehaviorEv(void *self);      /* slot 6  Behavior (PMF dispatch) */
int _ZN8Painting6RenderEv(void *self);      /* slot 9  Render (PMF dispatch) */
void _ZN8Painting16OnPendingDestroyEv(void);           /* slot 12 OnPendingDestroy (empty) */
int _ZN8PaintingD1Ev(void *self);      /* slot 16 D1 */

DSSTATE_BEGIN
void *_ZTV8Painting[33];            /* the host vtable daPicGate_c_classInit installs */
DSSTATE_END

/* The ROM D0 (_ZN8PaintingD0Ev) is the D1 body plus one Deallocate against
   the game heap. This file used to host it as a thunk of its own, the
   BrickBlock/BobOmb treatment, because the recovered src spells the vtable and
   the heap through decl_common's generic VT/HEAP externs -- names that would
   collide with every other class's D0 if the TU were compiled unbound. GATE 227
   COMPILES THE REAL BODY with those two names bound per-TU out of its own
   literal pool, so the thunk is gone and pt_d0 is now a face onto the ROM's
   word. data_020a0eac is Memory::gameHeapPtr, already host-provided by
   hal/cxxname_bridge.cpp, and it is what the -D row binds HEAP to. */
void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);
extern void *data_020a0eac;
}

/* ---- C++-linkage data aliases, the gate-33 actor_faces_bob treatment -------
   Four of the painting's globals are declared with C++ linkage in the .cpp
   TUs (func_ov080_02125630 and _ZN8Painting13InitResourcesEv spell them outside their
   extern "C" blocks), so a reference comes out decorated -- ?...@@3PAUDisp@@A
   and the like -- while the storage the mount and auto_bss emit is the plain C
   name. Data has no calling convention, so an alias onto the one object is
   exact. data_ov080_0212775c is the SharedFilePtr-pointer table from the mount
   (ov080_syms.txt); the other three are BSS (hal/auto_bss.cpp). */
#pragma comment(linker, "/alternatename:?data_ov080_02128628@@3PAUDisp@@A=_data_ov080_02128628")
#pragma comment(linker, "/alternatename:?data_ov080_0212869c@@3PAEA=_data_ov080_0212869c")
#pragma comment(linker, "/alternatename:?data_ov080_02128688@@3PAEA=_data_ov080_02128688")
#pragma comment(linker, "/alternatename:?data_ov080_0212775c@@3PAPAUSharedFilePtr@@A=_data_ov080_0212775c")

/* ---- the trap -------------------------------------------------------------
   One report for every trapped slot, naming which slot fired on which actor in
   which phase -- the enemy/world file's own form. */
static void pt_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n", slot, id,
                 port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define PT_TRAP(n) \
    static int __fastcall pt_trap##n(void *s, void *) \
    { pt_trap_report(s, n); return 0; }
PT_TRAP(13) PT_TRAP(14)
#undef PT_TRAP

static int __fastcall pt_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall pt_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall pt_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM, so the thunk calls
   the target directly rather than forwarding through the veneer's own face and
   dropping the argument riding in r1. */
static void __fastcall pt_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall pt_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall pt_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall pt_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall pt_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall pt_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall pt_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
static int __fastcall pt_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall pt_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall pt_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall pt_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall pt_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall pt_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall pt_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall pt_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall pt_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall pt_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall pt_aimed(void *s, void *)
{ _ZN8dActor_c16OnAimedAtWithEggEv(s); return 0; }
static void __fastcall pt_pdes_base(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); }

/* The shared half of the painting's 31-slot table. The class writes its own
   0/3/6/9/12/16/17 after. */
static void pt_fill_shared(void **vt)
{
    vt[1] = (void *)pt_binit;
    vt[2] = (void *)pt_ainit;
    vt[4] = (void *)pt_bclean;
    vt[5] = (void *)pt_aclean;
    vt[7] = (void *)pt_bbeh;
    vt[8] = (void *)pt_abeh;
    vt[10] = (void *)pt_bren;
    vt[11] = (void *)pt_aren;
    vt[13] = (void *)pt_trap13;
    vt[14] = (void *)pt_trap14;
    vt[15] = (void *)pt_heap;
    vt[18] = (void *)pt_yoshi;
    vt[19] = (void *)pt_egg;
    vt[20] = (void *)pt_v50;
    vt[21] = (void *)pt_pounded;
    vt[22] = (void *)pt_atk1;
    vt[23] = (void *)pt_atk2;
    vt[24] = (void *)pt_kicked;
    vt[25] = (void *)pt_pushed;
    vt[26] = (void *)pt_cannon;
    vt[27] = (void *)pt_mega;
    vt[28] = (void *)pt_under;
    vt[29] = (void *)pt_aimed;
    vt[30] = (void *)port_actor_s30_base;
}

/* ---- the painting's own seven slots ---------------------------------------
   InitResources, Behavior and Render each dispatch a pointer-to-member out of
   the seated data_ov080_02128628 table; the matched src does the fn/delta
   arithmetic itself (delta 0, complete class), so the thunk is a plain forward.
   OnPendingDestroy is an empty body; the ROM's slot-12 override is present so a
   painting does not fall to ActorBase::OnPendingDestroy. */
/* SM64DS_PAINT_TEX=1: what the painting's polygons are actually sampling, off
   unless set. Painting::InitResources stores func_ov080_02125630's five-word
   material record at this+0x1a8. That function is a CACHE keyed on an ov080
   bss byte (data_ov080_0212869c[0x18*i]): on the first call it uploads and
   writes the record, on every later call it hands the record back untouched.
   Word 0 of the record is Model::LoadCompressedTextureToVram's return, which
   is the texture VRAM bump cursor data_020a4bc8 at upload time -- an offset
   into a cursor every level boot rewinds. */
static int pt_tex_probe(void)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_PAINT_TEX") != 0;
    return on;
}
extern "C" unsigned char data_ov080_0212869c[];
extern "C" unsigned char data_ov080_02128688[];

/* THE CACHE BLOCK, sized from the ROM's own symbol table rather than guessed.
   config/arm9/overlays/ov080/symbols.txt has data_ov080_02128688 and
   data_ov080_0212869c as kind:bss, 0x14 apart and both indexed by 0x18*i, so
   each element is 0x18 bytes: twenty bytes of record then the loaded flag at
   +0x14. The slot count is the length of the SharedFilePtr array
   func_ov080_02125630 indexes with the same i -- data_ov080_0212775c runs to
   data_ov080_021277a8, 0x4c bytes, 19 pointers. */
enum { PT_TEX_SLOTS = 19, PT_TEX_STRIDE = 0x18 };

/* WHAT THE CARTRIDGE'S OVERLAY LOAD DOES TO THAT BLOCK, which this port was
   not doing. UnloadLevelOverlays then LoadLevelOverlays run on every level
   change, level 2's object overlays are 63/80/85/89/94/100 and Bob-omb
   Battlefield's are 62/69/78/84/91/95/100, so ov080 comes off and goes back
   on around a course -- and FS_LoadOverlay clears the overlay's bss. The
   cartridge therefore starts every castle entry with the flag at 0 and
   re-uploads the picture to the current cursor. The port's LoadOverlay face
   is empty and is never called at all, so the flag stayed 1 for the life of
   the process and the second visit re-bound the first visit's dead offset:
   the frame drew right and the picture was whatever the course had left in
   that VRAM. Called from hal/level_boot.cpp's port_stage_boot_body, which is
   this port's Stage::InitResources, at the ROM's own position for the call. */
extern "C" void port_painting_texcache_overlay_load(int id)
{
    if (id != 80) return;
    std::memset(data_ov080_02128688, 0, PT_TEX_SLOTS * PT_TEX_STRIDE);
    if (pt_tex_probe()) {
        std::fprintf(stderr, "[painttex] overlay load %d: texture cache "
                     "cleared (%d slots)\n", id, (int)PT_TEX_SLOTS);
        std::fflush(stderr);
    }
}

static int __fastcall pt_init(void *s, void *)
{
    int r = _ZN8Painting13InitResourcesEv(s);
    fprintf(stderr, "[paint] init: obj=%p buf=%p n=%u\n", s,
            *(void **)((char *)s + 0x1a0),
            *(unsigned short *)((char *)s + 0x1b8));
    if (pt_tex_probe()) {
        const unsigned char *rec = *(const unsigned char **)((char *)s + 0x1a8);
        const int slot = rec ? (int)((rec - data_ov080_02128688) / PT_TEX_STRIDE) : -1;
        std::fprintf(stderr, "[painttex] init obj=%p texslot=%d "
                     "record vram=%08x pal=%08x w=%08x h=%08x fmt=%08x\n",
                     s, slot,
                     rec ? *(const unsigned *)(rec + 0) : 0u,
                     rec ? *(const unsigned *)(rec + 4) : 0u,
                     rec ? *(const unsigned *)(rec + 8) : 0u,
                     rec ? *(const unsigned *)(rec + 0xc) : 0u,
                     rec ? *(const unsigned *)(rec + 0x10) : 0u);
    }
    fflush(stderr);
    return r;
}
static int __fastcall pt_clean(void *s, void *)
{ return _ZN8Painting16CleanupResourcesEv(s); }
static int __fastcall pt_behavior(void *s, void *)
{
    /* 2026-08-08, the star door to BoB: a painting ticked with +0x1a0 holding
       a small integer, not the wave buffer -- reads like Behavior before
       Init. Until the ordering is understood, refuse the tick and say so
       rather than walking a garbage array. */
    void *buf = *(void **)((char *)s + 0x1a0);
    if ((size_t)buf < 0x10000) {
        static unsigned squelch;
        if ((squelch++ & 0xff) == 0)
            fprintf(stderr, "[paint] tick REFUSED: obj=%p buf=%p n=%u "
                    "(Behavior before Init?)\n", s, buf,
                    *(unsigned short *)((char *)s + 0x1b8));
        fflush(stderr);
        return 1;
    }
    return _ZN8Painting8BehaviorEv(s);
}
/* The painting owns no Model at a fixed offset -- InitResources loads its
   textures straight to VRAM through func_ov080_02125630 rather than a
   Model/ModelAnim member -- so the render probe gets the object base, which is
   all it reads when SM64DS_ACTOR_PROBE names this class. */
static int __fastcall pt_render(void *s, void *)
{ port_actor_render_probe("PAINTING", s);
  if (!pt_tex_probe())
      return _ZN8Painting6RenderEv(s);
  std::size_t before = 0, after = 0;
  ntr::gx_polygons(before);
  const int r = _ZN8Painting6RenderEv(s);
  const ntr::GxTriangle *t = ntr::gx_polygons(after);
  const std::size_t n = after > before ? after - before : 0;
  float mnx = 1e30f, mxx = -1e30f, mny = 1e30f, mxy = -1e30f;
  unsigned amin = 255, amax = 0, tex_param = 0;
  int textured = 0, tw = 0, th = 0;
  unsigned long long sum = 0;
  for (std::size_t i = before; i < after; ++i) {
      for (int v = 0; v < 3; ++v) {
          const float X = t[i].v[v].x, Y = t[i].v[v].y;
          if (X < mnx) mnx = X;
          if (X > mxx) mxx = X;
          if (Y < mny) mny = Y;
          if (Y > mxy) mxy = Y;
      }
      if (t[i].alpha < amin) amin = t[i].alpha;
      if (t[i].alpha > amax) amax = t[i].alpha;
      if (t[i].tex) {
          ++textured;
          if (!tex_param) {
              tex_param = t[i].dbg_tex;
              tw = t[i].tw; th = t[i].th;
              const long px = (long)tw * th;
              for (long k = 0; k < px; ++k)
                  sum = sum * 1000003ull + t[i].tex[k];
          }
      }
  }
  static int call;
  if (n == 0)
      std::fprintf(stderr, "[painttex] render %d obj=%p: 0 triangles\n", call++, s);
  else
      std::fprintf(stderr, "[painttex] render %d obj=%p: %u tris screen "
                   "x[%.0f..%.0f] y[%.0f..%.0f] alpha %u..%u textured %d "
                   "teximage=%08x tex=%dx%d texelsum=%016llx\n",
                   call++, s, (unsigned)n, mnx, mxx, mny, mxy, amin, amax,
                   textured, tex_param, tw, th, sum);
  std::fflush(stderr);
  return r; }
static int __fastcall pt_pdes(void *, void *)
{ _ZN8Painting16OnPendingDestroyEv(); return 0; }
static int __fastcall pt_d1(void *s, void *)
{ return _ZN8PaintingD1Ev(s); }
/* slot 17, the ROM's own D0, GATE 227. This file used to host the deleting
   destructor as a HAND-WRITTEN THUNK -- run the D1, then Deallocate against the
   game heap -- because slice_gate50.txt had to exclude the real body: its
   recovered src spells the vtable and the heap through decl_common's generic
   VT/HEAP externs, "names that would collide with every other class's D0 if the
   TU were compiled". The collision is a naming problem, not a body problem, and
   port/CMakeLists.txt's gate 227 block removes it with a per-TU -D row read out
   of THIS body's own literal pool: 0x02125458 -> 0x021282b4 (the host array
   declared above) and 0x0212545c -> 0x020a0eac (Memory::gameHeapPtr).
   VERIFIED BY ADDRESS, not by name: the table word at 0x021282b4 + 17*4 =
   0x021282f8 relocates to 0x02125428 in config/arm9/overlays/ov080/relocs.txt,
   symbols.txt gives that address its own kind:function(arm,size=0x38) record,
   and the disassembly there is an ARM prologue -- so it is a real entry and not
   an interior address of a tail-shared body. The ROM body is the thunk's two
   steps written out (store the vtable, Actor::~Actor, Deallocate), which is why
   this replaces a stand-in rather than a decline. */
extern "C" int *_ZN8PaintingD0Ev(int *t);   /* ov080 0x02125428 */
static int __fastcall pt_d0(void *s, void *)
{ return (int)(size_t)_ZN8PaintingD0Ev((int *)s); }

extern "C" void hal_fill_painting_vtable(void)
{
    void **vt = _ZTV8Painting;
    pt_fill_shared(vt);
    vt[0] = (void *)pt_init;
    vt[3] = (void *)pt_clean;
    vt[6] = (void *)pt_behavior;
    vt[9] = (void *)pt_render;
    vt[12] = (void *)pt_pdes;
    vt[16] = (void *)PORT_D16(pt_d1);
    vt[17] = (void *)pt_d0;
}
