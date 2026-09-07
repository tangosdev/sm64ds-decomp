# Handoff: prod-sanbo-0907

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: no issue; direct fleet task
  `prod-sanbo-0907`, stage `reconstruct` (role producer), harness Claude Code. One
  finding here is filed as issue #2436.
- Source branch and previous accepted input SHA: branch `cpp/daSanbo_c-tu`. There
  is no predecessor branch. The scout's facts blob was already banked on main, so
  nothing was carried over out of band.
- Original source base SHA and installed workflow/tool SHA: launched against
  `origin/main` at `a278141a9`; `origin/main` advanced to
  `26f54f8fc8faaf00568e86d33f37860a46470906` during the run and this branch was
  rebased onto it, so the merge-base is `26f54f8fc`. Workflow/tool revision
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Separate evidence commits and required artifacts in this commit: five commits,
  `origin/main..HEAD`.
  - `de0a996c6` rename to the ROM RTTI name
  - `a2b9be3f8` the promotion
  - `b9690a3f5` attribution correction the gate demanded
  - `fc037ea02` regenerate the derived state note after the rebase
  - `475a5f548` queue line-count correction

  Facts: `notes/data/class-facts/daSanbo_c.json` -- the scout's blob, moved with
  the class, prose preserved verbatim plus one added key saying so. Stage
  artifacts: `src/actors/daSanbo_c.cpp`,
  `config/tu_manifest.d/ov096/daSanbo_c.json`, `include/daSanbo_c.h`.
- Next action, responsible role and blockers: independent verification of this
  commit (verifier). No blocker. One tool limitation is described under Coverage
  limits and is not specific to this class.
- Status: verified candidate.
- Remaining uncommitted/local-only material and where it is preserved: none. The
  worktree is clean; every artifact named here is committed.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daSanbo_c` in `ov096`, the
  contiguous run `0x02135700..0x02136db0`, 36 functions, folded from 36
  one-function sources into `src/actors/daSanbo_c.cpp` with a single `complete`
  delink span.
- Reserved source/header/config surfaces actually touched: the class header,
  renamed from its coined spelling to `include/daSanbo_c.h` and given an inline
  destructor; `include/decl_common.h`, one `_ZTV` declaration renamed;
  `config/arm9/overlays/ov096/symbols.txt` and that overlay's delinks file;
  `config/tu_manifest.d/ov096/daSanbo_c.json`; `config/converted-baseline.json`;
  `attribution.json`; `symbols/actor_renames.tsv` and its report;
  `notes/actor-leaf-provenance.md`, `notes/data/class-build-worklist.tsv`,
  `notes/data/tu-promotion-queue.tsv`, `notes/cpp-tu-current-state.md`.
- ROM observations:
  - `ov096:0x02137980` holds the eleven bytes of the typeinfo string for
    `daSanbo_c`. `_ZTI9daSanbo_c` at `0x0213798c` is
    `{0x0209a764 = the __si_class_type_info vtable, 0x02137980 = that string,
    0x0208e390 = _ZTI8dActor_c}`. The vtable object begins at `0x021379d0` with a
    zero offset-to-top word and `0x0213798c`; the overlay's symbols file names the
    address point eight bytes later at `0x021379d8`. So the base is `dActor_c` and
    the cartridge's own name for the class is `daSanbo_c`, not the coined `Pokey`.
  - `_ZTV9daSanbo_c` is 31 slots. Ten are this class's, all inside the run: 0
    InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 12 OnPendingDestroy,
    16 D1, 17 D0, 18 OnYoshiTryEat, 19 OnTurnIntoEgg, 29 OnAimedAtWithEgg. No new
    virtual is declared.
  - D1 sits at `0x02135700` ABOVE D0 at `0x02135748`, and there is no D2 anywhere
    in the image. That is the layout an inline in-class destructor emits.
  - `ov096` delinks no `.data`, so the route is text-only.
  - `ov096:0x02137920` holds twelve eight-byte pointer-to-member records, running
    up to the typeinfo string. All twelve point at `func_ov096_*` addresses inside
    this run.
  - `ov096` has exactly two static initializers and two `.ctor` entries.
- Lineage evidence or structural inference: the two `ACTOR_SPAWN_TABLE` factories
  both allocate 944 bytes, both store this class's vptr and both inline the same
  constructor body; there is no out-of-line constructor in the image. SANBO and
  SANBO_BODY therefore construct the SAME class, and the head/segment split is a
  runtime test on the actor id rather than two types.
- Hypothesized names/filenames, explicitly not recovered facts: the
  `daSanbo_c_classInit_SANBO` and `daSanbo_c_classInit_SANBO_BODY` spellings are
  reconstructed source-style names in the tree's established convention. The
  cartridge proves the class, the allocation size, the vtable identity and the
  registry IDs; it does not preserve the original symbol text. Historical project
  aliases `Pokey_Spawn` and `PokeySegment_Spawn` are recorded in the file. `Pokey`
  itself is a coined English name and is now prose only.
- Compiler experiments and measured barriers:
  - Include order is load-bearing. With the class header ahead of `common.h`,
    `InitResources` scalarises its trailing whole-struct `Matrix4x3` copy and
    compiles to `0x23c` against the cartridge's `0x220`. Putting `common.h` first
    -- the order the original one-function source used -- restores the match. A
    first hypothesis blaming two `int[]`-versus-`void*` data declarations was
    tested and refuted.
  - The destructor had to move inline into the header. Out of line, mwccarm emits
    D2, D0, D1 in that order and adds a homeless D2; the cartridge has D1 then D0
    and no D2.
  - This TU owns every virtual, so it emits the vtable and RTTI group no matter
    where the destructor is declared. That is not suppressible and was not fought.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 36 of 36 functions reproduce byte
  for byte over the whole `0x02135700..0x02136db0` span, with zero blind
  relocation slots. Emission order is ROM-ascending for all 36.
- Genuine methods; remaining free-function/ABI bridges: the 10 named members are
  real C++ methods of `daSanbo_c`, and the destructor pair is compiler-emitted from
  the inline body. The 24 `func_ov096_*` members remain free functions with C
  linkage, which is what their ROM symbols are; two of them are the state
  dispatchers. Cross-TU seams are declared in one file-scope block.
- Recovered layout/fields; remaining shadow structs/raw offsets: the header carries
  the typed member list and asserts `sizeof == 0x3b0`. Shadow declarations remain
  for surfaces the tree does not model at this revision -- the render view and its
  submodel, the pointer-to-member receiver, a small linked-node view and a flat
  matrix -- each reduced to a single spelling, and each conforming to a real header
  wherever one already declares the name.
- Lifecycle, vtable/RTTI, initializer and data ownership: the two factories are in
  the TU and store the vptr as `&_ZTV9daSanbo_c[2]`, the eight-byte address-point
  bias computed by the compiler. The one-function sources wrote the bare symbol,
  which was right there because theirs was undefined and bound to the address point
  directly; here the same spelling would bind to this TU's own definition, so the
  index is required. The vtable and the whole RTTI group are emitted and then
  licensed away as `compiler_only_output`, each row naming its configured ROM home.
  The state table is deliberately NOT defined in the TU, so no second static
  initializer is emitted: the compiled object carries no `.init`, no `.ctor` and no
  `__sinit` symbol, only 37 text sections and 9 data records.
- Attribution preserved through each move/rename: 36 `path#symbol` override rows
  were added, placed beside this class's six existing historical rows rather than
  at the tail of the overrides map. Three of them had to be corrected by hand; see
  finding 3.
- Remaining agreed issue scope: none for this class.

## The two named traps

- Twelve pointer-to-member state records live in unowned `.data` and dsd resolves
  them by symbol NAME. All twelve point at `func_ov096_*` addresses that never
  carried the coined name, so the rename left every record wired. The rename did
  change ten `_ZN5Pokey*` rows in the overlay's symbols file, and those edits are
  in the SAME commit as the source rename. One coined alias row for the vtable was
  DELETED, because the ROM-named row already existed at the same address and the
  rename tool refuses that collision by design. No alias row was added anywhere,
  and nothing here consults the rename ledger to satisfy any data gate.
- The state table is not defined in the TU, so mwcc emits no second `__sinit`.
  Confirmed by inspecting the compiled object's sections directly, not inferred.

## The fold is 36, not the queue's 34

Re-derived by address rather than taken from the queue. The 36 delink entries from
`0x02135700` to `0x02136db0` are contiguous with no gap, and `0x02136db0` is where
`daTor_c`'s run begins. `tools/tu_map.py` cuts a boundary at `0x02136d10` because
it segments on symbol NAME and neither factory spelling looks like a member, so it
returns the two factories as a class-less unit of two. The cartridge disagrees on
two independent counts: both factories store this class's vptr and inline its
constructor, and the module has two static initializers for what the map calls four
TUs. `tools/queue_audit.py` agrees once the class carries its ROM name -- the row
re-derived from 34 shards / 957 lines to 36 / 1013 immediately after the rename and
before any promotion. That is the whole of issue #2436 in one measurement: the
extension over zero-gap neighbours keys on a `<class>_classInit` spelling.

## Proof

Every figure below was produced on branch `cpp/daSanbo_c-tu` in a dedicated
worktree, against merge-base `26f54f8fc`, after the rebase.

- Full-ROM build: `python tools/rombuild.py -j16`, exit 0. 106/106 modules exact,
  100.000000% of compared bytes; 11,192 source-built functions, 11,192 reproducing,
  0 mismatching; 23 source-owned data claims reproducing, 0 mismatching; ROM-build
  analysis PASS. Built ROM sha256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`,
  byte-identical to the same build at the launch base `a278141a9`. ROM data from
  source: 696 verified, 220 partial, 4 differ, 397 unnamed.
- Explicit function/consumer relocation checks: `python tools/tubuild.py verify
  ov096/daSanbo_c` -- 36/36 MATCH, objisolate clean (relocation type and addend),
  reloc-destinations clean (relocation target identity), emission order all 36
  ROM-ascending, no unlicensed symbol left unhandled. Result TEXT-VERIFIED.
- `tools/linkcheck.py` driven BY SYMBOL, not by file stem, once per manifest
  function against the promoted source: 36 VERIFIED, 0 BLIND, 0 WRONG, 0 NO-SYM.
  This is the check that resolves each relocation and compares the LINKED bytes,
  so it closes the hole a wildcarded byte compare leaves open.
- Complete emitted TU and data/metadata checks: `python tools/romdata_check.py
  --files src/actors/daSanbo_c.cpp` -- 9 data records emitted, 5 VERIFIED (168
  bytes equal to the cartridge), 4 PARTIAL, 0 DIFFERS, 0 UNNAMED. The vtable
  verifies all 124 bytes of its 31 slots against `ov096:0x021379d8`, and the
  class's own typeinfo record all 12 bytes. The 4 PARTIAL are typeinfo NAME
  strings, whose ROM extent runs to the next symbol and so includes alignment that
  belongs to nobody; that is the documented normal verdict, not a shortfall. The
  compiled object was also inspected directly: 82 sections, 37 text, 9 data, and
  no static-initializer section or symbol of any kind.
- Whole-range scratch link, run BEFORE the delinks splice: `python
  tools/tubuild.py linkcheck ov096/daSanbo_c` -- SCRATCH-LINK-VERIFIED. The whole
  span reproduces from one object with 0 differing bytes, every module is
  byte-exact, the full ROM builds and is identical to the stock build, and `dsd
  check symbols --fail` shows 0 new errors against its own baseline control. That
  check is NOT green: it has 9 pre-existing errors, all present in the control, so
  none are attributable here. The report is banked in the manifest's verification
  block.
- Static gates, all on the rebased tree:
  - `tools/premerge_check.py --base origin/main HEAD` -- merge tree computed, 8
    gates run: converted-ratchet, dead-references, duplicate-sources,
    header-offsets, langmode-ratchet, layout-check, src-tu-refs and
    source-coverage. All pass at both base and merge. RESULT: nothing goes green to
    red. Its source-coverage line records the consolidation as entries 9066 to 9031
    (-35) with bytes flat.
  - `tools/validate_merge.py --base origin/main --head HEAD` -- byte-verified
    functions 11,164 of 11,347, +0; byte-verified code bytes +0;
    claimed-not-verified +0; contributor credit 0 added, 0 changed, 0 lost. Its one
    warning is that 36 address ranges left the byte-verified set while enrolled
    totals held steady, which is the expected shape of a many-to-one consolidation.
  - `tools/tiers_ratchet.py --check` -- PASS, baseline 2699, current 2702.
    `--update` was NOT run.
  - `tools/langmode_audit.py --check` against the tracked baseline -- PASS.
  - `tools/check_rename_ledger.py` -- PASS, 1996 mangled/vtable rows agree with
    their module's symbols file.
  - `tools/check_dead_references.py` -- PASS, no new dead references, no broken
    markdown links.
  - `tools/port_refcheck.py` -- PASS, 423 references resolve.
  - `tools/eligible.py` -- 11197 of 11273, identical to the base worktree's figure.
  - `tools/cpp_tu_state.py --check` -- note current.
  - `tools/queue_audit.py --check` -- FAILS, on three rows this branch does not
    own: daObjFire_c, daSCoin_c and daObjLava_c+daObjLava_c. Those landed on
    origin/main without a queue refresh and are stale there. This branch's own row
    agrees with the tree, and the three foreign rows were deliberately left alone
    rather than carried here.
- Private validation: not run. No PR was opened and nothing was merged, as
  instructed.

## Coverage limits and findings

1. `tools/tubuild.py linkcheck` is a PRE-promotion gate for the text-only route and
   cannot be re-run afterwards. Post-promotion it refuses at step 4 of 8 with
   "intact production requires one .text claim and at least one non-text claim".
   The cause is in the tool, not in the entry: `tools/tubuild.py` admits its
   candidate into the scratch run's intact-TU map unconditionally, keyed on the
   manifest's `source` field. Before promotion that field points into the shadow
   tree, which the enrolled source list never contains, so the intact path is never
   entered. Promotion rewrites `source` to the production path, which IS enrolled,
   so the scratch run then routes the entry through `tools/tu_production.py` and
   its `prepare_intact_object`, whose first gate requires a non-text claim that a
   text-only entry can never have. The real production build never does this:
   `tools/rombuild.py` admits only entries that request
   `production_mode: intact-object`, and this entry does not. Reproduced verbatim
   on the landed, merged, text-only precedent `ov002/daObjFloatBoard_c`, so it is a
   property of the route, not of this class. Reported, not worked around.
2. The published contributor chart moves even though the gate does not.
   `tools/chaos_db_ci.py` resolves attribution overrides by WHOLE PATH, so the 36
   `path#symbol` rows this promotion adds are inert there and all 36 functions
   collapse onto one login. Measured base against head: the run's per-function
   owners go from tangosdev 31, lunavyqo 4, andrewboudreau 1 to andrewboudreau 36,
   and the totals move tangosdev 6753 to 6722, lunavyqo 380 to 376, andrewboudreau
   3272 to 3307. This is not introduced here. Tree-wide there are 2,298
   `path#symbol` override rows over 211 promoted sources; 84 of those sources name
   more than one login across 1,880 records, and every one is invisible to the
   chart. One promoted source alone collapses nine distinct logins. No landed
   promoted TU carries a whole-path override, and adding one here would have
   mis-assigned five functions rather than fixed anything, so the established shape
   was kept. The honest statement is therefore split: the gate passed with 0 added,
   0 changed and 0 lost, and the chart DID move, for a reason that predates this
   work and affects 1,880 records.
3. `tools/tu_promote.py` banked three wrong owners. It resolves each absorbed
   symbol from the source it came from, and the overrides forcing those three to
   their real owner are keyed on the PRE-RENAME coined path. Once the class took
   its ROM name those keys matched nothing, the resolver fell back to git
   authorship of the renamed file, and the promotion recorded the fallback.
   `tools/validate_merge.py` named all three and they were restored by hand in
   `b9690a3f5`. Any promotion that renames a class in the same branch should expect
   this and check for it.
4. The queue's `shard_count` remains a floor rather than a count for any class
   whose factory is spelled `<class>_classInit_<SUFFIX>` while the class still
   carries a coined name. That is issue #2436, and the measurement recorded above
   is its evidence.
