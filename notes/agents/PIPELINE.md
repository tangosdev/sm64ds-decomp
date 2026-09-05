# Class reconstruction pipeline

Write the game's C++ classes back, one class at a time, with several agents
working at once and no human holding the thread in their head.

Every rule an agent must obey lives in this directory as plain Markdown. Nothing
here depends on a Claude skill, a Claude memory, or a particular model — a Codex
instance and a Claude instance read the same files and follow the same protocol.

## The shape

One class moves through four stages. Each stage is a separate agent run with a
fresh context, reading the previous stage's written output rather than inheriting
a conversation.

| stage | role | consumes | produces |
|---|---|---|---|
| 1 | `scout` | the ROM | `notes/data/class-facts/<class>.json` |
| 2 | `writer` | the facts file | `include/<class>.h` + source |
| 3 | `humanizer` | the written source | a revised source that reads like 2004 EAD C++ |
| 4 | `builder` | the revised source | green byte gates, then a PR |
| 5 | `reviewer` | the PR | merge, or a rejection with a named reason |

Stages 1-3 never run a byte gate; stage 4 is the only one that decides whether
the bytes are right. Stage 3 never changes semantics. Stage 5 is a different
agent from stage 4 — a producer's own green output is evidence, not review.

## The queue

`notes/data/class-build-worklist.tsv` is the ordered target list, base classes
before their subclasses. Columns:

    class_name  state  header_path  base_class  subclass_count  overlays  method_count

`state` is one of `DONE`, `PARTIAL`, `MISSING`. Rows are worked top-down; the
ordering is the dependency ordering, so do not skip ahead to a subclass whose
base is still `MISSING`.

## Claiming work

A claim is an atomic ref create on the remote. The remote accepts a given ref
name exactly once, so two agents racing for one class cannot both win, and no
shared file is edited — a claims *file* is what produces merge cascades.

    python tools/classqueue.py next    --role writer
    python tools/classqueue.py claim   dActor_c --role writer --worktree C:/tmp/sm64ds-dactor
    python tools/classqueue.py release dActor_c --role writer
    python tools/classqueue.py list

`claim` exits non-zero when the class is already held. That non-zero is the
lock. Do not retry it, do not force-push over it — take the next row.

Release your claim when your stage's output is committed and pushed, not when
you personally are done thinking.

## Launching an agent

Point any instance — Claude Code or Codex — at one role file and one class:

    Read notes/agents/PIPELINE.md and notes/agents/roles/<role>.md in
    C:\Users\andre\source\tangosdev\sm64ds-decomp, then execute that role for
    the next unclaimed class. Claim it first with tools/classqueue.py.

The role file carries the rest. An agent that finishes its class should claim
the next one rather than stopping.

## Rules that hold for every role

1. **Name your worktree and your ref in every claim about a file.** This tree
   routinely has 150+ sibling worktrees at different refs, and the main checkout
   is itself usually stale. `git show <ref>:<path>` carries the ref in the
   command; a bare file read does not.
2. **One worktree per agent**, created with the `decomp-worktree` recipe, torn
   down with `wt-remove.ps1`. Never `git worktree remove` — it deletes through
   the junctions and empties the real ROM dump, which cannot be re-downloaded.
3. **Never edit a shared ledger by hand.** `symbols/actor_renames.tsv` and
   `notes/data/c-cpp-classification.tsv` are append logs whose row *order* is
   load-bearing; a hand merge silently inverts rename chains.
4. **A near-miss never lands in `src/`.** Bank it in `nearmiss/db.jsonl` and
   restore the matched source.
5. **Report the outcome, not the effort.** If a gate failed, paste the failure.
