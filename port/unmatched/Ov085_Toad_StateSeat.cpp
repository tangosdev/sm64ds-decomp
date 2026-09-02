/* HOST COPIES of src/func_ov085_02129524.cpp and src/func_ov085_02129570.cpp
 * -- the TOAD's seat-and-enter and its per-frame Main dispatch, with the mwcc
 * pointer-to-member call hand-rolled as a plain one.
 *
 * The gate-16 case for the fifth time (OneUpMushroom_Behavior.cpp,
 * SignPost_StateDispatch.cpp, LakituBro_Behavior.cpp,
 * RabbitKey_StateSeat.cpp). Both matched sources spell
 *
 *     struct C; typedef void (C::*PMF)();
 *
 * and MSVC forms a pointer-to-member over an INCOMPLETE class as the four-word
 * GENERAL representation, so a PMF is sixteen bytes where the ROM's pair is
 * eight. For 02129524 that reads its neighbour's bytes as part of its own
 * dispatch; for 02129570 it does not even compile, because that TU sizes its
 * record as
 *
 *     struct Entry { char pad[8]; PMF pmf; char tail[0x14 - 8 - sizeof(PMF)]; };
 *
 * and 0x14 - 8 - 16 is -4 in an expression whose type is size_t, so it wraps.
 * Reproduce it with `cl /c src/func_ov085_02129570.cpp`:
 *
 *     func_ov085_02129570.cpp(3): error C2148: total size of array must not
 *     exceed 0x7fffffff bytes
 *
 * which is the one honest thing about this shape: the file announces the
 * mismatch instead of shipping it. (Under mwcc sizeof(PMF) is 8 and tail is
 * the four bytes the ROM's 0x14 record really has left over.)
 *
 * The bodies below are the ROM's own call sequence
 * (extracted/overlays/overlay_0085.bin at base 0x02129020, 0x4c bytes each):
 *
 *     ldr  r2, [r0, #0x1fc]      the state index, on the object
 *     mla  r3, r2, #0x14, base   base = data_ov085_0212fe88, stride 0x14
 *     ldr  r1, [r3, #4]          adj
 *     add  r0, r0, r1, asr #1    this += adj >> 1
 *     ands r1, r1, #1            adj & 1 ? virtual : plain
 *     ...                        vtable lookup or the value itself
 *     blx  r1
 *
 * 02129524 dispatches the pair at entry+0 (the ENTER half) after writing the
 * index; 02129570 dispatches the pair at entry+8 (the MAIN half) and writes
 * nothing.
 *
 * ---- THE TABLE, and why the pairs have to be seated ------------------------
 *
 * data_ov085_0212fe88 is not static data: __sinit_ov085_0212f2a8 (already in
 * port/slice_gate18.txt) copies four eight-byte statics into it, two per
 * state, and those four statics ARE mounted ROM bytes -- so what arrives in
 * the table is four DS code addresses. Read out of the raw overlay, with
 * config/arm9/overlays/ov085/relocs.txt agreeing on every one:
 *
 *     data_ov085_0212fe40 = { 0x021294f0, 0 }   state 0 "WAIT" enter
 *     data_ov085_0212fe48 = { 0x02129470, 0 }   state 0 "WAIT" main
 *     data_ov085_0212fe30 = { 0x0212943c, 0 }   state 1 "TALK" enter
 *     data_ov085_0212fe38 = { 0x021291ac, 0 }   state 1 "TALK" main
 *
 * (the two names are ASCII in the overlay at 0x0212fe28 "WAIT" and
 * 0x0212fe20 "TALK", the +0x10 field of each record.) Every adj is 0, so
 * every one is the plain nonvirtual form. port_toad_states_seat
 * (hal/actor_overlays.cpp) verifies those exact words against the mount and
 * repoints them at the host bodies BEFORE the sinit copies them, the same seat
 * RabbitKey's four pairs take eleven lines above it. The adj arithmetic is
 * kept anyway: it is what the ROM's own sequence does, and a future non-zero
 * pair should take the documented path rather than a silent wrong one.
 *
 * Callers, all matched and all cdecl: Toad::InitResources seats state 0 on its
 * last line but one, func_ov085_02129470 (WAIT main) seats state 1 when a
 * Player starts talking, func_ov085_021291ac (TALK main) seats state 0 again
 * when the message ends, and Toad::Behavior calls the Main half every frame.
 */

// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" {

struct ToadStatePair { unsigned fn; int delta; };
extern ToadStatePair data_ov085_0212fe88[];   /* stride 0x14 = 5 words */

static void toad_pmf_call(char *c, const ToadStatePair *pair)
{
    int adj = pair->delta;
    char *self = c + (adj >> 1);
    void (*fn)(char *);

    if (adj & 1)
        fn = *(void (**)(char *))(*(char **)self + pair->fn);
    else
        fn = (void (*)(char *))(size_t)pair->fn;
    fn(self);
}

/* the seat-and-enter: write the index, then run that state's ENTER half */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
void func_ov085_02129524(void *cv, int i)
{
    char *c = (char *)cv;
    const ToadStatePair *e;

    *(int *)(c + 0x1fc) = i;
    e = (const ToadStatePair *)((char *)data_ov085_0212fe88
                                + *(int *)(c + 0x1fc) * 0x14);
    toad_pmf_call(c, e);
}

/* the per-frame half: run the current state's MAIN, at entry + 8 */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
void func_ov085_02129570(void *cv)
{
    char *c = (char *)cv;
    const ToadStatePair *e;

    e = (const ToadStatePair *)((char *)data_ov085_0212fe88
                                + *(int *)(c + 0x1fc) * 0x14 + 8);
    toad_pmf_call(c, e);
}

}  /* extern "C" */
