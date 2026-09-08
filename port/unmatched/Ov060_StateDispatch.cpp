/* THE BOWSER FIGHT'S STATE MACHINES -- ov060's SEVEN runtime pointer-to-member
 * dispatch tables, filled from SIX source runs, seated with host bodies, plus
 * host copies of the five dispatchers whose pointer-to-member STRIDE does not
 * match the ROM record.
 *
 * Six and seven are both right and they count different things, which is what
 * the lane's "six tables" wording elided: there are six runs of source statics
 * that a sinit copies, and BOWSER FIRE's run (sixteen records at 0x0211a734)
 * fills TWO runtime tables out of one run, its init half and its behaviour
 * half.  The seven below are the runtime bss tables the dispatchers read, and
 * that is the count that governs the seats.
 *
 * run linkw wave 6, lane w6-A.
 *
 * ==== THE SEVEN TABLES, RE-DERIVED FROM THE OVERLAY IMAGE ===================
 *
 * Every one is bss, filled by an ov060 sinit out of 8-byte {code, adj} source
 * statics in .data, and every one of the 50 source records reads adj == 0 (the
 * non-virtual complete-class Itanium form) -- read from
 * extracted/overlays/overlay_0060.bin and cross-checked word for word against
 * config/arm9/overlays/ov060/relocs.txt.  The three arm9-side widths are the
 * next-symbol landings.
 *
 *   bss table            slots  built by                dispatched by
 *   0x0211aeb4 (0x20)      4    __sinit_ov060_021195dc  func_ov060_02112434
 *                                                       (BOWSER, idx +0x410)
 *   0x0211aed4 (0xa0)     20    __sinit_ov060_021195dc  func_ov060_021128c0
 *                                                       (BOWSER, idx +0x40c)
 *   0x0211ae9c (0x18)      3    __sinit_ov060_021195dc  func_ov060_02115b84
 *                                                       (BOWSER TAIL, idx +0x110)
 *   0x0211af74 (0x40)      8    __sinit_ov060_02119df0  BowserFire::InitResources
 *                                                       (idx +0x35c)
 *   0x0211afb4 (0x40)      8    __sinit_ov060_02119df0  BowserFire::Behavior
 *                                                       (idx +0x35c)
 *   0x0211b1d8 (0x20)      4    __sinit_ov060_0211a388  SpikeBomb::Behavior
 *                                                       (8-byte records -- it
 *                                                       reads the table as
 *                                                       int[] and indexes
 *                                                       [idx*2], idx +0x170)
 *   0x0211b1ac (0x18)      3    __sinit_ov060_0211a000  func_ov060_02118254
 *                                                       (SKY PLATFORM, idx +0x328)
 *
 * The lane brief carried a banked reading that "Bowser's two state-pair tables"
 * are 0x0211a4e0.. and 0x0211a734..  Corrected here from the readers: the
 * 0x0211a734 run is BOWSER FIRE's (both of its halves), and 0x0211a4e0 itself
 * is not a dispatch record at all -- it is the four-halfword message-id table
 * func_ov060_02115518 indexes with the actor's +0x414 variant byte.  Bowser's
 * own dispatch starts one record later, at 0x0211a4e8.  There are six source
 * runs and seven runtime tables, not two of anything, and one of them
 * (SpikeBomb's) was not in the bank at all.
 *
 * ==== WHY FIVE DISPATCHERS ARE HOST COPIES AND TWO ARE NOT ==================
 *
 * The ROM record is 8 bytes.  MSVC's pointer-to-member size depends on what it
 * knows about the class at the point the type is formed, and none of the
 * pointer-to-member shapes in this pack lands on 8.  RE-MEASURED at the wave-6
 * close with this build's compiler (MSVC 19.44 x86, the same switches the port
 * builds with).  The first row below was banked as 4 while the lane ran and is
 * corrected here -- the forward-declared-C case measures 16:
 *
 *   forward-declared C, DEFINED later in the same TU  ->  16
 *     (func_ov060_02112434, func_ov060_02115b84, func_ov060_02118254)
 *   complete empty `struct Actor { }`                 ->  4
 *     (_ZN10BowserFire13InitResourcesEv)
 *   forward-declared, never defined                   ->  16
 *     (_ZN10BowserFire8BehaviorEv)
 *   plain `struct { int a, b; }` + manual decode      ->  8   <-- correct
 *     (func_ov060_021128c0, _ZN17BowserSkyPlatform8BehaviorEv)
 *
 * The last row is not a pointer-to-member at all, it is two ints read by hand,
 * so NOTHING this pack hands MSVC as a pointer-to-member measures the ROM's 8.
 * All six host copies of this landing stay justified: the five here and Key's
 * dispatcher in port/unmatched/Ov089_StateDispatch.cpp, whose complete
 * single-inheritance class with a vtable measures 4.
 *
 * Either wrong stride destroys the table, in opposite directions.  A 16-byte
 * stride over-steps by exactly two records, so state 1 calls record 2's
 * function and the back half of the table runs off its own end into whatever
 * bss follows.  A 4-byte stride under-steps: it reads record 0 correctly and
 * then walks the adj words, so state 0 runs and every other state calls
 * through a zero.  Both shapes survive a short census -- the fight opens in
 * state 0 either way -- and die once it advances, which is exactly the failure
 * this pack exists to keep out.  On top of the stride, MSVC
 * emits a __thiscall through a pointer-to-member (receiver in ecx) while every
 * ov060 state body is a cdecl `func_ov060_xxxxxxxx(char *)` -- so even a
 * correctly strided read would hand the body a garbage argument.  Both faults
 * are the Crate / Chuckya / MontyMole case; the fix is the same one
 * port/unmatched/Crate_StateDispatch.cpp writes down: read the record as a
 * plain {function, adj} pair and call the function with `this`.
 *
 * func_ov060_021128c0 and _ZN17BowserSkyPlatform8BehaviorEv already decode the
 * record by hand into a two-int struct and call through a cdecl function
 * pointer, so they are RIGHT as matched src and stay in the slice.  They still
 * need the seat: the words they call are DS code addresses.
 *
 * ==== ONE NAMED HOLE IS A VERIFIED HOST COPY; THE OTHER IS REAL DECOMP ======
 *
 * w7a wrote host copies of BOTH named holes on the reading that neither had a
 * matched TU.  That reading was true for func_ov060_021140c0 and FALSE for
 * func_ov060_02116d78, and the reason it read true here is that this branch
 * forked from main at 7b2f913fe (2026-08-04) and cannot see what main has
 * matched since: src/func_ov060_02116d78.c landed on main in PR #1150
 * (817be2263, 2026-08-07), three days after the fork and four days before
 * w7a's host copy of it shipped.  Run linkw wave 9, lane w9-harvest, checked
 * out that TU and retired the host copy.
 *
 *   func_ov060_021140c0 (BOWSER state 9)        VERIFIED HOST COPY 7, below.
 *     Still genuinely unmatched: main has no TU of this name either (checked
 *     against origin/main at bc93fa767).  Not an abort stub -- the span was
 *     re-derived from the overlay image and transcribed instruction for
 *     instruction; the verification record is in its own banner below.
 *   func_ov060_02116d78 (BOWSER FIRE state 5)   src/func_ov060_02116d78.c.
 *     Byte-matched decomp, carried by port/slice_w9harvest.txt.  The host
 *     copy that used to sit below HOST COPY 7 is gone.
 *
 * The old abort-and-name stubs (the Koopa-0x02117724 / Rabbit-0x0212b8dc
 * precedent) are gone from both states either way: both RUN.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* The ROM's record, as the overlay image lays it out: {code address, adj}. */
struct PortPmf { unsigned fn; int adj; };

/* ---- BOWSER: 0x0211aeb4, four records (sinit 021195dc copy order) -------- */
extern PortPmf data_ov060_0211a578[], data_ov060_0211a510[],
    data_ov060_0211a518[], data_ov060_0211a520[];
/* ---- BOWSER: 0x0211aed4, twenty records --------------------------------- */
extern PortPmf data_ov060_0211a568[], data_ov060_0211a560[],
    data_ov060_0211a538[], data_ov060_0211a4e8[], data_ov060_0211a4f0[],
    data_ov060_0211a500[], data_ov060_0211a540[], data_ov060_0211a5b8[],
    data_ov060_0211a5b0[], data_ov060_0211a5a0[], data_ov060_0211a5a8[],
    data_ov060_0211a530[], data_ov060_0211a508[], data_ov060_0211a528[],
    data_ov060_0211a598[], data_ov060_0211a590[], data_ov060_0211a588[],
    data_ov060_0211a580[], data_ov060_0211a4f8[], data_ov060_0211a570[];
/* ---- BOWSER TAIL: 0x0211ae9c, three records ----------------------------- */
extern PortPmf data_ov060_0211a558[], data_ov060_0211a550[],
    data_ov060_0211a548[];
/* ---- BOWSER FIRE: 0x0211afb4 (behaviour) and 0x0211af74 (init) ---------- */
extern PortPmf data_ov060_0211a794[], data_ov060_0211a78c[],
    data_ov060_0211a76c[], data_ov060_0211a77c[], data_ov060_0211a764[],
    data_ov060_0211a774[], data_ov060_0211a744[], data_ov060_0211a784[];
extern PortPmf data_ov060_0211a75c[], data_ov060_0211a754[],
    data_ov060_0211a73c[], data_ov060_0211a74c[], data_ov060_0211a734[],
    data_ov060_0211a7ac[], data_ov060_0211a7a4[], data_ov060_0211a79c[];
/* ---- SPIKE BOMB: 0x0211b1d8, four records ------------------------------- */
extern PortPmf data_ov060_0211aa30[], data_ov060_0211aa48[],
    data_ov060_0211aa40[], data_ov060_0211aa38[];
/* ---- SKY PLATFORM: 0x0211b1ac, three records ---------------------------- */
extern PortPmf data_ov060_0211a938[], data_ov060_0211a940[],
    data_ov060_0211a930[];

/* the runtime tables the dispatchers read (bss, filled by the sinits) */
extern PortPmf data_ov060_0211aeb4[];
extern PortPmf data_ov060_0211ae9c[];
extern PortPmf data_ov060_0211b1ac[];
extern PortPmf data_ov060_0211af74[];
extern PortPmf data_ov060_0211afb4[];

/* ---- every state body, all matched src (cdecl, `this` by argument) ------- */
void func_ov060_021128c0(char *c);
void func_ov060_02112724(char *c);
void func_ov060_021125f0(char *c);

void func_ov060_02114f88(char *c);
void func_ov060_02113b5c(char *c);
void func_ov060_02113740(char *c);
void func_ov060_02113710(char *c);
void func_ov060_02112ddc(char *c);
void func_ov060_021154e8(char *c);
void func_ov060_021153f8(char *c);
void func_ov060_02113d8c(char *c);
void func_ov060_02114858(char *c);
void func_ov060_021142b4(char *c);
void func_ov060_02113fcc(char *c);
void func_ov060_021146d0(char *c);
void func_ov060_021143b8(char *c);
void func_ov060_02114d08(char *c);
void func_ov060_02114e9c(char *c);
void func_ov060_02114b60(char *c);
void func_ov060_02114300(char *c);
void func_ov060_02114ff8(char *c);
void func_ov060_02112bfc(char *c);

void func_ov060_02115d68(char *c);
void func_ov060_02115d50(char *c);
void func_ov060_02115c1c(char *c);

void func_ov060_0211747c(char *c);
void func_ov060_021169f8(char *c);
void func_ov060_02116b68(char *c);
void func_ov060_021167ec(char *c);
void func_ov060_021168c4(char *c);
void func_ov060_02116f90(char *c);
void func_ov060_021167c8(char *c);
void func_ov060_02116b18(char *c);
void func_ov060_02116c68(char *c);
/* run linkw wave 9, lane w9-harvest: 0x02116d78 joined this list. It used to
   be HOST COPY 8 in this same file; src/func_ov060_02116d78.c has been a
   byte-match on main since PR #1150 (817be2263, 2026-08-07) and the host copy
   outlived it only because this branch cannot see main's src/. */
void func_ov060_02116d78(char *c);
void func_ov060_021169b0(char *c);
void func_ov060_02116f74(char *c);
void func_ov060_0211722c(char *c);
void func_ov060_021171e8(char *c);

void func_ov060_02118970(char *c);
void func_ov060_021188e8(char *c);
void func_ov060_02118834(char *c);
void func_ov060_02118728(char *c);

void func_ov060_021181b4(char *c);
void func_ov060_021180e0(char *c);
void func_ov060_02117db8(char *c);

/* what the five host copies below call */
int Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
int _ZN5Actor14GetSubtractionEss(void *self, short a, short b);
char *_ZN5Actor10FindWithIDEj(unsigned id);
void _ZN12CylinderClsn5ClearEv(void *cc);
void _ZN12CylinderClsn6UpdateEv(void *cc);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *p);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *p);
int _ZN11ShadowModel12InitCylinderEv(void *self);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
    void *self, void *actor, int a, int b, unsigned c, unsigned d);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int a, int b, void *v, int c);
void _ZN13RaycastGroundC1Ev(void *self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(
    void *self, const void *v, void *actor);
int _ZN13RaycastGround10DetectClsnEv(void *self);
void _ZN13RaycastGroundD1Ev(void *self);
void WithMeshClsn_UpdateDiscreteNoLava_veneer(void *p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
void func_ov060_02116740(char *c);
void func_ov060_02117624(char *c);

}  /* extern "C" */

/* ============ THE TWO FORMERLY-NAMED HOLES ===============================
 * ONE body follows (HOST COPY 7, func_ov060_021140c0): a host copy
 * transcribed instruction for instruction from the overlay image, NOT matched
 * src and NOT a guess.  When the decomp banks the real TU it retires for the
 * slice line, which is exactly what happened to the other hole --
 * func_ov060_02116d78's host copy stood here until wave 9 replaced it with
 * main's src/func_ov060_02116d78.c.  Its span derivation is kept below
 * because it is what the retired host copy was checked against and it still
 * pins the 0x1fc the slice TU has to fill.
 *
 * ---- SPANS, RE-DERIVED (the old header's 0x1f4 / 0x1fc were CARRIED) ------
 * Bytes read from extracted/overlays/overlay_0060.bin at base 0x02111900
 * (extracted/dsd/arm9_overlays/overlays.yaml, ov060: base_address 34674944 =
 * 0x02111900, bss_size 1632).  Each span is confirmed TWICE and the two
 * derivations agree:
 *
 *   func_ov060_021140c0  0x021140c0..0x021142b3 = 0x1f4 bytes = 125 words
 *     116 instructions, push {r4-r8,lr} @0x021140c0 .. bx lr @0x0211428c
 *     +  9 literal words 0x02114290..0x021142b0, the last one (0x00000423)
 *        reached by this function's own ldr [pc, #0x68] @0x02114240
 *     next symbol func_ov060_021142b4; 0x021142b4 - 0x021140c0 = 0x1f4
 *
 *   func_ov060_02116d78  0x02116d78..0x02116f73 = 0x1fc bytes = 127 words
 *     121 instructions, push {r4,r5,r6,lr} @0x02116d78 .. bx lr @0x02116f58
 *     +  6 literal words 0x02116f5c..0x02116f70, the last one (0x00000011)
 *        reached by this function's own ldr [pc, #0x78] @0x02116ef0
 *     next symbol func_ov060_02116f74; 0x02116f74 - 0x02116d78 = 0x1fc
 *
 * ---- HOW THEY WERE VERIFIED, AND WHAT THAT DOES NOT COVER -----------------
 * The decomp oracle (mwccarm -> byte compare) is NOT AVAILABLE on this
 * machine: tools/mwccarm/2004/b56/mwccarm.exe does not exist in either the
 * repo or the worktree (tools/permuter/mwccarm_compile.sh names that path),
 * and tangos match reports "no compiler at 2004/b56" on a KNOWN-matched
 * control (src/func_ov060_02113fcc.c).  So neither body claims a compiler
 * diff.  What each body DOES claim, and what the reviewer can re-run:
 *
 *   1. TOTAL ACCOUNTING.  Every one of the 125 + 127 words is classified as
 *      instruction or literal, and every instruction is mapped to a statement
 *      below.  Nothing in either span is unexplained.
 *   2. EVERY CALL TARGET AND EVERY RELOCATED LOAD is confirmed against
 *      config/arm9/overlays/ov060/relocs.txt, independently of the
 *      disassembly.  For 0x021140c0 that table lists exactly 7 arm_call rows
 *      and 3 load rows; for 0x02116d78 exactly 6 arm_call rows (0x02043824
 *      TWICE -- the body really does call MarkForDestruction on two paths)
 *      and 2 load rows.  The counts and targets match the transcription
 *      one for one, and the literals the reloc table does NOT list are
 *      therefore plain numeric constants, not addresses -- which is what
 *      proves 0x92492493 / 0xcccccccd / 0xaaaaaaab / 0xf0f0f0f1 are division
 *      magics and not pointers.
 *   3. NO CHANGED CONSTANT, OFFSET OR BRANCH TARGET.  Every immediate, every
 *      structure offset and every branch is listed in the per-body banner.
 *      The divisors are re-derived from the magics, not guessed:
 *        0xcccccccd, umull + lsr #3          -> unsigned / 10
 *        0xaaaaaaab, umull + lsr #1          -> unsigned / 3
 *        0xf0f0f0f1, umull + lsr #4          -> unsigned / 17
 *        0x92492493, smull + add n + asr #9  -> signed  / 896
 *          (2^41 / 896 = 2454267026.28, ceil = 2454267027 = 0x92492493, the
 *           exact round-up magic; the trailing "+ (n >>> 31)" is C's
 *           round-toward-zero correction, i.e. plain `/ 896`)
 *   4. THE TRANSCRIPTION IDIOMS are the ones the MATCHED siblings in this
 *      same pack already use, so the shapes are not invented here:
 *        ((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 10
 *            -- src/func_ov060_021150d0.cpp, src/func_ov060_021151d4.c
 *        data_02082214[(*(u16 *)p >> 4) * 2] / [... * 2 + 1]
 *            -- src/func_ov060_021128c0.cpp, src/func_0203cc28.c
 *            (table re-read from arm9_dec.bin: entry[2i]=sin, [2i+1]=cos,
 *             scale 0x1000, 4096 entries -- checked at i=0/512/1024)
 *        *(u16 *)((c + 0x300) + 0xfc)  for the +0x3fc / +0x374 counters
 *            -- src/func_ov060_02113d8c.cpp, src/func_ov060_021128c0.cpp
 *
 * What this standard does NOT establish is that mwccarm would emit these
 * exact 116 / 121 instructions from this exact C.  It establishes that the
 * SEMANTICS are the ROM's, word for word, with no constant, offset or call
 * target altered.  That is the strongest claim this machine can support and
 * the report says so.
 */

extern "C" {
/* what HOST COPY 7 below calls, beyond the block above. Several of these were
   HOST COPY 8's callees too and are now referenced only by
   src/func_ov060_02116d78.c, which declares them for itself -- the
   declarations stay because they document the ov060/arm9 call surface this
   file's seat depends on. */
int _ZN6Player9GetHealthEv(void *self);        /* ov002 0x020bf548, thiscall
                                                  receiver in r0 -- a REAL
                                                  receiver, not the zero-arg
                                                  reader shape the
                                                  closestplayer guard hunts */
int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);   /* 0x02015a98 */
void _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    void *self, const void *pos, const void *rot, int horzSpeed, int unk35c,
    unsigned param1);                                        /* 0x020102b0 */
void func_02012694(int id, void *pos);                       /* 0x02012694 */
bool Bowser_IsAnimAtLastFrame(void *c);        /* ov060 0x02115a30 -- the
                                                  matched TU returns bool, so
                                                  this decl says bool: an int
                                                  decl would read the undefined
                                                  high 24 bits of MSVC's al */
void func_ov060_02111cc0(char *c, int idx, int fix);   /* ov060 0x02111cc0 --
                                                  THREE arguments: the ROM
                                                  callee reads r0/r1/r2 and the
                                                  matched TU spends r2 on its
                                                  SetAnim frame argument, so a
                                                  one-argument decl would hand
                                                  it a garbage animation id */
void func_ov060_02116518(char *self, unsigned kind, int a2, int a3);
                                                        /* ov060 0x02116518 */
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned actorID, unsigned param1, const void *pos, const void *rot,
    int areaID, int deathTableID);                           /* 0x02010e2c */
void _ZN9ActorBase18MarkForDestructionEv(void *self);        /* 0x02043824 */
int RandomIntInternal(int *seed);                            /* 0x0203b990 */
extern int data_0209e650;             /* the shared LCG seed both bodies draw */
extern short data_02082214[];         /* arm9 sin/cos table, {sin,cos} pairs */
extern int data_ov060_0211abe0[2];    /* the 0x36c SharedFilePtr; [1] is the
                                         BCA_File* the sinit's Construct
                                         resolves -- 8 bytes wide, next ov060
                                         symbol is data_ov060_0211abe8 */
}

namespace {
/* the two locals func_ov060_021140c0 hands Actor::SpawnFireball; the ROM
   builds them in its own 0x20-byte frame at sp+0x10 (Vector3) and sp+8
   (Vector3_16) and passes their addresses */
struct Ov60Vec3 { int x, y, z; };
struct Ov60Vec3_16 { unsigned short x, y, z; };
}
static_assert(sizeof(Ov60Vec3) == 12, "SpawnFireball's Vector3 is 3 words");
static_assert(sizeof(Ov60Vec3_16) == 6, "SpawnFireball's Vector3_16 is 3 halfwords");
static_assert(sizeof(short) == 2, "data_02082214 is indexed as s16[]");
static_assert(sizeof(PortPmf) == 8, "the ROM's dispatch record is 8 bytes");

/* ---- lane w7a's proof-of-life trace ---------------------------------------
 * OFF unless SM64DS_OV060_TRACE is set, stderr only, and it touches no game
 * state, so every stdout-comparing gate is byte-identical with the variable
 * set or unset.  It exists because walk_window has no per-actor state probe
 * and the two states this file just filled are exactly the ones that had to
 * be OBSERVED running. */
namespace {
bool ov60_trace(void)
{
    static int on = -1;
    if (on < 0) on = (std::getenv("SM64DS_OV060_TRACE") != 0);
    return on != 0;
}
int g_ov60_frame;                    /* Bowser::Behavior ticks = fight frames */
struct Ov60Seen { const void *who; int state; };
Ov60Seen g_ov60_seen[16];
int g_ov60_nseen;
void ov60_note(const char *what, const void *who, int state)
{
    int i;
    for (i = 0; i < g_ov60_nseen; ++i)
        if (g_ov60_seen[i].who == who)
            break;
    if (i == g_ov60_nseen) {
        if (g_ov60_nseen >= 16)
            return;                 /* trace capacity only; no game state */
        g_ov60_seen[i].who = who;
        g_ov60_seen[i].state = -1;
        ++g_ov60_nseen;
    }
    if (g_ov60_seen[i].state == state)
        return;
    if (ov60_trace())
        std::fprintf(stderr, "[ov060] frame %d: %s %p state %d -> %d\n",
                     g_ov60_frame, what, who, g_ov60_seen[i].state, state);
    g_ov60_seen[i].state = state;
}
void ov60_ran(const char *what, const void *who)
{
    if (ov60_trace())
        std::fprintf(stderr, "[ov060] frame %d: %s RAN on %p\n",
                     g_ov60_frame, what, who);
}
}

/* ============ VERIFIED HOST COPY 7: func_ov060_021140c0 ===================
 * BOWSER state 9 -- record data_ov060_0211a5a0 (overlay words 021140c0 /
 * 00000000), which __sinit_ov060_021195dc copies to
 * data_ov060_0211aed4 + 0x48, i.e. 8-byte record 9 of the twenty-record table
 * func_ov060_021128c0 indexes with *(s32 *)(c + 0x40c).  Reached from
 * src/func_ov060_021150d0.cpp and src/func_ov060_021151d4.c, both of which
 * pick it on a RandomIntInternal draw.
 *
 * ROM 0x021140c0, 0x1f4 bytes, 116 instructions + 9 literals.
 *
 * CALLS (all 7 confirmed in relocs.txt, in order of first appearance):
 *   0x021140e8 -> 0x020bf548 overlay(2)  _ZN6Player9GetHealthEv
 *   0x021140f8 -> 0x0203b990 main       RandomIntInternal
 *   0x02114160 -> 0x02015a98 main       _ZNK9Animation12WillHitFrameEi
 *   0x02114204 -> 0x020102b0 main       Actor::SpawnFireball
 *   0x02114210 -> 0x02012694 main       func_02012694
 *   0x02114218 -> 0x02115a30 overlay(60) Bowser_IsAnimAtLastFrame
 *   0x02114278 -> 0x02111cc0 overlay(60) func_ov060_02111cc0
 * RELOCATED LOADS (all 3): 0x0209e650, 0x0211abe0, 0x02082214.
 * PLAIN LITERALS (6): cccccccd, 0000000a, aaaaaaab, 00000003, 00000122,
 *   00000423 -- none of them in relocs.txt, so none of them is an address.
 * OFFSETS: 0x3fc(=0x300+0xfc) 0x3a0 0x428 0x134 0x124 0x5c 0x60 0x64 0x8c
 *   0x8e 0x90 0x74 0x423 0x40c 0x12c.
 * IMMEDIATES: 4, 16(shift), 10, 3, 1, 5, 0xe8, 0x58000, 0x1000, 0x1e000,
 *   0xa000, 0, 0x122, 0x14.
 * BRANCHES: bne 0x2114144 (skip the volley pick), beq 0x21140f4 + ble
 *   0x211413c (the `||` short circuit and its else), b 0x2114144, bne/beq
 *   0x2114214 (the two anim guards), the conditional epilogue at 0x02114220,
 *   bne 0x211426c and b 0x211427c.  Every one is reproduced structurally.
 * PORT_HOST_ABI: none -- this is a plain cdecl body; it is a host copy only
 * because no matched TU of this name exists. */
extern "C" void func_ov060_021140c0(char *r4)
{
    ov60_ran("BOWSER state 9 (021140c0)", r4);

    /* 0x021140cc..0x02114140 -- on the first frame of the state (the
       dispatcher zeroes +0x3fc on every state change) pick how many volleys
       this pass fires: 3 when the player is hurt, else 1 + rand%10%3. */
    if (*(unsigned short *)((r4 + 0x300) + 0xfc) == 0) {
        if (*(void **)(r4 + 0x3a0) == 0 ||
            _ZN6Player9GetHealthEv(*(void **)(r4 + 0x3a0)) > 4) {
            unsigned rv = (unsigned)RandomIntInternal(&data_0209e650) >> 0x10;
            *(unsigned char *)(r4 + 0x428) = (unsigned char)(rv % 10 % 3 + 1);
        } else {
            *(unsigned char *)(r4 + 0x428) = 3;
        }
    }

    /* 0x02114144..0x02114210 -- while the 0x36c animation is the one loaded,
       spit a fireball on the frame the animation crosses frame 5. */
    if (*(int *)(r4 + 0x134) == data_ov060_0211abe0[1]) {
        if (_ZNK9Animation12WillHitFrameEi(r4 + 0x124, 5) != 0) {
            Ov60Vec3 pos;
            Ov60Vec3_16 rot;
            int i;
            pos.x = *(int *)(r4 + 0x5c);
            pos.y = *(int *)(r4 + 0x60);
            pos.z = *(int *)(r4 + 0x64);
            rot.y = *(unsigned short *)(r4 + 0x8e);
            rot.x = *(unsigned short *)(r4 + 0x8c);
            rot.z = *(unsigned short *)(r4 + 0x90);
            i = rot.y >> 4;
            pos.x = data_02082214[i * 2] * 0xe8 + *(int *)(r4 + 0x5c);
            pos.z = data_02082214[i * 2 + 1] * 0xe8 + *(int *)(r4 + 0x64);
            pos.y = *(int *)(r4 + 0x60) + 0x58000;
            rot.x = 0x1000;
            _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
                r4, &pos, &rot, 0x1e000, 0xa000, 0);
            func_02012694(0x122, r4 + 0x74);
            ov60_ran("BOWSER state 9 SPIT (SpawnFireball branch)", r4);
        }
    }

    /* 0x02114214..0x02114228 -- everything past here waits for the animation
       to reach its last frame. */
    if (!Bowser_IsAnimAtLastFrame(r4))
        return;

    /* 0x0211422c..0x02114278 -- count the volley off; when the count reaches
       the number picked above, drop back to state 0.  If the spit animation
       is NOT the one loaded, load it (index 0x14) instead. */
    if (*(int *)(r4 + 0x134) == data_ov060_0211abe0[1]) {
        *(unsigned char *)(r4 + 0x423) =
            (unsigned char)(*(unsigned char *)(r4 + 0x423) + 1);
        if (*(unsigned char *)(r4 + 0x423) >= *(unsigned char *)(r4 + 0x428))
            *(int *)(r4 + 0x40c) = 0;
    } else {
        func_ov060_02111cc0(r4, 0x14, 0);
    }

    /* 0x0211427c..0x0211428c */
    *(int *)(r4 + 0x12c) = 0;
}

/* ============ HOST COPY 1: func_ov060_02112434 ============================
 * BOWSER's per-frame target/flag pass, called from Bowser::Behavior.  Line for
 * line with src/func_ov060_02112434.cpp; only the dispatch is respelled.
 * PORT_HOST_ABI: mwcc pointer-to-member stride/receiver, the Crate case. */
extern "C" void func_ov060_02112434(unsigned char *thiz)
{
    int zero[3];
    zero[0] = 0;
    zero[1] = 0;
    zero[2] = 0;
    *(int *)(thiz + 0x3f4) = Vec3_HorzDist(thiz + 0x5c, zero);
    *(short *)(thiz + 0x408) = Vec3_HorzAngle(thiz + 0x5c, zero);

    int s0 = _ZN5Actor14GetSubtractionEss(thiz, *(short *)(thiz + 0x8e),
                                          *(short *)(thiz + 0x406));
    int s1 = _ZN5Actor14GetSubtractionEss(thiz, *(short *)(thiz + 0x8e),
                                          *(short *)(thiz + 0x408));

    *(int *)(thiz + 0x418) &= ~0xff;
    if (s0 < 0x2000)
        *(int *)(thiz + 0x418) |= 2;
    if (s1 < 0x3800)
        *(int *)(thiz + 0x418) |= 4;
    if (*(int *)(thiz + 0x3f4) < 0x3e8000)
        *(int *)(thiz + 0x418) |= 0x10;
    if (*(int *)(thiz + 0x3ec) < 0x352000)
        *(int *)(thiz + 0x418) |= 8;

    {
        PortPmf *e = &data_ov060_0211aeb4[*(int *)(thiz + 0x410)];
        ((void (*)(char *))(size_t)e->fn)((char *)thiz + (e->adj >> 1));
    }

    if (*(int *)(thiz + 0x40c) == 4)
        return;

    unsigned char lo = thiz[0x41c];
    unsigned char hi = thiz[0x41d];
    if (hi == lo)
        return;
    if (hi > lo) {
        int v = lo + 0x14;
        if (v >= 0xff) {
            thiz[0x41c] = 0xff;
            return;
        }
        thiz[0x41c] = (unsigned char)(thiz[0x41c] + 0x14);
        return;
    }
    {
        int v = lo - 0x14;
        if (v <= 0)
            thiz[0x41c] = 0;
        else
            thiz[0x41c] = (unsigned char)(thiz[0x41c] - 0x14);
    }
}

/* ============ HOST COPIES 2, 3 AND 4 ARE GONE ============================
 * Run link100 lane PMFB5. func_ov060_02115b84 (BOWSER TAIL),
 * func_ov060_02118254 (SKY PLATFORM) and _ZN10BowserFire13InitResourcesEv
 * compile from src now and dispatch their own tables. What made that possible
 * is the seat below: the fourteen rows that feed data_ov060_0211ae9c,
 * _0211af74 and _0211b1ac hold __fastcall FACES instead of plain cdecl bodies,
 * because a matched TU emits `mov ecx, tab[i*8+4] / add ecx, this / call eax`
 * and pushes nothing, where these host copies called the code word cdecl with
 * `c + (adj >> 1)`.
 *
 * HOST COPY 5 IS GONE TOO, run link100 lane FWD, and the paragraph that used
 * to stand here said it had to stay. What it got right is the diagnosis:
 * src/_ZN10BowserFire8BehaviorEv.cpp defines a REAL C++ MEMBER -- `int
 * BowserFire::Behavior()`, which MSVC decorates ?Behavior@BowserFire@@QAEHXZ
 * and calls __thiscall -- while hal/actor_classes_ov060.cpp's bfire_behavior
 * face calls the FLAT C name, so retiring the copy on its own left that name
 * undefined and the link said so:
 *   actor_classes_ov060.cpp.obj : error LNK2019: unresolved external symbol
 *   __ZN10BowserFire8BehaviorEv referenced in function
 *   "int __fastcall bfire_behavior(void *,void *)"
 * What it got wrong is that this needs BowserFire's exact class SHAPE. It does
 * not: MSVC's decoration of a member function carries the method name, the
 * class name, the access specifier, the convention and the signature, and
 * nothing about the class's members or its size. port/hal/fwd_forwarders.cpp
 * declares the class the way include/BowserFire.h does and forwards the
 * receiver, and the nine decorations it emits were checked against the nine
 * matched objects with dumpbin before the link rather than assumed. Its table
 * data_ov060_0211afb4 measured clean all along -- eight whole-pair {code,0}
 * sources, a 64-byte span, ROM `add r3,r1,r0,lsl #3` at 021176e4 against
 * emitted [eax*8], /Zp4 a no-op -- so its eight seat rows are faces now like
 * the other three tables'. The sibling
 * _ZN10BowserFire13InitResourcesEv is taken because its src defines the FLAT
 * `extern "C" int _ZN10BowserFire13InitResourcesEv(char*)`, so the two halves
 * of one class split for a reason that is about the definition form and not
 * about the class.
 *
 * THE STRIDE, BOTH SIDES, per row, read at each body's OWN address out of
 * extracted/overlays/overlay_0060.bin (runs/link100/out/PMFB5/rom_gate3.txt)
 * and off each matched TU's own /FAsc listing (emit_gate3_out.txt):
 *   02115b84  ROM add r3,r1,r4,lsl #3 at 02115ba0, pool 02115c18 = 0211ae9c
 *             emitted ?data_ov060_0211ae9c@@3PAP8C@@AEXXZA[ebx*8]
 *   02117790  ROM add r3,r1,r0,lsl #3 at 021178e8, pool 02117934 = 0211af74
 *             emitted ?data_ov060_0211af74@@3PAP8Actor@@AEXXZA[eax*8]
 *   02118254  ROM add r3,r1,r0,lsl #3 at 02118264, pool 021182ac = 0211b1ac
 *             emitted ?data_ov060_0211b1ac@@3PAUEntry@@A[eax*8]
 * ROM 8 == emitted 8 on all three, and /Zp4 is a measured no-op on all three
 * (zero diff lines outside the TITLE line naming the .obj).
 *
 * THE ARITY IS ZERO ON ALL THREE and it is read off each listing: nothing is
 * pushed between the index load and `call eax`, and no `add esp, N` follows.
 * The `add esp, 4` before 02115b84's sequence is the cleanup of the PREVIOUS
 * cdecl call, not part of the dispatch.
 *
 * THE FOURTEEN SOURCE PAIRS all read {code, 0} in the overlay image at the
 * addresses their constructors copy them from, all whole-pair copies. ONE OF
 * THE THREE CONSTRUCTORS USES A SPELLING lane PMFB4's reader called a
 * FIELD-FORM FILL: src/__sinit_ov060_021195dc.c writes
 * `data_ov060_0211ae9c[0].lo = SRC;`, and `struct P4 { struct P2 lo, hi; }`
 * over `struct P2 { int a, b; }` makes `.lo` a whole eight-byte pair whose
 * ordinal is the slot, not one word of a pair. The flat slot is N * 2 +
 * ordinal, which is why 0211ae9c's three cells come from [0].lo, [0].hi and
 * [1].lo. src/__sinit_ov060_0211a000.c uses another one PMFB4's reader did not
 * know, `TAB[N] = SRC[0]`.
 *
 * EVERY ONE OF THE THREE TABLES IS FULLY COVERED, which matters more here than
 * anywhere else in this lane: an unfilled cell used to be read by a host copy
 * and handed to a switch, and after a seat it would be CALLED. The spans come
 * from config/arm9/overlays/ov060/symbols.txt -- 0211ae9c 24 bytes (3 cells),
 * 0211af74 64 (8), 0211b1ac 24 (3) -- and the constructors fill 3, 8 and 3.
 * The `[2]` bound the ov060 constructor declares for 0211ae9c is the decomp's
 * own P4 grouping, not the extent.
 *
 * THREE /alternatename DIRECTIVES, each read off the object with
 * dumpbin /symbols. */
#pragma comment(linker, "/alternatename:?data_ov060_0211ae9c@@3PAP8C@@AEXXZA=_data_ov060_0211ae9c")
#pragma comment(linker, "/alternatename:?data_ov060_0211af74@@3PAP8Actor@@AEXXZA=_data_ov060_0211af74")
#pragma comment(linker, "/alternatename:?data_ov060_0211b1ac@@3PAUEntry@@A=_data_ov060_0211b1ac")

/* ============ HOST COPY 5 IS RETIRED =====================================
 * Run link100 lane FWD. src/_ZN10BowserFire8BehaviorEv.cpp dispatches
 * data_ov060_0211afb4 now and port/hal/fwd_forwarders.cpp defines the flat C
 * name the actor-class face calls. The w7a ov60_note("BOWSERFIRE", ...) trace
 * line went with the body; BOWSER's own trace in HOST COPY 6 is untouched, and
 * the matched TU is not the place to put a trace back.
 */

/* ============ HOST COPY 6: _ZN6Bowser8BehaviorEv ==========================
 * NOT a pointer-to-member fault -- src/_ZN6Bowser8BehaviorEv.cpp does not
 * COMPILE under MSVC at all. It declares six shadow-class members and then
 * re-declares each of them at namespace scope:
 *
 *     struct Actor { Actor *ClosestPlayer(); ... };
 *     Actor *Actor::ClosestPlayer();          <- C2761
 *
 * which mwcc accepts and MSVC rejects with C2761 "redeclaration of member is
 * not allowed", six times. Measured on this tree: the TU is the only compile
 * failure in the whole wave-6 landing. (The by-address decomp-side fix for
 * this shape is wave-6 cut item 9 and belongs to main, not to the port.)
 *
 * Transcribed line for line, offsets from include/Bowser.h. The
 * ClosestPlayer call site passes the actor, which is the value the ROM leaves
 * in r0 -- the receiver seam the closestplayer guard watches for does not open
 * here (the src spells it `((Actor *)this)->ClosestPlayer()`, a real receiver,
 * not the zero-arg reader shape). */
extern "C" {
int RandomIntInternal(int *seed);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void *_ZN5Actor15FindWithActorIDEjPS_(unsigned id, void *prev);
void _ZN9Animation7AdvanceEv(void *a);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
    void *self, const void *v);
void func_ov060_02111a28(char *c);
void func_ov060_0211577c(char *c);
extern int data_0209e650;
extern char *data_0209f318;
}
/* PORT_HOST_ABI: MSVC C2761 rejects the matched TU's out-of-class member
   redeclarations; body is the matched source's line for line. */
extern "C" int _ZN6Bowser8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    ++g_ov60_frame;                                   /* w7a trace, stderr */
    ov60_note("BOWSER", c, *(int *)(c + 0x40c));
    RandomIntInternal(&data_0209e650);
    *(int *)(c + 0x3a0) = (int)(size_t)_ZN5Actor13ClosestPlayerEv(c);
    if (*(char **)(c + 0x3a0) != 0) {
        char *t = *(char **)(c + 0x3a0);
        *(short *)(c + 0x406) = Vec3_HorzAngle(c + 0x5c, t + 0x5c);
        *(int *)(c + 0x3ec) = Vec3_HorzDist(c + 0x5c, t + 0x5c);
    } else {
        *(short *)(c + 0x406) = *(short *)(c + 0x8e);
        *(int *)(c + 0x3ec) = ~0x80000000;
    }
    func_ov060_02112434((unsigned char *)c);
    func_ov060_02111a28(c);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x130) = *(int *)(c + 0x3f8);
    _ZN9Animation7AdvanceEv(c + 0x124);
    func_ov060_0211577c(c);
    *(char **)(data_0209f318 + 0x114) = c;
    _ZN12CylinderClsn5ClearEv(c + 0x360);
    {
        int v[3];
        v[2] = 0x50000;
        v[0] = 0;
        v[1] = 0;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
            c + 0x360, v);
    }
    _ZN12CylinderClsn6UpdateEv(c + 0x360);
    if (*(unsigned char *)(c + 0x42b) != 0) {
        if (_ZN5Actor15FindWithActorIDEjPS_(0x10d, 0) == 0)
            *(unsigned char *)(c + 0x42b) = 0;
    }
    return 1;
}

/* ============ THE SEAT ====================================================
 * Rewrites each SOURCE static's code word with its host body BEFORE the sinits
 * copy it into the runtime table (the WaterBomb / LakituBro / Crate reading:
 * one less mapping to get wrong), each checked against the ROM address the
 * body was compiled from, so a mount pointing at the wrong bytes says so
 * instead of calling into the overlay image.  All 50 adj halves are 0. */
#define OV60_FACE(tag, slot, sym)                                         \
    static void __fastcall ov60_##tag##_s##slot(void *self, void *dead_edx)\
    {                                                                     \
        (void)dead_edx;                                                   \
        sym((char *)self);                                                \
    }

/* data_ov060_0211ae9c -- BOWSER TAIL, three */
OV60_FACE(ae9c, 0, func_ov060_02115d68)
OV60_FACE(ae9c, 1, func_ov060_02115d50)
OV60_FACE(ae9c, 2, func_ov060_02115c1c)

/* data_ov060_0211af74 -- BOWSER FIRE init, eight */
OV60_FACE(af74, 0, func_ov060_021167c8)
OV60_FACE(af74, 1, func_ov060_02116b18)
OV60_FACE(af74, 2, func_ov060_02116c68)
OV60_FACE(af74, 3, func_ov060_021167c8)
OV60_FACE(af74, 4, func_ov060_021169b0)
OV60_FACE(af74, 5, func_ov060_02116f74)
OV60_FACE(af74, 6, func_ov060_0211722c)
OV60_FACE(af74, 7, func_ov060_021171e8)

/* data_ov060_0211b1ac -- SKY PLATFORM, three */
OV60_FACE(b1ac, 0, func_ov060_021181b4)
OV60_FACE(b1ac, 1, func_ov060_021180e0)
OV60_FACE(b1ac, 2, func_ov060_02117db8)

/* data_ov060_0211afb4 -- BOWSER FIRE behaviour, eight. Run link100 lane FWD:
   HOST COPY 5 is gone and src/_ZN10BowserFire8BehaviorEv.cpp dispatches this
   table itself, so its eight cells take faces like the other three tables.
   ONE FACE PER CELL: cells 6 and 7 carry the SAME code word (0x02116f90) and
   stay distinguishable that way. */
OV60_FACE(afb4, 0, func_ov060_0211747c)
OV60_FACE(afb4, 1, func_ov060_021169f8)
OV60_FACE(afb4, 2, func_ov060_02116b68)
OV60_FACE(afb4, 3, func_ov060_021167ec)
OV60_FACE(afb4, 4, func_ov060_021168c4)
OV60_FACE(afb4, 5, func_ov060_02116d78)
OV60_FACE(afb4, 6, func_ov060_02116f90)
OV60_FACE(afb4, 7, func_ov060_02116f90)

namespace {
/* the host column holds BOTH plain cdecl bodies (for the tables whose
   dispatcher is still a host copy in this file, including BOWSER FIRE's
   0211afb4) and the __fastcall faces above (for the three tables run
   link100 lane PMFB5 seated), so it is a void* and each row casts. */
struct Seat { PortPmf *slot; unsigned rom; void *host; const char *tab; };
const Seat g_ov060_states[] = {
    /* 0x0211aeb4 -- BOWSER, four */
    {data_ov060_0211a578, 0x021128c0, (void *)func_ov060_021128c0, "aeb4[0]"},
    {data_ov060_0211a510, 0x02112724, (void *)func_ov060_02112724, "aeb4[1]"},
    {data_ov060_0211a518, 0x021125f0, (void *)func_ov060_021125f0, "aeb4[2]"},
    {data_ov060_0211a520, 0x021125f0, (void *)func_ov060_021125f0, "aeb4[3]"},
    /* 0x0211aed4 -- BOWSER, twenty */
    {data_ov060_0211a568, 0x02114f88, (void *)func_ov060_02114f88, "aed4[0]"},
    {data_ov060_0211a560, 0x02113b5c, (void *)func_ov060_02113b5c, "aed4[1]"},
    {data_ov060_0211a538, 0x02113740, (void *)func_ov060_02113740, "aed4[2]"},
    {data_ov060_0211a4e8, 0x02113710, (void *)func_ov060_02113710, "aed4[3]"},
    {data_ov060_0211a4f0, 0x02112ddc, (void *)func_ov060_02112ddc, "aed4[4]"},
    {data_ov060_0211a500, 0x021154e8, (void *)func_ov060_021154e8, "aed4[5]"},
    {data_ov060_0211a540, 0x021153f8, (void *)func_ov060_021153f8, "aed4[6]"},
    {data_ov060_0211a5b8, 0x02113d8c, (void *)func_ov060_02113d8c, "aed4[7]"},
    {data_ov060_0211a5b0, 0x02114858, (void *)func_ov060_02114858, "aed4[8]"},
    {data_ov060_0211a5a0, 0x021140c0, (void *)func_ov060_021140c0, "aed4[9] HOLE"},
    {data_ov060_0211a5a8, 0x021142b4, (void *)func_ov060_021142b4, "aed4[10]"},
    {data_ov060_0211a530, 0x02113fcc, (void *)func_ov060_02113fcc, "aed4[11]"},
    {data_ov060_0211a508, 0x021146d0, (void *)func_ov060_021146d0, "aed4[12]"},
    {data_ov060_0211a528, 0x021143b8, (void *)func_ov060_021143b8, "aed4[13]"},
    {data_ov060_0211a598, 0x02114d08, (void *)func_ov060_02114d08, "aed4[14]"},
    {data_ov060_0211a590, 0x02114e9c, (void *)func_ov060_02114e9c, "aed4[15]"},
    {data_ov060_0211a588, 0x02114b60, (void *)func_ov060_02114b60, "aed4[16]"},
    {data_ov060_0211a580, 0x02114300, (void *)func_ov060_02114300, "aed4[17]"},
    {data_ov060_0211a4f8, 0x02114ff8, (void *)func_ov060_02114ff8, "aed4[18]"},
    {data_ov060_0211a570, 0x02112bfc, (void *)func_ov060_02112bfc, "aed4[19]"},
    /* 0x0211ae9c -- BOWSER TAIL, three */
    {data_ov060_0211a558, 0x02115d68, (void *)ov60_ae9c_s0, "ae9c[0]"},
    {data_ov060_0211a550, 0x02115d50, (void *)ov60_ae9c_s1, "ae9c[1]"},
    {data_ov060_0211a548, 0x02115c1c, (void *)ov60_ae9c_s2, "ae9c[2]"},
    /* 0x0211afb4 -- BOWSER FIRE behaviour, eight (faces, run link100 FWD) */
    {data_ov060_0211a794, 0x0211747c, (void *)ov60_afb4_s0, "afb4[0]"},
    {data_ov060_0211a78c, 0x021169f8, (void *)ov60_afb4_s1, "afb4[1]"},
    {data_ov060_0211a76c, 0x02116b68, (void *)ov60_afb4_s2, "afb4[2]"},
    {data_ov060_0211a77c, 0x021167ec, (void *)ov60_afb4_s3, "afb4[3]"},
    {data_ov060_0211a764, 0x021168c4, (void *)ov60_afb4_s4, "afb4[4]"},
    /* not a HOLE any more -- w9-harvest seated main's byte-matched
       src/func_ov060_02116d78.c here in place of w7a's host copy. */
    {data_ov060_0211a774, 0x02116d78, (void *)ov60_afb4_s5, "afb4[5]"},
    {data_ov060_0211a744, 0x02116f90, (void *)ov60_afb4_s6, "afb4[6]"},
    {data_ov060_0211a784, 0x02116f90, (void *)ov60_afb4_s7, "afb4[7]"},
    /* 0x0211af74 -- BOWSER FIRE init, eight */
    {data_ov060_0211a75c, 0x021167c8, (void *)ov60_af74_s0, "af74[0]"},
    {data_ov060_0211a754, 0x02116b18, (void *)ov60_af74_s1, "af74[1]"},
    {data_ov060_0211a73c, 0x02116c68, (void *)ov60_af74_s2, "af74[2]"},
    {data_ov060_0211a74c, 0x021167c8, (void *)ov60_af74_s3, "af74[3]"},
    {data_ov060_0211a734, 0x021169b0, (void *)ov60_af74_s4, "af74[4]"},
    {data_ov060_0211a7ac, 0x02116f74, (void *)ov60_af74_s5, "af74[5]"},
    {data_ov060_0211a7a4, 0x0211722c, (void *)ov60_af74_s6, "af74[6]"},
    {data_ov060_0211a79c, 0x021171e8, (void *)ov60_af74_s7, "af74[7]"},
    /* 0x0211b1d8 -- SPIKE BOMB, four */
    {data_ov060_0211aa30, 0x02118970, (void *)func_ov060_02118970, "b1d8[0]"},
    {data_ov060_0211aa48, 0x021188e8, (void *)func_ov060_021188e8, "b1d8[1]"},
    {data_ov060_0211aa40, 0x02118834, (void *)func_ov060_02118834, "b1d8[2]"},
    {data_ov060_0211aa38, 0x02118728, (void *)func_ov060_02118728, "b1d8[3]"},
    /* 0x0211b1ac -- SKY PLATFORM, three (sinit copy order) */
    {data_ov060_0211a938, 0x021181b4, (void *)ov60_b1ac_s0, "b1ac[0]"},
    {data_ov060_0211a940, 0x021180e0, (void *)ov60_b1ac_s1, "b1ac[1]"},
    {data_ov060_0211a930, 0x02117db8, (void *)ov60_b1ac_s2, "b1ac[2]"},
};
}  /* namespace */

extern "C" void port_ov060_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_ov060_states / sizeof g_ov060_states[0];
         ++i) {
        PortPmf *p = g_ov060_states[i].slot;
        if (p->fn != g_ov060_states[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: ov060 state %s: the mount holds "
                         "%08x/%d, the ROM's own record says %08x/0 -- WRONG "
                         "BYTES\n", g_ov060_states[i].tab, p->fn, p->adj,
                         g_ov060_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_ov060_states[i].host;
    }
}
