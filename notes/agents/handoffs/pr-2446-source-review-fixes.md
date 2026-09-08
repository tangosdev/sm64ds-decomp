# daBombking_c source-review continuation

Continue PR #2446 from `564d701f0c41cec7101786c0e37345cf2e5b0ddc` under
task `pr-2446-source-review-fixes`, following `bombking-ov078-0907`.
The source base is `71f7adf39168b5a4a296203012cbb21d948bd122`; the
protocol/tool revision is `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`.
Issue #2479 owns the remaining reconstruction through humanizer support
@andrewboudreau. This commit is a partial source correction awaiting a different
session's independent review. It preserves the existing promotion and credit.

## Changed source and findings

- **BKG-01:** the shadow-update helper and `decl_common.h` now agree on void.
  Its body ends after a void DropShadowRadHeight call and both tracked callers
  discard its result. The original void definition was changed to int by the
  promotion; no arbitrary return was added here.
- **BKG-02:** production still claims text only. Seven native definitions emit
  eight method symbols; 44 functions remain free, including the manual factory.
  The manifest's new checkpoint distinguishes this correction from its preserved
  historical link/ROM records. The PR description must retain this scope when
  the integrator updates its source head.
- **BKG-03:** corrected source-order, C-destructor, complete SharedFilePtr and
  reconstructed-name explanations. The manifest's own RTTI records have known
  canonical homes; their old pre-rename "homeless" explanations were false.
  The caller's retained SetAnim bridge is not justified by a callee-definition
  experiment. No new failed typed-SetAnim caller experiment is claimed.
- **BKG-04:** facts distinguish the 0x7c address-point payload from 0x84 of
  complete vtable storage, starting at 0x02126e44. Raw checker coverage remains
  partial where its symbol extent includes the 24 trailing bytes.
- **BKG-05:** removed the fabricated Base/Derived slot interface. Behavior uses
  virtual `mBlendModelAnim.UpdateVerts()` and real `Advance()`. Render uses
  explicitly qualified `mBlendModelAnim.Model::Render(0)`, preserving the ROM's
  direct base call rather than changing it to virtual dispatch.
- **BKG-06:** replaced the invented C member-pointer receiver with
  `daBombking_c::StateFunction` and the class's modeled state pointer. This
  preserves the dispatcher's current int result; it does not recover every
  handler's original signature. Raw state-table comparisons, object-manager
  access, live fields hidden by padding, matrix/vector views, other ABI calls
  and the manual factory remain explicit partial reconstruction under #2479.
  The next useful slice is named actor/player references and the shadow matrix,
  followed by exact caller-side interface probes. Retain existing bytes until
  a measured replacement succeeds.

## Proof and limits

Pinned compiler: 2004/b56; wired worktree canary passed. Every retained source
change was measured, including the real model calls and actual-class PMF receiver.

- Four changed symbols (shadow helper, SetState, Render and Behavior) each
  returned `VERIFIED`, `diffs: []`, `blind: 0` from explicit `linkcheck.py`
  commands. The final all-symbol driver rechecked all 52 manifest functions:
  **52 VERIFIED, zero blind or differing results**.
- `python tools/tubuild.py --manifest build/bkg-manifest.d verify
  ov078/daBombking_c`: **52/52 MATCH**, clean isolation/relocation destinations,
  ROM-ascending emission order. The scratch manifest protects historical proof.
- `python tools/romdata_check.py --files src/actors/daBombking_c.cpp`: five
  VERIFIED records (56 bytes), six PARTIAL records (172 equal bytes), no
  differing or unnamed records. These checks do not make the production TU own
  the class data that is still supplied from the cartridge.
- `affected_src.py include/daBombking_c.h` finds only the class TU. The shared
  declaration change also requires the independent `decl_common.h` consumer
  closure and complete production build; their terminal results belong in the
  queue evidence for this immutable commit, not in its historical records.

No function identity, enrollment, section claim, compiler-output disposition,
attribution entry or shared symbol ledger is changed. The old recovery handoff
remains historical. This handoff supersedes its current source-quality claims;
it does not assign its older measurements to this commit.

The verifier must inspect the complete class, both changed headers, facts and
manifest, carry every finding above forward, and record exact source/base SHAs
and proof limits. Byte proof alone does not accept the partial reconstruction.
