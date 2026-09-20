# Credits

This decompilation builds on reverse-engineering knowledge from the SM64DS community.

## Symbol names & struct layouts
- **SplattyDS / DynamicAllocationDecomp** (https://github.com/SplattyDS/DynamicAllocationDecomp)
 , source of verified function/data symbol names (`symbols.x`) and struct/actor-system
  header knowledge for the EU build. We import their **names and field offsets** (facts /
  interface) into our symbol database and notes; we do **not** copy their source code -
  all matching C in `src/` is written from scratch against our own ROM. Their addresses
  were independently verified against our `dsd` analysis (≈72% land exactly on functions
  we found, confirming the same EU build).
- **SplattyDS / SM64DS-ASM-Reference**, **Arisotura / SM64DSe**, and **Gota7 /
  MoreObjectsMod**, further upstream symbol / reference documentation the above builds on.

If you contribute, keep this separation: import knowledge, write code.

## Matched-function contributions
- **Ryan Copley** (https://github.com/RyanCopley) - hand-wrote and verified matching C for a
  set of functions across ov002, ov006, arm9, and ov034 (PR #1). 22 of these were ones not
  yet matched here, including the first two functions in ov034 and several that had resisted
  our automated passes; all were independently re-verified byte-for-byte before landing.
- **Andrew Boudreau** (https://github.com/andrewboudreau) - hand-wrote and verified matching C
  for 9 functions across arm9, ov002, ov006, and ov007 (PR #2). These are reloc tail-call,
  guard-return, and call-forwarding shapes the automated passes deliberately skip; all were
  independently re-verified byte-for-byte, identical across the 1.2 base/sp2/sp2p3 trio, before
  landing. Has since hand-matched a large further set across arm9 and many overlays
  (PRs #45 and #48, including a 41 function batch across ov002/ov004/ov006/ov007/ov015/ov060/ov102),
  all independently re-verified byte-for-byte before landing, and contributed codegen notes on
  boolean materialization and predicated-select shapes (PR #49).
- **Moundistz** (https://github.com/Moundistz) - contributed 3 matched functions and 8
  nonmatching floor entries, plus a pass of placeholder-to-resolved callee renames across the
  arm9 corpus. Every candidate was independently re-verified against the ROM before landing;
  duplicate re-submissions of already-matched functions and a handful of match-to-nonmatching
  regressions were dropped.
- **lplaat** (https://github.com/lplaat) - hand-matched a batch of arm9 flag setters,
  thunks and small helpers, together with a set of ov002 cleanups (PR #86).
- **lunavyqo** (https://github.com/lunavyqo) - hand-matched functions across ov001 and a
  wide spread of the scene overlays from PR #89 on, banked near-miss tips for the ones
  that would not fall (Wiggler, Dorrie, the TTC moving cube), and has since carried the
  class translation-unit cleanup work: promoting a folded actor TU and then clearing the
  one-function leftovers it strands.
- **ruspecial** (https://github.com/ruspecial) - hand-matched large batches across ov002,
  ov006 and the arm9 BIOS SVC stubs from PR #90 on, banked 174 near-misses with the C++
  exception-handling and split-symbol triage notes that came out of them (PR #806), and
  contributed oracle-verified host copies for the two Bowser ov060 holes on the port side
  (PR #1505).
- **natbree** (https://github.com/natbree) - hand-matched functions in arm9 and ov075,
  including a batch of fifteen (PRs #347, #349, #504).
- **ai-tdd-labs** (https://github.com/ai-tdd-labs) - hand-matched functions across arm9
  and many overlays in a long run of small byte-identical batches (PRs #361 to #471),
  verified relocation targets on several more, and taught the worklist generator to skip
  sources already tracked in the tree (PR #428).
- **mitchellcairns** (https://github.com/mitchellcairns) - hand-matched functions in arm9,
  ov002 and ov006, including the cut crash screen (PR #466), and improved three arm9
  near-miss drafts from Ghidra full bodies (PR #604).
- **NitroShellMKDS** (https://github.com/NitroShellMKDS) - hand-matched functions in ov006
  (PRs #840, #937).
- **mitch030504** (https://github.com/mitch030504) - hand-matched functions across arm9
  and the overlays in five batches (PRs #1054 to #1081).

## Tooling contributions
- **webheadvr** (https://github.com/webheadvr) - made the relocation symbol resolver
  module-aware: it now resolves by `(module, address)` instead of address alone, so a
  reloc is no longer mis-resolved to a symbol from a different overlay where overlay
  address ranges overlap. Also added ITCM/DTCM symbol support.
- **Andrew Boudreau** (https://github.com/andrewboudreau) - added a relocation destination audit
  (`tools/reloc_audit.py`) and an opt-in `match.py --strict-relocs` check that verifies each
  reloc slot points at the address `config/**/relocs.txt` records, closing a gap where the
  byte compare wildcards relocated words without checking their destination (PR #47).
- **mitchellcairns** (https://github.com/mitchellcairns) - added the C++ decompilation
  index, the safe renaming tools and the ABI naming guide (PR #861); measured the C/C++
  language-mode gap and added the CI ratchet that stops it growing (PRs #1140, #1141);
  migrated the GX, GXS and G2S namespaces out of C mode (PRs #1142, #1144); and corrected
  the setup documentation, which could not actually get you the pinned 2004/b56 compiler
  (PR #1139).
- **liveteklol** (https://github.com/liveteklol) - made `tools/recover_cw2004.py` run on
  Linux by removing its `fsutil` and backslash-basename Windows-isms (PR #1112), and
  mapped the `func_ov080_02125460` wall across repeated full-angle sweeps, including the
  `gccext` statement-expression attractor recorded in `notes/mwccarm-codegen.md` 6bb
  (PRs #1025, #1111, #1133).

## Documentation contributions
- **Alberto12345678999** (https://github.com/Alberto12345678999) - fixed markdown syntax
  and code-fence languages across the notes tree (PRs #1578, #1658, #1664, #2060, #2732),
  and ran the phase 1 to 8 passes that link the overlay and data symbols named in the
  provenance and reference notes to the records they stand for (PRs #2123, #2166, #2180,
  #2305, #2381, #2643, #2675, #2724).
