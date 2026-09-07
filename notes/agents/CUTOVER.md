# Stop v1, preserve work, restart on v2

This procedure adopts existing work. It does not discard old branches or require
classes to be reconstructed again. Installing these files is not fleet cutover.
The user must actually stop the old workers before the coordinator activates v2.

## 1. Checkpoint every old session

Use the continuation prompt in [LAUNCH.md](LAUNCH.md). Each old worker records:

- Its harness/session, class or TU, worktree, branch, and exact HEAD.
- Dirty files, local-only commits, published refs, and open PRs.
- Existing facts, compiler probes, verification logs, and next useful action.
- The exact legacy claim(s) it owns and any independent range-service claim.
- Whether its output is a verified candidate or an unfinished checkpoint.

Use `git status --short`, `git log`, and named refs. Do not reset, stash shared
state, delete a worktree, rebase a published branch, or run a broad cleanup.
Commit a checkpoint only after inspecting exactly which files it includes. A
nonmatching checkpoint remains WIP and never becomes eligible source by declaration.
Keep ROM/compiler inputs and private receipts out of commits.

A worker releases only its own known legacy claim, after preserving its output.
The old `release` command is not owner-fenced: verify the remote claim identity
immediately before release. If identity is uncertain, stop and have the original
holder/coordinator reconcile it; do not guess from a username shared by all harnesses.
Record that the worker has stopped its old intake loop.

## 2. Inventory and reconcile

From the updated checkout:

```powershell
git worktree list --porcelain
git ls-remote origin 'refs/claims/*'
git ls-remote origin 'refs/heads/*'
gh pr list --repo tangosdev/sm64ds-decomp --state open --limit 100
```

These are inventories, not active-session counts. Match them to checkpoint
acknowledgments. A branch with no claim can still hold useful or running work.
Inspect existing scout branches and facts before opening a replacement task.

Inspect the separate range-claim workflow too. V2 cannot protect work against
unregistered tools or stale clients. Stop their intake or assign independently
verified disjoint resources. No automatic timeout/claim reclamation is part of v2.

Check the exact overlapping file/ROM scopes. Independently named classes can all
edit one shared header. Serialize that dependency or schedule a separately owned
prerequisite; do not silently promise each producer the whole header.

## 3. Choose the installed protocol checkout

All restarted sessions must run the same committed v2 tool revision. Source
worktrees may retain their previous branches/commits. Record `workflow_commit`
separately from the source base and continuation commit.

The queue command is run from that installed checkout; the claim's `--worktree`
identifies the source worktree. Linked worktrees share Git objects. If a source
commit comes from another clone, fetch its named published ref into the protocol
checkout before registering it. A mutable branch name alone is not an accepted
input.

Ensure `python tools/classqueue.py v2 --help` exposes the documented commands.
Never copy only the docs while assuming an old worktree has the new tool.

## 4. Initialize once, after stop acknowledgments

Follow [queue-v2.md](queue-v2.md). Initialization requires the stopped/upgraded
legacy-client condition and refuses visible legacy claim refs. That refusal is
a diagnostic guard; old binaries do not know about v2, so the stop condition is
operationally necessary even when the list is empty.

The coordinator creates the visible `agents/coordination` Git branch. It contains
queue metadata, not source to merge into main. Keep it reachable and do not reset
its history. Initialization and task publication are remote writes.

## 5. Register continuations, not duplicate jobs

Reuse an existing class/TU issue when one exists. Otherwise use
[the issue template](templates/issue.md). State the preserved checkpoint, facts
commit, old PR(s), and remaining work. Do not close an old PR merely because it
predates the protocol; first establish whether its content is retained or superseded.

Use `issue-N-reconstruct` or another stable unique task ID. Record the original
source base (an ancestor of the checkpoint), exact `input_commit`, `workflow_commit`,
resource set, and
remaining stages. Use producer -> verifier -> integrator for unfinished source.
If adopting already finished source directly into verification, record its actual
producer identity and require an independent verifier. Unknown authorship is a
fact to resolve, not an invented session name that makes self-review pass.

Enqueue stores a private coordinator receipt; claim stores a different worker
receipt. Keep both in ignored local storage. Publish/accept transitions preserve
the output and identify the next owner. See queue-v2 for exact JSON keys and CLI.

Do not substitute today's main for a stale branch's actual source base. Record the
current integration base separately; update the source deliberately when needed.

Facts-only work starts from its published facts commit; mixed uncommitted work
must first become a carefully inspected checkpoint. A prior green result remains
evidence for its recorded SHA and inputs; changing either requires new proof.

## 6. Pilot and restart

Pilot a few already inventoried tasks across the available harnesses. Exercise:

- Resume from a pre-v2 checkpoint without losing its facts or local commits.
- Competing claims and overlapping shared resources: exactly one owner succeeds.
- Producer handoff accepted by a different verifier.
- Failed verification returned for rework with the failure evidence retained.
- A stopped/restarted session recovering its receipt and next action.
- Integration recording the candidate, tested base, PR, and resulting main SHA.

Then restart the remaining workers using LAUNCH. Check for any old intake loop
still running. A quiet issue or elapsed timer is not proof a worker stopped.

## Recovery

Read the durable record before every recovery. Preserve outputs and failed proof.
Use the supported release/rework/amend/cancel commands with the correct receipt.
Do not manually rewrite state JSON, delete refs, or assume a lost receipt means
the work is free. Uncertain ownership requires coordinator reconciliation.

Use the wired-worktree removal helper only after all useful work is banked and
its owner has confirmed disposal. Cutover itself requires no worktree deletion.
