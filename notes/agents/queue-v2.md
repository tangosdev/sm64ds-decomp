# Queue v2 pilot: durable ownership and handoffs

This is an opt-in Git protocol, invoked as `python tools/classqueue.py v2 ...`.
Installing it does not change the active v1 fleet. The current default role sequence
and integration policy are in [PIPELINE.md](PIPELINE.md); this tool supports explicit
per-task stages.
GitHub issues can supply the human task identity (`issue-2400`) and discussion;
the queue records exact ownership, resources, input commits and handoffs. No
realtime messaging service is needed: agents poll `next` and `list` between tasks.

The [source-review upgrade](SOURCE-REVIEW-CUTOVER.md) adds state schema 3 while
keeping v2 commands and receipt schema 2. It is explicitly activated by the fleet
coordinator after checkpointing/upgrading clients. Original task workflow pins,
leases, outputs and history are preserved; old clients reject the upgraded state.

## Cutover boundary

**Do not run v1 and v2 agents concurrently.** Before initialization, stop or upgrade
all legacy launchers, reconcile their outputs, and have the actual holders release
their v1 claims. Inventory the independent `tools/claims.py` range-service work
as well and assign those agents disjoint work or stop them for the pilot.

V2 refuses operations while *any* `refs/claims/*` ref is visible. This is a diagnostic
guard, not mutual exclusion against old clients: an old client can create a claim
after this check, and v1 does not consult v2. Tests explicitly demonstrate this
boundary. Updated v1 `next`/`claim` refuse acquisition once the v2 ref exists;
legacy `list`/`status`/`release` remain available for draining. Truly old binaries
remain vulnerable until cutover. Merely
installing the new tool does not upgrade already running worktrees or sessions.

No claims are automatically imported, deleted, expired, or stolen. This change
does not unify the separate range service. It checks range overlap only among
resources explicitly registered in v2, and cannot infer aliases between a class,
its TU, a shared header, and a ROM range. Declare all relevant resources.

After that operational cutover, initialize once:

```powershell
python tools/classqueue.py v2 init --session fleet-0907 --legacy-clients-stopped-and-upgraded
```

The command writes `refs/heads/agents/coordination` on `origin`. It is a metadata branch
whose tree contains `state.json`, not a source branch to merge or check out. Every
update compares the remote ref to its exact previously observed SHA; a failed or
uncertain push is an error. Do not force-reset/delete this branch. The transport
must permit Git's normal fast-forward updates and explicit creation leases.

Test commands use disposable local bare remotes. For a local-only pilot,
`v2 --remote <configured-local-remote> ...` uses the same protocol, but all agents
must use that one remote, and the state will not be on GitHub.

## Explicit task specification

Save a JSON specification outside tracked source (for example under `build/`):

```json
{
  "task_id": "issue-2400",
  "issue_url": "https://github.com/tangosdev/sm64ds-decomp/issues/2400",
  "coordinator": "fleet-0907",
  "workflow_commit": "FULL_WORKFLOW_COMMIT_SHA",
  "resources": [
    "class:ov006/dExample_c+dExampleChild_c",
    "tu:REPLACE_WITH_SOURCE_PATH",
    "file:REPLACE_WITH_HEADER_PATH",
    "range:ov006:0x020c0000:0x020c0100"
  ],
  "base_commit": "FULL_BASE_COMMIT_SHA",
  "input_commit": "FULL_CONTINUATION_COMMIT_SHA",
  "evidence_inputs": [],
  "next_action": "Read the pinned facts and reconstruct the class",
  "stages": [
    {
      "id": "reconstruct",
      "role": "producer",
      "mode": "write",
      "requires": ["REPLACE_WITH_FACTS_PATH"],
      "produces": ["REPLACE_WITH_SOURCE_PATH"]
    },
    {
      "id": "verify",
      "role": "verifier",
      "mode": "verify",
      "requires": ["REPLACE_WITH_SOURCE_PATH"],
      "produces": []
    },
    {
      "id": "integrate",
      "role": "integrator",
      "mode": "verify",
      "requires": ["REPLACE_WITH_SOURCE_PATH"],
      "produces": []
    }
  ]
}
```

Names, issue number, paths, range and SHA above are examples, not recovered project
facts. Resolve source paths with `srcpath.py`. Use a real full commit SHA and paths
that exist at that commit; no symbolic `HEAD` or mutable branch inputs are accepted.
The initial stage's requirements must exist in `input_commit`, which defaults to
`base_commit` for new work and must descend from it.
`workflow_commit` pins the instructions/tools revision used for this task, which
can differ from the older source base being continued.

### Continue from the previous version

After the old agents have stopped, keep their worktrees, branches and commits.
Use the original pinned main SHA as `base_commit`, and the existing source tip as
`input_commit`. Choose stages for the **remaining** work. For example, an already
written TU can begin with a sustained producer cleanup stage or an independent
verification stage. Do not represent unverified v1 work as a completed v2 stage.
For a verify-first task, set `input_session` to the prior source producer's unique
session identity; the queue denies that session a verification lease. Prefer a
producer stage followed by verification when further source work may be needed.

If scout facts live on another branch, pin them separately without changing or
merging source just to import metadata:

```json
"evidence_inputs": [
  {
    "kind": "scout-facts",
    "commit": "FULL_SCOUT_COMMIT_SHA",
    "paths": ["REPLACE_WITH_FACTS_PATH"]
  }
]
```

Fetch those exact commits into the coordinator's repository before enqueueing.
The queue checks the commits and paths, retains them in its Git ancestry, and
includes them in `next` results. Stage `requires` still refers to the source input
commit; separately pinned facts belong in `evidence_inputs`. Producers should read
the listed revision, including what it marks unproven, before continuing. Optional
fields such as `previous_branch` and `previous_session` are preserved in the task
specification for the coordinator's audit trail.

Enqueue is adoption of explicit metadata, not proof that old agents stopped. It
does not delete their claims or change their branches. The cutover precondition
above still applies, including reconciling work in the independent range service.

Stages are configured per task. A producer can own one stage throughout scouting,
writing and local verification, or the coordinator can configure separate scout,
writer and follow-up stages. Role names may repeat; stage IDs must be unique.
`verify` requires a different session from the source-producing write stage (or the
adopted `input_session`), pins the same
source commit, and requires an evidence object. It does not run or interpret byte
gates itself. Final stage completion means **this task's stages completed**, not
that its PR passed CI, merged, or reached main. Integration policy stays separate.
For an integration stage, verify the accepted candidate SHA and put the resulting
`merged_main_sha` in evidence separately. A squash merge is not required to descend
from the candidate commit; never pass a squash SHA as the verify stage's output.

An explicit follow-up task works for an already promoted class: v2 does not filter
the promotion TSV. Reuse the resource identifiers and choose a new task/issue ID.

```powershell
python tools/classqueue.py v2 enqueue build/issue-2400.json --request-id enqueue-issue-2400 --receipt build/issue-2400-coordinator-receipt.json
python tools/classqueue.py v2 next --role producer
```

Enqueue atomically reserves every declared resource for the task. Classes separated
by `+` are individual resources; overlay-qualified class names normalize to bare
class names. TU and file paths refer to the same resource and compare without case
to match the Windows workspace. Ranges are `[start, end)` and compare only within
the same module. Overlapping tasks are denied across all roles, even when the
current task is awaiting handoff or its running lease has been released.
The coordinator receipt is saved before enqueueing and is separate from a worker's
claim receipt. Preserve it: only that credential can amend or cancel an idle task.

## Accept, publish, and resume

Copy the exact stage ID and input SHA from `next`; inspect that commit before
claiming. Each harness run chooses a unique session ID. The receipt is a private
local credential saved **before** the push, so a connection failure cannot strand
the only token. Keep it under the worktree's ignored `build/` directory and never
commit or paste it into an issue, PR, shared log or report. Unix permissions are
restricted on creation; Windows privacy depends on the directory's ACL.

```powershell
python tools/classqueue.py v2 claim issue-2400 --stage reconstruct --role producer --session codex-run-0907-a --input-commit FULL_INPUT_SHA --worktree C:/tmp/sm64ds-example --receipt build/issue-2400-producer-receipt.json
```

Only the receipt's exact lease ID, session ID and secret token can modify a running
lease through this client. The remote stores a token hash, never the token. This
is cooperative fencing, not security against someone with Git write access who
edits the state manually or assumes another session identity.

Commit the output, then publish its immutable SHA with the next action:

```powershell
python tools/classqueue.py v2 publish --receipt build/issue-2400-producer-receipt.json --output-commit FULL_OUTPUT_SHA --request-id publish-issue-2400-reconstruct --next-action "Run relocation-aware and whole-TU verification"
python tools/classqueue.py v2 next --role verifier
```

The command verifies required artifact paths and commit ancestry, then stores the
output and offers the next stage atomically. It **pushes the referenced commit and
its Git ancestors** as parents of the state commit. Therefore artifacts remain
fetchable after a source branch is deleted; do not publish commits containing
private files. Normal source-branch publication remains a separate workflow step.

An offered handoff retains the task's resource reservation. Its producer can no
longer publish or release that lease. The successor claims the exact offered stage
and input with a fresh private receipt; this records `accepted_by`/`accepted_at`
and fences the predecessor. A missed notification cannot erase the offer.

For a verify stage, publish the **same** input SHA with `--evidence build/gates.json`.
That object must include `"verdict": "pass"` and `"tested_commit": "FULL_INPUT_SHA"`.
It can also hold gate commands, tested source/base SHAs and
links to reports. Evidence is retained in state; the queue does not certify its
truth or replace the independent private validator.

After source-review activation, source-affecting verify stages additionally
require the populated `source_review` from the verification template, an exact
`tested_base`, `reviewer_session` equal to the lease holder, and `workflow_commit`
equal to the active review-policy pin. Every source write must lead to independent
verification. Integrator claims reject an older byte-only predecessor pass.

For continuations, `predecessor_tasks` lists existing task IDs. Enqueue retains
their findings and contributing sessions as immutable inherited context. Optional
`producer_sessions` supplies additional known authors of adopted source; do not
omit an earlier writer merely because a different session last touched the branch.
No contributing writer may verify their own work after another writer intervenes.

An integrator records `composition_commit`, `composition_base`, and
`composition_review_task` for a changed composition. That task must inherit the
original task and publish its own independent verify output for the composition.
It reserves its report artifact, with source files declared as read-only
`requires`; it does not acquire the existing source reservation. The composition
must preserve the accepted source and declared base as ancestors.

Refresh the external check after a review decision:

```powershell
gh workflow run source-review.yml --repo tangosdev/sm64ds-decomp -f pr=2447
python tools/check_pr_source_review.py --pr 2447
```

Use the actual PR number. The second command reads current state and exits
nonzero on missing/stale review; it does not publish or merge anything.

### Failed verification and additional dependencies

Failed or blocked evidence cannot complete a verify stage. Return it to the most
recent producer stage with an evidence object containing `verdict: "fail"` or
`"blocked"`, the exact `tested_commit`, and the actual failure details:

```powershell
python tools/classqueue.py v2 rework --receipt build/issue-2400-verifier-receipt.json --request-id rework-issue-2400-attempt1 --evidence build/failure.json --next-action "Repair the named WRONG-DEST and reverify"
```

The rejected outputs and evidence are archived, all resources stay reserved, and
`next --role producer` offers the rejected source commit for revision. Original
input artifacts remain pinned even when promotion removed their paths. The new
output must descend from that rejected source and pass a fresh independent verify
stage. A verifier cannot use an old receipt to pass the revised commit.

If a producer discovers a shared dependency outside the reservation, pause the
edit and release its running lease. The coordinator adds resources atomically:

```powershell
python tools/classqueue.py v2 amend --receipt build/issue-2400-coordinator-receipt.json --request-id reserve-issue-2400-header --resource file:REPLACE_WITH_SHARED_HEADER_PATH --reason "Reserve dependency before continuing"
```

Amend only adds resources; another task's overlapping claim denies the entire
update. It cannot alter a running lease or silently reduce an existing reservation.
Resume with a new worker receipt after inspecting the updated task.

To supersede or abandon a task, first have its holder release the running lease.
Then the coordinator can cancel it, preserving history and freeing its resources:

```powershell
python tools/classqueue.py v2 cancel --receipt build/issue-2400-coordinator-receipt.json --request-id cancel-issue-2400 --reason "Superseded by issue-2401; continue from the recorded source and facts"
```

A verify-only adopted task has no producer stage to return to. On failure, record
the failure on its issue, release, and have the coordinator cancel and enqueue a
new explicit continuation with producer and verifier stages using the same source
commit and pinned evidence. This recovery changes no source branch and leaks no
resource reservation. Stage graphs are not edited in place.

If interrupted before producing an output, relinquish the running lease:

```powershell
python tools/classqueue.py v2 release --receipt build/issue-2400-producer-receipt.json --request-id release-issue-2400-attempt1 --reason "Resume reconstruction from the recorded input"
```

Release makes the same stage ready again and retains predecessor outputs and the
task's reservation. It cannot delete a replacement owner's lease. Use a new receipt
file for a new attempt; replaying an old claim never restores its old authority.
Unpublished work is not recovered by release: record the working branch/commit in
the reason or issue before relinquishing when there is work to preserve.

Request IDs are durable idempotency keys. After a failed/uncertain push, rerun the
same operation with the same request ID and receipt. Reusing an ID with different
arguments is denied. A replay returns the recorded result of that operation;
consult `list` for current state. A claim replay also checks that its lease is
still the current running owner. Contention is an error, not an automatic steal;
reread before retrying.

```powershell
python tools/classqueue.py v2 list
python -m unittest discover -s tools -p 'test_classqueue*.py' -v
```

## Pilot limits

The single ref deliberately serializes short metadata transactions; it is suited
to this small fleet, not a high-volume event service. Tasks, event history and
idempotency records currently accumulate without compaction. Reads fetch the state
and its retained artifacts; the cost grows with history. Files changed beyond
declared resources are not detected. There are no heartbeat expiry, forced
takeover, dynamic stage replacement, or administrative receipt recovery commands.
A lost receipt needs explicit coordinator investigation; the
client never infers abandonment from elapsed time. Establish the pilot's tasks
and preserve receipts before relying on it for the whole fleet.
