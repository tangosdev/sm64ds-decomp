/* ov091's THREE DECLINED IDS, and the two classes behind them --
 * ARROW_PATH_LIFT (157) and SQUARE_METAL_NET_LIFT (144), which SHARE ONE
 * CLASS, plus FWOOSH (231). Run rel0215 wave 3, lane w3-f2, executing lane
 * w3-f's banked recon.
 *
 * ov091 was already mounted whole at gate 32 (port/ov091_syms.txt, the
 * chomb-drags-it-in mount) and the two vtable spans this file hosts are
 * already EXCLUDED from that mount -- 0x02134ec0..0x02134f40 and
 * 0x021353a4..0x02135440 are both gaps in its symbol list. So this seat adds
 * NO mount rows at all. Slice: port/slice_w3f2.txt.
 *
 * THE CAST MAP, every column re-derived on the ROM image before it was written
 * (extracted/overlays/overlay_0091.bin, and extracted/arm9_dec.bin at base
 * 0x02004000 for the spawn table -- never a dsd export, T4):
 *
 *   id   class                    SpawnInfo   vtable      slots  spawn fn
 *  157   RotatingUpDownPlatform   0x02134e80  0x02134ec0    32   0x021323cc
 *  144   RotatingUpDownPlatform   0x02134e9c  0x02134ec0    32   0x02132394
 *  231   Stump                    0x02135388  0x021353ac    31   0x021344a0
 *
 * ONE CLASS, TWO ROWS. ArrowPathLift_Spawn (0x021323cc) and
 * SquareMetalNetLift_Spawn (0x02132394) are byte-identical 0x38-byte factories
 * and each one's single ov091 pool literal is 0x02134ec0. Two ids, one table,
 * one fill; the variant they differ by is read at run time out of the
 * INSTANCE's own parameter word, not the SpawnInfo's -- RotatingUpDownPlatform
 * ::InitResources takes `mParam >> 8` from the object at +0x8 as mVariant and
 * indexes data_ov091_021344fc / _021344f4 / _02134e5c with it, so the two ids
 * differ by the spawn parameter the level data hands each instance.
 *
 * BOTH ROUTES AGREE ON ALL THREE IDS. arm9 ACTOR_SPAWN_TABLE[157] at
 * 0x02090ad8 reads 0x02134e80, [144] at 0x02090aa4 reads 0x02134e9c, and [231]
 * at 0x02090c00 reads 0x02135388; each record's own +4 low halfword reads back
 * 157, 144 and 231. Placement, measured on the base build's censuses rather
 * than assumed: 157 is placed on level 39 (Bowser in the Sky), 144 on level 37,
 * 231 on level 22, and each was a `[spawn-declined] actor 0x..` line there.
 *
 * WIDTHS. vtspan resolves 0x02134ec0 at THIRTY-TWO on every route (semantic
 * tail, typeinfo, terminator, raw run and next-dsd-symbol all say 32): slot 31
 * is ov002's Platform::Kill, the Platform signature. 0x021353ac resolves at
 * THIRTY-ONE, slot 30 Actor::OnAimedAtWithEggReturnVec and then data. The
 * typeinfo base words say the same thing a second way -- 0x02134e64's base
 * record is 0x021089ec (Platform / dBgActor_c) and 0x0213536c's is 0x021081c0
 * (Enemy) -- and so do the factories, which call PlatformC2Ev and EnemyC2Ev
 * respectively.
 *
 * TWO RTTI SPELLINGS, ONE BRIDGE. Reading each table's vtable[-1] typeinfo
 * through to its name string gives "13daLinelift2_c" for 0x02134ec0 and
 * "10daHyuhyu_c" for 0x021353ac.
 *   * _ZN5StumpD0Ev spells _ZTV10daHyuhyu_c while _ZN5StumpD1Ev and
 *     Fwoosh_Spawn spell _ZTV5Stump: ONE table under two names, so it gets an
 *     /alternatename. The LHS is defined nowhere else in this link -- checked,
 *     and port/tools/alternatename_guard.py re-checks it post-link.
 *   * BOTH RotatingUpDownPlatform destructors spell _ZTV13daLinelift2_c and
 *     THEN _ZTV10dBgActor_c. The first needs a bridge for the same reason
 *     Stump's does. THE SECOND MUST NOT GET ONE: _ZTV10dBgActor_c is ALREADY
 *     DEFINED in this link, by hal/actor_classes.cpp:591, as the 32-word
 *     Platform base table hal_fill_platform_vtable() fills. An /alternatename
 *     whose LHS is defined is defeated SILENTLY (the wave-5 R1/R2 class), so
 *     the destructors just reference the real array and get the real table.
 *     That is also why hal_fill_platform_vtable() is called from the lift fill
 *     below: the base table is installed BETWEEN the two member teardowns and
 *     an unfilled one is a dispatch to address 0.
 *
 * THE _ZTV5Stump NAME IS NOT ACTOR 27's TABLE, and port/ov091_syms.txt's
 * header already derived that. Re-checked here from the bytes:
 * Stump_SpawnInfo (0x02135298) has +4 halfword 27 and names factory
 * 0x02133938, whose pool literal is 0x021352bc, RTTI "11daObjPile_c" -- actor
 * 27's own table, which the gate-32 mount excludes and the registry fills.
 * The config symbol _ZTV5Stump sits at 0x021353ac with RTTI "10daHyuhyu_c" and
 * is FWOOSH's. So the seven _ZN5Stump* bodies in src/ are FWOOSH's methods.
 *
 * IDS 29 AND 30 ARE NOT A BUG. Both RotatingUpDownPlatform_Spawn (0x02131bdc)
 * and RotatingUpDownPlatformUtm_Spawn (0x02131ba4) install
 * _ZTV25RotatingUpDownPlatformUtm (0x02134c5c), so the existing single shared
 * fill in hal/actor_classes_l7.cpp is correct and this file leaves it alone.
 *
 * THE TWO RENDERS SPLIT, AND THE RECON RULED THEM TOGETHER. w3-f banked
 * "Render LINKS PLAIN because _ZTV5Model is dual-filled -- do not host-copy
 * it". That is right for ONE of these two classes and wrong for the other, and
 * which one depends on the MEMBER the shadow sits over, not on the shadow:
 *   RotatingUpDownPlatform::Render shadows the plain MODEL at +0xd4 -- its D1
 *     calls _ZN5ModelD1Ev on +0xd4 -- and hal/cxxname_bridge.cpp:507/511
 *     DUAL-FILLS _ZTV5Model at [4] AND [5] with Model::Render, so a shadow TU
 *     counting in ROM numbering lands on the right body. It serves from src/
 *     unchanged, and levels 39 and 37 run 300 frames rc 0 with it doing so.
 *   Stump::Render shadows the MODELANIM at +0x300 -- Fwoosh_Spawn calls
 *     _ZN9ModelAnimC1Ev on +0x300 -- and _ZTV9ModelAnim is NOT dual-filled: it
 *     is filled in MSVC numbering, where slot 5 is Virtual18 and Render has
 *     moved. Index 5 lands on the wrong body. This is the ov090 / Whomp / Fish
 *     / Spiny collision (T1) and it is a HOST COPY,
 *     unmatched/Ov091_HostSites.cpp, naming ModelAnim::Render outright.
 * FOUND BY RUNNING IT, not by reading it: the first level-22 run with this seat
 * in faulted on frame 1, c0000005 accessing 00000000, and the frames resolved
 * Stump::Render+0x26 -> ModelAnim::Virtual18 -> ModelAnim::Virtual10 ->
 * Model::Virtual10. The lifts' Render was left in the slice on the same
 * evidence: it was run and it does not fault.
 * The HAZARD CLASS was already written down, though, and the next reader should
 * start there rather than at a fault: hal/cxxname_bridge.cpp:517-520 states it
 * beside the ModelAnim2 fill -- "No dual-fill here: Render's ROM slot (5) is
 * Virtual18's MSVC slot, so shadow-TU Render dispatch cannot be served by the
 * same array". A Render shadowing a ModelAnim is that case; a Render shadowing
 * a plain Model is not.
 *
 * THE STATE SEAT, NINE RECORDS, TWO SINITS.
 * Both classes run pointer-to-member state machines whose SOURCE records the
 * gate-32 mount carries with DS CODE addresses in them (ovdata.py's pointer
 * pass rebases only pointers into other MOUNTED DATA). port_ov091_states_seat()
 * rewrites each record's fn word with its host body's address after VALIDATING
 * the mounted word against the ROM's own, so a wrong mount aborts loudly
 * instead of seating garbage. It must run BEFORE the sinits copy the records
 * into bss, which is why hal/actor_overlays.cpp calls it between
 * port_ov091_syms_patch() and the first __sinit_ov091_*.
 *
 * A CORRECTION TO THE BANKED RECON, RE-DERIVED HERE. w3-f banked the three
 * lift records against "__sinit_ov091_021346e8". The copying sinit is
 * __sinit_ov091_021345dc: 0x021345dc + 0x10c = 0x021346e8, so the literal pool
 * w3-f correctly quoted (0x021346d8..0x021346e4) is 021345dc's own TAIL pool.
 * Disassembling 021346e8 shows a SharedFilePtr sinit that never touches
 * 0x021354e0. The three-record ORDER w3-f banked is right and reproduces; only
 * the function name was one entry late. The FWOOSH half is new: six more
 * records, copied by __sinit_ov091_02134a30 into three two-record bss cells.
 * Both orders below are read out of each sinit's own store offsets against its
 * own pool, instruction by instruction, not off the pool order alone.
 */
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
#include "RotatingUpDownPlatform.h"
#include "Stump.h"

extern "C" {

/* ---- the shared arm9 half both tables name ------------------------------- */
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
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);  /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                 /* slot 29 default */
void _ZN8Platform4KillEv(void *self);                          /* slot 31, ov002 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* the dBgActor_c base table */

/* ---- the two host vtable arrays. Both spans are gaps in the gate-32 mount,
        so nothing else defines them and a factory gets host addresses. ---- */
void *_ZTV22RotatingUpDownPlatform[32];  /* ov091 0x02134ec0, == _ZTV13daLinelift2_c */
void *_ZTV5Stump[31];                    /* ov091 0x021353ac, == _ZTV10daHyuhyu_c */

/* ---- ARROW_PATH_LIFT (157) + SQUARE_METAL_NET_LIFT (144) ----------------- */
int _ZN22RotatingUpDownPlatform13InitResourcesEv(void *self);    /* face: below */
int _ZN22RotatingUpDownPlatform16CleanupResourcesEv(void *self); /* face: below */
int _ZN22RotatingUpDownPlatform6RenderEv(void *self);            /* face: below */
int _ZN22RotatingUpDownPlatform8BehaviorEv(void *self);          /* unmatched/Ov091_HostSites */
int *_ZN22RotatingUpDownPlatformD1Ev(int *self);                 /* .c, C linkage */
int *_ZN22RotatingUpDownPlatformD0Ev(int *self);                 /* .c, C linkage */
void *ArrowPathLift_Spawn(void);
void *SquareMetalNetLift_Spawn(void);
extern unsigned char ArrowPathLift_SpawnInfo[];
extern unsigned char SquareMetalNetLift_SpawnInfo[];

/* ---- FWOOSH (231) -------------------------------------------------------- */
int _ZN5Stump13InitResourcesEv(void *self);      /* face: below */
int _ZN5Stump16CleanupResourcesEv(void *self);   /* face: below */
int _ZN5Stump6RenderEv(void *self);              /* unmatched/Ov091_HostSites */
int _ZN5Stump8BehaviorEv(void *self);            /* unmatched/Ov091_HostSites */
void _ZN5Stump16OnPendingDestroyEv(void);        /* slot 12, .c body takes void */
int *_ZN5StumpD1Ev(int *self);                   /* slot 16 */
int *_ZN5StumpD0Ev(int *self);                   /* slot 17 */
int func_ov091_02134498(void);                   /* slot 18, own OnYoshiTryEat: returns 4 */
/* Slot 19's body is a tail-call VENEER -- the ROM bytes at 0x0213448c are
   `e59fc000 e12fff1c 02043824`, ldr ip,[pc]; bx ip onto
   _ZN9ActorBase18MarkForDestructionEv -- and src declares it (void). It is
   declared WITH a self parameter here, exactly as gate 193 declares the
   byte-identical func_ov072_02121fa0 and hal/actor_classes_ov030.cpp declares
   func_ov030_021145d4: a veneer is a tail jump on the host too, so it forwards
   whatever this thunk pushed, and pushing nothing would leave
   MarkForDestruction reading the thunk's own return address as its self. */
void func_ov091_0213448c(void *self);            /* slot 19, own OnTurnIntoEgg */
int func_ov091_02134484(void);                   /* slot 29, own: returns 0x1f000 */
void *Fwoosh_Spawn(void);
extern unsigned char Fwoosh_SpawnInfo[];

/* ---- the NINE mounted PMF source records --------------------------------- */
extern unsigned int data_ov091_02134e44[];   /* lifts: state 1 */
extern unsigned int data_ov091_02134e4c[];   /* lifts: state 2 */
extern unsigned int data_ov091_02134e54[];   /* lifts: state 0 */
extern unsigned int data_ov091_0213533c[];   /* fwoosh: 021356c0 tick */
extern unsigned int data_ov091_02135344[];   /* fwoosh: 021356c0 enter */
extern unsigned int data_ov091_0213534c[];   /* fwoosh: 021356b0 enter */
extern unsigned int data_ov091_02135354[];   /* fwoosh: 021356b0 tick */
extern unsigned int data_ov091_0213535c[];   /* fwoosh: 021356d0 tick */
extern unsigned int data_ov091_02135364[];   /* fwoosh: 021356d0 enter */

/* ---- the nine host state bodies ------------------------------------------ */
void func_ov091_02131ef0(void *c);
void func_ov091_02131f9c(void *c);
void func_ov091_02132000(void *c);
int func_ov091_02133c6c(void *c);
int func_ov091_02133d1c(void *c);
int func_ov091_02133d30(void *c);
int func_ov091_02133f24(void *c);
int func_ov091_02133f60(void *c);
int func_ov091_0213400c(void *c);

}  /* extern "C" */

/* ============================================================================
 * THE STATE SEAT
 *
 * Every `rom` column is the word the ROM image actually holds at that record,
 * read out of extracted/overlays/overlay_0091.bin at base 0x02130f00 (never
 * dsd's copy, T4). Every delta word is 0 in the ROM, which is what makes a
 * plain call through the fn word the same call the ROM makes -- checked for
 * all nine, and the seat re-checks it at run time.
 *
 * THE LIFTS: __sinit_ov091_021345dc copies THREE records into
 * data_ov091_021354e0 (0x18 of bss). Read instruction by instruction from
 * 0x02134654 onward -- r2 = 0x021354e0, r1 = 0x02134e54 stored to [r2,#0]/[r2,#4],
 * r0 = 0x02134e44 stored to [r2,#8]/[r2,#12], then 0x02134e4c to [r2,#16]/[r2,#20]:
 *     state 0  <- 0x02134e54 = { 0x02132000, 0 }
 *     state 1  <- 0x02134e44 = { 0x02131f9c, 0 }
 *     state 2  <- 0x02134e4c = { 0x02131ef0, 0 }
 * RotatingUpDownPlatform::Behavior indexes it with *(int *)(this + 0x320).
 *
 * FWOOSH: __sinit_ov091_02134a30 copies SIX records into three two-record bss
 * cells, from 0x02134a8c onward, same shape:
 *     0x021356d0[0] <- 0x02135364 = { 0x0213400c, 0 }   enter
 *     0x021356d0[1] <- 0x0213535c = { 0x02133f60, 0 }   tick
 *     0x021356b0[0] <- 0x0213534c = { 0x02133f24, 0 }   enter
 *     0x021356b0[1] <- 0x02135354 = { 0x02133d30, 0 }   tick
 *     0x021356c0[0] <- 0x02135344 = { 0x02133d1c, 0 }   enter
 *     0x021356c0[1] <- 0x0213533c = { 0x02133c6c, 0 }   tick
 * func_ov091_02134044 stores the chosen cell's address at this+0x364 and
 * dispatches record 0; Stump::Behavior dispatches record 1 through it. The
 * record-0-enter / record-1-tick split is the ov090 pack's exactly.
 *
 * THE SIX BSS CELLS THEMSELVES ARE NOT TOUCHED HERE. Only the nine SOURCE
 * records are rewritten, and the ROM's own sinits then copy them across, so
 * the copy the game makes is the copy the game runs.
 * ==========================================================================*/
namespace {
struct Ov091Seat {
    unsigned int *rec;
    unsigned int rom;
    void (*host)(void *);
    const char *name;
};
/* the four FWOOSH bodies whose src signature returns int; the seat stores a
   function ADDRESS, so the cast is only about the table's own type */
template <typename F> inline void (*as_state(F f))(void *)
{ return (void (*)(void *))(void *)f; }

const Ov091Seat g_ov091_seats[] = {
    /* the lifts, into data_ov091_021354e0 by __sinit_ov091_021345dc */
    {data_ov091_02134e54, 0x02132000, func_ov091_02132000, "lift/state0"},
    {data_ov091_02134e44, 0x02131f9c, func_ov091_02131f9c, "lift/state1"},
    {data_ov091_02134e4c, 0x02131ef0, func_ov091_02131ef0, "lift/state2"},
    /* FWOOSH, into 021356d0 / 021356b0 / 021356c0 by __sinit_ov091_02134a30 */
    {data_ov091_02135364, 0x0213400c, as_state(func_ov091_0213400c), "fwoosh/d0-enter"},
    {data_ov091_0213535c, 0x02133f60, as_state(func_ov091_02133f60), "fwoosh/d0-tick"},
    {data_ov091_0213534c, 0x02133f24, as_state(func_ov091_02133f24), "fwoosh/b0-enter"},
    {data_ov091_02135354, 0x02133d30, as_state(func_ov091_02133d30), "fwoosh/b0-tick"},
    {data_ov091_02135344, 0x02133d1c, as_state(func_ov091_02133d1c), "fwoosh/c0-enter"},
    {data_ov091_0213533c, 0x02133c6c, as_state(func_ov091_02133c6c), "fwoosh/c0-tick"},
};
bool g_ov091_seated = false;
}  /* namespace */

extern "C" void port_ov091_states_seat(void)
{
    if (g_ov091_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov091_seats / sizeof g_ov091_seats[0]; ++i) {
        const Ov091Seat &s = g_ov091_seats[i];
        if (s.rec[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov091 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, s.rec[0], s.rom);
            std::abort();
        }
        if (s.rec[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov091 state seat %s: delta word is 0x%08x, every ov091 "
                "PMF record's delta is 0 in the ROM\n", s.name, s.rec[1]);
            std::abort();
        }
        s.rec[0] = (unsigned int)(size_t)s.host;
    }
    g_ov091_seated = true;
}

// ============================================================================
// THE SHARED ACTOR HALF. Every shared slot below was compared address for
// address between the two ROM tables and against ov090's and l7's shared
// halves before this was written to be one body: 0x02134ec0 and 0x021353ac
// carry the SAME arm9 addresses in 1, 2, 4, 5, 7, 8, 10, 11, 13, 14, 15,
// 20..28 and 30. They differ only where each class overrides, which is what
// the two fills below write.
// ============================================================================
namespace {
void ov91_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov091 vtable slot %d is not hosted (actor id %u "
                 "%s, phase %d, spawn step %d)\n", slot, id,
                 port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov091 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
}  /* namespace */
/* 13 and 14 are ActorBase::Virtual34/Virtual38 -- neither body is linked
   anywhere in this port, the standing repo-wide reason. 30 is
   Actor::OnAimedAtWithEggReturnVec, an SRET method no __fastcall(void*, void*)
   thunk shape models -- the Enemy/Platform trap ov077/ov090/ov094/l7 all take.
   All three are LOUD faces, not silent stubs. */
static int __fastcall ov91_trap13(void *s, void *) { ov91_trap_report(s, 13); return 0; }
static int __fastcall ov91_trap14(void *s, void *) { ov91_trap_report(s, 14); return 0; }

static int __fastcall ov91_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov91_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov91_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov91_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov91_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov91_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov91_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov91_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov91_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov91_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov91_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19, OnTurnIntoEgg(Player &player): the caller PUSHES the player, so the
   three-parameter veneer pops it. */
static int __fastcall ov91_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov91_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov91_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov91_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov91_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov91_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov91_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov91_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov91_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov91_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov91_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- the shared Actor half both ROM
   tables carry identically. A caller writes its own 0/3/6/9/16/17 and whichever
   of 12/18/19/29/31 it overrides. */
static void ov91_fill_shared(void **vt)
{
    vt[1]  = (void *)ov91_binit;
    vt[2]  = (void *)ov91_ainit;
    vt[4]  = (void *)ov91_bclean;
    vt[5]  = (void *)ov91_aclean;
    vt[7]  = (void *)ov91_bbeh;
    vt[8]  = (void *)ov91_abeh;
    vt[10] = (void *)ov91_bren;
    vt[11] = (void *)ov91_aren;
    vt[12] = (void *)ov91_pdes;
    vt[13] = (void *)ov91_trap13;
    vt[14] = (void *)ov91_trap14;
    vt[15] = (void *)ov91_heap;
    vt[18] = (void *)ov91_yoshi;
    vt[19] = (void *)ov91_turn_egg;
    vt[20] = (void *)ov91_v50;
    vt[21] = (void *)ov91_pounded;
    vt[22] = (void *)ov91_atk1;
    vt[23] = (void *)ov91_atk2;
    vt[24] = (void *)ov91_kicked;
    vt[25] = (void *)ov91_pushed;
    vt[26] = (void *)ov91_cannon;
    vt[27] = (void *)ov91_mega;
    vt[28] = (void *)ov91_under;
    vt[29] = (void *)ov91_aimed;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// ARROW_PATH_LIFT (157) and SQUARE_METAL_NET_LIFT (144), both
// RotatingUpDownPlatform (13daLinelift2_c). 32 slots. Own: 0 Init,
// 3 Cleanup, 6 Behavior, 9 Render, 16 D1, 17 D0. Slot 12 is ActorBase's own
// OnPendingDestroy and slot 31 is ov002's Platform::Kill -- both INHERITED,
// read off the ROM table's own relocations (0x02043ac0 and 0x020ee55c).
// ============================================================================
static int __fastcall lift_init(void *s, void *)
{ return _ZN22RotatingUpDownPlatform13InitResourcesEv(s); }
static int __fastcall lift_clean(void *s, void *)
{ return _ZN22RotatingUpDownPlatform16CleanupResourcesEv(s); }
static int __fastcall lift_behavior(void *s, void *)
{ return _ZN22RotatingUpDownPlatform8BehaviorEv(s); }
static int __fastcall lift_render(void *s, void *)
{ port_actor_render_probe("ARROW_PATH_LIFT/SQUARE_METAL_NET_LIFT", (char *)s + 0xd4);
  return _ZN22RotatingUpDownPlatform6RenderEv(s); }
static int __fastcall lift_d1(void *s, void *)
{ return (int)(size_t)_ZN22RotatingUpDownPlatformD1Ev((int *)s); }
static int __fastcall lift_d0(void *s, void *)
{ return (int)(size_t)_ZN22RotatingUpDownPlatformD0Ev((int *)s); }
static int __fastcall lift_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

extern "C" void hal_fill_rotating_up_down_platform_lift_vtable(void)
{
    /* seat the nine records BEFORE anything can dispatch through them:
       RotatingUpDownPlatform::InitResources runs on the first frame and
       Behavior indexes the table on the second. */
    port_ov091_states_seat();
    /* the base table the two destructors install mid-teardown */
    hal_fill_platform_vtable();
    void **vt = _ZTV22RotatingUpDownPlatform;
    ov91_fill_shared(vt);
    vt[0]  = (void *)lift_init;
    vt[3]  = (void *)lift_clean;
    vt[6]  = (void *)lift_behavior;
    vt[9]  = (void *)lift_render;
    vt[16] = (void *)lift_d1;
    vt[17] = (void *)lift_d0;
    vt[31] = (void *)lift_kill;
}

// ============================================================================
// FWOOSH (231), Stump (10daHyuhyu_c). 31 slots, a plain Enemy-derived Actor.
// Own: 0, 3, 6, 9, 12 OnPendingDestroy, 16 D1, 17 D0, 18 OnYoshiTryEat,
// 19 OnTurnIntoEgg, 29 OnAimedAtWithEgg. Slot 30 stays the trap.
// ============================================================================
static int __fastcall fwo_init(void *s, void *)
{ return _ZN5Stump13InitResourcesEv(s); }
static int __fastcall fwo_clean(void *s, void *)
{ return _ZN5Stump16CleanupResourcesEv(s); }
static int __fastcall fwo_behavior(void *s, void *)
{ return _ZN5Stump8BehaviorEv(s); }
static int __fastcall fwo_render(void *s, void *)
{ port_actor_render_probe("FWOOSH", (char *)s + 0x300);
  return _ZN5Stump6RenderEv(s); }
static int __fastcall fwo_pdes(void *s, void *)
{ (void)s; _ZN5Stump16OnPendingDestroyEv(); return 0; }
static int __fastcall fwo_d1(void *s, void *)
{ return (int)(size_t)_ZN5StumpD1Ev((int *)s); }
static int __fastcall fwo_d0(void *s, void *)
{ return (int)(size_t)_ZN5StumpD0Ev((int *)s); }
static int __fastcall fwo_yoshi(void *s, void *)
{ (void)s; return func_ov091_02134498(); }
static int __fastcall fwo_egg(void *s, void *, void *)
{ func_ov091_0213448c(s); return 0; }
static int __fastcall fwo_aimed(void *s, void *)
{ (void)s; return func_ov091_02134484(); }

extern "C" void hal_fill_fwoosh_vtable(void)
{
    port_ov091_states_seat();
    void **vt = _ZTV5Stump;
    ov91_fill_shared(vt);
    vt[0]  = (void *)fwo_init;
    vt[3]  = (void *)fwo_clean;
    vt[6]  = (void *)fwo_behavior;
    vt[9]  = (void *)fwo_render;
    vt[12] = (void *)fwo_pdes;
    vt[16] = (void *)fwo_d1;
    vt[17] = (void *)fwo_d0;
    vt[18] = (void *)fwo_yoshi;
    vt[19] = (void *)fwo_egg;
    vt[29] = (void *)fwo_aimed;
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ---- method faces ----------------------------------------------------------
// Init/Clean/Render are real MSVC methods in src against include/, so each
// needs the flat ROM name the vtable and the ROM's own callers use. Behavior is
// NOT here for either class: it is host-copied and defines its own flat name.
extern "C" {
int _ZN22RotatingUpDownPlatform13InitResourcesEv(void *self)
{ return ((RotatingUpDownPlatform *)self)->RotatingUpDownPlatform::InitResources(); }
int _ZN22RotatingUpDownPlatform16CleanupResourcesEv(void *self)
{ return ((RotatingUpDownPlatform *)self)->RotatingUpDownPlatform::CleanupResources(); }
int _ZN22RotatingUpDownPlatform6RenderEv(void *self)
{ return ((RotatingUpDownPlatform *)self)->RotatingUpDownPlatform::Render(); }
int _ZN5Stump13InitResourcesEv(void *self)
{ return ((Stump *)self)->Stump::InitResources(); }
int _ZN5Stump16CleanupResourcesEv(void *self)
{ return ((Stump *)self)->Stump::CleanupResources(); }
}

/* THE TWO RTTI-SPELLING BRIDGES. Each is ONE table under two names; the
 * right-hand side is the array defined above and the left-hand side is defined
 * nowhere in this link -- checked before these were written, because an
 * /alternatename whose LHS is already defined is defeated SILENTLY, and
 * port/tools/alternatename_guard.py fails the build post-link if that ever
 * stops being true. Neither belongs in port/tools/alternatename_baseline.txt:
 * that file is the allowlist of aliases whose LHS IS deliberately defined.
 *
 * _ZTV10dBgActor_c GETS NO LINE. Both RotatingUpDownPlatform destructors spell
 * it, and hal/actor_classes.cpp:591 already DEFINES it -- an alias would be
 * inert and the guard would fail the build. The destructors reference the real
 * array, which hal_fill_platform_vtable() above has filled. */
#pragma comment(linker, "/alternatename:__ZTV13daLinelift2_c=__ZTV22RotatingUpDownPlatform")
#pragma comment(linker, "/alternatename:__ZTV10daHyuhyu_c=__ZTV5Stump")

/* ============================================================================
 * SIXTEEN LINKAGE ALIASES, ALL MEASURED OFF THE FIRST LINK, NONE PREDICTED.
 *
 * The gate-10 cxx_aliases shape (hal/cxx_aliases.cpp's own header states it):
 * a `//cpp` TU that declares a C-named symbol WITHOUT extern "C" emits an MSVC
 * decoration for what is a C name everywhere else, and the alias closes the gap
 * without touching src/. Every left-hand side below was copied VERBATIM out of
 * the link log's LNK2019 lines; every right-hand side is a symbol this link
 * already defines. None of the sixteen left-hand sides is defined anywhere,
 * which is the condition port/tools/alternatename_guard.py re-checks post-link.
 *
 * TEN ov091 / ov002 DATA SYMBOLS, each wanted under whatever struct type its
 * reading TU declared for it. The mounts emit one flat C array per symbol, so
 * each is the same object under a decorated name. FOUR ov091 addresses appear
 * TWICE because two TUs of this slice declare the same cell differently:
 * Stump::InitResources takes 0x02135674/_0213567c/_02135684 as `char *` while
 * Stump::CleanupResources takes them as `SharedFilePtr`, and 0x021356d0 is a
 * `char *` in InitResources and a `void *` in func_ov091_02133d30. One line per
 * variant is the tree's convention. data_ov002_0210d9c0's SharedFilePtr variant
 * is NOT here: hal/actor_classes_ov065.cpp:193 already carries it, and a second
 * copy of the same directive is what the alternatename guard's own baseline
 * exists to keep out. */
#pragma comment(linker, "/alternatename:?data_ov091_021344fc@@3PAHA=_data_ov091_021344fc")
#pragma comment(linker, "/alternatename:?data_ov091_021344f4@@3PAHA=_data_ov091_021344f4")
#pragma comment(linker, "/alternatename:?data_ov091_02135674@@3PADA=_data_ov091_02135674")
#pragma comment(linker, "/alternatename:?data_ov091_0213567c@@3PADA=_data_ov091_0213567c")
#pragma comment(linker, "/alternatename:?data_ov091_02135684@@3PADA=_data_ov091_02135684")
#pragma comment(linker, "/alternatename:?data_ov091_021356d0@@3PADA=_data_ov091_021356d0")
#pragma comment(linker, "/alternatename:?data_ov091_021356d0@@3PAXA=_data_ov091_021356d0")
#pragma comment(linker, "/alternatename:?data_ov091_02135674@@3USharedFilePtr@@A=_data_ov091_02135674")
#pragma comment(linker, "/alternatename:?data_ov091_0213567c@@3USharedFilePtr@@A=_data_ov091_0213567c")
#pragma comment(linker, "/alternatename:?data_ov091_02135684@@3USharedFilePtr@@A=_data_ov091_02135684")
#pragma comment(linker, "/alternatename:?data_ov002_0210da40@@3USharedFilePtr@@A=_data_ov002_0210da40")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9a0@@3USharedFilePtr@@A=_data_ov002_0210d9a0")

/* THREE arm9 / ov002 SYMBOLS the lifts' InitResources spells without extern "C".
 * Two are free functions decorated cdecl on both sides (`@@YAX...@Z`) and the
 * third is a FUNCTION the TU declares as `extern char`, so it decorates as
 * DATA (`@@3DA`) and asks the linker for an object where an entry point lives.
 * All three are already in this link under their flat names, and pointing a
 * data reference at a function address is what the ROM's own literal pool does
 * -- the TU only ever takes its address and hands it to func_020393d4. */
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAXPAX0HH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
#pragma comment(linker, "/alternatename:?_ZN8Platform19UpdateClsnPosAndRotEv@@YAXPAX@Z=__ZN8Platform19UpdateClsnPosAndRotEv")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3DA=__ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")

/* ONE MISSPELLING, AND IT IS NOT A DECORATION VARIANT -- worth its own note
 * because the fix looks like the twelve above and is a different kind of thing.
 * src/_ZN22RotatingUpDownPlatform13InitResourcesEv.cpp calls `Vec3_equal`, a
 * lowercase-e name that include/decl_common.h:416 declares and that NOTHING in
 * this repo defines and no config symbols file names. Settled by address: the
 * TU's own call site at 0x021322f4 relocates to arm9 0x0203d064, which
 * config/arm9/symbols.txt calls Vec3_Equal, capital E -- a three-comparison
 * body already in src/Vec3_Equal.c and already in this build through
 * slice_gate13 / slice_gate33.
 *
 * THIS LINE COSTS AND EARNS NOTHING, MEASURED. It was written expecting to pull
 * a /OPT:REF-stripped object back in; it does not, because Vec3_Equal.c.obj was
 * ALREADY in the link before this lane -- hal/actor_faces_bob.cpp:98's
 * `?Vec3_Equal@@YAHPAX0@Z` alias is live and resolves into that object, and the
 * arm9 module's linked count is 1994 both before and after this branch. So the
 * alias is a pure spelling route and every one of this lane's 31 linked TUs is
 * an ov091 one. The expectation is recorded rather than deleted because a
 * reader looking at an alias whose RHS lives in someone else's gate will ask
 * exactly this question. */
#pragma comment(linker, "/alternatename:_Vec3_equal=_Vec3_Equal")
