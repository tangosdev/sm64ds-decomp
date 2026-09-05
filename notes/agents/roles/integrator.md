# Role: integrator — compose N proven promotions into one validator run

This role exists because of a hard resource limit, not a quality one. The
validator is **one box, one job, ~20-33 minutes**, and every merge to `main`
invalidates the pinned base of every other open PR. So five separately-proven
class PRs do not cost five validator runs — they cost five runs plus four
rebases plus the conflict cascade the rebases cause. Composed into one branch
they cost **one run**.

You do not re-derive anything. Every class you compose has already been proven
by a builder. Your job is to move proven commits onto a fresh base without
losing or inventing anything, and to prove *that*.

## When to compose

- Two or more class PRs are open, each already green from its builder.
- They touch the same ledger files (they always do), so they will conflict with
  each other in sequence no matter what order they land in.
- Nobody has started rebasing them by hand yet. If someone has, read their
  branch first — hand-resolved ledger files are the one thing you cannot trust.

Do **not** compose a class whose builder has not finished. An unproven class in
the batch makes the whole batch unmergeable, and the validator will not tell you
which one.

## The shape

Cut from `origin/main` at a named SHA — write the SHA in the PR body, it is the
only record of what you rebased onto:

    git fetch origin main
    git log -1 --format=%H origin/main          # record this
    git checkout -b integrate/<ovNN>-promotions-<MMDD> origin/main

Then cherry-pick each class's **content** commits, in class order, class by
class. Two things get dropped deliberately:

- **`Preserve <X> TU attribution and state` hook commits.** The pre-push hook
  regenerates these; carrying them forward replays stale state onto a new base.
  Attribution is a documented non-goal in this repo — do not spend effort
  reconciling it, and do not let a hook commit decide your tree.
- **Anything touching the two ratchet files.** Take `origin/main`'s copies and
  reconcile once at the end (below). Cherry-picking them class by class is how
  double-banked rows get created.

Everything else — the TU `.cpp`, the deleted shards, `delinks.txt`,
`symbols.txt`, the manifest — is content and comes across intact.

## Ledger reconciliation, once, at the end

This is the step that goes wrong, and the failure is silent in both directions.
Do it non-destructively:

1. Copy `config/converted-baseline.json` and
   `config/converted-backslide-exceptions.jsonl` aside.
2. `git checkout origin/main --` both.
3. Run `python tools/tiers_ratchet.py --update --reason "<why>"` **once**.
4. `git diff --no-index` the regenerated files against your copies.

Read the diff, do not just look at the exit code:

- **Identical** — the composed tree already carried the right rows.
  `git checkout HEAD --` both and commit nothing.
- **Differs only in `reason` strings** — your copies win. The writers wrote one
  informative reason per class; a regeneration replaces all of them with N
  copies of your single `--reason`. Nothing reads those strings, so no gate goes
  red. It is real data loss with no alarm.
- **Differs in the path set, order, or position** — the regenerated file wins,
  and say so in the PR body with the count.

Then audit what you are shipping: **count byte-identical whole records** in the
exceptions file. Zero is the target. Dedupe by *whole record*, never by path —
one path legitimately recurs under different reasons, and `main` carries dozens
of such rows from past promotions. A path-based dedupe deletes real history.

## Prove the composition, not the classes

The builders proved the classes. You prove that composing them changed nothing:

    python tools/tubuild.py verify <ov>/<Class>        # once per class
    python tools/rombuild.py -j 16 --no-rom            # 106/106 exact
    python tools/romdata_check.py --files src/actors/<Class>.cpp   # per class
    python tools/tiers_ratchet.py --check              # PASS
    python tools/cpp_tu_state.py --check-note

Run `git status` after **every** `verify` — it writes to the manifest when it
fails, and a dirty tree after a green-looking run means it did not pass.

Two things to re-confirm per class rather than trust the builder's PR body,
because both are cheap and both have been wrong before:

- the delink **row count** against the formula, and
- that `externalized_output` is empty (a non-empty block means the class took
  the text-verified-only route and cannot be promoted at all).

Also re-verify every canonical address against the **owning module's**
`symbols.txt` — not the class's own overlay. Cross-module homes are normal and
are what make the promotion legal.

## The PR

Title: `Integrate <n> <ovNN> TU promotions (<Class>, <Class>)`.

Body must carry, because none of it is recoverable from the diff:

- the base SHA you cut from,
- the list of source PRs being superseded, by number,
- which commits you dropped and why,
- the ledger verdict from step 4 above, with the identical-record count,
- one gate block per class, plus the single `rombuild` and `tiers_ratchet` lines
  that cover the whole batch.

When it merges, **close the source PRs as superseded** with a comment naming the
integration PR. They will not close themselves, and a stale open PR whose work
already landed is what makes the next agent rebase something twice.

## What not to do

- Do not "fix" a partial promotion you are composing. A builder that shipped
  22-of-61 measured a real refusal; raising the count in an integration branch
  means you re-derived something without the evidence.
- Do not compose across overlays without a reason. Same-overlay batches share
  the ledger rows that were going to conflict anyway; cross-overlay batches just
  make the blast radius bigger for the same one validator slot.
- Do not use `git merge-tree` to predict whether this will merge. The exceptions
  file is declared `merge=union`, GitHub ignores the driver, and `merge-tree`
  honours it — so it reports clean where GitHub reports CONFLICTING. Use
  `gh pr view <n> --json mergeable`.
