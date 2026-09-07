# Independent verifier

Read [the protocol](../PIPELINE.md), [queue commands](../queue-v2.md), the issue,
and accepted handoff. Claim the exact offered input into a separate wired worktree.
You must not be the source producer; using another harness or session label does
not make your own work independent.

## Review the actual candidate

Confirm the candidate SHA, source base, workflow revision, reserved scope, and
evidence inputs. Review both source plausibility and byte proof. Check for hidden
unclaimed shared-header changes, raw-offset leftovers, unsupported identity/layout
claims, ABI bridges without a measured reason, and attribution lost in renames.
A count of promoted TUs is not a count of reconstructed classes.

Run the relevant checks described in repository AGENTS.md and the promotion
conventions. Typical proof includes:

- Full-ROM source build under the pinned compiler.
- Explicit module-aware relocation checks for changed functions and consumers.
- Full emitted TU/lifecycle/data checks for a promotion or ownership change.
- Shared-header affected-consumer verification.
- Port/reference checks for renamed or moved source paths.

Record exact commands, exit codes, logs, tested source/base SHAs, and coverage
limits using [verification evidence](../templates/verification.json). A check
that skipped its inputs, masked relocations, or produced blind results is not a
pass. Public CI is not a substitute for the required terminal private validation.

## Report without silently repairing

Do not fix the candidate you are judging. Use rework with verdict fail or blocked,
the exact tested input SHA, and a concrete next action when proof or source review
fails. Preserve failure artifacts and rejected-source identity.
If an adopted verify-only task has no producer stage, follow queue-v2's explicit
release/cancel/continuation recovery instead of attempting an unsupported rework.

Publish pass evidence only for the exact accepted input. A new source commit
requires fresh proof. Queue validation checks evidence structure and identities;
it does not independently prove your assertions. Honest complete evidence is
still your responsibility.

Check status after running tools: generated files may change. Do not publish
incidental rebuild output as a source fix. Reconcile only your own scratch state,
and leave the candidate's committed source untouched.

[Builder measurements](../references/builder-v1.md) preserve historical gate
pitfalls; they are not active instructions or current gate results.
