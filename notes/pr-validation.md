# Full PR validation

The merge gate should answer several different questions without compressing them into
one misleading percentage:

| Metric | Meaning | Gate |
|---|---|---|
| Matched functions and bytes | A verified-looking `src/` file exists for a configured ROM function | Must not regress |
| Coverage denominator | Configured function count and code-byte universe | Must not change silently |
| Source-built functions and bytes | Code actually linked from our translation units instead of a ROM gap object | Must not regress |
| Module fidelity | Linked executable-module bytes equal retail | Stock head must pass, unless base and head have the same recorded pre-existing build failure |
| Contributor lineage | The first matcher still owns a surviving match after moves/renames | Must not change or disappear |
| Relocations | Affected source reproduces bytes and names the correct destinations | No WRONG or NO-REPRO |
| Port references | `port/`'s manifests and symbol bridges still name files and symbols that exist | No stale reference (optional phase) |

`tools/rombuild.py` emits the build/fidelity artifact. `tools/validate_merge.py` compares
two committed revisions, combines the build artifacts with `pr_linkcheck` JSON, and emits
both stable JSON and the Markdown table shown on a PR.

## Private worker sequence

The compiler and ROM remain on the private worker. For each relay job:

1. Check out the requested `baseSha` and run
   `python tools/rombuild.py --profile stock --report-json build/base-rom.json`.
   The worker may cache this artifact by commit SHA.
2. Create a temporary branch at `baseSha`, merge the requested PR head SHA, and
   **commit the merge before measuring**. Attribution follows Git history; a staged but
   uncommitted move has no lineage.
3. Run `python tools/rombuild.py --profile stock --report-json build/head-rom.json`.
4. Run `python tools/pr_linkcheck.py --base <baseSha> --json build/link.json --md build/link.md`.
5. If the PR touched `src/` or `include/`, run
   `python tools/port_refcheck.py --json build/port.json`. No compiler and no ROM, so
   it costs about a second. It is optional on both sides: a worker that does not run it,
   or a base that predates the tool, simply reports no port row.
6. Run:

   ```
   python tools/validate_merge.py --base <baseSha> --head HEAD \
     --require-merge-commit --expected-pr-head <headSha> \
     --base-rom-report build/base-rom.json \
     --head-rom-report build/head-rom.json --link-report build/link.json \
     --port-refcheck-report build/port.json \
     --out build/validate-report.json --markdown build/validate-report.md
   ```

7. Return `status`, `summary`, `details` (the existing per-file table), and the new
   `reportMarkdown` field to the relay. The public GitHub workflow remains unable to run
   PR code or access ROM material; it only renders the worker's result.

A base failure is not hidden. If base and merge fail in the same phase with the same
failure signature, the report shows a warning and permits a non-regressing PR. If a green
base becomes red, or the failure changes, validation fails.

The merge gate always uses the stock profile. `--profile mods` is a developer tool for
building intentional experiments and is never accepted as reconstruction proof.

## Where a job's time goes

Both ROM builds in that sequence used to recompile all 9,116 enrolled sources, and step
4 checked its files one at a time, so a job took 10–20 minutes for a handful of edited
files. Two changes address that, and neither weakens a verdict:

- **Steps 1 and 3 reuse unchanged objects.** `rombuild.py` keys objects on their exact
  contents (see [`rom-build.md`](rom-build.md#the-object-cache)), so a build compiles
  only what the PR reaches. The cache lives in `build/`, which survives the worker's
  `reset --hard` and its scoped `clean` between jobs — that persistence is the point,
  and it is safe only because nothing in the key is a timestamp. Each report states how
  many objects it reused, so the number is auditable rather than assumed.
- **Step 4 checks files in parallel.** `pr_linkcheck.py` takes `-j`; every file is an
  independent compiler-version sweep, and the classification and printing that follow
  stay serial and in diff order, so the JSON and Markdown are identical to a `-j1` run.

The worker still runs one job at a time. That is not the bottleneck it looks like: the
container has a two-CPU quota, so concurrent jobs would divide the same cores and finish
no sooner. Give the box more CPU before adding a second worker slot, and if a slot is
ever added, give it its own clone — jobs share `build/` and would otherwise overwrite
each other's objects.

## What a pull request sees

The workflow submits and exits; the relay owns everything visible after that. It drives
one check run and one comment, both edited in place as the job moves through
received → validating → verdict, so a pull request can tell "queued behind other work"
from "building right now" from "nothing is happening" — which matters because the queue
runs one job at a time and a wait is normal rather than a fault.

`https://tangos.dev/validator` shows the queue itself: depth, what is building and for
how long, and recent verdicts. Note that worker liveness there is inferred from work in
progress, not from the heartbeat — only claiming a job records one, so a worker mid-build
looks silent for the entire build.

## Giving up on work nobody is waiting for

With one build slot, the queue's worst enemy is spending it on a verdict that will never
be read. Two cases produce those, and both now end the job instead of running it out:

- **A newer push.** GitHub gates merging on the latest commit, so once a pull request is
  pushed again, its older head's verdict is unreadable. Submitting a job retires that
  pull request's other in-flight heads as `Superseded`.
- **An explicit release.** `POST /api/pr-validate/{id}/cancel` marks a job `Cancelled`.
  The workflow does not call it: since it submits and exits, a cancelled run has nothing
  left to cancel. This is for the case the queue cannot infer — a job that is stuck, or
  one an operator has decided nobody wants. It is also the only way to free a job whose
  worker died mid-build without waiting out the sweeper's ceiling, because `Enqueue`
  hands back the in-flight job for a commit and so a re-run alone cannot replace it.

Both are terminal, both report the check as GitHub's `cancelled` conclusion rather than a
pass or a failure, and neither can be overwritten afterwards — a worker only notices
between polls, so it can genuinely finish and post a result late, and letting that land
would walk a retired head back to a verdict.

**Superseding a build that is already running is deliberately restrained**, because the
obvious version of this is what the per-SHA concurrency group exists to avoid. A running
job is interrupted only while it is still young (`PrValidationStore.YoungBuild`), and only
until a pull request has had `MaxRunningSupersedeStreak` builds killed in a row; one
completed verdict refills that budget. Without both guards, a branch the matching console
pushes to repeatedly would never reach a verdict at all — every build cancelled by the
next push, exactly the starvation that made per-PR concurrency grouping unusable.

The worker polls `/api/pr-validate/{id}/state` between phases while it builds. Only an
explicit `Superseded` or `Cancelled` stops it: a failed poll or an unreadable answer means
keep going, so a brief relay outage can never throw away a build. Cancellation signals the
`timeout` process rather than the phase directly, because GNU `timeout` runs what it
manages in its own process group — signalling anything else would leave wine and mwccarm
running on the two CPUs the next job needs. `tests/test-worker-cancel.sh` in the validator
repo asserts exactly that: the phase's own process is gone afterwards.
