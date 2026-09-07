# Handoff: issue-2410

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  https://github.com/tangosdev/sm64ds-decomp/issues/2410 — task `issue-2410`,
  stage `reconstruct` (role producer), session `prod-2410-0907`, harness Claude Code.
- Source branch and previous accepted input SHA:
  `cpp/daMky_c-v2`, continued from accepted input `3e931ef05c60621201384397652bfdc73c2e86cd`
  (branch `cpp/Ukiki-tu`, PR #2397). The new branch descends from that commit; the
  work was brought forward by two merges of `origin/main`, never a rebase, because
  the queue requires the published output to descend from the accepted input.
- Original source base SHA and installed workflow/tool SHA:
  base `b2bd6a323832d244a12e750713b1405b26a94c66`;
  workflow/tools `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Separate evidence commits and required artifacts in this commit:
  no separate evidence commits. `notes/data/class-facts/daMky_c.json` is committed on
  this branch. Required artifacts present: `src/actors/daMky_c.cpp`,
  `config/tu_manifest.d/ov030/daMky_c.json`.
- Next action, responsible role and blockers:
  independent verification (verifier). No known blocker. One scope note below:
  `symbols/actor_renames.tsv` is not reserved to this task.
- Status: verified candidate for the stage's scope — the red gate this stage was
  opened to clear is now green, and every other applicable gate was re-run and
  recorded below. The class itself is NOT finished reconstruction; see
  "Remaining agreed issue scope".
- Remaining uncommitted/local-only material and where it is preserved:
  none. The private queue receipt and all build output stay in gitignored `build/`
  in worktree `C:/tmp/sm64ds-mky2410` and are not committed. The v1 worktree
  `C:/tmp/sm64ds-scout-ukiki-0905` still holds `cpp/Ukiki-tu` at the input commit,
  untouched.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope:
  `daMky_c` in ov030. The promoted TU owns `.text 0x02111688..0x021145e0`
  (44 functions). No other section is claimed.
- Reserved source/header/config surfaces actually touched by THIS stage: none.
  This stage changed exactly one file, `symbols/actor_renames.tsv`, plus this
  handoff. Everything else on the branch is the adopted v1 input, carried forward
  unchanged through two merges.
- ROM observations:
  ov030 0x02115b78 holds the NUL-terminated `_ZTS` payload `7daMky_c`, pointed at
  by `_ZTI7daMky_c` at 0x02115b84; the class vtable symbol is at 0x02115bfc. The
  string `Ukiki` occurs nowhere in the cartridge. `tools/rtti_vtables.py` answers
  only to `daMky_c`. That is why the coined spelling was retired, and it is what
  made the eight ledger rows false.
- Lineage evidence or structural inference:
  the eleven `EnterState<N>` members are inferred, not read: each writes a unique
  immediate 0..10 to the state word at +0x3b4 and the descriptor table at
  0x02115e0c holds exactly eleven 0x10-byte records. The word "EnterState" is
  coined; only the class name and the number come from the cartridge. Each such
  row states this in its own ledger `why` column.
- Hypothesized names/filenames, explicitly not recovered facts:
  the file name `src/actors/daMky_c.cpp` is a repository convention, not a
  recovered original filename. `EnterState`, and the `_classInit_MONKEY_STAR` /
  `_classInit_MONKEY_THIEF` factory spellings, are coined.
- Compiler experiments and measured barriers:
  none run in this stage. The input's own measured barriers are recorded in
  `config/tu_manifest.d/ov030/daMky_c.json` under `compiler_only_output`: the TU
  defines the class key function (`~daMky_c`, the first virtual declared), so
  mwccarm emits `_ZTV7daMky_c` as vague-linkage data, 0x84 of storage. The
  cartridge's canonical copy at 0x02115bfc lies outside the entry's only licensed
  range, so production isolation discards the emitted copy and dsd supplies the
  ROM's own bytes. `romdata_check` compares the emitted copy before that discard.

### The gate this stage was opened to clear

`check_rename_ledger` reads every row's fourth column in the present tense as a
live address to symbol map (`tools/cpp_index.py`, `tools/cpp_rename.py` consume it
that way). It has no last-row-wins rule, so appending a correction is not enough.

This branch renames the ov030 symbols to `daMky_c` in
`config/arm9/overlays/ov030/symbols.txt`. Eight earlier rows still asserted the
retired `Ukiki` spelling at those same addresses. Measured: the checker is green on
`origin/main` (1923 rows checked) and red only with this branch's symbols.txt
applied (1944 rows, 8 findings, all ov030). The stale rows are therefore this
change's own regression, not pre-existing integration-lane drift.

`python tools/check_rename_ledger.py --repo . --fix` rewrote all eight and left
none for a human. Addresses and slot notes are unchanged; each row's `why` column
records the retired claim:

| address | was | now | slot |
|---|---|---|---|
| 0x02111688 | `_ZN5UkikiD1Ev` | `_ZN7daMky_cD1Ev` | 16 |
| 0x021116d0 | `_ZN5UkikiD0Ev` | `_ZN7daMky_cD0Ev` | 17 |
| 0x021141c4 | `_ZN5Ukiki16CleanupResourcesEv` | `_ZN7daMky_c16CleanupResourcesEv` | 3 |
| 0x0211422c | `_ZN5Ukiki16OnPendingDestroyEv` | `_ZN7daMky_c16OnPendingDestroyEv` | 12 |
| 0x02114230 | `_ZN5Ukiki6RenderEv` | `_ZN7daMky_c6RenderEv` | 9 |
| 0x02114278 | `_ZN5Ukiki8BehaviorEv` | `_ZN7daMky_c8BehaviorEv` | 6 |
| 0x02114378 | `_ZN5Ukiki13InitResourcesEv` | `_ZN7daMky_c13InitResourcesEv` | 0 |
| 0x02115bfc | `_ZTV5Ukiki` | `_ZTV7daMky_c` | vtable |

No `alloc=` figure was withdrawn — the vtable row already carried `alloc=?`. The
`RollingLogTtm` generation displaced from the `why` column survives on that class's
own rows at 0x02111624, 0x02111638, 0x0211164c and 0x02115a48, so the file loses no
name. The edit is isolated in its own single-file commit so it can be dropped or
replayed alone.

## Reconstruction dimensions

Recorded separately, not combined into one score.

- Exact function/byte and relocation coverage:
  44 of 44 declared functions MATCH under `tools/match.py` extract+compare;
  `tools/objisolate.py plan()` clean on relocation type and addend;
  `tools/reloc_audit.py check_destinations()` clean on relocation target identity;
  emission order is the expected ROM-ascending section order. Coverage limit: the
  TU claims `.text` only, so the byte gate says nothing about its data sections —
  that is what `romdata_check` covers, below, and it is only PARTIAL there.
- Genuine methods; remaining free-function/ABI bridges:
  20 of the 44 are compiler-spelled `_ZN7daMky_c*` methods. 24 are still
  `func_ov030_*` free functions inside the TU (0x02111734, 0x02111890, 0x02111908,
  0x02111a00, 0x02111b20, 0x02111bc4, 0x02111dd0, 0x02111ea4, 0x02111f6c,
  0x02112094, 0x021122b0, 0x02112400, 0x02112578, 0x02112a84, 0x02112da0,
  0x02113094, 0x02113324, 0x021136b0, 0x02113b38, 0x02113d20, 0x02113ff0,
  0x02114134, 0x02114170, 0x021141a8). Not all of them need to become methods;
  some are plausibly original file-static helpers.
- Recovered layout/fields; remaining shadow structs/raw offsets:
  `include/daMky_c.h` is 126 lines and still carries 5 `unk_` fields
  (`unk_380`, `unk_384`, `unk_388`, `unk_3a8`, `unk_3cb`) plus 5 `pad_` runs.
  `src/actors/daMky_c.cpp` matches `tiers.py`'s own `RAW_OFFSET` regex on
  **381 of its 2,320 lines, 447 occurrences** — 90 lines inside the 20 converted
  methods, 291 inside the 24 bridges — and the file contains **zero** named-field
  accesses. Layout recovery is partial, and the remaining scope should be planned
  from 381.
  (Corrected during verification and re-measured at integration. The first draft
  of this document said 8 `unk_` fields, a `grep -c unk_` artifact that counted 3
  comment lines, and 52 raw-offset lines, which understated the gap by ~7x.)
- Lifecycle, vtable/RTTI, initializer and data ownership:
  the TU owns the key function and therefore emits the vtable, but claims no
  non-text section, so `_ZTV7daMky_c`, `_ZTI7daMky_c`, `_ZTS7daMky_c` and the four
  inherited base RTTI records are all externalized — the ROM supplies them. This
  is the text-only promoted shape, shared by 107 of the tree's 124 promoted TUs.
  The branch also touches `src/__sinit_ov029_02112c10.c`, a cross-overlay static
  initializer in ov029; it is covered by the full-ROM build below, which is green.
- Attribution preserved through each move/rename:
  `attribution.json` carries the 44-shard fold's `path#symbol` overrides; that was
  the input commit's own last change. `tools/port_refcheck.py` reports 402
  references checked and none stale, so no `port/` reference was stranded by the
  file move or the rename.
- Remaining agreed issue scope:
  24 free functions to convert to methods where evidenced, 5 `unk_` fields to name,
  381 raw-offset lines (447 occurrences) to retire, and the `romdata_check` extent
  shortfall to close — which for `_ZTV7daMky_c` means correcting the `symbols.txt`
  extent, not the class model; see the PARTIAL breakdown under Proof.
  "Promoted" is packaging state; this class is not a finished reconstruction.

## Proof

All commands run in worktree `C:/tmp/sm64ds-mky2410` at this commit's tree, on the
pinned compiler `tools/mwccarm/2004/b56/mwccarm.exe`.

- Full-ROM build:
  `python tools/rombuild.py -j16 --no-rom` — exit 0. 11,191 source-built functions,
  11,191 reproducing, 0 mismatching (2,140,496 / 2,238,108 code bytes, 95.64%);
  module fidelity 106/106 exact, 100.000000% of compared bytes; intact TU gates
  "dsd modules PASS, zero new symbol errors, storage aliases exact"; ROM data from
  source 686 verified / 215 partial / 5 differ / 407 unnamed. Report at
  `build/rombuild-report.json` (gitignored). The baseline control in the same run
  reports **9** `dsd check symbols` error lines with NO TU substitution —
  `overlay_100`, `overlay_102`, `data_020ad524`, `data_020ad560`, `func_01ff98f4`,
  `func_01ff99a4`, `func_01ff9d40`, `_deq`, `func_01ff9e2c` — not the two this
  document first named. By that control's own definition all nine belong to the
  tree, not to this TU, and the operative comparison (zero *new* symbol errors)
  passed. The 5 differing data symbols are in arm9, ov002 and ov084 — none in
  ov030, and `romdata_check` on this TU alone reports DIFFERS 0.

- Explicit function/consumer relocation checks:
  `python tools/prepush_linkcheck.py --range origin/main..HEAD` — exit 0.
  927 checked, 629 verified, 298 warnings, 0 blocking. Coverage limit: the 298
  warnings are `NO-SYM` rows, which are names with no symbols.txt counterpart; that
  class of warning is present tree-wide and is not evidence about this change.

- Complete emitted TU and data/metadata checks:
  `python tools/tubuild.py verify ov030/daMky_c` — exit 0.
  "44/44 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED",
  all 44 in expected ROM-ascending emission order.
  `python tools/romdata_check.py --files src/actors/daMky_c.cpp` — exit 0.
  9 emitted data symbols: VERIFIED 4 (44 bytes: `_ZTI7daMky_c`, `_ZTI8dActor_c`,
  `_ZTI7dBase_c`, `_ZTI7fBase_c`), PARTIAL 5 (156 bytes equal, coverage short of
  the ROM's extent), DIFFERS 0, UNNAMED 0.
  The 5 PARTIALs were run down during verification rather than assumed, because
  `romdata_check` hides per-symbol verdicts in both stdout and `--json`. Four are
  `_ZTS` type-name strings short by trailing alignment — `_ZTS7dBase_c`,
  `_ZTS7daMky_c`, `_ZTS7fBase_c`, `_ZTS8dActor_c`, each `emitted=9..10`,
  `romExtent=12`, `compared=8`, `differing=0` — the benign sibling pattern.
  The fifth is the **vtable**: `_ZTV7daMky_c`, `emitted=124 romExtent=132
  compared=124 differing=0 blindWords=0`. The cartridge at ov030 0x02115bfc holds
  31 real slots and then two zero words, and the ov030 initialised image ends at
  0x02115c80 where `data_ov030_02115c80 kind:bss` begins, so the 8-byte shortfall
  is unnamed module tail padding. **All 31 real vtable slots are byte-exact with
  zero blind words.** This is an extent overrun in `symbols.txt`, not a short
  vtable; closing it means correcting the extent, and that belongs to a separate
  change. Still do not read PARTIAL as VERIFIED — but the shortfall is now
  characterised rather than unknown.
  NOT AVAILABLE: `python tools/tubuild.py linkcheck ov030/daMky_c -j16 --no-rom`
  exits 1 at step 4/8 with "intact production requires one .text claim and at least
  one non-text claim". That is the harness path, not this TU: the identical command
  on `ov002/daObjAbuku_c`, already landed on main, fails with the identical message,
  and 107 of the tree's 124 promoted TUs are text-only. (That 107/124 was
  re-measured at integration and reproduces exactly: of the 124 manifests whose
  `status` is `promoted`, 107 declare no non-`.text` section and no `data`/`bss`
  claim.) The intact-object link gate that does apply is the one inside
  `rombuild.py` step 4/6, which passed.
  A second coverage limit worth stating plainly: `daMky_c` is itself one of
  `prepush_linkcheck`'s 298 `NO-SYM` warnings, so that gate proves nothing about
  the TU under test. Its relocation proof rests on `tubuild verify` and the
  full-ROM link, not on linkcheck.

- Shared-header consumer expansion:
  not applicable to this stage — it edited no header. `include/daMky_c.h` is
  carried unchanged from the accepted input and its consumers are covered by the
  full-ROM build above.

- Port/path/reference and other applicable static gates:
  `python tools/port_refcheck.py` — exit 0, 402 references checked, all resolve.
  `python tools/check_dead_references.py` — exit 0, no new dead references, no
  broken markdown links.
  `python tools/check_rename_ledger.py --repo .` — exit 0, "every mangled/vtable
  row agrees with its module's symbols.txt", 1944 checked. Coverage limit, stated
  by the tool itself: 1532 coined rows are out of scope and unchecked, so this
  green does not cover coined spellings such as `UkikiStar_Spawn`.
  `python tools/premerge_check.py HEAD --base origin/main` — exit 0, computed on
  the merge tree: converted-ratchet, dead-references, duplicate-sources,
  header-offsets, langmode-ratchet, layout-check, src-tu-refs and source-coverage
  all pass on both base and merge; nothing goes green to red.
  `python tools/queue_audit.py` — exit 0, reports 3 stale columns on ONE row,
  `dScMgAmida_c` (ov006). That row is byte-identical on `origin/main`, so it is
  pre-existing drift on an unrelated class; it was deliberately not rewritten here,
  because doing so would put an unrelated ov006 row and a CRLF renormalization into
  this candidate.
  `python tools/cpp_tu_state.py` — exit 0.

- Private validation, if run, and the exact PR head/base it tested:
  NOT RUN at this commit. The private validator ran on the predecessor branch
  `cpp/Ukiki-tu` head `3e931ef0` against main, and passed — "ROM data reproduced
  from source | 686 symbol(s) exact", no lost-symbol line — on PR #2397, whose only
  red check was `ledger`. That result tested a different head from this one and is
  not proof for this commit. A fresh private validation on this candidate's actual
  head and base is still required before it lands.

## Scope note for the coordinator and integrator

`symbols/actor_renames.tsv` is integration-lane state and is NOT reserved to
task `issue-2410`. The issue lists it among the files deliberately left unreserved
because every branch in this wave edits them, and names running the ledger fix as
this task's next action; the accepted input commit already modified this file
(+21 rows). This stage's edit is confined to 8 rows and isolated in a single
commit so it can be reverted or replayed alone. Please ratify it with
`classqueue.py v2 amend` on the task, or tell the producer to drop that commit.

A log generated after this commit belongs in separately recorded evidence.
