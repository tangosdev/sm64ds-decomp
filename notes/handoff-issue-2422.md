# Handoff: issue-2422

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2422, `issue-2422`, stage
  `reconstruct` (producer), session `prod-2422-0907`, Claude Code.
- Source branch and previous accepted input SHA: `cpp/daBrq_c-bank`; accepted
  input `9fded5f5242a10f04f318da3bfd477993a192d20` (tip of the local-only
  `cpp/dabrq-sinit-resource-tu`).
- Original source base SHA and installed workflow/tool SHA: original base
  `6f0a3ab9bd6d01373744d32cedb7e0f3dc6cffb8`; this commit is built on `main` at
  `88dbe66db2cb3f0cd1dc704e9f2775eb37aea646`; workflow/tool pin
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Separate evidence commits and required artifacts in this commit: none
  separate. This commit adds `notes/sinit-dabrq-ownership.md`,
  `notes/sinit-probes/dabrq.cpp`, and this handoff. The private queue receipt
  stays under an ignored `build/` directory and is not part of the commit.
- Next action, responsible role and blockers: independent verification
  (verifier) of the reproduction recorded in the note, then integration. No
  blockers. The `rom_data_regressions` gate defect tracked at
  https://github.com/tangosdev/sm64ds-decomp/issues/2409 is not exercised
  here because this commit renames no symbol and touches no `symbols.txt`.
- Status: verified candidate for the narrowed scope (a proof note whose claim
  was re-measured under the pinned compiler on this base). Nothing is WIP.
- Remaining uncommitted/local-only material and where it is preserved: the
  original branch `cpp/dabrq-sinit-resource-tu` at `9fded5f52` and its
  worktree remain in place, untouched. Its other two commits are superseded
  (see below), not lost.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov070/daBrq_c`; the
  initializer `__sinit_ov070_02122d80` at `ov070:0x02122d80..0x02122f30`, its
  `.data` PMF descriptors `ov070:0x0212320c..0x0212323c`, and its `.bss`
  objects `ov070:0x021235ec..0x02123698`. Nothing in those ranges is
  re-enrolled or re-owned by this commit.
- Reserved source/header/config surfaces actually touched: none. Only
  `notes/` changed. The reserved TU, header, manifest, `delinks.txt`,
  `symbols.txt`, and `symbols/actor_renames.tsv` are untouched; zero rename
  rows appended.
- ROM observations: symbol spacing in `config/arm9/overlays/ov070/symbols.txt`
  shows five 8-byte handles, six 12-byte registration nodes, one 12-byte
  vector, one 48-byte state array ending exactly at the next symbol, and six
  8-byte PMF descriptors ending exactly at `_ZTS7daBrq_c`. No absorbed array,
  no phantom index.
- Lineage evidence or structural inference: the original task assumed the
  branch's promotion was unbanked. It is banked: the branch's promotion commit
  `883c4cf4a` is the same change as `7402d8be8`, already on `main` via
  https://github.com/tangosdev/sm64ds-decomp/pull/2084, and the later profile
  campaign moved the TU to `src/game/actors/daBrq_c.cpp` and renamed
  `Amp_Spawn` to `daBrq_c_classInit` (now in that cpp) and `Amp_SpawnInfo` to
  `g_profile_BIRIKYU`. Every differing line between the branch TU and `main`'s
  is that rename or comment prose. The branch's tools commit `e4650db21` was
  superseded by https://github.com/tangosdev/sm64ds-decomp/pull/2074. The only
  content unique to the branch was the note and probe banked here.
- Hypothesized names/filenames, explicitly not recovered facts: the probe's
  `Brq*Probe` class names are deliberately neutral stand-ins; the note says so
  and forbids promoting them.
- Compiler experiments and measured barriers: the guarded probe compiled with
  `swarm.CPP_FLAGS` plus `-DSINIT_OWNERSHIP_PROBE` reproduces
  `__sinit_ov070_02122d80` byte-for-byte (`0x1b0/0x1b0`) with 34/34
  relocation offsets, types, and addends equal; only the 34 symbol spellings
  differ. Without the define the probe emits no production sections.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: unchanged by this commit. For
  the record, `main`'s manifest lists 18 functions and `symbols.txt` lists 18
  `_ZN7daBrq_c*` symbols (set difference empty both ways); the 19th class
  symbol is the factory `daBrq_c_classInit` at `0x021210ac`, now enrolled in
  `src/game/actors/daBrq_c.cpp` with the rest of the class. Neither side
  dropped a function.
- Genuine methods; remaining free-function/ABI bridges: unchanged; the factory
  is the one C bridge.
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The
  destructor stays inline in `include/daBrq_c.h` with `InitResources` as the
  key function; the manifest's `data` list still owns `_ZTS`, `_ZTI`,
  `g_profile_BIRIKYU`, and `_ZTV`. The initializer stays separately enrolled
  in `src/__sinit_ov070_02122d80.cpp`, identical between the branch and `main`.
- Attribution preserved through each move/rename: no move or rename here.
- Remaining agreed issue scope: none after this commit; the promotion is
  already on `main`. The engine-level reconstruction of the resource wrapper
  types at `0x020178b4..0x02017b64` that would let the initializer move into
  the TU is a separate, unscoped follow-up named in the note.

## Proof

All commands were run in the worktree on this commit's tree at base
`88dbe66db`, pinned mwccarm 2004/b56 toolchain (wired and canary-verified by
the worktree helper).

- Full-ROM build: not run; not applicable. No enrolled source, header,
  linker configuration, manifest, or symbol changed (`git diff --name-only`
  against `main` lists only `notes/`), so the ROM cannot differ from `main`.
- Explicit function/consumer relocation checks: the probe reproduction above,
  exit 0: `bytes: 0x1b0/0x1b0 equal=True`, `relocations: 34/34`, all offsets,
  types, and addends equal, 34 name-only differences, probe without the define
  emits no `.text`/`.init`/`.data`/`.bss`/`.ctor`/`.rodata` content. Compared
  `__sinit_dabrq.cpp` from the probe object against `__sinit_ov070_02122d80`
  compiled from the committed transcription with the same flags.
- Complete emitted TU and data/metadata checks: not run; no TU changed.
- Shared-header consumer expansion: not applicable; no header changed.
- Port/path/reference and other applicable static gates, all exit 0:
  `python tools/check_dead_references.py` (no new dead references, no broken
  markdown links, nothing flagged in the two new files);
  `python tools/layout_check.py --quiet` (clean);
  `python tools/port_refcheck.py` (402 checked, all resolve);
  `python tools/check_rename_ledger.py --repo .` (1923 rows agree);
  `python tools/check_src_tu.py --json build/src_tu_refs.json` (every
  reference resolves). No notes-directory policy file exists on `main`; the
  tree-shape workflow runs `tools.test_srcpath` and `tools.test_bytegate`,
  which do not read `notes/`.
- Private validation, if run, and the exact PR head/base it tested: not run
  locally; the PR's CI result is separately recorded evidence.
