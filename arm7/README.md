# ARM7

The ARM7 binary (`arm7.bin`, 0x24B24 bytes, SHA-1 in `arm7.sha1`) built from source.
There was no ARM7 precedent in this tree, so it lives in its own directory and follows
the arm9 rules where they apply: one function per file named by storage address, the
`// NONMATCHING` banners from `notes/asm-policy.md`, and retail bytes for anything that
is not built from source (as the ROM build does with dsd gap bytes).

    python arm7/build.py [--base extracted/dsd/arm7/arm7.bin]

Needs `tools/mwccarm` (or `ARM7_MW`) and the dsd extract. The retail image is only read;
nothing extracted is written into the tree. Output goes to `build/arm7/`.

## Layout

- `src/<storage>.c` - one function each. `<storage>` is the address in the ARM7 binary as
  loaded (0x02380000 + file offset); the comment header gives the runtime address.
  The `wram` part (storage 0x02380168..0x0238E088) runs at 0x037F8000, the `main` part
  (0x0238E088..0x023A4B0C) at 0x027C0000.
- `src/02380000.s` (crt0) and `src/02388130.s` (Thumb SWI veneers) - mwasmarm sources.
- `swap.txt` - the enrolled functions: storage, size, symbol, source, region. Only these
  are compiled and linked at their address. A file in `src/` with no row is not enrolled.
- `syms.txt` - absolute runtime addresses for symbols that source refers to but that are
  not defined by an enrolled object.

## Kinds of file

- Real C, no asm of any kind: enrolled, counts as matched.
- `// NONMATCHING (ASM-PRIMITIVE)` + `// HAND-ASM PRIMITIVE`: the original was assembly
  (MRS/MSR, SWP, SWI, divide runtime, BX veneers, crt0). Enrolled, counts as matched.
- `// NONMATCHING (ASM-STANDIN)`: a function that was C but has no matching C yet, kept as
  byte-exact whole-function asm so the full binary builds from source. Enrolled here,
  NOT counted as matched (upstream tools read it as a plain `NONMATCHING` draft).
- `// NONMATCHING` C: the closest C attempt, not enrolled (the build keeps retail bytes),
  banked in `nearmiss/db.jsonl`.
