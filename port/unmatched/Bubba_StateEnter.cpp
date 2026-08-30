/* HOST COPY of src/func_ov032_02111ff4.cpp -- the ENTER half of BUBBA's
 * five-cell pointer-to-member state machine (id 228, ov032, level 24).
 * Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member formed on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x3b0]; PMF *pp; };
 *     extern "C" int func_ov032_02111ff4(C *c, PMF *p)
 *     { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
 *
 * mwcc's PMF there is the ROM's own 8-byte {function, delta} pair, which is
 * also the stride of the five 16-byte cells __sinit_ov032_02112c10 builds
 * ({lo = enter, hi = tick}); MSVC's representation for an incomplete class is
 * the "unknown inheritance" form and does not reproduce those semantics. This
 * is the Ukiki eleven-cell shape (port/unmatched/Ukiki_StateEnter.cpp) and the
 * BabyPenguin six-cell shape with FIVE cells and the cell pointer at +0x3b0
 * instead of +0x3a4.
 *
 * THE DISPATCH IS TRANSCRIBED, not simplified. ROM 0x02111ff4:
 *     str  r1, [r0, #0x3b0]      c->pp = p
 *     ldr  r3, [r0, #0x3b0]
 *     ldr  r2, [r3]              the function word
 *     cmp  r2, #0 -> return 1
 *     ldr  r1, [r3, #4]          the delta word
 *     add  r0, r0, r1, asr #1    the receiver, this + (delta >> 1)
 *     ands r1, r1, #1            bit 0 selects a VIRTUAL call
 *     ldrne r1, [r0]             ... through the receiver's vptr
 *     ldrne r1, [r1, r2]         ... at byte offset `function`
 *     ldreq r1, [r3]             otherwise `function` IS the address
 *     blx  r1
 * Both arms are kept even though all ten of BUBBA's ROM pairs carry delta 0 and
 * a direct address: the seat verifies that (hal/actor_classes_ov032.cpp aborts
 * if a cell's delta is not zero), and a transcription that silently dropped the
 * virtual arm would make the abort the only thing standing between a changed
 * mount and a wild call.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov032.txt. hal/actor_classes_ov032.cpp rewrites both
 * halves of all five cells with HOST addresses (after verifying the ROM words)
 * before any actor can reach this, so `fn` below is a host function pointer.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
extern "C" {

struct PortBubbaPmf { unsigned fn, delta; };
typedef int (*PortBubbaFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class. */
int func_ov032_02111ff4(void *selfv, void *cellv)
{
    char *c = (char *)selfv;
    *(void **)(c + 0x3b0) = cellv;

    PortBubbaPmf *q = (PortBubbaPmf *)*(void **)(c + 0x3b0);
    if (q->fn == 0)
        return 1;

    char *recv = c + ((int)q->delta >> 1);
    PortBubbaFn fn;
    if (q->delta & 1)
        fn = (PortBubbaFn)(size_t)(*(unsigned **)recv)[q->fn / 4];
    else
        fn = (PortBubbaFn)(size_t)q->fn;
    return fn(recv);
}
}
