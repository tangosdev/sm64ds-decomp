/* cstd::fdiv (run linkfull wave 27, lane SMALLS1; BANNER3's block cleared).
 *
 * THE GAME TARGETS RUN THE ROM'S OWN fdiv NOW. walk_window,
 * walk_window_hires and smoke_player link the matched
 * src/_ZN4cstd4fdivEii.cpp (port/slice_w27_smalls1.txt): fdiv_async starts
 * the divider in 64/32 mode on (a << 32) / b and fdiv_result reads the
 * quotient back, both as matched TUs over port/ntr/io.cpp's run_divide().
 * hal/reverse_bridges.cpp's old bridge (C++ name -> this flat body) is gone;
 * an alias there sends the flat name to the matched body instead, and the
 * CMake block "W27 SMALLS1" defines SM64DS_PORT_FDIV_SEATED for this file on
 * exactly those three targets, so the body below is not compiled there.
 *
 * THE BODY BELOW is what the six narrow harnesses that link no divider model
 * still call, and it now computes what the cartridge computes:
 *
 *   q = (a << 32) / b            the unit's 64/32 quotient (GBATEK DIVCNT mode 1)
 *   return (q + 0x80000) >> 20   fdiv_result: round the quotient to 20.12
 *
 * Two things were wrong before. (1) THE ZERO DIVISOR: this returned
 * a < 0 ? -1 : 1. GBATEK's division-by-zero rule gives the quotient +1 or
 * -1 with the sign OPPOSITE the numerator (run_divide's `n < 0 ? 1 : -1`,
 * so BANNER3 was right that the sign was backwards), but fdiv does not
 * return the quotient: fdiv_result rounds it to 20.12, and (+-1 + 0x80000)
 * >> 20 is 0. So the cartridge's cstd::fdiv(a, 0) is 0 for every a, and so
 * is this body now. (2) ROUNDING: this truncated (a << 12) / b, where the
 * cartridge rounds to nearest (the + 0x80000), so e.g. fdiv(2, 3) was 2730
 * here and is 2731 on the DS. The probe that measured both, old body vs new
 * body vs the ROM's own arithmetic over the GBATEK divider, is lane
 * SMALLS1's fdiv_probe (run linkfull out/SMALLS1).
 */
typedef int s32;

#ifndef SM64DS_PORT_FDIV_SEATED
/* PORT_HOST_ABI: the narrow harnesses link no divider model (ntr/io.cpp's run_divide) and no fdiv_async / fdiv_result; this is the cartridge's arithmetic without the MMIO. See the header. */
s32 _ZN4cstd4fdivEii(s32 a, s32 b)
{
    const long long n = (long long)a * 4294967296LL;   /* (a << 32), 64-bit */
    long long q;
    if (b == 0)
        q = n < 0 ? 1 : -1;               /* GBATEK: +-1, sign opposite the numerator */
    else if (b == -1 && a == (s32)0x80000000)
        q = n;                            /* GBATEK: -2^63 / -1 overflows to -2^63 */
    else
        q = n / b;
    return (s32)((q + 0x80000) >> 20);    /* fdiv_result */
}
/* The C++ spelling, for the narrow harnesses only (run linkfull, lane RS5B):
   the matched sphere pass (src/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp)
   calls cstd::fdiv as a namespace function, ?fdiv@cstd@@YAHHH@Z. On the
   seated targets src/_ZN4cstd4fdivEii.cpp defines that name and this block
   is compiled out, so the alias never stands beside the ROM body. */
#pragma comment(linker, "/alternatename:?fdiv@cstd@@YAHHH@Z=__ZN4cstd4fdivEii")
#endif

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
