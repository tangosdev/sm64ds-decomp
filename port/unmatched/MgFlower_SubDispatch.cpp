/* PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, dScMgFlower_c's half -- and
 * it is a form of the wall that NEITHER a link NOR a `::*` sweep can find.
 * Run mg5, lane FLW.
 *
 * Read unmatched/MgBase_StateDispatch.cpp's header first for the ROM
 * disassembly of the dispatch sequence, the finding that the constructors copy
 * every pair correctly at the right stride, and the rulebook corollary about
 * spellings. None of that is repeated here.
 *
 * ---- A THIRD HIDING PLACE, AND IT IS THE POINT OF THIS FILE ---------------
 *
 * port/mg_fanout_costs.txt section 4 measures that the wall is twelve TUs
 * where the link names six, and enumerates exactly two ways a dispatcher can
 * be silent:
 *
 *     TWO declare their PMF table inside extern "C", so the global mangles as
 *       the plain C name the mount already defines.
 *     FOUR have no PMF global at all -- the member pointer is a FIELD of the
 *       object, so nothing external is emitted in any linkage.
 *
 * and it concludes "the fan-out sweeps for `::*` in its own slice before it
 * believes an LNK1120". THIS LANE DID THAT SWEEP, IT CAME BACK CLEAN, AND THE
 * WALL WAS STILL THERE. src/func_ov006_020c3d18.cpp contains no `::*`, no
 * member-pointer type and no PMF global; it OPEN-CODES the Itanium sequence in
 * plain ints:
 *
 *     int v = *(int*)(c+4);            // the adjustment
 *     void *p = c + (v >> 1);          // this, advanced
 *     if (v & 1) {                     // the virtual bit
 *         int *vt = *(int**)p;
 *         fn = (int*)*(int*)((char*)vt + *(int*)c);
 *     } else {
 *         fn = (int*)*(int*)c;         // the code word
 *     }
 *     ((void(*)(void*))fn)(p);
 *
 * and the ROM at 0x020c3d18 is that sequence verbatim, five instructions:
 *
 *     020c3d20  ldr   r1,[r4,#4]
 *     020c3d24  add   r0,r4,r1,asr #1
 *     020c3d28  ands  r1,r1,#1
 *     020c3d2c  ldrne r2,[r0] / ldrne r1,[r4] / ldrne r1,[r2,r1]
 *     020c3d38  ldreq r1,[r4]
 *     020c3d3c  blx   r1
 *
 * So the detector list needs a third row, and it is the worst of the three:
 *
 *     A DISPATCHER THAT OPEN-CODES THE ITANIUM SEQUENCE IN PLAIN INTS emits no
 *     symbol, contains no member-pointer token, compiles clean, LINKS CLEAN,
 *     and jumps to a raw DS address the first time it runs.
 *
 * Neither of section 4's two searches finds it. What found it was RUNNING the
 * scene:
 *
 *     FAULT c0000005 at eip 020c395c accessing 020c395c
 *       port_scene_tick -> port_actor_tick -> func_02043288
 *         -> flw_beh (slot 6) -> _ZN13dScMgFlower_c8BehaviorEv -> func_ov006_020c3d18+0x23
 *
 * eip IS the DS address. That is what an unrouted code word looks like when it
 * is finally called, and it is why "the link closed" is not evidence that the
 * wall is answered.
 *
 * ---- THE FOUR ADDRESSES, READ OUT OF THE ROM ------------------------------
 *
 * Not guessed and not read from a running table. Three matched TUs copy an
 * eight-byte {code, adjustment} pair into the object's own head, and a fourth
 * global is the sentinel a predicate compares against. All four pairs read out
 * of extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *   pair address          words                  written by
 *   -------------------   --------------------   ---------------------------
 *   data_ov006_0213aec0   020c3ad8  00000000     func_ov006_020c3adc
 *   data_ov006_0213aec8   020c395c  00000000     func_ov006_020c3990
 *   data_ov006_0213aed0   020c38ac  00000000     func_ov006_020c38b0
 *   data_ov006_0213aed8   020c3904  00000000     func_ov006_020c3908
 *   data_ov006_0213aee0   020c3ad8  00000000     NOT a state: the sentinel
 *                                                func_ov006_020c3b80 tests
 *                                                the live pair against
 *
 * EVERY ADJUSTMENT IS ZERO, which is the same measurement MgBase_StateDispatch
 * records for its own closure, so `v >> 1` is 0, `p` is `c`, and the virtual
 * branch is never taken. The virtual arm is REPORTED rather than implemented
 * below, for that file's reason: a dispatch shape nobody has measured is
 * exactly the plausible body port/tools/inferred_stub_guard exists to refuse.
 *
 * ALL FOUR BODIES HAVE MATCHED src TUs and all four are already in
 * port/slice_flw.txt, so this costs no decomp and creates no floor. Three of
 * them are four-byte `bx lr` stubs in the ROM (0x020c3ad8, 0x020c38ac,
 * 0x020c3904 -- the idle states of a three-state animation machine) and the
 * fourth, 0x020c395c, is the one that does work: it returns early unless
 * Animation::Finished, then re-arms through func_ov006_020c3adc.
 *
 * ---- THE STORED WORD IS DELIBERATELY NOT REWRITTEN ------------------------
 *
 * The alternative to routing at the dispatch site is the ov085 / ov100
 * port_*_states_seat shape: write HOST addresses over the pairs before
 * anything copies them. IT WOULD BREAK A PREDICATE HERE, and that is worth
 * recording because the fan-out will meet the same choice.
 * src/func_ov006_020c3b80.c compares the object's live pair against
 * data_ov006_0213aee0 WORD FOR WORD:
 *
 *     int *g = data_ov006_0213aee0;
 *     if (c[0] == g[0] && (c[1] == g[1] || c[0] == 0)) ...
 *
 * -- an "is this object in the idle state" test, and src/_ZN13dScMgFlower_c8BehaviorEv.cpp
 * (the class's Behavior) calls it twice. Rewriting the stored words would make
 * that comparison ask whether a host address equals a DS address, which is
 * false forever, and the minigame would silently never see its own idle state.
 * Routing at the dispatch site leaves every reader of the word correct and
 * touches nothing but the one call. This is the MgCurling_StateDispatch choice
 * and it is made for a sharper reason here.
 *
 * ---- WHY IT DOES NOT ROUTE THROUGH port_mg_call0 --------------------------
 *
 * MgBase_StateDispatch.cpp's port_mg_call0 is "the one entry point ... so
 * there is exactly one place that decides what an adjustment word means", and
 * routing through it would be the tidier shape. It cannot be done from here
 * without editing another class's file: its ov006 arm is port_mg_try_ov006_0,
 * which is DEFINED in unmatched/MgCurling_StateDispatch.cpp and DECLARED in
 * MgBase_StateDispatch.cpp, so a second class cannot add cases to it.
 * port/slice_mga361.txt already flagged this exact singleton --
 * "before the fan-out widens, that pair wants to become a registry the class
 * files add themselves to, rather than a hook one class owns" -- and this is
 * the second lane to hit it. The refactor is not this lane's to take across
 * two other lanes' files, so this file carries its own switch AND its own
 * adjustment ruling, spelled identically, and reports its own counts.
 *
 * IT DEFINES func_ov006_020c3d18, so src/func_ov006_020c3d18.cpp is OUT of
 * port/slice_flw.txt. That is the second linked function this seat trades for
 * a working dispatch, and it is the same trade MG2 made five times.
 */

#include <cstdio>
#include <cstdlib>   /* std::abort, for the seat (run linkfull lane PMFMG1) */

extern "C" {

/* the four state bodies, all matched, all in the slice. Three take no
   parameter in src because their ROM bodies are a bare `bx lr` and never look
   at r0; they are declared here with the receiver the ROM passes so that this
   file's call sites all read the same, and the extra cdecl argument the callee
   ignores is cleaned by this caller. */
void func_ov006_020c3ad8(void *self);
void func_ov006_020c38ac(void *self);
void func_ov006_020c3904(void *self);
int  func_ov006_020c395c(int *self);

/* the rest of the dispatching TU, verbatim from src */
int  _ZN9Animation7AdvanceEv(void *);
int  func_ov006_020c35e8(void *);

void func_ov006_020c3d18(char *c);
unsigned port_mg_flower_sub_calls(void);
unsigned port_mg_flower_sub_unknown(void);

}  /* extern "C" */

static unsigned g_sub_calls, g_sub_unknown;

extern "C" unsigned port_mg_flower_sub_calls(void)   { return g_sub_calls; }
extern "C" unsigned port_mg_flower_sub_unknown(void) { return g_sub_unknown; }

static void sub_unhandled(const char *why, unsigned code, int adj)
{
    static int said;
    ++g_sub_unknown;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] dScMgFlower_c SUB-OBJECT STATE %s: "
                     "code %08x adj %d. No state was run. "
                     "port/unmatched/MgFlower_SubDispatch.cpp\n",
                     why, code, adj);
        std::fflush(stderr);
    }
}

/* The address switch. Four measured cases and no default body: an address this
   does not know is REPORTED, never guessed at, which is the whole difference
   between this and the raw `blx r1` it replaces. */
static void sub_call(void *p, unsigned code, int adj)
{
    ++g_sub_calls;
    if (code == 0)
        return;                     /* the ROM's own null-pmf guard */
    if (adj != 0) {
        sub_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                      "this closure has", code, adj);
        return;
    }
    switch (code) {
    case 0x020c3ad8u: func_ov006_020c3ad8(p);         return;
    case 0x020c38acu: func_ov006_020c38ac(p);         return;
    case 0x020c3904u: func_ov006_020c3904(p);         return;
    case 0x020c395cu: func_ov006_020c395c((int *)p);  return;
    default: break;
    }
    sub_unhandled("UNHANDLED", code, adj);
}

/* src/func_ov006_020c3d18.cpp -- RETIRED, run linkfull lane PMFMG1. The five
   .data pairs that can ever reach the object's head are seated below with
   host words, so the matched TU's own open-coded call reaches a host body.

   ---- WHY THE HEADER'S "DELIBERATELY NOT REWRITTEN" NO LONGER HOLDS ----------
   The header above is right that rewriting ONE side breaks the idle test:
   src/func_ov006_020c3b80.c compares the object's live pair with
   data_ov006_0213aee0 word for word. It is answered the way lanes MGWRITER
   (MgMemory2_FieldPmf.cpp) and FWD gate 3 (MgTrampolineTerror_MarioDispatch
   .cpp) answered it: ONE HOST WORD PER CODE WORD, written into every pair that
   holds it, the sentinel included. 0213aec0 and the sentinel 0213aee0 both
   hold 0x020c3ad8 and both get the same wrapper, so the comparison answers
   exactly what the cartridge answers, and `c[0] == 0` stays zero on an
   unwritten head.

   THE UNIVERSE IS CLOSED BY THE ROM (runs/linkfull/out/PMFMG1/
   rom_records.txt, rom_flower_code_refs.txt): the four code words have
   exactly five relocations in the whole of ov006, all five `load` rows from
   these five .data pairs, every one {code, 0}; no literal pool in code holds
   any of them, so nothing compares the head against a DS constant. The four
   writers (020c3adc, 020c3990, 020c38b0, 020c3908) copy a pair whole and the
   one reader by value is 020c3b80.

   __cdecl, AND THE LISTING SAYS WHY (runs/linkfull/out/PMFMG1/
   func_ov006_020c3d18.asm): `push ecx / call eax`, then the caller's own
   `add esp, 8` covering this push and the Animation::Advance one. The receiver
   is on the stack and the caller cleans it, so each pair holds a __cdecl
   wrapper that counts what sub_call counted and calls the body. Three of the
   four bodies take nothing (their ROM bodies are `bx lr`); a cdecl callee that
   declares nothing reads nothing, and the caller cleans the word it pushed. */
static void fw_c3ad8(void *self) { ++g_sub_calls; func_ov006_020c3ad8(self); }
static void fw_c395c(void *self) { ++g_sub_calls; func_ov006_020c395c((int *)self); }
static void fw_c38ac(void *self) { ++g_sub_calls; func_ov006_020c38ac(self); }
static void fw_c3904(void *self) { ++g_sub_calls; func_ov006_020c3904(self); }

struct MgFlwPair { unsigned code; int adj; };
extern "C" {
extern MgFlwPair data_ov006_0213aec0;   /* written  src/func_ov006_020c3adc.c */
extern MgFlwPair data_ov006_0213aec8;   /* written  src/func_ov006_020c3990.c */
extern MgFlwPair data_ov006_0213aed0;   /* written  src/func_ov006_020c38b0.c */
extern MgFlwPair data_ov006_0213aed8;   /* written  src/func_ov006_020c3908.c */
extern MgFlwPair data_ov006_0213aee0;   /* SENTINEL src/func_ov006_020c3b80.c */
void port_mg_flower_sub_seat(void);
}

/* Called from port_mg_framework_states_seat (unmatched/MgBase_StateDispatch
   .cpp), which hal/scene_mg.cpp runs once per process after the thirty-five
   overlay constructors and before any minigame object exists, so no writer
   can have copied a pair yet. Compare against the cartridge's own word and a
   zero adjustment first; either mismatch is a loud abort. */
extern "C" void port_mg_flower_sub_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct { MgFlwPair *p; unsigned rom; void *host; const char *what; }
    seats[] = {
        {&data_ov006_0213aec0, 0x020c3ad8u, (void *)fw_c3ad8, "0213aec0 written"},
        {&data_ov006_0213aec8, 0x020c395cu, (void *)fw_c395c, "0213aec8 written"},
        {&data_ov006_0213aed0, 0x020c38acu, (void *)fw_c38ac, "0213aed0 written"},
        {&data_ov006_0213aed8, 0x020c3904u, (void *)fw_c3904, "0213aed8 written"},
        {&data_ov006_0213aee0, 0x020c3ad8u, (void *)fw_c3ad8, "0213aee0 SENTINEL"},
    };
    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        if (seats[i].p->code != seats[i].rom || seats[i].p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgFlower_c sub-object pair %s: the "
                         "mount holds %08x/%d, the ROM's own word says %08x/0 "
                         "-- WRONG BYTES\n", seats[i].what, seats[i].p->code,
                         seats[i].p->adj, seats[i].rom);
            std::abort();
        }
        seats[i].p->code = (unsigned)(size_t)seats[i].host;
    }
}
