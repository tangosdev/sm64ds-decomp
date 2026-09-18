# Handoff: manta-contract-repair-0918

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Follow-up to PR #2729, which another session merged without this repair.
- Reconstruction follow-up: https://github.com/tangosdev/sm64ds-decomp/issues/2750.
- Task: `manta-contract-repair-0918`; producer: `codex-integrator-0918`, Codex.
- The same producing session previously used `codex-review-integrate-0917`.
  Neither identity is eligible to provide independent acceptance of this repair.
- Branch: `integrate/review-queue-0918`; original accepted input/main:
  `c1b266f36554734dc19851f88e08025cf3d43a25`.
- Composed main for this candidate: `98516ad63023ea49f2485e5f2c04029ffa31e862`.
- Installed task workflow: `162d1796ff405da01b39d21ad753ab40aac71acb`;
  active source-review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Status: producer-verified narrow repair, awaiting independent acceptance.
- Next owner: an independent verifier must review the complete two methods and
  their shared declarations, compile and link-check the exact committed source,
  and decide whether the retained reconstruction work is acceptable as partial.
- Private receipts and compiler artifacts remain under ignored `build/` paths.

## What changed and why

`MantaRay::InitResources` now calls `Model::LoadFile`, `Animation::LoadFile`,
and `mModelAnim.SetFile` through their actual declarations. The removed local
declaration incorrectly gave `Animation::LoadFile` a `void` return instead of
the shared declaration's `char*`. The caller discards the result, but a matching
instruction stream did not make that conflicting contract correct.

`MantaRay::Behavior` now calls inherited `UpdatePosWithOnlySpeed` and the
collision object's `Clear` and `Update` methods directly. These replacements
remove six manually spelled mangled declarations across the two methods.
No headers, member layouts, metadata, enrollment, or attribution entries change.

ROM scope is ov090, Behavior at `0x02132c94` (420 bytes) and InitResources at
`0x02132e38` (432 bytes), ending at `0x02132fe8`. This is a source-interface
repair to existing matched methods, not a new match or a complete class recovery.

## Reconstruction dimensions and retained findings

- `PATHPTR-2729-01` (correctness): loader contract corrected using the shared
  declaration; independent reviewer must confirm the fix.
- `PATHPTR-2729-02` (reconstruction): the six unnecessary loader/model/collision
  aliases identified in these two methods are removed. The separate collision
  initialization bridge remains and is recorded below.
- `PATHPTR-2729-03` (reconstruction): Behavior still uses `C3`/`Obj` shadow types
  for its state callback. The Manta portion is tracked in issue #2750, owned by
  `codex-integrator-0918` for a separately claimed continuation. The Shark
  observation from the broader #2729 review is outside this repair and is not
  closed by it.
- `PATHPTR-2729-04` (reconstruction): raw path/node reinterpretations, vector
  overlays on actor scalars, and the manually declared collision Init bridge
  remain. The Manta portion is tracked in issue #2750 with the same next owner;
  observations on Chomp and the other #2729 files are not closed by this patch.
- `unk_37c` and `unk_380` have inferred path-ID/node-count roles. This patch
  claims no recovered original identifiers or original TU boundary.
- Lifecycle, vtable/RTTI, data and initializer ownership are unchanged. No
  compiler-barrier exemption is claimed for the remaining scaffolding.
- Original source paths and attribution remain intact. Reconstruction is partial.

## Producer proof

Pinned compiler: `2004/b56`, wired-worktree canary passed.

At original main `c1b266f36554734dc19851f88e08025cf3d43a25` plus this repair:

- `python tools/prepush_linkcheck.py --files src/_ZN8MantaRay13InitResourcesEv.cpp src/_ZN8MantaRay8BehaviorEv.cpp --json build/manta-linkcheck.json`:
  exit 0, both VERIFIED, no differing bytes or blind relocations.
- `python tools/rombuild.py -j 16 --no-rom --report-json build/manta-rombuild.json`:
  exit 0, 106/106 exact modules, 11,206 reproducing functions, zero mismatches.
  8,758 objects reused from the content cache and two compiled. Full-module
  equality does not establish complete source reconstruction or complete data
  coverage. Data report: 717 verified, 231 partial, 4 differing, 371 unnamed;
  intact-TU gates report zero new symbol errors against the baseline control.

The earlier prototype against PR #2729's original head also produced complete
objects identical to freshly compiled controls for both methods. That is
historical supporting evidence, not acceptance of this candidate or its base.

At composed main `98516ad63023ea49f2485e5f2c04029ffa31e862`, the explicit two-method
relocation check passed again. Subsequent full-build, static-gate, and exact
committed-candidate results are recorded separately in verification evidence;
do not infer a pending result from this handoff. There is no shared-header edit
or file move, so no new header consumer expansion or metadata ownership proof
is required for this narrow change. Private PR validation remains pending.


## Independent review rework

The first candidate was rejected for two additional retained return-contract
errors. `PATHPTR-2729-05` identified that `func_ov090_02132ac4` returns `int`,
not `void`; `PATHPTR-2729-06` identified the same mistake for
`ApproachLinear(short&, short, short)`. Both callers discard the return value,
which is why byte equality alone did not reveal either incorrect declaration.
This revision corrects both declarations to match their definitions. It also
restores the `char*` parameter of `func_ov090_02132b14` and the const input
pointers of the two vector-angle helpers. Fresh committed-candidate results
belong to the separate rework verification evidence. The initial review and its
findings remain in the queue history; the first candidate was not accepted.
