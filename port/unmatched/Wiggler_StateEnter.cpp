/* HOST COPY of src/func_ov034_021125b8.cpp -- the ENTER half of the Wiggler's
 * eleven-cell pointer-to-member state machine (id 248, ov034, level 26, RTTI
 * 12daHanachan_c). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member formed on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Entry { PMF pmf; char pad[12]; };
 *     extern Entry data_ov034_02114538[];
 *     struct C { char pad[0x8c4]; int idx; };
 *     extern "C" void func_ov034_021125b8(C *c, int i)
 *     { c->idx = i; int j = c->idx; (c->*data_ov034_02114538[j].pmf)(); }
 *
 * mwcc's PMF there is the ROM's own 8-byte {function, delta} pair, which is why
 * the entry stride is 20: {enter pmf 8, tick pmf 8, name pointer 4}. MSVC's
 * representation for an incomplete class is the "unknown inheritance" form and
 * does not reproduce those semantics. Same class of body as the Ukiki
 * (port/unmatched/Ukiki_StateEnter.cpp) and BUBBA
 * (port/unmatched/Bubba_StateEnter.cpp); here the state is an INDEX at +0x8c4
 * rather than a cell pointer, and the table is indexed rather than walked.
 *
 * THE DISPATCH IS TRANSCRIBED, not simplified. ROM 0x021125b8:
 *     str  r1, [r0, #0x8c4]      c->idx = i
 *     ldr  r2, [r0, #0x8c4]
 *     ldr  r3, =0x02114538       the table
 *     mov  r1, #0x14
 *     mla  r3, r2, r1, r3        &table[idx]
 *     ldr  r1, [r3, #4]          the delta word
 *     add  r0, r0, r1, asr #1    the receiver, this + (delta >> 1)
 *     ands r1, r1, #1            bit 0 selects a VIRTUAL call
 *     ldrne r2, [r0] / ldrne r1, [r3] / ldrne r1, [r2, r1]
 *     ldreq r1, [r3]
 *     blx  r1
 * Both arms are kept even though all eleven ROM pairs carry delta 0 and a
 * direct address: the seat verifies that (hal/actor_classes_ov034.cpp aborts if
 * a cell's delta is not zero), and a transcription that dropped the virtual arm
 * would make the abort the only thing between a changed mount and a wild call.
 * NOTE the ROM does NOT null-check the function word here, and neither does
 * this copy: state 0 (DEMOWAIT) is entered by InitResources before anything can
 * dispatch, and __sinit_ov034_021138ec fills all twenty-two halves.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov034.txt. hal/actor_classes_ov034.cpp rewrites both halves
 * of all eleven entries with HOST addresses (after verifying the ROM words
 * the sinit wrote) before any Wiggler can reach this.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
extern "C" {

struct PortWigglerEntry { unsigned enter_fn, enter_delta, tick_fn, tick_delta, name; };
typedef void (*PortWigglerFn)(void *);
extern PortWigglerEntry data_ov034_02114538[];

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class. */
void func_ov034_021125b8(void *selfv, int i)
{
    char *c = (char *)selfv;
    *(int *)(c + 0x8c4) = i;

    PortWigglerEntry *e = &data_ov034_02114538[*(int *)(c + 0x8c4)];
    char *recv = c + ((int)e->enter_delta >> 1);
    PortWigglerFn fn;
    if (e->enter_delta & 1)
        fn = (PortWigglerFn)(size_t)(*(unsigned **)recv)[e->enter_fn / 4];
    else
        fn = (PortWigglerFn)(size_t)e->enter_fn;
    fn(recv);
}
}
