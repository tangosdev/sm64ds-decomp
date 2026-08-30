/* HOST COPY of src/func_ov055_021112c4.cpp -- the ENTER half of MirrorLuigi's
 * one-cell pointer-to-member state machine (id 195, ov055, level 47, RTTI
 * 9daLuigi_c). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member formed on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef int (C::*PMF)(int);
 *     struct C { char pad[0x1d8]; PMF *pp; };
 *     extern "C" int func_ov055_021112c4(C *c, PMF *p, int a2)
 *     { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(a2); }
 *
 * mwcc's PMF there is the ROM's own 8-byte {function, delta} pair, which is why
 * the cell at data_ov055_02111b70 is sixteen bytes for two halves. MSVC's
 * representation for an incomplete class is the "unknown inheritance" form and
 * does not reproduce those semantics. Same shape as the Ukiki
 * (port/unmatched/Ukiki_StateEnter.cpp), BUBBA and the Wiggler; this is the
 * smallest instance of it in the tree -- ONE cell -- and the only one whose
 * dispatch carries an extra argument through.
 *
 * THE DISPATCH IS TRANSCRIBED, not simplified. ROM 0x021112c4:
 *     str  r1, [r0, #0x1d8]      c->pp = p
 *     ldr  ip, [r0, #0x1d8]
 *     ldr  r3, [ip]              the function word
 *     cmp  r3, #0 -> return 1
 *     ldr  r1, [ip, #4]          the delta word
 *     add  r0, r0, r1, asr #1    the receiver, this + (delta >> 1)
 *     ands r1, r1, #1            bit 0 selects a VIRTUAL call
 *     ldrne r1, [r0] / ldrne r3, [r1, r3]
 *     mov  r1, r2                the pass-through argument
 *     ldreq r3, [ip]
 *     blx  r3
 * Both arms are kept even though both ROM pairs carry delta 0 and a direct
 * address: the seat verifies that (hal/actor_classes_ov055.cpp aborts if a
 * cell half's delta is not zero), and a transcription that dropped the virtual
 * arm would make the abort the only thing between a changed mount and a wild
 * call.
 *
 * WHO CALLS IT: MirrorLuigi::InitResources, once, with the cell and the player
 * pointer. The enter half it reaches is func_ov055_021112bc, whose whole body
 * is `return 1`.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov055.txt. hal/actor_classes_ov055.cpp rewrites both halves
 * of the cell with HOST addresses (after verifying the ROM words the sinit
 * wrote) before any MirrorLuigi can reach this.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
extern "C" {

struct PortMirrorLuigiPmf { unsigned fn, delta; };
typedef int (*PortMirrorLuigiFn)(void *, void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class. */
int func_ov055_021112c4(void *selfv, void *cellv, void *a2)
{
    char *c = (char *)selfv;
    *(void **)(c + 0x1d8) = cellv;

    PortMirrorLuigiPmf *q = (PortMirrorLuigiPmf *)*(void **)(c + 0x1d8);
    if (q->fn == 0)
        return 1;

    char *recv = c + ((int)q->delta >> 1);
    PortMirrorLuigiFn fn;
    if (q->delta & 1)
        fn = (PortMirrorLuigiFn)(size_t)(*(unsigned **)recv)[q->fn / 4];
    else
        fn = (PortMirrorLuigiFn)(size_t)q->fn;
    return fn(recv, a2);
}
}
