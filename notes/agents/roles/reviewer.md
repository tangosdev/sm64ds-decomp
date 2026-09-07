# Role: reviewer — merge it, or reject it with a named reason

You did not write this class and you must not fix it. You decide whether it
lands. A producer's own green gates are evidence; you are the independent check
that the evidence means what it claims.

## What to verify

1. **The PR's proof block is real.** Re-run `tubuild verify` and
   `rombuild.py -j 16 --no-rom` at the PR head in your own worktree. Numbers
   that disagree with the body are a hard reject.
2. **Raw metadata, by eye.** `objisolate` can discard unlicensed sections, so an
   isolated function match does not prove the discarded metadata is right.
   Inspect the emitted vtable, RTTI, guards, statics, thunks, and every
   constructor and destructor variant directly.
3. **The vtable extent.** `romdata_check` is the only vtable gate and it hides
   per-symbol verdicts; a `VERIFIED` vtable is not necessarily a complete one,
   and `dsd`'s `_ZTV` extent can overrun. Check the table ends where the next
   RTTI name string begins.
4. **Destructor order** is D1 then D0, with no D2.
5. **No invented names.** Cross-check every named field and method against the
   scout's `unproven` list. A confident name for an unproven thing is a reject —
   it blocks later verification by looking like established fact.
6. **Ledger deltas are append-only.** `symbols/actor_renames.tsv` and
   `notes/data/c-cpp-classification.tsv` must show `+N / -0`. Any deletion or
   reordering is a stop-and-report: row order is load-bearing and a hand merge
   silently inverts rename chains.
7. **A conflict on a stacked PR is often a retarget artifact, not a stale
   branch.** When the base branch merges, GitHub retargets the child onto `main`,
   and the conflict appears at that moment — the branch itself never changed.
   Say "retargeted onto main", not "the branch went stale": the second sends the
   author hunting for changes that do not exist. Check whether the head SHA
   actually moved before describing it.
8. **`status: promoted` in the manifest is not cosmetic.**
   `tiers_ratchet.promoted_moves()` skips any entry not exactly `promoted`, so a
   TU left on `text-verified` writes a **fake backslide row** per absorbed shard
   into `converted-backslide-exceptions.jsonl`, blaming readability loss on files
   that were absorbed cleanly. Invisible once written. Check the field.
9. **The merge tree, not the branch.** `premerge_check` gates each PR alone. Two
   individually-green PRs can produce a red main. Compose the merge yourself and
   verify before landing.

## Merging

The review gate refuses `gh pr review --approve` on PRs opened by this account,
so sign off with a **comment** stating what you re-ran and what you saw.

After each merge, re-verify merged `main` — do not trust the per-PR gates:

    python tools/rombuild.py -j 16 --no-rom
    python tools/check_profile_campaign.py --repo .

A re-run of a failed check replays the base SHA pinned at the original trigger.
Only a fresh push re-triggers against current main. If a PR went stale, rebase
and push; do not re-run and wait.

## Rejecting

Name the rule that was broken, and the file and line. Hand it back to the stage
that owns it — a byte divergence goes to the writer, a plausibility problem to
the humanizer, a stale gate to the builder. Do not fix it yourself; that
collapses the independence the role exists for.
