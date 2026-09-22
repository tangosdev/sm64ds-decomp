# Launch and restart prompts

Use the same prompts in Codex, Kimi K3, Claude, or another shell-capable harness.
The protocol lives in the repository; prior conversation memory is supporting
context. Install this revision first and complete [CUTOVER.md](CUTOVER.md)
before v2 workers start.

Repository: `C:/Users/andre/source/tangosdev/sm64ds-decomp`.

## Continue from the previous version

Paste into the existing session, replacing the final line with its known class,
issue, or branch when available:

> Continue your previous SM64DS work under the updated agent protocol.
> Read notes/agents/README.md, notes/agents/PIPELINE.md,
> notes/agents/CUTOVER.md, and your current role entry from the updated
> repository checkout. Do not follow the old automatic claim/release loop.
> First identify your existing worktree, branch, HEAD, uncommitted changes,
> facts, prior proof, open PR, and legacy claim. Preserve all of that work.
> Produce the handoff described in notes/agents/templates/handoff.md and
> report the exact next action. Before further source edits, confirm the fleet
> cutover is complete and obtain or resume the v2 task assigned to this work.
> Resume its accepted input or explicitly adopted checkpoint; do not start the
> class over. Follow the continuous producer role unless the task assigns you
> independent verification or integration. Do not initialize the shared queue
> or remove another session's claim. If the old workflow has not stopped yet,
> finish the checkpoint and wait for the coordinator's recorded adoption.
> Existing work: [class, issue, branch, or "identify from this session"].

The prompt works even if the source branch predates these docs: read this
installed protocol revision in the updated checkout, then return to the existing
source worktree. Keep the protocol/tool checkout separate from the source input.
The coordinator records both commits. Do not merge workflow files into every
in-flight source branch just to get a launcher.

## Fleet coordinator: stop, inventory, and adopt

> Read notes/agents/README.md, PIPELINE.md, CUTOVER.md, queue-v2.md, and
> roles/coordinator.md in the updated repository checkout.
> Coordinate the v1-to-v2 cutover. Inventory the existing workers, worktrees,
> claims, branches, PRs and facts; distinguish unknown liveness from abandonment.
> Use a GitHub issue for each active class/evidenced TU and record its current
> output and remaining scope. The human is stopping old workers: verify their
> checkpoint/release acknowledgments before initializing v2.
> Reuse existing research and branches. Keep shared-header overlaps serialized.
> Register explicit continuation tasks with exact input SHAs, resources,
> producer identity when known, and remaining stages. Start with a small pilot.
> Never infer that an agent stopped merely because its claim is old.

## Producer: one class/TU through reconstruction

> Read notes/agents/PIPELINE.md, roles/producer.md, and queue-v2.md.
> Work the assigned GitHub issue/task using its recorded protocol revision.
> Read the issue, queue input, facts, and handoff; inspect existing work before
> creating anything. Obtain the v2 producer claim, use your own wired worktree,
> and own the class through method/layout reconstruction and local proof.
> Ask the coordinator to reserve newly discovered shared dependencies before
> editing them. Commit a concise handoff with the final candidate, publish its
> exact SHA to the next stage, and report the remaining reconstruction work.
> Task: [issue URL and task ID].

## Independent verifier

> Read notes/agents/PIPELINE.md, roles/verifier.md, and queue-v2.md.
> Accept the assigned task's exact offered input into your own verification
> worktree. Confirm you did not produce it. Re-run the required proof and review
> its source/provenance claims. Record commands, exit codes, tested source/base
> SHAs, logs, and coverage limits. Return failures through the rework transition;
> do not fix the candidate yourself or publish failed evidence as a pass.
> Task: [issue URL and task ID].

## Integrator

> Read notes/agents/PIPELINE.md, roles/integrator.md, and queue-v2.md.
> Own the shared integration queue for the assigned accepted candidates.
> Recheck candidate and main SHAs, compose compatible work in your own wired
> worktree, reconcile shared bookkeeping once, and run the required composition
> gates. Publish only the coherent reviewed result, monitor terminal validation,
> and record the PR and merged-main SHA. Account for superseded work and
> attribution before completing the task. Do not convert a pending check to a pass.
> Assigned tasks: [task IDs and issue URLs].

## Specialists

For a dedicated source reviewer, use:

> Read notes/agents/PIPELINE.md, roles/humanizer.md and SOURCE-REVIEW-CUTOVER.md.
> Review the assigned exact candidate and its inherited findings. Inspect existing
> real interfaces before accepting shadow structs, raw fields or mangled calls.
> Publish an independent source-review verdict with concrete dispositions. Return
> unresolved findings through rework; do not edit the candidate you are reviewing.
> Task: [issue URL, task ID, candidate SHA and report worktree].

Scouts read [roles/scout.md](roles/scout.md); source reviewers read
[roles/humanizer.md](roles/humanizer.md). They assist the producer or take an
explicitly scheduled stage. They do not independently edit the producer's worktree.

## Checking status

Run these from the installed protocol/tool checkout:

```powershell
python tools/classqueue.py v2 list
python tools/classqueue.py v2 next --role producer
python tools/classqueue.py v2 next --role verifier
gh pr list --repo tangosdev/sm64ds-decomp --state open --limit 100
```

Before initialization, `v2 list` is expected to refuse; use the read-only legacy
inventory commands in CUTOVER. Private receipts remain in ignored local storage.
A missing receipt or unreachable queue is a recovery task, not an invitation to
claim a second copy of the work.
