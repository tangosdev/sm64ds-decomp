# Require source review on the existing fleet

This upgrades the in-progress fleet. It does not restart classes, rebase worker
branches, erase earlier proof or replace the designated coordinator. The tooling
PR for [#2449](https://github.com/tangosdev/sm64ds-decomp/issues/2449) installs the
mechanism; the steps below activate it. Do not claim it is live merely because
the files exist on a branch or have merged.

## What the gate accepts

The independent verifier publishes `source_review` using
[the evidence template](templates/verification.json). It identifies the exact
candidate, tested base, reviewer session, inspected file paths and concrete source
judgment. Include changed/retired source paths, shared headers, enrollment/symbol/
relocation configuration and TU manifests. The GitHub check requires coverage of
every such path in the actual PR, including both paths of a rename.

The review examines existing real interfaces, fake virtual tables, raw field
accesses, integer pointers, manually spelled method symbols, lifecycle machinery,
helper ownership, naming evidence, attribution and truthful completion claims.
It is an accountable review, not a regex score or proof of original authorship.
Byte and relocation validation remains independently required.

Each finding has `id`, `location`, `summary`, `kind`, `disposition` and `reason`:

| Disposition | Acceptance requirement |
|---|---|
| `fixed` | The reviewer inspected the correction in this candidate; changed source receives fresh byte proof. |
| `compiler_constraint` | Reconstruction finding only; measured cleaner alternative under `2004/b56`, with pinned experiment source and a durable committed artifact. |
| `deferred` | Reconstruction finding only; reviewer explicitly accepts partial scope, names the issue and next owner, and sets `completion: partial`. |
| `open` | Cannot publish pass. Return the candidate through rework. |

Correctness and provenance findings cannot be relabeled as reconstruction to
defer them. Prior finding IDs must remain in later reviews with dispositions.
A compiler constraint's `evidence` contains `tested_commit`, `compiler`,
`attempted_change`, `command`, `result`, `log`, `artifact_commit`, `artifact_path`.
The queue verifies that the experiment/artifact commits and artifact file exist
and retains them in ancestry. Inspect those commits before publishing: referenced
history is pushed, so it must contain no ROM, compiler, receipts or credentials.
The reviewer still judges whether the experiment actually supports the claim.

## Activate deliberately

1. Merge the reviewed tooling PR. Keep the existing global coordinator. In this
   fleet that is `fleet-0907`, whose installed checkout was
   `C:/tmp/sm64ds-attr-rescue-0907`. Give that session the PR and this file.
2. Inventory the live queue, workers and open PRs again. Obtain checkpoint and
   upgrade acknowledgments. Running holders preserve their work and receipts,
   then publish or release their own leases. No age-based takeover. Only this
   brief protocol checkpoint requires draining running leases.
3. Every session uses a checkout containing the new tools. Keep original task
   workflow/source SHAs unchanged. Record the full reviewed tooling SHA separately
   as the review-policy pin; preserve a checkout at that revision.
4. The recorded fleet coordinator runs the following from that checkout, with
   actual immutable SHAs from its inventory:

   ```powershell
   python tools/classqueue.py v2 enable-source-review --session fleet-0907 --workflow-commit FULL_REVIEWED_TOOLING_SHA --expected-state FULL_OBSERVED_QUEUE_SHA --request-id enable-source-review-0907 --legacy-clients-stopped-and-upgraded
   ```

   The command refuses a stale inventory, another coordinator or a running lease.
   It changes remote state schema to 3. Private receipts remain schema 2; tasks,
   reservations, original pins, outputs, rejected attempts and history survive.
   Old clients reject state 3 instead of publishing an unreviewed pass. `next`
   reports both the original workflow pin and the active review-policy pin.
5. Dispatch `source-review.yml` from main and inspect its per-PR **Source review**
   check runs. In the repository's main ruleset, add **Source review**, pinned to
   the GitHub Actions app that produced that check, alongside the existing
   app-pinned **PR validation** requirement. Preserve existing protections and
   bypass settings. Require branches to be up to date before merging, so a check
   for an old base cannot authorize a newer composition. Do not weaken byte
   validation or set a blanket administrative bypass.
6. Prove the deployed boundary: an unreviewed source PR must fail Source review;
   a populated independently published review must pass for the exact head/base;
   a changed head/base must lose that acceptance. Record the workflow run, ruleset
   readback, queue SHA and review-policy SHA on the cutover issue.

**"Changed base" means changed reviewably.** The target is not the raw tip of
main: `check_pr_source_review.stable_target` walks back from the tip past
commits whose diff contains no `source_path()` entry, and anchors on the newest
commit that changed reconstruction source, a header or a TU manifest. Every
commit walked over is accepted as a `tested_base`, because all of them carry the
same reviewable tree.

This is a property of the deployed target, not a relaxation of the requirement:
a commit that touches reviewable source stops the walk, and every uncertain case
(an unreadable commit, a merge, a file list at the API cap, a malformed entry)
fails closed onto the live tip. Without it the check is unsatisfiable in steady
state rather than merely strict -- main receives periodic bot progress refreshes
of `contributions.json` and `docs/` marked `[skip ci]`, so acceptance expired on
a timer with nobody touching the PR. Measured 2026-09-12 before the change:
**0 of 117 open PRs** contained the live tip and could clear ancestry at all;
after it, 72 did.

Ruleset activation is a repository setting, not a consequence of this source PR.
Until it is configured, the check is advisory at GitHub even though upgraded
queue transitions enforce review. Coordinators must still honor rejected reviews.

## Adopt existing PRs and changed compositions

An older byte-only pass remains valid historical byte evidence. It is not a
source-review pass. Preserve it and arrange a fresh review of the actual PR head.
For a completed task or a composed PR, use a separate verify-only review task:

- `input_commit`: actual PR/composition head; `base_commit`: its preserved source base.
- `input_session`: the session that produced the adopted/composed head.
- `producer_sessions`: other known source contributors from earlier work.
- `predecessor_tasks`: original task IDs, including the previous review task when
  revising a composition. Enqueue inherits findings and contributing sessions.
- `resources`: the new review's own report/handoff path. This is a read-only
  review; the source owner's existing reservations stay intact.
- Stage: role `humanizer` or `verifier`, mode `verify`, source/header inputs in
  `requires`, and no source output in `produces`.

Publish the same input SHA with the source-review evidence. A failure remains a
failure: preserve it on the class issue and use the existing verify-only recovery
or an owned producer continuation. The review task never edits the candidate.

For a still-running integration task, set `composition_commit`, `composition_base`
and `composition_review_task` in its final evidence. The referenced task must
inherit the original task and have actually published an independent verify
output for that composition/base. A nested JSON assertion naming a reviewer is
insufficient. The composition must retain both source and base ancestry.

The first concrete adoption case is [PR #2447](https://github.com/tangosdev/sm64ds-decomp/pull/2447),
`daSanbo_c`, task `sanbo-ov096-0907b`: accepted source
`58a735d6ffc733823fda12aaee1d9fec724a8721`, proposed PR head at review
`fb3babaef848aa1e4fd038fd4705d2162f4295c7`. The old task recorded the composition
under `composed_head`. That old pass has no source review and must not be
reinterpreted as approval of the PR. Refresh its live head before assigning work.

## Keep GitHub and the coordinator current

The workflow executes default-branch tools and API metadata only. It never checks
out or executes PR code. It refreshes on PR changes, main pushes, explicit dispatch
and a five-minute schedule. After review, rejection, rework or cancellation, run:

```powershell
gh workflow run source-review.yml --repo tangosdev/sm64ds-decomp -f pr=2447
python tools/check_pr_source_review.py --pr 2447
```

Use the actual PR number. The Python command is read-only unless `--publish` is
explicitly supplied. Its current queue/head/base result is required immediately
before landing. The GitHub check is a snapshot: queue updates can precede the
scheduled/queued refresh, so a previously green check never overrides a current
rejection. PR and queue identities are rechecked before publishing a verdict.
Transport failure or incomplete file coverage cannot become a pass.

## Review the existing classes

Inventory all promoted manifests against a pinned main SHA and reuse class
issues, PRs, facts and handoffs. Mark missing review evidence as pending; a grep
inventory only supplies leads. Review a whole class/TU, record the reviewed SHA
and concrete findings on its issue, and route narrow proven fixes through PRs.
Keep build/promotion counts in the C++ TU reports separate from source acceptance.
An accepted partial review leaves its named follow-up work open.
