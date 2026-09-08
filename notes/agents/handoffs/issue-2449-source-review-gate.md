# Source-review enforcement handoff

Issue: [#2449](https://github.com/tangosdev/sm64ds-decomp/issues/2449).
Producer: `codex-source-review-gate-producer-20260907`.
Task: `issue-2449-source-review-gate`.
Branch: `refs/heads/tools/source-review-gate-0907`.
Worktree: `C:/tmp/sm64ds-source-review-gate-0907`.
Source base: `788643caffdfa041eb625c868a8f200542c0bd61`.
Task workflow pin: `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
The queue publication supplies the immutable final candidate SHA.

This continues the existing fleet, with `fleet-0907` retaining global coordination.
The separate humanizer repair candidate `848da2bb` and its reservations are retained.
No source/header changes, queue activation, ruleset changes or merges are part of
this tooling publication.

The queue validates source review on source-affecting verification and integration,
preserves findings/authors through rework and explicit predecessors, and requires
a separately published independent review of a changed composition. Explicit
activation upgrades remote state to 3 while retaining receipt format 2 and all
existing task history. Trusted default-branch GitHub tooling publishes Source review
for the actual PR head/base and complete relevant changed-path set.

Local validation at this checkpoint: 37 queue tests pass against disposable local
bare remotes; 15 source-review/API fixture tests pass. These include stale and
missing review, self-review, retained findings, fake composition-review assertions,
changed queue snapshots, read-only adoption and preserving existing receipts.
Independent verification and publication are recorded by the queue successor.

Next action: independently inspect the frozen candidate, rerun the queue and review
tests, inspect the trusted workflow, then publish the tooling PR. After it merges,
the existing coordinator follows [SOURCE-REVIEW-CUTOVER.md](../SOURCE-REVIEW-CUTOVER.md).
Only that explicit activation plus the required app-pinned GitHub check establishes
the live merge boundary. The scheduled check can lag a queue update; integrations
must also run the current read-only check immediately before landing.
