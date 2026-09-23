/* RE-READ, run linkfull wave 23 lane BANNER3, LINK15 batch W23-3: NOT
 * RETIRED. KEPT AS HOST, blocked outside this lane's owned files.
 *
 * The batch's premise held for the body but not for the symbol: fdiv's own
 * callees (fdiv_async, fdiv_result) ARE linked matched TUs on walk_window,
 * walk_window_hires and smoke_player, and the divider is modelled at
 * port/ntr/io.cpp's run_divide(). But src/_ZN4cstd4fdivEii.cpp is not a flat
 * function to seat beside them -- it was already language-mode-flipped to
 * real C++ (notes/plan-cpp-language-mode.md phase 1) and defines
 * `namespace cstd { s32 fdiv(s32, s32) {...} }` directly, mangling to
 * `?fdiv@cstd@@YAHHH@Z`. hal/reverse_bridges.cpp:224 already defines that
 * exact symbol -- `int cstd::fdiv(int,int) { return _ZN4cstd4fdivEii(a,b); }`,
 * a bridge written for when this row was still flat-named -- so seating the
 * real TU on any of the three targets is LNK2005, multiply defined, against
 * reverse_bridges.cpp.obj (measured: two build attempts, not assumed).
 * Clearing it means retiring reverse_bridges.cpp's now-redundant bridge
 * (its declaration at line ~43, its definition at line ~223-224), and that
 * file is not one this lane's brief owns; recorded here as `blocked:` rather
 * than edited. This host body is UNCHANGED and still serves all nine
 * targets that reference it.
 *
 * THE b == 0 CASE, checked anyway since the re-read asked for it, for
 * whoever picks the block up: this file returns `a < 0 ? -1 : 1`.
 * port/ntr/io.cpp's run_divide() returns `n < 0 ? 1 : -1` (GBATEK: division
 * by zero yields remainder = numerator and a sign-based quotient; see
 * io.cpp's own comment there) -- the OPPOSITE sign. They disagree; per this
 * batch's own ruling the ROM (run_divide) would be right, so this is a
 * real, narrow, pre-existing behaviour gap on the zero-divisor case, left
 * as found and unfixed pending the reverse_bridges.cpp retirement above.
 *
 * On the DS, cstd::fdiv feeds the hardware divider (fdiv_async writes the
 * DIV registers, fdiv_result spins on DIV_BUSY). The operation itself is
 * just a 20.12 divide: (a / b) in Fix12 is (a << 12) / b.
 */
typedef int s32;

/* PORT_HOST_ABI: src is a thin wrapper over the DS hardware divider's
 * MMIO (the DIV registers, spinning on DIV_BUSY). See the header. */
s32 _ZN4cstd4fdivEii(s32 a, s32 b)
{
    if (b == 0)
        return a < 0 ? -1 : 1;
    return (s32)(((long long)a << 12) / b);
}

/* ---- DS INTEGER DIVISION SEMANTICS ----------------------------------------
 *
 * These are NOT cstd::div. They are what mwccarm's `/` and `%` OPERATORS
 * compile to: a call to __aeabi_idiv / _s32_div_f, the CodeWarrior helper the
 * ROM keeps in ITCM at 0x01ffabe4, which returns the quotient in r0 and the
 * remainder in r1.
 *
 * The zero-divisor path is the whole reason this exists, and it is read out
 * of the ROM rather than assumed. 0x01ffabe4 saves the operand signs, then:
 *
 *     0x01ffabf8  cmp   r1, #0
 *     0x01ffac00  beq   0x01ffaddc
 *     ...
 *     0x01ffaddc  ands  r3, r12, #0x80000000
 *     0x01ffade0  rsbne r0, r0, #0        ; restore the numerator's sign
 *     0x01ffade4  ands  r3, r12, #1
 *     0x01ffade8  rsbne r1, r1, #0        ; r1 is still 0
 *     0x01ffadec  bx    lr
 *
 * so on a zero divisor the ROM returns QUOTIENT = NUMERATOR and REMAINDER = 0.
 * It does not fault, and x86's idiv does -- which is a real behavioural
 * difference, not a corner case: 181 of the 321 particle definitions ship a
 * ZERO emission interval, and func_0204a730 gates emission on
 * `counter % interval == 0`. On the ROM that is always true, so a zero
 * interval means "emit every frame". Hosted with a raw x86 idiv it is an
 * immediate crash the first time the Player kicks up dust.
 *
 * hostgen.py routes the affected expressions here (DS_DIV in its patch
 * table); see the note there for why the sites are named one by one.
 */
int ds_idiv(int a, int b) { return b ? a / b : a; }
int ds_imod(int a, int b) { return b ? a % b : 0; }
unsigned ds_udiv(unsigned a, unsigned b) { return b ? a / b : a; }
unsigned ds_umod(unsigned a, unsigned b) { return b ? a % b : 0u; }

/* ---- DS SHIFT SEMANTICS ---------------------------------------------------
 *
 * The same shape as the division helpers above and for the same reason: a
 * shift whose COUNT COMES FROM A REGISTER means different things on the two
 * machines, and the difference is not a corner case.
 *
 * On the ARM a register-specified shift reads the bottom byte of the count
 * register and saturates: LSR by 32 or more yields ZERO. mwccarm compiles a
 * variable `x >> n` straight into `mov rD, rX, lsr rN`, so that is what the
 * cartridge does.
 *
 * On x86 the shift count is masked to five bits, so a count of 32 becomes a
 * count of 0 and `x >> 32` returns X UNCHANGED. In C the expression is
 * undefined at or above the operand width, so the compiler owes nothing
 * either way.
 *
 * src/func_0201b100.c -- the composer that lays the level-clear save menu's
 * button captions into the sub engine's BG0 tiles -- reaches exactly that
 * count. It takes the pen's sub-tile offset `sh = acc & 7`, forms
 * `rshift = (8 - sh) << 2`, and uses it to pull the part of a glyph cell that
 * spills into the next tile column. Its fast path is taken only when
 * (acc & 0xf) == 0, so a pen position that is a multiple of 8 but not of 16
 * arrives here with sh == 0 and rshift == 32. Hosted raw, the spill statement
 * then wrote the glyph's LEFT tile unshifted into the column to its right:
 * every caption glyph that happened to land on an odd tile boundary came out
 * with a ghost of its own left half eight pixels further along, which is the
 * stray mark after the "&" and the merged "IT" of SAVE & QUIT.
 *
 * Message::AddChar spells the same idiom for the other composer and guards it
 * with `if (sh != 0)`, which is why the top screen's text was never affected.
 *
 * hostgen.py routes the two spill expressions here (DS_SHIFT in its patch
 * table); src/ itself is untouched.
 */
unsigned ds_lsr(unsigned x, unsigned n) { return n >= 32u ? 0u : (x >> n); }
