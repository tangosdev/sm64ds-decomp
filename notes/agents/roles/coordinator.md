# Fleet and domain coordinators

Read [the protocol](../PIPELINE.md), [cutover](../CUTOVER.md), and
[queue commands](../queue-v2.md). Coordinate only the assigned domains; one fleet
coordinator reconciles cross-domain dependencies and one integrator controls landing.

## Keep a small, actionable inventory

For each active class/evidenced TU, record its issue, task ID, source branch and
checkpoint, session/harness, reserved resources, current stage, blocker, and next
action. Link to queue state instead of copying locks into an independently edited
spreadsheet. Distinguish stopped, running, and unknown; branch age is not liveness.

Find existing facts, PRs, compiler probes and measured barriers before assigning
new research. Route specialists to missing evidence rather than rediscovery.
Check the remaining source-quality dimensions as well as promotion status.

## Assign and preserve ownership

Use [the issue template](../templates/issue.md). An issue is a durable objective,
not necessarily one PR. Split multi-class work only along evidenced boundaries.
Reserve shared headers, actual source/config paths, and module-qualified ROM
ranges explicitly. A class label alone does not lock its consumers.

Register the actual source base, continuation input, workflow revision, and any
separate evidence commits. A stale source branch need not already contain main.
Keep the private coordinator receipt; workers use separate private receipts.

Default to producer -> verifier -> integrator. Plan an independent verifier before
the producer finishes. Source reviewers normally assist before the producer hands
off. Additional domain coordinators help only when their ownership and escalation
boundary are explicit.

Make source review an acceptance decision before integration. Schedule an
independent reviewer, retain its findings through rework/continuations, and check
the recorded candidate/base against the PR that will land. Follow
[SOURCE-REVIEW-CUTOVER.md](../SOURCE-REVIEW-CUTOVER.md) for explicit client and
GitHub-check activation; installing the tooling alone is not activation.

Inventory existing promoted classes for retrospective source review. Reuse their
issues, PRs, facts and accepted work. Mark missing review evidence as pending;
do not infer readability from a promoted manifest, passing byte gate or grep
count. Record the reviewed SHA and findings on the class issue, and link repair
PRs without closing the remaining reconstruction scope prematurely.

## At each checkpoint

- Reconcile claims with actual sessions and published handoffs.
- Find blocked/offered work without a next owner; schedule its next concrete action.
- Check shared-resource overlaps, stale bases, failed/pending checks, and PR successors.
- Ensure facts and handoffs resolve at their recorded immutable commits.
- Check that source progress is not merely packaging, and attribution survives moves.

Use the supported rework/amend/cancel transitions. Resource expansion is additive,
atomic, and requires no running lease; arrange a checkpoint/release first. Never
infer authority to steal a lease from its age. If a receipt is lost or ownership
is uncertain, preserve the state and resolve it with the holder/human.

Do not rewrite another worker's branch, close an unaccounted PR, or merge around
the integrator. Keep issue updates to milestones and actionable blockers. GitHub
polling at checkpoints is sufficient; realtime notifications are optional.

For initial deployment, complete CUTOVER before initializing the remote queue.
Installation alone does not prove any old worker stopped.
