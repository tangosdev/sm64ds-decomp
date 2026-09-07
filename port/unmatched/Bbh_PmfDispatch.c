/* HOST COPIES of the three ov063 TUs that dispatch through mwcc
 * POINTER-TO-MEMBER tables -- the Painting_Dispatch.cpp case in its source
 * form: mwcc's PMF over an incomplete class is the ROM's 8-byte {fn, delta}
 * record, MSVC widens it to the 16-byte general form, so the compiled
 * &table[idx] strides wrong AND the call reads the neighbouring pair's words
 * as this-adjustments. The three matched sources stay untouched in src/ and
 * off the slice; each copy below spells the mwcc layout explicitly.
 *
 * The two tables are BSS the ov063 sinits fill by copying four 8-byte .data
 * pairs each -- so the port seats the .data pair fn words with host function
 * addresses BEFORE running the sinits (ov63_bringup in
 * hal/actor_classes_ov063.cpp), and the sinit's own matched copy then
 * propagates host pointers into the tables these dispatchers read. The
 * Painting recipe (hal/actor_overlays.cpp seats ov080's twelve statics the
 * same way). ROM pair values, read from overlay_0063.bin + relocs (delta 0
 * on all eight):
 *
 *   furniture table data_ov063_0211ef38[4] (sinit __sinit_ov063_0211e3cc):
 *     [0] <- data_ov063_0211e9b4 = {func_ov063_0211cc18, 0}  MansionSteps
 *     [1] <- data_ov063_0211e9bc = {func_ov063_0211cb54, 0}  TrapDoor
 *     [2] <- data_ov063_0211e9c4 = {func_ov063_0211c89c, 0}  Bookshelf
 *     [3] <- data_ov063_0211e9ac = {func_ov063_0211c7b0, 0}  MerryGoRound
 *   piano table data_ov063_0211efbc (2 Entries x 2 PMFs, sinit
 *   __sinit_ov063_0211e5fc):
 *     [0].pmf0 <- data_ov063_0211ecd8 = {func_ov063_0211dd78, 0}
 *     [0].pmf1 <- data_ov063_0211ece8 = {func_ov063_0211dbb8, 0}
 *     [1].pmf0 <- data_ov063_0211ece0 = {func_ov063_0211dba4, 0}
 *     [1].pmf1 <- data_ov063_0211ecf0 = {func_ov063_0211d8cc, 0}
 *   All eight bodies are matched TUs on slice_w5a.txt.
 */

extern unsigned char data_ov063_0211ef38[];
extern unsigned char data_ov071_02122ecc[];
extern unsigned char data_ov063_0211efbc[];
extern int func_ov063_0211c684(char *c);
extern int func_ov063_0211c6f8(char *c);

typedef void (*BbhPmfFn)(char *self);

/* one mwcc {fn, delta} dispatch, layout spelled out */
static void bbh_pmf_call(unsigned char *pair, char *self)
{
    BbhPmfFn fn = (BbhPmfFn) *(void **)pair;
    fn(self + *(int *)(pair + 4));
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (8-byte {fn,delta} vs
   MSVC's 16-byte form); the matched _ZN12MansionSteps8BehaviorEv.cpp strides
   data_ov063_0211ef38 wrong under MSVC. Layout hand-rolled here. */
int _ZN12MansionSteps8BehaviorEv(char *c)
{
    unsigned char before = *(unsigned char *)(c + 0x150);
    int idx = *(int *)(c + 0x140);
    bbh_pmf_call(data_ov063_0211ef38 + idx * 8, c);
    *(unsigned short *)(c + 0x14c) = (unsigned short)(*(unsigned short *)(c + 0x14c) + 1);
    if (before != *(unsigned char *)(c + 0x150))
        *(unsigned short *)(c + 0x14c) = 0;
    func_ov063_0211c684(c);
    func_ov063_0211c6f8(c);
    *(int *)(c + 0x124) = 0;
    return 1;
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch, same ruling -- the matched
   func_ov063_0211ddac.cpp reads Entry[idx].pmf[0] over the piano table. */
void func_ov063_0211ddac(char *c, int i)
{
    unsigned char *e;
    *(int *)(c + 0x6c8) = i;
    e = data_ov063_0211efbc + (*(int *)(c + 0x6c8)) * 16;
    bbh_pmf_call(e, c);
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch, same ruling -- the matched
   func_ov063_0211ddf4.cpp reads Entry[idx].pmf[1]. */
void func_ov063_0211ddf4(char *c)
{
    unsigned char *e = data_ov063_0211efbc + (*(int *)(c + 0x6c8)) * 16;
    bbh_pmf_call(e + 8, c);
}

/* ---- run linkw wave 5 addendum: CRAZED_CRATE's two dispatchers (ov080) ----
 *
 * The crate keeps a POINTER to its current state record at +0x36c
 * (func_ov080_0212513c, matched and on the slice, computes it as
 * data_ov080_0212847c + state*16 -- three states, each TWO mwcc {fn, delta}
 * pairs). These two helpers dispatch through that pointer: 0x02125104 calls
 * the record's FIRST pair (state enter), 0x021250c8 the SECOND (state tick).
 * Same MSVC 16-byte-PMF stride break as above, spelled out here; the six
 * pair values are re-seated with host bodies by hal_fill_crazed_crate_vtable
 * AFTER __sinit_ov080_02127a60 copies the ROM's DS-address pairs in (that
 * sinit already runs at boot from hal/actor_overlays.cpp, so the
 * seat-the-sources-first Painting shape cannot apply; overwriting the copied
 * DESTINATION is the same final state).
 */

/* BOTH crate dispatchers are back on the slice now and NEITHER is host-copied
   here any more. func_ov080_021250c8, the tick half at record +8, went back at
   wave 18. func_ov080_02125104, the enter half at record +0, goes back in run
   link100 lane PMF3 (port/slice_pmf3.txt) on the same evidence carried one
   step further: its object form is a TAIL JUMP under the port's own flags, and
   the six source pairs at ov080 0x0212812c..0x02128154 were re-read out of
   overlay_0080.bin with their relocations and every adjustment word is ROM
   zero. hal_fill_crazed_crate_vtable also WRITES each destination adjustment
   word to zero as it installs the host body, so the matched dispatcher's
   `this + delta` cannot be anything but `this + 0`.

   The wave 18 reading, kept because it is what both rows rest on: run linkw
   wave 18 compiles src/func_ov080_021250c8.cpp with /vmg /vmm, which gives
   MSVC the 8-byte
   {fn, delta} representation the ROM's record already is, so `c->pp + 1`
   strides eight onto the tick pair's own function word. All twelve of ov080's
   source statics at 0x02128214 carry a ROM-zero delta, so the matched dispatch
   (which adds delta to `this`) and the host body it replaces (which did not)
   agree word for word. See port/slice_w18a.txt and the R9 block in
   port/CMakeLists.txt.

   NOTE for whoever reads this file's header next: it says MSVC "widens it to
   the 16-byte general form". That is wrong, and the objs say so -- MSVC's
   representation for a pointer-to-member of an INCOMPLETE class is FOUR bytes
   here, which is why the symptom was a call to zero (the adj word of record 0)
   rather than a call to a wrong-but-nonzero address. The R9 block carries the
   before/after disassembly. The three dispatchers still hosted below are
   unaffected either way. */

/* ---- run linkw wave 5 addendum 2: the COFFIN's two dispatchers (ov071) ----
 *
 * The coffin's state table data_ov071_02122ecc is TWO 20-byte entries
 * ({pmf@0, pmf@8, extra-ptr@0x10}), filled by __sinit_ov071_02122a64
 * (already running at boot) from four 8-byte source pairs; the sinit's own
 * matched source spells the destination struct out, and the raw pairs read
 * {func_ov071_021223b0,0} {func_ov071_021221bc,0} for entry 0 and
 * {func_ov071_02122194,0} {func_ov071_021220c8,0} for entry 1 (deltas all
 * zero). hal_fill_coffin_vtable re-seats the four fn words with the host
 * bodies after the sinit ran, the CrazedCrate treatment. The dispatchers:
 * 0x021223c8 sets the state index at +0x320 and calls pmf@0; 0x02122414
 * calls pmf@8 of the current entry.
 */

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch over 20-byte entries; the
   matched func_ov071_021223c8.cpp mis-strides under MSVC's 16-byte PMF. */
void func_ov071_021223c8(char *c, int i)
{
    unsigned char *e;
    *(int *)(c + 0x320) = i;
    e = data_ov071_02122ecc + (*(int *)(c + 0x320)) * 20;
    bbh_pmf_call(e, c);
}

/* PORT_HOST_ABI: same ruling -- the matched func_ov071_02122414.cpp calls
   the entry's SECOND pair (+8). */
void func_ov071_02122414(char *c)
{
    unsigned char *e = data_ov071_02122ecc + (*(int *)(c + 0x320)) * 20;
    bbh_pmf_call(e + 8, c);
}
