# GitHub class reconstruction protocol, version 2

Reconstruct period-accurate SM64DS C++ and prove its emitted code and relocations
against the retail ROM under the pinned compiler. Keep ROM observations, inferred
structure, coined identifiers, and hypotheses about original files distinct.
A byte match cannot recover every original identifier or prove one unique source.

This file is the operational authority for the coordinated v2 fleet. Repository
AGENTS.md still defines source and byte requirements. Old notes, harness memories,
and installed role skills must not override this workflow. Each task records the
commit containing the protocol/tool version it uses. Upgrading that version is an
explicit handoff, not a side effect of another worktree fetching main.

## One durable record for each kind of information

| Record | Authority |
|---|---|
| GitHub issue | Task objective, scope, decisions, dependencies, links |
| Git branch `agents/coordination` | Task resources, stage, current owner, accepted input/output SHAs |
| Working source branch | Source, manifests, facts and a committed handoff snapshot |
| PR and gate evidence | Review of a specific candidate and composed base |
| Optional Project board | A view of the records above |

Use one issue per active class or evidenced multi-class TU. A class may have
several independently scoped tasks over time; use unique task IDs associated with
that issue. Reserve every constituent class of a multi-class TU. Keep a class
issue open while agreed reconstruction work remains; partial PRs reference it.
Use a closing keyword only for the final agreed scope.

The issue and handoff document point to authoritative state; do not manually
maintain a second lock in labels, comments, or another TSV. GitHub account
assignment is not session identity when several harnesses share an account.
Give each run a unique session ID, record its harness, and retain its private
queue receipt in ignored local storage.

## Roles and continuous ownership

A producer owns one coherent class/TU scope through research, method and layout
reconstruction, compiler experiments, and a local verified commit. Scouts and
source reviewers assist that owner. Independent verification follows the finished
source-quality pass. A verifier does not repair the commit it is judging.

Domain coordinators choose useful scopes, locate existing facts and branches,
sequence shared dependencies, and own blocked-task follow-through. The fleet
coordinator reconciles domains and inventories the running sessions. One integrator
owns the order and composition of work landing on main.

The default task sequence is `producer -> verifier -> integrator`. A scout-only
deliverable or a focused follow-up may use fewer stages if its scope says so.
A source reviewer reports findings before handoff. Independent source acceptance
is required at verification and before integration; it is recorded against the
exact candidate and tested base, not inferred from a byte pass. The verifier may
perform this review, or a separate humanizer may own an earlier verify stage.
If a reviewer edits, make that an explicitly owned write stage and have another
session review and verify the resulting commit afterward.
Role names describe responsibilities, not preferred models.

The [source-review upgrade](SOURCE-REVIEW-CUTOVER.md) adds executable enforcement
and a required GitHub check. It preserves this v2 task protocol and receipt format;
remote state schema 3 rejects old clients after explicit coordinator activation.
Original task workflow/source pins remain historical facts. The separately recorded
`source_review_policy.workflow_commit` identifies the active review tools.

## Before editing

1. Read the issue, queue record, input commit, and accepted handoff.
2. Inspect prior branches, PRs, and local output before rediscovering the class.
   [IN-FLIGHT.md](IN-FLIGHT.md) preserves the v1 backlog investigation and inventory
   commands; recompute its dated counts before using them.
3. Declare classes, TU/source paths, shared headers, symbol/config surfaces, and
   module-qualified half-open ROM ranges. Names alone do not imply those resources.
4. Obtain a successful v2 claim for the exact offered stage and input SHA.
5. Work in your own wired worktree with private build state.

A denied claim or unavailable queue is not permission to proceed. Shared headers
are shared dependencies even when classes differ. If new resources are needed,
checkpoint the work and coordinate an atomic reservation update before editing
them. Do not broaden your scope silently. Global attribution and generated state
are reconciled in the integration lane.

Use the repository path resolver and the task's reviewed manifest. A worker does
not introduce a new directory/naming convention based on a sibling's appearance.
A deliberate convention change belongs to a separate reviewed decision.

## Handoff and resumption

Use [the handoff template](templates/handoff.md). Commit it with the source it
describes. Refer to the candidate as "this commit" inside the document to avoid
trying to embed its own SHA; the queue records the exact output SHA.

Before publication, read current queue ownership again. Publish the immutable
output through `classqueue.py v2`; the next stage must accept that exact input.
Publishing a queue artifact sends its commit history to the remote. Inspect the
committed files first, and never include private receipts, compiler inputs, or ROMs.

The task retains its resources while offered to the next stage. Releasing a
running lease relinquishes that attempt, not the durable task or its evidence.
The previous output remains discoverable if a harness stops or misses a notification.
Do not delete a source branch holding unbanked work.

For an interruption, preserve the branch, committed checkpoint, private receipt,
and exact next action. If a checkpoint does not pass the source gates, label it
WIP and keep it out of source integration. "Continue" means inspect and resume
that checkpoint; it never means reset, clean, abandon the branch, or claim a
different class automatically.

For verification failure, record evidence and use the queue's rework transition
back to the producer. Never publish a failed verification as a successful handoff.
A corrected commit receives a fresh independent verification.
For a verify-only adopted task with no producer stage, preserve the failure on
the issue, release, and have the coordinator cancel and register an explicit
producer continuation as described in queue-v2.

## Communication without realtime delivery

Read shared state at startup/resume, before scope changes, and before handoff or
publication. Coordinators check their assigned tasks at regular checkpoints; a
small fleet can start with a few-minute polling interval and API backoff. Workers
doing a long build report its running state at their next available checkpoint.

Record milestone updates and actionable blockers on the issue. A blocker names
the needed fact/action, its responsible role, and the exact artifact to inspect.
No per-command issue chatter or API heartbeat spam is required. No elapsed-time
threshold authorizes stealing a claim. A stopped session needs an explicit,
evidence-preserving recovery.

## Source quality and proof are separate

Each handoff records these dimensions without combining them into one score:

- Exact functions/bytes and relocation destinations, with incomplete coverage named.
- Compiler-spelled methods versus surviving free-function/ABI bridges.
- Fields and object layout recovered versus raw-offset or shadow-struct accesses.
- Lifecycle, key-function, vtable, RTTI, initializer and data ownership.
- Remaining naming, scope, and measured compiler barriers.
- Attribution carried through file moves and symbol renames.

"Promoted" is a packaging/build state, not a declaration that reconstruction is
complete. Keep the next concrete improvement on the class issue. Original helper
functions need not become methods merely to increase a count.

Each source finding has a stable ID, location, kind and disposition. Correctness
and provenance findings must be fixed. A reconstruction finding may retain a
measured compiler constraint with pinned experiment artifacts, or be accepted as
deferred work with an issue, owner and a `partial` completion assessment. Rework
must carry every previous finding forward; it cannot make a blocker disappear by
renaming its kind or omitting it. Continuations name `predecessor_tasks` so the queue
preserves findings and contributing sessions across task boundaries.

Required proof is defined by AGENTS.md, the task's change scope, and current gate
tools. Source/header edits require exact affected-consumer and full-ROM checks;
TU/lifecycle work also requires complete emitted-object and metadata evidence.
Treat queued validation, masked byte matches, unexplained blind relocations, and
report-only success as incomplete. Record the exact tested source and base SHAs.

## Integration and completion

Only the integrator composes independently accepted candidates, reconciles shared
bookkeeping, publishes the final PR, and coordinates its landing. Small coherent
batches may be useful; compatibility follows actual shared resources and dependencies.
Use the current private validator and relevant static gates on the proposed composition.
A previous green head/base is not proof for a changed composition.

A different composition commit needs a separately published independent review
of that commit and base. Use a verify-only review task that reserves its own
report path, reads the source via `requires`, names the integrator as
`input_session`, and inherits the original task with `predecessor_tasks`.
The source reservation stays with its existing owner. The integrator records
`composition_review_task`; an embedded report with an invented session label is
not an independently published review. Refresh the GitHub check after review or
rework and recheck live review state immediately before landing.

A queue's completed stages do not themselves prove a merge. The integrator records
the PR URL, tested candidate, tested base, and resulting main commit in its evidence.
A squash merge's SHA differs from the candidate SHA; preserve both identities.

Close superseded PRs after their content is accounted for, preserve contributor
attribution, and leave no unresolved output without an issue and next owner.
Tools/CI/protocol work remains separate from source reconstruction PRs.

See [CUTOVER.md](CUTOVER.md) before starting v2 and [queue-v2.md](queue-v2.md)
for the actual commands, state transitions, and pilot limitations.
