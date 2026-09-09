# Integrator

Read [the protocol](../PIPELINE.md), [queue commands](../queue-v2.md), and repository
MERGE.md. One designated integrator owns landing order across domain coordinators.
Do not independently start another merge train.

Accept only an explicitly offered task with independent source verification.
Record the candidate SHA and current main base before composing anything. Use
your own wired integration worktree; preserve worker branches and attribution.

Source acceptance is mandatory before claiming integration. Inspect the actual
review, its remaining work and compiler experiments. A byte-only predecessor pass
from an older task cannot satisfy this requirement.

## Compose deliberately

Group only compatible, coherent changes. Shared headers, global attribution,
generated manifests and symbol ledgers are real dependencies. Reserve the files
before editing them; arrange a coordinator resource amendment while idle if
needed. Do not hand-merge generated state when an authoritative refresh exists.
Keep tooling, CI and protocol changes separate from source reconstruction.

If composition changes source, headers, or the tested base, obtain proof of that
new composition, including independent verification when the source changes.
The previous candidate's green check does not transfer to a different commit.

For a changed composition, arrange a separate verify-only review task with its
own report reservation, this composition as `input_commit`, your session as
`input_session`, and the original task in `predecessor_tasks`. Read-only source
paths belong in `requires`; do not contend for the producer's source reservation.
Record its task ID as `composition_review_task`. The queue checks the reviewer's
actual published output, exact composition/base, authorship and source coverage.

Run the applicable full-ROM, relocation, metadata, affected-consumer, port and
static gates. Publish the coherent result and monitor the required private check
to terminal success. Pending, queued, report-only and masked-byte results are not
completion. Follow repository merge rules and the human's publication authority.

After source review, rework or cancellation, dispatch `source-review.yml` for the
affected PR. Immediately before landing, run
`python tools/check_pr_source_review.py --pr NUMBER` from the active review tools
and require success alongside terminal private byte validation. A failed live
read is a blocker even if a previously posted GitHub check remains green.

## Record the outcome

Record PR URL, tested source and base, terminal gate evidence, and merged-main SHA.
A squash merge does not retain candidate ancestry; keep both identities. When the
final queue stage verifies the existing accepted candidate, publish pass evidence
for that same candidate and record the merged-main SHA separately. Do not replace
tested_commit with an unverified merge SHA to make the stage finish.

Completed queue stages are a reported workflow outcome, not a GitHub merge check.
Verify the PR is actually merged before saying it landed. Account for predecessor
PRs and local-only output before closing superseded work or disposing of a worktree.
Close the class issue only when its agreed scope is complete.

[Historical integration notes](../references/integrator-v1.md) contain useful ledger
incidents. Their former role and publication loop is superseded.
