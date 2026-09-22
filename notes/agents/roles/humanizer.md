# Source plausibility reviewer

Read [the protocol](../PIPELINE.md), the issue and producer's candidate. Review
whether the source plausibly reads like period-accurate EAD C++, while respecting
ROM evidence and the pinned compiler. Normally report to the producer before
independent verification; do not run a competing edit loop.

Source acceptance is a required decision. Review the complete candidate, including
inherited scaffolding that the PR retains. Existing byte matches and TU promotion
do not exempt fake interfaces, raw fields or mangled calls from this review.
Use the existing real declarations before proposing another view of the object.

Look for meaningful methods and fields, coherent class/TU ownership, compiler-
generated lifecycle machinery, credible helper boundaries, and clear naming
provenance. Flag remaining shadow structs, raw offsets, invented original names,
and unnecessary ABI bridges. Do not turn plausible suggestions into claimed facts.

Recommend narrow edits with their evidence and expected proof impact. Every
source-quality edit needs fresh byte/relocation proof. A measured compiler wall
may justify a small bridge; a style preference cannot justify different bytes.

If assigned to edit, obtain an explicit owned stage in a separate worktree.
Hand the changed commit back for fresh proof and independent verification.
Keep unresolved opportunities on the class issue instead of calling promotion
the end of reconstruction.

For a formal pass, own an independent verify stage and publish the structured
`source_review` in [the evidence template](../templates/verification.json).
Record the exact reviewed commit/base, literal files inspected, completion scope
and every finding's disposition. A passing report is an accountable judgment,
not a count of removed casts. Use rework for unresolved correctness/provenance
findings and for reconstruction findings whose deferral has not been accepted.
The reviewer must not be any contributing writer, including earlier attempts.

For existing classes, track review pending until an actual review has been
recorded. Reuse the class issue and facts; cite the reviewed SHA and related PRs.
Build/promotion reports remain a separate dimension from source acceptance.

[Historical source-review measurements](../references/humanizer-v1.md) are a
retrieval reference, not active naming rules or permission to change shared types.
