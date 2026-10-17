# Handoff: fold-ov006-curling2-1005

## What changed and why

- **Class/TU and ROM scope:** `dScMgCurling2_c`, ov006 `.text 0x020e59b0..0x020e6bf4`,
  20 functions — the upper side of the 52-function linker run
  `tu_map` calls `0x020e3854..0x020e6bf4`. The sourceless hole at
  `func_ov006_020e5450` (a banked near-miss draft, not enrolled) splits the
  run, so it licenses as two contiguous sides: the lower TU
  `src/minigames/d_s_mg_curling2.cpp` (already promoted) and the new
  `src/game/actors/dScMgCurling2_c_upper.cpp`. The zero-gap `d_s_mg_curling2`
  classInit at `0x020e6bf4` is the next unit and stays separate.
- **Fold:** the 20 scattered shards (16 `func_ov006_*` helpers + 4 already-
  mangled vtable slots) became one TU under `#pragma defer_codegen off`,
  absorbed delinks consolidated into one `.text` block, shard files retired.
- **Deslop:** all 20 are now real `dScMgCurling2_c::` methods (membership is
  ROM-proven — 8-byte {code pointer, zero adjustment} pointer-to-member
  records in `.data 0x0213c3d4..0x0213c47c`, filled into the BSS dispatch
  tables by `__sinit_ov006_02130758`). The twelve `func_ov006_*` helpers the
  lower TU kept C-named pending this promotion also converted
  (DrawValues..SeparateStones), all byte-neutral.
- **Header:** `include/dScMgCurling2_c.h` gained the `mMark[5]` record
  (0x4870), the bg-scroll/game fields, and the 28 method declarations; the
  stone record fields are named from their uses.
- **Config:** ov006 `delinks.txt`, `symbols.txt` (28 renames to mangled
  spellings — required, `.data` PMF records resolve by name), a new
  `config/tu_manifest.d/ov006/dScMgCurling2_c_upper.json`
  (`compiler_only_output: []` — the class's key function is the destructor
  in the lower TU, the outcome the lower manifest predicted), 20
  `path#symbol` keys in `attribution.json` (`tangosdev`, the shards' bulk-
  import credit), 11 dead `decl_common.h` decls removed, 6 stale
  decl-agreement baseline rows removed, a note appended to the lower
  manifest.

## Leftovers (measured, documented in the files)

- `func_ov006_020e5450` stays `extern "C"` and unenrolled: the hole's own
  cartridge bytes cover it; the banked draft is nonmatching (30/344).
- StoneSpin, StoneSlide, NextStone, SeedStones, Play, ResetGame (upper) and
  DrawMarks, AgeMarks (lower) keep the shards' raw `this + 0x46xx/0x55xx`
  addressing; folding through the named members loses the base
  rematerialization, same measured reason as `SpawnValue`.

## Verification

- `tools/match.py --module ov006` per function: 20/20 upper + 12/12
  converted lower = 32/32 MATCH under mwccarm 2004/b56, strict reloc-
  destination check on.
- Gates: see the PR body for `prepush_linkcheck`, `rombuild`,
  `check_src_tu_compiles`, `check_decl_agreement`, `port_refcheck`,
  `check_dead_references`, `the old credit check`, `queue_audit`,
  `romdata_check` results.

## Resumption

- Worktree `fold-ov006-curling2`, branch `fold/ov006-curling2-1005`, fork
  `lunavyqo`, one non-draft PR to `tangosdev/main`.
