//cpp
// NONMATCHING: register allocation and one spill slot (div=58). Logic verified
// correct vs ROM; not byte-matchable from C at mwccarm 1.2/base, sp2 or sp2p3.
// Counts as decompiled, not matched.
//
// THE RESIDUE IS MEASURED RATHER THAN ASSERTED, using run mg7 section 15's
// instrument: the candidate compiles to 165 instructions against the ROM's
// 165, and the MNEMONIC MULTISET IS IDENTICAL -- nothing inserted, nothing
// deleted. Of the 58 differing words, 17 are an immediate or an offset only
// (the frame is 0x14 in the ROM and 0xc here, which shifts every incoming
// stack-argument offset by 8), 19 are a register number only, and 22 are a
// schedule move. The ROM caches y1 in a local at [sp,#0xc] and re-homes val
// into its own incoming slot at [sp,#0x3c]; both are spill decisions no source
// shape reached. What was tried: the abs as a ternary and as an if (the if is
// shipped and took the count from 999 to 58), dx-first and dy-first
// declaration order, endpoint locals in six placements, self-copy locals for
// val and n, and the loop tests against parameters and against locals. 58 is
// the floor for every variant that keeps the size, and every variant that
// changed the size changed it by a whole block.
//
// dScMgBase_c's LINE RASTERISER: a Bresenham walk from (x0,y0) to (x1,y1) that
// stamps vtable slot 34 at every lattice point it visits.
//
// Slot 34 is the BRUSH -- the base's own body is src/func_ov004_020ae3b4.c,
// which writes an n-by-n block of `val` through MultiCopy_Int -- so this
// function is "draw a line of thickness n in colour val", and its callers are
// the amidakuji board. src/func_ov006_020d3ba0.c draws the four VERTICAL RAILS
// with it (x = 0x20, 0x60, 0xa0, 0xe0, each from y = -0xb4 down to the layout
// value at +0x4700) and the horizontal RUNGS as (x, y) -> (x + 0x40, y), which
// is the ladder the player of "Mario's Slides" adds routes to. That is the
// mechanism behind both titles' rules text, "draw lines on the Touch Screen".
//
// THE SEVEN PARAMETERS ARE READ OFF THE FRAME, not chosen. The prologue pushes
// nine registers (0x24) and subtracts 0x14, so the first incoming stack word
// sits at [sp,#0x38]: four arguments in r0..r3 and three above it, and the
// three call sites in this class (src/func_ov006_020d1ba0.c, _020d2580.c,
// _020d3ba0.c) all spell the same seven.
//
// THE DISPATCH IS SLOT 34 AND THAT IS ARITHMETIC: every call site here is
// `ldr Rd,[r0]; ldr Rd,[Rd,#0x88]; blx Rd` and 0x88 = 34*4. Five arguments
// reach it -- r0..r3 plus one word stored at [sp] before the blx -- which is
// exactly what src/func_ov004_020ae3b4.c declares. This is the tree's ONLY
// body that dispatches slot 34.
//
// THE STRUCT BELOW IS THE DISPATCH SPELLING, not a claim about the class.
// src/func_ov006_020d3ba0.c uses the same device for slot 36. Only the slot-34
// signature is spelled out; the rest are placeholders to put it at index 34.

struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33();
    virtual void v34(int cx, int cy, int val, int n);
};

#define PLOT(o, x, y, v, n)  (((struct Obj *)(o))->v34((x), (y), (v), (n)))

extern "C" void func_ov004_020ae5c4(void *obj, int x0, int y0, int x1, int y1,
                                    int val, int n)
{
    /* DECLARATION ORDER IS LOAD-BEARING HERE, which is the pret idiom this
       repo records: dy is declared before dx because the ROM colours dy into
       fp and dx into r6, and declaring dx first hands fp to dx instead. */
    int dy;
    int dx;
    int adx;
    int ady;
    int err;

    /* The degenerate line. The ROM tests before it plots and plots in two
       different blocks, so the test is written first here too. */
    if (x0 == x1 && y0 == y1) {
        PLOT(obj, x0, y0, val, n);
        return;
    }

    PLOT(obj, x0, y0, val, n);

    dy = y1 - y0;
    dx = x1 - x0;
    /* THE ABSOLUTE VALUES ARE IF-STATEMENTS AND NOT TERNARIES, which is run
       mg7 section 15's lever read from the other end: a ternary compiles to a
       conditional PAIR (rsbmi/movpl), where the ROM emits an unconditional mov
       followed by one conditional rsb -- and that is an if. */
    adx = dx;
    if (dx < 0) adx = -dx;
    ady = dy;
    if (dy < 0) ady = -dy;

    /* EACH STEP ARM CARRIES ITS OWN PLOT AND ITS OWN LOOP TEST, which is the
       ROM's shape rather than a stylistic choice: there are EIGHT `blx` sites
       in this body and only eight, one per block below, so a source that
       shared one plot between the two arms would come out 0x68 short. */
    if (adx >= ady) {
        /* X-MAJOR: step x every iteration, y on error overflow. */
        err = adx / 2;
        for (;;) {
            if (dx == 0) {
                PLOT(obj, x0, y0, val, n);
                return;
            }
            if (dx > 0) {
                err += ady;
                x0++;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                PLOT(obj, x0, y0, val, n);
                if (x0 == x1) return;
            } else {
                err += ady;
                x0--;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                PLOT(obj, x0, y0, val, n);
                if (x0 == x1) return;
            }
        }
    } else {
        /* Y-MAJOR: step y every iteration, x on error overflow. */
        err = ady / 2;
        for (;;) {
            if (dy == 0) {
                PLOT(obj, x0, y0, val, n);
                return;
            }
            if (dy > 0) {
                err += adx;
                y0++;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                PLOT(obj, x0, y0, val, n);
                if (y0 == y1) return;
            } else {
                err += adx;
                y0--;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                PLOT(obj, x0, y0, val, n);
                if (y0 == y1) return;
            }
        }
    }
}
