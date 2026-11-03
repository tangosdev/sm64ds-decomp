# BTA shared prefix handoff

Task: `issue-4570-bta-shared-prefix-1103`  
Base: `8bcb5a981bde3dbe4375aab971226ccee72ee8e6`  
Queue producer session: `codex-bta-1103-a`

## Change

Added `include/private/BTA_File.h` for the one field established across current
consumers: `numFrames` at offset zero. Both the C++ `dExtAnmTexSRT_c` header
and OV007 C helper use this definition. This is explicitly a prefix view; the
remaining file layout and total size are unknown and are not asserted.

## Verification so far

- `func_ov007_020bc32c` (`ov007`, `0x020bc32c`, `0x48`): `match.py` passed;
  strict `linkcheck.py` returned `VERIFIED`, `diffs: []`, `blind: 0`.
- `dExtAnmTexSRT_c::SetFile` (`arm9`, `0x020158ac`, `0x54`) and `Prepare`
  (`arm9`, `0x0201587c`, `0x0c`): focused matches and strict linkchecks passed;
  both linkchecks returned `VERIFIED`, `diffs: []`, `blind: 0`.
- `python tools/rombuild.py -j16 --no-rom`: 11,395 functions and 468
  source-owned data claims reproduce; 106/106 modules exact; zero mismatches.
- Declaration agreement has no new branch disagreements. Dead references,
  TU conflicts, and port references pass.

## Remaining review

Independently inspect the prefix-only claim and rerun the focused checks on the
queued candidate SHA. Keep the PR draft until source review and hosted byte
validation are complete.
