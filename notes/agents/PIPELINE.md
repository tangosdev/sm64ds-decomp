# Class reconstruction pipeline

Write the game's C++ classes back, one class at a time, with several agents
working at once and no human holding the thread in their head.

Every rule an agent must obey lives in this directory as plain Markdown. Nothing
here depends on a Claude skill, a Claude memory, or a particular model — a Codex
instance and a Claude instance read the same files and follow the same protocol.

## The shape

One class moves through five stages, plus a sixth that is not per-class. Each is a separate agent run with a fresh
context, reading the previous stage's written output rather than inheriting a
conversation.

| stage | role | consumes | produces |
|---|---|---|---|
| 1 | `scout` | the ROM | `notes/data/class-facts/<Class>.json` |
| 2 | `writer` | the facts file | one `src/actors/<Class>.cpp`, its manifest, and the bookkeeping below |
| 3 | `humanizer` | the written source | a revised source that reads like 2004 EAD C++ |
| 4 | `builder` | the revised source | green byte gates, then a PR |
| 5 | `reviewer` | the PR | merge, or a rejection with a named reason |
| - | `integrator` | several already-green PRs | one branch, one validator run |

The `integrator` is not a stage - it runs only when several proven class PRs are
open at once, and it exists purely because the validator is one box. See
`roles/integrator.md`. It re-derives nothing; it moves proven commits onto a
fresh base and proves that the composition changed nothing.

Stage 2 is wider than its one row suggests. Besides the source and manifest, a
promotion edits `delinks.txt`, `converted-baseline.json` (via `tiers_ratchet
--update`, never by hand), `converted-backslide-exceptions.jsonl`, any `port/`
slice manifest naming a deleted shard, and any prose naming one. Six non-source
files is typical.

The writer is **gathering, not authoring**: 387 of 429 classes already have a
real header, and the shards being folded together are existing matched code. Most
promotions touch no header at all.

Stage 4 is the only stage that DECIDES whether the bytes are right, and it owns
`linkcheck` and `rombuild`. That is not the same as "stages 1-3 never run a byte
gate", which this file used to say and which is actively harmful at stage 3: a
humanizer's edits move bytes routinely, `tubuild.py verify` costs 2.6 seconds on
a 71-member TU, and running it after every batch is the difference between
knowing and bisecting. Stage 3 never changes semantics. Stage 5 is a different
agent from stage 4 — a producer's own green output is evidence, not review.

**Every merge to `main` invalidates the pinned base of every other open PR.** So
the cost of N separately-landed promotions is not N validator runs; it is N runs
plus N-1 rebases plus the ledger conflicts those rebases cause. Past two or
three open class PRs, the integrator is cheaper than the queue.

## The queue

**`notes/data/tu-promotion-queue.tsv` is the live queue.** Each row is a class
whose method bodies are still scattered across one-function shards, ranked
best-first by how many shards the promotion absorbs. It carries `shard_count`,
`promotion_route`, `blockers`, and a `sibling_oracle` naming an already-promoted
class in the same overlay to copy.

`notes/data/class-build-worklist.tsv` tracks a **different axis** — whether a
class has a real *header* — and 387 of 429 are already `DONE` there. It is the
right list for the 27-class header tail and the wrong one for this work:
`dBgActor_c` reads `DONE` in it while sitting at row 12 of the promotion queue
with 11 shards outstanding. `classqueue.py` prefers the promotion queue for
exactly this reason.

## Claiming work

A claim is a ref under `refs/claims/` pointing at a **parentless commit** unique
to the claiming agent. Two parentless commits are never ancestor-related, so a
second claimant's push is always a non-fast-forward and always rejected; the
claim is then read back from the remote to confirm our object actually landed.
No shared file is edited — a claims *file* is what produces merge cascades.

The parentless part is load-bearing. An earlier version pushed `HEAD`, which git
rejects only when the pusher is *behind* the holder. Agents branching off one
commit push the same SHA, git answers `Everything up-to-date` and exits 0, and
**every agent believes it won**. Do not simplify it back.

    python tools/classqueue.py next    --role writer
    python tools/classqueue.py claim   dActor_c --role writer --worktree C:/tmp/sm64ds-dactor

`claim` takes the **bare class name**. `ovNNN/dActor_c` is accepted and
normalized to the same ref — a class lives in one overlay, so the prefix adds
nothing. Before that normalization the two spellings built two different refs
and two agents could hold one class simultaneously; live claims existed in both
forms. Create the worktree first and claim once with `--worktree`: re-claiming
to attach the path afterwards is denied against yourself.

**Use forward slashes in `--worktree`, as the example above does — it is
required, not cosmetic.** A backslash path passed through a shell arrives with
its separators eaten and the claim records
`"worktree": "C:tmpsm64ds-memory2-build"`, which points at nothing and defeats
the whole reason the field exists.

    python tools/classqueue.py release dActor_c --role writer
    python tools/classqueue.py list

`claim` exits non-zero when the class is already held. That non-zero is the
lock. Do not retry it, do not force-push over it — take the next row.

Release your claim when your stage's output is committed and pushed, not when
you personally are done thinking.

**A released claim does not mean nobody has done the work.** The claim ref is
the only thing `classqueue.py` consults, so a class whose scout pushed a branch,
opened no PR and released its claim is offered again as unclaimed — and the next
agent rediscovers the whole thing. That happened to `dScMgCoin_c`. Before you
start, look:

    git branch -r --list 'origin/cpp/<Class>*'

If one exists, read it before you cut a new one.

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
6. **Force-pushes are blocked. Merge, do not rebuild — and do not rebase a
   branch that is already on the remote at all.** Every stage after the first
   inherits a pushed branch, so rebasing it makes your own push a
   non-fast-forward against the remote's copy of *the same commit*; repairing
   that costs an add/add conflict on the promoted `.cpp`, resolved correctly
   only by first proving the remote side byte-identical to what you started
   from. Merge `origin/main` in instead. And a branch rebuilt as a fast-forward
   from an older commit ships stale tree-derived work past review — which does
   not look stale, it looks confident.
