/* HOST COPIES of src/func_ov085_0212a430.cpp and src/func_ov085_0212a46c.cpp
 * -- PRINCESS_PEACH's (actor 186, ov085) two state dispatchers, with the mwcc
 * pointer-to-member call hand-rolled as a plain one.
 *
 * The gate-16 case for the sixth time (OneUpMushroom_Behavior.cpp,
 * SignPost_StateDispatch.cpp, LakituBro_Behavior.cpp, RabbitKey_StateSeat.cpp,
 * Ov085_Toad_StateSeat.cpp). Both matched sources spell
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[0x350]; PMF *pp; };
 *
 * and MSVC forms a pointer-to-member over an INCOMPLETE class as the four-word
 * general representation, so a PMF is sixteen bytes where the ROM's pair is
 * eight. For 0212a46c that reads twelve bytes past the pair it wants; for
 * 0212a430, whose whole job is `c->pp + 1`, the stride is wrong as well as the
 * width -- it lands sixteen bytes in where the ROM's second pair is at eight.
 *
 * THIS PAIR DIFFERS FROM TOAD'S IN TWO WAYS, both read off the ROM rather than
 * carried over: the object holds a POINTER to the current record at +0x350
 * (Toad holds an INDEX at +0x1fc and the dispatcher does the multiply), and
 * the record stride is 0x10 rather than 0x14. So these two take no index and
 * no base -- the seat function that moves between states is
 * func_ov085_0212a4a4, which is ordinary C and stays in the slice.
 *
 * The bodies below are the ROM's own call sequence
 * (extracted/overlays/overlay_0085.bin at base 0x02129020):
 *
 *   0x0212a46c, the ENTER half, 0x38 bytes
 *       ldr  r3, [r0, #0x350]      the current record
 *       ldr  r1, [r3, #4]          adj
 *       add  r0, r0, r1, asr #1    this += adj >> 1
 *       ands r1, r1, #1            adj & 1 ? virtual : plain
 *       ldrne r2, [r0]             vtable
 *       ldrne r1, [r3]             the offset within it
 *       ldrne r1, [r2, r1]
 *       ldreq r1, [r3]             or the address itself
 *       blx  r1
 *
 *   0x0212a430, the MAIN half, 0x3c bytes: the same, with one instruction
 *       more -- `add r3, r1, #8` -- so it dispatches the pair at record+8.
 *
 * ---- THE TABLE, and why the pairs have to be seated ------------------------
 *
 * data_ov085_0213055c is BSS: ov085's image ends at 0x02130480, so the record
 * table does not exist until __sinit_ov085_0212f3a0 (already in
 * port/slice_gate18.txt) copies ten eight-byte statics into it, two per
 * record. Those ten ARE mounted ROM bytes, so what arrives is ten DS code
 * addresses. Read out of the raw overlay, config/arm9/overlays/ov085/relocs.txt
 * agreeing on every one, every adj zero:
 *
 *   rec 0  +0x00 data_ov085_0212ff7c = { 0x0212a3ec, 0 }   enter, writes state 0
 *          +0x08 data_ov085_0212ff64 = { 0x0212a37c, 0 }   main
 *   rec 1  +0x10 data_ov085_0212ff44 = { 0x0212a328, 0 }   enter, writes state 1
 *          +0x18 data_ov085_0212ff6c = { 0x0212a220, 0 }   main
 *   rec 2  +0x20 data_ov085_0212ff4c = { 0x0212a1d4, 0 }   enter, writes state 2
 *          +0x28 data_ov085_0212ff74 = { 0x0212a19c, 0 }   main
 *   rec 3  +0x30 data_ov085_0212ff54 = { 0x0212a150, 0 }   enter, writes state 3
 *          +0x38 data_ov085_0212ff5c = { 0x0212a148, 0 }   main
 *   rec 4  +0x40 data_ov085_0212ff3c = { 0x0212a0e8, 0 }   enter, writes state 4
 *          +0x48 data_ov085_0212ff34 = { 0x0212a0b8, 0 }   main
 *
 * The order is the sinit's own literal pool (0x0212f5c4..0x0212f5e8, read
 * against its str offsets 0x00..0x4c), and each ENTER body writing its own
 * index to +0x354 confirms it a second way. port_princess_peach_states_seat
 * (hal/actor_overlays.cpp) verifies those exact words against the mount and
 * repoints them at the host bodies BEFORE the sinit copies them.
 *
 * THE PAIR TABLE IS ALSO WHY TOAD'S VTABLE IS 31 AND NOT 54. The gate-205
 * header adjudicated 0x0212ff34 as "a pointer-to-member SOURCE table, not more
 * vtable" from its shape alone. This lane names its owner: it is PRINCESS_
 * PEACH's, sitting between TOAD's table and her own SpawnInfo, and the sinit
 * that consumes it is hers.
 *
 * Callers, all matched and all cdecl: PrincessPeach::InitResources seats state
 * 0 on its last line but one (through func_ov085_0212a4a4), PrincessPeach::
 * Behavior calls the MAIN half every frame, func_ov085_0212a37c seats state 1
 * when a Player starts talking, and func_ov085_0212a220 seats state 0 again
 * when the message ends.
 */

// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" {

struct PeachStatePair { unsigned fn; int delta; };

static void peach_pmf_call(char *c, const PeachStatePair *pair)
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

/* the ENTER half: run the current record's pair at +0 */
void func_ov085_0212a46c(void *cv)
{
    char *c = (char *)cv;
    const PeachStatePair *e = *(const PeachStatePair **)(c + 0x350);
    peach_pmf_call(c, e);
}

/* the per-frame half: run the current record's pair at +8 */
void func_ov085_0212a430(void *cv)
{
    char *c = (char *)cv;
    const PeachStatePair *e =
        (const PeachStatePair *)(*(char **)(c + 0x350) + 8);
    peach_pmf_call(c, e);
}

}  /* extern "C" */
