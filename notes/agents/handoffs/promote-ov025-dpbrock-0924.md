# Handoff: promote-ov025-dpbrock-0924

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3080, task
  `promote-ov025-dpbrock-0924`, stage `revise`, role producer, session
  `claude-prod-promote-ov025-dpbrock-0924`, harness Claude Code (Opus 5.5).
- Source branch `promote/promote-ov025-dpbrock-0924`. Input commit
  `787633e5e7e2a7c4e101db521958920c68ebd9f2` (origin/main at enqueue); it is
  also the original source base and the installed workflow and tool SHA.
- Six commits on top of the input: the byte-neutral class rename, the staged
  TU candidate, the promotion, the manifest evidence, the declaration
  agreement fix, and a first handoff checkpoint. This commit repoints one prose
  path and finalises this note. No separate evidence commits.
- Next action: independent verification (byte, relocation, whole-object and
  source review) of this commit, then the integrator. No blockers.
- Status: verified candidate, from the evidence below.
- Nothing uncommitted. Gate logs stayed in the producer's private worktree and
  are summarised here.

### Resolved blocker

The first checkpoint stopped because
[notes/bgobject-provenance.md](../../bgobject-provenance.md) line 760 still named
the class's old header path, and that file was outside the reservation. The
coordinator added `file:notes/bgobject-provenance.md`. This commit repoints
that one line at `include/daObjDpBrock_c.h`; the sentence is otherwise
unchanged. Line 724 of the same file keeps the old class name `PyramidStep` in
a table row, as history. `config/dead-reference-baseline.json` is not changed.

## What changed and why

- Scope: class `daObjDpBrock_c` (formerly `PyramidStep`), TU
  `ov025/daObjDpBrock_c`, ov025 .text 0x02111d40..0x021120e4, 9 functions,
  including the folded factory `daObjDpBrock_c_classInit` at 0x021120ac.
- Touched: [include/daObjDpBrock_c.h](../../../include/daObjDpBrock_c.h) (renamed from the coined header),
  ov025 [delinks.txt](../../../config/arm9/overlays/ov025/delinks.txt) and
  [symbols.txt](../../../config/arm9/overlays/ov025/symbols.txt), the new
  [manifest entry](../../../config/tu_manifest.d/ov025/daObjDpBrock_c.json), the promoted
  [src/actors/daObjDpBrock_c.cpp](../../../src/actors/daObjDpBrock_c.cpp), and these ledgers, which the
  integrator reconciles. Each edit is to this class's own rows, in place:
  `attribution.json` (9 overrides), `config/converted-baseline.json` (4
  identities re-keyed), `config/decl-agreement-baseline.json` (5 stale keys
  dropped, 2 banked), `notes/data/tu-promotion-queue.tsv` (row renamed,
  promoted cell flipped), `notes/data/class-build-worklist.tsv` (header path;
  its stale alias annotation dropped) and `symbols/actor_renames.tsv` (the
  class_rename rows).
- Retired: the six one-function member sources, the two helper sources and the
  C factory source that this TU absorbs. The manifest's `legacy_source` fields
  name each one.
- symbols.txt: the coined `_ZTV11PyramidStep` row at 0x02113914 is deleted.
  The ROM-named `_ZTV14daObjDpBrock_c` row was already present at that same
  address, and its address is unchanged. The six member symbols are renamed.
- Reverted after class_rename: its edits to `include/decl_common.h`
  (reserved by another task), `notes/data/tu-merge-candidates.json`,
  `notes/experiments/pr2874-integration-0920.json`,
  `config/tu_manifest.d/ov025/daDpLift_c.json` (another class's manifest),
  `notes/bgobject-provenance.md` and `notes/plan-tu-merge-queue.md` (outside
  the reservation at the time). Only the one path line in
  `notes/bgobject-provenance.md` was later fixed by hand, as above. `--with-derived` was not used.
- class_rename refused the rename because `_ZTV14daObjDpBrock_c` already
  existed. The harness also denied its `--allow-collision` flag. The coined
  `_ZTV11PyramidStep` row was deleted first. The tool then ran with
  `--paths src src_tu include config symbols notes
  ':(exclude)config/arm9/overlays/ov025/symbols.txt' --apply`. Its own
  mangled-name rule (`11PyramidStep` to `14daObjDpBrock_c`, inside `_Z`
  tokens only) was then applied to that one symbols.txt. The result has
  exactly one `_ZTV14daObjDpBrock_c` row and no `PyramidStep` text.
- ROM observations: build/rtti.json records `_ZTI14daObjDpBrock_c` at ov025
  0x021138d0 (kind si, name string at 0x021138dc) and names the vtable
  at 0x02113914. ov025 relocs.txt relocates every word from 0x02113910 (the
  typeinfo pointer) through 0x02113990 (slot 31, dBgActor_c::Kill). So the
  vtable is 2 header words plus 32 slots, 0x88 bytes. The next word,
  0x02113994, is `_ZTI10daDpLift_c`.
- Structural inference: func_ov025_02111e30 puts the class's own model
  (mStepModel, 0x320) on the actor position scaled by 1/8, and
  func_ov025_02111dec does the same for the collider matrix mClsnMat2 (0x374)
  and calls dBgW_KcMbg::Transform. These are the class's counterparts of
  dBgActor_c's UpdateModelPosAndRotY and UpdateClsnPosAndRot. mState 0 sinks
  and 1 rises, and each state flips after 100 frames.
- Not recovered facts: the helper roles and the state descriptions are
  inferred from the bodies. No original names are claimed for the two helpers.
  `daObjDpBrock_c_classInit` is the tree's reconstructed factory name.
- Compiler experiments, each re-verified or measured to fail: InitResources
  must read `param1 & 3` before its three initialising stores. Read inside the
  switch, 11 words differ. Every other member compiled byte-identical on its
  first clean spelling, with no int-cast field forms. The helpers' and
  func_020393d4's declarations were changed to agree with their definitions
  (char *, and (int *, int)), and the TU stayed 9/9.

## Reconstruction dimensions

- Coverage: 9/9 functions MATCH (tubuild verify), objisolate clean,
  relocation destinations clean. prepush_linkcheck VERIFIES all 9 again from
  the production file.
- Genuine methods: InitResources, Behavior, Render and CleanupResources are
  real members. D1 and D0 come from the header's inline destructor, and the
  factory is `return new daObjDpBrock_c()`. Real calls replace the legacy
  mangled externs: Model::LoadFile, ModelBase::SetFile, dBgW_Kc::LoadFile,
  dBgW::Disable, dBgW_KcMbg::Transform, SharedFilePtr::Release and
  Model::Render. ABI bridges: dBgW_KcMbg::SetFile and
  dBgActor_c::IsClsnInRange stay mangled externs (Fix12 by value, wall 6az).
  func_020393d4 is an unnamed arm9 helper. The two func_ov025 helpers stay
  `extern "C"`, taking `char *` as include/decl_common.h spells them.
- Layout: every access goes through named fields. There are no raw offsets and
  no unk_ fields. The header now types the 0x374 tail as `Matrix4x3 mClsnMat2`,
  not a u8 marker, and types `mStateTimer` as u16, since the cartridge uses only
  ldrh and strh on it. The flat C twin is gone: no C file includes the header.
  Its comment credited a C destructor shard, but that shard was already C++.
- Lifecycle and data: the inline empty destructor makes Behavior the key
  function. The TU emits `_ZTV14daObjDpBrock_c` (0x88), `_ZTI` and
  `_ZTS14daObjDpBrock_c` and the four inherited bases' RTTI records. All are
  licensed as deadstrip-data at their cartridge homes in
  `compiler_only_output`. The folded factory also materialises one homeless
  `_ZN10dBgActor_cD2Ev`, deadstripped as in daDkk_c's manifest. D1 is at
  0x02111d40 and D0 at 0x02111d8c, in ROM order, with no D2 of this class.
  The two file handles and the CLPS block are unowned ov025 rows.
- Factory: folded. The factory is the next function after InitResources, and
  the following function starts daDpLift_c's own promoted run. The same shape
  as the promoted neighbour
  [config/tu_manifest.d/ov025/daDkk_c.json](../../../config/tu_manifest.d/ov025/daDkk_c.json).
  `return new daObjDpBrock_c()` reproduces it byte for byte through a leaf
  operator new in the class header.
- Attribution: tu_promote added 9 `path#symbol` overrides carrying the base
  credit (andrewboudreau for InitResources, which was already its first
  matcher at the input commit, and tangosdev for the other eight).
  prepush_attribution reports 9 consolidated with credit intact, 0 changed and
  0 lost. No credit went to github-actions[bot].
- Remaining issue scope: member spellings for the two helpers;
  `include/decl_common.h` still declares `_ZTV11PyramidStep`, which nothing
  references now (that header is reserved by another task). The unnamed data
  rows and arm9 helper also remain.
- Deferred: `symbols/profile_reconstruction_registry.json` still names the
  retired factory source as the DP_BROCK row's `current_factory_file`. The
  field is stale now that the factory lives in
  [src/actors/daObjDpBrock_c.cpp](../../../src/actors/daObjDpBrock_c.cpp). That file is
  generated data outside this reservation, so it is not edited here; the fix
  belongs with its generator or a registry refresh. No gate reads the field.
- No rows for the old mangled names exist in `config/match_provenance.jsonl`
  or `config/match_attempts.jsonl`. None were edited.

## Proof

All runs use pinned mwccarm 2004/b56, in the producer's worktree. The source
is identical in this commit. After the byte gates ran, the last two commits
added this note, reworded one manifest `notes` sentence and repointed one
prose path in notes/bgobject-provenance.md. None of those is compiled. The
static gates below were rerun on this commit.

- Rename commit: `python tools/rombuild.py -j16 --no-rom` exit 0, 7589
  enrolled and compiled (the six renamed objects among them), 11,210
  reproducing and 0 mismatching, 106/106 modules exact.
  `python tools/prepush_linkcheck.py --range 787633e5e7..HEAD` exit 0: 1
  changed header fanning out to 7 sources, 7 checked, 7 verified, 0 blocking.
- `python tools/tubuild.py verify ov025/daObjDpBrock_c` exit 0: 9/9 MATCH,
  objisolate clean, reloc destinations clean, compiler-only exact deadstrip
  of `_ZN10dBgActor_cD2Ev`, TEXT-VERIFIED. Rerun after the declaration fix:
  still 9/9.
- `python tools/tubuild.py linkcheck ov025/daObjDpBrock_c -j16` exit 0:
  SCRATCH-LINK-VERIFIED. The .text range 0x02111d40..0x021120e4 (932 bytes)
  is IDENTICAL, 106/106 modules are exact, and dsd check symbols has 0 new
  errors. It still fails on the 9 errors the baseline control also has, so
  that check is not green. The ROM sha256 d1506e90...c478e8 is IDENTICAL to
  the stock control.
- `python tools/romdata_check.py --files src/actors/daObjDpBrock_c.cpp`:
  11 emitted data symbols, 6 VERIFIED (184 bytes, including
  `_ZTV14daObjDpBrock_c` 128 bytes and `_ZTI14daObjDpBrock_c`), 5 PARTIAL
  (the `_ZTS` strings), 0 DIFFERS.
- After promotion, `python tools/tubuild.py linkcheck --baseline --module ov025
  -j16 --clean` exit 0 (control refreshed). Then `python tools/rombuild.py
  -j16` exit 0: 7581 enrolled with 1 compiled (the promoted TU), 11,210
  reproducing and 0 mismatching, 106/106 modules exact, intact TU gates PASS
  with zero new symbol errors, ROM sha256 d1506e90...c478e8.
- `python tools/prepush_linkcheck.py --range 787633e5e7..HEAD` exit 0: 9
  checked, 9 verified, 0 blocking.
- `python tools/prepush_attribution.py --base 787633e5e7 --head HEAD` exit 0:
  9 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 787633e5e7` exit 0: no new
  declaration disagreements.
- `python tools/check_dead_references.py` exit 0: no new dead references,
  no broken markdown links. It exited 1 at the first checkpoint, before the
  prose fix.
- `python tools/tiers_ratchet.py --check` exit 1: one backslide,
  `src/actors/dScMgLuigi_c.cpp#_ZN12dScMgLuigi_c10ResetBoardEv` (No unk_
  fields). This is pre-existing: the same command at the input commit also
  exits 1 with the same single lost member (+64 gained there, +65 here). This
  TU's four banked identities are retained and not listed as lost. Not
  `--update`d. Base evidence: a scratch checkout of 787633e5e7 run with the
  same command exits 1, and `src/actors/dScMgLuigi_c.cpp` and its includes
  are unchanged by this branch. The coordinator reports that #3077 fixes it on
  main.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted` exit 0.
- `python tools/check_src_tu_compiles.py` exit 0: 271 of 271 TUs compile.
- `python tools/port_refcheck.py` exit 0: 408 checked, all resolve.
- `git diff 787633e5e7..HEAD -- include/decl_common.h` is empty.
- Private validation: not run (no PR).
