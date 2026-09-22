# Launch and restart prompts

Use the same prompts in Codex, Kimi K3, Claude, or another shell-capable harness.
The protocol lives in the repository; prior conversation memory is supporting
context. Read the current [protocol](PIPELINE.md). Use [CUTOVER.md](CUTOVER.md)
only for an unadopted legacy fleet; normal work resumes the existing v2 queue.

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

## Fleet coordinator: continue assigned work

> Read notes/agents/README.md, PIPELINE.md, queue-v2.md and roles/coordinator.md.
> Inspect live tasks, sessions, worktrees, branches, PRs and accepted handoffs.
> Resume existing candidates and facts; do not infer abandonment from claim age.
> For each assigned task, name the production deliverable, exact input, remaining
> scope, independent verifier and integration owner. Reserve shared dependencies.
> For TU promotion, use notes/tu-promotion-conventions.md. Finish existing viable
> candidates through their authorized endpoint before opening redundant shadows.
> Keep offered or blocked work attached to a next owner/action; report production
> enrollment, source reconstruction and PR/merge state separately.
> Do not reinitialize the queue or begin a competing integration train.
> Assigned scope: [issues/tasks and publication or merge limits].

For a genuinely unadopted legacy fleet, use CUTOVER's inventory/adoption procedure
before this prompt. Installing documentation alone does not stop old workers.

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

## TU promotion assignment

Use this alongside the producer prompt, preserving an existing candidate:

> Promote the assigned evidenced TU using notes/tu-promotion-conventions.md.
> Resolve its current source owner and reuse prior work. Deliver one canonical
> production compiler input under src/, complete default-build enrollment,
> retirement of absorbed source copies and per-symbol attribution. Reconstruct
> genuine C++ and prove bytes, relocations, emitted output and affected consumers.
> Do not add folder-localization or shadow-only PRs as routine phases. If a concrete
> blocker requires staging or a smaller production slice, record its exact proof,
> remaining promotion changes and next owner, and report that limit to the user.
> Hand the final candidate to independent review and the named integrator;
> follow the task's authorized publication/landing endpoint.
> TU/task: [ID, accepted input and intended scope].

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
