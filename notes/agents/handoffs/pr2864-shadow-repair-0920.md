# Handoff: pr2864-shadow-repair-0920

This document describes this commit. The queue records any later immutable output.

## Identity and ownership

- PR: https://github.com/tangosdev/sm64ds-decomp/pull/2864.
- Producer: `codex-repair-koopa2864-0920`, Codex child `/root/repair2747`.
  Coordinator/integrator: `codex-integrator-0920` (`/root`).
- Branch `fix/pr2864-shadow-0920`; own wired worktree
  `C:/tmp/sm64ds-koopa2864-0920`. The older Koopa writer worktree was not adopted,
  reclaimed or modified; its session liveness remains unknown.
- Exact input `74796d608f77ca9351bb6b2c907104edd615976c`; original parent/base
  `9022c77b3834c5ee77c31b4fb4af587418137d6c`.
- Task workflow and active Source policy pin:
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Reserved classes Koopa, KoopaSmall, daNknk_c; half-open ROM range
  ov062:0x021174cc..0x0211975c. Writes are limited to the existing shadow TU,
  this handoff, and `notes/experiments/koopa-2864-source-forms.md`.
- Status: locally verified bounded shadow-source repair; **not formal source
  acceptance and not a promotion candidate**. Coordinator reviews the exact scope
  before any queue publication; an independent verifier is the next role.

## Changes and retained findings

All six original preflight IDs remain in scope accounting:

| Finding | Disposition in this commit |
| --- | --- |
| KP2864-01, introduced field ownership defect | Fixed: mWalkState/mInvincibleTimer replace writes expressed outside mModelAnim's extent. Absolute addresses and emitted bytes are unchanged. |
| KP2864-02, inherited conflicting callee contracts | Fixed within the shadow TU: one void/two-argument animation helper; its close-actor transition caller is also void. Removed both namespace aliases and corrected the SetAnim bridge to its actual current definition. Production shards remain unchanged; caller population and shared SetAnim declaration boundary are documented in the experiment note. |
| KP2864-03, inherited reconstruction debt | Partially repaired: real ModelAnim::Render replaces the fabricated Mdl virtual class. Retained raw offsets, other shadows, volatile Render storage, manual factories, and address-named helpers have not received blanket source acceptance. One genuine SetAnim form was measured and failed; no absolute compiler-wall claim. |
| KP2864-04, inherited complete-output/order defects | Unresolved and freshly reproduced on original and final source: 13 unlicensed outputs, two missing compiler-only policies, and destructor ordinal pair (0,1). Whole-TU promotion is refused. |
| KP2864-05, inherited RTTI/class identity | Unresolved: emitted Koopa RTTI is not the retail daNknk_c identity. Shared header, symbols and consumer migration are outside this reservation. |
| KP2864-06, inherited enrollment/accounting | Unresolved: 39 legacy shards remain enrolled, shadow TU remains unenrolled, and 24 address-named functions lack symbol markers. No promotion accounting, counter, ledger, attribution or metadata policy changed. |

This is a repair atop Andrew Boudreau's original commit, which remains an ancestor.
There are no function moves or attribution overrides. Eight ordinary real methods
plus one real destructor definition (two manifest destructor variants), 27
address-named helpers and two manual factories remain the same reconstruction count.
This remains the community-named Koopa shadow. Retail RTTI is daNknk_c; the
emitted Koopa RTTI/name and destructor order are unaccepted for production. The
owned TU comments now state this ceiling, distinguish the retail vtable address
from emitted RTTI identity, and remove an obsolete one-function destructor
stripping claim. KP2864-05 is a provenance defect that must be corrected before
promotion, not an accepted deferral. Shared header and manifest remain unchanged.
The original finding kind remains provenance. No remaining false claim of verified
retail RTTI was identified in their prose: Koopa.h explicitly names daNknk_c, and
the manifest says text-verified while recording the output/order failures. The
unresolved evidence is the actual emitted identity mismatch; its review disposition
must distinguish that production prerequisite from a false historical assertion.

## Proof and coverage

- Wired worktree setup canary: PASS, pinned 2004/b56.
- `python .../build/reviewer-0919/koopa2864-probes.py`: exit 0.
  Original plus five cumulative probes compiled. The four accepted repair stages
  preserve the entire input object; the additional genuine SetAnim experiment
  yields a 0x5c helper instead of 0x50 and was not adopted.
- `python .../build/reviewer-0919/koopa2864-final-check.py`: exit 0.
  Final canonical source: **39/39 VERIFIED**, 8848 bytes, zero blind relocations,
  empty differences. Six unchanged enrolled callee/caller controls also VERIFIED.
- Complete object identity, original and final SHA256:
  `24100cde460a1703e174ef4c0ec071463ec51e669d0ca8b62625f8f16d11a041`.
  This comparison includes metadata; it does not license that metadata.
- `python tools/tubuild.py --manifest build/koopa2864-probes/manifest.json verify ov062/Koopa+KoopaSmall`:
  exit **1**, expected unresolved complete-output failures listed under KP2864-04.
  Repeated with `original-manifest.json` pointing to the unmodified input source:
  same failures. The real manifest was never written.
- Four recorded incoming calls to `func_ov062_02118058` in three production files
  all discard the result. ROM disassembly confirms no result read before return.
  The former third argument to `func_ov062_02117994` is a preceding state-store
  register; its incoming value is overwritten by the callee before use.
- Static checks: git diff --check passes; python tools/check_dead_references.py
  exits 0 with no new dead references or broken Markdown links.
- Full-ROM build: not run for this shadow-only change. The production build still
  uses the unchanged 39 legacy shards and would not validate this shadow TU.
- Shared-header consumer expansion: no headers changed. No class rename, port
  source move, or shared declaration/attribution baseline change.
- The ordinary declaration scanner explicitly excludes `src_tu`; a green changed
  range result cannot establish this TU's contracts. The targeted source/ROM audit
  above supplies the bounded evidence; other inherited declarations remain debt.
- Existing private PR validation for input 74796d6 did not compile this shadow TU;
  no new private check or formal Source review is claimed here.

Evidence: own ignored `build/koopa2864-probes/` contains original, intermediate and
final objects, strict JSON results, complete-output logs and ROM caller disassembly.
The coordinator retains the preflight and task/ownership records under ignored
`build/reviewer-0919/`. Private receipts are not committed.

Next action: coordinator checks this local bounded repair, then assigns independent
verification at the exact commit. Any promotion requires a separately reserved
continuation addressing KP2864-03 through KP2864-06, including real class identity,
header/legacy-source closure, emitted-output ownership and contributor attribution.
This handoff does not approve those unresolved items or authorize publication.
