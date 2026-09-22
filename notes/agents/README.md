# Agent workflow

Start with [LAUNCH.md](LAUNCH.md). For an existing session or branch, use its
"Continue from the previous version" prompt.

This is the GitHub coordination protocol for Codex, Kimi K3, Claude, and other
harnesses on the same machine. [PIPELINE.md](PIPELINE.md) defines ownership and
handoffs. [CUTOVER.md](CUTOVER.md) covers stopping the old fleet and adopting its
work. [queue-v2.md](queue-v2.md) documents the executable queue.

For an unadopted legacy fleet, follow CUTOVER before starting v2 work. For normal
resumption, inspect the live queue and resume the recorded task; do not rerun
cutover or initialize a second queue.

The 2026-09-07 cutover is done: [CUTOVER-0907.md](CUTOVER-0907.md) records the
pinned workflow commit, what the branch inventory found, and what is deliberately
not adopted yet. Read it before assuming a branch is abandoned.

For TU assignments, [the promotion workflow](../tu-promotion-conventions.md)
defines the production result and the limited reasons to retain a shadow. Folder
regrouping and local shadow commits are not completed promotions. Technical
runbooks supply experiments, not additional mandatory staging phases.

The repository documents are the workflow authorities: [AGENTS.md](../../AGENTS.md)
for repository rules, [PIPELINE.md](PIPELINE.md) for coordination, and
[TU promotion](../tu-promotion-conventions.md) for production completion. The
former class/TU/review skill wrappers are retired; no installed coordinator skill
is needed. Compiler and review observations remain ordinary technical references.
Keep using the [worktree automation](../worktree-inputs.md) for private inputs.

## Read only what your task needs

- Everyone: PIPELINE, then their current task and accepted handoff.
- Fleet/domain coordinator: [coordinator](roles/coordinator.md).
- Continuous class/TU owner: [producer](roles/producer.md).
- Evidence specialist: [scout](roles/scout.md).
- Source plausibility review: [humanizer](roles/humanizer.md).
- Independent byte verification: [verifier](roles/verifier.md).
- Final composition and publication: [integrator](roles/integrator.md).

Old writer, builder, and reviewer launch names remain redirects. Historical
measurements are in [references](references/README.md); load a relevant passage,
not the entire archive. Operational instructions in that archive are superseded.

Templates: [issue](templates/issue.md), [handoff](templates/handoff.md),
[verification evidence](templates/verification.json). Replace every placeholder
and use actual repository paths and immutable commits.
