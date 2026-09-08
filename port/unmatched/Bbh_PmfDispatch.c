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
extern int func_ov063_0211c684(char *c);
extern int func_ov063_0211c6f8(char *c);

typedef void (*BbhPmfFn)(char *self);


/* HOST COPY RETIRED, run link100 lane FWD gate 2.
   src/actors/MansionSteps/_ZN12MansionSteps8BehaviorEv.cpp dispatches
   data_ov063_0211ef38 now. The banner's reading -- "MSVC's 16-byte form" --
   expired when block R8's /vmg /vmm landed: MSVC's pointer to member on this
   target IS the ROM's eight-byte {fn, delta} pair, and the matched TU emits
   [eax*8] against the ROM's own `add r3,r1,r0,lsl #3`. What was actually left
   was the definition form (the TU defines the C++ MEMBER
   ?Behavior@MansionSteps@@QAEHXZ where hal/actor_classes_ov063.cpp's
   ms_behavior face calls the flat C name), and port/hal/fwd_forwarders.cpp
   bridges it. bbh_pmf_call went with this body: it had no other caller. */

/* func_ov063_0211ddac and func_ov063_0211ddf4 RETIRED (run link100, lane
   PMFB1). src/unnamed/ov063/func_ov063_0211ddac.cpp and
   src/unnamed/ov063/func_ov063_0211ddf4.cpp carry both on
   port/slice_pmfc.txt. The piano table is the one record in this file that
   MSVC already sized right without help: its Entry is two PMFs, sixteen
   bytes, and the ROM strides it with `add r3, r2, r1, lsl #4` -- sixteen. The
   eleven twenty-byte rows in the same slice needed a per-TU /Zp4 (block R9d
   in port/CMakeLists.txt), because MSVC gives a struct containing a
   pointer-to-member eight-byte alignment and rounds twenty up to twenty-four.
   ov63_bringup in hal/actor_classes_ov063.cpp still writes the host addresses
   into the four SOURCE pairs before the sinit copies them, unchanged.
   bbh_pmf_call below STAYS: MansionSteps::Behavior dispatches the furniture
   table through it. */

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

/* func_ov071_021223c8 and func_ov071_02122414 RETIRED (run link100, lane
   PMFB1). src/func_ov071_021223c8.cpp and src/func_ov071_02122414.cpp carry
   both on port/slice_pmfc.txt. The header above blamed "MSVC's 16-byte PMF"
   and that reading is dead: with /vmg /vmm target-wide (block R8) the
   pointer-to-member IS the ROM's eight-byte {function, delta} pair. What was
   still wrong was the RECORD, not the pointer -- MSVC gives a struct that
   contains a pointer-to-member eight-byte alignment, so the coffin's twenty-
   byte entry came out twenty-four and the matched TUs strode 24 where the ROM
   strides 0x14. A per-TU /Zp4 (block R9d in port/CMakeLists.txt) makes it
   twenty. hal_fill_coffin_vtable in hal/actor_classes_scuttlebug.cpp still
   re-seats the four function words after the sinit, unchanged. */
