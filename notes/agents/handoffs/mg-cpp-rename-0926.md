# Handoff: mg-cpp-rename-0926

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171, task `mg-cpp-rename-0926`, stage `revise`, role producer, session `claude-prod-mg-cpp-rename-0926`, Claude Code.
- Source branch and previous accepted input SHA: `readable/mg-cpp-rename-0926`, input `52e467a1ef96ad22b705f9ad1abb8dc5871d61b8`.
- Original source base SHA and installed workflow and tool SHA: both `52e467a1ef96ad22b705f9ad1abb8dc5871d61b8`.
- Separate evidence commits and required artifacts in this commit: none. The branch has three commits on the base: `a9094c9074` (the six renames, nothing else), `937bd706a4` (delinks, registry, pilot, notes and baseline), and this handoff.
- Next action, responsible role and blockers: independent verification of the rename. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted or local-only material: the base build lives in a second worktree at `C:/tmp/claude-mgp-cpp-rename-base` (detached at the base). The candidate build lives in the ignored build folder of the producer worktree. The four comparison scripts (object, module, cell and command-line comparisons) were in the session scratchpad and are not committed. Each is described below well enough to reproduce.

## What changed and why

- Scope: the six one-function ov006 factories that #3190 rewrote as `return new dScMgX_c;`. They were C++ under a `.c` name. Each now has a `.cpp` name.

| New path | Symbol | ov006 address, size | Registry row |
|---|---|---|---|
| `src/d_s_mg_bomroom.cpp` | `dScMgBomroom_c_classInit` | 0x020d9574, 0x30 | `MG_BOMROOM` |
| `src/d_s_mg_curling.cpp` | `dScMgCurling_c_classInit` | 0x020e3820, 0x34 | `MG_CURLING` |
| `src/d_s_mg_curling2.cpp` | `dScMgCurling2_c_classInit` | 0x020e6bf4, 0x34 | `MG_CURLING_J` |
| `src/d_s_mg_pachinko.cpp` | `dScMgPachinko_c_classInit` | 0x020ff3ec, 0x34 | `MG_PACHINKO` |
| `src/d_s_mg_panel.cpp` | `dScMgPanel_c_classInit` | 0x02107858, 0x34 | `MG_PANEL` |
| `src/d_s_mg_teresa.cpp` | `dScMgTeresa_c_classInit` | 0x021207a8, 0x34 | `MG_TERESA` |

- `srcpath.new_path_for(<symbol>, '.cpp')` returns each new path, and `srcpath.path_for(<symbol>)` resolves to it after the rename.
- Surfaces touched:
  - the six sources (rename only);
  - `config/arm9/overlays/ov006/delinks.txt`: the six path header lines only;
  - `symbols/profile_reconstruction_registry.tsv` and `.json`, `symbols/profile_reconstruction_pilot.tsv` and `.json`;
  - `notes/tu-filename-reconstruction-pilot.md`, `notes/minigames/themes.md`;
  - `config/dead-reference-baseline.json`;
  - this handoff.
- No header, class source, `attribution.json` or declaration baseline change. No generator was run: `tools/profile_reconstruction.py` was not run in any mode.
- Names: none coined.

### Own-row grants, and exactly what they changed

Cells were compared between the base and HEAD blobs, as parsed TSV rows and as JSON trees. Nothing else differs: the same header, row count, key order and line count, and LF endings in the index.

- `symbols/profile_reconstruction_registry.tsv`: 12 cells. `current_factory_file` (the path under `src/`) and `factory_filename` (the bare file name) change extension from `.c` to `.cpp` for `MG_PACHINKO`, `MG_BOMROOM`, `MG_CURLING`, `MG_CURLING_J` (`curling2`), `MG_PANEL` and `MG_TERESA`.
- `symbols/profile_reconstruction_registry.json`: the same 12 cells, in `rows[378]`, `[380]`, `[384]`, `[385]`, `[390]` and `[397]`.
- `symbols/profile_reconstruction_pilot.tsv` and `.json`: 2 cells each. `current_factory_file` of `MG_CURLING` (json `rows[18]`) and `MG_CURLING_J` (`rows[19]`). No other pilot cell named an old path. `class_filename_candidate` and `probable_filename` were already `.cpp`.
- `notes/tu-filename-reconstruction-pilot.md`: rows `MG_CURLING` and `MG_CURLING_J` (lines 107 and 108). Only the link text and target of the current-source column changed.
  - PR #3195 changes line 94 (`STAR_CAMERA`) of this doc. It also changes line 377 of ov006 `delinks.txt`.
  - `git merge-tree --write-tree 937bd706a4 13beca99a4` (the #3195 head) exits 0 with tree `f5e33b1aa6`. That tree carries both edits in both files. The same command from this commit also exits 0.
  - No other open PR touches any file in this change.
- The generator would agree with the 12 registry cells. `profile_reconstruction.source_for` resolves each factory through `srcpath.path_for`, which now returns the `.cpp` path. `assign_factory_filenames` copies that extension. This was checked by calling `source_for` alone.

### Theme 4 and the dead-reference bank

- `notes/minigames/themes.md` theme 4 names the six `.cpp` paths. The sentence about `.c` factories is replaced by one saying the form is C++, with a `.cpp` name and `//cpp`, and that the rename changed no byte.
- `config/dead-reference-baseline.json` gains six rows, one per old path, for `notes/agents/handoffs/mg-p1-factories-0926.md`, which is a historical record and is not edited. They were inserted by hand after the `issue-2411-humanizer-2.md` rows and before the `mg-p1-tamaire-0925.md` rows. That is the sorted (file, ref) position.
  - The file still round-trips exactly through `json.dumps(indent=1)`: same bytes, same order, no duplicates.
  - `--update` was not run.

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 6 of 6 functions. The objects are byte-identical to the base build's, and so are the linked modules (below).
- Genuine methods and bridges: unchanged from #3190. Each file is `//cpp`, the class header, then an `extern "C"` factory returning `new dScMgX_c`.
- Layout, lifecycle, vtable and data ownership: unchanged. Relocations are `R_ARM_PC24` to `_ZN7fBase_cnwEj` (+0x08), `R_ARM_PC24` to `_ZN11dScMgBase_cC2Ev` (+0x14), and `R_ARM_ABS32` to `_ZTV<C>`. The `_ZTV<C>` literal is at +0x2c for Bomroom and +0x30 for the others.
- Attribution preserved through each rename: yes. All six renames are R100, in a commit of their own. The per-file lineage in the gates table is unchanged.
- Remaining agreed issue scope: out of scope here are the Slot3 factory, which stays C and will be absorbed by the Slot1+Slot3 TU, and Pachinko2, which is retired by #3196.

## R100 evidence

- `git diff -M --stat 52e467a1ef HEAD -- src/` lists 6 files with 0 insertions and 0 deletions.
- `git diff -M --summary 52e467a1ef HEAD` prints `rename src/{d_s_mg_<name>.c => d_s_mg_<name>.cpp} (100%)` for each of `bomroom`, `curling`, `curling2`, `pachinko`, `panel` and `teresa`.
- Commit `a9094c9074` contains only those six renames.

## Proof

All commands ran at `-j8` with the pinned `2004/b56` compiler.
- **Base:** a separate worktree at `52e467a1ef`.
- **Candidate:** the producer worktree at `937bd706a4`. Its tree differs from this commit only by this handoff. `check_dead_references` and `git diff --check` were re-run with the handoff present.
- **Inputs:** both worktrees have their own copies of `extracted/`, the compiler and `tools/bin/`, not junctions.

### Bytes

| Command | Exit | Result |
|---|---|---|
| `python tools/rombuild.py -j8 --no-cache` at base | 0 | `intactTuRom.identical` true, ROM sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`; 11,214 of 11,214 functions reproducing; 106 of 106 modules exact; object cache off, 6,884 compiled, 0 reused |
| the same at candidate | 0 | identical figures and the same sha256 |
| byte comparison of the base and candidate `build/build/` trees | 0 | 111 files in each (106 module `.bin` files), 0 differ; `sm64ds.nds` and `final_link.o` also byte-identical |
| ELF comparison of the six `build/src/d_s_mg_<name>.o` | 0 | whole files byte-identical (sha256 prefixes `d086a400`, `57005b89`, `85fe66a4`, `ee11a43a`, `0e88cc5e`, `8bfd142c`); same sections, symbols and `.rela.text` entries; no `STT_FILE` symbol, so the source name is not in the object |

Each `.cpp` was compiled and linked:

- `build/objects.txt` lists `build/src/d_s_mg_<name>.o` for all six.
- The candidate tree has no `.c` file for them, and its `delinks.txt` names only the `.cpp`.
- The link map (`build/final_link.o.xMAP`) places each factory in its object at the address and size in the table above. The six map rows are identical to the base build's.

Compiler command line:

- rombuild's `compile_one` was called for each file with `subprocess.run` intercepted before anything ran. The recorded command lines are identical at base and candidate apart from the worktree root and the source extension.
- Both are `mwccarm.exe -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off -i include -c <src> -o build/src/d_s_mg_<name>.o`, with no per-file version override.
- rombuild picks `-lang c++` from the leading `//cpp`, not from the extension, so the language mode did not change.

### Gates

| Command | Exit | Result |
|---|---|---|
| `python tools/prepush_linkcheck.py --range 52e467a1ef..HEAD` | 0 | 6 checked, 6 verified, 0 warnings, 0 blocking |
| `python tools/prepush_attribution.py --base 52e467a1ef` | 0 | 7,056 tracked, 0 moved, 0 renamed, 0 consolidated, 0 changed, 0 lost (see findings) |
| `prepush_attribution.lineage` and `chaos_db_ci.first_matchers`, base against HEAD, for the six stems | 0 | unchanged: `andrewboudreau` for bomroom, curling and curling2; `tangosdev` for pachinko, panel and teresa |
| `python tools/check_decl_agreement.py --changed 52e467a1ef` | 0 | 12 files in scope, 6 symbols, 0 disagreements, no new |
| `python tools/check_decl_agreement.py`, at base and candidate | 0, 0 | byte-identical output: 35,671 declarations, 14,422 disagreements, 1,505 banked gone, no new |
| `python tools/check_decl_agreement.py --list`, at base and candidate | 0, 0 | byte-identical (57,692 lines); none of the six files appears |
| `python tools/check_dead_references.py`, at base | 0 | 635 non-resolving, none new |
| `python tools/check_dead_references.py`, at candidate with this handoff | 0 | 641 non-resolving: the six banked rows; no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check`, at base and candidate | 0, 0 | both CONVERTED PASS, baseline 3008, current 3238 |
| `python tools/langmode_audit.py --check langmode-baseline.json`, at base and candidate | 0, 0 | ratchet PASS |
| `python tools/langmode_audit.py --json <file> --full`, at base and candidate | 0, 0 | only `totals.c_extension` (4,497 to 4,491) and `totals.cpp_extension` (2,559 to 2,565) differ; not-migrated count stays 73 (27 `.c`, 46 `.cpp`) |
| `python tools/check_profile_campaign.py`, at base and candidate | 0, 0 | byte-identical output: 401 rows, 391 in scope, 391 complete, 0 pending, 0 diverged; 970 checks complete; 201 exempt; 790 ledger claims, 0 diverge |
| per-row read of the six rows through `check_profile_campaign`'s own functions | 0 | descriptor, factory symbol and filename all COMPLETE, at base on the `.c` and at candidate on the `.cpp` |
| control: `scan_registry` with the base registry against the renamed tree | 0 | 385 complete, 6 pending (the six filename checks), 0 diverged |
| `python -m unittest tools.test_check_profile_campaign tools.test_profile_reconstruction tools.test_attribution` | 0 | 108 tests OK |
| `python tools/port_refcheck.py` | 0 | 408 references resolve |
| `python tools/check_src_tu_compiles.py --quiet` | 0 | 306 of 306 compile |
| `python tools/check_tubuild_conflicts.py` | 0 | 306 entries; every conflict recorded |
| `python tools/queue_audit.py --check-promoted` | 0 | 226 of 228 rows audited; all agree |
| `git diff --check 52e467a1ef HEAD` | 0 | clean |

Private validation: not run. Independent verification of this exact candidate is a separate acceptance gate.

## Remaining old-path mentions

`git grep -n -E "d_s_mg_(bomroom|curling2?|pachinko|panel|teresa)\.c\b"` at this commit finds only these:

| File | Mentions | What it is | Disposition |
|---|---|---|---|
| `notes/agents/handoffs/mg-p1-factories-0926.md` | 8 lines | historical handoff of #3190 | kept; its six dead references are banked |
| `config/dead-reference-baseline.json` | 6 rows | the bank for the handoff above | added by this change |
| `config/tu_manifest.d/ov006/dScMgPanel_c.json` | 2 (`boundary_evidence[3]`, `notes[7]`) | prose in the Panel TU manifest | kept unchanged; no gate reads them as paths |
| `notes/data/class-facts/dScMgPanel_c.json` | 3 (`candidate_text.factory_is_excluded`, `factory.verification`, `queue_row_corrections[0].why`) | prose in a scout facts record | kept unchanged, as a record |
| `notes/experiments/pr2874-integration-0920.json` | 25 (`strict_relocations[].file`, `consumer_partition_audit` commands, files and inventory rows) | record of the 0920 integration experiment | kept unchanged, as a record |
| `notes/data/c-cpp-classification.tsv` | 6 rows | generated census of `.c` files | kept unchanged; a refresh drops these six rows |
| `notes/data/tu-merge-candidates.json` | 6 (`single_file_tus[].files[]`) | generated TU table | kept unchanged, until regenerated |

## Findings

1. rombuild picks the language from the `//cpp` marker alone. A `//cpp` file renamed from `.c` to `.cpp` gets the same command line apart from the path, and here produced byte-identical objects. The `assign_factory_filenames` docstring says a `.c` to `.cpp` rename "would cost the byte match". That holds only for a `.c` file without `//cpp`.
2. `prepush_attribution.py` keys lineage on the path without its extension. An extension-only rename is neither "moved" nor "renamed" in its summary, so it prints `0 renamed with credit intact`, not 6. Credit is still checked, on the stem, and the per-stem authors above are unchanged. No `attribution.json` override was needed.
3. Without the registry edit, `check_profile_campaign` would not fail. The control above reads the six rows as pending (385 complete, 6 pending), and the tool fails on pending rows only under `--strict`. Only the unchanged counts show that the edit is complete.
4. PR #3195 also edits ov006 `delinks.txt` (line 377), not only the pilot doc. Both files merge cleanly with this branch.
