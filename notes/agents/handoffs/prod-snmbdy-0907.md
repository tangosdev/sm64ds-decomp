# Handoff: prod-snmbdy-0907

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness:
  no issue was opened for this task. Task `prod-snmbdy-0907`, stage `reconstruct`
  (role producer), session `prod-snmbdy-0907`, harness Claude Code.
- Source branch and previous accepted input SHA:
  `cpp/daBgSnmBdy_c-tu`. Greenfield — there is no accepted input commit and no
  predecessor branch. The branch was cut from `origin/main` at
  `a278141a946931f7e243f2886de692bc4423674b` and brought forward by two merges of
  `origin/main`, never a rebase.
- Original source base SHA and installed workflow/tool SHA:
  base at branch creation `a278141a946931f7e243f2886de692bc4423674b`; every gate
  below was re-run against the pinned base
  `ecc97391e7b356afbd5a1ede6f924d92559bacb5`; workflow/tools
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
  Two base discrepancies, both stated rather than smoothed over. The task briefing
  named `e4fb1300e` as `origin/main`; the fetched ref was already
  `a278141a9`, which descends from it and differs only in generated progress
  artifacts. And `origin/main` moved three times during the run —
  `a278141a9` to `6e04e1090` to `ecc97391e` to `26f54f8fc`. The base is pinned at
  `ecc97391e` because everything after it (`26f54f8fc`) is a progress refresh
  touching only README, `contributions.json` and the two generated docs pages: no
  source, config, symbol or delink file this candidate reads or writes.
- Separate evidence commits and required artifacts in this commit:
  no separate evidence commits. Required artifacts present:
  `src/actors/daBgSnmBdy_c.cpp`, `include/daBgSnmBdy_c.h` and
  `config/tu_manifest.d/ov072/daBgSnmBdy_c.json`.
- Next action, responsible role and blockers:
  independent verification (verifier), then central sequencing. No PR was opened
  and nothing was merged, as instructed. No known blocker. Two scope notes below:
  `symbols/actor_renames.tsv` and `notes/data/tu-promotion-queue.tsv` are
  integration-lane files this task had to touch.
- Status: verified candidate for this stage's scope. Every gate that can run in a
  public checkout was run against the pinned base and is recorded below with its
  exit code, including the two that are not green and why. The class itself is NOT
  a finished reconstruction; see "Remaining agreed issue scope".
- Remaining uncommitted/local-only material and where it is preserved:
  none is uncommitted. All build output stays in gitignored `build/` in worktree
  `C:/tmp/sm64ds-sm64ds-snmbdy`; the base control build stays in gitignored
  `build/` in the second wired worktree `C:/tmp/sm64ds-snmbdy-base`. Both worktrees
  were left in place and neither was torn down.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope:
  `daBgSnmBdy_c` in ov072 — the big rolling snowman's body actor. The promoted TU
  claims two ranges and nothing else: `.text 0x0211f000..0x0211ff34` (29 functions)
  and `.data 0x02122780..0x0212283c`. The route is `intact-object`, so the `.data`
  claim is load-bearing rather than optional.
- Reserved source/header/config surfaces actually touched by THIS stage:
  the class's own source, header and manifest; `config/arm9/overlays/ov072`
  `delinks.txt` and `symbols.txt`; `include/decl_common.h` (one duplicate
  declaration removed); `config/converted-baseline.json`; `attribution.json`;
  `symbols/actor_renames.tsv`; and one row of `notes/data/tu-promotion-queue.tsv`.
  The remaining touched files are mechanical re-spellings written by
  `tools/class_rename.py`.
- ROM observations:
  ov072 0x0212278c holds the length-prefixed Itanium `_ZTS` payload
  `12daBgSnmBdy_c`, NUL-terminated. `_ZTI12daBgSnmBdy_c` at 0x02122780 points its
  +4 word at that string; the vtable's -4 header word at 0x021227bc points back at
  the `_ZTI`. The `_ZTI` is a three-word `__si_class_type_info` whose +8 word is
  0x0208e390 == `_ZTI8dActor_c`, so `dActor_c` is the sole base at offset zero.
  The vtable at 0x021227c0 is 31 slots — exactly `dActor_c`'s own length, which a
  `dBgActor_c` child could not be, since `dBgActor_c` adds a 32nd. Seven slots are
  overridden: 0, 3, 6, 9, 12, 16 and 17, read word by word out of the cartridge.
  The class size 0x3a8 comes from the factory's own `fBase_c::operator new(936)`.
  The string `SnowmanBody` occurs nowhere in the cartridge.
- Lineage evidence or structural inference:
  the fold is 29, not the queue's 28. `build/tu_map.json` cuts the class run at
  0x0211f000..0x0211fedc and leaves the immediately following factory run
  0x0211fedc..0x0211ff34 unattributed. An earlier draft of this note explained
  that by saying the tree spelled the factory under the retired coined class
  name. That explanation is false and is withdrawn: at the pinned base
  `config/arm9/overlays/ov072/symbols.txt` already read
  `daBgSnmBdy_c_classInit` at 0x0211fedc, and regenerating `tu_map` at this tree,
  with all 29 names ROM-spelled, still leaves the run unattributed. The
  observation reproduces; the causal claim does not. The mechanism is in
  `tools/tu_map.py`: `factory_vtable_labels` only ever considers a name ending
  `_Spawn`, so a `_classInit` factory never enters the attribution path no matter
  how its class is spelled. The join is not a
  tu_map inference: the main registry maps BIG_SNOWMAN_BODY to the 0x0212279c
  descriptor whose first word relocates to `daBgSnmBdy_c_classInit` at 0x0211fedc,
  that function allocates the factory-proven 0x3a8, installs
  `_ZTV12daBgSnmBdy_c`, constructs the five header-declared subobjects, and ends
  exactly at the next class's D1 at 0x0211ff34. The run is contiguous with no gap
  on either side of the join.
  The six state pairs are likewise read, not guessed from adjacency: the static
  initializer at 0x02122018 copies twelve ROM pointer-to-member constants into
  `data_ov072_02122b64` in those exact pairs; `SetState` indexes that table at
  `state * 0x10`, `CallStateInit` invokes the first of each pair and
  `CallStateBehavior` the second.
- Hypothesized names/filenames, explicitly not recovered facts:
  `daBgSnmBdy_c_classInit`, `g_profile_BIG_SNOWMAN_BODY` and the path
  `src/actors/daBgSnmBdy_c.cpp` are reconstructed source-style spellings, not
  recovered identifiers; the historical coined aliases were `SnowmanBody_Spawn`
  and `SnowmanBody_SpawnInfo`. `State<N>` and `InitState<N>` claim the PMF table
  INDEX and nothing more — the cartridge carries no descriptive identifier for any
  of the twelve. `IsPlayerNearCenter`, `UpdateRollAngle`, `AdvancePath`,
  `HurtPlayer`, `UpdateGroundCollision`, `UpdateModel`, `CallStateInit`,
  `CallStateBehavior` and `SetState` are inferred private spellings: their class
  ownership, callers, bodies, relocations and codegen are proven, the original
  English words are not. `UpdateGroundCollision`'s mangled name asserts a POINTER
  parameter (`P10dBgCh_Actr`); that is a disclosed guess. Rule 1 of
  `notes/tu-promotion-conventions.md` requires the disclosure in all three places
  that can carry it. As produced, only two carried it -- the manifest entry's
  `notes` and the `why` column of the rename ledger row. Point 3, the comment on
  the declaration, is the gap rule 1 names by name as the one #2055 left open,
  and this branch left it open too. The integration added it at both
  `include/daBgSnmBdy_c.h`'s declaration and the definition site in
  `src/actors/daBgSnmBdy_c.cpp`; all three places carry it now.
- Compiler experiments and measured barriers:
  three, all recorded because each changed the shape of the result.
  1. The first compile emitted a static initializer object and a
     `data_ov072_02122b20` definition of size 1. Cause: six extern data
     declarations inside the `extern "C"` block were written without `extern`, so
     they were definitions, and `SharedFilePtr`'s constructor forced an sinit.
     Adding `extern` removed the sinit and the spurious definition.
  2. `tubuild.py linkcheck --baseline` builds the WORKING TREE. Running it with
     the final header in place is a compile error, not a link one: the inline
     destructor redefines the two destructor shards that are still enrolled at
     that moment. The baseline was taken with a rename-only header, then the final
     header restored.
  3. `tu_promote.py` is not atomic. It spliced `delinks.txt` and moved the source
     before failing on a `git rm` against a path carrying both a staged rename and
     a working-tree edit. Recovery was to restore `delinks.txt`, put the source
     back, commit a throwaway checkpoint so HEAD, index and worktree agreed, and
     re-run. The throwaway commits were squashed out; the branch carries none.

### Oracle decisions copied from `daBgSnwmn_c`

`config/tu_manifest.d/ov072/daBgSnwmn_c.json` and
`src/game/actors/d_a_bg_snwmn.cpp` were used as a line-by-line template. What was
copied, and nothing else:

- the `intact-object` route with a real `.data` claim rather than the text-only
  shape that 107 of the tree's promoted TUs use;
- the destructor form: declared LAST in the class body and inline, so mwccarm
  emits the retail D1 then D0 pair at the bottom of the run and no separate D2
  body. The brace stays on the signature line, because `check_header_offsets`
  only arms its body skip when the signature line itself carries the `{`;
- the class-local `static void *operator new` routing to `_ZN7fBase_cnwEj`, so an
  ordinary `new daBgSnmBdy_c()` reaches the retail allocator and the COMPILER
  emits and owns the vptr store. No `_ZTV` address is spelled by hand anywhere in
  the source;
- the typed 0x1c profile struct with a compile-time size assert, and the
  `extern "C" <Class> *<Class>_classInit() { return new <Class>(); }` factory;
- the two `STB_LOPROC -> STB_GLOBAL` binding rewrites for D1 and D0;
- the six `canonical-import` externalized vague-linkage RTTI records
  (`_ZTI7fBase_c`, `_ZTS7fBase_c`, `_ZTS7dBase_c`, `_ZTS8dActor_c`,
  `_ZTI7dBase_c`, `_ZTI8dActor_c`) with their canonical arm9 addresses;
- the manifest key order and the style of its `notes` block.

Not copied: the sibling's own addresses, sizes, relocations, member list or
`compiler_only_output` entries, all of which were derived from this class's own
ROM bytes.

## Reconstruction dimensions

Recorded separately, not combined into one score.

- Exact function/byte and relocation coverage:
  29 of 29 declared functions MATCH under `tools/match.py` extract+compare;
  `tools/objisolate.py` `plan()` clean on relocation type and addend;
  `tools/reloc_audit.py` `check_destinations()` clean on relocation target
  identity; all 29 emitted in the expected ROM-ascending section order. Unlike a
  text-only promotion this one also has its `.data` compared: `tubuild.py
  linkcheck` reports 0 differing bytes in BOTH licensed ranges.
- Genuine methods; remaining free-function/ABI bridges:
  28 of the 29 are compiler-spelled `_ZN12daBgSnmBdy_c*` methods and the 29th is
  the registry factory. There are no `func_ov072_*` shards left in the run — that
  is what made this candidate well shaped. The remaining ABI debt is in the other
  direction: the TU still reaches ~30 other-module seams by writing their mangled
  names in one `extern "C"` block instead of calling declared C++ methods.
- Recovered layout/fields; remaining shadow structs/raw offsets:
  `include/daBgSnmBdy_c.h` is 122 lines and declares the five constructed
  subobjects typed (`Model` at 0xd4, `ShadowModel` at 0x124, `dCcAc_c` at 0x14c,
  `dBgCh_Actr` at 0x180, `PathPtr` at 0x380) plus the scalar tail, with a
  `sizeof == 0x3a8` assert. One `unk_` field remains (`unk_3a4`) and three
  explicit pad runs. `check_header_offsets` reads 15 commented fields, 0
  mismatched and **0 unparsed**, spanning 0x38c.
  The source has not caught up with the header. Measured with `tools/tiers.py`'s
  own `RAW_OFFSET` regex: **18 of the 27 out-of-line members still reach the
  object through raw byte offsets on a char\* handle, 154 occurrences across 117
  of the file's 639 lines.** The worst are `State2` (26), `State3` and `State5`
  (20 each), `UpdateModel` (19) and `InitState0` (18). Plan the remaining scope
  from 117, not from the header's tidy appearance.
- Lifecycle, vtable/RTTI, initializer and data ownership:
  the TU defines the class key function, so mwccarm emits `_ZTV12daBgSnmBdy_c`,
  `_ZTI12daBgSnmBdy_c` and `_ZTS12daBgSnmBdy_c` as vague-linkage data — and
  because this is the intact-object route the TU also CLAIMS them, at their
  natural ROM addresses, rather than letting a gap object supply them. The
  manifest spells both `emitted_storage_address` 0x021227b8 and
  `address_point_bias` 0x8 for the vtable, since the compiler's storage carries
  the two-word ABI preamble while the configured public address point stays at
  0x021227c0. The four `.data` entries, 36 relocations and the section emission
  order (12, 16, 17, 19 — ROM-ascending, matching mwld's ascending-size `.data`
  layout) were all derived from the cartridge, and every relocation target
  resolves to a named symbol in a checked-in `symbols.txt`.
  Two `compiler_only_output` entries record what the compiler emits that the
  cartridge does not keep here: `_ZN7Vector3D1Ev` (`deadstrip-duplicate`;
  `InitResources`, `HurtPlayer`, `UpdateGroundCollision` and `State3` each declare
  a `Vector3` local, and the canonical copy lives at arm9 0x020072c0) and
  `_ZN9Matrix4x3D1Ev` (`deadstrip`; emitted while lowering the inline destructor
  over the `Matrix4x3` member at 0x350, with no configured ROM symbol of that name
  in any module).
  There is no static initializer: the TU emits none, which is the correct answer
  for this class and was confirmed by removing the accidental one described above.
- Attribution preserved through each move/rename:
  `attribution.json` carries **two** blocks for this class, 58 rows for 29
  functions, and both are required because the two gates key differently.
  `validate_merge` resolves `path#symbol` on the symbol's CURRENT name;
  `prepush_attribution` looks each deleted shard up by its own retired file STEM.
  With only the current-name rows the first gate is green and the second reports
  29 CREDIT LOST. Both blocks sit beside this class's own rows rather than at the
  tail of the object, so the diff against the base is two insertion hunks with
  zero deletions and no collision with any other class doing the same thing. The
  landed ov030 sibling carries the same pair (60 rows for 44 functions).
  Five of the current-name rows had the wrong author on the first attempt and were
  corrected against the base tree's own resolution — override, then match
  finisher, then first matcher — rather than by guesswork.
- Remaining agreed issue scope:
  117 raw-offset lines (154 occurrences) across 18 members to retire; one `unk_`
  field to name; ~30 mangled other-module seams to replace with declared C++
  calls; and the `romdata_check` extent shortfall on the four `_ZTS` strings to
  close. "Promoted" is packaging state. This class is not a finished
  reconstruction.

## Proof

All commands run in worktree `C:/tmp/sm64ds-sm64ds-snmbdy` at this commit's tree
except where a second worktree is named, on the pinned compiler
`tools/mwccarm/2004/b56/mwccarm.exe`. Base for every comparison is the pinned
`ecc97391e7b356afbd5a1ede6f924d92559bacb5`. Both ROM reports were regenerated at
these exact trees; neither is a cached report from an earlier base.

- Full-ROM build:
  `python tools/rombuild.py -j16` — exit 0. 11,192 source-built functions, 11,192
  reproducing, 0 mismatching (2,140,712 / 2,238,108 code bytes, 95.65%); module
  fidelity 106/106 exact, 100.000000% of compared bytes; 24 source-owned data
  claims, 24 reproducing, 0 mismatching; intact TU gates "dsd modules PASS, zero
  new symbol errors, storage aliases exact"; ROM data from source 697 verified,
  220 partial, 4 differ, 397 unnamed. The linked ROM hashes sha256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, which is the
  stock cartridge and the same hash the baseline control produces.
  The base control, `python tools/rombuild.py -j16 --no-rom` in the second wired
  worktree `C:/tmp/sm64ds-snmbdy-base` at `ecc97391e` — exit 0: 11,192 / 11,192 /
  0, 106/106 exact, 23 source-owned data claims, ROM data 695 verified / 219
  partial / 4 differ. Head gains 2 verified data symbols and 40 verified bytes
  over base and loses none.
  Coverage limit stated by the run itself: `dsd check symbols --fail` FAILS with 9
  error lines (`overlay_100`, `overlay_102`, `data_020ad524`, `data_020ad560`,
  `func_01ff98f4`, `func_01ff99a4`, `func_01ff9d40`, `_deq`, `func_01ff9e2c`). The
  baseline control with NO TU substitution has exactly the same 9, so 0 are new —
  but the check is not green and must not be reported as green.

- Explicit function/consumer relocation checks:
  `python tools/pr_linkcheck.py --base ecc97391e -j16 --fail` — exit 0. The two
  changed headers fan out to 917 files, all checked. `src/actors/daBgSnmBdy_c.cpp`
  is VERIFIED on all 29 slots. Five files are not plain VERIFIED — one DRAFT
  (`_ZN12dScStarSel_c8BehaviorEv`) and four BLIND (`RollingIronBall`,
  `daObjMarioCap_c`, `dScDSMT_c`, `func_ov089_0213162c`) — and every one of them is
  in a file and module this candidate does not touch.
  `python tools/prepush_linkcheck.py --range ecc97391e..HEAD` — exit 0, 914
  checked, 620 verified, 294 warnings, 0 blocking. Coverage limit: this class is
  itself one of those warnings. `prepush_linkcheck` returns `NO-SYM` for a
  promoted multi-symbol TU by construction, so it proves nothing about this
  change; the relocation proof rests on `linkcheck.py` driven by symbol,
  `pr_linkcheck`, `tubuild.py verify` and the full-ROM link.
  `python tools/linkcheck.py --module ov072 --name <symbol> --addr <addr>
  --size <size> --c src/actors/daBgSnmBdy_c.cpp`, run for each of the 29 members
  — **29/29 VERIFIED, 0 otherwise.**
  A correction, because the first version of this document got it wrong and a
  wrong version would be read as a fact about the repository. `--name` used ALONE
  does need the per-function ledger `progress/matched.jsonl`, which is gitignored
  and was absent from this worktree, and the read of it is unguarded, so a bare
  `--name` dies with `FileNotFoundError` rather than the tool's own message. That
  is a property of THIS worktree's missing gitignored inputs, not of the tool:
  the ledger lookup is skipped entirely when `--addr` and `--size` are supplied,
  and the tool says so itself — "not in progress/matched.jsonl; pass
  --addr/--size/--module". Both values come straight out of this class's manifest.
  Do not read the earlier claim that the tool is unavailable; it is available,
  and this is its result.

- Complete emitted TU and data/metadata checks:
  `python tools/tubuild.py linkcheck ov072/daBgSnmBdy_c -j16` — exit 0,
  **SCRATCH-DATA-VERIFIED**. Both licensed ranges reproduce from one object with
  **0 differing bytes**: `.text 0x0211f000..0x0211ff34` and `.data
  0x02122780..0x0212283c`. Object audit 33 LICENSED, 0 non-licensed symbols, 0
  unlicensed sections, emitted text order ROM-ascending. `dsd check symbols
  --fail` fails on the same 9 pre-existing errors with 0 new and 0 resolved. The
  scratch ROM hashes to the stock sha256 above and is IDENTICAL to the tree's
  stock build.
  `python tools/romdata_check.py --files src/actors/daBgSnmBdy_c.cpp` — exit 0.
  10 emitted data symbols: VERIFIED 6 (196 bytes) — `_ZTV12daBgSnmBdy_c` 124
  bytes, `_ZTI12daBgSnmBdy_c` 12, `g_profile_BIG_SNOWMAN_BODY` 28, plus the three
  inherited `_ZTI` records; PARTIAL 4 (36 bytes equal); DIFFERS 0; UNNAMED 0. The
  four PARTIALs are all `_ZTS` type-name strings whose emitted length falls short
  of the ROM's aligned extent, with 0 differing bytes — the benign sibling
  pattern. A PARTIAL is still not a VERIFIED and is not reported as one.
  `python tools/tubuild.py verify ov072/daBgSnmBdy_c` — **exit 1**, and this is
  the one result that is not green. It reports "29/29 MATCH, objisolate clean,
  reloc-destinations clean -> TEXT-VERIFIED" and then refuses: "6 unlicensed
  section/symbol(s) present -> PROMOTION REFUSED". The six are exactly the
  inherited vague-linkage RTTI records that the manifest already declares under
  `externalized_output`; `verify` reads only the `data`/`bss` lists and reports
  them as "not in manifest". This is a harness path, not a property of this TU:
  the identical command on the LANDED sibling `ov072/daBgSnwmn_c` on main exits 1
  with the identical six records and the identical wording. `tubuild.py linkcheck`
  looks at the same object with `externalized_output` in scope and reports 0
  unlicensed sections.

- Shared-header consumer expansion:
  covered by `pr_linkcheck` above — two changed headers, 917 files fanned out and
  checked, and by the full-ROM build, which compiles every enrolled consumer.

- Port/path/reference and other applicable static gates, each exit 0 unless said
  otherwise:
  `port_refcheck.py` 423 references checked, 0 stale.
  `check_rename_ledger.py` "every mangled/vtable row agrees with its module's
  symbols.txt", 2025 rows checked; coverage limit stated by the tool itself —
  1532 coined rows are out of scope and unchecked.
  `check_dead_references.py` no new dead references, no broken markdown links.
  `check_duplicate_sources.py` 9175 stems, none doubled.
  `check_src_tu.py` 30 TUs, 316 includes, 783 mangled references, every one
  resolves. `check_tubuild_conflicts.py` 160 manifest entries.
  `check_data_definitions.py` 8770 objects, none define a ROM data symbol.
  `check_python_names.py` PASS. `check_layout_free.py` 0 stored as data.
  `check_profile_campaign.py` 786 claims, 0 diverge.
  `check_header_offsets.py include/daBgSnmBdy_c.h` 15 commented fields, 0
  mismatched, 0 unparsed, struct spans 0x38c.
  `langmode_audit.py --check langmode-baseline.json` PASS.
  `tiers_ratchet.py --check` PASS, baseline 2699, current 2703, +4 gained and
  deliberately NOT banked — `--update` appends rather than replaces, so running it
  here would double-bank rows permanently.
  `premerge_check.py HEAD --base ecc97391e` exit 0 on the MERGE TREE: all eight
  static gates pass on both base and merge; nothing goes green to red. Its
  source-coverage info line reads "entries: 9066 -> 9039 (-27) (consolidation, not
  a loss -- bytes are flat)".
  `prepush_attribution.py --base ecc97391e --head HEAD` exit 0, 29 consolidated
  with credit intact, 0 changed, 0 lost — green only after the retired-stem rows
  described above were added; it was red with 29 CREDIT LOST before them.
  `queue_audit.py` exit 0 and reports four disagreeing rows. One is this class's
  own, re-derived here. The other three (`daObjFire_c`, `daObjLava_c`,
  `daSCoin_c`) are ov002 rows that main's own just-landed promotion staled; they
  belong to that change and were deliberately left alone.
  `cpp_tu_state.py` exit 0.
  `config/converted-baseline.json` was audited by SHAPE, not arithmetic, because
  it merges silently: 10 whole-file rows under the retired per-shard names out, 10
  `src/actors/daBgSnmBdy_c.cpp#…` member rows in, `count` unchanged at 2699.

- Private validation, if run, and the exact PR head/base it tested:
  the private validator was NOT run — no PR was opened, as instructed. What WAS
  run is `tools/validate_merge.py`, the same policy and schema the private worker
  calls, on a committed CI-shaped test merge built locally with `git commit-tree`
  so that its FIRST parent is the base, exactly as the validator requires. Base
  `ecc97391e7b356afbd5a1ede6f924d92559bacb5`, PR head this branch's tip, both ROM
  reports regenerated at those trees, plus the `pr_linkcheck` and `port_refcheck`
  JSON above. **Exit 0, status Passed, "Committed merge introduces no
  reconstruction or attribution regression."**
  Byte-verified functions 11,164 / 11,347 (+0); byte-verified code bytes 2,126,760
  (+0); claimed-not-byte-verified 115 functions (+0); contributor credit **0
  added, 0 changed, 0 lost**; relocation check 2052 checked, 2047 VERIFIED, 4
  BLIND, 1 DRAFT; port references 423 checked, 0 stale; module fidelity 106/106
  exact; ROM data 697 exact, 220 partial, 4 differ. The report's `rom.regression`
  field is `false`.
  Two warnings, both expected and neither a blocker: 29 address ranges left the
  byte-verified set while the enrolled totals held steady — that is what a
  consolidating promotion looks like to a gate that keys on per-function delink
  ranges — and the 4 BLIND relocation results, all in files this change does not
  touch. This is a local reproduction of the gate's policy, not the private
  worker's own run, and a fresh private validation on this candidate's actual head
  and base is still required before it lands.

### Two things a reader should not over-read

- **`rom_data_regressions` did not fire, and did not need help to not fire.** This
  change retires the coined `_ZTV11SnowmanBody` alias row from ov072
  `symbols.txt`, leaving one spelling at an unchanged address 0x021227c0. A
  name-anchored comparison reads that de-aliasing as a lost symbol. The tools on
  this tree anchor on `(module, addr, bytes)` instead, and both ROM reports were
  regenerated with them, so the gate saw the truth: 697 exact at head against 695
  at base, nothing lost. **No alias row was added to any overlay `symbols.txt`,
  and nothing was made to consult `symbols/actor_renames.tsv` to satisfy that
  gate.** The two forbidden workarounds were not needed and were not used.
- **The gate passing is not the same claim as the published chart being
  preserved, and this promotion does move the chart.** `validate_merge` resolves
  `path#symbol`, so the address-level credit model survives the fold intact — 0
  changed, 0 lost, measured. The published contributor chart is built by
  `chaos_db_ci`, which resolves overrides by WHOLE PATH and therefore never reads a
  `path#symbol` row at all. Measured base versus head with that same whole-path
  resolution, over this class only:

  | contributor | before | after | delta |
  |---|---|---|---|
  | andrewboudreau | 20 | 29 | +9 |
  | tangosdev | 9 | 0 | −9 |

  Nothing outside this class moves. This is not specific to `daBgSnmBdy_c`: every
  consolidating promotion does it, and it cannot be answered with a whole-path
  override row, because one path can only carry one author while this TU carries
  two. It is a known, separately filed defect and is deliberately not fixed here.
  State it as "the gate passed and the address-level credit model is intact",
  never as "no contributor's totals moved".

## Scope note for the coordinator and integrator

Two integration-lane files are not reserved to this task and were edited anyway,
each isolated in its own commit so it can be dropped or replayed alone.

1. `symbols/actor_renames.tsv` — 29 in-place re-spellings of existing ov072 rows
   plus 29 new rows. It conflicted on BOTH merges of `origin/main`, always for the
   same reason: it is an append log and every branch in this wave appends at the
   tail. It was resolved by replay, never by hand-merge and never by letting
   `git -M` pair the renames: main's file was taken whole, this branch's in-place
   re-spellings were re-applied to the rows they came from, and this branch's new
   rows re-appended. The two address sets are disjoint.
2. `notes/data/tu-promotion-queue.tsv` — one row. `queue_audit.py --write` was run
   and then reduced to this class's own line, so the three unrelated ov002 rows
   main staled are untouched.

Please ratify both with `classqueue.py v2 amend` on this task, or tell the
producer to drop those commits.

A log generated after this commit belongs in separately recorded evidence.
