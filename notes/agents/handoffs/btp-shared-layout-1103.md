# BTP shared layout handoff

Task: `issue-4570-btp-shared-layout-1103`  
Base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`  
Queue producer session: `codex-btp-1103-a`

## Change

Added `include/private/BTP_File.h` as the C-compatible shared definition used
by the `dExtAnmTexPat_c` public header and the two C consumers that had local
views. The header names the three key tables at `+0x10/+0x14/+0x18`, shares the
8-byte texture/palette record and 0x0c-byte sequence record, and asserts the
record and 0x24-byte file sizes for the ROM target.

Layout evidence comes from `func_02046bbc`, which indexes the three parallel
key tables and reads `numKeys`/`firstKey`, and
`dExtAnmTexPat_c::UpdateFileOffsets`, which rebases every table pointer and
each record's name pointer. `func_02046d50` resolves names and writes resource
indices. The `0xffff` resource sentinel remains represented as `u16`; this
slice does not claim signedness beyond the observed 16-bit storage.

## Verification so far

- `func_02046bbc` (`arm9`, `0x02046bbc`, `0x194`): focused match passed.
- `dExtAnmTexPat_c::UpdateFileOffsets` (`arm9`, `0x020468a0`, `0x10c`): focused match passed.
- `func_ov007_020bc2e4` (`ov007`, `0x020bc2e4`, `0x48`): focused match passed.
- Strict `linkcheck.py` returned `VERIFIED`, `diffs: []`, `blind: 0` for all three functions above.
- `python tools/rombuild.py -j16 --no-rom`: 11,395 functions and 468 source-owned data claims reproduce; 106/106 modules exact; zero mismatches. Strict baseline control passed.
- Declaration agreement has no new branch disagreements. Dead references, TU conflicts, and port references pass.
- The local `prepush_linkcheck.py --range origin/main..HEAD` expanded the headers to 168 files and was stopped after a prolonged sequential run. Every row reported before stopping was nonblocking (`VERIFIED` or baseline `BLIND-*`). The exact-head hosted PR validation passed, including the affected-header consumer build.
- `source_style.py` could not run because clang-format 19.1.7 is unavailable in this environment.

## Remaining review

Independently verify the exact output SHA through the queued `verify` stage.
The PR should remain draft and source acceptance should remain pending until
that review completes. Hosted PR validation and all other currently reported CI
checks pass on the exact producer head; do not infer merge or source-acceptance
status from those checks.
