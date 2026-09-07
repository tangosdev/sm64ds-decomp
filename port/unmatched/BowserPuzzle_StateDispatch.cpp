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

/* PORT_HOST_ABI: mwcc pointer-to-member on a local struct, indexed by idx. The
   matched TU forms `(this->*data_ov064_0211c904[idx].pmf)()` over a local
   BowserPuzzleManager; here the record is read as a plain { fn, 0 } and the fn
   called with `this`. The pre/post steps and the link-actor bump are the matched
   Behavior's own, spelled qualified. idx lives at this+0x336. */
extern "C" int _ZN19BowserPuzzleManager8BehaviorEv(void *self)
{
    char *c = (char *)self;
    func_ov064_02118ee4(c);
    /* (this->*state[idx].pmf)() */
    {
        unsigned idx = *(unsigned char *)(c + 0x336);
        ((void (*)(void *))(size_t)data_ov064_0211c904[idx].fn)(c);
    }
    {
        unsigned int id = *(unsigned int *)(c + 0x320);
        char *p = 0;
        if (id != 0)
            p = _ZN5Actor10FindWithIDEj(id);
        if (p == 0 || *(unsigned char *)(p + 0xd6) == 0) {
            unsigned short *ctr = (unsigned short *)(c + 0x334);
            *ctr = *ctr + 1;
        }
    }
    func_ov064_02119010(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        func_ov064_02118fa4(c);
    return 1;
}

/* func_ov064_0211982c IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov064_0211982c.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* PORT_HOST_ABI: mwcc pointer-to-member on the forward-declared struct C. The
   matched Piece::Behavior reads Obj.pmf (Obj at this+0x300, .pmf at +8 = record
   [1], the .b half) and, if non-null, calls it with `this`. Read as a plain
   { fn, 0 }. The three unk copies and the trailing helper are the matched
   Behavior's own. */
extern "C" int _ZN17BowserPuzzlePiece8BehaviorEv(void *self)
{
    char *c = (char *)self;
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        PortPmf *obj = *(PortPmf **)(c + 0x300);   /* ((C*)this)->obj */
        if (obj[1].fn)                             /* obj->pmf (record[1], .b) */
            ((int (*)(void *))(size_t)obj[1].fn)(c);
    }
    *(short *)(c + 0x08c) = *(short *)(c + 0x092);
    *(short *)(c + 0x08e) = *(short *)(c + 0x094);
    *(short *)(c + 0x090) = *(short *)(c + 0x096);
    func_ov064_0211987c(c);
    return 1;
}

/* ---- the two seats -------------------------------------------------------- */

static const struct { PortPmf *slot; unsigned rom; void (*host)(void *); }
g_manager_states[] = {
    {data_ov064_0211bf80, 0x02118da0, func_ov064_02118da0},  /* state 0 */
    {data_ov064_0211bf68, 0x02118d3c, func_ov064_02118d3c},  /* state 1 */
    {data_ov064_0211bf90, 0x02118d20, func_ov064_02118d20},  /* state 2 */
    {data_ov064_0211bf70, 0x02118d08, func_ov064_02118d08},  /* state 3 */
    {data_ov064_0211bf88, 0x02118cec, func_ov064_02118cec},  /* state 4 */
    {data_ov064_0211bf78, 0x02118cd4, func_ov064_02118cd4},  /* state 5 */
};

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
    {data_ov064_0211c2dc, 0x021193b4, func_ov064_021193b4},  /* .b, the per-frame */
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
