# Handoff: issue-2414

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- **Issue URL, task ID, stage, session and harness:**
  https://github.com/tangosdev/sm64ds-decomp/issues/2414 — task `issue-2414`,
  stage `reconstruct` (role `producer`), session `prod-2414-0907`, harness
  Claude Code (Opus 5). Claimed against the exact SHA the queue offered.
- **Source branch and previous accepted input SHA:** branch `cpp/daMip_c-v2`,
  branched at `f10902c14b91ceb4618062c119c3075852292519` — the accepted input,
  and the tip of the v1 branch `cpp/Rabbit-tu`. Nothing was rebased, reset or
  restarted; the v1 branch is untouched and still holds that tip.
- **Original source base SHA and installed workflow/tool SHA:** both
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- **Separate evidence commits and required artifacts in this commit:** the task
  pins no separate evidence commits. The scout facts for this class are
  committed on this branch at `notes/data/class-facts/Rabbit.json` (still under
  the retired coined name — see remaining work). The stage's required artifacts,
  `src/actors/daMip_c.cpp` and `config/tu_manifest.d/ov085/daMip_c.json`, are
  both present.
- **Next action, responsible role and blockers:** independent byte verification
  of this commit by a session that is not `prod-2414-0907` and is not the v1
  producer recorded in this branch's history. No blocker inside this scope. Two
  items are deliberately left for the integration lane, both listed below.
- **Status:** verified candidate for the checks a producer can run. The full-ROM
  build, the TU byte/relocation pass and the composed static gates all pass at
  this content; the private validator has not been run and no PR exists yet.
- **Remaining uncommitted/local-only material and where it is preserved:** none.
  Working tree clean. The private queue receipt and every build log stay in the
  worktree's gitignored `build/` and are not committed.

## What changed and why

- **Class/TU/symbol and module-qualified ROM scope:** `daMip_c` in ov085.
  `.text` `0x0212a6d4..0x0212cc88`, 32 functions, one translation unit at
  `src/actors/daMip_c.cpp`. Class data in ov085 at `0x021300bc` (typeinfo name),
  `0x021300c8` (`__si_class_type_info`) and `0x021300f8` (31-slot vtable address
  point).
- **Reserved source/header/config surfaces actually touched by *this* stage:**
  `src/actors/daMip_c.cpp` and `include/daMip_c.h`. Nothing else. The reserved
  ov085 `symbols.txt`/`delinks.txt`, the TU manifest and the retired coined
  header were already in their final state at the accepted input.
- **ROM observations:** unchanged from the accepted input and independently
  re-measured here, not re-asserted: 32 of 32 functions reproduce the cartridge
  byte for byte with correct relocation destinations, and the class's own
  vtable and typeinfo record compare equal to the ROM.
- **Lineage evidence or structural inference:** the class name is read from the
  image, not coined. Member-ness is ROM-proven for `SetState` and the sixteen
  `State*` functions, each of which is the target of an eight-byte
  pointer-to-member-function constant in `0x0213003c..0x021300bc`.
- **Hypothesized names/filenames, explicitly not recovered facts:** the file
  name `src/actors/daMip_c.cpp`; the factory spelling `daMip_c_classInit`; the
  six coined helper names `TestWaterBelow`, `UpdateGrab`,
  `UpdateMatrixAndShadow`, `UpdateCarriedMatrix`, `UpdateMirrorShadow` and
  `RenderMirrorImage`, whose member-versus-file-local form the cartridge does
  not distinguish; the field names in `include/daMip_c.h`.
- **Compiler experiments and measured barriers:** one change was made at this
  stage, and it was made because a gate that is green on the base goes red on
  the merge result, not for style. `tools/premerge_check.py` reported the
  langmode ratchet failing with `codegen_hacks.extern_vtable` 165 -> 166 and
  `extern_vtable_classes` 168 -> 169. `tools/langmode_audit.py` counts a file
  under `src/` that spells `extern ... _ZTV...;` itself, because that idiom
  normally marks a vptr stamped by hand for a class with no vtable at all.
  daMip_c is the opposite case — this TU owns the key function, the compiler
  emits the real table, and it compares equal to the cartridge — so what the
  gate caught was the declaration site. The declaration moved to
  `include/daMip_c.h` beside the class, which is what the landed sibling
  `include/Scuttlebug.h` does for the identical factory seam, and the type
  changed to `int[]` so it agrees with the spelling `include/decl_common.h`
  already carries. The counts return to 165/168 and the bytes do not move: the
  TU still verifies 32/32.
  The hand-written store itself stays. A natural `new daMip_c()` would select
  the global `operator new`, which this image does not contain, and `fBase_c`
  cannot declare its own `operator new` in-class; `include/fBase_c.h` records
  that measurement and `src/actors/Scuttlebug.cpp` retains the same seam for the
  same reason.

## Reconstruction dimensions

- **Exact function/byte and relocation coverage:** 32/32 functions byte-exact,
  relocation-aware, with the emitted object's relocation types and addends
  checked as well; all 32 emit in ROM-ascending section order. Coverage is the
  whole licensed run and nothing in it is excluded or masked.
- **Genuine methods; remaining free-function/ABI bridges:** 31 of the 32 are
  real `daMip_c::` member functions. The one remaining free function is
  `daMip_c_classInit`, and that is correct rather than debt: the cartridge
  carries it unmangled as the registry factory. Inside it, seven calls stay
  spelled as mangled `extern "C"` declarations — `fBase_c::operator new`, the
  `dEnemyBase_c` base constructor and five member constructors — because the
  cartridge folded the constructor into the factory and emitted no
  `_ZN7daMip_cC1Ev`/`C2Ev` of its own.
- **Recovered layout/fields; remaining shadow structs/raw offsets:** the class
  is a real `daMip_c : dEnemyBase_c` and the five sub-objects at `0x110`,
  `0x144`, `0x300`, `0x368` and `0x3c0` are typed and close on each other; the
  `0x474` size assert is the ROM's own allocation literal. Still unrecovered:
  seven padding spans (`pad_390`, `pad_3e8`, `pad_424`, `pad_42b`, `pad_44c`,
  `pad_460`, `pad_46c`); `mState` at `0x364`, which is really a pointer to a
  state record but is kept an opaque `s32` because `Behavior` compares it by
  address against four ov085 objects; the `daMip_cSelf` shadow struct the
  pointer-to-member call goes through; a `V3Blk { s32 w[3]; }` block in
  `StateFleeMain`, which is load-bearing — the scalar spelling costs eight
  bytes; and the ov085 file statics, which stay block-scope inside the member
  that recovered each one, because the legacy shards disagreed about their
  types and canonicalising them is a measured codegen hazard rather than a
  tidy-up.
- **Lifecycle, vtable/RTTI, initializer and data ownership:** `~daMip_c` is
  declared first among the virtuals and defined out of line, so it is the key
  function and this TU emits `_ZTV7daMip_c`, `_ZTI7daMip_c` and `_ZTS7daMip_c`.
  `tools/romdata_check.py` reports 11 emitted data records for this source: 6
  VERIFIED (180 bytes equal to the cartridge, including this class's own vtable
  and typeinfo record and the four ancestor typeinfo records), 5 PARTIAL, and
  **0 DIFFERS**. The PARTIAL rows are typeinfo-name strings whose emitted
  coverage is shorter than the ROM extent the config names; they are not
  disagreements. Two compiler-only symbols deadstrip exactly and are recorded in
  the manifest: `_ZN7daMip_cD2Ev` (the cartridge runs D1 straight into D0 with
  no third variant) and `_ZN7Vector3D1Ev`. The TU claims `.text` only; it owns
  no `.data` or `.bss`.
- **Attribution preserved through each move/rename:** every one of the 32
  functions carries its originating shard path in the manifest's per-function
  `legacy_source` field, and each member carries an `// @symbol` marker. The
  ledger rows recording the retired coined spelling were written at the accepted
  input; `tools/check_rename_ledger.py` passes over 1932 mangled/vtable rows.
- **Remaining agreed issue scope:** the padding spans and `mState`'s real type
  are the next concrete reconstruction improvements and belong on the issue,
  not to this stage.

## Proof

Every command below was run in this worktree at exactly this source content,
against base `origin/main` `88dbe66db2cb3f0cd1dc704e9f2775eb37aea646`.

- **Full-ROM build:** `python tools/rombuild.py -j16 --no-rom` — exit 0. 11,191
  of 11,191 source-built functions reproducing, 0 mismatching; module fidelity
  106/106 exact at 100.000000% of compared bytes; intact-TU gates report dsd
  modules PASS, zero new symbol errors and exact storage aliases. `--no-rom`
  means the linked modules were compared but no `.nds` image was assembled, so
  no ROM checksum is claimed here. The run's own baseline control reports a
  pre-existing `dsd check symbols --fail` failure with no TU substituted, which
  belongs to the tree and not to this change.
- **Explicit function/consumer relocation checks:**
  `python tools/prepush_linkcheck.py --range origin/main..HEAD` — exit 0. The
  three changed headers fan out to 932 sources; 929 checked, 632 verified, 297
  warnings, **0 blocking**, 3 NONMATCHING drafts skipped. **Coverage limit,
  named:** this tool derives one symbol from each file's stem, so a promoted
  multi-symbol TU cannot be checked by it — `daMip_c` appears in that run as
  `NO-SYM`. The relocation destinations of the 32 functions are covered instead
  by the objisolate pass below and by the full-ROM link above, not by this tool.
- **Complete emitted TU and data/metadata checks:**
  `python tools/tubuild.py verify ov085/daMip_c` — exit 0: 32/32 MATCH,
  objisolate clean (relocation type and addend), reloc-destinations clean,
  emission order all 32 in ROM-ascending section order, compiler-only symbols an
  exact deadstrip — TEXT-VERIFIED.
  `python tools/romdata_check.py --files src/actors/daMip_c.cpp` — exit 0: 6
  VERIFIED / 5 PARTIAL / 0 DIFFERS / 0 UNNAMED, as detailed above.
  **Not available, and why:** `python tools/tubuild.py linkcheck ov085/daMip_c`
  exits 1 and does not run — `intact TU preparation refused: ov085/daMip_c:
  intact production requires one .text claim and at least one non-text claim`.
  This TU claims `.text` only. That refusal was recorded at the accepted input
  and is reproduced here rather than taken on trust.
- **Shared-header consumer expansion:**
  `python tools/affected_src.py include/daMip_c.h` reports exactly one consumer,
  `src/actors/daMip_c.cpp`; the only other file naming that header names it in
  prose. The header edit is therefore fully covered by the TU verification and
  by the full-ROM build, and the 932-source fan-out above is the wider expansion
  the branch's other header changes carry.
- **Port/path/reference and other applicable static gates:**
  `python tools/premerge_check.py HEAD --base origin/main` — exit 0, and this is
  the check that matters most here because it evaluates the **merge result**:
  converted-ratchet, dead-references, duplicate-sources, header-offsets,
  langmode-ratchet, layout-check, src-tu-refs and source-coverage are all `ok`,
  nothing goes green to red. (On the accepted input the same command exited 1
  with langmode-ratchet as a REGRESSION; that is what this stage fixed.)
  Also run, each exit 0: `python tools/port_refcheck.py` (402 references, all
  resolve), `python tools/check_rename_ledger.py`,
  `python tools/check_tubuild_conflicts.py` (154 manifest entries; every
  recorded conflict matches its source and vice versa),
  `python tools/check_profile_campaign.py` (786 live claims, 0 diverge),
  `python tools/check_python_names.py`, `python tools/check_dead_references.py`
  (no new dead references, no broken links) and
  `python tools/cpp_tu_state.py --check-note`.
- **Private validation, if run, and the exact PR head/base it tested:** not run.
  No PR is open for this branch. Nothing above should be read as a validator
  pass, and the terminal private validation the issue requires for acceptance
  has not happened.

### Left to the integration lane, deliberately

Neither is a defect in this candidate and neither is reconciled here, because
both are generated bookkeeping shared by every branch in this wave and neither
is reserved to this task.

1. `python tools/queue_audit.py --check` exits 1 on one row —
   `total_lines` for the daMip_c row, 1890 against a tree that now holds 1891,
   the net one line this stage's comment added. Regenerating it rewrites a
   shared generated TSV outside this task's reservation, so it was left alone.
   No workflow under `.github/workflows/` runs this gate.
2. The branch already carried edits to `include/decl_common.h`,
   `symbols/actor_renames.tsv`, `config/converted-baseline.json` and
   `config/converted-backslide-exceptions.jsonl` at the accepted input. This
   stage did not touch any of them.

Separately, `notes/data/class-facts/Rabbit.json` still carries the retired
coined name in its path. Renaming it needs a reservation this task does not
hold; it is recorded here so it is not lost.

A log generated after this commit belongs in separately recorded evidence.
