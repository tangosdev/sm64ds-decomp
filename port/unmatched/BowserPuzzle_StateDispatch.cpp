/* HOST COPIES of the three BowserPuzzle (ov064, gate 179) pointer-to-member
 * dispatchers, and the seats of the two tables they read.
 *
 * BOWSER_PUZZLE_MANAGER (79, daObjFl_Puzzle_c) and BOWSER_PUZZLE_PIECE (78,
 * daObjFl_Coin_c) each drive a state machine through an mwcc {function, delta}
 * table, and each dispatcher forms its pointer-to-member over a LOCAL or
 * FORWARD-DECLARED struct -- so MSVC hands it the general (worst-case-inheritance)
 * PMF representation, a multi-word value plus a this-adjust thunk, not the single
 * code pointer a complete single-inheritance class gets. The Manager's Behavior
 * also INDEXES its table by idx, striding it at MSVC's sizeof(PMF) (4) instead of
 * the ROM's 8. Either fault mangles `this` into a state index -- the
 * KnockDownPlank / MontyMole / Scuttlebug case exactly. Each body below is the
 * matched source's control flow line for line; only the dispatch is read as a
 * plain { function, 0 } and the function called with `this`.
 *
 * The words the sinits copy are the overlay image's own -- DS CODE ADDRESSES, the
 * ovdata contract, matched against ov064's relocs. Each seat rewrites the SOURCE
 * records' first words with the host bodies BEFORE the sinit copies them into the
 * bss table (the MontyMole / Crate reading: seat the source, before the copy, one
 * less mapping to get wrong), each checked against the ROM address the body was
 * compiled from, so a mount pointing at the wrong bytes says so instead of copying
 * an overlay-image address into the live table. All eight .delta halves are 0
 * (non-virtual complete-class form: no reloc at any +4 offset in the overlay
 * bytes), and all eight target functions are matched src (slice_gate179.txt), so
 * there are no state traps.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* ==== BOWSER_PUZZLE_MANAGER =============================================== */

/* the Manager Behavior's three C-linkage helpers, all matched src */
void func_ov064_02118ee4(void *c);   /* the pre-dispatch step */
void func_ov064_02119010(void *c);   /* the matrix step after */
void func_ov064_02118fa4(void *c);   /* the collider transform */
char *_ZN5Actor10FindWithIDEj(unsigned int id);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);

/* the six Manager state functions, all matched src (the daBbl_c-family movers +
   the LavaBubble Kill/Spawn) */
void func_ov064_02118da0(void *c);   /* table slot 0 <- data_ov064_0211bf80 */
void func_ov064_02118d3c(void *c);   /* table slot 1 <- data_ov064_0211bf68 */
void func_ov064_02118d20(void *c);   /* table slot 2 <- data_ov064_0211bf90 */
void func_ov064_02118d08(void *c);   /* table slot 3 <- data_ov064_0211bf70 */
void func_ov064_02118cec(void *c);   /* table slot 4 <- data_ov064_0211bf88 */
void func_ov064_02118cd4(void *c);   /* table slot 5 <- data_ov064_0211bf78 */

/* the RUNTIME table Manager::Behavior reads (bss, six 8-byte records, filled by
   __sinit_ov064_0211b1d4) */
extern PortPmf data_ov064_0211c904[6];
/* the six SOURCE records the sinit copies from (data, {fn, 0}) */
extern PortPmf data_ov064_0211bf68[], data_ov064_0211bf70[],
    data_ov064_0211bf78[], data_ov064_0211bf80[], data_ov064_0211bf88[],
    data_ov064_0211bf90[];

/* ==== BOWSER_PUZZLE_PIECE ================================================= */

/* the Piece Behavior's one C-linkage helper, matched src */
void func_ov064_0211987c(void *c);
unsigned short DecIfAbove0_Short(unsigned short *p);
/* the two Piece state functions, matched src */
int func_ov064_021193b4(void *c);    /* the .b half <- data_ov064_0211c2dc (per-frame) */
int func_ov064_021197fc(void *c);    /* the .a half <- data_ov064_0211c2e4 (the reset) */

/* the RUNTIME Obj the Piece points its this+0x300 at (bss, {a:{fn,0}, b:{fn,0}},
   filled by __sinit_ov064_0211b4dc) */
extern PortPmf data_ov064_0211c934[2];    /* [0] = .a, [1] = .b */
/* the two SOURCE records the Piece sinit copies from */
extern PortPmf data_ov064_0211c2dc[];     /* {fn=021193b4, 0} -> c934[1] (.b) */
extern PortPmf data_ov064_0211c2e4[];     /* {fn=021197fc, 0} -> c934[0] (.a) */

}  /* extern "C" */

/* HOST COPY RETIRED, run link100 lane FWD. BowserPuzzleManager::Behavior
 * dispatches this table from src/_ZN19BowserPuzzleManager8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */

/* func_ov064_0211982c IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov064_0211982c.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN17BowserPuzzlePiece8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

/* ---- the two seats -------------------------------------------------------- */

/* RUN link100, LANE FWD: THIS TABLE'S CELLS HOLD __fastcall FACES NOW.
 * The host copy below is retired and the matched TU dispatches the table
 * itself. A matched TU dispatches a pointer to member as
 *     mov ecx, TAB[i*8+4] / mov eax, TAB[i*8] / add ecx, this / call eax
 * -- receiver in ecx, NOTHING pushed and no `add esp` after, read off the TU's
 * own /FAsc listing (runs/link100/out/FWD/emit_gate1_out.txt) -- where this
 * seat used to install a plain cdecl body that takes its self off the stack.
 * A zero-argument __fastcall face has exactly the convention the matched TU
 * calls with, and hands the receiver on as the cdecl argument the ROM's own
 * state bodies take. One face per CELL, not per body: two cells that carry the
 * same code word stay distinguishable (lane PMFB5's rule).
 */
 /* THE PIECE'S TWO ROWS BELOW DO NOT CHANGE. They feed the Obj record that
  * BowserPuzzlePiece::Behavior reads, and that dispatcher is still a host copy
  * in this file calling the cell cdecl with an explicit self.
  */
#define BPM_FACE(cell, sym)                                               \
    static void __fastcall bpm_c##cell(void *self, void *dead_edx)        \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

BPM_FACE(0, func_ov064_02118da0)
BPM_FACE(1, func_ov064_02118d3c)
BPM_FACE(2, func_ov064_02118d20)
BPM_FACE(3, func_ov064_02118d08)
BPM_FACE(4, func_ov064_02118cec)
BPM_FACE(5, func_ov064_02118cd4)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_manager_states[] = {
    {data_ov064_0211bf80, 0x02118da0, (void *)bpm_c0},  /* state 0 */
    {data_ov064_0211bf68, 0x02118d3c, (void *)bpm_c1},  /* state 1 */
    {data_ov064_0211bf90, 0x02118d20, (void *)bpm_c2},  /* state 2 */
    {data_ov064_0211bf70, 0x02118d08, (void *)bpm_c3},  /* state 3 */
    {data_ov064_0211bf88, 0x02118cec, (void *)bpm_c4},  /* state 4 */
    {data_ov064_0211bf78, 0x02118cd4, (void *)bpm_c5},  /* state 5 */
};

/* run link100 lane PMFB7 gate 1: the per-frame half's face. */
static void __fastcall bpp_perframe_face(void *self, void *dead_edx)
{
    (void)dead_edx;
    func_ov064_021193b4(self);
}

extern "C" void port_bowser_puzzle_manager_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_manager_states /
                             sizeof g_manager_states[0]; ++i) {
        PortPmf *p = g_manager_states[i].slot;
        if (p->fn != g_manager_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: BowserPuzzleManager source %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_manager_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_manager_states[i].host;
    }
}

/* the Piece has one 16-byte Obj = two source PMFs, both reachable: .a (the reset,
   called once from func_ov064_0211982c at init) and .b (the per-frame state,
   called from Behavior). */
static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_piece_states[] = {
    {data_ov064_0211c2e4, 0x021197fc, func_ov064_021197fc},  /* .a, the reset */
    /* run link100 lane PMFB7 gate 1: the .b half is a FACE now --
       src/_ZN17BowserPuzzlePiece8BehaviorEv.cpp dispatches it as a real
       pointer to member (mov eax,[cell+8] / mov ecx,[cell+12] / add ecx,this /
       call eax, arity 0). The .a half keeps its plain cdecl body. */
    {data_ov064_0211c2dc, 0x021193b4,
     (int (*)(void *))(void *)bpp_perframe_face},  /* .b, the per-frame */
};

extern "C" void port_bowser_puzzle_piece_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_piece_states /
                             sizeof g_piece_states[0]; ++i) {
        PortPmf *p = g_piece_states[i].slot;
        if (p->fn != g_piece_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: BowserPuzzlePiece source %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_piece_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_piece_states[i].host;
    }
}
