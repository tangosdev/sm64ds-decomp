# Agent workflow

Start with [LAUNCH.md](LAUNCH.md). For an existing session or branch, use its
"Continue from the previous version" prompt.

This is the GitHub coordination protocol for Codex, Kimi K3, Claude, and other
harnesses on the same machine. [PIPELINE.md](PIPELINE.md) defines ownership and
handoffs. [CUTOVER.md](CUTOVER.md) covers stopping the old fleet and adopting its
work. [queue-v2.md](queue-v2.md) documents the executable queue.

The workflow files can be installed before cutover. Installation does not stop
old agents or activate the new queue. Do not start v2 work until CUTOVER is complete.

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
