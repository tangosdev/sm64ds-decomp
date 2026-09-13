# Handoff: prod-pkn-0907 — daPkn_c (ov084)

2026-09-08 continuation: this is the original promotion record; its proof stays
scoped to the commits named below. Current source-review corrections and remaining
work are recorded in [pr-2450-source-review-fixes.md](pr-2450-source-review-fixes.md).
The vtable-boundary explanation below is corrected against the existing dossier.

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no issue yet; task `prod-pkn-0907`,
  producer stage, session `01AQcMFFbm4nBC42tFbMKCj6`, Claude Code / Opus 5. The
  coordinator enqueues from the SHA below.
- Source branch and previous accepted input SHA: `cpp/daPkn_c-tu`; no previous
  producer commit — this branch starts from main.
- Original source base SHA and installed workflow/tool SHA: base
  `4caf9765c9b57e96b29485d1ec4170ebb0660b58` (`origin/main`, #2442). Workflow pin
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
  **The brief pinned base `26f54f8fc`. main moved three times during this task
  (#2439 stamp, #2437 daMip_c, #2442 daObjBk_Dossunbar_c); this branch was
  restacked onto the tip and every figure below is measured against
  `4caf9765c`, with a rebuilt base control at that same SHA.**
- Separate evidence commits and required artifacts in this commit: three commits —
  the rename, the shadow TU, the promotion. No separate evidence commit.
- Next action, responsible role and blockers: verifier, then integrator. No
  blockers. One deferred improvement is named under "Remaining agreed issue scope".
- Status: verified candidate.
- Remaining uncommitted/local-only material and where it is preserved: none. Build
  outputs are gitignored.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daPkn_c`, ov084
  `.text 0x0212eaf0..0x02130174`, 24 functions, folded into one translation unit at
  `src/actors/daPkn_c.cpp`. This also opens ov084, which had no manifest before.
- Reserved source/header/config surfaces actually touched: `ov084/delinks.txt`
  (24 per-function entries → one `complete` span), `ov084/symbols.txt` (8 member
  rows and one `_ZTV` row renamed; the duplicate coined `_ZTV` alias row dropped),
  a new `ov084/daPkn_c.json` manifest, the class header (renamed from the coined
  spelling), `decl_common.h`, `attribution.json`, `converted-baseline.json`,
  `actor_renames.tsv`, the queue and worklist rows, the class-facts dossier, the
  enemy provenance note and the generated TU-state note.
- ROM observations:
  - **The name is the cartridge's.** ov084 0x02130bec holds the nine bytes of the
    length-prefixed `_ZTS` payload for `daPkn_c`; `_ZTI7daPkn_c` at 0x02130bf8
    points its +4 word at that string and the vtable's −4 header word at 0x02130c24
    points back at the `_ZTI`. `PiranhaPlant` occurs in none of the 106 extracted
    images. That spelling is now gone from `src/`, `include/` and `symbols.txt`.
  - **Base `dEnemyBase_c`**, from the destructor at 0x0212eaf0: it stores this
    vtable, destroys six member subobjects in reverse declaration order and tails
    into `_ZN12dEnemyBase_cD2Ev`. Chain daPkn_c → dEnemyBase_c → dActor_c →
    dBase_c → fBase_c. 31 vtable slots, 8 own overrides, no new virtuals.
  - **Vtable extent, corrected 2026-09-08.** The 31 slots at ov084:0x02130c28
    end at 0x02130ca4; seven zero words separate them from the `.data` end at
    0x02130cc0. The base/peer slot counts and inherited final slot support this
    extent, as recorded in the dossier. The section boundary alone does not:
    this last-table case still has the sizing limitation described below.
  - D1 at 0x0212eaf0 sits BELOW D0 at 0x0212eb48 and there is no D2 anywhere.
- Lineage evidence or structural inference: object size 0x47c is the ROM's own
  literal, loaded at 0x0213016c and handed to `fBase_c::operator new`; the last
  touched word (0x478) closes on it. The registry profile is `PAKUN`, id 0xfa, at
  0x02130c04.
- Hypothesized names/filenames, explicitly not recovered facts: `daPkn_c_classInit`
  is a reconstructed source-style factory name (historical alias
  `PiranhaPlant_Spawn`); the exact original spelling is not preserved. The 15
  `func_ov084_*` members keep their address names — see the deferred item below.
- Compiler experiments and measured barriers:
  - **The destructor is inline, and the ordering recipe was followed literally.**
    D1-below-D0 with no D2 is what mwccarm 2004/b56 emits for an inline in-class
    destructor; the out-of-line form emits D2/D0/D1, in the wrong order and with a
    homeless D2. The header carries the empty in-class body and the TU writes no
    destructor at all. The `tubuild linkcheck --baseline` control was taken BEFORE
    the flip, while the header still declared the destructor out of line, so the
    still-enrolled D0/D1 shards compiled; the flip and the fold are one commit.
    `defer_codegen off` was not needed — emission order came out ROM-ascending on
    the first compile.
  - The generator's preamble was discarded rather than patched: it carried 25
    declaration CONFLICTs and one truncated declaration. The file follows the
    declaration policy `daBmb_c` established — ONE file-scope `extern "C"` region
    carrying only what the C++-named members need, and every free member wrapped in
    its own `extern "C"` block. `decl_common.h` is not included: it contradicts
    five of this TU's own members and restates the vtable the class header now owns.
  - `extern int _ZTV7daPkn_c[];` is declared in the class HEADER, not restated in
    the source. Measured consequence: the langmode `extern _ZTV vptr store` count
    is 164 on both sides of the change.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 24/24 byte-match; 24/24 VERIFIED by
  `linkcheck.py` driven by symbol; objisolate clean; reloc destinations clean;
  emission order ROM-ascending; 0 unlicensed sections.
- **The fold is 24, not the 23 the queue said.** Both the queue's `shard_count`
  and `build/tu_map.json` segment on symbol NAME, and `daPkn_c_classInit` at
  0x02130110 does not look like a member of this class, so it was cut off from a
  run it physically abuts. Confirmed by address against `delinks.txt`: no gap
  between 0x02130110 and the function below it, and the factory closes exactly on
  the end of ov084 `.text` at 0x02130174. This is issue #2436. The run below the
  TU (0x0212ea18..0x0212eaf0) is daFPkn_c's three factories and is NOT absorbed.
- Genuine methods; remaining free-function/ABI bridges: 6 C++-named members
  (`InitResources`, `Behavior`, `Render`, `OnPendingDestroy`, `CleanupResources`,
  `OnAimedAtWithEgg`), the two compiler-emitted destructor bodies, the factory, and
  15 free `func_ov084_*` members still reaching fields through this-relative
  arithmetic.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header's typed
  member list is unchanged from the pre-fold state and closes exactly on 0x47c.
  Three uniquified shadow types remain in the TU (`PknVec3`, `PknMtx43`,
  `PknStateCls`) plus one two-word view of the shared-file records
  (`PknSharedFile`); the byte match is in those spellings.
- Lifecycle, vtable/RTTI, initializer and data ownership: **route is text-only.**
  ov084 delinks no `.data` at all — every entry in its delinks file is a `.text`
  range — so the TU claims `.text` only. It still compiles a vtable and an RTTI
  group, because the inline destructor makes `daPkn_c::OnAimedAtWithEgg` the key
  function and it is defined here; 12 sections are licensed as
  `compiler_only_output` and deadstripped, and the cartridge's copies stay
  ROM-supplied. The `compiler-only:>=11` floor the queue predicted is met exactly
  by the 11 data rows; the twelfth is the `Vector3` inline destructor duplicate.
  - An **intact-object** route with a `.data` claim was NOT attempted, and is
    blocked on tooling rather than on anything about this class: `daPkn_c` is one
    of the 27 classes whose vtable is the last `.data` object in its overlay with
    no bounding symbol row after it, so `dsd` would size it out to the section end
    and refuse in `[2/8] dsd delink`. Text-only does not claim `.data` and does not
    reach that. Flagged as blocked on the tool fix, not as a property of the class.
- Attribution preserved through each move/rename: 24 override rows added,
  **placed beside this overlay's existing rows rather than at the tail** — the
  diff is 24 insertions and 0 deletions, so it cannot collide with another
  producer's tail append. The same-branch rename+promote hazard was checked and
  did NOT bite here: `prepush_attribution.lineage` follows renames, and all 24
  owners agree between HEAD and the base (verified stem by stem against the
  pre-rename spellings). `validate_merge` reports 0 added, 0 changed, 0 lost, and
  per-contributor base-vs-head totals moved for 0 of 13 contributors, function for
  function and byte for byte.
- Remaining agreed issue scope: **the nine pointer-to-member records are left
  name-resolved by address.** They live at 0x02130ba4..0x02130bec and
  `__sinit_ov084_02130654` builds them into the state table in `.bss` at
  0x02130e80; `daPkn_c::Behavior` dispatches through it. `dsd` resolves those
  `.data` words by SYMBOL NAME, so renaming any target is a same-commit
  `ov084/symbols.txt` edit, and a mangled TU beside a stale name there links every
  one of those words as 0. The evidence to name them safely was not gathered, so
  the address names stay and this is recorded as the next improvement rather than
  guessed at.

## Proof

Base control rebuilt at `4caf9765c` in a second wired worktree; head figures from
this commit's tree. No cached report reused.

- Full-ROM build: `python tools/rombuild.py -j16`, exit 0.
  - head: 106/106 modules exact, 100.000000% of compared bytes; source-built
    functions 11,192 reproducing / **0 mismatching**; intact-TU gates PASS
    (dsd modules PASS, zero new symbol errors, storage aliases exact).
  - base control at `4caf9765c`: 106/106 exact, 100.000000%, 11,192 reproducing /
    0 mismatching.
  - ROM sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`
    on BOTH sides — byte-identical.
- Explicit function/consumer relocation checks:
  - `tools/tubuild.py verify ov084/daPkn_c`: **24/24 MATCH**, objisolate clean,
    reloc destinations clean, emission order ROM-ascending, 0 unlicensed
    section/symbol.
  - `tools/linkcheck.py` driven **by symbol** (`--c`/`--name`/`--addr`/`--size`/
    `--module`, once per manifest function, not by file stem): **24/24 VERIFIED** —
    every relocation resolved and every linked byte equal to the ROM. 0 BLIND,
    0 WRONG, 0 NO-SYM.
- Complete emitted TU and data/metadata checks:
  - `tools/romdata_check.py`: head VERIFIED 698 / PARTIAL 222 / DIFFERS 4 /
    UNNAMED 393; base control 697 / 221 / 4 / 395. **DIFFERS unchanged**; +1
    verified, +1 partial, −2 unnamed.
  - `tools/tubuild.py linkcheck ov084/daPkn_c`: **REFUSED** —
    `intact production requires one .text claim and at least one non-text claim`.
    **Control run before reporting it**: `tubuild.py linkcheck ov102/daBmb_c`, a
    landed and merged text-only TU, refuses identically, together with the same two
    unrelated failures (`d_a_obj_rc_carpet.cpp`, `d_a_propeller_heyho.cpp`, vtable
    partition baseline fingerprint). Once delinks carries the final `complete`
    entry, linkcheck routes a text-only TU down the intact-object path, which by
    construction demands a non-text claim it cannot have. Structural to the tool,
    not a defect in this TU; the full ROM build and the by-symbol linkcheck are the
    link proof.
  - `tubuild.py linkcheck --baseline`, taken before the destructor flip: modules
    PASS, ROM built, sha256 identical. `dsd check symbols --fail` FAILs there with
    9 pre-existing tree-wide errors (ITCM `func_01ff98f4`/`func_01ff99a4`/
    `func_01ff9d40`/`_deq`/`func_01ff9e2c`, `overlay_100`, `overlay_102`,
    `data_020ad524`, `data_020ad560`) — present with NO TU substitution at all.
- Shared-header consumer expansion: `check_src_tu` 30 TUs / 316 includes / 783
  mangled references, every reference resolves. `check_header_offsets` passes
  through `premerge_check`. `eligible.py` 11,195 / 11,273.
- Port/path/reference and other applicable static gates, all exit 0:
  - `validate_merge --base 4caf9765c --head HEAD` with both ROM reports: byte-verified
    functions +0, code bytes +0, contributor credit 0/0/0, module fidelity 106/106.
    Warning: 24 address ranges left the byte-verified set while enrolled totals held
    steady — that is the fold, one range per absorbed member, and nothing else.
  - `premerge_check --base 4caf9765c HEAD`: 8/8 gates pass on base and on the merge
    tree; **nothing goes green → red**. source-coverage info: entries 9011 → 8988
    (−23), consolidation, bytes flat.
  - `check_rename_ledger`: 2,014 mangled/vtable rows all agree with symbols.txt.
    Rows were rewritten IN PLACE for the 8 renamed members and the `_ZTV`, with the
    9 hop rows appended at the tail; the rebase conflict against the parallel ov085
    ledger append was resolved as a union in landing order, never hand-edited.
  - `check_dead_references`: no new dead references, no broken markdown links.
  - `check_tubuild_conflicts`: every recorded conflict matches the source and every
    conflict in a source is recorded — all 25 generator CONFLICT notes rewritten as
    `(RESOLVED)` with the resolution stated.
  - `langmode_audit`: every ratcheted count flat or lower against the base control.
    `extern _ZTV vptr store` 164 on both sides; `local struct body` 1701 → 1699;
    `...and no include` 905 → 904.
  - `tiers_ratchet --check` only (never `--update`, which appends): PASS,
    baseline 2701, current 2704.
  - `layout_check` clean; `port_refcheck` 423 references, 0 stale;
    `cpp_tu_compat --require-ready` all READY.
  - `queue_audit --check`: **FAIL, and a base control at `4caf9765c` fails
    identically** — 4 `already_promoted`, 3 `compiler-only`, 4 `shard_count`,
    4 `total_lines`, 1 `unmatched`, the same counts on both sides. Pre-existing on
    main, belonging to other PRs. This branch's own row was updated by renaming it
    to `daPkn_c`, re-running the tool, and then restoring every row the tool
    rewrote that belongs to someone else; the committed diff is exactly one line.
    `--write` was NOT banked.
- Private validation, if run, and the exact PR head/base it tested: not run — no PR
  was opened, per the brief.

Retired shards, by basename only: `d_a_pkn.c`, `func_ov084_0212ebb4.c`,
`func_ov084_0212ec04.c`, `func_ov084_0212ec60.c`, `func_ov084_0212ef00.c`,
`func_ov084_0212f1d0.c`, `func_ov084_0212f204.c`, `func_ov084_0212f298.c`,
`func_ov084_0212f2dc.c`, `func_ov084_0212f33c.cpp`, `func_ov084_0212f460.cpp`,
`func_ov084_0212f588.cpp`, `func_ov084_0212f630.cpp`, `func_ov084_0212f6d8.c`,
`func_ov084_0212fa7c.c`, `func_ov084_0212fc10.c`, and the eight mangled member
files for D1, D0, InitResources, Behavior, Render, OnPendingDestroy,
CleanupResources and OnAimedAtWithEgg.
